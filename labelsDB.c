#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "structuresDatabase.h"
#include "labelsDB.h"
#include "operationsDB.h"




/**
 * Check if both existing label and new label with matching names, are extern labels,
 * and return error code enum value accordingly
 * @param newLabelDataPtr pointer to new label data structure
 * @param existingLabelDataPtr pointer to new label data structure
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes legalDoubleDefinition(definedLabel *newLabelDataPtr, definedLabel *existingLabelDataPtr);


databasePtr initLabelsDB(){
    return initDatabase();
}


static boolean labelExists(databasePtr head, char *newLabelName, databasePtr *existingAddressPtr){
    databasePtr currentAddress = head;
    databasePtr lastAddress = currentAddress;
    definedLabel *currentData = NULL;
    boolean alreadyExists = FALSE;

    if(!isDBEmpty(head)){/* not first label definition */
        while(currentAddress){/* go through database, check if label is already defined */
            lastAddress = currentAddress;
            currentData = getEntryDataPtr(currentAddress);
            if(!strcmp(currentData->labelId.name, newLabelName)){/* label exists */
                alreadyExists = TRUE;
                currentAddress = NULL;/* terminate loop */
            }
            else{/* keep looking */
                currentAddress = getNextEntryAddress(currentAddress);
            }
        }
    }

    *existingAddressPtr = lastAddress;

    return alreadyExists;
}


static errorCodes legalDoubleDefinition(definedLabel *newLabelDataPtr, definedLabel *existingLabelDataPtr) {
    errorCodes encounteredError = NO_ERROR;

    if(newLabelDataPtr->type == EXTERN_LABEL){
        if(existingLabelDataPtr->type == EXTERN_LABEL){/* double extern declarations are legal */
            encounteredError = NO_ERROR;
        }
        else{
            encounteredError = LABEL_LOCAL_AND_EXTERN;
        }
    }
    else{
        if(existingLabelDataPtr->type == EXTERN_LABEL){
            encounteredError = LABEL_LOCAL_AND_EXTERN;
        }
        else{
            encounteredError = DOUBLE_LABEL_DEFINITION;
        }
    }

    return encounteredError;
}


errorCodes addNewLabel(databasePtr head, definedLabel *labelDataPtr) {
    errorCodes encounteredError = NO_ERROR;
    databasePtr lastAddress = NULL;
    definedLabel *newDataPtr = NULL;

    if(!labelExists(head, labelDataPtr->labelId.name, &lastAddress)){/* new label definition */
        newDataPtr = addNewDatabaseEntry(lastAddress, sizeof(definedLabel));
        if(newDataPtr){/* memory allocated for data */
            strcpy(newDataPtr->labelId.name, labelDataPtr->labelId.name);
            newDataPtr->labelId.address = labelDataPtr->labelId.address;
            newDataPtr->type = labelDataPtr->type;
        }
        else{
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }
    else{/* label already exists */
        encounteredError = legalDoubleDefinition(labelDataPtr, getEntryDataPtr(lastAddress));
    }

    return encounteredError;
}


errorCodes getLabelAttributes(databasePtr head, char *name, definedLabel **destinationPtr) {
    databasePtr matchingNameLabel = NULL;
    errorCodes encounteredError =LABEL_NOT_FOUND;

    if(labelExists(head, name, &matchingNameLabel)){/* there is a label with such a name */
        *destinationPtr = getEntryDataPtr(matchingNameLabel);
        encounteredError = NO_ERROR;
    }

    return encounteredError;
}


void updateDataLabels(databasePtr head, long offset){
    databasePtr currentLabelAddress = head;
    definedLabel *currentDataPtr = NULL;

    for(; currentLabelAddress; currentLabelAddress = getNextEntryAddress(currentLabelAddress)){
        currentDataPtr = getEntryDataPtr(currentLabelAddress);
        if(currentDataPtr && currentDataPtr->type == DATA_LABEL){
            currentDataPtr->labelId.address += offset;
        }
    }
}


void clearLabels(databasePtr head){
    clearDatabase(head);
}
