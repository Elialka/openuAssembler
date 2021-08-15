#ifndef MAX_LINE
#include "data.h"
#endif

labelCallPtr initLabelCallsDB();

boolean setLabelCall(labelCallPtr head, long IC, char *labelName, operationClass commandOpType, errorCodes *lineErrorPtr);

boolean getLabelCall(labelCallPtr head, int index, labelCall *destination);

void clearLabelCallsDB(labelCallPtr head);
