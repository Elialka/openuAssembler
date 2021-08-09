#include <stdlib.h>
#include <string.h>

#include "entryCallsDB.h"

/* todo check if corresponding label already exists, ignore */


typedef struct entryCall *entryCallPtr;

typedef struct entryCall{
    char name[MAX_LABEL_LENGTH];
    int value;
    entryCallPtr next;
}entryCall;


void *initEntryCallsDB(){
    void *head;

    head = calloc(1, sizeof(entryCall));

    if(!head){
        /* todo print error quit*/
    }

    return head;
}


boolean addEntryCall(void *head, char *labelName, errorCodes *lineErrorPtr){
    static int entryCallsCounter = 0;/* how many entry calls currently in database */
    entryCallPtr current;
    entryCallPtr prev;

    current = head;

    if(entryCallsCounter){/* not first label */
        /* find next available node */
        while(current){
            if(!strcmp(labelName, current->name)){/* already added this name */
                return TRUE;
            }
            prev = current;
            current = current->next;
        }

        /* allocate memory for new label - memory for first label is allocated when database was initialized */
        current = calloc(1, sizeof(entryCall));
        if(!current){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }

        /* link new node to database */
        prev->next = current;
    }

    entryCallsCounter++;
    /* write down label name */
    strcpy(current->name, labelName);

    return TRUE;
}


void * getNextEntryCall(void *currentEntryPtr){
    void *nextEntryPtr;
    if(currentEntryPtr == NULL){
        nextEntryPtr = NULL;
    }
    else{
        nextEntryPtr = ((entryCallPtr)currentEntryPtr)->next;
    }

    return nextEntryPtr;
}


char * getEntryCallName(void *currentEntryPtr){
    char *namePtr;
    if(!currentEntryPtr){
        namePtr = NULL;
    }
    else{
        namePtr = ((entryCallPtr)currentEntryPtr)->name;
    }

    return namePtr;
}


void *setEntryCallValue(void *currentEntryPtr, long address) {
    if(currentEntryPtr){
        ((entryCallPtr)currentEntryPtr)->value = address;
    }
}


void clearEntryCallsDB(void *head){
    entryCallPtr current = head;
    entryCallPtr prev;

    while(current){
        prev = current;
        current = current->next;
        free(prev);
    }
}



