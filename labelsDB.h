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

boolean addNewLabel(void *head, char *labelName, int value, labelType type, errorCodes *lineErrorPtr);

void clearLabels(void *head);

