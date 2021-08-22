#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "structuresDatabase.h"
#include "labelCallsDB.h"


databasePtr initLabelCallsDB(){

    return initDatabase();
}


errorCodes addLabelCall(databasePtr head, labelCall *newCallPtr) {
    databasePtr lastAddress = seekLastDatabaseEntry(head);
    labelCall *currentDataPtr = NULL;
    errorCodes encounteredError = NO_ERROR;

    if(newCallPtr->type != I_BRANCHING && newCallPtr->type != J_JMP && newCallPtr->type != J_CALL_OR_LA){
        encounteredError = IMPOSSIBLE;
    }
    else{/* legal label call */
        currentDataPtr = addNewDatabaseEntry(lastAddress, sizeof(labelCall));
        if(currentDataPtr){/* memory allocated for data */
            strcpy(currentDataPtr->labelId.name, newCallPtr->labelId.name);
            currentDataPtr->labelId.address = newCallPtr->labelId.address;
            currentDataPtr->type = newCallPtr->type;
            strcpy(currentDataPtr->lineId.line, newCallPtr->lineId.line);
            currentDataPtr->lineId.count = newCallPtr->lineId.count;
        }
        else{
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


labelCall * getLabelCallData(databasePtr currentLabelCallPtr){

    return getEntryDataPtr(currentLabelCallPtr);
}


databasePtr getNextLabelCall(databasePtr currentLabelCallPtr){

    return getNextEntryAddress(currentLabelCallPtr);
}


void clearLabelCallsDB(databasePtr head){
    clearDatabase(head);
}
