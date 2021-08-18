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

static void printObjectFile(databaseRouter databases, char *sourceFileName, long ICF, long DCF);

static void printLabelAddressesFile(void *database, char *sourceFileName, fileType currentFileType);

static char * getCurrentLabelName(void *currentLabel, fileType currentFileType);

static long getCurrentLabelAddress(void *currentLabel, fileType currentFileType);

void * getNextLabel(void *currentLabel, fileType currentFileType);


void writeFiles(databaseRouter databases, char *sourceFilename, long ICF, long DCF) {
    printObjectFile(databases, sourceFilename, ICF, DCF);

    if(!isEntryCallDBEmpty(databases.entryCallsDB)){/* need to print entry file */
        printLabelAddressesFile(databases.entryCallsDB, sourceFilename, ENTRY_TYPE);
    }

    if(!isExternDBEmpty(databases.externUsesDB)){/* need to print extern file */
        printLabelAddressesFile(databases.externUsesDB, sourceFilename, EXTERN_TYPE);
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
    destination[length] = '\0';

    /* add extension */
    strcat(destination, newExtension);
}


static void printObjectFile(databaseRouter databases, char *sourceFileName, long ICF, long DCF) {
    FILE *objectFile;
    char objectFileName[MAX_FILENAME_LENGTH];
    unsigned char nextByte;
    int i;
    long imageCounter;/* how many bytes already printed */
    long dataCounter;/* how many data bytes already printed */

    replaceExtension(sourceFileName, OBJECT_FILE_EXTENSION, objectFileName);

    /* create file */
    objectFile = fopen(objectFileName, "w");
    if(!objectFile){/* cannot create file */
        printErrorMessage(COULD_NOT_CREATE_FILE, 0);
        return;/* todo refactor */
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
        nextByte = getNextCodeByte(databases.codeImageDB, imageCounter);
        fprintf(objectFile, " %02X", nextByte);
    }

    /* print data image */
    for(dataCounter = 0; dataCounter < DCF; dataCounter++, imageCounter++){
        if(!(imageCounter % BYTES_IN_ROW)){/* print in new line + print current address */
            fprintf(objectFile, "\n%04ld", imageCounter + STARTING_ADDRESS);
        }
        nextByte = getNextDataByte(databases.dataImageDB, dataCounter);
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
    char *extension = currentFileType == ENTRY_TYPE ? ENTRY_FILE_EXTENSION : EXTERN_FILE_EXTENSION;
    replaceExtension(sourceFileName, extension, labelsFileName);

    /* create file */
    labelsFile = fopen(labelsFileName, "w");
    if(!labelsFile){/* cannot create file */
        printErrorMessage(COULD_NOT_CREATE_FILE, 0);
        return;/* todo refactor */
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

/* todo maybe add line and token printing - fix line numbering */
void printErrorMessage(errorCodes encounteredError, long lineNumber){
    switch(encounteredError){
        case NO_ERROR:
            printf("INTERNAL ERROR - line %ld  NO_ERROR reached print error message!\n", lineNumber);
            break;
        case DOUBLE_LABEL_DEFINITION:
            printf("ERROR! line %ld  A label cannot be defined twice!\n", lineNumber);
            break;
        case LABEL_LOCAL_AND_EXTERN:
                printf("ERROR! line %ld  A label cannot be defined locally and declared as external!\n", lineNumber);
                break;
        case LABEL_NOT_FOUND:
            printf("ERROR! line %ld  The label does not exist!\n", lineNumber);
            break;
        case NO_SPACE_AFTER_LABEL:
            printf("ERROR! line %ld  A white character after label definition is mandatory!\n", lineNumber);
            break;
        case LABEL_IS_OPERATION:
            printf("ERROR! line %ld  Cannot define label - saved word!\n", lineNumber);
            break;
        case LABEL_TOO_LONG:
            printf("ERROR! line %ld  Label is longer than max length supported for labels: %d characters!\n",
                   lineNumber, MAX_LABEL_LENGTH);
            break;
        case ILLEGAL_LABEL_NAME:
            printf("ERROR! line %ld  Label names must begin with a letter, and contain only letters and numbers!\n",
                   lineNumber);
            break;
        case ADDRESS_DISTANCE_OVER_LIMITS:
            printf("ERROR! line %ld  Distance between labels cannot exceed %d bytes!\n",
                   lineNumber, I_TYPE_IMMED_MAX_VALUE_UNSIGNED);
            break;
        case ENTRY_IS_EXTERN:
            printf("ERROR! line %ld  Cannot declare label as both entry and extern!\n", lineNumber);
            break;
        case ENTRY_NOT_DEFINED:
            printf("ERROR! line %ld  Cannot declare not locally defined label as entry\n", lineNumber);
            break;
        case MEMORY_ALLOCATION_FAILURE:
            printf("FATAL ERROR! line %ld  Memory allocation failure!\n", lineNumber);
            break;
        case UNIDENTIFIED_OPERATION_NAME:
            printf("ERROR! line %ld  Unrecognized command!\n", lineNumber);
            break;
        case MISSING_OPERATION_NAME:
            printf("ERROR! line %ld  Missing command name!\n", lineNumber);
            break;
        case MISSING_PARAMETER:
            printf("ERROR! line %ld  Missing one or more parameters for given command!\n", lineNumber);
            break;
        case EXPECTED_REGISTER_FIRST:
            printf("ERROR! line %ld  First parameter for given command should be a register!\n", lineNumber);
            break;
        case EXPECTED_REGISTER_SECOND:
            printf("ERROR! line %ld  Second parameter for given command should be a register!\n", lineNumber);
            break;
        case EXPECTED_REGISTER_THIRD:
            printf("ERROR! line %ld  Third parameter for given command should be a register!\n", lineNumber);
            break;
        case EXPECTED_NUMBER_SECOND:
            printf("ERROR! line %ld  Second parameter for given command should be a number!\n", lineNumber);
            break;
        case EXPECTED_LABEL_FIRST:
            printf("ERROR! line %ld  First parameter for given command should be a Label!\n", lineNumber);
            break;
        case REGISTER_OUT_OF_RANGE:/* todo maybe unite enum values */
        case REGISTER_ILLEGAL_CHAR:
            printf("ERROR! line %ld  Illegal register identifier, should be \'$n\', when n is between %d - %d!\n",
                   lineNumber, REGISTER_MIN_INDEX, REGISTER_MAX_INDEX);
            break;
        case MISSING_QUOTE:
            printf("ERROR! line %ld  asciz string operand should be inside quotes!\n", lineNumber);
            break;
        case NOT_PRINTABLE_CHAR:
            printf("ERROR! line %ld  Asciz string operand can only contain printable characters!\n", lineNumber);
            break;
        case NOT_NUMBER:
            printf("ERROR! line %ld  Mixed digits and other characters in number operand!\n", lineNumber);
            break;
        case NOT_INTEGER:
            printf("ERROR! line %ld  This program only supports integers!\n", lineNumber);
            break;
        case NOT_REGISTER:
            printf("ERROR! line %ld  Illegal register token!\n", lineNumber);/* todo check enum value */
            break;
        case MISSING_COMMA:
            printf("ERROR! line %ld  Missing comma between operands!\n", lineNumber);
            break;
        case ILLEGAL_COMMA:/* todo add check after code operands extraction */
            printf("ERROR! line %ld  Extraneous comma after arguments!\n", lineNumber);
            break;
        case FILENAME_LENGTH_NOT_SUPPORTED:
            printf("ERROR! Illegal file name - max filename length supported is %d!\n", MAX_FILENAME_LENGTH);
            break;
        case ILLEGAL_FILE_EXTENSION:
                printf("ERROR! Illegal file name - supported file extension is: \"%s\"!\n", SOURCE_FILE_EXTENSION);
            break;
        case NO_FILES_TO_COMPILE:
            printf("ERROR! No Files to compile! Quitting program!\n");
            break;
        case COULD_NOT_OPEN_FILE:
            printf("ERROR! Cannot open file!\n");
            break;
        case EXTRANEOUS_TEXT:
            printf("ERROR! line %ld  Extraneous text at the end of the line!\n", lineNumber);
            break;
        case COULD_NOT_CREATE_FILE:
            printf("ERROR! Cannot create output file!\n");
            break;
        case LINE_TOO_LONG:
            printf("WARNING! line %ld  Line is longer than max supported length %d, didn't read some characters!\n",
                   lineNumber, MAX_LINE);
            break;
        case DEFINED_LABEL_ENTRY_EXTERN:
            printf("WARNING! line %ld  Label definition at entry or extern line is ignored!\n", lineNumber);
            break;
        case IMPOSSIBLE:
            printf("INTERNAL ERROR! line %ld  Impossible scenario detected!\n", lineNumber);
            break;
    }
}
