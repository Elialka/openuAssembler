
/**
 * Print warning message to the user
 * @param encounteredWarning warning code enum representing encountered warning
 * @param lineIdPtr pointer to struct containing data regarding line of input
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 */
void printWarningMessage(warningCodes encounteredWarning, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr);

/**
 * Print file-specific error message to the user
 * @param encounteredError error code enum representing encountered error
 * @param lineIdPtr pointer to struct containing data regarding line of input
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 */
void printFileErrorMessage(errorCodes encounteredError, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr);

/**
 * Print project-wide error message to the user
 * @param errorCode error code enum representing encountered error
 */
void printProjectErrorMessage(projectErrors errorCode);

/**
 * If any errors were printed for current file, print spacer
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 */
void divideFileErrorPrinting(fileErrorStatus *fileStatusPtr);
