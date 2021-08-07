#ifndef MAX_LINE
#include "data.h"
#endif

void *initEntryCallsDB();

boolean addEntryCall(void *head, char *labelName, errorCodes *lineErrorPtr);

void clearEntryCallsDB(void *head);
