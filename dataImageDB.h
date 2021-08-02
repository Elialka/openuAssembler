#ifndef MAX_LINE
#include "data.h"
#endif


void * initDataImageDB();

boolean addNumber(void *head, int *DCPtr, long value, int numOfBytes);

boolean addNumberArray(void *head, int *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

boolean addString(void *head, int *DCPtr, char *str);
