#ifndef MAX_LINE
#include "data.h"
#endif


boolean ignoreLine(char *line);

int extractToken(char *current, char *buffer);

boolean isLabelDefinition(char *line, int *lineIndexPtr, char *currentLabel);

boolean extractCommandName(char *line, int *lineIndexPtr, char *commandName,
                           boolean labelDefinition, errorCodes *lineError);

boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue);

boolean getStringFromLine(char *line, int *indexPtr, char *buffer, errorCodes *lineErrorPtr);

int getNumbersFromLine(char *line, int *indexPtr, long *buffer, dataOps dataOpType, errorCodes *lineErrorPtr);

boolean idRegister(char *token, int *regPtr, errorCodes *lineErrorPtr);

boolean
extractOperands(char *line, int *lineIndexPtr, operationClass commandOpType, int IC, boolean *jumIsRegPtr, int *reg1Ptr,
                int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr);

boolean tokenIsLabel(char *token, int tokenLength, errorCodes *lineErrorPtr);

boolean validateComma(char **currentPtr, errorCodes *lineErrorPtr);
