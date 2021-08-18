#include <stdlib.h>
#include <string.h>

#include "labelsDB.h"
#include "operationsDB.h"


/* todo write function */

typedef struct label{
    char name[LABEL_ARRAY_SIZE];
    long address;
    labelClass type;
    labelPtr next;
}label;


static boolean isLabelsDBEmpty(labelPtr head);


labelPtr initLabelsDB(){
    labelPtr head = calloc(1, sizeof(label));

    return head;
}


static boolean isLabelsDBEmpty(labelPtr head){
    /* check if any labels has been set - cannot have empty name */
    return head && *head->name;
}


/*
 * todo may be redundant
 * return if label with matching name already exists in database
 */
boolean seekLabel(labelPtr head, char *name) {
    labelPtr curr;

    curr = head;

    while(curr){
        if(!strcmp(name, curr->name)){/* found */
            return TRUE;
        }
    }
    return FALSE;
}

/* todo split function - can remodel seekLabel */
errorCodes addNewLabel(labelPtr head, char *labelName, long address, labelClass type) {
    errorCodes encounteredError = NO_ERROR;
    labelPtr current;
    labelPtr prev;

    current = head;

    if(!isLabelsDBEmpty(head)){/* not first label */
        /* find last defined label, in the process look for new name in defined labels */
        while(current){
            if(!strcmp(labelName, current->name)){/* label already defined */
                if(current->type == EXTERN_LABEL || type == EXTERN_LABEL){/* if the old or the new label is external */
                    if(current->type == EXTERN_LABEL && type == EXTERN_LABEL){/* both labels are external */
                        /* multiple extern declarations are allowed */
                    }
                    else{/* only one of them is external */
                        encounteredError = LABEL_LOCAL_AND_EXTERN;
                    }
                }
                else{/* two local label definitions */
                    encounteredError = DOUBLE_LABEL_DEFINITION;
                }
            }
            else{/* new label name is not used so far */
                prev = current;
                current = current->next;
            }
        }

        if(!encounteredError){
            /* allocate memory for new label - memory for first label is allocated when database was initialized */
            current = calloc(1, sizeof(label));
            if(!current){
                encounteredError = MEMORY_ALLOCATION_FAILURE;
            }
        }

        /* link new node to database */
        prev->next = current;
    }

    if(!encounteredError){
        /* update fields */
        strcpy(current->name, labelName);
        current->address = address;
        current->type = type;
    }

    return encounteredError;
}


boolean getLabelAttributes(labelPtr head, char *name, long *addressPtr, labelClass *typePtr) {
    labelPtr current = head;
    boolean result = FALSE;

    while(current){
        if(!strcmp(current->name, name)){/* found match */
            *addressPtr = current->address;
            *typePtr = current->type;
            current = NULL;
            result = TRUE;
        }
        else{/* keep searching */
            current = current->next;
        }
    }
    return result;
}


void updateDataLabels(labelPtr head, long offset){
    labelPtr currentLabelPtr = head;

    while(currentLabelPtr){/* there is another label in the database */
        if(currentLabelPtr->type == DATA_LABEL){
            currentLabelPtr->address += offset;
        }

        /* get next label */
        currentLabelPtr = currentLabelPtr->next;
    }
}


void clearLabels(labelPtr head) {
    labelPtr temp;

    while(head){
        temp = head;
        head = temp->next;
        free(temp);
    }
}
