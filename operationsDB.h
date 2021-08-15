operationPtr setOperations();

int seekOp(operationPtr head, char *str);

boolean seekDataOp(char *str, dataOps *dataOpTypePtr);

boolean getOpcode(operationPtr head, char *str, commandOps *opCodePtr, functValues *functPtr, operationClass *opTypePtr);

void clearOperationDB(operationPtr head);
