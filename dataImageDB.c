#include <stdlib.h>
#include "dataImageDB.h"
#include "string.h"

#define BITS_IN_BYTE (8)


static boolean addByte(dataImagePtr *headPtr, unsigned char byte, long DC);


/*
 * initialize database - allocate first block
 * */
dataImagePtr initDataImageDB(){
    dataImagePtr database = calloc(IMAGE_BLOCK_SIZE, sizeof(char));
    if(!database){
        /* todo print error quit */
    }
    return database;
}


static boolean addByte(dataImagePtr *headPtr, unsigned char byte, long DC){
    boolean result = TRUE;
    void *temp;

    if(DC && !(DC & IMAGE_BLOCK_SIZE)){/* out of allocated memory */
        temp = realloc(*headPtr, DC + IMAGE_BLOCK_SIZE);
        if(!temp){
            result =  FALSE;
            /* todo free allocated memory - quit program */
        }
        *headPtr = temp;
    }

    /* add byte to database */
    if(result){
        memcpy(*headPtr + DC, &byte, sizeof(char));
    }
    return result;
}


/*
 * this function adds number to database
 */
/* todo add error pointer, check overall + negative numbers */
/* todo maybe make static */
boolean addNumber(dataImagePtr *headPtr, long *DCPtr, long value, int numOfBytes){
    int i, offset;
    unsigned long mask;
    unsigned long formattedNumber;

    mask = 0xFF;

    /* implement two's complement */
    if(value < 0){
        value *= -1;
        formattedNumber = ~value + 1;
    }
    else{
        formattedNumber = value;
    }

    for(i = 0; i < numOfBytes; i++, (*DCPtr)++){
        offset = i * BITS_IN_BYTE;
        if(!addByte(headPtr, (formattedNumber & (mask << offset)) >> offset, *DCPtr)){
            return FALSE;
        }
    }

    return TRUE;
}

errorCodes addNumberArray(dataImagePtr *headPtr, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType) {
    int i;
    int numOfBytes;
    errorCodes encounteredError = NO_ERROR;

    /* determine how many bytes to span each value */
    switch(dataOpType){
        case DB:
            numOfBytes = SIZE_OF_BYTE;
            break;
        case DH:
            numOfBytes = SIZE_OF_HALF_WORD;
            break;
        case DW:
            numOfBytes = SIZE_OF_WORD;
            break;
        default:
            /*  */
            break;
    }

    for(i = 0; !encounteredError &&  i < amountOfNumbers; i++){
        if(!addNumber(headPtr, DCPtr, array[i], numOfBytes)){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


/*
 * this function adds a string to database
 */
errorCodes addString(dataImagePtr *headPtr, long *DCPtr, char *str) {
    char *current;
    char *prev;
    errorCodes encounteredError = NO_ERROR;

    /* add each character to database */
    for(current = str, prev = current; !encounteredError && *prev; current++, (*DCPtr)++){
        prev = current;
        if(!addByte(headPtr, *prev, *DCPtr)){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


unsigned char getNextDataByte(dataImagePtr headPtr, long index){

    return (headPtr)[index];
}



void clearDataImageDB(dataImagePtr headPtr){
    free(headPtr);
}












/* delete - test */


boolean checkDataImage(long DC, unsigned char *head, unsigned char *buffer) {
    int i;

    for(i = 0; i < DC; i++){
        if(buffer[i] != head[i]){
            return FALSE;
        }
    }
    return TRUE;
}


void testAddByte(void *head) {
    unsigned char byte;
    long DC = 0;
    unsigned char buffer[] = {3, 253, 100, 156};

    byte = 3;
    addByte(head, byte, DC);
    DC++;

    byte = ~byte + 1;
    addByte(head, byte, DC);
    DC++;

    byte = 100;
    addByte(head, byte, DC);
    DC++;

    byte = ~byte + 1;
    addByte(head, byte, DC);
    DC++;

    checkDataImage(4, head, buffer);
}

