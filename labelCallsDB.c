#include <stdlib.h>
#include <string.h>

#include "labelCallsDB.h"


typedef struct call *labelCallNodePtr;

typedef struct call{
    labelCall attributes;
    labelCallNodePtr next;
}labelCallNode;


void * initLabelCallsDB(){
    void *head;

    head = calloc(1, sizeof(labelCallNode));

    if(!head){
        /* todo print error */
    }

    return head;

}


boolean setLabelCall(void *head, long IC, char *labelName, operationClass commandOpType, errorCodes *lineErrorPtr) {
    static int labelCallsCounter = 0;/* counts how many label calls added to the database */
    labelCallNodePtr current;
    labelCallNodePtr prev;

    current = head;

    if(labelCallsCounter){/* not first label call */
        /* find last label call record */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new label - memory for first label is allocated when database was initialized */
        current = calloc(1, sizeof(labelCallNode));
        if(!current){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }

        /* link new node to database */
        prev->next = current;
    }

    /* impossible value */
    if(commandOpType != I_BRANCHING && commandOpType != J_JUMP){
        *lineErrorPtr = IMPOSSIBLE;
        return FALSE;
    }

    /* count another label call */
    labelCallsCounter++;

    /* update fields */
    current->attributes.IC = IC;
    strcpy(current->attributes.name, labelName);
    current->attributes.type = commandOpType;

    return TRUE;
}


boolean getLabelCall(void *head, int index, labelCall *destination){
    int i;
    labelCallNodePtr current = head;

    for(i = 0; current && i < index; current = current->next, i++)
        ;

    if(current){/* label call in given index exists */
        *destination = current->attributes;
        return TRUE;
    }
    else{
        return FALSE;
    }
}


void clearLabelCallsDB(void *head){
    labelCallNodePtr current = head;
    labelCallNodePtr prev;

    while(current){
        prev = current;
        current = current->next;
        free(prev);
    }
}
