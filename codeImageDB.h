

/**
 * Initialize code image database
 * @return pointer to the database
 */
codeImageDBPtr initCodeImage();

/**
 * Add an R type command to the database, update IC counter
 * @param headPtr pointer to the database
 * @param ICPtr pointer to IC counter
 * @param commandData structure containing command and operand data
 * @return
 */
errorCodes addRCommand(codeImageDBPtr *headPtr, long *ICPtr, rTypeData commandData);

/**
 * Add an I type command to the database, update IC counter
 * @param headPtr pointer to the database
 * @param ICPtr pointer to IC counter
 * @param commandData structure containing command and operand data
 * @return
 */
errorCodes addICommand(codeImageDBPtr *headPtr, long *ICPtr, iTypeData commandData);

/**
 * Add an J type command to the database, update IC counter
 * @param headPtr pointer to the database
 * @param ICPtr pointer to IC counter
 * @param commandData structure containing command and operand data
 * @return
 */
errorCodes addJCommand(codeImageDBPtr *headPtr, long *ICPtr, jTypeData commandData);

/**
 * Update immed field of I type commands that use a label as an argument
 * @param headPtr pointer to the database
 * @param IC value of IC counter
 * @param labelAddress address of the label used as argument
 * @return
 */
errorCodes updateITypeImmed(codeImageDBPtr headPtr, long IC, long labelAddress);

/**
 * Update immed field of J type commands that use a label as an argument
 * @param headPtr pointer to the database
 * @param IC value of IC counter
 * @param labelAddress address of the label used as argument
 */
void updateJTypeAddress(codeImageDBPtr headPtr, long IC, long labelAddress);

/**
 * Get the serial byte in the code image, as specified by index
 * @param headPtr pointer to the database
 * @param index byte index in code image
 * @return the byte as unsigned char
 */
unsigned char getEncodedCodeByte(void *headPtr, long index);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearCodeImageDB(codeImageDBPtr head);
