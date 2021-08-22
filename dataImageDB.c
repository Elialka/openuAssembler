#include <stdlib.h>
#include "dataImageDB.h"
#include "string.h"

#define BITS_IN_BYTE (8)
#define FIRST_BYTE_MASK (0xFF)
#define SIZE_OF_BYTE (1)
#define SIZE_OF_HALF_WORD (2)
#define SIZE_OF_WORD (4)


typedef struct dataImageDB{
    unsigned char byte;
}dataImageDB;


/**
 * Add a byte to the database
 * @param headPtr pointer to database
 * @param byte the byte to be added
 * @param DC value of DC counter
 * @return TRUE if added successfully, FALSE otherwise
 */
static boolean addByte(dataImageDBPtr *headPtr, unsigned char byte, long DC);

/**
 * Add a number to the database, update DC counter
 * @param headPtr pointer to database
 * @param DCPtr pointer to DC counter
 * @param value value of the number
 * @param numOfBytes how many bytes should the number span across
 * @return TRUE if added successfully, FALSE otherwise
 */
static boolean addNumber(dataImageDBPtr *headPtr, long *DCPtr, long value, int numOfBytes);


dataImageDBPtr initDataImageDB(){
    dataImageDBPtr database = calloc(IMAGE_BLOCK_SIZE, sizeof(dataImageDB));

    return database;
}


static boolean addByte(dataImageDBPtr *headPtr, unsigned char byte, long DC){
    boolean result = TRUE;
    void *temp;

    if(DC && !(DC % IMAGE_BLOCK_SIZE)){/* out of allocated memory */
        temp = realloc(*headPtr, DC + IMAGE_BLOCK_SIZE);
        if(!temp){
            result =  FALSE;
        }
        *headPtr = temp;
    }

    /* add byte to database */
    if(result){
        memcpy(&((*headPtr + DC)->byte), &byte, sizeof(char));
    }

    return result;
}


static boolean addNumber(dataImageDBPtr *headPtr, long *DCPtr, long value, int numOfBytes){
    boolean result = TRUE;
    int i, offset;
    unsigned long mask;
    unsigned long formattedNumber;

    mask = FIRST_BYTE_MASK;

    /* implement two's complement */
    if(value < 0){
        value *= -1;
        formattedNumber = ~value + 1;
    }
    else{
        formattedNumber = value;
    }

    /* add the number byte by byte using masks */
    for(i = 0; result && i < numOfBytes; i++, (*DCPtr)++){
        offset = i * BITS_IN_BYTE;
        if(!addByte(headPtr, (formattedNumber & (mask << offset)) >> offset, *DCPtr)){
            result = FALSE;
        }
    }

    return result;
}


errorCodes addNumberArray(dataImageDBPtr *headPtr, long *DCPtr, long *array, int amountOfNumbers, dataOps dataOpType){
    int i;
    int numOfBytes;/* how many bytes should the number span */
    errorCodes encounteredError = NO_ERROR;

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
            encounteredError = IMPOSSIBLE;
            break;
    }

    /* add each number in the array */
    for(i = 0; !encounteredError &&  i < amountOfNumbers; i++){
        if(!addNumber(headPtr, DCPtr, array[i], numOfBytes)){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


errorCodes addString(dataImageDBPtr *headPtr, long *DCPtr, char *str){
    char *next;/* pointer to next character in string */
    char *current;/* pointer to current character being added */
    errorCodes encounteredError = NO_ERROR;

    /* add each character to database, including '\0' */
    for(next = str, current = next; !encounteredError && *current; next++, (*DCPtr)++){
        current = next;
        if(!addByte(headPtr, *current, *DCPtr)){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
    }

    return encounteredError;
}


unsigned char getNextDataByte(void *headPtr, long index){

    return (((dataImageDBPtr)headPtr) + index)->byte;
}


void clearDataImageDB(dataImageDBPtr head){
    if(head){
        free(head);
    }
}

