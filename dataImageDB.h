#ifndef MAX_LINE
#include "data.h"
#endif


dataImagePtr initDataImageDB();

boolean addNumber(dataImagePtr *headPtr, long *DCPtr, long value, int numOfBytes);

errorCodes addNumberArray(dataImagePtr *headPtr, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

errorCodes addString(dataImagePtr *headPtr, long *DCPtr, char *str);

unsigned char getNextDataByte(dataImagePtr headPtr, long index);

void clearDataImageDB(dataImagePtr head);
