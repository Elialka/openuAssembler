#include <stdio.h>
#include <string.h>

#include "print.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"
#include "codeImageDB.h"
#include "dataImageDB.h"

#define BYTES_IN_ROW (4)
#define SPACES_BEFORE_HEADLINE (5)

typedef enum{
    ENTRY_TYPE,
    EXTERN_TYPE
}fileType;

static void replaceExtension(char *sourceFileName, char *newExtension, char *destination);

static void printObjectFile(void **databasePointers, char *sourceFileName, long ICF, long DCF);

static void printLabelAddressesFile(void *database, char *sourceFileName, fileType currentFileType);

static char * getCurrentLabelName(void *currentLabel, fileType currentFileType);

static long getCurrentLabelAddress(void *currentLabel, fileType currentFileType);

void * getNextLabel(void *currentLabel, fileType currentFileType);


void writeFiles(void **databasePointers, char *sourceFilename, long ICF, long DCF) {
    printObjectFile(databasePointers, sourceFilename, ICF, DCF);

    if(!entryCallDBIsEmpty(databasePointers[ENTRY_CALLS_POINTER])){/* need to print entry file */
        printLabelAddressesFile(databasePointers[ENTRY_CALLS_POINTER], sourceFilename, ENTRY_TYPE);
    }

    if(!externDBIsEmpty(databasePointers[EXTERN_POINTER])){/* need to print extern file */
        printLabelAddressesFile(databasePointers[EXTERN_POINTER], sourceFilename, EXTERN_TYPE);
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
        if(!(imageCounter % BYTES_IN_ROW)){/* print in new line + print current address */
            fprintf(objectFile, "\n%04ld", imageCounter + STARTING_ADDRESS);
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

    /* close file */
    fclose(objectFile);
}


static void printLabelAddressesFile(void *database, char *sourceFileName, fileType currentFileType) {
    FILE *labelsFile;
    char labelsFileName[MAX_FILENAME_LENGTH];
    void *currentLabel = database;
    char *labelName;
    long labelAddress;

    /* get file name including relevant extension */
    char *extension = currentFileType == ENTRY_TYPE ? "ent" : "ext";
    replaceExtension(sourceFileName, extension, labelsFileName);

    /* create file */
    labelsFile = fopen(labelsFileName, "w");
    if(!labelsFile){/* cannot create file */
        /* todo print error quit function */
    }

    /* print each entry line */
    while(currentLabel){
        /* get entry attributes and print them in format */
        labelName = getCurrentLabelName(currentLabel, currentFileType);
        labelAddress = getCurrentLabelAddress(currentLabel, currentFileType);
        fprintf(labelsFile, "%s %04ld\n", labelName, labelAddress);

        /* get next entry */
        currentLabel = getNextLabel(currentLabel, currentFileType);
    }

    /* close file */
    fclose(labelsFile);
}


static char * getCurrentLabelName(void *currentLabel, fileType currentFileType){
    char *result;

    if(currentFileType == ENTRY_TYPE){
        result = getEntryCallName(currentLabel);
    }
    else{
        result = getExternUseName(currentLabel);
    }

    return result;
}


static long getCurrentLabelAddress(void *currentLabel, fileType currentFileType){
    long result;

    if(currentFileType == ENTRY_TYPE){
        result = getEntryCallAddress(currentLabel);
    }
    else{
        result = getExternUseAddress(currentLabel);
    }

    return result;
}


void * getNextLabel(void *currentLabel, fileType currentFileType){
    void *result;

    if(currentFileType == ENTRY_TYPE){
        result = getNextEntryCall(currentLabel);
    }
    else{
        result = getNextExternUse(currentLabel);
    }

    return result;
}


