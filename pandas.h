
/**
 * Check if line should be encoded
 * @param line line of input
 * @return TRUE if line is empty or a comment line, FALSE otherwise
 * */
boolean needToReadLine(char *line);

/**
 * If a label is defined in current line, copy it's name
 * If input is faulty, update error code
 * @param currentPosPtr Pointer to position in line array
 * @param currentLabel Address of label name array
 * @param lineErrorPtr Pointer to line error code enum
 * @return TRUE if label is defined here, FALSE otherwise
 */
boolean isLabelDefinition(char **currentPosPtr, char *currentLabel, errorCodes *errorPtr);

/**
 * Get command name from input line
 * @param currentPosPtr Pointer to position in line array
 * @param commandName Address of command name array
 * @return errorCodes enum value describing function success/failure
 */
errorCodes extractCommandName(char **currentPosPtr, char *commandName);

/**
 * read string of ascii characters between enclosed quotes
 * @param currentPosPtr Pointer to position in line array
 * @param destination Pointer to string buffer - will store result
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getStringFromLine(char **currentPosPtr, char *destination);

/**
 * Read a label from current position in line array, copy label name to destination
 * @param currentPosPtr Pointer to position in line array
 * @param destination where to copy label name
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getLabelFromLine(char **currentPosPtr, char *destination);

/**
 * Read numbers from line, validate supported value range and store them in numbers array, update error enum if occurred
 * @param currentPosPtr Pointer to position in line array
 * @param numbersArray where to store numbers
 * @param dataOpType type of instruction
 * @param lineErrorPtr pointer to error code enum
 * @return amount of numbers read from line
 */
int getNumbersFromLine(char **currentPosPtr, long *numbersArray, dataOps dataOpType, errorCodes *lineErrorPtr);

/**
 * Check if a comma is present, and update line position to point after comma
 * @param currentPosPtr Pointer to position in line array
 * @return errorCodes enum value describing function success/failure
 */
errorCodes readComma(char **currentPosPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * first operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getFirstOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * second operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getSecondOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * third operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getThirdOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/**
 * Check if there is any extraneous text, set line position to last character
 * @param currentPosPtr Pointer to position in line array
 * @return errorCodes enum value describing function success/failure
 */
errorCodes checkLineTermination(char **currentPosPtr);
