
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


/**
 * Initialize operations database
 * @return pointer to database
 */
operationsDBPtr setOperations();

/**
 * Search the database for a operations command with identical name
 * @param head pointer to database
 * @param operationName name of the operation
 * @return index of the operation in the database, or NOT_FOUND macro value
 */
int seekOp(operationsDBPtr head, char *operationName);

/**
 * Seek for data instruction with identical name
 * @param instructionName name of the instruction
 * @param dataOpTypePtr address where to store data instruction enum value
 * @return TRUE if found, FALSE otherwise
 */
boolean seekDataOp(char *instructionName, dataOps *dataOpTypePtr);

/**
 * Get opcode and funct(if relevant), store in given addresses and return whether found
 * @param head pointer to database
 * @param operationName name of the operation
 * @param opCodePtr address where to store opcode enum value
 * @param functPtr address where to store funct enum value, if relevant
 * @param opTypePtr address where to store operation class enum value
 * @return TRUE if found, FALSE otherwise
 */
boolean getOpcode(operationsDBPtr head, char *operationName, opcodes *opCodePtr, functValues *functPtr, operationClass *opTypePtr);

/**
 * Set pointers for first operand values base on operation class
 * @param commandOpType enum value representing operation class
 * @param codeLineDataPtr pointer to union struct where operand data should be stored
 * @param currentOperandPtr pointer to struct where pointers should be set by this function
 * @return TRUE if first operand is needed, FALSE otherwise
 */
boolean firstOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr);

/**
 * Set pointers for second operand values base on operation class
 * @param commandOpType enum value representing operation class
 * @param codeLineDataPtr pointer to union struct where operand data should be stored
 * @param currentOperandPtr pointer to struct where pointers should be set by this function
 * @return TRUE if second operand is needed, FALSE otherwise
 */
boolean secondOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                            operandAttributes *currentOperandPtr);

/**
 * Set pointers for third operand values base on operation class
 * @param commandOpType enum value representing operation class
 * @param codeLineDataPtr pointer to union struct where operand data should be stored
 * @param currentOperandPtr pointer to struct where pointers should be set by this function
 * @return TRUE if third operand is needed, FALSE otherwise
 */
boolean thirdOperandFormat(operationClass commandOpType, codeLineData *codeLineDataPtr,
                           operandAttributes *currentOperandPtr);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearOperationDB(operationsDBPtr head);
