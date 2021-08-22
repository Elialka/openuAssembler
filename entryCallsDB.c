#include <stdlib.h>
#include <string.h>

#include "structuresDatabase.h"
#include "entryCallsDB.h"


databasePtr initEntryCallsDB(){

    return initDatabase();
}

/**
 * Check if label already declared as entry, if not, mark last address usd in the database
 * @param head pointer to database
 * @param newEntryName name of new entry call
 * @param lastAddressPtr pointer to last address variable
 * @return TRUE if label already declared as entry, FALSE otherwise
 */
static boolean entryExists(databasePtr head, char *newEntryName, databasePtr *lastAddressPtr){
    databasePtr currentAddress = head;
    databasePtr lastAddress = currentAddress;
    entryCall *currentData = NULL;
    boolean alreadyExists = FALSE;

    if(!isDBEmpty(head)){/* not first entry call */
        while(currentAddress){/* go through database, check if entry already declared *//* todo check if error strcmp for first entry call */
            currentData = getDataPtr(currentAddress);
            if(!strcmp(currentData->labelId.name, newEntryName)){/* entry exists */
                alreadyExists = TRUE;
                currentAddress = NULL;/* terminate loop */
            }
            else{/* keep looking */
                lastAddress = currentAddress;
                currentAddress = getNextUnitAddress(currentAddress);
            }
        }
    }

    if(!alreadyExists){
        *lastAddressPtr = lastAddress;
    }

    return alreadyExists;
}


errorCodes addEntryCall(databasePtr head, char *newLabelName, lineID lineId) {
    errorCodes encounteredError = NO_ERROR;
    databasePtr lastAddress = NULL;
    entryCall *newDataPtr = NULL;

    if(!entryExists(head, newLabelName, &lastAddress)){/* new entry declaration */
        newDataPtr = addNewUnit(lastAddress, sizeof(entryCall));
        if(newDataPtr){/* memory allocated for data */
            strcpy(newDataPtr->labelId.name, newLabelName);
            memcpy(&newDataPtr->lineId, &lineId, sizeof(lineID));
        }
        else{/* could not add new entry call */
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


entryCall * getEntryCallData(databasePtr entryCallPtr){

    return getDataPtr(entryCallPtr);
}


databasePtr getNextEntryCall(databasePtr entryCallPtr){

    return getNextUnitAddress(entryCallPtr);
}

void setEntryCallValue(databasePtr entryCallPtr, long address){/* check if redundant */
    entryCall *currentDataPtr = getDataPtr(entryCallPtr);

    if(currentDataPtr){
        currentDataPtr->labelId.address = address;
    }
}


boolean isEntryCallsDBEmpty(databasePtr head){
    boolean result = TRUE;

    if(head){
        result = isDBEmpty(head);
    }

    return result;
}


void clearEntryCallsDB(databasePtr head){
    clearDatabase(head);
}



