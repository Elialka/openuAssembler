#include <stddef.h>
#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "printErrors.h"

static errorCodes
validateExternalUsage(databasePtr externDatabase, labelCall *currentCallPtr, labelType definedLabelType);

static errorCodes
updateCodeImage(codeImageDBPtr codeImageDatabase, labelCall *currentCallPtr, long labelAddress);

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr, fileErrorStatus *fileStatusPtr);

static errorCodes fillMissingAddress(databaseRouterPtr databasesPtr, labelCall *currentCallDataPtr);

static boolean
locateEntryDefinitions(databasePtr entryCallsDatabase, databasePtr labelsDatabase, fileErrorStatus *fileStatusPtr);


boolean secondPass(databaseRouterPtr databasesPtr, long ICF, fileErrorStatus *fileStatusPtr) {
    boolean result;

    /* update data-type labels' addresses, to appear after code image */
    updateDataLabels(databasesPtr->labelsDB, ICF + STARTING_ADDRESS);

    /* handle labels as operators usages */
    result = fillMissingLabelAddresses(databasesPtr, fileStatusPtr);

    if(result){/* no errors */
        if(!isEntryCallsDBEmpty(databasesPtr->entryCallsDB)){
            result = locateEntryDefinitions(databasesPtr->entryCallsDB, databasesPtr->labelsDB, fileStatusPtr);
        }
    }

    return result;
}

/* todo split function */
static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr, fileErrorStatus *fileStatusPtr) {
    boolean result = TRUE;
    errorCodes encounteredError = NO_ERROR;/* if errors occur, track encounteredError code */
    databasePtr labelCallPtr = databasesPtr->labelCallsDB;
    labelCall *currentCallDataPtr;/* pointer to current label call data structure */

    /* resolve each labels call not handle in first pass */
    for(; labelCallPtr; labelCallPtr = getNextLabelCall(labelCallPtr)){
        /* get label call data */
        currentCallDataPtr = getLabelCallData(labelCallPtr);

        if(currentCallDataPtr){/* data exists */
            encounteredError = fillMissingAddress(databasesPtr, currentCallDataPtr);

            if(encounteredError){
                printFileErrorMessage(encounteredError, &currentCallDataPtr->lineId, fileStatusPtr);
                result = FALSE;
            }
        }
    }

    return result;
}


static errorCodes fillMissingAddress(databaseRouterPtr databasesPtr, labelCall *currentCallDataPtr) {
    errorCodes encounteredError = NO_ERROR;
    definedLabel *definedLabelDataPtr;/* pointer to defined label with matching name's data structure */

    encounteredError = getLabelAttributes(databasesPtr->labelsDB, currentCallDataPtr->labelId.name, &definedLabelDataPtr);

    /* validate legality of label type with command type */
    if(!encounteredError){/* found labels attributes */
        encounteredError = validateExternalUsage(databasesPtr->externUsesDB, currentCallDataPtr,
                                                 definedLabelDataPtr->type);
    }

    /* update code image */
    if(!encounteredError){/* labels is of legal type for command type */
        encounteredError = updateCodeImage(databasesPtr->codeImageDB, currentCallDataPtr,
                                           definedLabelDataPtr->labelId.address);
    }




    return encounteredError;
}


static errorCodes
validateExternalUsage(databasePtr externDatabase, labelCall *currentCallPtr, labelType definedLabelType) {
    errorCodes encounteredError = NO_ERROR;

    if(definedLabelType == EXTERN_LABEL){
        if(currentCallPtr->type == I_BRANCHING){/* illegal - I_branching commands cannot use external labels */
            encounteredError = CANNOT_BE_EXTERN;
        }
        else{/* add extern use to database */
            encounteredError = addExternUse(externDatabase, currentCallPtr->labelId);
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
        encounteredError = IMPOSSIBLE;
    }

    return encounteredError;
}


static boolean
locateEntryDefinitions(databasePtr entryCallsDatabase, databasePtr labelsDatabase, fileErrorStatus *fileStatusPtr) {
    boolean result = TRUE;/* reset error flag */
    errorCodes encounteredError = NO_ERROR;
    databasePtr currentEntryCall = entryCallsDatabase;/* get first entry call */
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
            printFileErrorMessage(encounteredError, &entryCallData->lineId, fileStatusPtr);
            result = FALSE;
        }
    }

    return result;
}

