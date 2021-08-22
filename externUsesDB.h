#ifndef MAX_LINE
#include "data.h"
#endif


/**
 * Initialize extern uses database
 * @return pointer to the database
 */
externUsesDBPtr initExternUsesDB();

/**
 *
 * @param head pointer to database
 * @param labelName
 * @param IC
 * @return
 */
errorCodes addExternUse(externUsesDBPtr head, char *labelName, long IC);

/**
 *
 * @param head pointer to database
 * @return
 */
boolean isExternDBEmpty(externUsesDBPtr head);

/**
 *
 * @param currentExternUsePtr
 * @return
 */
externUsesDBPtr getNextExternUse(externUsesDBPtr currentExternUsePtr);


labelID * getExternUseData(externUsesDBPtr currentExternUsePtr);

/**
 *
 * @param currentExternUsePtr
 * @return
 */
char * getExternUseName(externUsesDBPtr currentExternUsePtr);

/**
 *
 * @param currentExternUsePtr
 * @return
 */
long getExternUseAddress(externUsesDBPtr currentExternUsePtr);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearExternUsesDB(externUsesDBPtr head);
