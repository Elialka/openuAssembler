
#define MAX_COMMAND_LENGTH (7)
#define COMMAND_ARRAY_SIZE (MAX_COMMAND_LENGTH + 1)
#define NOT_FOUND (-1)

typedef enum{
    ADD = 0,
    SUB = 0,
    AND = 0,
    OR = 0,
    NOR = 0,
    MOVE = 1,
    MVHI = 1,
    MVLO = 1,
    ADDI = 10,
    SUBI = 11,
    ANDI = 12,
    ORI = 13,
    NORI = 14,
    BNE = 15,
    BEQ = 16,
    BLT = 17,
    BGT = 18,
    LB = 19,
    SB = 20,
    LW = 21,
    SW = 22,
    LH = 23,
    SH = 24,
    JMP = 30,
    LA = 31,
    CALL = 32,
    STOP = 63
}opcodes;

typedef enum{
    ADD_FUNCT = 1,
    SUB_FUNCT = 2,
    AND_FUNCT = 3,
    OR_FUNCT = 4,
    NOR_FUNCT = 5,
    MOVE_FUNCT = 1,
    MVHI_FUNCT = 2,
    MVLO_FUNCT = 3
}functValues;


typedef union codeLineData{
    rTypeData rAttributes;
    iTypeData iAttributes;
    jTypeData jAttributes;
}codeLineData;


operationsDBPtr setOperations();

int seekOp(operationsDBPtr head, char *str);

boolean seekDataOp(char *str, dataOps *dataOpTypePtr);

boolean getOpcode(operationsDBPtr head, char *str, opcodes *opCodePtr, functValues *functPtr, operationClass *opTypePtr);

boolean firstOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr);

boolean secondOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                            operandAttributes *currentOperandPtr);

boolean thirdOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr);

void clearOperationDB(operationsDBPtr head);
