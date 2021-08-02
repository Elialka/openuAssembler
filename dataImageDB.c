#include <stdlib.h>
#include "dataImageDB.h"
#include "string.h"

#include <stdio.h>/* temp */

#define BITS_IN_BYTE (8)


/* todo remove global variable */
char *head;/* start of database */

static boolean addByte(unsigned char byte, int DC);

/*
 * initialize database - allocate first block
 * */
void * initDataImageDB(){
    void *database = calloc(IMAGE_BLOCK_SIZE, sizeof(char));
    if(!database){
        /* todo print error quit */
    }
    head = database;
    return database;
}


static boolean addByte(unsigned char byte, int DC){
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
boolean addNumber(int *DCPtr, long value, int numOfBytes){
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
        if(!addByte((formattedNumber & (mask << offset)) >> offset, *DCPtr)){
            return FALSE;
        }
    }

    return TRUE;
}

boolean addNumberArray(int *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType){
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
        if(!addNumber(DCPtr, array[i], numOfBytes)){
            return FALSE;
        }
    }

    return TRUE;
}


/*
 * this function adds a string to database
 */
boolean addString(int *DCPtr, char *str){
    char *current;
    char *prev;

    /* add each character to database */
    for(current = str, prev = current; *prev; current++, (*DCPtr)++){
        prev = current;
        if(!addByte(*prev, *DCPtr)){
            return FALSE;
        }
    }

    return TRUE;
}












/* delete - test */
void printData(int DC){
    int i;
    char c;

    for(i = 0; i < DC; i++){
        c = (unsigned char)head[i];
        printf("%c | ", c ? c : '/');
    }
    printf("\n-----------------------------------\n");
}


void testAddByte(){
    unsigned char byte;

    byte = 3;
    addByte(byte, 0);
    byte = ~byte + 1;
    addByte(byte, 1);
    byte = 100;
    addByte(byte, 2);
    byte = ~byte + 1;
    addByte(byte, 3);
    printData(4);
}

