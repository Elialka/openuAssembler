#include <stdio.h>
#include <string.h>

#include "global.h"
#include "codeImageDB.h"
#include "dataImageDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "firstPass.h"
#include "labelCallsDB.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "printErrors.h"
#include "printFiles.h"
#include "secondPass.h"

/* todo possible refactors */
/* wrong error for .dh number out of range */
/* error printing: handle file headline + mark end of file  - error printing database */

/**
 * Initialize static databases to be used throughout the program,
 * set pointers to other databases to NULL
 * @param headPtr address of pointer to the database
 * @return TRUE if memory allocated successfully, FALSE otherwise
 */
static boolean initProjectDatabases(databaseRouterPtr databasesPtr);

/**
 * Initialize dynamic databases to be used for a file being compiled
 * @param headPtr address of pointer to the database
 * @return TRUE if memory allocated successfully, FALSE otherwise
 */
static boolean initFileDataBases(databaseRouterPtr databasesPtr, fileErrorStatus *fileStatusPtr);

/**
 * Validate name of source file matching required extension, and supported name length.
 * Print relevant error message if necessary
 * @param sourceFileName source file name
 * @return TRUE if legal name, FALSE otherwise
 */
static boolean supportedFileName(char *sourceFileName);

/**
 * If errors are found in sourcefile, print relevant error message.
 * Otherwise, compile file and generate output files
 * @param sourceFileName name of file to open
 * @param databases struct holding pointers to project databases
 */
static void compileFile(char *sourceFileName, databaseRouter databases);

/**
 * Open given file, if not successful print error
 * @param sourceFileName file to open
 * @param sourceFilePtr address to file pointer, file address will be assigned here
 * @return TRUE if file opened successfully, FALSE otherwise
 */
static boolean openFile(char *sourceFileName, FILE **sourceFilePtr, fileErrorStatus *fileStatusPtr);

/**
 * Free memory allocated for per-file databases, set pointers to databases to NULL
 * @param headPtr address of pointer to the database
 */
static void clearFileDatabases(databaseRouterPtr databasesPtr);

/**
 * Free memory allocated for per-project databases, set pointers to databases to NULL
 * @param headPtr address of pointer to the database
 */
static void clearProjectDatabases(databaseRouterPtr databasesPtr);


int main(int argc, char *argv[]){
    int i;
    databaseRouter databases;

    if(argc < 2){/* no files to compile */
        printProjectErrorMessage(NO_FILES_TO_COMPILE);
    }
    else{/* program inline parameters present */
        /* initialize operation names database */
        initProjectDatabases(&databases);

        /* compile files */
        for(i = 1; i < argc; i++){
            if(supportedFileName(argv[i])){
                compileFile(argv[i], databases);
            }
        }

        /* free remaining memory allocations */
        clearProjectDatabases(&databases);
    }

    return 0;
}

static boolean initProjectDatabases(databaseRouterPtr databasesPtr){
    boolean allocationSuccess;

    /* all of these will be generated per file compiled */
    databasesPtr->codeImageDB = NULL;
    databasesPtr->dataImageDB = NULL;
    databasesPtr->entryCallsDB = NULL;
    databasesPtr->externUsesDB = NULL;
    databasesPtr->labelCallsDB = NULL;
    databasesPtr->labelsDB = NULL;

    /* operations database is static - does not change from file to file */
    databasesPtr->operationsDB = setOperations();
    allocationSuccess = databasesPtr->operationsDB ? TRUE : FALSE;

    if(!allocationSuccess){
        printProjectErrorMessage(PROJECT_MEMORY_FAILURE);
    }

    return allocationSuccess;
}


static boolean initFileDataBases(databaseRouterPtr databasesPtr, fileErrorStatus *fileStatusPtr) {
    boolean allocationSuccess = TRUE;

    if (!(databasesPtr->codeImageDB = initCodeImage()) ||
        !(databasesPtr->dataImageDB = initDataImageDB()) ||
        !(databasesPtr->entryCallsDB = initEntryCallsDB()) ||
        !(databasesPtr->externUsesDB = initExternUsesDB()) ||
        !(databasesPtr->labelCallsDB = initLabelCallsDB()) ||
        !(databasesPtr->labelsDB = initLabelsDB())){

            allocationSuccess = FALSE;
    }

    if(!allocationSuccess){
        printFileErrorMessage(MEMORY_ALLOCATION_FAILURE, NULL, fileStatusPtr);
    }

    return allocationSuccess;
}


static boolean supportedFileName(char *sourceFileName){
    char *currentChar = sourceFileName;
    boolean result = TRUE;
    projectErrors error = NOT_OCCURRED;

    /* validate extension */
    for(; *currentChar; currentChar++)/* go to end of name */
        ;

    for(; *currentChar != '.'; currentChar--)/* locate start of extension */
        ;

    if(strcmp(currentChar, SOURCE_FILE_EXTENSION) != 0){/* illegal extension */
        error = ILLEGAL_FILE_EXTENSION;
    }

    /* validate length */
    if(!error && strlen(sourceFileName) > MAX_FILENAME_LENGTH){
        error = FILENAME_LENGTH_NOT_SUPPORTED;
    }

    if(error){
        result = FALSE;
        printProjectErrorMessage(error);
    }

    return result;
}


static void compileFile(char *sourceFileName, databaseRouter databases){
    long ICF = 0;/* will store size of code image, in bytes */
    long DCF = 0;/* will store size of data image, in bytes */
    databaseRouterPtr databasesPtr = &databases;/* pointer to databases struct */
    boolean validFile;/* track if any errors occurred */
    fileErrorStatus fileStatus = {0};/* file attribute to be used when printing errors */
    FILE *sourceFile = NULL;/* pointer to current file */

    validFile = openFile(sourceFileName, &sourceFile, &fileStatus);
    
    if(validFile){/* file opened */
        validFile = initFileDataBases(databasesPtr, &fileStatus);
    }

    if(validFile){/* databases initialized */
        validFile = firstPass(sourceFile, &ICF, &DCF, databasesPtr, &fileStatus);
    }

    if(validFile){/* first pass no errors */
        validFile = secondPass(databasesPtr, ICF, &fileStatus);
    }

    if(validFile){/* second pass no errors */
        writeFiles(databases, sourceFileName, ICF, DCF, &fileStatus);
    }

    if(sourceFile){/* the file was opened */
        fclose(sourceFile);
    }

    divideFileErrorPrinting(&fileStatus);

    /* free any memory allocated during file compilation */
    clearFileDatabases(databasesPtr);
}


static boolean openFile(char *sourceFileName, FILE **sourceFilePtr, fileErrorStatus *fileStatusPtr) {
    boolean result = TRUE;
    *sourceFilePtr = fopen(sourceFileName, "r");

    fileStatusPtr->sourceFileName = sourceFileName;

    if(!*sourceFilePtr){/* could not open file */
        printFileErrorMessage(COULD_NOT_OPEN_FILE, NULL, fileStatusPtr);
        result = FALSE;
    }

    return result;
}


static void clearFileDatabases(databaseRouterPtr databasesPtr){

    clearCodeImageDB(databasesPtr->codeImageDB);
    databasesPtr->codeImageDB = NULL;

    clearDataImageDB(databasesPtr->dataImageDB);
    databasesPtr->dataImageDB = NULL;

    clearEntryCallsDB(databasesPtr->entryCallsDB);
    databasesPtr->entryCallsDB = NULL;

    clearExternUsesDB(databasesPtr->externUsesDB);
    databasesPtr->externUsesDB = NULL;

    clearLabelCallsDB(databasesPtr->labelCallsDB);
    databasesPtr->labelCallsDB = NULL;

    clearLabels(databasesPtr->labelsDB);
    databasesPtr->labelsDB = NULL;
}


static void clearProjectDatabases(databaseRouterPtr databasesPtr){
    clearOperationDB(databasesPtr->operationsDB);
    databasesPtr->operationsDB = NULL;
}

