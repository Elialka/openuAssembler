#ifndef MAX_LINE
#include "data.h"
#endif

void * initLabelCallsDB();

boolean addLabelCall(void *head, int IC, char *labelName, operationClass commandOpType, errorCodes *lineErrorPtr);
