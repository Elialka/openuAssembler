#include <stdlib.h>
#include <string.h>

#include "labelsDB.h"
#include "operationsDB.h"


typedef struct node *labelPtr;

/* todo write function */

typedef struct node{
    char name[LABEL_ARRAY_SIZE];
    int value;
    labelType type;
    labelPtr next;
}label;


void *initLabelsDB(){
    void *head;

    head = calloc(1, sizeof(label));

    if(!head){
        /* todo print error quit*/
    }

    return head;
}



/*
 * return if label with matching name already exists in database
 */
boolean seekLabel(void *head, char *name) {
    labelPtr curr;

    curr = head;

    while(curr){
        if(!strcmp(name, curr->name)){/* found */
            return TRUE;
        }
    }
    return FALSE;
}


boolean addNewLabel(void *head, char *labelName, int value, labelType type, errorCodes *lineErrorPtr){
    static int labelsCounter = 0;/* how many labels currently in database */
    labelPtr current;
    labelPtr prev;

    current = head;

    if(labelsCounter){/* not first label */
        /* find last defined label, in the process look for new name in defined labels */
        while(current){
            if(!strcmp(labelName, current->name)){/* label already defined */
                if(current->type == EXTERN_LABEL || type == EXTERN_LABEL){/* if the old or the new label is external */
                    if(current->type == EXTERN_LABEL && type == EXTERN_LABEL){/* both labels are external */
                        return TRUE;/* multiple extern declarations are allowed */
                    }
                    else{/* only one of them is external */
                        *lineErrorPtr = LABEL_LOCAL_AND_EXTERN;
                    }
                }
                else{/* two local label definitions */
                    *lineErrorPtr = DOUBLE_LABEL_DEFINITION;
                }
                return FALSE;
            }
            else{/* new label name is not used so far */
                prev = current;
                current = current->next;
            }
        }

        /* allocate memory for new label - memory for first label is allocated when database was initialized */
        current = calloc(1, sizeof(label));
        if(!current){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }

        /* link new node to database */
        prev->next = current;
    }

    /* count label addition */
    labelsCounter++;

    /* update fields */
    strcpy(current->name, labelName);
    current->value = value;
    current->type = type;

    return TRUE;
}


void clearLabels(void *head) {
    labelPtr temp;
    while(head){
        temp = head;
        head = temp->next;
        free(temp);
    }
}
