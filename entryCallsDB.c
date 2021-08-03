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

    /* write down label name */
    strcpy(current->name, labelName);

    return TRUE;
}



