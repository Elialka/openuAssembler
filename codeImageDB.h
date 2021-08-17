#ifndef MAX_LINE
#include "data.h"
#endif


codeImagePtr initCodeImage();

boolean addRCommand(codeImagePtr *headPtr, long *ICPtr, rTypeData commandData);

boolean addICommand(codeImagePtr *headPtr, long *ICPtr, iTypeData commandData);

boolean addJCommand(codeImagePtr *headPtr, long *ICPtr, jTypeData commandData);

boolean updateITypeImmed(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

boolean updateJTypeAddress(codeImagePtr headPtr, long IC, long address, errorCodes *lineErrorPtr);

unsigned char getNextCodeByte(codeImagePtr headPtr, long index);

void clearCodeImageDB(codeImagePtr headPtr);
