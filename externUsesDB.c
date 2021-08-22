#include <string.h>

#include "global.h"
#include "structuresDatabase.h"
#include "externUsesDB.h"


databasePtr initExternUsesDB(){
    return initDatabase();
}


errorCodes addExternUse(databasePtr head, labelID labelId) {
    databasePtr lastAddress = seekLastDatabaseEntry(head);
    labelID *currentDataPtr = NULL;
    errorCodes encounteredError = NO_ERROR;

    currentDataPtr = addNewDatabaseEntry(lastAddress, sizeof(labelID));
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

    return getNextEntryAddress(currentExternUsePtr);
}


labelID * getExternUseData(databasePtr currentExternUsePtr){

    return getEntryDataPtr(currentExternUsePtr);
}


void clearExternUsesDB(databasePtr head){
    clearDatabase(head);
}
