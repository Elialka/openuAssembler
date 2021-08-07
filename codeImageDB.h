#ifndef MAX_LINE
#include "data.h"
#endif


void *initCodeImage();

boolean addRCommand(void *headPtr, long *ICPtr, int reg1, int reg2, int reg3,
                    commandOps opcode, functValues funct);

boolean addICommand(void *headPtr, long *ICPtr, int reg1, int reg2, int immed, commandOps opcode);

boolean addJCommand(void *headPtr, long *ICPtr, boolean isReg, long address, commandOps opcode);

boolean updateITypeImmed(void *head, long IC, long address, errorCodes *lineErrorPtr);

boolean updateJTypeAddress(void *head, long IC, long address, errorCodes *lineErrorPtr);

void clearCodeImageDB(void *head);
