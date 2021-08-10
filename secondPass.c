#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"

static boolean
validateExternalUsage(void *externDatabase, labelCall currentCall, labelClass labelType, errorCodes *errorPtr);

static boolean
updateCodeImage(void *codeImageDatabase, labelCall currentCall, long labelAddress, errorCodes *callErrorPtr);

static boolean fillMissingLabelAddresses(void **databasePointers);

static boolean locateEntryDefinitions(void *entryCallsDatabase, void *labelsDatabase, errorCodes *lineErrorPtr);

/* todo test EVERYTHING */
boolean secondPass(void **databasePointers, long ICF) {
    boolean validPass;
    errorCodes lineError = NO_ERROR;

    /* update data-type labels' addresses, to appear after code image */
    updateDataLabels(databasePointers[LABELS_POINTER], ICF + STARTING_ADDRESS);

    /* handle labels as operators usages */
    validPass = fillMissingLabelAddresses(databasePointers);

    if(validPass){/* no errors */
        /*  */
        validPass = locateEntryDefinitions(databasePointers[ENTRY_CALLS_POINTER],
                                           databasePointers[LABELS_POINTER],
                                           &lineError);
    }

    return validPass;
}

static boolean fillMissingLabelAddresses(void **databasePointers){
    int i;/* count how many label calls did we already handled */
    long labelAddress;/* store label definition address */
    labelClass labelType;/* store label type - code\data\external */
    boolean validCall;/* track if an error occurred during current call */
    boolean validPass = TRUE;/* track whether any errors occurred during second pass */
    errorCodes error = NO_ERROR;/* if errors occur, track error code */
    labelCall currentCall;/* pointer to next label call in database */

    /* resolve each label call not handle in first pass */
    for(i = 0; getLabelCall(databasePointers[LABEL_CALLS_POINTER], i, &currentCall); i++){
        /* get label value */
        validCall = getLabelAttributes(databasePointers[LABELS_POINTER],
                                       currentCall.name, &labelAddress, &labelType);

        /* validate legality of label type with command type */
        if(validCall){/* found label */
            validCall = validateExternalUsage(databasePointers[EXTERN_POINTER], currentCall, labelType, &error);
        }

        /* update code image */
        if(validCall){/* label is of legal type for command type */
            validCall = updateCodeImage(databasePointers[CODE_IMAGE_POINTER], currentCall, labelAddress, &error);
        }

        if(!validCall){/* mark error occurred */
            /* todo print error */
            validPass = FALSE;
        }
    }

    return validPass;
}


static boolean validateExternalUsage(void *externDatabase, labelCall currentCall,
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


static boolean updateCodeImage(void *codeImageDatabase, labelCall currentCall, long labelAddress, errorCodes *callErrorPtr){
    boolean result;

    if(currentCall.type == I_BRANCHING){
        result = updateITypeImmed(codeImageDatabase, currentCall.IC, labelAddress, callErrorPtr);
    }
    else if(currentCall.type == J_JUMP){
        result = updateJTypeAddress(codeImageDatabase, currentCall.IC, labelAddress, callErrorPtr);
    }
    else{/* impossible scenario - only I_branching and jump commands use labels */
        *callErrorPtr = IMPOSSIBLE;
        result = FALSE;
    }

    return result;
}


static boolean locateEntryDefinitions(void *entryCallsDatabase, void *labelsDatabase, errorCodes *lineErrorPtr) {
    boolean validOp = TRUE;/* reset error flag */
    void *currentEntryCall = getNextEntryCall(entryCallsDatabase);/* get first entry call */
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
            /* todo handle */
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
            *lineErrorPtr = ENTRY_NOT_DEFINED;
            validOp = FALSE;
        }

        currentEntryCall = getNextEntryCall(currentEntryCall);
    }

    return validOp;
}

