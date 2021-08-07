#include "secondPass.h"
#include "labelsDB.h"
#include "labelCallsDB.h"
#include "codeImageDB.h"

static boolean validateExternalUsage(labelCall currentCall, labelClass labelType);

static boolean updateCodeImage(void *codeImageDatabase, labelCall currentCall, long labelAddress, errorCodes *callErrorPtr);

static boolean fillMissingLabelAddresses(void **databasePointers);

boolean secondPass(void **databasePointers, long ICF) {
    boolean validPass;

    /* add ICF to data-type labels' addresses */
    updateDataLabels(databasePointers[LABELS_POINTER], ICF);

    /* handle labels as operators usages */
    validPass = fillMissingLabelAddresses(databasePointers);

    /* todo validate entry declarations */

    return validPass;
}

static boolean fillMissingLabelAddresses(void **databasePointers){
    int i;/* count how many label calls did we already handled */
    long labelAddress;/* store label definition address */
    labelClass labelType;/* store label type - code\data\external */
    boolean validCall;/* track if an error occurred during current call */
    boolean validPass = TRUE;/* track whether any errors occurred during second pass */
    errorCodes callError;/* if errors occur, track error code */
    labelCall currentCall;/* pointer to next label call in database */

    /* resolve each label call not handle in first pass */
    for(i = 0; getLabelCall(databasePointers[LABEL_CALLS_POINTER], i, &currentCall); i++){
        /* get label value */
        validCall = getLabelAttributes(databasePointers[LABELS_POINTER],
                                       currentCall.name, &labelAddress, &labelType);

        /* validate legality of label type with command type */
        if(validCall){/* found label */
            validCall = validateExternalUsage(currentCall, labelType);
        }
        else{/* label does not exist */
            /* todo print error */
        }

        /* update code image */
        if(validCall){/* label is of legal type for command type */
            validCall = updateCodeImage(databasePointers[CODE_IMAGE_POINTER], currentCall, labelAddress, &callError);
        }
        else{/* illegal label type */
            /* todo print error */
        }

        if(!validCall){/* mark error occurred */
            validPass = FALSE;
        }
    }

    return validPass;
}


static boolean validateExternalUsage(labelCall currentCall, labelClass labelType){
    boolean result = TRUE;

    /* I_branching commands cannot use external labels */
    if(labelType == EXTERN_LABEL){
        if(currentCall.type == I_BRANCHING){
            result = FALSE;
        }
        else{
            /* todo add entry to extern database */
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

