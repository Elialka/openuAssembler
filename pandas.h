#ifndef MAX_LINE
#include "data.h"
#endif

/**
 *
 * @param line line of input
 * @return TRUE if line is empty or a comment line, FALSE otherwise
 * */
boolean ignoreLine(char *line);

/**
 * Copy next token from source to buffer, terminate buffer with '/0'
 * Stops copying at white character/end of string/comma
 * @param sourcePtr where to copy from
 * @param buffer where to copy to
 * @return amout of characters copied
 */
int extractNextToken(char **sourcePtr, char *buffer);

/**
 * If a label is defined in current line, copy it's name
 * If input is faulty, update error code
 * @param currentPosPtr Pointer to position in line array
 * @param currentLabel Address of label name array
 * @param lineErrorPtr Pointer to line error code enum
 * @return TRUE if label is defined here, FALSE otherwise
 */
errorCodes isLabelDefinition(char **currentPosPtr, char *currentLabel);

/**
 * Get command name from input line
 * @param currentPosPtr Position in line array
 * @param commandName Address of command name array
 * @return errorCodes enum value describing function success/failure
 */
errorCodes extractCommandName(char **currentPosPtr, char *commandName);

/**
 * Convert number string to long variable, validating possible range
 * @param token number string
 * @param valuePtr Pointer to long variable - result will be stored at this address
 * @param endPtrPtr Will be set to point to the first character after number
 * @param maxValue Maximum positive value possible with current representation
 * @return TRUE if number is in range, FALSE otherwise
 */
boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue);

/**
 * read string of ascii characters between enclosed quotes
 * @param currentPosPtr Position in line array
 * @param destination Pointer to string buffer - will store result
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getStringFromLine(char **currentPosPtr, char *destination);

int getNumbersFromLine(char **currentPosPtr, long *buffer, dataOps dataOpType, errorCodes *lineErrorPtr);


/**
 * Check if token syntax matches label name as parameter
 * @param token The name
 * @param tokenLength Total token length
 * @param lineErrorPtr Pointer to line error code enum
 * @return errorCodes enum value describing function success/failure
 */
errorCodes tokenIsLabel(char *token, int tokenLength);

/**
 * Check if a comma is present, and update line position to point after comma
 * @param currentPtr pointer to current location pointer
 * @return errorCodes enum value describing function success/failure
 */
errorCodes readComma(char **currentPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * first operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getFirstOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * second operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getSecondOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/**
 * Read next token in line, check if operand type matches operation type's
 * third operand specification, save relevant information about operand,
 * and increment line position
 * @param currentPosPtr position in line array
 * @param commandOpType type of current command
 * @param operandAttributesPtr pointer to struct to save relevant information in
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getThirdOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr);

/* delete after firstPass refactor */
boolean getFirstOperandOLD(char *token, int tokenLength, operationClass commandOpType, long IC, boolean *jIsRegPtr,
                           int *regPtr, errorCodes *lineErrorPtr, void *labelCallsHead);
/* delete after firstPass refactor */
boolean getSecondOperandOLD(char *token, operationClass commandOpType, int *regPtr, long *immedPtr,
                            errorCodes *lineErrorPtr);
/* delete after firstPass refactor */
boolean getThirdOperandOLD(char *token, int tokenLength, long IC, operationClass commandOpType, int *regPtr, long *immedPtr,
                           errorCodes *lineErrorPtr, void *labelCallsHead);

/* delete after firstPass refactor */
boolean extractOperandsOLD(char **currentPosPtr, operationClass commandOpType, long IC, boolean *jIsRegPtr, int *reg1Ptr,
                           int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr, void *labelCallsHead);

/**
 * Read token, try to id register number
 * @param token string representing register id without white characters
 * @param regPtr pointer to result buffer
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getRegisterOperand(char *token, unsigned char *regPtr);

/**
 * Read token, check if possible label name and copy to destination
 * @param token string representing label name without white characters
 * @param tokenLength number of characters in token
 * @param destination pointer to store name if legal
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getLabelOperand(char *token, int tokenLength, char *destination);

/**
 * Read token, try convert string to integer
 * @param token string representing number without white characters
 * @param destination
 * @return
 */
errorCodes getNumberOperand(char *token, int *destination);

boolean checkLineTermination(char **currentPtr, errorCodes *lineErrorPtr);


