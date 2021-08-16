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

/* todo left to refactor before new firstPass */
/* codeImageDB - refactor functions API to match new format */
/* labelCallsDB - add support for J_CALL_OR_LA */
/* secondPass - add support for J_CALL_OR_LA */
/* tests - maybe remake tests to work with new API */
/* print - start to write error printing function */

/* todo check memory limits 25 bits */

/* todo split function */
boolean firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr) {
    /* buffers */
    char line[LINE_ARRAY_SIZE];/* used to load one line from file */
    char command[COMMAND_ARRAY_SIZE];/* extracted command name for each line */
    char string[TOKEN_ARRAY_SIZE];/* extracted string */
    long numbers[NUMBERS_ARRAY_SIZE];/* list of numbers */
    char label[TOKEN_ARRAY_SIZE];/* store label name if a label is declared or used in entry\extern */

    /* counters */
    long IC = STARTING_ADDRESS;
    long DC = 0;/* code and data image counters */
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
        lineError = NO_ERROR;
        labelDefinition = FALSE;

        /* check if current line includes label definition */
        if(isLabelDefinition(&currentPos, label)){
            if(seekOp(databasesPtr->operationsDB, label) != NOT_FOUND){/* label name is operation name */
                /* todo print error LABEL_IS_OPERATION */
                generalError = TRUE;
            }
            else{/* possibly legal label declaration */
                labelDefinition = TRUE;
            }
        }

        /* try to read command name */
        if((lineError = extractCommandName(&currentPos, command))){
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
                    if(getLabelOperand(currentPos, 0, label)){/* todo does not work after getLabelOperand refactor */
                        if(dataOpType == ENTRY){
                            addEntryCall(databasesPtr->entryCallsDB, label, &lineError);
                        }
                        else{/* extern declaration */
                            addNewLabel(databasesPtr->labelsDB,
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
                        addNewLabel(databasesPtr->labelsDB, label, DC, DATA_LABEL, &lineError);
                    }
                    if(dataOpType == ASCIZ){
                        if(!(lineError = getStringFromLine(&currentPos, string))){
                            /* todo print error */
                            generalError = TRUE;
                        }
                        else{/* string read successfully */
                            if(!addString(&databasesPtr->dataImageDB,
                                          &DC, string)){/* can't add to database */
                                /* todo print error quit program */
                                generalError = TRUE;
                            }
                        }
                    }
                    else if(dataOpType == DB || dataOpType == DH || dataOpType == DW){
                        amountOfNumbers = getNumbersFromLine(&currentPos, numbers, dataOpType, &lineError);
                        if(!amountOfNumbers){/* no numbers read - error occurred */
                            /* todo print error */
                            generalError = TRUE;
                        }
                        else{/* numbers read successfully */
                            if(!addNumberArray(&databasesPtr->dataImageDB,
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
            if(!getOpcode(databasesPtr->operationsDB,
                          command, &opCode, &funct, &commandOpType)){/* operation command not found */
                /* todo print error continue */
                generalError = TRUE;
            }
            else{/* legal operation command name */
                /* add new label if label definition is present */
                if(labelDefinition){
                    addNewLabel(databasesPtr->labelsDB, label, IC, CODE_LABEL, &lineError);
                }
                /* get operation operands */
                /*if(extractOperandsOLD(&currentPos, commandOpType, IC,
                                      &jIsReg, &reg1, &reg2, &reg3, &immed,
                                      &lineError, databasesPtr->labelCallsDB))*/
                if(TRUE)
                {
                    /* add command to code image */
                    if(commandOpType == R_ARITHMETIC || commandOpType == R_COPY){
                        if(!addRCommand(&databasesPtr->codeImageDB,
                                        &IC, reg1, reg2, reg3, opCode, funct)){
                            /* todo print error memory alloc */
                        }
                    }
                    else if(commandOpType == I_BRANCHING ||
                    commandOpType == I_MEMORY_LOAD ||
                    commandOpType == I_ARITHMETIC)
                    {
                        if(!addICommand(&databasesPtr->codeImageDB,
                                        &IC, reg1, reg2, immed, opCode)){
                            /* todo print error memory alloc */
                        }
                    }
                    else{/* is J command */
                        if(!addJCommand(&databasesPtr->codeImageDB,
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
}/* end firstPass */

/* todo make static */
errorCodes extractOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                                  labelCallPtr labelCallsDB, long IC) {
    errorCodes encounteredError = NO_ERROR;
    operandAttributes currentOperand;
    boolean needAnotherOperand = firstOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);

    currentOperand.isLabel = FALSE;/* reset flag */

    if(needAnotherOperand){/* an operand is needed */
        encounteredError = getFirstOperand(currentPosPtr, commandOpType, &currentOperand);
    }

    /* read second operand */
    if(!encounteredError && needAnotherOperand){/* first operand read successfully */
        needAnotherOperand = secondOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);
        if(needAnotherOperand){/* second operand needed */
            encounteredError = readComma(currentPosPtr);
            if(!encounteredError){/* comma present */
                encounteredError = getSecondOperand(currentPosPtr, commandOpType, &currentOperand);
            }
        }
    }

    /* read third operand */
    if(!encounteredError && needAnotherOperand){/* second operand read successfully */
        needAnotherOperand = thirdOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);
        if(needAnotherOperand){/* third operand needed */
            encounteredError = readComma(currentPosPtr);
            if(!encounteredError){/* comma present */
                encounteredError = getThirdOperand(currentPosPtr, commandOpType, &currentOperand);
            }
        }
    }

    if(!encounteredError && currentOperand.isLabel){/* a label has been used as a parameter */
        encounteredError = setLabelCall(labelCallsDB, IC, currentOperand.labelName, commandOpType);
    }

    return encounteredError;
}




/* todo check clearLine EOF*/
boolean clearLine(char *currentPos, errorCodes *lineErrorPtr, FILE *sourceFile) {
    boolean result = TRUE;
    int currentChar;

    if(*(currentPos) != '\n'){/* source line is longer than maximum supported length */
        for(; (currentChar = fgetc(sourceFile)) != EOF && currentChar != '\n' ; currentPos++)
            ;
        *lineErrorPtr = LINE_TOO_LONG;
        result = FALSE;
    }

    return result;
}
