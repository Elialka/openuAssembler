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


/* todo code and data image - array size counters will not reinitialize for next file - refactor to use externDBIsEmpty*/
/* todo output files */
/* todo split firstPass */


static boolean initDataBases(void **databasePointers);

static boolean legitFileName(char *name);

static void clearDatabases(void **databasePointers);




int main(int argc, char *argv[]){
    long ICF, DCF, i;
    boolean validFile;/* track if any error occurred during current file */
    FILE *sourceFile;
    void *databasePointers[DATABASE_POINTER_ARRAY_SIZE];

    if(argc < 2){/* no files to compile */
        /* todo print error - quit program */
    }

    /* initialize operation names database */
    if(!(databasePointers[OPERATIONS_POINTER] = setOperations())){
        /* todo print error */
    }

    /* test - delete */
    initDataBases(databasePointers);
    testFunctions(databasePointers);
    clearDatabases(databasePointers);
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
            if(!initDataBases(databasePointers)){
                /* todo print error memory alloc - quit program */
            }
        }
        else{/* couldn't open file */
            /* TODO print error */
            validFile = FALSE;
        }

        /* read source file */
        if(validFile){/* no errors so far */
            validFile = sourceFilePass(sourceFile, &ICF, &DCF, databasePointers);
        }

        /* fill missing data in codeImage */
        if(validFile){/* no errors so far  */
            /* todo update data labels + ICF */
            validFile = secondPass(databasePointers, ICF);
        }

        /* generate output files */
        if(validFile){
            writeFiles(databasePointers, argv[i]);
        }

        /* clear databases */
        if(sourceFile){/* file was opened - databases were initialized */
            clearDatabases(databasePointers);
        }
    }

    /* free remaining memory allocations */
    clearOperationDB(databasePointers[OPERATIONS_POINTER]);

    return 0;
}


static boolean initDataBases(void *databasePointers[]){
    boolean allocationSuccess = TRUE;

    if(!(databasePointers[LABELS_POINTER] = initLabelsDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePointers[DATA_IMAGE_POINTER] = initDataImageDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePointers[CODE_IMAGE_POINTER] = initCodeImage())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePointers[LABEL_CALLS_POINTER] = initLabelCallsDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePointers[ENTRY_CALLS_POINTER] = initEntryCallsDB())){
        allocationSuccess = FALSE;
    }
    else if(!(databasePointers[EXTERN_POINTER] = initExternUsesDB())){
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


static void clearDatabases(void **databasePointers){
    clearLabels(databasePointers[LABELS_POINTER]);
    clearDataImageDB(databasePointers[DATA_IMAGE_POINTER]);
    clearCodeImageDB(databasePointers[CODE_IMAGE_POINTER]);
    clearLabelCallsDB(databasePointers[LABEL_CALLS_POINTER]);
    clearEntryCallsDB(databasePointers[ENTRY_CALLS_POINTER]);
    clearExternUsesDB(databasePointers[EXTERN_POINTER]);
}

