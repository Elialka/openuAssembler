#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"

static boolean
validateExternalUsage(externUsePtr externDatabase, labelCall currentCall, labelClass labelType, errorCodes *errorPtr);

static boolean
updateCodeImage(codeImagePtr codeImageDatabase, labelCall currentCall, long labelAddress, errorCodes *callErrorPtr);

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr);

static boolean locateEntryDefinitions(entryCallPtr entryCallsDatabase, labelPtr labelsDatabase, errorCodes *lineErrorPtr);

/* todo refactor - ugly file */
/* change inner functions return value to error codes */

/* todo test EVERYTHING */
boolean secondPass(databaseRouterPtr databasesPtr, long ICF) {
    boolean validPass;
    errorCodes lineError = NO_ERROR;

    /* update data-type labels' addresses, to appear after code image */
    updateDataLabels(databasesPtr->labelsDB, ICF + STARTING_ADDRESS);

    /* handle labels as operators usages */
    validPass = fillMissingLabelAddresses(databasesPtr);

    if(validPass){/* no errors */
        /*  */
        validPass = locateEntryDefinitions(databasesPtr->entryCallsDB,
                                           databasesPtr->labelsDB,
                                           &lineError);
    }

    return validPass;
}

static boolean fillMissingLabelAddresses(databaseRouterPtr databasesPtr){
    int i;/* count how many label calls did we already handle */
    long labelAddress;/* store label definition address */
    labelClass labelType;/* store label type - code\data\external */
    boolean validCall;/* track if an error occurred during current call */
    boolean validPass = TRUE;/* track whether any errors occurred during second pass */
    errorCodes error = NO_ERROR;/* if errors occur, track error code */
    labelCall currentCall;/* pointer to next label call in database */

    /* resolve each label call not handle in first pass */
    for(i = 0; getLabelCall(databasesPtr->labelCallsDB, i, &currentCall); i++){
        /* get label value */
        validCall = getLabelAttributes(databasesPtr->labelsDB,
                                       currentCall.name, &labelAddress, &labelType);

        /* validate legality of label type with command type */
        if(validCall){/* found label */
            validCall = validateExternalUsage(databasesPtr->externUsesDB, currentCall, labelType, &error);
        }

        /* update code image */
        if(validCall){/* label is of legal type for command type */
            validCall = updateCodeImage(databasesPtr->codeImageDB, currentCall, labelAddress, &error);
        }

        if(!validCall){/* mark error occurred */
            printErrorMessage(error, currentCall.line, currentCall.lineCounter);
            validPass = FALSE;
        }
    }

    return validPass;
}


static boolean validateExternalUsage(externUsePtr externDatabase, labelCall currentCall,
                                     labelClass labelType, errorCodes *errorPtr) {
    boolean result = TRUE;

    if(labelType == EXTERN_LABEL){
        if(currentCall.type == I_BRANCHING){/* illegal - I_branching commands cannot use external labels */
            result = FALSE;
        }
        else{/* add extern use to database */
            addExternUse(externDatabase, currentCall.name, currentCall.IC, errorPtr);
        }
    }

    return result;
}


static boolean updateCodeImage(codeImagePtr codeImageDatabase, labelCall currentCall, long labelAddress, errorCodes *callErrorPtr){
    boolean result;

    if(currentCall.type == I_BRANCHING){
        result = updateITypeImmed(codeImageDatabase, currentCall.IC, labelAddress, callErrorPtr);
    }
    else if(currentCall.type == J_JMP || currentCall.type == J_CALL_OR_LA){
        result = updateJTypeAddress(codeImageDatabase, currentCall.IC, labelAddress, callErrorPtr);
    }
    else{/* impossible scenario - only I_branching, call, la and jmp commands use labels */
        *callErrorPtr = IMPOSSIBLE;
        result = FALSE;
    }

    return result;
}


static boolean locateEntryDefinitions(entryCallPtr entryCallsDatabase, labelPtr labelsDatabase, errorCodes *lineErrorPtr) {
    boolean validOp = TRUE;/* reset error flag */
    entryCallPtr currentEntryCall = entryCallsDatabase;/* get first entry call */
    char *currentEntryName;/* point to string representing current entry call name */
    labelClass labelType;/* current label type */
    long labelAddress;/* address where label was defined */

    /* validate each label declared as entry is defined in current .as file */
    while(currentEntryCall){
        /* get entry name */
        if((currentEntryName = getEntryCallName(currentEntryCall))){
            /* get label type */
            validOp = getLabelAttributes(labelsDatabase, currentEntryName, &labelAddress, &labelType);
        }
        else{/* entry name doesn't exist*/
            printErrorMessage(ENTRY_NOT_DEFINED, getEntryCallLine(currentEntryCall),
                              getEntryCallLineCount(currentEntryCall));
        }

        if(validOp){/* label exists */
            if(labelType != EXTERN_LABEL){/* label is locally defined */
                setEntryCallValue(currentEntryCall, labelAddress);/* update definition location */
            }
            else{/* cannot declare entry with external label */
                *lineErrorPtr = ENTRY_IS_EXTERN;
                validOp = FALSE;
            }
        }
        else{/* label does not exist */
            *lineErrorPtr = ENTRY_NOT_DEFINED;/* redundant */
            validOp = FALSE;
        }

        currentEntryCall = getNextEntryCall(currentEntryCall);
    }

    return validOp;
}

