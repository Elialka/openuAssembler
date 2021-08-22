#include <stdlib.h>
#include <string.h>

#include "labelsDB.h"
#include "operationsDB.h"


/* todo write function */

typedef struct labelsDB{
    definedLabel data;
    labelsDBPtr next;
}labelCallNode;


static boolean isLabelsDBEmpty(labelsDBPtr head);


labelsDBPtr initLabelsDB(){
    labelsDBPtr head = calloc(1, sizeof(labelCallNode));

    return head;
}


static boolean isLabelsDBEmpty(labelsDBPtr head){
    /* check if any labels has been set - cannot have empty name */
    return head && *head->data.labelId.name;
}


/*
 * return if labels with matching name already exists in database
 */
boolean seekLabel(labelsDBPtr head, char *name){
    labelsDBPtr curr;

    curr = head;

    while(curr){
        if(!strcmp(name, curr->data.labelId.name)){/* found */
            return TRUE;
        }
    }
    return FALSE;
}

/* todo split function to 3 - can remodel seekLabel */
errorCodes addNewLabel(labelsDBPtr head, definedLabel *labelDataPtr) {
    errorCodes encounteredError = NO_ERROR;
    labelsDBPtr current;
    labelsDBPtr prev;

    current = head;

    if(!isLabelsDBEmpty(head)){/* not first labelsDB */
        /* find last defined labelsDB, in the process look for new name in defined labels */
        while(current){
            if(!strcmp(labelDataPtr->labelId.name, current->data.labelId.name)){/* labelsDB already defined */
                if(current->data.type == EXTERN_LABEL || labelDataPtr->type == EXTERN_LABEL){/* if the old or the new labelsDB is external */
                    if(current->data.type == EXTERN_LABEL && labelDataPtr->type == EXTERN_LABEL){/* both labels are external */
                        /* multiple extern declarations are allowed */
                    }
                    else{/* only one of them is external */
                        encounteredError = LABEL_LOCAL_AND_EXTERN;
                    }
                }
                else{/* two local labelsDB definitions */
                    encounteredError = DOUBLE_LABEL_DEFINITION;
                }
            }
            else{/* new labelsDB name is not used so far */
                prev = current;
                current = current->next;
            }
        }

        if(!encounteredError){
            /* allocate memory for new labelsDB - memory for first labelsDB is allocated when database was initialized */
            current = calloc(1, sizeof(labelCallNode));
            if(!current){
                encounteredError = MEMORY_ALLOCATION_FAILURE;
            }
        }

        /* link new labelCallNode to database */
        prev->next = current;
    }

    if(!encounteredError){
        /* update fields */
        strcpy(current->data.labelId.name, labelDataPtr->labelId.name);
        current->data.labelId.address = labelDataPtr->labelId.address;
        current->data.type = labelDataPtr->type;
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
