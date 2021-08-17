#ifndef MAX_LINE
#include "data.h"
#endif


codeImagePtr initCodeImage();

errorCodes addRCommand(codeImagePtr *headPtr, long *ICPtr, rTypeData commandData);

errorCodes addICommand(codeImagePtr *headPtr, long *ICPtr, iTypeData commandData);

errorCodes addJCommand(codeImagePtr *headPtr, long *ICPtr, jTypeData commandData);

boolean updateITypeImmed(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

boolean updateJTypeAddress(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

unsigned char getNextCodeByte(codeImagePtr headPtr, long index);

void clearCodeImageDB(codeImagePtr headPtr);
