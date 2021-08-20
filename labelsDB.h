#ifndef MAX_LINE
#include "data.h"
#endif


labelPtr initLabelsDB();

boolean seekLabel(labelPtr head, char *name);

errorCodes addNewLabel(labelPtr head, char *labelName, long address, labelClass type);

boolean getLabelAttributes(labelPtr head, char *name, long *addressPtr, labelClass *typePtr);

void updateDataLabels(labelPtr head, long offset);

void clearLabels(labelPtr head);

