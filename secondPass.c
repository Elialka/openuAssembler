#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"

static errorCodes
validateExternalUsage(databasePtr externDatabase, labelCall *currentCallPtr, labelType definedLabelType);

static errorCodes
updateCodeImage(codeImageDBPtr codeImageDatabase, labelCall *currentCallPtr, long labelAddress);

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr);

static boolean locateEntryDefinitions(entryCallsDBPtr entryCallsDatabase, labelsDBPtr labelsDatabase);


boolean secondPass(databaseRouterPtr databasesPtr, long ICF){
    boolean result;

    /* update data-type labels' addresses, to appear after code image */
    updateDataLabels(databasesPtr->labelsDB, ICF + STARTING_ADDRESS);

    /* handle labels as operators usages */
    result = fillMissingLabelAddresses(databasesPtr);

    if(result){/* no errors */
        result = locateEntryDefinitions(databasesPtr->entryCallsDB, databasesPtr->labelsDB);
    }

    return result;
}

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr){
    boolean result = TRUE;
    errorCodes encounteredError = NO_ERROR;/* if errors occur, track encounteredError code */
    labelCallsDBPtr currentPtr = databasesPtr->labelCallsDB;
    labelCall *currentCallPtr;/* pointer to current label call data structure */
    definedLabel *definedLabelDataPtr;/* pointer to defined label with matching name's data structure */

    /* resolve each labels call not handle in first pass */
    for(; currentPtr; currentPtr = getNextLabelCall(currentPtr)){
        /* get label call data */
        currentCallPtr = getLabelCallData(currentPtr);

        encounteredError = getLabelAttributes(databasesPtr->labelsDB,
                                              currentCallPtr->labelId.name, &definedLabelDataPtr);

        /* validate legality of labels type with command type */
        if(!encounteredError){/* found labels attributes */
            encounteredError = validateExternalUsage(databasesPtr->externUsesDB, currentCallPtr,
                                                     definedLabelDataPtr->type);
        }

        /* update code image */
        if(!encounteredError){/* labels is of legal type for command type */
            encounteredError = updateCodeImage(databasesPtr->codeImageDB, currentCallPtr,
                                               definedLabelDataPtr->labelId.address);
        }

        if(encounteredError){/* mark encounteredError occurred */
            printErrorMessage(encounteredError, &currentCallPtr->lineId);
            result = FALSE;
        }
    }

    return result;
}


static errorCodes
validateExternalUsage(databasePtr externDatabase, labelCall *currentCallPtr, labelType definedLabelType) {
    errorCodes encounteredError = NO_ERROR;

    if(definedLabelType == EXTERN_LABEL){
        if(currentCallPtr->type == I_BRANCHING){/* illegal - I_branching commands cannot use external labels */
            encounteredError = CANNOT_BE_EXTERN;
        }
        else{/* add extern use to database *//* todo change two parameters to labelID */
            encounteredError = addExternUse(externDatabase, currentCallPtr->labelId.name, currentCallPtr->labelId.address);
        }
    }

    return encounteredError;
}


static errorCodes updateCodeImage(codeImageDBPtr codeImageDatabase, labelCall *currentCallPtr, long labelAddress){
    errorCodes encounteredError = NO_ERROR;

    if(currentCallPtr->type == I_BRANCHING){
        encounteredError = updateITypeImmed(codeImageDatabase, currentCallPtr->labelId.address, labelAddress);
    }
    else if(currentCallPtr->type == J_JMP || currentCallPtr->type == J_CALL_OR_LA){
        updateJTypeAddress(codeImageDatabase, currentCallPtr->labelId.address, labelAddress);
    }
    else{/* impossible scenario - only I_branching, call, la and jmp commands use labels */
        encounteredError = IMPOSSIBLE_UPDATE_CODE_IMAGE;
    }

    return encounteredError;
}


static boolean locateEntryDefinitions(entryCallsDBPtr entryCallsDatabase, labelsDBPtr labelsDatabase){
    boolean result = TRUE;/* reset error flag */
    errorCodes encounteredError = NO_ERROR;
    entryCallsDBPtr currentEntryCall = entryCallsDatabase;/* get first entry call */
    entryCall *entryCallData;/* pointer to entry call data structure */
    definedLabel *definedLabelDataPtr;/* pointer to defined label with matching name's data structure */

    /* validate each labels declared as entry is defined in current .as file, update addresses */
    for(; currentEntryCall; currentEntryCall = getNextEntryCall(currentEntryCall)){
        entryCallData = getEntryCallData(currentEntryCall);

        encounteredError = getLabelAttributes(labelsDatabase, entryCallData->labelId.name, &definedLabelDataPtr);

        if(!encounteredError){/* label exists */
            if(definedLabelDataPtr->type != EXTERN_LABEL){/* legal entry declaration */
                setEntryCallValue(currentEntryCall, definedLabelDataPtr->labelId.address);
            }
            else{/* cannot declare entry with external labels */
                encounteredError = ENTRY_IS_EXTERN;
            }
        }

        if(encounteredError){
            if(encounteredError == LABEL_NOT_FOUND){/* label is declared as entry but not defined */
                encounteredError = ENTRY_NOT_DEFINED;
            }
            printErrorMessage(encounteredError, &entryCallData->lineId);
            result = FALSE;
        }
    }

    return result;
}

