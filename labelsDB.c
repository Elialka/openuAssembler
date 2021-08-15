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
    labelPtr head;

    head = calloc(1, sizeof(label));

    if(!head){
        /* todo print error quit*/
    }

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
boolean addNewLabel(labelPtr head, char *labelName, long address, labelClass type, errorCodes *lineErrorPtr){
    boolean result = TRUE;
    labelPtr current;
    labelPtr prev;

    current = head;

    if(!isLabelsDBEmpty(head)){/* not first label */
        /* find last defined label, in the process look for new name in defined labels */
        while(current){
            if(!strcmp(labelName, current->name)){/* label already defined */
                if(current->type == EXTERN_LABEL || type == EXTERN_LABEL){/* if the old or the new label is external */
                    if(current->type == EXTERN_LABEL && type == EXTERN_LABEL){/* both labels are external */
                        result =  TRUE;/* multiple extern declarations are allowed */
                    }
                    else{/* only one of them is external */
                        *lineErrorPtr = LABEL_LOCAL_AND_EXTERN;
                        result =  FALSE;
                    }
                }
                else{/* two local label definitions */
                    *lineErrorPtr = DOUBLE_LABEL_DEFINITION;
                    result =  FALSE;
                }
            }
            else{/* new label name is not used so far */
                prev = current;
                current = current->next;
            }
        }

        if(result){
            /* allocate memory for new label - memory for first label is allocated when database was initialized */
            current = calloc(1, sizeof(label));
            if(!current){
                *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
                result =  FALSE;
            }
        }

        /* link new node to database */
        prev->next = current;
    }

    if(result){
        /* update fields */
        strcpy(current->name, labelName);
        current->address = address;
        current->type = type;
    }

    return result;
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


void updateDataLabels(labelPtr head, long ICF){
    labelPtr currentLabelPtr = head;

    while(currentLabelPtr){/* there is another label in the database */
        if(currentLabelPtr->type == DATA_LABEL){
            currentLabelPtr->address += ICF;
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
