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
 * @param source where to copy from
 * @param buffer where to copy to
 * @return amout of characters copied
 */
int copyNextToken(char *source, char *buffer);

/**
 * If a label is defined in current line, copy it's name
 * If input is faulty, update error code
 * @param currentPosPtr Pointer to position in line array
 * @param currentLabel Address of label name array
 * @param lineErrorPtr Pointer to line error code enum
 * @return TRUE if label is defined here, FALSE otherwise
 */
boolean isLabelDefinition(char **currentPosPtr, char *currentLabel, errorCodes *lineErrorPtr);

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
 * @param valuePtr Pointer to long variable - address will store result
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

boolean idRegister(char *token, int *regPtr, errorCodes *lineErrorPtr);

boolean extractOperands(char **currentPosPtr, operationClass commandOpType, long IC, boolean *jIsRegPtr, int *reg1Ptr,
                        int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr, void *labelCallsHead);

/**
 * Check token syntax
 * @param token The name
 * @param tokenLength Total token length
 * @param lineErrorPtr Pointer to line error code enum
 * @return TRUE if possible label name, FALSE otherwise
 */
boolean tokenIsLabel(char *token, int tokenLength, errorCodes *lineErrorPtr);

boolean readComma(char **currentPtr, errorCodes *lineErrorPtr);

boolean getFirstOperand(char *token, int tokenLength, operationClass commandOpType, long IC, boolean *jIsRegPtr,
                        int *regPtr, errorCodes *lineErrorPtr, void *labelCallsHead);

boolean getSecondOperand(char *token, operationClass commandOpType, int *regPtr, long *immedPtr,
                         errorCodes *lineErrorPtr);

boolean getThirdOperand(char *token, int tokenLength, long IC, operationClass commandOpType, int *regPtr, long *immedPtr,
                        errorCodes *lineErrorPtr, void *labelCallsHead);

boolean getLabel(char **currentPosPtr, char *labelName, errorCodes *lineErrorPtr);

boolean checkLineTermination(char **currentPtr, errorCodes *lineErrorPtr);


