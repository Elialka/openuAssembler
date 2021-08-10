#ifndef MAX_LINE
#include "data.h"
#endif

void *initExternUsesDB();

boolean addExternUse(void *head, char *labelName, long IC, errorCodes *errorPtr);

boolean externDBIsEmpty(void *head);



void clearExternUsesDB(void *head);
