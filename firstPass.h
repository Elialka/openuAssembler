#ifndef MAX_LINE
#include "data.h"
#endif

boolean firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr);


errorCodes extractCodeOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                               labelCallPtr labelCallsDB, long IC);
