#ifndef MAX_LINE
#include "data.h"
#endif

void * initLabelCallsDB();

boolean setLabelCall(void *head, long IC, char *labelName, operationClass commandOpType, errorCodes *lineErrorPtr);

boolean getLabelCall(void *head, int index, labelCall *destination);

void clearLabelCallsDB(void *head);
