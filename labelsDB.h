#ifndef MAX_LINE
#include "data.h"
#endif


typedef enum{
    IS_EXTERN = 'X',
    IS_ENTRY = 'N',
    DATA = 'D',
    CODE = 'C'
}attribs;


labelPtr initLabelsDB();

boolean seekLabel(labelPtr head, char *name);

errorCodes addNewLabel(labelPtr head, char *labelName, long address, labelClass type);

boolean getLabelAttributes(labelPtr head, char *name, long *addressPtr, labelClass *typePtr);

void updateDataLabels(labelPtr head, long ICF);

void clearLabels(labelPtr head);

