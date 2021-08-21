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
entryCallsDBPtr initEntryCallsDB();

/**
 *
 * @param head
 * @param labelName
 * @param lineId
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addEntryCall(entryCallsDBPtr head, char *labelName, lineID lineId);

/**
 * Get current entry call data
 * @param entryCallPtr
 * @return pointer to data structure
 */
entryCall * getEntryCallData(entryCallsDBPtr entryCallPtr);

/**
 * Get next entry call pointer
 * @param entryCallPtr
 * @return next entry call pointer
 */
entryCallsDBPtr getNextEntryCall(entryCallsDBPtr entryCallPtr);

/*/* temp - delete
char * getEntryCallName(entryCallsDBPtr currentEntryPtr);

long getEntryCallAddress(entryCallsDBPtr currentEntryCallPtr);

char * getEntryCallLine(entryCallsDBPtr currentEntryPtr);

long getEntryCallLineCount(entryCallsDBPtr currentEntryCallPtr);
*/

/**
 * Set entry call definition address
 * @param entryCallPtr pointer to current entry call
 * @param address definition address
 */
void setEntryCallValue(entryCallsDBPtr entryCallPtr, long address);

/**
 * Check if database has been used
 * @param head pointer to database
 * @return TRUE if any data has been added, FALSE otherwise
 */
boolean isEntryCallsDBEmpty(entryCallsDBPtr head);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearEntryCallsDB(entryCallsDBPtr head);
