#ifndef MAX_LINE
#include "data.h"
#endif

void testFunctions(databaseRouterPtr databasesPtr);

/* pandas tests */

void testIsLabelDefinition();

void testGetStringFromLine();

void testStringToLong();

void testGetNumbersFromLine();

void testIdRegister();

void testExtractOperands(void *head);

/* ----------------------------------------------------------------------------------------------- */


/* labelsDB tests */

void testAddNewLabel(void *head);

/* ----------------------------------------------------------------------------------------------- */


/* dataImageDB tests */

void testAddNumber(void *head);

void testAddByte(void *head); /* in dataImageDB */

boolean checkDataImage(long DC, unsigned char *head, unsigned char *buffer);/* in dataImageDB */

void testAddString(void *head);

/* ----------------------------------------------------------------------------------------------- */


/* codeImageDB tests */

void printCode(void *head, long IC);
