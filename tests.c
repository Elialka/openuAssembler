#include <stdio.h>
#include <string.h>
#include "tests.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "labelsDB.h"
#include "codeImageDB.h"
#include "firstPass.h"

/* run all tests */

void testFunctions(databaseRouterPtr databasesPtr){
    /* pandas */
    testIsLabelDefinition();
    testGetStringFromLine();
    testStringToLong();
    testGetNumbersFromLine();
    testIdRegister();
    testExtractOperands(databasesPtr->labelCallsDB);

    /* labelsDB */
    testAddNewLabel(databasesPtr->labelsDB);

    /* dataImageDB tests */
    testAddNumber(&databasesPtr->dataImageDB);
    testAddByte(&databasesPtr->dataImageDB);
    testAddString(&databasesPtr->dataImageDB);
}

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
    if (isLabelDefinition(&currentPos, currentLabel, NULL))
    {
        printf("isLabelDefinition test 1 failed - return value\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "hELLO") != 0){
        printf("isLabelDefinition test 1 failed - currentLabel string\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 1 failed - currentPos\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }

    /* test 2*/
    currentPos = "asdf: ";
    if (isLabelDefinition(&currentPos, currentLabel, NULL))
    {
        printf("isLabelDefinition test 2 failed - return value\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "asdf") != 0){
        printf("isLabelDefinition test 2 failed - currentLabel string\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 2 failed - currentPos\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }


    /* test 3*/
    currentPos = "asdd2: ";
    if (isLabelDefinition(&currentPos, currentLabel, NULL))
    {
        printf("isLabelDefinition test 3 failed - return value\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(currentLabel, "asdd2") != 0){
        printf("isLabelDefinition test 3 failed - currentLabel string\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }
    else if(*(currentPos-1) != ':'){
        printf("isLabelDefinition test 3 failed - currentPos\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }

    /* test 4*/
    currentPos = "  2asd2: ";
    temp = currentPos;
    if(!isLabelDefinition(&currentPos, currentLabel, NULL)){
        generalError = TRUE;
        printf("isLabelDefinition test 4 failed - return value\n");
        printf("-------------------------\n");
    }
    else if(currentPos != temp){
        printf("isLabelDefinition test 4 failed - currentPos\n");
        printf("-------------------------\n");
        generalError = TRUE;
    }


    if(!generalError){
        printf("isLabelDefinition test good\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");

}


void testGetStringFromLine(){
    char line[LINE_ARRAY_SIZE];
    char *currentPos = line;
    char string[TOKEN_ARRAY_SIZE];
    boolean generalError;

    generalError = FALSE;

    /* test 1 */
    strcpy(line, "\"asdfg  asd\"");
    if(getStringFromLine(&currentPos, string)){
        printf("pandas - testGetStringFromLine test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(string, "asdfg  asd") != 0){
        printf("pandas - testGetStringFromLine test 1 failed:\n");
        printf("should be  \"asdfg asd\"  instead of %s\n", string);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 2 */
    currentPos = line;
    strcpy(line, "\"  &^     hjagfsdy5\"");
    if(getStringFromLine(&currentPos, string)){
        printf("pandas - testGetStringFromLine test 2 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    else if(strcmp(string, "  &^     hjagfsdy5") != 0){
        printf("pandas - testGetStringFromLine test 2 failed:\n");
        printf("should be  \"  &^     hjagfsdy5\"  instead of %s\n", string);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 3 */
    strcpy(line, "\"asdfg  asdasd asd");
    if(!getStringFromLine(&currentPos, string)){
        printf("pandas - testGetStringFromLine test 3 failed - returned TRUE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 4 */
    strcpy(line, "asdfg  asdasd asd\"");
    if(!getStringFromLine(&currentPos, string)){
        printf("pandas - testGetStringFromLine test 4 failed - returned TRUE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }


    if(!generalError){
        printf("pandas - getStringFromLine - good!\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");

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

    printf("-----------------------------------------------------------------------------------------------\n");


}

void testGetNumbersFromLine(){
    errorCodes errorTemp;
    long numbers[NUMBERS_ARRAY_SIZE];
    char line[LINE_ARRAY_SIZE];
    char *currentPos = line;
    boolean generalError;

    generalError = FALSE;

    /* todo noa test values in numbers and update error prints to match case */

    /* test 1 */
    strcpy(line, "12, 34, 56");
    if (getNumbersFromLine(&currentPos, numbers, DW, &errorTemp) != 3) {
        printf("pandas - testGetNumberFromLine test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 2*/
    strcpy(line, "12    ,    34,  56  ");
    if ((getNumbersFromLine(&currentPos, numbers, DW, &errorTemp)) != 3){
        printf("pandas - testGetNumberFromLine test 2 failed - returned FALSE\n" )
        /*printf("pandas - testGetNumberFromLine test 2 failed - returned FALSE\n")*/;
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 3 */
    strcpy(line, "   12     34,  56 ");
    if (getNumbersFromLine(&currentPos, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 4 */
    strcpy(line, "   12$34,  56");
    if (getNumbersFromLine(&currentPos, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 4 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 5 */
    strcpy(line, "   12.54    34,  56 ");
    if (getNumbersFromLine(&currentPos, numbers, DW, &errorTemp)){
        printf("pandas - testGetNumberFromLine test 5 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 6 */
    strcpy(line, "  132543,212,2221,12 ");
    if (getNumbersFromLine(&currentPos, numbers, DW, &errorTemp) != 4){
        printf("pandas - testGetNumberFromLine test 6 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /* test 7 */
    strcpy(line, "   12  ,   34,  56, 7        ");
    if (!(getNumbersFromLine(&currentPos, numbers, DH, &errorTemp))){
        printf("pandas - testGetNumberFromLine test 7 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    if(!generalError){
        printf("pandas - getNumbersFromLine - good!\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");
}

void testIdRegister()
{
    boolean generalError;
    unsigned char tempReg;

    generalError = FALSE;

    /* todo noa check also value in tempReg after each call and update error prints to match case */

    /* test 1 */
    if (getRegisterOperand("$12", &tempReg))
    {
        printf("pandas - testIsRegister test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }


    /*test 2*/
    if (!getRegisterOperand("$", &tempReg))
    {
        printf("pandas - testIsRegister test 2 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 3*/
    if (!getRegisterOperand("$12.5", &tempReg))
    {
        printf("pandas - testIsRegister test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 4*/
    if (!getRegisterOperand("12", &tempReg))
    {
        printf("pandas - testIsRegister test 4 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 5*/
    if (!getRegisterOperand("$125", &tempReg))
    {
        printf("pandas - testIsRegister test 5 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 6*/
    if (!getRegisterOperand("$12#", &tempReg))
    {
        printf("pandas - testIsRegister test 6 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    if ((getRegisterOperand("$0", &tempReg)))
    {
        printf("pandas - testIsRegister test 3 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    if(!generalError){
        printf("pandas - isRegister - good!\n");
    }

    printf("-----------------------------------------------------------------------------------------------\n");

}

void testExtractOperands(void *head) {
    boolean generalError;
    errorCodes errorTemp;
    long IC = 100;
    char line[LINE_ARRAY_SIZE];
    char *currentPos = line;
    codeLineData currentLineData;
    generalError = FALSE;

    /*test 1*/
    strcpy(line, "$10, $20, $30");
    if (extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head)) {
        printf("pandas - testExtractOperands test 1 failed - returned FALSE\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }


    /*test 2*/
    currentPos = line;
    strcpy(line, "$10, $20, $30");
    extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head);

    if (currentLineData.rAttributes.rs != 10 || currentLineData.rAttributes.rt != 20 ||
        currentLineData.rAttributes.rd != 30) {
        printf("pandas - testExtractOperands test 2 failed - saved incorrect values\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 3*/
    currentPos = line;
    strcpy(line, "$10   , $20");
    extractCodeOperandsTest(&currentPos, R_COPY, &currentLineData, &IC, head);

    if (currentLineData.rAttributes.rs != 10 || currentLineData.rAttributes.rd != 20) {
        printf("pandas - testExtractOperands test 3 failed - saved incorrect values\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 4*/
    currentPos = line;
    strcpy(line, "$10");
    if (!(errorTemp = extractCodeOperandsTest(&currentPos, R_COPY, &currentLineData, &IC, head))){
        printf("pandas - testExtractOperands test 4 failed - expected register\n");
        printf("%d", errorTemp);
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 5*/
    currentPos = line;
    strcpy(line, "$10, -32, $30");
    extractCodeOperandsTest(&currentPos, I_ARITHMETIC, &currentLineData, &IC, head);

    if (currentLineData.iAttributes.rs != 10 || currentLineData.iAttributes.rt != 30 ||
    currentLineData.iAttributes.immed != -32) {
        printf("pandas - testExtractOperands test 5 failed - saved incorrect values \n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 6*/
    currentPos = line;
    strcpy(line, "$10, -21, $22");
    extractCodeOperandsTest(&currentPos, I_MEMORY_LOAD, &currentLineData, &IC, head);
    if (currentLineData.iAttributes.rs != 10 || currentLineData.iAttributes.rt != 22 ||
    currentLineData.iAttributes.immed != -21) {
        printf("pandas - testExtractOperands test 6 failed - saved incorrect values\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 7*/
    currentPos = line;
    strcpy(line, "$10, -21 $22");
    extractCodeOperandsTest(&currentPos, I_MEMORY_LOAD, &currentLineData, &IC, head);
    if (errorTemp != MISSING_COMMA) {
        printf("pandas - testExtractOperands test 7 failed - expected another error code\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 8*/
    currentPos = line;
    strcpy(line, "$10 -21 $22");
    if(!extractCodeOperandsTest(&currentPos, I_MEMORY_LOAD, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 8 failed - missing comma\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 9*/
    currentPos = line;
    strcpy(line, "$10# $22");
    if (!extractCodeOperandsTest(&currentPos, I_MEMORY_LOAD, &currentLineData, &IC, head))

    {
        printf("pandas - testExtractOperands test 9 failed - not a register - illegal sign\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 10*/
    currentPos = line;
    strcpy(line, "$10, -2, $22");
    if (!extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head))

    {
        printf("pandas - testExtractOperands test 10 failed - expected register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 11*/
    currentPos = line;
    strcpy(line, "$10, $3,, $22");
    if (!extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head))

    {
        printf("pandas - testExtractOperands test 11 failed - should not run\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 12*/
    currentPos = line;
    strcpy(line, "$10, $2,");
    errorTemp = extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head);
    if (errorTemp != MISSING_PARAMETER)
    {
        printf("pandas - testExtractOperands test 12 failed - missing parameter\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 13*/
    currentPos = line;
    strcpy(line, "$10, $30, $2");
    errorTemp = extractCodeOperandsTest(&currentPos, I_ARITHMETIC, &currentLineData, &IC, head);
    if (errorTemp != EXPECTED_NUMBER_SECOND)
    {
        printf("pandas - testExtractOperands test 13 failed - missing register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 14*/
    currentPos = line;
    strcpy(line, "10, $15, $20");
    if ((errorTemp = extractCodeOperandsTest(&currentPos, R_ARITHMETIC, &currentLineData, &IC, head))) {
        if (errorTemp != EXPECTED_REGISTER_FIRST) {
            printf("pandas - testExtractOperands test 14 failed - missing register\n");
            printf("--------------------------------------\n");
            generalError = TRUE;
        }
    }
    /*test 15*/
    currentPos = line;
    strcpy(line, "New, $22");
    if (extractCodeOperandsTest(&currentPos, J_JMP, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 15 failed - labelsDB should be enough\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 16*/
    currentPos = line;
    strcpy(line, "New, $22");
    if (extractCodeOperandsTest(&currentPos, J_JMP, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 16 failed - missing register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 17*/
    currentPos = line;
    strcpy(line, "New, $22");
    if (!extractCodeOperandsTest(&currentPos, R_COPY, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 17 failed - expected register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 18*/
    currentPos = line;
    strcpy(line, "ajhasdijbhisdnflhdfusdvnsfnhsbefsesndjcjhvfhskb, $22");
    errorTemp = extractCodeOperandsTest(&currentPos, R_COPY, &currentLineData, &IC, head);
    if (errorTemp != EXPECTED_REGISTER_FIRST)
    {
        printf("pandas - testExtractOperands test 18 failed - expected register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 19*/
    currentPos = line;
    strcpy(line, "ajhflqnelnadvhsfvlskmfjsdhfndnjsnchvjdejdnw32ncndjnd, $22");
    if(!extractCodeOperandsTest(&currentPos, J_JMP, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 19 failed - labelsDB is too long\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 20*/
    currentPos = line;
    strcpy(line, "$22       ,     22");
    if (!extractCodeOperandsTest(&currentPos, R_COPY, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 20 failed - expected register\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 21*/
    currentPos = line;
    strcpy(line, "$21, -321,        $0");
    extractCodeOperandsTest(&currentPos, I_MEMORY_LOAD, &currentLineData, &IC, head);
    if (currentLineData.iAttributes.rs != 21 || currentLineData.iAttributes.rt != 0 ||
    currentLineData.iAttributes.immed != -321 )
    {
        printf("pandas - testExtractOperands test 21 failed - saved incorrect values \n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }
    /*test 22*/
    currentPos = line;
    strcpy(line, "New,");
    if (extractCodeOperandsTest(&currentPos, J_CALL_OR_LA, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 22 failed - expected register error\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    /*test 23*/
    currentPos = line;
    strcpy(line, "$12, $22, $30");
    if (!extractCodeOperandsTest(&currentPos, I_BRANCHING, &currentLineData, &IC, head))
    {
        printf("pandas - testExtractOperands test 23 failed - missing labelsDB\n");
        printf("--------------------------------------\n");
        generalError = TRUE;
    }

    if (!generalError) {
        printf("pandas - extractCodeOperandsTest - good!\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");
}



/* ----------------------------------------------------------------------------------------------- */

/* labelsDB tests */
/* todo remake tests */
void testAddNewLabel(void *head) {

    printf("You didnt write the test yet you idiot!\n");
    printf("-----------------------------------------------------------------------------------------------\n");
}


/* ----------------------------------------------------------------------------------------------- */


/* dataImageDB tests */

void testAddNumber(void *head) {
    long DC;
    unsigned char buffer[] = {10, 150, 0, 84, 242, 255, 255, 244, 1};

    DC = 0;

    addNumber(head, &DC, 10, SIZE_OF_BYTE);

    addNumber(head, &DC, 150, SIZE_OF_HALF_WORD);

    addNumber(head, &DC, -3500, SIZE_OF_WORD);

    addNumber(head, &DC, 500, SIZE_OF_HALF_WORD);

    if(checkDataImage(DC, head, buffer)){
        printf("dataImageDB - addNumber - good!\n");
    }
    else{
        printf("!!TEST FAILED!!! dataImageDB - addNumber\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");
}


void testAddString(void *head) {
    long DC = 0;
    unsigned char buffer[] = {'a', 's', 'd', '\0', ' ', ' ', 'w', 'S', '$', ' ', ' ', '#', 'a', '\0',
                              'a', 's', 'd', '\0', '\0'};
    addString(head, &DC, "asd");
    addString(head, &DC, "  wS$  #a");
    addString(head, &DC, "asd");
    addString(head, &DC, "");

    if(checkDataImage(DC, head, buffer)){
        printf("dataImageDB - addString - good!\n");
    }
    else{
        printf("!!TEST FAILED!!! dataImageDB - addString\n");
    }
    printf("-----------------------------------------------------------------------------------------------\n");
}


/* ----------------------------------------------------------------------------------------------- */


