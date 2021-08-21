#include <stdlib.h>
#include <string.h>

#include "labelCallsDB.h"


typedef struct labelCallsDB{
    labelCall data;
    labelCallsDBPtr next;
}labelCallNode;


static boolean isLabelCallsEmpty(labelCallsDBPtr head);


labelCallsDBPtr initLabelCallsDB(){
    labelCallsDBPtr head = calloc(1, sizeof(labelCallNode));

    return head;
}


static boolean isLabelCallsEmpty(labelCallsDBPtr head){
    /* check if name has been set - empty name is impossible */
    return head && !*head->data.labelId.name;
}


errorCodes
setLabelCall(labelCallsDBPtr head, long IC, char *labelName, operationClass commandOpType, char *line, long lineCounter){
    errorCodes encounteredError = NO_ERROR;
    labelCallsDBPtr current;
    labelCallsDBPtr prev;

    current = head;

    if(!isLabelCallsEmpty(head)){/* not first labelsDB call */
        /* find last labelsDB call record */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new labelsDB - memory for first labelsDB is allocated when database was initialized */
        current = calloc(1, sizeof(labelCallNode));
        if(!current){
             encounteredError = MEMORY_ALLOCATION_FAILURE;
            /* todo handle error - free all allocated memory, quit program */
        }

        /* link new labelCallNode to database */
        prev->next = current;
    }

    /* impossible value */
    if(commandOpType != I_BRANCHING && commandOpType != J_JMP && commandOpType != J_CALL_OR_LA){
        encounteredError = IMPOSSIBLE_SET_LABEL_CALL;
    }

    if(!encounteredError){
        /* update fields */
        strcpy(current->data.labelId.name, labelName);
        current->data.labelId.address = IC;
        current->data.type = commandOpType;
        strcpy(current->data.lineId.line, line);
        current->data.lineId.count = lineCounter;
    }

    return encounteredError;
}


boolean getLabelCall(labelCallsDBPtr head, int index, labelCall *destination){
    int i;
    labelCallsDBPtr current = head;

    for(i = 0; current && i < index; current = current->next, i++)
        ;

    if(current){/* labelsDB call in given index exists */
        *destination = current->data;
        return TRUE;
    }
    else{
        return FALSE;
    }
}


void clearLabelCallsDB(labelCallsDBPtr head){
    labelCallsDBPtr prev;

    while(head){
        prev = head;
        head = head->next;
        free(prev);
    }
}
