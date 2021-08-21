#ifndef MAX_LINE
#include "data.h"
#endif


codeImageDBPtr initCodeImage();

errorCodes addRCommand(codeImageDBPtr *headPtr, long *ICPtr, rTypeData commandData);

errorCodes addICommand(codeImageDBPtr *headPtr, long *ICPtr, iTypeData commandData);

errorCodes addJCommand(codeImageDBPtr *headPtr, long *ICPtr, jTypeData commandData);

errorCodes updateITypeImmed(codeImageDBPtr headPtr, long IC, long address);

void updateJTypeAddress(codeImageDBPtr headPtr, long IC, long address);

unsigned char getNextCodeByte(codeImageDBPtr headPtr, long index);

void clearCodeImageDB(codeImageDBPtr head);
