#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct{
    labelID labelId;
    lineID lineId;
}entryCall;

/**
 * Initialize entry calls database
 * @return pointer to the database
 */
databasePtr initEntryCallsDB();

/**
 * Add new entry declaration to the database
 * @param head pointer to database
 * @param newLabelName name of label declared
 * @param lineId structure containing line array + counter
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addEntryCall(databasePtr head, char *newLabelName, lineID lineId);

/**
 * Get current entry call data
 * @param entryCallPtr current entry call address in database
 * @return pointer to data structure
 */
entryCall * getEntryCallData(databasePtr entryCallPtr);

/**
 * Get next entry call address in the database
 * @param entryCallPtr current entry call address in database
 * @return address of next entry call
 */
databasePtr getNextEntryCall(databasePtr entryCallPtr);

/**
 * Set entry call definition address
 * @param entryCallPtr pointer to current entry call
 * @param address definition address
 */
void setEntryCallValue(databasePtr entryCallPtr, long address);

/**
 * Check if database has been used
 * @param head pointer to database
 * @return TRUE if any data has been added, FALSE otherwise
 */
boolean isEntryCallsDBEmpty(databasePtr head);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearEntryCallsDB(databasePtr head);
