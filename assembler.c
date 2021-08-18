#include <stdio.h>
#include <string.h>

#include "firstPass.h"
#include "secondPass.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "dataImageDB.h"
#include "codeImageDB.h"
#include "labelCallsDB.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "print.h"

#ifndef MAX_LINE
#include "data.h"
#endif

#include "tests.h"

/* todo possible refactors */
/* reset new memory after reallocation */
/* program does not address label definition without ':' - wrond error message */
/* pandas refactor long functions/functions that receive many parameters */
/* split firstPass */
/* for every database - extract finding end of DB\allocating additional memory to different function */
/* for every database - check if head not NULL */
/* in the end, make as many functions as static as possible */

/**
 * Initialize static databases to be used throughout the program,
 * set pointers to other databases to NULL
 * @param databasesPtr pointer to database router struct
 * @return TRUE if memory allocated successfully, FALSE otherwise
 */
static boolean initProjectDatabases(databaseRouterPtr databasesPtr);

/**
 * Initialize dynamic databases to be used for a file being compiled
 * @param databasesPtr pointer to database router struct
 * @return TRUE if memory allocated successfully, FALSE otherwise
 */
static boolean initFileDataBases(databaseRouterPtr databasesPtr);

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
static boolean openFile(char *sourceFileName, FILE **sourceFilePtr);

/**
 * Free memory allocated for per-file databases, set pointers to databases to NULL
 * @param databasesPtr pointer to database router struct
 */
static void clearFileDatabases(databaseRouterPtr databasesPtr);

/**
 * Free memory allocated for per-project databases, set pointers to databases to NULL
 * @param databasesPtr pointer to database router struct
 */
static void clearProjectDatabases(databaseRouterPtr databasesPtr);

long lineCounterGlobal = 0;/* temp - delete */

int main(int argc, char *argv[]){
    long i;
    databaseRouter databases;


    if(argc < 2){/* no files to compile */
        printErrorMessage(NO_FILES_TO_COMPILE, 0);
    }
    else{/* program inline parameters present */
        /* initialize operation names database */
        initProjectDatabases(&databases);

        /* test - delete */
        /*
        initFileDataBases(&databases);
        testFunctions(&databases);
        clearFileDatabases(&databases);
         */
        /* end of test */

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
        printErrorMessage(MEMORY_ALLOCATION_FAILURE, 0);
    }

    return allocationSuccess;
}


static boolean initFileDataBases(databaseRouterPtr databasesPtr){
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
        printErrorMessage(MEMORY_ALLOCATION_FAILURE, 0);
    }

    return allocationSuccess;
}


/* TODO test function */
static boolean supportedFileName(char *sourceFileName) {
    char *currentChar = sourceFileName;
    boolean result = TRUE;
    errorCodes encounteredError = NO_ERROR;

    /* validate extension */
    for(; *currentChar; currentChar++)/* go to end of name */
        ;

    for(; *currentChar != '.'; currentChar--)/* locate start of extension */
        ;

    if(strcmp(currentChar, SOURCE_FILE_EXTENSION) != 0){/* illegal extension */
        encounteredError = ILLEGAL_FILE_EXTENSION;
    }

    /* validate length */
    if(!encounteredError && strlen(sourceFileName) > MAX_FILENAME_LENGTH){
        encounteredError = FILENAME_LENGTH_NOT_SUPPORTED;
    }

    if(encounteredError){
        result = FALSE;
        printErrorMessage(encounteredError, 0);
    }

    return result;
}


static void compileFile(char *sourceFileName, databaseRouter databases){
    long ICF = 0;/* will store size of code image, in bytes */
    long DCF = 0;/* will store size of data image, in bytes */
    databaseRouterPtr databasesPtr = &databases;/* pointer to databases struct */
    boolean validFile;/* track if any errors occurred */
    FILE *sourceFile = NULL;/* pointer to current file */

    validFile = openFile(sourceFileName, &sourceFile);
    
    if(validFile){/* file opened */
        validFile = initFileDataBases(databasesPtr);
    }

    if(validFile){/* databases initialized */
        validFile = firstPass(sourceFile, &ICF, &DCF, databasesPtr);
    }

    if(validFile){/* first pass no errors */
        validFile = secondPass(databasesPtr, ICF);
    }

    if(validFile){/* second pass no errors */
        writeFiles(databases, sourceFileName, ICF, DCF);
    }

    if(sourceFile){/* file has been opened */
        fclose(sourceFile);/* todo check if need to use returned value */
    }

    /* free any memory allocated during file compilation */
    clearFileDatabases(databasesPtr);
}


static boolean openFile(char *sourceFileName, FILE **sourceFilePtr){
    boolean result = TRUE;
    *sourceFilePtr = fopen(sourceFileName, "r");

    if(!*sourceFileName){/* could not open file */
        printErrorMessage(COULD_NOT_OPEN_FILE, 0);
        result = FALSE;
    }

    return result;
}


static void clearFileDatabases(databaseRouterPtr databasesPtr){

    if(databasesPtr->codeImageDB){/* memory needs to be freed */
        clearCodeImageDB(databasesPtr->codeImageDB);
        databasesPtr->codeImageDB = NULL;
    }

    if(databasesPtr->dataImageDB){/* memory needs to be freed */
        clearDataImageDB(databasesPtr->dataImageDB);
        databasesPtr->dataImageDB = NULL;
    }

    if(databasesPtr->entryCallsDB){/* memory needs to be freed */
        clearEntryCallsDB(databasesPtr->entryCallsDB);
        databasesPtr->entryCallsDB = NULL;
    }

    if(databasesPtr->externUsesDB){/* memory needs to be freed */
        clearExternUsesDB(databasesPtr->externUsesDB);
        databasesPtr->externUsesDB = NULL;
    }

    if(databasesPtr->labelCallsDB){/* memory needs to be freed */
        clearLabelCallsDB(databasesPtr->labelCallsDB);
        databasesPtr->labelCallsDB = NULL;
    }

    if(databasesPtr->labelsDB){/* memory needs to be freed */
        clearLabels(databasesPtr->labelsDB);
        databasesPtr->labelsDB = NULL;
    }
}


static void clearProjectDatabases(databaseRouterPtr databasesPtr){
    /* free static database */
    if(databasesPtr->operationsDB){/* memory needs to be freed */
        clearOperationDB(databasesPtr->operationsDB);
        databasesPtr->operationsDB = NULL;
    }
}

