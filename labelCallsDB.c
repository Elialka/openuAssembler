#include <stdlib.h>
#include <string.h>

#include "labelCallsDB.h"


typedef struct labelCallsDB{
    labelCall data;
    labelCallsDBPtr next;
}labelCallNode;

/**
 * Find address to store new label call node, allocate memory or update error code enum if necessary
 * @param head pointer to the database
 * @param errorPtr pointer to error enum
 * @return pointer containing address of new node
 */
static labelCallsDBPtr newLabelCallNode(labelCallsDBPtr head, errorCodes *errorPtr);

/**
 * Check if database has been used
 * @param head pointer to database
 * @return TRUE if any data has been added, FALSE otherwise
 */
static boolean isLabelCallsEmpty(labelCallsDBPtr head);


labelCallsDBPtr initLabelCallsDB(){
    labelCallsDBPtr head = calloc(1, sizeof(labelCallNode));

    return head;
}


static labelCallsDBPtr newLabelCallNode(labelCallsDBPtr head, errorCodes *errorPtr){
    labelCallsDBPtr current = head;/* current node being examined */
    labelCallsDBPtr prev = current;/* pointer to previous node */

    if(!isLabelCallsEmpty(head)){/* not first label call in database - need to allocate memory */
        /* find last labelsDB call record */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new label call node */
        current = calloc(1, sizeof(labelCallNode));
        if(!current){
            *errorPtr = MEMORY_ALLOCATION_FAILURE;
        }

        /* link new node to database */
        prev->next = current;
    }

    return current;
}


errorCodes addLabelCall(labelCallsDBPtr head, labelCall *newCallPtr) {
    errorCodes encounteredError = NO_ERROR;
    labelCallsDBPtr current = NULL;/* pointer to new node */

    if(!head){
        encounteredError = IMPOSSIBLE_SET_LABEL_CALL;
    }
    else{
        current = newLabelCallNode(head, &encounteredError);
    }

    /* impossible value */
    if(newCallPtr->type != I_BRANCHING && newCallPtr->type != J_JMP && newCallPtr->type != J_CALL_OR_LA){
        encounteredError = IMPOSSIBLE_SET_LABEL_CALL;
    }

    if(!encounteredError){
        /* update fields */
        strcpy(current->data.labelId.name, newCallPtr->labelId.name);
        current->data.labelId.address = newCallPtr->labelId.address;
        current->data.type = newCallPtr->type;
        strcpy(current->data.lineId.line, newCallPtr->lineId.line);
        current->data.lineId.count = newCallPtr->lineId.count;
    }

    return encounteredError;
}


labelCall * getLabelCallData(labelCallsDBPtr labelCallPtr){
    labelCall *data = NULL;

    if(labelCallPtr){
        data = &labelCallPtr->data;
    }

    return data;
}


labelCallsDBPtr getNextLabelCall(labelCallsDBPtr labelCallPtr){
    labelCallsDBPtr data = NULL;

    if(labelCallPtr){
        data = labelCallPtr->next;
    }

    return data;
}


static boolean isLabelCallsEmpty(labelCallsDBPtr head){
    boolean result = TRUE;

    if(head){
        /* check if first node's name is empty */
        result = *(head->data.labelId.name) ? FALSE : TRUE;
    }

    return result;
}


void clearLabelCallsDB(labelCallsDBPtr head){
    labelCallsDBPtr prev;

    while(head){
        prev = head;
        head = head->next;
        free(prev);
    }
}
