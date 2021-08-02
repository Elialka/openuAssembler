#include "data.h"
#include <stdlib.h>
#include <string.h>
#ifndef MAX_LINE
#include "data.h"
#endif

#include "operationsDB.h"

/* number of different operations available */
#define NUM_OF_OPERATIONS (27)
/* number of character in longest operation name, including terminating '\0' */
#define OPERATION_NAME_LENGTH (5)


typedef struct{
    char name[OPERATION_NAME_LENGTH];
    commandOps opcode;
    functValues funct;
    operationClass commandOpType;
}operation;


typedef operation *operationPtr;


/*
 * This function is used to set a database, containing operation names, types and OpCodes
 * Return value is TRUE if memory allocation succeeded, FALSE otherwise
 */
void *setOperations(){
    int i;
    operationPtr head;
    char *names[] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
                    "addi", "subi", "andi", "ori", "nori", "bne", "beq",
                    "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh",
                    "jmp", "la", "call", "stop"};
    commandOps opcodes[] = {ADD, SUB,AND, OR ,NOR, MOVE, MVHI, MVLO, ADDI, SUBI, ANDI, ORI, NORI,
                    BNE, BEQ, BLT, BGT, LB, SB, LW, SW, LH, SH, JMP, LA, CALL, STOP};
    functValues functs[] = {ADD_FUNCT, SUB_FUNCT, AND_FUNCT, OR_FUNCT, NOR_FUNCT, MOVE_FUNCT, MVHI_FUNCT, MVLO_FUNCT};
    operationPtr current;

    head = calloc(NUM_OF_OPERATIONS, sizeof(operation));
    if(!head){/* memory allocation failed */
        return head;
    }
    current = head;

    /* manually set the values */
    for(i = 0; i < NUM_OF_OPERATIONS; i++, current++){
        strcpy(current->name, names[i]);
        current->opcode = opcodes[i];
        if(current->opcode <= MVLO){/* is R type */
            if(current->opcode <= NOR){
                current->commandOpType = R_ARITHMETIC;
            }
            else{
                current->commandOpType = R_COPY;
            }
            current->funct = functs[i];
        }
        else if(current->opcode <= NORI){
            current->commandOpType = I_ARITHMETIC;
        }
        else if(current->opcode <= BGT){
            current->commandOpType = I_BRANCHING;
        }
        else if(current->opcode <= SH){
            current->commandOpType = I_MEMORY_LOAD;
        }
        else if(current->opcode == JMP){
            current->commandOpType = J_JUMP;
        }
        else if(current->opcode <= CALL){
            current->commandOpType = J_CALL_OR_LA;
        }
        else{/* must be stop command */
            current->commandOpType = J_STOP;
        }

    }
    return head;
}

/*
 * look for an operation with a name matching to str
 * return it's position in the database (non zero) if found
 * return zero if not found
 */
int seekOp(void *head, char *str) {
    int i;
    operationPtr current = head;
    for (i = 0; i <= NUM_OF_OPERATIONS; i++, current++){
        if (!strcmp(str, current->name))
        {
            return i;
        }
    }

    return FALSE;
}

/*
 * find an operation with a name matching str
 * return opcode if found, zero otherwise
 */
boolean getOpcode(void *head, char *str, commandOps *opCodePtr, functValues *functPtr, operationClass *opTypePtr) {
    int operationIndex;
    operationPtr current;

    current = head;

    if((operationIndex = seekOp(head, str)))
    {
        /* go to correct operation node */
        current += operationIndex;
        *opCodePtr = current->opcode;
        *functPtr = current->funct;
        *opTypePtr = current->commandOpType;
    }
    else{/* operation not found */
        return FALSE;
    }

    return TRUE;
}

/*
 * look for a matching data operation name
 * insert dataOps enum value to *dataOpTypePtr if found match
 * return TRUE if found match, FALSE otherwise
 */
boolean seekDataOp(char *str, dataOps *dataOpTypePtr){
    if(!strcmp(str, ".dw")){
        *dataOpTypePtr = DW;
    }
    else if(!strcmp(str, ".dh")){
        *dataOpTypePtr = DH;
    }
    else if(!strcmp(str, ".db")){
        *dataOpTypePtr = DB;
    }
    else if(!strcmp(str, ".asciz")){
        *dataOpTypePtr = ASCIZ;
    }
    else if(!strcmp(str, ".entry")){
        *dataOpTypePtr = ENTRY;
    }
    else if(!strcmp(str, ".extern")){
        *dataOpTypePtr = EXTERN;
    }
    else{
        return FALSE;
    }

    return TRUE;
}
