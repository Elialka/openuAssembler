#include "global.h"
#include <stdlib.h>
#include <string.h>

#include "operationsDB.h"

/* number of different operations available */
#define NUM_OF_OPERATIONS (27)
/* number of character in the longest operation name, including terminating '\0' */
#define OPERATION_NAME_LENGTH (5)


typedef struct operationsDB{
    char name[OPERATION_NAME_LENGTH];
    opcodes opcode;
    functValues funct;
    operationClass commandOpType;
}operation;


operationsDBPtr setOperations(){
    int i;
    operationsDBPtr head;
    char *names[] = {"add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
                    "addi", "subi", "andi", "ori", "nori", "bne", "beq",
                    "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh",
                    "jmp", "la", "call", "stop"};
    opcodes opcodes[] = {ADD, SUB, AND, OR , NOR, MOVE, MVHI, MVLO, ADDI, SUBI, ANDI, ORI, NORI,
                         BNE, BEQ, BLT, BGT, LB, SB, LW, SW, LH, SH, JMP, LA, CALL, STOP};
    functValues functs[] = {ADD_FUNCT, SUB_FUNCT, AND_FUNCT, OR_FUNCT, NOR_FUNCT, MOVE_FUNCT, MVHI_FUNCT, MVLO_FUNCT};
    operationsDBPtr current;

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
            current->commandOpType = J_JMP;
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
 * return its position in the database (non-zero) if found
 * return zero if not found
 */
int seekOp(operationsDBPtr head, char *str){
    int i;
    operationsDBPtr current = head;
    for (i = 0; i <= NUM_OF_OPERATIONS; i++, current++){
        if (!strcmp(str, current->name))
        {
            return i;
        }
    }

    return NOT_FOUND;
}

/*
 * find an operation with a name matching str
 * return opcode if found, zero otherwise
 */
boolean getOpcode(operationsDBPtr head, char *str, opcodes *opCodePtr, functValues *functPtr, operationClass *opTypePtr){
    int operationIndex;
    operationsDBPtr current;

    current = head;

    if((operationIndex = seekOp(head, str)) != NOT_FOUND)
    {
        /* go to correct operation labelCallNode */
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


boolean firstOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr){
    boolean needMoreOperands = TRUE;

    /* set relevant pointer to correct attribute's address */
    if(commandOpType == J_STOP){
        needMoreOperands = FALSE;
    }
    else if(commandOpType <= R_COPY){/* is R type */
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->rAttributes.rs;
    }
    else if(commandOpType <= I_MEMORY_LOAD){/* is I type */
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->iAttributes.rs;
    }
    else{/* is J type - not stop */
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->jAttributes.address;
    }

    return needMoreOperands;
}


boolean secondOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                            operandAttributes *currentOperandPtr){
    boolean needMoreOperands = TRUE;

    if(commandOpType >= J_JMP){/* is J type */
        needMoreOperands = FALSE;
    }
    else if(commandOpType == R_ARITHMETIC){
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->rAttributes.rt;
    }
    else if(commandOpType == R_COPY){
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->rAttributes.rd;
    }
    else if(commandOpType == I_ARITHMETIC || commandOpType == I_MEMORY_LOAD){
        currentOperandPtr->valuePointer.immedPtr = &codeLineDataPtr->iAttributes.immed;
    }
    else if(commandOpType == I_BRANCHING){
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->iAttributes.rt;
    }

    return needMoreOperands;
}

boolean thirdOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr){
    boolean needMoreOperands = TRUE;

    if(commandOpType >= J_JMP || commandOpType == R_COPY){/* is J type or R_COPY */
        needMoreOperands = FALSE;
    }
    else if(commandOpType == R_ARITHMETIC){
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->rAttributes.rd;
    }
    else if(commandOpType == I_ARITHMETIC || commandOpType == I_MEMORY_LOAD){
        currentOperandPtr->valuePointer.regPtr = &codeLineDataPtr->iAttributes.rt;
    }

    return needMoreOperands;
}


void clearOperationDB(operationsDBPtr head){
    if(head){
        free(head);
    }
}
