#include <stdio.h>
#include <string.h>
#include <fvec.h>

#include "global.h"
#include "printFiles.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "codeImageDB.h"
#include "dataImageDB.h"
#include "printErrors.h"

#define BYTES_IN_ROW (4)
#define SPACES_BEFORE_HEADLINE (5)

typedef enum{
    OBJECT_TYPE,
    ENTRY_TYPE,
    EXTERN_TYPE
}fileType;

/**
 * Generate new file name with relevant extension
 * @param sourceFileName name of source file
 * @param newExtension extension of relevant file
 * @param destination where to copy name
 */
static void replaceExtension(char *sourceFileName, char *newExtension, char *destination);

/**
 * Generate output object file
 * @param databases struct holding database pointers
 * @param sourceFileName name of source file
 * @param ICF size of coda image
 * @param DCF size of data image
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 */
static void
printObjectFile(databaseRouter databases, char *sourceFileName, long ICF, long DCF, fileErrorStatus *fileStatusPtr);

/**
 * Create file with relevant name
 * @param sourceFileName name of source file
 * @param type enum value representing type of file
 * @param fileStatusPtr pointer to struct containing data regarding file error status
 * @return pointer to file
 */
static FILE *createFile(char *sourceFileName, fileType type, fileErrorStatus *fileStatusPtr);

/**
 * Print code/data image
 * @param newFile object file pointer
 * @param sizeOfDatabase size of relevant database
 * @param startingAddress first address to print as
 * @param databasePtr pointer to relevant database
 * @param getNextByte pointer to function to get next byte in relevant image
 */
static void printImage(FILE *newFile, long sizeOfDatabase, long startingAddress, void *databasePtr,
                       unsigned char (*getNextByte)(void *, long));

/**
 * Print entry/extern file
 * @param database pointer to relevant database
 * @param sourceFileName name of source file
 * @param type enum value representing type of file
 */
static void
printLabelAddressesFile(void *database, char *sourceFileName, fileType currentFileType, fileErrorStatus *fileStatusPtr);


void writeFiles(databaseRouter databases, char *sourceFilename, long ICF, long DCF, fileErrorStatus *fileStatusPtr) {
    printObjectFile(databases, sourceFilename, ICF, DCF, fileStatusPtr);

    if(!isEntryCallsDBEmpty(databases.entryCallsDB)){/* need to print entry file */
        printLabelAddressesFile(databases.entryCallsDB, sourceFilename, ENTRY_TYPE, fileStatusPtr);
    }

    if(!isExternDBEmpty(databases.externUsesDB)){/* need to print extern file */
        printLabelAddressesFile(databases.externUsesDB, sourceFilename, EXTERN_TYPE, fileStatusPtr);
    }
}


static void replaceExtension(char *sourceFileName, char *newExtension, char *destination){
    char *position = sourceFileName;
    int length;

    /* find end of source file */
    for(;*position; position++)
        ;

    /* find start of extension */
    for(;*position != '.'; position--)
        ;

    /* calculate length of name without extension */
    length = (int)(position - sourceFileName);

    /* copy source file name without the extension */
    strncpy(destination, sourceFileName, length);
    destination[length] = '\0';

    /* add extension */
    strcat(destination, newExtension);
}


static void
printObjectFile(databaseRouter databases, char *sourceFileName, long ICF, long DCF, fileErrorStatus *fileStatusPtr) {
    FILE *objectFile;
    int i;

    objectFile = createFile(sourceFileName, OBJECT_TYPE, fileStatusPtr);

    if(objectFile){
        /* print headline */
        for(i = 0; i < SPACES_BEFORE_HEADLINE; i++){
            fprintf(objectFile, " ");
        }
        fprintf(objectFile, "%ld %ld", ICF, DCF);

        /* print code image */
        printImage(objectFile, ICF, STARTING_ADDRESS, databases.codeImageDB, getEncodedCodeByte);

        /* print data image */

        printImage(objectFile, DCF, STARTING_ADDRESS + ICF, databases.dataImageDB, getNextDataByte);

        /* close file */
        fclose(objectFile);
    }
}


static FILE *createFile(char *sourceFileName, fileType type, fileErrorStatus *fileStatusPtr) {
    char newFileName[MAX_FILENAME_LENGTH];
    char *extension = NULL;
    FILE *newFile = NULL;

    switch(type){
        case OBJECT_TYPE:
            extension = OBJECT_FILE_EXTENSION;
            break;
        case ENTRY_TYPE:
            extension = ENTRY_FILE_EXTENSION;
            break;
        case EXTERN_TYPE:
            extension = EXTERN_FILE_EXTENSION;
            break;
        default:
            printFileErrorMessage(IMPOSSIBLE, NULL, fileStatusPtr);
    }

    replaceExtension(sourceFileName, extension, newFileName);

    newFile = fopen(newFileName, "w");
    if(!newFile){
        printFileErrorMessage(COULD_NOT_CREATE_FILE, NULL, fileStatusPtr);
    }

    return newFile;
}


static void printImage(FILE *newFile, long sizeOfDatabase, long startingAddress, void *databasePtr,
                       unsigned char (*getNextByte)(void *, long)) {
    long imageCounter;/* how many bytes printed */
    unsigned char nextByte;/* next byte in database */
    for(imageCounter = 0; imageCounter < sizeOfDatabase; imageCounter++){
        if(!(imageCounter % BYTES_IN_ROW)){/* print in new line + print current address */
            fprintf(newFile, "\n%04ld", imageCounter + startingAddress);
        }
        /* print next byte */
        nextByte = getNextByte(databasePtr, imageCounter);
        fprintf(newFile, " %02X", nextByte);
    }
}


static void
printLabelAddressesFile(void *database, char *sourceFileName, fileType currentFileType, fileErrorStatus *fileStatusPtr) {
    FILE *newFile;
    void *nextLabel = database;
    void *currentLabel = NULL;
    labelID *labelIdPtr;

    newFile = createFile(sourceFileName, currentFileType, NULL);

    if(newFile){
        /* print each line separately */
        while(nextLabel){
            currentLabel = nextLabel;
            if(currentFileType == ENTRY_TYPE){
                labelIdPtr = &getEntryCallData(currentLabel)->labelId;
                nextLabel = getNextEntryCall(currentLabel);
            }
            else{/* is extern type */
                labelIdPtr = getExternUseData(currentLabel);
                nextLabel = getNextExternUse(currentLabel);
            }

            fprintf(newFile, "%s %04ld\n", labelIdPtr->name, labelIdPtr->address);
        }
        /* close file */
        fclose(newFile);
    }
    else{
        printFileErrorMessage(COULD_NOT_CREATE_FILE, NULL, fileStatusPtr);
    }
}
