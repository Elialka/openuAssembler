#include <stdio.h>
#include <string.h>
#include "tests.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "labelsDB.h"
#include "codeImageDB.h"



/* pandas tests */

void testIsLabelDefinition()
{
    char currentLabel[TOKEN_ARRAY_SIZE];
    char *currentPos;
    char *temp;
    boolean generalError;

    generalError = FALSE;

    /* test 1*/
    currentPos = "  	hELLO: ";
    if (!isLabelDefinition(&currentPos, currentLabel))
    {
        printf("isLabelDefinition test 1 failed - return value\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "hELLO")){
        printf("isLabelDefinition test 1 failed - currentLabel string\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 1 failed - currentPos\n");
        generalError = TRUE;
    }

    /* test 2*/
    currentPos = "asdf: ";
    if (!isLabelDefinition(&currentPos, currentLabel))
    {
        printf("isLabelDefinition test 2 failed - return value\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "asdf")){
        printf("isLabelDefinition test 2 failed - currentLabel string\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 2 failed - currentPos\n");
        generalError = TRUE;
    }


    /* test 3*/
    currentPos = "asdd2:";
    if (!isLabelDefinition(&currentPos, currentLabel))
    {
        printf("isLabelDefinition test 3 failed - return value\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "asdd2")){
        printf("isLabelDefinition test 3 failed - currentLabel string\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 3 failed - currentPos\n");
        generalError = TRUE;
    }

    /* test 4*/
    currentPos = "  2asd2: ";
    temp = currentPos;
    if(isLabelDefinition(&currentPos, currentLabel)){
        generalError = TRUE;
        printf("isLabelDefinition test 4 failed - return value\n");
    }
    else if(currentPos != temp){
        printf("isLabelDefinition test 4 failed - currentPos\n");
        generalError = TRUE;
    }


    if(!generalError){
        printf("-------------------------\n");
        printf("isLabelDefinition test good\n");
        printf("-------------------------\n");
    }

}


void testGetStringFromLine(){
    int indexTemp;
    errorCodes errorTemp;
    char string[TOKEN_ARRAY_SIZE];
    boolean generalError;

    generalError = FALSE;

    /* test 1 */
    indexTemp = 0;
    if(!getStringFromLine("\"asdfg  asd\"", &indexTemp, string, &errorTemp)){
        printf("pandas - testGetStringFromLine test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(string, "asdfg  asd")){
        printf("pandas - testGetStringFromLine test 1 failed:\n");
        printf("should be  \"asdfg asd\"  instead of %s\n", string);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 2 */
    indexTemp = 0;
    if(!getStringFromLine("\"  &^     hjagfsdy5\"", &indexTemp, string, &errorTemp)){
        printf("pandas - testGetStringFromLine test 2 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(string, "  &^     hjagfsdy5")){
        printf("pandas - testGetStringFromLine test 2 failed:\n");
        printf("should be  \"  &^     hjagfsdy5\"  instead of %s\n", string);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 3 */
    indexTemp = 0;
    if(getStringFromLine("\"asdfg  asdasd asd", &indexTemp, string, &errorTemp)){
        printf("pandas - testGetStringFromLine test 3 failed - returned TRUE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 4 */
    indexTemp = 0;
    if(getStringFromLine("asdfg  asdasd asd\"", &indexTemp, string, &errorTemp)){
        printf("pandas - testGetStringFromLine test 4 failed - returned TRUE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }


    if(!generalError){
        printf("pandas - getStringFromLine - good!\n");
        printf("--------------------------------------\n");
    }

}


void testStringToLong(){
    long value;
    char *endPtr;
    boolean generalError;

    generalError = FALSE;

    /* test 1 */
    if(!stringToLong("127", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 1 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 127){
        printf("stringToLong test 1 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 2 */
    if(!stringToLong("-128", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 2 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != -128){
        printf("stringToLong test 2 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 3 */
    if(!stringToLong("32767", &value, &endPtr, HALF_WORD_MAX_VALUE)){
        printf("stringToLong test 3 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 32767){
        printf("stringToLong test 3 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 4 */
    if(!stringToLong("-32768", &value, &endPtr, HALF_WORD_MAX_VALUE)){
        printf("stringToLong test 4 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != -32768){
        printf("stringToLong test 4 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 5 */
    if(!stringToLong("+2147483647", &value, &endPtr, WORD_MAX_VALUE)){
        printf("stringToLong test 5 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 2147483647){
        printf("stringToLong test 5 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 6 - modified second condition*/
    if(!stringToLong("-2147483648", &value, &endPtr, WORD_MAX_VALUE)){
        printf("stringToLong test 6 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 2147483648ul){
        printf("stringToLong test 6 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 7 */
    if(stringToLong("128", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 7 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 8 */
    if(stringToLong("-129", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 8 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 9 */
    if(stringToLong("32768", &value, &endPtr, HALF_WORD_MAX_VALUE)){
        printf("stringToLong test 9 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 10 */
    if(stringToLong("-32769", &value, &endPtr, HALF_WORD_MAX_VALUE)){
        printf("stringToLong test 10 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 11 */
    if(stringToLong("2147483648", &value, &endPtr, WORD_MAX_VALUE)){
        printf("stringToLong test 11 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 12 */
    if(stringToLong("-2147483649", &value, &endPtr, WORD_MAX_VALUE)){
        printf("stringToLong test 12 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 13 */
    if(stringToLong("922337203685477580765", &value, &endPtr, WORD_MAX_VALUE)){
        printf("stringToLong test 13 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 14 */
    if(!stringToLong("123asd", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 14 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 123){
        printf("stringToLong test 14 failed - value issue\n");
        generalError = TRUE;
    }
    else if(*endPtr != 'a'){
        printf("stringToLong test 14 failed - endPtr issue\n");
        generalError = TRUE;
    }

    /* test 15 */
    if(stringToLong("asd", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 15 failed - range issue\n");
        generalError = TRUE;
    }
    else if(*endPtr != 'a'){
        printf("stringToLong test 15 failed - endPtr issue\n");
        generalError = TRUE;
    }

    /* test 16 */
    if(stringToLong("", &value, &endPtr, BYTE_MAX_VALUE)){
        printf("stringToLong test 16 failed - range issue\n");
        generalError = TRUE;
    }
    else if(*endPtr != '\0'){
        printf("stringToLong test 16 failed - endPtr issue\n");
        generalError = TRUE;
    }

    /* test 17 */
    if(!stringToLong("33554432", &value, &endPtr, ADDRESS_MAX_VALUE)){
        printf("stringToLong test 17 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != 33554432){
        printf("stringToLong test 17 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 18 */
    if(!stringToLong("-33554433", &value, &endPtr, ADDRESS_MAX_VALUE)){
        printf("stringToLong test 18 failed - range issue\n");
        generalError = TRUE;
    }
    else if(value != -33554433){
        printf("stringToLong test 18 failed - value issue\n");
        generalError = TRUE;
    }

    /* test 19 */
    if(stringToLong("33554433", &value, &endPtr, ADDRESS_MAX_VALUE)){
        printf("stringToLong test 19 failed - range issue\n");
        generalError = TRUE;
    }

    /* test 20 */
    if(stringToLong("-33554434", &value, &endPtr, ADDRESS_MAX_VALUE)){
        printf("stringToLong test 20 failed - range issue\n");
        generalError = TRUE;
    }

    if(!generalError){
        printf("pandas - stringToLong - good!\n");
    }

    printf("---------------------------------------------------------------------\n");


}

void testGetNumbersFromLine(){
    int indexTemp;
    errorCodes errorTemp;
    long numbers[NUMBERS_ARRAY_SIZE];
    boolean generalError;

    generalError = FALSE;

    /* todo noa test values in numbers and update error prints to match case */

    /* test 1 */
    indexTemp =0;
    if (getNumbersFromLine("12, 34, 56", &indexTemp, numbers, DW, &errorTemp) != 3) {
        printf("pandas - testGetNumberFromLine test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 2*/
    indexTemp = 0;
    if ((getNumbersFromLine("12    ,    34,  56  ", &indexTemp, numbers, DW, &errorTemp)) != 3){
        printf("pandas - testGetNumberFromLine test 2 failed - returned FALSE\n" )
/*printf("pandas - testGetNumberFromLine test 2 failed - returned FALSE\n")*/;
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
/* test 3 */
    indexTemp = 0;
    if (getNumbersFromLine("   12     34,  56 ", &indexTemp, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 4 */
    indexTemp = 0;
    if (getNumbersFromLine("   12$34,  56", &indexTemp, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 4 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /* test 5 */
    indexTemp = 0;
    if (getNumbersFromLine("   12.54    34,  56 ", &indexTemp, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 5 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /* test 6 */
    indexTemp = 0;
    if (getNumbersFromLine("  132543,212,2221,12 ", &indexTemp, numbers, DW, &errorTemp)!=4){
        printf("pandas - testGetNumberFromLine test 6 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /* test 7 */
    indexTemp = 0;
    if (!(getNumbersFromLine("   12  ,   34,  56, 7        ", &indexTemp, numbers, DH, &errorTemp))){
        printf("pandas - testGetNumberFromLine test 7 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    if(!generalError){
        printf("pandas - getNumbersFromLine - good!\n");
        printf("--------------------------------------\n");
    }
}

void testIdRegister()
{
    boolean generalError;
    int tempReg;
    errorCodes errorTemp;

    generalError = FALSE;

    /* todo noa check also value in tempReg after each call and update error prints to match case */

    /* test 1 */
    if (!(idRegister("$12", &tempReg, &errorTemp)))
    {
        printf("pandas - testIsRegister test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }


    /*test 2*/
    if (idRegister("$ 12", &tempReg, &errorTemp) )
    {
        printf("pandas - testIsRegister test 2 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 3*/
    if (idRegister("$12.5", &tempReg, &errorTemp))
    {
        printf("pandas - testIsRegister test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 4*/
    if (idRegister("12", &tempReg, &errorTemp))
    {
        printf("pandas - testIsRegister test 4 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 5*/
    if (idRegister("$125", &tempReg, &errorTemp))
    {
        printf("pandas - testIsRegister test 5 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 6*/
    if (idRegister("$12#", &tempReg, &errorTemp))
    {
        printf("pandas - testIsRegister test 6 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    if (!(idRegister("$0", &tempReg, &errorTemp)))
    {
        printf("pandas - testIsRegister test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    if(!generalError){
        printf("pandas - isRegister - good!\n");
        printf("--------------------------------------\n");
    }

}

void testExtractOperands() {
    boolean generalError;
    int tmpPtr;
    errorCodes errorTemp;
    boolean jumpIsReg = FALSE;
    int IC = 100;
    int tmpReg1;
    int tmpReg2;
    int tmpReg3;
    long tmpImmed;

    generalError = FALSE;

    /*test 1*/
    tmpPtr = 0;
    if (!(extractOperands("$10, $20, $30", &tmpPtr, R_ARITHMETIC, IC, &jumpIsReg,
                          &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL))) {
        printf("pandas - testExtractOperands test 1 failed - returned VALUE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 2*/
    tmpPtr = 0;
    extractOperands("$10, $20, $30", &tmpPtr, R_ARITHMETIC, IC, &jumpIsReg,
                    &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL);

    if (tmpReg1 != 10 || tmpReg2 != 20 || tmpReg3 != 30) {
        printf("pandas - testExtractOperands test 2 failed - extracted values\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 3*/
    tmpPtr = 0;
    extractOperands("$10   , $20", &tmpPtr, R_COPY, IC, &jumpIsReg,
                    &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL);

    if (tmpReg1 != 10 || tmpReg2 != 20) {
        printf("pandas - testExtractOperands test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 4*/
    tmpPtr = 0;
    if (extractOperands("$10", &tmpPtr, R_COPY, IC, &jumpIsReg,
                        &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL)) {
        printf("pandas - testExtractOperands test 4 failed - returned FALSE\n");
        printf("%d", errorTemp);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 5*/
    tmpPtr = 0;
    tmpReg1 = -1;
    tmpReg2 = -1;
    tmpReg3 = -1;
    extractOperands("$10, -32, $30", &tmpPtr, I_ARITHMETIC, IC, &jumpIsReg,
                    &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL);

    if (tmpReg1 != 10 || tmpReg2 != 30 || tmpImmed != -32) {
        printf("pandas - testExtractOperands test 5 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 6*/
    tmpPtr = 0;
    extractOperands("$10, -21, $22", &tmpPtr, I_MEMORY_LOAD, IC, &jumpIsReg,
                    &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL);
    if (tmpReg1 != 10 || tmpReg2 != 22 || tmpImmed != -21) {
        printf("pandas - testExtractOperands test 6 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 7*/
    tmpPtr = 0;
    extractOperands("$10, -21 $22", &tmpPtr, I_MEMORY_LOAD, IC, &jumpIsReg,
                    &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL);
    if (errorTemp != MISSING_COMMA) {
        printf("pandas - testExtractOperands test 7 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 8*/
    tmpPtr = 0;
    if (extractOperands("$10 -21 $22", &tmpPtr, I_MEMORY_LOAD, IC, &jumpIsReg,
                        &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL))
    {
        printf("pandas - testExtractOperands test 8 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 9*/
    tmpPtr = 0;
    if (extractOperands("$10# $22", &tmpPtr, I_MEMORY_LOAD, IC, &jumpIsReg,
                        &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL))

    {
        printf("pandas - testExtractOperands test 9 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 10*/
    tmpPtr = 0;
    if (extractOperands("$10, -2, $22", &tmpPtr, R_ARITHMETIC, IC, &jumpIsReg,
                        &tmpReg1, &tmpReg2, &tmpReg3, &tmpImmed, &errorTemp, NULL))

    {
        printf("pandas - testExtractOperands test 10 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    if (!generalError) {
        printf("pandas - extractOperands - good!\n");
        printf("--------------------------------------\n");
    }
}



/* ----------------------------------------------------------------------------------------------- */

/* labelsDB tests */
/* todo remake tests */
void testAddNewLabel(void *head) {

    printf("You didnt write the test yet you idiot!\n");
    printf("-----------------------------------------------------------------------------------------------");

    clearLabels(head);
}


/* ----------------------------------------------------------------------------------------------- */


/* dataImageDB tests */

void testAddNumber(void *head) {
    int DC;

    DC = 0;

    addNumber(head, &DC, 10, DB);

    addNumber(head, &DC, 150, DH);

    addNumber(head, &DC, -3500, DW);

    addNumber(head, &DC, 500, DH);

    printNumbers(DC, head);

}


void testAddString(void *head) {
    int DC = 0;
    addString(head, &DC, "asd");
    addString(head, &DC, "  wS$  #a");
    addString(head, &DC, "asd");
    addString(head, &DC, "");
    printData(19, head);
}


/* ----------------------------------------------------------------------------------------------- */


/* codeImageDB tests */


void testAddingCommands(void *head){
    int IC = STARTING_ADDRESS;

    /* test 1 */
    if(!addRCommand(&head, &IC, 1, 2, 3, 0, 3)){
        printf("addingCommands test 1 - not good\n");
    }
    else{
        if(IC != 104){
            printf("addingCommands test 1 - IC value wrong\n");
        }
    }

    /* test 2 */
    if(!addRCommand(&head, &IC, 10, 30, 0, 1, 2)){
        printf("addingCommands test 2 - not good\n");
    }
    else{
        if(IC != 108){
            printf("addingCommands test 2 - IC value wrong\n");
        }
    }

    /* test 3 */
    if(!addRCommand(&head, &IC, 10, 5, 4, 0, 5)){
        printf("addingCommands test 3 - not good\n");
    }
    else{
        if(IC != 112){
            printf("addingCommands test 3 - IC value wrong\n");
        }
    }

}

