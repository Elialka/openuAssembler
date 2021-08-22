
#include "printErrors.h"

/**
 * Creat relevant output files
 * @param databases pointer to databases structure
 * @param sourceFilename name of source file
 * @param ICF size of code image
 * @param DCF size of data image
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 */
void writeFiles(databaseRouter databases, char *sourceFilename, long ICF, long DCF, fileErrorStatus *fileStatusPtr);


