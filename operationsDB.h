operationsDBPtr setOperations();

int seekOp(operationsDBPtr head, char *str);

boolean seekDataOp(char *str, dataOps *dataOpTypePtr);

boolean getOpcode(operationsDBPtr head, char *str, opcodes *opCodePtr, functValues *functPtr, operationClass *opTypePtr);

boolean firstOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                           operandAttributes *currentOperandPtr);

boolean secondOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                            operandAttributes *currentOperandPtr);

boolean thirdOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                           operandAttributes *currentOperandPtr);

void clearOperationDB(operationsDBPtr head);
