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


/* todo add error printing */
/* todo forgot to address LA or CALL operations to get a label */

/* todo possible refactors */
/* pandas refactor long functions/functions that receive many parameters */
/* split firstPass */
/* for every database - extract finding end of DB\allocating additional memory to different function */
/* in the end, make as many functions as static as possible */


static boolean initDataBases(databaseRouterPtr databasePtr);

static boolean legitFileName(char *name);

static void clearDatabases(databaseRouterPtr databasePtr);


int main(int argc, char *argv[]){
    long ICF, DCF, i;
    boolean validFile;/* track if any error occurred during current file */
    FILE *sourceFile;
    databaseRouter databases;
    databaseRouterPtr databasesPtr = &databases;

    if(argc < 2){/* no files to compile */
        /* todo print error - quit program */
        printf("No arguments to program, argc:%d\n", argc);
    }

    /* initialize operation names database */
    if(!(databasesPtr->operationsDB = setOperations())){
        /* todo print error */
    }

    /* test - delete */
    testFunctions(databasesPtr);
    /* end of test */

    /* compile files */
    for(i = 1; i < argc; i++){
        validFile = legitFileName(argv[i]);

        /* open file */
        if(validFile){/* legal file name */
            sourceFile = fopen(argv[i], "r");
        }

        /* initialize/reset databases */
        if(sourceFile){/* file opened */
            if(!initDataBases(databasesPtr)){
                /* todo print error memory alloc - quit program */
            }
        }
        else{/* couldn't open file */
            /* TODO print error */
            validFile = FALSE;
        }

        /* read source file */
        if(validFile){/* no errors so far */
            validFile = firstPass(sourceFile, &ICF, &DCF, databasesPtr);
        }

        /* fill missing data in codeImage */
        if(validFile){/* no errors so far  */
            /* todo update data labels + ICF */
            validFile = secondPass(databasesPtr, ICF);
        }
        else{/* temp - delete */
            printf("firstPassFailed\n");
        }

        /* generate output files */
        if(validFile){
            writeFiles(databases, argv[i], ICF, DCF);
        }

        /* clear databases, close file */
        if(sourceFile){/* file was opened - databases were initialized */
            clearDatabases(databasesPtr);
            fclose(sourceFile);/* todo check if need to use returned value */
        }
    }

    /* free remaining memory allocations */
    clearOperationDB(databasesPtr->operationsDB);

    return 0;
}


static boolean initDataBases(databaseRouterPtr databasePtr){
    boolean allocationSuccess = TRUE;

    if(!(databasePtr->codeImageDB = initCodeImage())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePtr->dataImageDB = initDataImageDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePtr->entryCallsDB = initEntryCallsDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePtr->externUsesDB = initExternUsesDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePtr->labelCallsDB = initLabelCallsDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePtr->labelsDB = initLabelsDB())){
        allocationSuccess = FALSE;
    }

    return allocationSuccess;
}


/* TODO test function */
boolean legitFileName(char *name) {
    char *c;
    boolean result;
    errorCodes error = NO_ERROR;

    /* seek . in name */
    for(c = name; *c && *c != '.'; c++)
        ;

    if(strlen(name) > MAX_FILENAME_LENGTH){/* not supported filename length */
        error = FILENAME_LENGTH_NOT_SUPPORTED;
        result = FALSE;
    }
    else if(*c == '.' && *(c+1) == 'a' && *(c+2) == 's'){/* legal file extension */
        result = TRUE;
    }
    else{
        error = ILLEGAL_FILE_EXTENSION;
        result = FALSE;
    }

    if(error){
        /* todo print error */
    }

    return result;
}


static void clearDatabases(databaseRouterPtr databasePtr){
    clearCodeImageDB(databasePtr->codeImageDB);
    databasePtr->codeImageDB = NULL;

    clearDataImageDB(databasePtr->dataImageDB);
    databasePtr->dataImageDB = NULL;

    clearEntryCallsDB(databasePtr->entryCallsDB);
    databasePtr->entryCallsDB = NULL;

    clearExternUsesDB(databasePtr->externUsesDB);
    databasePtr->externUsesDB = NULL;

    clearLabelCallsDB(databasePtr->labelCallsDB);
    databasePtr->labelCallsDB = NULL;

    clearLabels(databasePtr->labelsDB);
    databasePtr->labelsDB = NULL;

}

