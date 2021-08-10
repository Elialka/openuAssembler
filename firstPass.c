#include <stdio.h>

#include "firstPass.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "codeImageDB.h"
#include "labelCallsDB.h"
#include "entryCallsDB.h"

static boolean clearLine(char *currentPos, errorCodes *lineErrorPtr, FILE *sourceFile);

/* todo split function */
boolean sourceFilePass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, void **databasePointers) {
    /* buffers */
    char line[LINE_ARRAY_SIZE];/* used to load one line from file */
    char command[TOKEN_ARRAY_SIZE];/* extracted command name for each line */
    char string[TOKEN_ARRAY_SIZE];/* extracted string */
    long numbers[NUMBERS_ARRAY_SIZE];/* list of numbers */
    char label[TOKEN_ARRAY_SIZE];/* store label name if a label is declared or used in entry\extern */

    /* counters */
    long IC = STARTING_ADDRESS;
    long DC = 0;/* code and data image counters */
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
                                        label, EXTERN_LABEL_VALUE, EXTERN_LABEL, &lineError);
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

        if(!checkLineTermination(&currentPos, &lineError)){
            /* todo print error */
        }
        if(!clearLine(currentPos, &lineError, sourceFile)){
            /* todo print warning */
        }
    }

    *ICFPtr = IC - STARTING_ADDRESS;
    *DCFPtr = DC;

    if(IC + DC > ADDRESS_MAX_VALUE){
        generalError = TRUE;
        /* todo print error + check if >= instead */
    }

    return !generalError;
}/* end sourceFilePass */


/* todo check clearLine EOF*/
boolean clearLine(char *currentPos, errorCodes *lineErrorPtr, FILE *sourceFile) {
    int currentChar;

    if(*(currentPos) != '\n'){/* source line is longer than maximum supported length */
        for(; (currentChar = fgetc(sourceFile)) != EOF && currentChar != '\n' ; currentPos++)
            ;
        *lineErrorPtr = LINE_TOO_LONG;
        return FALSE;
    }
    return TRUE;


}
