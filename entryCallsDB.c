#include <stdlib.h>
#include <string.h>

#include "entryCallsDB.h"

/* todo check if corresponding label already exists, ignore */


typedef struct entryCall{
    char name[MAX_LABEL_LENGTH];
    long value;
    entryCallPtr next;
}entryCall;


entryCallPtr initEntryCallsDB(){
    entryCallPtr head = calloc(1, sizeof(entryCall));

    return head;
}


errorCodes addEntryCall(entryCallPtr head, char *labelName) {
    entryCallPtr current;
    entryCallPtr prev;
    errorCodes encounteredError = NO_ERROR;
    boolean finished = FALSE;/* flag turns on if entry name is already on database or an error occurred */

    current = head;

    if(!isEntryCallDBEmpty(head)){/* not first entry call */
        /* find next available node */
        while(!finished && current){
            if(!strcmp(labelName, current->name)){/* already added this name */
                finished = TRUE;
            }
            prev = current;
            current = current->next;
        }

        if(!finished){
            /* allocate memory for new node call - memory for first node is allocated when database was initialized */
            current = calloc(1, sizeof(entryCall));
            if(!current){
                encounteredError = MEMORY_ALLOCATION_FAILURE;
                finished = TRUE;
            }

            /* link new node to database */
            prev->next = current;
        }
    }

    if(!finished){
        /* add new extern use */
        strcpy(current->name, labelName);
    }

    return encounteredError;
}


entryCallPtr getNextEntryCall(entryCallPtr currentEntryPtr){
    entryCallPtr nextEntryPtr;
    if(currentEntryPtr == NULL){
        nextEntryPtr = NULL;
    }
    else{
        nextEntryPtr = ((entryCallPtr)currentEntryPtr)->next;
    }

    return nextEntryPtr;
}


char * getEntryCallName(entryCallPtr currentEntryPtr){
    char *namePtr;
    if(!currentEntryPtr){
        namePtr = NULL;
    }
    else{
        namePtr = ((entryCallPtr)currentEntryPtr)->name;
    }

    return namePtr;
}


long getEntryCallAddress(entryCallPtr currentEntryCallPtr){
    long address;

    if(!currentEntryCallPtr){
        address = 0;
    }
    else{
        address =  ((entryCallPtr)currentEntryCallPtr)->value;
    }

    return address;
}


void setEntryCallValue(entryCallPtr currentEntryPtr, long address) {
    if(currentEntryPtr){
        ((entryCallPtr)currentEntryPtr)->value = address;
    }
}


boolean isEntryCallDBEmpty(entryCallPtr head){
    boolean result;

    /* check if first node's name is empty */
    result = *(((entryCallPtr)head)->name) ? FALSE : TRUE;

    return result;
}


void clearEntryCallsDB(entryCallPtr head){
    entryCallPtr current = head;
    entryCallPtr prev;

    while(current){
        prev = current;
        current = current->next;
        free(prev);
    }
}



