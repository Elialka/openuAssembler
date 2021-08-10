#include <stdlib.h>
#include "dataImageDB.h"
#include "string.h"

#include <stdio.h>/* temp */

#define BITS_IN_BYTE (8)



static boolean addByte(char *head, unsigned char byte, long DC);


/*
 * initialize database - allocate first block
 * */
void * initDataImageDB(){
    void *database = calloc(IMAGE_BLOCK_SIZE, sizeof(char));
    if(!database){
        /* todo print error quit */
    }
    return database;
}

/* todo change head to char ** - head is not updated when reallocating memory */
static boolean addByte(char *head, unsigned char byte, long DC) {
    static int size = IMAGE_BLOCK_SIZE;/* current size of database */
    void *temp;

    /* check if enough memory */
    if(DC >= size){
        temp = realloc(head, size + IMAGE_BLOCK_SIZE);
        if(!temp){
            return FALSE;
        }
        head = temp;
    }

    memcpy(head + DC, &byte, sizeof(char));
    return TRUE;
}


/*
 * this function adds number to database
 */
/* todo add error pointer, check overall + negative numbers */
boolean addNumber(void *head, long *DCPtr, long value, int numOfBytes) {
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
        if(!addByte(head, (formattedNumber & (mask << offset)) >> offset, *DCPtr)){
            return FALSE;
        }
    }

    return TRUE;
}

boolean addNumberArray(void *head, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType) {
    int i;
    int numOfBytes;

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

    for(i = 0; i < amountOfNumbers; i++){
        if(!addNumber(head, DCPtr, array[i], numOfBytes)){
            return FALSE;
        }
    }

    return TRUE;
}


/*
 * this function adds a string to database
 */
boolean addString(void *head, long *DCPtr, char *str) {
    char *current;
    char *prev;

    /* add each character to database */
    for(current = str, prev = current; *prev; current++, (*DCPtr)++){
        prev = current;
        if(!addByte(head, *prev, *DCPtr)){
            return FALSE;
        }
    }

    return TRUE;
}


unsigned char getNextDataByte(void *head, long index){

    return ((char *)head)[index];
}



void clearDataImageDB(void *head){
    free(head);
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

