#include <stdio.h>
#include <string.h>

#include "print.h"
#include "entryCallsDB.h"
#include "externUsesDB.h"

static void removeFileExtension(char *buffer, char *source);



void writeFiles(void **databasePointers, char *sourceFilename, long ICF, long DCF) {
    char fileNameNoExtension[MAX_FILENAME_LENGTH];

    removeFileExtension(fileNameNoExtension, sourceFilename);

    /* todo print object file */

    if(!entryCallDBIsEmpty(databasePointers[ENTRY_CALLS_POINTER])){/* need to print entry file */
        /* todo write print function here */
    }

    if(!externDBIsEmpty(databasePointers[EXTERN_POINTER])){/* need to print extern file */
        /* todo write print function here */
    }


}


static void removeFileExtension(char *buffer, char *source){
    char *position = source;
    int length;

    /* find end of source file */
    for(;*position; position++)
        ;

    /* find start of extension */
    for(;*position != '.'; position--)
        ;


    length = (int)(position - source);

    strncpy(buffer, source, length);
}


static void addExtension(char *name, char *extension){
    /* add dot */
    strcat(name, ".");
    /* add extension */
    strcat(name, extension);
}
