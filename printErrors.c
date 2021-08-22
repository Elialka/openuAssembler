#include <stdio.h>

#include "global.h"
#include "printErrors.h"



void printWarningMessage(warningCodes encounteredWarning, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr) {
    if(!fileStatusPtr->errorOccurred){/* first error for current file */
        printf("Errors occurred during compiling file: \"%s\"\n", fileStatusPtr->sourceFileName);
    }

    switch(encounteredWarning){
        case LINE_TOO_LONG:
            printf("WARNING! Line is longer than max supported length %d, didn't read some characters!\n",
                   MAX_LINE);
            break;
            case DEFINED_LABEL_ENTRY_EXTERN:
                printf("WARNING! Label definition at entry or extern line is ignored!\n");
                break;
                default:
                    printf("INTERNAL ERROR! Reached impossible scenario in printWarningMessage !\n");
                    break;
    }

    printf("Line %ld: %s\n", lineIdPtr->count, lineIdPtr->line);
}


void printFileErrorMessage(errorCodes encounteredError, lineID *lineIdPtr, fileErrorStatus *fileStatusPtr) {
    if(!fileStatusPtr->errorOccurred){/* first error for current file */
        printf("Errors occurred during compiling file: \"%s\"\n", fileStatusPtr->sourceFileName);
        fileStatusPtr->errorOccurred = TRUE;
    }

    switch(encounteredError){
        case NO_ERROR:
            printf("INTERNAL ERROR - NO_ERROR reached print error message!\n");
            break;
        case DOUBLE_LABEL_DEFINITION:
            printf("ERROR! A label cannot be defined twice!\n");
            break;
        case LABEL_LOCAL_AND_EXTERN:
            printf("ERROR! A label cannot be defined locally and declared as external!\n");
            break;
        case LABEL_NOT_FOUND:
            printf("ERROR! The label does not exist!\n");
            break;
        case NO_SPACE_AFTER_LABEL:
            printf("ERROR! A white character after label definition is mandatory!\n");
            break;
        case LABEL_IS_OPERATION:
            printf("ERROR! Cannot define label - saved word!\n");
            break;
        case LABEL_TOO_LONG:
            printf("ERROR! Label is longer than max length supported for labels: %d characters!\n",
                   MAX_LABEL_LENGTH);
            break;
        case ILLEGAL_LABEL_NAME:
            printf("ERROR! Label names must begin with a letter, and contain only letters and numbers!\n");
            break;
        case ADDRESS_DISTANCE_OVER_LIMITS:
            printf("ERROR! Distance between labels cannot exceed %d bytes!\n", I_TYPE_IMMED_MAX_VALUE_UNSIGNED);
            break;
        case ENTRY_IS_EXTERN:
            printf("ERROR! Cannot declare label as both entry and extern!\n");
            break;
        case ENTRY_NOT_DEFINED:
            printf("ERROR! Cannot declare not locally defined label as entry\n");
            break;
        case CANNOT_BE_EXTERN:
            printf("ERROR! This command does not support external labels\n");
            break;
        case MEMORY_ALLOCATION_FAILURE:
            printf("FATAL ERROR! Memory allocation failure!\n");
            break;
        case EXCEEDING_MEMORY_LIMITS:
            printf("FATAL ERROR! Program memory exceeds 25 bit memory limits!\n");
            break;
        case UNIDENTIFIED_OPERATION_NAME:
            printf("ERROR! Unrecognized command!\n");
            break;
        case MISSING_OPERATION_NAME:
            printf("ERROR! Missing command name!\n");
            break;
        case MISSING_PARAMETER:
            printf("ERROR! Missing one or more parameters for given command!\n");
            break;
        case EXPECTED_REGISTER_FIRST:
            printf("ERROR! First parameter for given command should be a register!\n");
            break;
        case EXPECTED_REGISTER_SECOND:
            printf("ERROR! Second parameter for given command should be a register!\n");
            break;
        case EXPECTED_REGISTER_THIRD:
            printf("ERROR! Third parameter for given command should be a register!\n");
            break;
        case EXPECTED_NUMBER_SECOND:
            printf("ERROR! Second parameter for given command should be a number!\n");
            break;
        case EXPECTED_LABEL_FIRST:
            printf("ERROR! First parameter for given command should be a Label!\n");
            break;
            case ILLEGAL_REGISTER_ID:
            printf("ERROR! Illegal register identifier, should be \'$n\', when n is between %d - %d!\n",
                   REGISTER_MIN_INDEX, REGISTER_MAX_INDEX);
            break;
        case MISSING_QUOTE:
            printf("ERROR! asciz string operand should be inside quotes!\n");
            break;
        case NOT_PRINTABLE_CHAR:
            printf("ERROR! Asciz string operand can only contain printable characters!\n");
            break;
        case NOT_NUMBER:
            printf("ERROR! Mixed digits and other characters in number operand!\n");
            break;
        case NOT_INTEGER:
            printf("ERROR! This program only supports integers!\n");
            break;
        case NOT_REGISTER:
            printf("ERROR! Illegal register token!\n");
            break;
        case NUMBER_OUT_OF_RANGE:
            printf("ERROR! Given number is out of range for given data instruction!\n");
            break;
        case MISSING_COMMA:
            printf("ERROR! Missing comma between operands!\n");
            break;
        case ILLEGAL_COMMA:
            printf("ERROR! Extraneous comma after arguments!\n");
            break;
        case EXTRANEOUS_TEXT:
            printf("ERROR! Extraneous text at the end of the line!\n");
            break;
        case COULD_NOT_CREATE_FILE:
            printf("ERROR! Cannot create output file!\n");
            break;
        case COULD_NOT_OPEN_FILE:
            printf("ERROR! Cannot open file!\n");
            break;
        case IMPOSSIBLE:
            printf("INTERNAL ERROR! Impossible scenario detected!\n");
            break;
        default:
            printf("INTERNAL ERROR! Reached impossible scenario in printFileErrorMessage !\n");
            break;
    }

    if(lineIdPtr){/* error is line specific */
        printf("Line %ld: %s\n", lineIdPtr->count, lineIdPtr->line);
    }
}


void printProjectErrorMessage(projectErrors errorCode){
    switch(errorCode){
        case FILENAME_LENGTH_NOT_SUPPORTED:
            printf("ERROR! Illegal file name - max filename length supported is %d!\n", MAX_FILENAME_LENGTH);
            break;
        case ILLEGAL_FILE_EXTENSION:
            printf("ERROR! Illegal file name - supported file extension is: \"%s\"!\n", SOURCE_FILE_EXTENSION);
            break;
        case NO_FILES_TO_COMPILE:
            printf("ERROR! No Files to compile! Quitting program!\n");
            break;
        default:
            printf("INTERNAL ERROR! Reached impossible scenario in printProjectErrorMessage !\n");
            break;
    }
}


void divideFileErrorPrinting(fileErrorStatus *fileStatusPtr){
    if(fileStatusPtr->errorOccurred){
        printf("--------------------------------------------------------------------------------------------------\n");
    }
}
