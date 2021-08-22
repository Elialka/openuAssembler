#include "stdlib.h"

#include "structuresDatabase.h"


typedef struct unit{
    void *dataPtr;
    databasePtr next;
}unitNode;


databasePtr initDatabase(){
    databasePtr head = calloc(1, sizeof (unitNode));

    return head;
}


boolean isDBEmpty(databasePtr head){
    boolean result = TRUE;

    if(head->dataPtr){/* a node has been set */
        result = FALSE;
    }

    return result;
}


void * addNewUnit(databasePtr lastUnitAddress, int sizeOfData) {
    void * newUnitDataPtr = NULL;
    databasePtr newNode = NULL;

    if(lastUnitAddress){
        if(lastUnitAddress->dataPtr){/* current node is in use */
            newNode = calloc(1, sizeof(unitNode));
            lastUnitAddress->next = newNode;
        }
        else{/* is first node in database */
            newNode = lastUnitAddress;
        }

        if(newNode){/* allocated memory for node */
            newUnitDataPtr = lastUnitAddress->dataPtr = calloc(1, sizeOfData);
            if(!lastUnitAddress->dataPtr){/* memory allocation for data failed */
                free(newNode);
            }
        }
    }

    return newUnitDataPtr;
}


void *getDataPtr(databasePtr unitAddress){
    if(unitAddress){
        return unitAddress->dataPtr;
    }
    else{
        return NULL;
    }
}


databasePtr getNextUnitAddress(databasePtr unitAddress){
    if(unitAddress){
        return unitAddress->next;
    }
    else{
        return NULL;
    }
}


void clearDatabase(databasePtr head){
    databasePtr currentNode = head;
    databasePtr prev = NULL;

    while(currentNode){
        if(currentNode->dataPtr){
            free(currentNode->dataPtr);
        }
        prev = currentNode;
        currentNode = currentNode->next;
        free(prev);
    }
}

