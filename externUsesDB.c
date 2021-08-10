#include <stdlib.h>
#include <string.h>

#include "externUsesDB.h"

typedef struct externUse *externUsePtr;

typedef struct externUse{
    char name[MAX_LABEL_LENGTH];
    long IC;
    externUsePtr next;
}externUse;


void *initExternUsesDB(){
    void *head;

    head = calloc(1, sizeof(externUse));

    if(!head){
        /* todo print error quit*/
    }

    return head;
}


boolean addExternUse(void *head, char *labelName, long IC, errorCodes *errorPtr){
    externUsePtr current = head;
    externUsePtr prev;
    boolean result = TRUE;

    if(!externDBIsEmpty(head)){/* not first extern use */
        /* mark first node use */
        /* find next available node */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new node - memory for first node is allocated when database was initialized */
        current = calloc(1, sizeof(externUse));
        if(!current){/* memory allocation failed */
            *errorPtr = MEMORY_ALLOCATION_FAILURE;
            result =  FALSE;
        }

        /* link new node to database */
        prev->next = current;
    }

    /* add new extern use */
    strcpy(current->name, labelName);
    current->IC = IC;

    return result;
}


boolean externDBIsEmpty(void *head){
    boolean result;

    /* check if first node was used */
    result = ((externUsePtr)head)->IC ? FALSE : TRUE;

    return result;
}


void * getNextExternUse(void *currentExternUsePtr){
    void *nextExternUsePtr;
    if(!currentExternUsePtr){
        nextExternUsePtr = NULL;
    }
    else{
        nextExternUsePtr = ((externUsePtr)currentExternUsePtr)->next;
    }

    return nextExternUsePtr;
}


char * getExternUseName(void *currentExternUsePtr){
    char *namePtr;

    if(!currentExternUsePtr){
        namePtr = NULL;
    }
    else{
        namePtr = ((externUsePtr)currentExternUsePtr)->name;
    }

    return namePtr;
}


long getExternUseAddress(void *currentExternUsePtr){
    long address;

    if(!currentExternUsePtr){
        address = 0;
    }
    else{
        address = ((externUsePtr)currentExternUsePtr)->IC;
    }

    return address;
}


void clearExternUsesDB(void *head){
    externUsePtr current = head;
    externUsePtr prev;

    while(current){
        prev = current;
        current = current->next;
        free(prev);
    }
}
