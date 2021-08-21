#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct{
    labelID labelId;
    lineID lineId;
}entryCall;

entryCallsDBPtr initEntryCallsDB();

errorCodes addEntryCall(entryCallsDBPtr head, char *labelName, char *line, long lineCounter);

entryCallsDBPtr getNextEntryCall(entryCallsDBPtr currentEntryPtr);

char * getEntryCallName(entryCallsDBPtr currentEntryPtr);

long getEntryCallAddress(entryCallsDBPtr currentEntryCallPtr);

char * getEntryCallLine(entryCallsDBPtr currentEntryPtr);

long getEntryCallLineCount(entryCallsDBPtr currentEntryCallPtr);

void setEntryCallValue(entryCallsDBPtr currentEntryPtr, long address);

boolean isEntryCallDBEmpty(entryCallsDBPtr head);

void clearEntryCallsDB(entryCallsDBPtr head);
