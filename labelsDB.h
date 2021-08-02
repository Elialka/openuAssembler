#ifndef MAX_LINE
#include "data.h"
#endif


typedef enum{
    IS_EXTERN = 'X',
    IS_ENTRY = 'N',
    DATA = 'D',
    CODE = 'C'
}attribs;


boolean seekLabel(char *name);

boolean legalLabelDeclaration(char *currentLabel, errorCodes *lineErrorPtr);

boolean addNewLabel(char *name, int value, labelType type, errorCodes *lineErrorPtr);

void clearLabels();

