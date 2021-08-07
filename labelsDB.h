#ifndef MAX_LINE
#include "data.h"
#endif


typedef enum{
    IS_EXTERN = 'X',
    IS_ENTRY = 'N',
    DATA = 'D',
    CODE = 'C'
}attribs;


void *initLabelsDB();

boolean seekLabel(void *head, char *name);

boolean addNewLabel(void *head, char *labelName, long address, labelClass type, errorCodes *lineErrorPtr);

boolean getLabelAttributes(void *head, char *name, long *addressPtr, labelClass *typePtr);

void updateDataLabels(void *head, long ICF);

void clearLabels(void *head);

