#include <stdlib.h>
#include <string.h>

#include "entryCallsDB.h"

/* todo check if corresponding labelsDB already exists, ignore */


typedef struct entryCallsDB{
    entryCall data;
    entryCallsDBPtr next;
}entryCallNode;


entryCallsDBPtr initEntryCallsDB(){
    entryCallsDBPtr head = calloc(1, sizeof(entryCallNode));

    return head;
}


errorCodes addEntryCall(entryCallsDBPtr head, char *labelName, char *line, long lineCounter) {
    entryCallsDBPtr current;
    entryCallsDBPtr prev;
    errorCodes encounteredError = NO_ERROR;
    boolean finished = FALSE;/* flag turns on if entry name is already on database or an error occurred */

    current = head;

    if(!isEntryCallDBEmpty(head)){/* not first entry call */
        /* find next available labelCallNode */
        while(!finished && current){
            if(!strcmp(labelName, current->data.labelId.name)){/* already added this name */
                finished = TRUE;
            }
            prev = current;
            current = current->next;
        }

        if(!finished){
            /* allocate memory for new labelCallNode call - memory for first labelCallNode is allocated when database was initialized */
            current = calloc(1, sizeof(entryCallNode));
            if(!current){
                encounteredError = MEMORY_ALLOCATION_FAILURE;
                finished = TRUE;
            }

            /* link new labelCallNode to database */
            prev->next = current;
        }
    }

    if(!finished){
        /* add new extern use */
        strcpy(current->data.labelId.name, labelName);
        strcpy(current->data.lineId.line, line);
        current->data.lineId.count = lineCounter;
    }

    return encounteredError;
}


entryCallsDBPtr getNextEntryCall(entryCallsDBPtr currentEntryPtr){
    entryCallsDBPtr nextEntryPtr;
    if(currentEntryPtr == NULL){
        nextEntryPtr = NULL;
    }
    else{
        nextEntryPtr = currentEntryPtr->next;
    }

    return nextEntryPtr;
}


char * getEntryCallName(entryCallsDBPtr currentEntryPtr){
    char *namePtr;
    if(!currentEntryPtr){
        namePtr = NULL;
    }
    else{
        namePtr = currentEntryPtr->data.labelId.name;
    }

    return namePtr;
}


long getEntryCallAddress(entryCallsDBPtr currentEntryCallPtr){
    long address;

    if(!currentEntryCallPtr){
        address = 0;
    }
    else{
        address =  currentEntryCallPtr->data.labelId.address;
    }

    return address;
}


char * getEntryCallLine(entryCallsDBPtr currentEntryPtr){
    char *line;
    if(!currentEntryPtr){
        line = NULL;
    }
    else{
        line = currentEntryPtr->data.lineId.line;
    }

    return line;
}


long getEntryCallLineCount(entryCallsDBPtr currentEntryCallPtr){
    long lineCount;

    if(!currentEntryCallPtr){
        lineCount = 0;
    }
    else{
        lineCount =  currentEntryCallPtr->data.lineId.count;
    }

    return lineCount;
}


void setEntryCallValue(entryCallsDBPtr currentEntryPtr, long address) {
    if(currentEntryPtr){
        currentEntryPtr->data.labelId.address = address;
    }
}


boolean isEntryCallDBEmpty(entryCallsDBPtr head){
    boolean result;

    /* check if first labelCallNode's name is empty */
    result = *(head->data.labelId.name) ? FALSE : TRUE;

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



