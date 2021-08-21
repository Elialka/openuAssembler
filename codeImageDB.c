#include <stdlib.h>

#include<stdio.h>/* for tests - delete */

#include "codeImageDB.h"

#define FIRST_16_BITS_MASK (0xFFFF)

/* todo refactor addition function parameters */

typedef union codeImageDB{
    struct{
        unsigned int :6;
        unsigned int funct:5;
        unsigned int rd:5;
        unsigned int rt:5;
        unsigned int rs:5;
        unsigned int opcode:6;
    }R;
    struct{
        signed int immed:16;
        unsigned int rt:5;
        unsigned int rs:5;
        unsigned int opcode:6;
    }I;
    struct{
        unsigned int address1:16;
        unsigned int address2:9;
        unsigned int isReg:1;
        unsigned int opcode:6;
    }J;
    struct{
        unsigned char first;
        unsigned char second;
        unsigned char third;
        unsigned char fourth;
    }bytes;
}encodedCommand;

static errorCodes addCommandToDatabase(codeImageDBPtr *headPtr, long *ICPtr, encodedCommand *current);

static void resetCodeLine(encodedCommand *newPtr);


codeImageDBPtr initCodeImage(){
    void *head = calloc(IMAGE_BLOCK_SIZE, sizeof(encodedCommand));

    return head;
}


static void resetCodeLine(encodedCommand *newPtr){
    newPtr->bytes.first = 0;
    newPtr->bytes.second = 0;
    newPtr->bytes.third = 0;
    newPtr->bytes.fourth = 0;
}


static errorCodes addCommandToDatabase(codeImageDBPtr *headPtr, long *ICPtr, encodedCommand *current){
    errorCodes encounteredError = NO_ERROR;
    long nextFreeIndex = (*ICPtr - STARTING_ADDRESS) / (long)sizeof(encodedCommand); /* how many code lines already saved */
    void *temp;
    codeImageDBPtr currentPtr;

    if(*ICPtr != STARTING_ADDRESS && !(nextFreeIndex % IMAGE_BLOCK_SIZE)){/* out of allocated memory */
        temp = realloc(*headPtr, nextFreeIndex + IMAGE_BLOCK_SIZE);
        if(!temp){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
        *headPtr = temp;
    }

    if(!encounteredError){
        currentPtr = *headPtr + nextFreeIndex;

        /* add command to database */
        currentPtr->bytes.first = current->bytes.first;
        currentPtr->bytes.second = current->bytes.second;
        currentPtr->bytes.third = current->bytes.third;
        currentPtr->bytes.fourth = current->bytes.fourth;

        *ICPtr += sizeof(encodedCommand);
    }
    return encounteredError;
}


errorCodes addRCommand(codeImageDBPtr *headPtr, long *ICPtr, rTypeData commandData) {
    encodedCommand new;

    resetCodeLine(&new);

    new.R.opcode = commandData.opcode;
    new.R.rs = commandData.rs;
    new.R.rt = commandData.rt;
    new.R.rd = commandData.rd;
    new.R.funct = commandData.funct;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addICommand(codeImageDBPtr *headPtr, long *ICPtr, iTypeData commandData) {
    encodedCommand new;

    resetCodeLine(&new);

    new.I.opcode = commandData.opcode;
    new.I.rs = commandData.rs;
    new.I.rt = commandData.rt;
    new.I.immed = (int)commandData.immed;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addJCommand(codeImageDBPtr *headPtr, long *ICPtr, jTypeData commandData) {
    encodedCommand new;

    resetCodeLine(&new);

    new.J.opcode = commandData.opcode;
    new.J.isReg = commandData.isReg;
    new.J.address1 = commandData.address & FIRST_16_BITS_MASK;
    new.J.address2 = commandData.address >> 16;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}

/* todo maybe handle IC out of range */
errorCodes updateITypeImmed(codeImageDBPtr headPtr, long IC, long address) {
    errorCodes encounteredError = NO_ERROR;
    encodedCommand *current = headPtr;
    long distance = address - IC;
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(encodedCommand));

    /* append current to relevant code line */
    current += index;

    /* verify calculated value is in range */
    if(distance > I_TYPE_IMMED_MAX_VALUE_SIGNED || distance < I_TYPE_IMMED_MIN_VALUE){
        encounteredError = ADDRESS_DISTANCE_OVER_LIMITS;
    }
    else{
        /* update value */
        current->I.immed = distance;
    }

    return encounteredError;
}


/* todo maybe handle IC out of range, maybe literal number 16 */
void updateJTypeAddress(codeImageDBPtr headPtr, long IC, long address) {
    encodedCommand *current = headPtr;
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(encodedCommand));

    /* append current to relevant code line */
    current += index;

    /* update addresses */
    current->J.address1 = address & FIRST_16_BITS_MASK;
    current->J.address2 = address >> 16;


}


unsigned char getNextCodeByte(codeImageDBPtr headPtr, long index) {

    return ((char *)headPtr)[index];
}


void printCode(void *head, long IC){
    int i, j;
    encodedCommand *current;

    current = head;

    for(i = 0; i < (IC - 100) / 4; i++, current++) {
        for (j = 0; j < 8; j++) {
            printf("%d", ((current->bytes.fourth) & (1 << (7 - j))) >> (7 - j));
        }
        putchar(' ');
        for (j = 0; j < 8; j++) {
            printf("%d", ((current->bytes.third) & (1 << (7 - j))) >> (7 - j));
        }
        putchar(' ');
        for (j = 0; j < 8; j++) {
            printf("%d", ((current->bytes.second) & (1 << (7 - j))) >> (7 - j));
        }
        putchar(' ');
        for (j = 0; j < 8; j++) {
            printf("%d", ((current->bytes.first) & (1 << (7 - j))) >> (7 - j));
        }
        putchar('\n');
    }

    printf("\n");
}/* temp - delete */


void clearCodeImageDB(codeImageDBPtr head){
    if(head){
        free(head);
    }
}
