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

boolean addNewLabel(labelPtr head, char *labelName, long address, labelClass type, errorCodes *lineErrorPtr);

boolean getLabelAttributes(labelPtr head, char *name, long *addressPtr, labelClass *typePtr);

void updateDataLabels(labelPtr head, long ICF);

void clearLabels(labelPtr head);

