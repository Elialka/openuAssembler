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

void testAddNewLabel();

/* ----------------------------------------------------------------------------------------------- */


/* dataImageDB tests */

void testAddNumber();

void testAddByte(); /* in dataImageDB */

void printData(int DC);/* in dataImageDB */

void testAddString();

/* ----------------------------------------------------------------------------------------------- */


/* codeImageDB tests */

void printCode(void *head, int IC);

void testAddingCommands(void *head);
