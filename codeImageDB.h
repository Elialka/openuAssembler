#ifndef MAX_LINE
#include "data.h"
#endif


codeImagePtr initCodeImage();

boolean addRCommand(codeImagePtr *headPtr, long *ICPtr, int reg1, int reg2, int reg3,
                    commandOps opcode, functValues funct);

boolean addICommand(codeImagePtr *headPtr, long *ICPtr, int reg1, int reg2, int immed, commandOps opcode);

boolean addJCommand(codeImagePtr *headPtr, long *ICPtr, boolean isReg, long address, commandOps opcode);

boolean updateITypeImmed(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

boolean updateJTypeAddress(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

unsigned char getNextCodeByte(codeImagePtr headPtr, long index);

void clearCodeImageDB(codeImagePtr headPtr);
