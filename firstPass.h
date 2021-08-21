#ifndef MAX_LINE
#include "data.h"
#endif

boolean firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr);



/* temp - delete - test */
errorCodes extractCodeOperandsTest(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                                   long *ICPtr, labelCallsDBPtr labelCallsDB);
