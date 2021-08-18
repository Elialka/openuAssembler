#ifndef MAX_LINE
#include "data.h"
#endif

labelCallPtr initLabelCallsDB();

errorCodes
setLabelCall(labelCallPtr head, long IC, char *labelName, operationClass commandOpType, char *line, long lineCounter);

boolean getLabelCall(labelCallPtr head, int index, labelCall *destination);

void clearLabelCallsDB(labelCallPtr head);
