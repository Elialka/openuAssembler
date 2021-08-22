#ifndef MAX_LINE
#include "data.h"
#endif


/**
 * Initialize extern uses database
 * @return pointer to the database
 */
databasePtr initExternUsesDB();

/**
 * Add new extern label use to the database
 * @param head pointer to database
 * @param labelName name of label used
 * @param IC address in memory where used
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addExternUse(databasePtr head, char *labelName, long IC);

/**
 * Check if database has been used
 * @param head pointer to database
 * @return TRUE if any data has been added, FALSE otherwise
 */
boolean isExternDBEmpty(databasePtr head);

/**
 * Get next extern use address in the database
 * @param currentExternUsePtr current extern use address in database
 * @return address of next extern use
 */
databasePtr getNextExternUse(databasePtr currentExternUsePtr);

/**
 * Get current extern use data
 * @param currentExternUsePtr current extern use address in database
 * @return pointer to data structure
 */
labelID * getExternUseData(databasePtr currentExternUsePtr);


/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearExternUsesDB(databasePtr head);
