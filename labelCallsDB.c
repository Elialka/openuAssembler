#include <stdlib.h>
#include <string.h>

#include "labelCallsDB.h"


typedef struct call{
    labelCall attributes;
    labelCallPtr next;
}labelCallNode;


static boolean isLabelCallsEmpty(labelCallPtr head);


labelCallPtr initLabelCallsDB(){
    labelCallPtr head = calloc(1, sizeof(labelCallNode));

    return head;
}


static boolean isLabelCallsEmpty(labelCallPtr head){
    /* check if any attributes has been set, IC is starting at non-zero value - will return FALSE only if not set */
    return head && head->attributes.IC;
}


errorCodes setLabelCall(labelCallPtr head, long IC, char *labelName, operationClass commandOpType) {
    errorCodes encounteredError = NO_ERROR;
    labelCallPtr current;
    labelCallPtr prev;

    current = head;

    if(!isLabelCallsEmpty(head)){/* not first label call */
        /* find last label call record */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new label - memory for first label is allocated when database was initialized */
        current = calloc(1, sizeof(labelCallNode));
        if(!current){
             encounteredError = MEMORY_ALLOCATION_FAILURE;
            /* todo handle error - free all allocated memory, quit program */
        }

        /* link new node to database */
        prev->next = current;
    }

    /* impossible value */
    if(commandOpType != I_BRANCHING && commandOpType != J_JUMP && commandOpType != J_CALL_OR_LA){
        encounteredError = IMPOSSIBLE;
    }

    if(!encounteredError){
        /* update fields */
        current->attributes.IC = IC;
        strcpy(current->attributes.name, labelName);
        current->attributes.type = commandOpType;
    }

    return encounteredError;
}


boolean getLabelCall(labelCallPtr head, int index, labelCall *destination){
    int i;
    labelCallPtr current = head;

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


void clearLabelCallsDB(labelCallPtr head){
    labelCallPtr current = head;
    labelCallPtr prev;

    while(current){
        prev = current;
        current = current->next;
        free(prev);
    }
}
