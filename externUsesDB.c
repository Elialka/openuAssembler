#include <string.h>

#include "structuresDatabase.h"
#include "externUsesDB.h"


databasePtr initExternUsesDB(){
    return initDatabase();
}


errorCodes addExternUse(databasePtr head, labelID labelId) {
    databasePtr currentAddress = head;
    databasePtr lastAddress;
    labelID *currentDataPtr;
    errorCodes encounteredError = NO_ERROR;

    while(currentAddress){/* find last unit address in database */
        lastAddress = currentAddress;
        currentAddress = getNextUnitAddress(currentAddress);
    }

    currentDataPtr = addNewUnit(lastAddress, sizeof(labelID));
    if(currentDataPtr){/* memory allocated for data */
        strcpy(currentDataPtr->name, labelId.name);
        currentDataPtr->address = labelId.address;
    }
    else{
        encounteredError = MEMORY_ALLOCATION_FAILURE;
    }

    return encounteredError;
}


boolean isExternDBEmpty(databasePtr head){
    boolean result = TRUE;

    if(head){
        result = isDBEmpty(head);
    }

    return result;
}


databasePtr getNextExternUse(databasePtr currentExternUsePtr){

    return getNextUnitAddress(currentExternUsePtr);
}


labelID * getExternUseData(databasePtr currentExternUsePtr){

    return getDataPtr(currentExternUsePtr);
}


void clearExternUsesDB(databasePtr head){
    clearDatabase(head);
}
