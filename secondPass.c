#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"

static errorCodes
validateExternalUsage(externUsesDBPtr externDatabase, labelCall currentCall, labelType labelType);

static errorCodes
updateCodeImage(codeImageDBPtr codeImageDatabase, labelCall currentCall, long labelAddress);

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr);

static boolean locateEntryDefinitions(entryCallsDBPtr entryCallsDatabase, labelsDBPtr labelsDatabase);

/* todo refactor - ugly file */
/* change inner functions return value to error codes */

/* todo test EVERYTHING */
boolean secondPass(databaseRouterPtr databasesPtr, long ICF) {
    boolean result;

    /* update data-type labels' addresses, to appear after code image */
    updateDataLabels(databasesPtr->labelsDB, ICF + STARTING_ADDRESS);

    /* handle labels as operators usages */
    result = fillMissingLabelAddresses(databasesPtr);

    if(result){/* no errors */
        /*  */
        result = locateEntryDefinitions(databasesPtr->entryCallsDB, databasesPtr->labelsDB);
    }

    return result;
}

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr){
    int i;/* count how many labelsDB calls did we already handle */
    long labelAddress;/* store labelsDB definition address */
    labelType labelType;/* store labelsDB type - code\data\external */
    boolean result = TRUE;/* track whether any errors occurred during second pass */
    errorCodes encounteredError = NO_ERROR;/* if errors occur, track encounteredError code */
    labelCall currentCall;/* pointer to next labelsDB call in database */

    /* resolve each labelsDB call not handle in first pass */
    for(i = 0; getLabelCall(databasesPtr->labelCallsDB, i, &currentCall); i++){
        /* get labelsDB value */
        encounteredError = getLabelAttributes(databasesPtr->labelsDB,
                                              currentCall.labelId.name, &labelAddress, &labelType);

        /* validate legality of labelsDB type with command type */
        if(encounteredError){/* found labelsDB */
            encounteredError = validateExternalUsage(databasesPtr->externUsesDB, currentCall, labelType);
        }

        /* update code image */
        if(encounteredError){/* labelsDB is of legal type for command type */
            encounteredError = updateCodeImage(databasesPtr->codeImageDB, currentCall, labelAddress);
        }

        if(encounteredError){/* mark encounteredError occurred */
            printErrorMessage(encounteredError, currentCall.lineId.line, currentCall.lineId.count);
            result = FALSE;
        }
    }

    return result;
}


static errorCodes validateExternalUsage(externUsesDBPtr externDatabase, labelCall currentCall, labelType labelType) {
    errorCodes encounteredError = NO_ERROR;

    if(labelType == EXTERN_LABEL){
        if(currentCall.type == I_BRANCHING){/* illegal - I_branching commands cannot use external labels */
            encounteredError = CANNOT_BE_EXTERN;
        }
        else{/* add extern use to database */
            encounteredError = addExternUse(externDatabase, currentCall.labelId.name, currentCall.labelId.address);
        }
    }

    return encounteredError;
}


static errorCodes updateCodeImage(codeImageDBPtr codeImageDatabase, labelCall currentCall, long labelAddress) {
    errorCodes encounteredError = NO_ERROR;

    if(currentCall.type == I_BRANCHING){
        encounteredError = updateITypeImmed(codeImageDatabase, currentCall.labelId.address, labelAddress);
    }
    else if(currentCall.type == J_JMP || currentCall.type == J_CALL_OR_LA){
        updateJTypeAddress(codeImageDatabase, currentCall.labelId.address, labelAddress);
    }
    else{/* impossible scenario - only I_branching, call, la and jmp commands use labels */
        encounteredError = IMPOSSIBLE_UPDATE_CODE_IMAGE;
    }

    return encounteredError;
}


static boolean locateEntryDefinitions(entryCallsDBPtr entryCallsDatabase, labelsDBPtr labelsDatabase) {
    boolean validOp = TRUE;/* reset error flag */
    errorCodes encounteredError = NO_ERROR;
    entryCallsDBPtr currentEntryCall = entryCallsDatabase;/* get first entry call */
    char *currentEntryName;/* point to string representing current entry call name */
    labelType labelType;/* current labelsDB type */
    long labelAddress;/* address where labelsDB was defined */

    /* validate each labelsDB declared as entry is defined in current .as file */
    while(currentEntryCall){
        /* get entry name */
        if((currentEntryName = getEntryCallName(currentEntryCall))){
            /* get labelsDB type */
            encounteredError = getLabelAttributes(labelsDatabase, currentEntryName, &labelAddress, &labelType);
        }
        else{/* entry name doesn't exist*/
            printErrorMessage(ENTRY_NOT_DEFINED, getEntryCallLine(currentEntryCall),
                              getEntryCallLineCount(currentEntryCall));
        }

        if(validOp){/* labelsDB exists */
            if(labelType != EXTERN_LABEL){/* labelsDB is locally defined */
                setEntryCallValue(currentEntryCall, labelAddress);/* update definition location */
            }
            else{/* cannot declare entry with external labelsDB */
                encounteredError = ENTRY_IS_EXTERN;
                validOp = FALSE;
            }
        }
        else{/* labelsDB does not exist */
            encounteredError = ENTRY_NOT_DEFINED;/* redundant */
            validOp = FALSE;
        }

        if(encounteredError){
            printErrorMessage(encounteredError, getEntryCallLine(currentEntryCall),
                              getEntryCallLineCount(currentEntryCall));
        }

        currentEntryCall = getNextEntryCall(currentEntryCall);
    }

    return validOp;
}

