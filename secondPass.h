
/**
 * Run second pass on the file - as specified in readme
 * @param databasesPtr pointer to databases struct
 * @param ICF size of code image
 * @param fileStatusPtr pointer to structure containing data about file error status
 * @return TRUE if no errors occurred, false otherwise
 */
boolean secondPass(databaseRouterPtr databasesPtr, long ICF, fileErrorStatus *fileStatusPtr);
