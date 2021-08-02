#ifndef MAX_LINE
#include "data.h"
#endif


void *initCodeImage();

boolean addRCommand(void **headPtr, int *ICPTR, int reg1, int reg2, int reg3,
                    commandOps opcode, functValues funct);

boolean addICommand(void **headPtr, int *ICPtr, int reg1, int reg2, int immed, commandOps opcode);

boolean addJCommand(void **headPtr, int *ICPtr, boolean isReg, long address, commandOps opcode);
