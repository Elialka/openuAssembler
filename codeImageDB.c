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
        unsigned int immed:16;
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

static boolean addCommandToDatabase(codeLine **headPtr, int *ICPtr, codeLine current){
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
    (*headPtr)[nextFreeIndex].bytes.first = current.bytes.first;
    (*headPtr)[nextFreeIndex].bytes.second = current.bytes.second;
    (*headPtr)[nextFreeIndex].bytes.third = current.bytes.third;
    (*headPtr)[nextFreeIndex].bytes.fourth = current.bytes.fourth;

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

boolean addRCommand(void **headPtr, int *ICPtr, int reg1, int reg2, int reg3,
                    commandOps opcode, functValues funct){
    codeLine current;

    current.R.opcode = opcode;
    current.R.rs = reg1;
    current.R.rt = reg2;
    current.R.rd = reg3;
    current.R.funct = funct;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, current);
}


boolean addICommand(void **headPtr, int *ICPtr, int reg1, int reg2, int immed, commandOps opcode){
    codeLine current;

    current.I.opcode = opcode;
    current.I.rs = reg1;
    current.I.rt = reg2;
    current.I.immed = immed;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, current);
}

boolean addJCommand(void **headPtr, int *ICPtr, boolean isReg, long address, commandOps opcode){
    codeLine current;

    current.J.opcode = opcode;
    current.J.isReg = isReg;
    current.J.address1 = address & FIRST_16_BITS_MASK;
    current.J.address2 = address >> 16;

    return addCommandToDatabase((codeLine **)headPtr, ICPtr, current);
}

void printCode(void *head, int IC){
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
}
