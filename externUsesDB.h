#ifndef MAX_LINE
#include "data.h"
#endif

externUsesDBPtr initExternUsesDB();

errorCodes addExternUse(externUsesDBPtr head, char *labelName, long IC);

boolean isExternDBEmpty(externUsesDBPtr head);

externUsesDBPtr getNextExternUse(externUsesDBPtr currentExternUsePtr);

char * getExternUseName(externUsesDBPtr currentExternUsePtr);

long getExternUseAddress(externUsesDBPtr currentExternUsePtr);

void clearExternUsesDB(externUsesDBPtr head);
