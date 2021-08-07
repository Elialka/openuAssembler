#include <stdio.h>
#include "labelsDB.h"
#include "operationsDB.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "codeImageDB.h"
#include "labelCallsDB.h"
#include "entryCallsDB.h"
#ifndef MAX_LINE
#include "data.h"
#endif

#include "tests.h"

#include "dataImageDB.h"/* temp test*/


/* todo next eli setup extern and entry databases */


boolean legitFileName(char *name);

boolean firstPass(FILE *sourceFile, int *ICFPtr, int *DCFPtr, void **databasePointers);


int main(int argc, char *argv[]){
    int ICF, DCF, i;
    boolean generalError;
    FILE *sourceFile;
    void *databasePointers[DATABASE_POINTER_ARRAY_SIZE];

    if(argc < 2){/* no files to compile */
        /* todo print error - quit program */
    }

    /* initialize databases */
    databasePointers[LABELS_POINTER] = initLabelsDB();
    databasePointers[OPERATIONS_POINTER] = setOperations();
    databasePointers[DATA_IMAGE_POINTER] = initDataImageDB();
    databasePointers[CODE_IMAGE_POINTER] = initCodeImage();
    databasePointers[LABEL_CALLS_POINTER] = initLabelCallsDB();
    databasePointers[ENTRY_CALLS_POINTER] = initEntryCallsDB();


    /* test zone*/
    testGetStringFromLine();
    /*testGetStringFromLine();
    testGetNumbersFromLine();*/
    testExtractOperands(databasePointers[LABEL_CALLS_POINTER]);
    /* end of test zone */


    for(i = 1; i < argc; i++){
        if(legitFileName(argv[i])){
            sourceFile = fopen(argv[i], "r");
        }
        else{
            /* TODO print error */
            continue;
        }

        if(!sourceFile){/* couldn't open file */
            /* TODO print error */
            continue;
        }
        else{
            generalError = firstPass(sourceFile, &ICF, &DCF, databasePointers);

            if(generalError){/* error happened during first pass  */
                /* TODO print error - maybe continue*/
            }
            else{
                /* TODO send filename to second pass, return generalError */
                if(generalError){/* error happened during second pass  */
                    /* TODO print error */
                }
                else{
                    /* TODO make output files */
                }
            }
        }
    }

    return 0;
}


/* TODO test function */
boolean legitFileName(char *name){
    char *c;

    /* seek . in name */
    for(c = name; *c && *c != '.'; c++)
        ;

    if(*c == '.' && *(c+1) == 'a' && *(c+2) == 's') {
        return TRUE;
    }
    else{
        return FALSE;
    }

}


boolean firstPass(FILE *sourceFile, int *ICFPtr, int *DCFPtr, void **databasePointers) {
    /* buffers */
    char line[LINE_ARRAY_SIZE];/* used to load one line from file */
    char command[TOKEN_ARRAY_SIZE];/* extracted command name for each line */
    char string[TOKEN_ARRAY_SIZE];/* extracted string */
    long numbers[NUMBERS_ARRAY_SIZE];/* list of numbers */
    char label[TOKEN_ARRAY_SIZE];/* store label name if a label is declared or used in entry\extern */

    /* counters */
    int IC = STARTING_ADDRESS;
    int DC = 0;/* code and data image counters */
    int lineIndex;/* counter for current index in line array */
    int lineCounter;/* number of current line */

    /* current line identifiers */
    char *currentPos;/* keep track of current position in line buffer */
    int amountOfNumbers;/* when reading numbers array from input, count how many read */
    int reg1, reg2, reg3;/* for commands with register type operands, stores number of register */
    long immed;/* for commands with immed encoding, store immed value */
    boolean jIsReg;/* for J type commands, track if parameter used is register or not(if FALSE - parameter was label) */
    commandOps opCode;/* if relevant, stores opcode */
    functValues funct;/* if relevant, stores funct */
    operationClass commandOpType;/* if relevant, stores type of operation by parameters */
    dataOps dataOpType;/* if command is data type */
    errorCodes lineError;/* type of error in current line, if present */
    boolean generalError = FALSE;/* flag if error found in current file */
    boolean labelDefinition;/* flag if current line defines a label */


    /* reset file wide counters and flags */
    /*IC = STARTING_ADDRESS;
    DC = 0;
    generalError = FALSE;
    */

    /* todo check condition */
    for(lineCounter = 1; !feof(sourceFile); lineCounter++){
        fgets(line, MAX_LINE, sourceFile);/* TODO check if MAX_LINE + 1 */

        if(ignoreLine(line)){/* empty line or comment */
            continue;
        }

        /* reset 'per line' pointers, counters and flags */
        currentPos = line;
        lineIndex = 0;
        lineError = NO_ERROR;
        labelDefinition = FALSE;

        /* check if current line includes label definition */
        if(isLabelDefinition(&currentPos, label)){
            if(seekOp(databasePointers[OPERATIONS_POINTER], label)){/* label name is operation name */
                /* todo print error LABEL_IS_OPERATION */
                generalError = TRUE;
            }
            else{/* possibly legal label declaration */
                labelDefinition = TRUE;
            }
        }

        /* try to read command name */
        if(!extractCommandName(line, &lineIndex, command, labelDefinition, &lineError)){
            /* todo print error */
            generalError = TRUE;
            continue;
        }

        if(*command == '.'){/* line is data command */
            if(!seekDataOp(command, &dataOpType)){/* data command name not found */
                /* todo print error continue */
                generalError = TRUE;
            }
            else{/* legal data command name */
                if(dataOpType == ENTRY || dataOpType == EXTERN){
                    /* read label operand */
                    if(getLabel(&currentPos, label, &lineError)){
                        if(dataOpType == ENTRY){
                            addEntryCall(databasePointers[ENTRY_CALLS_POINTER], label, &lineError);
                        }
                        else{/* extern declaration */
                            addNewLabel(databasePointers[LABELS_POINTER],
                                        label, EXTEN_LABEL_VALUE, EXTERN_LABEL, &lineError);
                        }
                    }
                    else{
                        /* todo print error */
                    }
                }
                else{
                    /* add new label if label definition is present */
                    if(labelDefinition){
                        addNewLabel(databasePointers[LABELS_POINTER], label, DC, DATA_LABEL, &lineError);
                    }
                    if(dataOpType == ASCIZ){
                        if(!getStringFromLine(line, &lineIndex, string, &lineError)){
                            /* todo print error */
                            generalError = TRUE;
                        }
                        else{/* string read successfully */
                            if(!addString(databasePointers[DATA_IMAGE_POINTER],
                                          &DC, string)){/* can't add to database */
                                /* todo print error quit program */
                                generalError = TRUE;
                            }
                        }
                    }
                    else if(dataOpType == DB || dataOpType == DH || dataOpType == DW){
                        amountOfNumbers = getNumbersFromLine(line, &lineIndex, numbers, dataOpType, &lineError);
                        if(!amountOfNumbers){/* no numbers read - error occurred */
                            /* todo print error */
                            generalError = TRUE;
                        }
                        else{/* numbers read successfully */
                            if(!addNumberArray(databasePointers[DATA_IMAGE_POINTER],
                                               &DC, numbers, amountOfNumbers, dataOpType)){/* cannot add to data image */
                                /* todo print error */
                                generalError = TRUE;
                            }
                        }
                    }
                    else{
                        /* todo print error impossible scenario */
                    }
                }
            }
        }
        else{/* line is operation command */
            if(!getOpcode(databasePointers[OPERATIONS_POINTER],
                          command, &opCode, &funct, &commandOpType)){/* operation command not found */
                /* todo print error continue */
                generalError = TRUE;
            }
            else{/* legal operation command name */
                /* add new label if label definition is present */
                if(labelDefinition){
                    addNewLabel(databasePointers[LABELS_POINTER], label, IC, CODE_LABEL, &lineError);
                }
                /* get operation operands */
                if(extractOperands(line, &lineIndex, commandOpType, IC,
                                   &jIsReg, &reg1, &reg2, &reg3, &immed,
                                   &lineError, databasePointers[LABEL_CALLS_POINTER]))
                {
                    /* add command to code image */
                    if(commandOpType == R_ARITHMETIC || commandOpType == R_COPY){
                        if(!addRCommand(databasePointers[CODE_IMAGE_POINTER],
                                    &IC, reg1, reg2, reg3, opCode, funct)){
                            /* todo print error memory alloc */
                        }
                    }
                    else if(commandOpType == I_BRANCHING ||
                            commandOpType == I_MEMORY_LOAD ||
                            commandOpType == I_ARITHMETIC)
                    {
                        if(!addICommand(databasePointers[CODE_IMAGE_POINTER],
                                       &IC, reg1, reg2, immed, opCode)){
                            /* todo print error memory alloc */
                        }
                    }
                    else{/* is J command */
                        if(!addJCommand(databasePointers[CODE_IMAGE_POINTER],
                                        &IC, jIsReg, immed, opCode)){
                            /* todo print error memory alloc */
                        }
                    }
                }
                else{/* operator input error */
                    /* todo print error */
                }
            }
        }

        /* todo check extra characters */
        /* todo check line too big */
    }

    *ICFPtr = IC;
    *DCFPtr = DC + IC;

    return generalError;
}/* end firstPass */
