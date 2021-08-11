#include <stdio.h>
#include <string.h>

#include "print.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "codeImageDB.h"
#include "dataImageDB.h"

#define BYTES_IN_ROW (4)
#define SPACES_BEFORE_HEADLINE (5)

static void replaceExtension(char *sourceFileName, char *newExtension, char *destination);

static void printObjectFile(void **databasePointers, char *sourceFileName, long ICF, long DCF);

static void printEntryFile(void *entryDatabase, char *sourceFileName);

static void printExternFile(void *externDatabase, char *sourceFileName);



void writeFiles(void **databasePointers, char *sourceFilename, long ICF, long DCF) {
    char fileNameNoExtension[MAX_FILENAME_LENGTH];

    printObjectFile(databasePointers, sourceFilename, ICF, DCF);

    if(!entryCallDBIsEmpty(databasePointers[ENTRY_CALLS_POINTER])){/* need to print entry file */
        printEntryFile(databasePointers[ENTRY_CALLS_POINTER], sourceFilename);
    }

    if(!externDBIsEmpty(databasePointers[EXTERN_POINTER])){/* need to print extern file */
        printEntryFile(databasePointers[EXTERN_POINTER], sourceFilename);
    }


}


static void replaceExtension(char *sourceFileName, char *newExtension, char *destination) {
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

    /* add dot */
    strcat(destination, ".");

    /* add extension */
    strcat(destination, newExtension);
}


static void printObjectFile(void **databasePointers, char *sourceFileName, long ICF, long DCF) {
    FILE *objectFile;
    char objectFileName[MAX_FILENAME_LENGTH];
    unsigned char nextByte;
    int i;
    long imageCounter;/* how many bytes already printed */
    long dataCounter;/* how many data bytes already printed */

    replaceExtension(sourceFileName, "ob", objectFileName);

    /* create file */
    objectFile = fopen(objectFileName, "w");
    if(!objectFile){/* cannot create file */
        /* todo print error quit function */
    }

    /* print headline */
    for(i = 0; i < SPACES_BEFORE_HEADLINE; i++){
        fprintf(objectFile, " ");
    }
    fprintf(objectFile, "%ld %ld", ICF, DCF);

    /* print code image */
    for(imageCounter = 0; imageCounter < ICF; imageCounter++){
        if(!imageCounter % BYTES_IN_ROW){/* print in new line + print current address */
            fprintf(objectFile, "\n%03ld", imageCounter + STARTING_ADDRESS);
        }
        nextByte = getNextCodeByte(databasePointers[CODE_IMAGE_POINTER], imageCounter);
        fprintf(objectFile, " %02X", nextByte);
    }

    /* print data image */
    for(dataCounter = 0; dataCounter < DCF; dataCounter++, imageCounter++){
        if(!imageCounter % BYTES_IN_ROW){/* print in new line + print current address */
            fprintf(objectFile, "\n%03ld", imageCounter + STARTING_ADDRESS);
        }
        nextByte = getNextDataByte(databasePointers[DATA_IMAGE_POINTER], dataCounter);
        fprintf(objectFile, " %02X", nextByte);
    }
}


static void printEntryFile(void *entryDatabase, char *sourceFileName){

}


static void printExternFile(void *externDatabase, char *sourceFileName){

}

