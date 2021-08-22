#include <string.h>

#include "structuresDatabase.h"
#include "externUsesDB.h"


databasePtr initExternUsesDB(){
    return initDatabase();
}


errorCodes addExternUse(databasePtr head, char *labelName, long IC){
    databasePtr currentAddress = head;
    databasePtr lastAddress;
    labelID *currentData;
    errorCodes encounteredError = NO_ERROR;

    while(currentAddress){
        lastAddress = currentAddress;
        currentAddress = getNextUnitAddress(currentAddress);
    }

    if((currentData = addNewUnit(lastAddress, sizeof(labelID)))){/* memory allocated for data */
        strcpy(currentData->name, labelName);
        currentData->address = IC;
    }
    else{
        encounteredError = MEMORY_ALLOCATION_FAILURE;
    }

    return encounteredError;
}


boolean isExternDBEmpty(databasePtr head){
    boolean result = TRUE;

    if(head){
        /* check if first labelCallNode was used */
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
