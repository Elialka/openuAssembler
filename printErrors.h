#ifndef MAX_LINE
#include "data.h"
#endif


void printWarningMessage(warningCodes encounteredWarning, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr);

void printFileErrorMessage(errorCodes encounteredError, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr);

void printProjectErrorMessage(projectErrors errorCode);

void divideFileErrorPrinting(fileErrorStatus *fileStatusPtr);
