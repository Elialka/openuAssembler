
/**
 * Run first pass on the file - as specified in readme
 * @param sourceFile file pointer to current file
 * @param ICFPtr address where to store size of code image
 * @param DCFPtr address where to store size of data image
 * @param databasesPtr pointer to databases struct
 * @param fileStatusPtr pointer to structure containing data about file error status
 * @return TRUE if no errors occurred, false otherwise
 */
boolean
firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr, fileErrorStatus *fileStatusPtr);
