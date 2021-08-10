#ifndef MAX_LINE
#include "data.h"
#endif

void *initExternUsesDB();

boolean addExternUse(void *head, char *labelName, long IC, errorCodes *errorPtr);

boolean externDBIsEmpty(void *head);

void * getNextExternUse(void *currentExternUsePtr);

char * getExternUseName(void *currentExternUsePtr);

long getExternUseAddress(void *currentExternUsePtr);

void clearExternUsesDB(void *head);
