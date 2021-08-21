#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct{
    labelID labelId;
    operationClass type;
    lineID lineId;
}labelCall;

labelCallsDBPtr initLabelCallsDB();

errorCodes
setLabelCall(labelCallsDBPtr head, long IC, char *labelName, operationClass commandOpType, char *line, long lineCounter);

boolean getLabelCall(labelCallsDBPtr head, int index, labelCall *destination);

void clearLabelCallsDB(labelCallsDBPtr head);
