#include <stdlib.h>

#include<stdio.h>/* for tests - delete */

#include "codeImageDB.h"

#define FIRST_16_BITS_MASK (0xFFFF)

typedef union{
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


/* todo change function call to use address of head in first pass - head may not be updated when reallocating memory */
static boolean addCommandToDatabase(codeLine **headPtr, long *ICPtr, codeLine *current){
    static int currentSize = IMAGE_BLOCK_SIZE;/* size of allocated memory, counted by codeLine type units */
    int nextFreeIndex;
    void *temp;

    /* calculate next unused spot in database */
    nextFreeIndex = (*ICPtr - STARTING_ADDRESS) / (int)sizeof(codeLine);

    if(nextFreeIndex >= currentSize){/* out of allocated memory */
        temp = realloc(*headPtr, currentSize + IMAGE_BLOCK_SIZE);
        if(!temp){/* memory allocation failed */
            return FALSE;
        }
        *headPtr = temp;
        currentSize += IMAGE_BLOCK_SIZE;
    }

    /* copy to database */
    (*headPtr)[nextFreeIndex].bytes.first = current->bytes.first;
    (*headPtr)[nextFreeIndex].bytes.second = current->bytes.second;
    (*headPtr)[nextFreeIndex].bytes.third = current->bytes.third;
    (*headPtr)[nextFreeIndex].bytes.fourth = current->bytes.fourth;

    /* update code image counter */
    *ICPtr += sizeof(codeLine);

    return TRUE;
}


void *initCodeImage(){
    void *head;

    head = calloc(IMAGE_BLOCK_SIZE, sizeof(codeLine));
    if(!head){/* memory allocation failed */
        /* todo print error quit */
    }
    return head;
}

boolean addRCommand(void *headPtr, long *ICPtr, int reg1, int reg2, int reg3,
                    commandOps opcode, functValues funct){
    codeLine new;

    new.R.opcode = opcode;
    new.R.rs = reg1;
    new.R.rt = reg2;
    new.R.rd = reg3;
    new.R.funct = funct;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, &new);
}


boolean addICommand(void *headPtr, long *ICPtr, int reg1, int reg2, int immed, commandOps opcode){
    codeLine new;

    new.I.opcode = opcode;
    new.I.rs = reg1;
    new.I.rt = reg2;
    new.I.immed = immed;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, &new);
}


boolean addJCommand(void *headPtr, long *ICPtr, boolean isReg, long address, commandOps opcode){
    codeLine new;

    new.J.opcode = opcode;
    new.J.isReg = isReg;
    new.J.address1 = address & FIRST_16_BITS_MASK;
    new.J.address2 = address >> 16;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, &new);
}

/* todo maybe handle IC out of range */
boolean updateITypeImmed(void *head, long IC, long address, errorCodes *lineErrorPtr) {
    boolean result = TRUE;/* return value - if operation was successful */
    codeLine *current = head;
    long distance = address - IC;
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(codeLine));

    /* append current to relevant code line */
    current += index;

    /* verify calculated value is in range */
    if(distance > I_TYPE_IMMED_MAX_VALUE || distance < I_TYPE_IMMED_MIN_VALUE){
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
boolean updateJTypeAddress(void *head, long IC, long address, errorCodes *lineErrorPtr) {
    codeLine *current = head;
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


void clearCodeImageDB(void *head){
    free(head);
}
