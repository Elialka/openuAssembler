#include <stdlib.h>
#include <string.h>

#include "labelCallsDB.h"


typedef struct call *labelCallPtr;

typedef struct call{
    int IC;
    char name[MAX_LABEL_LENGTH];
    operationClass type;
    labelCallPtr next;
}labelCall;


void * initLabelCallsDB(){
    void *head;

    head = calloc(1, sizeof(labelCall));

    if(!head){
        /* todo print error */
    }

    return head;

}


boolean addLabelCall(void *head, int IC, char *labelName, operationClass commandOpType, errorCodes *lineErrorPtr) {
    static int labelCallsCounter = 0;/* counts how many label calls added to the database */
    labelCallPtr current;
    labelCallPtr prev;

    current = head;

    if(labelCallsCounter){/* not first label call */
        /* find last label call record */
        while(current){
            prev = current;
            current = current->next;
        }

        /* allocate memory for new label - memory for first label is allocated when database was initialized */
        current = calloc(1, sizeof(labelCall));
        if(!current){
            *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
            return FALSE;
        }

        prev->next = current;
    }

    /* impossible value */
    if(commandOpType != I_BRANCHING && commandOpType != J_JUMP){
        *lineErrorPtr = IMPOSSIBLE;
        return FALSE;
    }

    /* count another label call */
    labelCallsCounter++;

    /* update fields */
    current->IC = IC;
    strcpy(current->name, labelName);
    current->type = commandOpType;

    return TRUE;
}
