#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct{
    labelID labelId;
    operationClass type;
    lineID lineId;
}labelCall;


/**
 * Initialize label calls database
 * @return pointer to the database
 */
labelCallsDBPtr initLabelCallsDB();

/**
 * Add label call instance to database
 * @param head pointer to the database
 * @param labelId structure containing label name and image counter address where it was called
 * @param lineId structure containing current input line and input file line counter
 * @param commandOpType type of command
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addLabelCall(labelCallsDBPtr head, labelCall *newCallPtr);

/**
 * Get current label call data
 * @param labelCallPtr pointer to label call
 * @return pointer to data structure
 */
labelCall * getLabelCallData(labelCallsDBPtr labelCallPtr);

/**
 * Get next label call pointer
 * @param labelCallPtr
 * @return next label call pointer
 */
labelCallsDBPtr getNextLabelCall(labelCallsDBPtr labelCallPtr);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearLabelCallsDB(labelCallsDBPtr head);
