#ifndef MAX_LINE
#include "data.h"
#endif


boolean ignoreLine(char *line);

int extractToken(char *current, char *buffer);

boolean isLabelDefinition(char **currentPosPtr, char *currentLabel);

boolean extractCommandName(char *line, int *lineIndexPtr, char *commandName,
                           boolean labelDefinition, errorCodes *lineError);

boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue);

boolean getStringFromLine(char *line, int *indexPtr, char *buffer, errorCodes *lineErrorPtr);

int getNumbersFromLine(char *line, int *indexPtr, long *buffer, dataOps dataOpType, errorCodes *lineErrorPtr);

boolean idRegister(char *token, int *regPtr, errorCodes *lineErrorPtr);

boolean extractOperands(char *line, int *lineIndexPtr, operationClass commandOpType, long IC, boolean *jIsRegPtr,
                        int *reg1Ptr, int *reg2Ptr, int *reg3Ptr, long *immedPtr,
                        errorCodes *lineErrorPtr, void *labelCallsHead);

boolean tokenIsLabel(char *token, int tokenLength, errorCodes *lineErrorPtr);

boolean readComma(char **currentPtr, errorCodes *lineErrorPtr);

boolean getFirstOperand(char *token, int tokenLength, operationClass commandOpType, long IC, boolean *jIsRegPtr,
                        int *regPtr, errorCodes *lineErrorPtr, void *labelCallsHead);

boolean getSecondOperand(char *token, operationClass commandOpType, int *regPtr, long *immedPtr,
                         errorCodes *lineErrorPtr);

boolean getThirdOperand(char *token, int tokenLength, long IC, operationClass commandOpType, int *regPtr, long *immedPtr,
                        errorCodes *lineErrorPtr, void *labelCallsHead);

boolean getLabel(char **currentPtr, char *labelName, errorCodes *lineErrorPtr);

boolean checkLineTermination(char **currentPtr, errorCodes *lineErrorPtr);


