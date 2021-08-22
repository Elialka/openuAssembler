#include <stdlib.h>
#include <string.h>

#include "labelsDB.h"
#include "operationsDB.h"


/* todo write function */

typedef struct labelsDB{
    definedLabel data;
    labelsDBPtr next;
}labelCallNode;

/**
 *
 * @param head
 * @return
 */
static boolean isLabelsDBEmpty(labelsDBPtr head);

/**
 * Find address to store new entry call node, allocate memory or update error code enum if necessary
 * If identical name already exists in the database, return NULL pointer + update error pointer
 * @param head
 * @param labelName
 * @param identicalPtr
 * @param errorPtr
 * @return
 */
static labelsDBPtr newLabelNode(labelsDBPtr head, char *labelName, labelsDBPtr *identicalPtr, errorCodes *errorPtr);

/**
 * Check if both existing label and new label with matching names, are extern labels,
 * and return error code enum value accordingly
 * @param newLabelDataPtr pointer to new label data structure
 * @param existingLabelDataPtr pointer to new label data structure
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes legalDoubleDefinition(definedLabel *newLabelDataPtr, definedLabel *existingLabelDataPtr);


labelsDBPtr initLabelsDB(){
    labelsDBPtr head = calloc(1, sizeof(labelCallNode));

    return head;
}


static boolean isLabelsDBEmpty(labelsDBPtr head){
    /* check if any labels has been set - cannot have empty name */
    return head && *head->data.labelId.name;
}


static labelsDBPtr newLabelNode(labelsDBPtr head, char *labelName, labelsDBPtr *identicalPtr, errorCodes *errorPtr) {
    labelsDBPtr current = head;
    labelsDBPtr prev = current;
    boolean alreadyExists = FALSE;

    if(!isLabelsDBEmpty(head)){/* not first label call in database - need to allocate memory */
        /* find last label node */
        while(current){
            if(!strcmp(current->data.labelId.name, labelName)){/* label already defined */
                alreadyExists = TRUE;
                current = NULL;
                *identicalPtr = current;
            }
            else{
                prev = current;
                current = current->next;
            }
        }

        if(alreadyExists){
            *errorPtr = DOUBLE_LABEL_DEFINITION;
        }
        else{/* new label */
            /* allocate memory for new label node */
            current = calloc(1, sizeof(labelCallNode));
            if(!current){
                *errorPtr = MEMORY_ALLOCATION_FAILURE;
            }
            else{/* allocated successfully */
                /* link new node to database */
                prev->next = current;
            }
        }
    }

    return current;
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


errorCodes addNewLabel(labelsDBPtr head, definedLabel *labelDataPtr) {
    errorCodes encounteredError = NO_ERROR;
    labelsDBPtr current = NULL;
    labelsDBPtr identical = NULL;

    current = newLabelNode(head, labelDataPtr->labelId.name, &identical, &encounteredError);
    if(current){/* new node allocated */
        /* update fields */
        strcpy(current->data.labelId.name, labelDataPtr->labelId.name);
        current->data.labelId.address = labelDataPtr->labelId.address;
        current->data.type = labelDataPtr->type;
    }
    else{/* label already exists or memory allocation failure */
        if(encounteredError == DOUBLE_LABEL_DEFINITION){
            encounteredError = legalDoubleDefinition(labelDataPtr, &identical->data);
        }
    }

    return encounteredError;
}


errorCodes getLabelAttributes(labelsDBPtr head, char *name, definedLabel **destinationPtr) {
    labelsDBPtr currentNode = head;
    errorCodes encounteredError =LABEL_NOT_FOUND;

    while(currentNode){
        if(!strcmp(currentNode->data.labelId.name, name)){/* found match */
            *destinationPtr = &currentNode->data;
            currentNode = NULL;/* terminate loop */
            encounteredError = NO_ERROR;
        }
        else{/* keep searching */
            currentNode = currentNode->next;
        }
    }

    return encounteredError;
}


void updateDataLabels(labelsDBPtr head, long offset){
    labelsDBPtr currentLabelPtr = head;

    while(currentLabelPtr){/* there is another labelsDB in the database */
        if(currentLabelPtr->data.type == DATA_LABEL){
            currentLabelPtr->data.labelId.address += offset;
        }

        /* get next labelsDB */
        currentLabelPtr = currentLabelPtr->next;
    }
}


void clearLabels(labelsDBPtr head){
    labelsDBPtr temp;

    while(head){
        temp = head;
        head = temp->next;
        free(temp);
    }
}
