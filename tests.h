#ifndef MAX_LINE
#include "data.h"
#endif


/* pandas tests */

void testIsLabelDefinition();

void testGetStringFromLine();

void testStringToLong();

void testGetNumbersFromLine();

void testIdRegister();

void testExtractOperands();

/* ----------------------------------------------------------------------------------------------- */


/* labelsDB tests */

void testAddNewLabel(void *head);

/* ----------------------------------------------------------------------------------------------- */


/* dataImageDB tests */

void testAddNumber(void *head);

void testAddByte(void *head); /* in dataImageDB */

void printNumbers(int DC, char *head);/* in dataImageDB */

void printData(int DC, char *head);/* in dataImageDB */

void testAddString(void *head);

/* ----------------------------------------------------------------------------------------------- */


/* codeImageDB tests */

void printCode(void *head, int IC);

void testAddingCommands(void *head);
