#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct definedLabel{
    labelID labelId;
    labelType type;
}definedLabel;


labelsDBPtr initLabelsDB();

boolean seekLabel(labelsDBPtr head, char *name);

errorCodes addNewLabel(labelsDBPtr head, char *labelName, long address, labelType type);

errorCodes getLabelAttributes(labelsDBPtr head, char *name, long *addressPtr, labelType *typePtr);

void updateDataLabels(labelsDBPtr head, long offset);

void clearLabels(labelsDBPtr head);

