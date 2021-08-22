#include <stdlib.h>
#include <string.h>

#include "externUsesDB.h"

typedef struct externUsesDB{
    labelID labelId;
    externUsesDBPtr next;
}externUseNode;


externUsesDBPtr initExternUsesDB(){
    externUsesDBPtr head = calloc(1, sizeof(externUseNode));

    return head;
}


errorCodes addExternUse(externUsesDBPtr head, char *labelName, long IC){
    externUsesDBPtr current = head;
    externUsesDBPtr prev;
    errorCodes encounteredError = NO_ERROR;

    if(!isExternDBEmpty(head)){/* not first extern use */
        /* find next available labelCallNode */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new labelCallNode - memory for first labelCallNode is allocated when database was initialized */
        current = calloc(1, sizeof(externUseNode));
        if(!current){/* memory allocation failed */
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
        else{/* allocated successfully */
            /* link new labelCallNode to database */
            prev->next = current;
        }
    }

    /* add new extern use */
    strcpy(current->labelId.name, labelName);
    current->labelId.address = IC;

    return encounteredError;
}


boolean isExternDBEmpty(externUsesDBPtr head){
    boolean result = TRUE;

    if(head){
        /* check if first labelCallNode was used */
        result = ((externUsesDBPtr)head)->labelId.address ? FALSE : TRUE;
    }

    return result;
}


externUsesDBPtr getNextExternUse(externUsesDBPtr currentExternUsePtr){
    externUsesDBPtr nextExternUsePtr;

    if(!currentExternUsePtr){
        nextExternUsePtr = NULL;
    }
    else{
        nextExternUsePtr = currentExternUsePtr->next;
    }

    return nextExternUsePtr;
}


labelID * getExternUseData(externUsesDBPtr currentExternUsePtr){
    labelID *data;

    if(currentExternUsePtr){
        data = &currentExternUsePtr->labelId;
    }
    else{
        data = NULL;
    }

    return data;
}


void clearExternUsesDB(externUsesDBPtr head){
    externUsesDBPtr prev;

    while(head){
        prev = head;
        head = head->next;
        free(prev);
    }
}
