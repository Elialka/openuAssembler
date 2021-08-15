#ifndef MAX_LINE
#include "data.h"
#endif


dataImagePtr initDataImageDB();

boolean addNumber(dataImagePtr *headPtr, long *DCPtr, long value, int numOfBytes);

boolean addNumberArray(dataImagePtr *headPtr, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

boolean addString(dataImagePtr *headPtr, long *DCPtr, char *str);

unsigned char getNextDataByte(dataImagePtr headPtr, long index);

void clearDataImageDB(dataImagePtr headPtr);
