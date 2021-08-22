
typedef struct{
    labelID labelId;
    operationClass type;
    lineID lineId;
}labelCall;


/**
 * Initialize label calls database
 * @return pointer to the database
 */
databasePtr initLabelCallsDB();

/**
 * Add label call instance to database
 * @param head pointer to the database
 * @param labelId structure containing label name and image counter address where it was called
 * @param lineId structure containing current input line and input file line counter
 * @param commandOpType type of command
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addLabelCall(databasePtr head, labelCall *newCallPtr);

/**
 * Get current label call data
 * @param currentLabelCallPtr pointer to label call
 * @return pointer to data structure
 */
labelCall * getLabelCallData(databasePtr currentLabelCallPtr);

/**
 * Get next label call pointer
 * @param currentLabelCallPtr
 * @return next label call pointer
 */
databasePtr getNextLabelCall(databasePtr currentLabelCallPtr);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearLabelCallsDB(databasePtr head);
