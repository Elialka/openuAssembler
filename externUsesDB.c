#include <stdlib.h>

#include "externUsesDB.h"

typedef struct externUse *externUsePtr;

typedef struct externUse{
    char name[MAX_LABEL_LENGTH];
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


boolean addExternUse(void *head){

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