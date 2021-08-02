#include <stdlib.h>
#include <string.h>

#include "labelsDB.h"
#include "operationsDB.h"


typedef struct node *labelPtr;

/* todo write function */

typedef struct node{
    char name[LABEL_ARRAY_SIZE];
    long value;
    labelPtr next;
    labelType type;
}label;

/* todo remove global variable */
static labelPtr head;

/*
 * return if label with matching name already exists in database
 */
boolean seekLabel(char *name){
    labelPtr curr;

    curr = head;

    while(curr){
        if(!strcmp(name, curr->name)){/* found */
            return TRUE;
        }
    }
    return FALSE;
}

/*
 * return true if legal name - not a saved keyword
 */
/* todo check if necessary */
boolean legalLabelDeclaration(char *currentLabel, errorCodes *lineErrorPtr){
    boolean result;

    if(seekOp(currentLabel)){/* label name is operation name */
        *lineErrorPtr = LABEL_IS_OPERATION;
        result = FALSE;
    }
    else if(seekLabel(currentLabel)){/* label already exists */
        *lineErrorPtr = DOUBLE_LABEL_DEFINITION;
        result = FALSE;
    }
    else{
        result = TRUE;
    }

    return result;
}


boolean addNewLabel(char *name, int value, labelType type, errorCodes *lineErrorPtr) {
    labelPtr curr, prev;
    char *c;

    if(!head) {/* first label */
        head = calloc(1, sizeof(label));
        if(!head){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }
        curr = head;
    }
    else{
        curr = head;
        /* lookup name in DB, until reached tail */
        while(curr){
            if(!strcmp(name, curr->name)){/* label already defined */
                *lineErrorPtr = DOUBLE_LABEL_DEFINITION;/* todo remove - redundant */
                return FALSE;
            }
            else{
                prev = curr;
                curr = curr->next;
            }
        }

        /* end of DB */
        curr = calloc(1, sizeof(label));
        if(!curr){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }
        prev->next = curr;
    }

    /* set label */
    strcpy(curr->name, name);
    curr->value = value;
    curr->type = type;


    return TRUE;
}

/* refactor to use boolean return value */
long int getLabelValue(char *name, errorCodes *lineErrorPtr){
    labelPtr curr;
    int value;

    curr = head;
    while(curr){
        if(!strcmp(name, curr->name)){/* found match */
            value = curr->value;
            break;
        }
        else{
            curr = curr->next;
        }
    }
    if(!curr) {/* label name not found */
        value = FALSE;
        *lineErrorPtr = LABEL_NOT_FOUND;
    }

    return value;
}


void clearLabels(){
    labelPtr temp;
    while(head){
        temp = head;
        head = head->next;
        free(temp);
    }
}
