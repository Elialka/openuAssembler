#ifndef MAX_LINE
#include "data.h"
#endif


void * initDataImageDB();

boolean addNumber(void *head, long *DCPtr, long value, int numOfBytes);

boolean addNumberArray(void *head, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

boolean addString(void *head, long *DCPtr, char *str);

unsigned char getNextDataByte(void *head, long index);

void clearDataImageDB(void *head);
