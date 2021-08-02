#ifndef MAX_LINE
#include "data.h"
#endif


void * initDataImageDB();

boolean addNumber(int *DCPtr, long value, int numOfBytes);

boolean addNumberArray(int *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

boolean addString(int *DCPtr, char *str);
