#include <stdlib.h>

#include "global.h"
#include "codeImageDB.h"

#define FIRST_16_BITS_MASK (0xFFFF)
#define BITS_ALREADY_READ (16)

typedef union codeImageDB{
    struct{
        unsigned int :6;/* bits not used */
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
        unsigned int address1:16;/* first 16 bytes of 25-bit address */
        unsigned int address2:9;/* last 9 bits of 25-bit address */
        unsigned int isReg:1;
        unsigned int opcode:6;
    }J;
    struct{
        unsigned char first;
        unsigned char second;
        unsigned char third;
        unsigned char fourth;
    }bytes;
}encodedOperation;

/**
 * Add encoded command to database
 * @param headPtr address of pointer to the database
 * @param ICPtr pointer to IC counter
 * @param newPtr pointer to encoded code line to be added
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes addCommandToDatabase(codeImageDBPtr *headPtr, long *ICPtr, encodedOperation *newPtr);


codeImageDBPtr initCodeImage(){
    void *head = calloc(IMAGE_BLOCK_SIZE, sizeof(encodedOperation));

    return head;
}


static errorCodes addCommandToDatabase(codeImageDBPtr *headPtr, long *ICPtr, encodedOperation *newPtr){
    errorCodes encounteredError = NO_ERROR;
    long nextFreeIndex = (*ICPtr - STARTING_ADDRESS) / (long)sizeof(encodedOperation); /* how many code lines already saved */
    void *temp = NULL;
    encodedOperation *currentPtr = NULL;/* pointer to location in the database, where new command will be added */

    if(*ICPtr != STARTING_ADDRESS && !(nextFreeIndex % IMAGE_BLOCK_SIZE)){/* out of allocated memory */
        temp = realloc(*headPtr, sizeof(encodedOperation) * (nextFreeIndex + IMAGE_BLOCK_SIZE));
        if(!temp){
            encounteredError = MEMORY_ALLOCATION_FAILURE;
        }
        *headPtr = temp;
    }

    if(!encounteredError){
        currentPtr = *headPtr + nextFreeIndex;

        /* add command to database */
        currentPtr->bytes.first = newPtr->bytes.first;
        currentPtr->bytes.second = newPtr->bytes.second;
        currentPtr->bytes.third = newPtr->bytes.third;
        currentPtr->bytes.fourth = newPtr->bytes.fourth;

        *ICPtr += sizeof(encodedOperation);
    }
    return encounteredError;
}


errorCodes addRCommand(codeImageDBPtr *headPtr, long *ICPtr, rTypeData commandData){
    encodedOperation new = {0};

    new.R.opcode = commandData.opcode;
    new.R.rs = commandData.rs;
    new.R.rt = commandData.rt;
    new.R.rd = commandData.rd;
    new.R.funct = commandData.funct;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addICommand(codeImageDBPtr *headPtr, long *ICPtr, iTypeData commandData){
    encodedOperation new = {0};

    new.I.opcode = commandData.opcode;
    new.I.rs = commandData.rs;
    new.I.rt = commandData.rt;
    new.I.immed = (int)commandData.immed;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes addJCommand(codeImageDBPtr *headPtr, long *ICPtr, jTypeData commandData){
    encodedOperation new = {0};

    new.J.opcode = commandData.opcode;
    new.J.isReg = commandData.isReg;
    new.J.address1 = commandData.address & FIRST_16_BITS_MASK;
    new.J.address2 = commandData.address >> 16;

    return addCommandToDatabase(headPtr, ICPtr, &new);
}


errorCodes updateITypeImmed(codeImageDBPtr headPtr, long IC, long labelAddress){
    errorCodes encounteredError = NO_ERROR;
    long distance = labelAddress - IC;/* immed attribute value */
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(encodedOperation));
    encodedOperation *operationPtr = headPtr + index;/* pointer to updated operation encoding */


    /* verify calculated value is in range */
    if(distance > I_TYPE_IMMED_MAX_VALUE_SIGNED || distance < I_TYPE_IMMED_MIN_VALUE){
        encounteredError = ADDRESS_DISTANCE_OVER_LIMITS;
    }
    else{/* supported distance */
        operationPtr->I.immed = distance;
    }

    return encounteredError;
}


void updateJTypeAddress(codeImageDBPtr headPtr, long IC, long labelAddress){
    /* calculate index of code line with given IC */
    long index = (long)((IC - STARTING_ADDRESS) / sizeof(encodedOperation));
    encodedOperation *operationPtr = headPtr + index;/* pointer to updated operation encoding */

    /* update addresses */
    operationPtr->J.address1 = labelAddress & FIRST_16_BITS_MASK;
    operationPtr->J.address2 = labelAddress >> BITS_ALREADY_READ;
}


unsigned char getEncodedCodeByte(void *headPtr, long index){

    return ((char *)headPtr)[index];
}


void clearCodeImageDB(codeImageDBPtr head){
    if(head){
        free(head);
    }
}
