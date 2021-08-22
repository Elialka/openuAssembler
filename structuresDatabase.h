#ifndef MAX_LINE
#include "data.h"
#endif

databasePtr initDatabase();

boolean isDBEmpty(databasePtr head);

databasePtr seekLastUnit(databasePtr head);

void * addNewUnit(databasePtr lastUnitAddress, int sizeOfData);

void *getDataPtr(databasePtr unitAddress);

databasePtr getNextUnitAddress(databasePtr unitAddress);

void clearDatabase(databasePtr head);
