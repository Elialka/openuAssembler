#ifndef MAX_LINE
#include "data.h"
#endif

void *initEntryCallsDB();

boolean addEntryCall(void *head, char *labelName, errorCodes *lineErrorPtr);

void * getNextEntryCall(void *entryCallPtr);

char * getEntryCallName(void *currentEntryPtr);

long getEntryCallAddress(void *currentExternUsePtr);

void *setEntryCallValue(void *currentEntryPtr, long address);

boolean entryCallDBIsEmpty(void *head);

void clearEntryCallsDB(void *head);
