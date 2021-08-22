#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct unit *databasePtr;

databasePtr initDatabase();

boolean isDBEmpty(databasePtr head);

void * addNewUnit(databasePtr lastUnitAddress, int sizeOfData);

void *getDataPtr(databasePtr unitAddress);

databasePtr getNextUnitAddress(databasePtr unitAddress);
