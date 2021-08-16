operationPtr setOperations();

int seekOp(operationPtr head, char *str);

boolean seekDataOp(char *str, dataOps *dataOpTypePtr);

boolean getOpcode(operationPtr head, char *str, commandOps *opCodePtr, functValues *functPtr, operationClass *opTypePtr);

boolean firstOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                           operandAttributes *currentOperandPtr);

boolean secondOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                            operandAttributes *currentOperandPtr);

boolean thirdOperandFormat(operationClass commandOpType, codeLineData *currentLineDataPtr,
                           operandAttributes *currentOperandPtr);

void clearOperationDB(operationPtr head);
