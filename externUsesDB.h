#ifndef MAX_LINE
#include "data.h"
#endif

externUsePtr initExternUsesDB();

boolean addExternUse(externUsePtr head, char *labelName, long IC, errorCodes *errorPtr);

boolean isExternDBEmpty(externUsePtr head);

externUsePtr getNextExternUse(externUsePtr currentExternUsePtr);

char * getExternUseName(externUsePtr currentExternUsePtr);

long getExternUseAddress(externUsePtr currentExternUsePtr);

void clearExternUsesDB(externUsePtr head);
