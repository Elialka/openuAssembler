#include <stdlib.h>

#include<stdio.h>/* for tests - delete */

#include "codeImageDB.h"

#define FIRST_16_BITS_MASK (0xFFFF)

/* todo refactor addition function parameters */

typedef union codeLine{
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
}codeLine;

static errorCodes addCommandToDatabase(codeImagePtr *headPtr, long *ICPtr, codeLine *current);

static void resetCodeLine(codeLine *newPtr);


codeImagePtr initCodeImage(){
    void *head = calloc(IMAGE_BLOCK_SIZE, sizeof(codeLine));

    return head;
}


static void resetCodeLine(codeLine *newPtr){
    newPtr->bytes.first = 0;
    newPtr->bytes.second = 0;
    newPtr->bytes.third = 0;
    newPtr->bytes.fourth = 0;
}


static errorCodes addCommandToDatabase(codeImagePtr *headPtr, long *ICPtr, codeLine *current){
    errorCodes encounteredError = NO_ERROR;
    long nextFreeIndex = (*ICPtr - STARTING_ADDRESS) / (long)sizeof(codeLine); /* how many code lines already saved */
    void *temp;
    codeImagePtr currentPtr;

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

        *ICPtr += sizeof(codeLine);
    }
    return encounteredError;
}


errorCodes addRCommand(codeImagePtr *headPtr, long *ICPtr, rTypeData commandData) {
    codeLine new;

    resetCodeLine(&new);

    new.R.opcode = commandData.opcode;
    new.R.rs = commandData.rs;
    new.R.rt = commandData.rt;
    new.R.rd = commandData.rd;
    new.R.funct = commandData.funct;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addICommand(codeImagePtr *headPtr, long *ICPtr, iTypeData commandData) {
    codeLine new;

    resetCodeLine(&new);

    new.I.opcode = commandData.opcode;
    new.I.rs = commandData.rs;
    new.I.rt = commandData.rt;
    new.I.immed = (int)commandData.immed;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addJCommand(codeImagePtr *headPtr, long *ICPtr, jTypeData commandData) {
    codeLine new;

    resetCodeLine(&new);

    new.J.opcode = commandData.opcode;
    new.J.isReg = commandData.isReg;
    new.J.address1 = commandData.address & FIRST_16_BITS_MASK;
    new.J.address2 = commandData.address >> 16;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}

/* todo maybe handle IC out of range */
/* todo refactor to return errorCodes type */
boolean updateITypeImmed(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr) {
    boolean result = TRUE;/* return value - if operation was successful */
    codeLine *current = headPtr;
    long distance = address - IC;
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(codeLine));

    /* append current to relevant code line */
    current += index;

    /* verify calculated value is in range */
    if(distance > I_TYPE_IMMED_MAX_VALUE_SIGNED || distance < I_TYPE_IMMED_MIN_VALUE){
        *lineErrorPtr = ADDRESS_DISTANCE_OVER_LIMITS;
        result = FALSE;
    }
    else{
        /* update value */
        current->I.immed = distance;
    }

    return result;
}


/* todo maybe handle IC out of range, maybe literal number 16 */
/* todo refactor to return errorCodes type */
boolean updateJTypeAddress(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr) {
    codeLine *current = headPtr;
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(codeLine));

    /* append current to relevant code line */
    current += index;

    /* update addresses */
    current->J.address1 = address & FIRST_16_BITS_MASK;
    current->J.address2 = address >> 16;

    *lineErrorPtr = NO_ERROR;/* temp - delete */
    return TRUE;
}


unsigned char getNextCodeByte(codeImagePtr headPtr, long index) {

    return ((char *)headPtr)[index];
}


void printCode(void *head, long IC){
    int i, j;
    codeLine *current;

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


void clearCodeImageDB(codeImagePtr head){
    if(head){
        free(head);
    }
}
