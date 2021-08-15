#ifndef MAX_LINE
#include "data.h"
#endif

entryCallPtr initEntryCallsDB();

boolean addEntryCall(entryCallPtr head, char *labelName, errorCodes *lineErrorPtr);

entryCallPtr getNextEntryCall(entryCallPtr currentEntryPtr);

char * getEntryCallName(entryCallPtr currentEntryPtr);

long getEntryCallAddress(entryCallPtr currentEntryCallPtr);

void setEntryCallValue(entryCallPtr currentEntryPtr, long address);

boolean entryCallDBIsEmpty(entryCallPtr head);

void clearEntryCallsDB(entryCallPtr head);
