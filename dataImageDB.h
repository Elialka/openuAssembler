#ifndef MAX_LINE
#include "global.h"
#endif


/**
 * Initialize data image database
 * @return pointer to the database
 */
dataImageDBPtr initDataImageDB();

/**
 * Add an array of numbers to the database
 * @param headPtr pointer to database
 * @param DCPtr pointer to DC counter
 * @param array array of numbers
 * @param amountOfNumbers how many numbers to add from array
 * @param dataOpType enum value of data type
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addNumberArray(dataImageDBPtr *headPtr, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType);

/**
 * Add a string of characters to the database
 * @param headPtr pointer to database
 * @param DCPtr pointer to DC counter
 * @param str the string
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addString(dataImageDBPtr *headPtr, long *DCPtr, char *str);

/**
 * Get the serial byte in the data image, as specified by index
 * @param headPtr pointer to database
 * @param index byte index in data image
 * @return the byte as unsigned char
 */
unsigned char getNextDataByte(void *headPtr, long index);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearDataImageDB(dataImageDBPtr head);
