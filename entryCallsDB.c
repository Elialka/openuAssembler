#include <stdlib.h>
#include <string.h>

#include "entryCallsDB.h"


typedef struct entryCallsDB{
    entryCall data;
    entryCallsDBPtr next;
}entryCallNode;
/**
 * Find address to store new label call node, allocate memory or update error code enum if necessary
 * @param head pointer to the database
 * @param errorPtr pointer to error enum
 * @return pointer containing address of new node
 */

/**
 * Find address to store new entry call node, allocate memory or update error code enum if necessary
 * If identical name already exists in the database, return NULL pointer
 * @param head pointer to the database
 * @param labelName name of new entry call
 * @param errorPtr pointer to error enum
 * @return pointer containing address of new node or NULL if already registered
 */
static entryCallNode *newEntryCallNode(entryCallsDBPtr head, char *labelName, errorCodes *errorPtr);


entryCallsDBPtr initEntryCallsDB(){
    entryCallsDBPtr head = calloc(1, sizeof(entryCallNode));

    return head;
}


static entryCallNode *newEntryCallNode(entryCallsDBPtr head, char *labelName, errorCodes *errorPtr) {
    entryCallNode *current = head;/* current node being examined */
    entryCallNode *prev = NULL;/* pointer to previous node */
    boolean alreadyExists = FALSE;

    if(!isEntryCallsDBEmpty(head)){/* not first entry call in database - need to allocate memory */
        /* go through database, check if entry call already registered */
        while(!alreadyExists && current){
            if(!strcmp(labelName, current->data.labelId.name)){/* already added this name */
                alreadyExists = TRUE;
            }
            prev = current;
            current = current->next;
        }

        if(!alreadyExists){
            /* allocate memory for new entry call node */
            current = calloc(1, sizeof(entryCallNode));
            if(!current){
                *errorPtr = MEMORY_ALLOCATION_FAILURE;
            }

            /* link new node to database */
            prev->next = current;
        }
        else{/* entry call already exists */
            current = NULL;
        }
    }

    return current;
}


errorCodes addEntryCall(entryCallsDBPtr head, char *labelName, lineID lineId) {
    errorCodes encounteredError = NO_ERROR;
    entryCallNode *current = NULL;/* pointer to new node */

    if(!head){
        encounteredError = IMPOSSIBLE_ENCODE_DATA;
    }
    else{
        current = newEntryCallNode(head, labelName, &encounteredError);
    }

    if(current){
        /* add new entry call */
        strcpy(current->data.labelId.name, labelName);
        strcpy(current->data.lineId.line, lineId.line);
        current->data.lineId.count = lineId.count;
    }

    return encounteredError;
}


/* todo try const return value */
entryCall * getEntryCallData(entryCallsDBPtr entryCallPtr){
    entryCallNode *current = entryCallPtr;
    entryCall *data = NULL;

    if(current){
        data = &current->data;
    }

    return data;
}


entryCallsDBPtr getNextEntryCall(entryCallsDBPtr entryCallPtr){
    entryCallNode *current = entryCallPtr;
    entryCallsDBPtr nextEntryPtr = NULL;

    if(current){
        nextEntryPtr = current->next;
    }

    return nextEntryPtr;
}

void setEntryCallValue(entryCallsDBPtr entryCallPtr, long address){
    if(entryCallPtr){
        entryCallPtr->data.labelId.address = address;
    }
}


boolean isEntryCallsDBEmpty(entryCallsDBPtr head){
    boolean result = TRUE;

    if(head){
        /* check if first node's name is empty */
        result = *(head->data.labelId.name) ? FALSE : TRUE;
    }

    return result;
}


void clearEntryCallsDB(entryCallsDBPtr head){
    entryCallsDBPtr prev;

    while(head){
        prev = head;
        head = head->next;
        free(prev);
    }
}



