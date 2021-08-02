#include <stdio.h>
#include "labelsDB.h"
#include "operationsDB.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "codeImageDB.h"
#ifndef MAX_LINE
#include "data.h"
#endif

#include "tests.h"

#include "dataImageDB.h"/* temp test*/


/* todo next eli setup extern and entry databases */


boolean legitFileName(char *name);

boolean firstPass(FILE *sourceFile, int *ICFPtr, int *DCFPtr);


int main(int argc, char *argv[]){
    int ICF, DCF, i;
    boolean generalError;
    FILE *sourceFile;
    void *databasePointers[DATABASE_POINTER_ARRAY_SIZE];

    if(argc < 2){/* no files to compile */
        /* todo print error - quit program */
    }

    /* initialize databases */
    databasePointers[LABELS_POINTER] = NULL;
    databasePointers[OPERATIONS_POINTER] = setOperations();
    databasePointers[DATA_IMAGE_POINTER] = initDataImageDB();
    databasePointers[CODE_IMAGE_POINTER] = initCodeImage();

    generalError = FALSE;

    /* test zone*/
    testExtractOperands();
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
            generalError = firstPass(sourceFile, &ICF, &DCF);

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


boolean firstPass(FILE *sourceFile, int *ICFPtr, int *DCFPtr){
    /* buffers */
    char line[LINE_ARRAY_SIZE];/* used to load one line from file */
    char command[TOKEN_ARRAY_SIZE];/* extracted command name for each line */
    char string[TOKEN_ARRAY_SIZE];/* extracted string */
    long numbers[NUMBERS_ARRAY_SIZE];/* list of numbers */
    char definedLabel[TOKEN_ARRAY_SIZE];/* if new label is defined, store new label name */

    /* counters */
    int IC, DC;/* code and data image counters */
    int lineIndex;/* counter for current index in line array */
    int lineCounter;/* number of current line */

    /* current line identifiers */
    int amountOfNumbers;/* when reading numbers array from input, count how many read */
    int reg1, reg2, reg3;/* for commands with register type operands, stores number of register */
    long immed;/* for commands with immed encoding, store immed value */
    boolean jumpIsReg;/* for jump command, track if parameter used is register or not (if FALSE - parameter was label) */
    commandOps opCode;/* if relevant, stores opcode */
    functValues funct;/* if relevant, stores funct */
    operationClass commandOpType;/* if relevant, stores type of operation by parameters */
    dataOps dataOpType;/* if command is data type */
    errorCodes lineError;/* type of error in current line, if present */
    boolean generalError;/* flag if error found in current file */
    boolean labelDefinition;/* flag if current line defines a label */
    labelType currLineType;/* stores if current line is code type or data type */


    /* reset file wide counters and flags */
    IC = STARTING_ADDRESS;
    DC = 0;
    generalError = FALSE;


    /* todo check condition */
    for(lineCounter = 1; !feof(sourceFile); lineCounter++){
        fgets(line, MAX_LINE, sourceFile);/* TODO check if MAX_LINE + 1 */

        if(ignoreLine(line)){/* empty line or comment */
            continue;
        }


        /* reset 'per line' counters and flags */
        lineIndex = 0;
        lineError = NO_ERROR;
        labelDefinition = FALSE;
        /* todo reset parameters */

        /* check if current line includes label definition */
        if(isLabelDefinition(line, &lineIndex, definedLabel)){
            if(!legalLabelDeclaration(definedLabel, &lineError)){/* cannot define label with given name */
                /* todo print error */
                generalError = TRUE;
            }
            else{/* new legal label */
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
                currLineType = DATA_LINE;
                if(dataOpType == ASCIZ){
                    if(!getStringFromLine(line, &lineIndex, string, &lineError)){
                        /* todo print error */
                        generalError = TRUE;
                    }
                    else{/* string read successfully */
                        if(!addString(&DC, string)){/* can't add to database */
                            /* todo print error quit program */
                            generalError = TRUE;
                        }
                    }
                }
                else if(dataOpType == ENTRY || dataOpType == EXTERN){
                    labelDefinition = FALSE;
                    /* todo maybe light flag for second pass */
                }
                else if(dataOpType == DB || dataOpType == DH || dataOpType == DW){
                    amountOfNumbers = getNumbersFromLine(line, &lineIndex, numbers, dataOpType, &lineError);
                    if(!amountOfNumbers){/* no numbers read - error occurred */
                        /* todo print error */
                        generalError = TRUE;
                    }
                    else{/* numbers read successfully */
                        if(!addNumberArray(&DC, numbers, amountOfNumbers, dataOpType)){/* cannot add to data image */
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
        else{/* line is operation command */
            if(!getOpcode(command, &opCode, &funct, &commandOpType)){/* operation command not found */
                /* todo print error continue */
                generalError = TRUE;
            }
            else{/* legal operation command name */
                currLineType = CODE_LINE;/* todo add label to DB */
                if(extractOperands(line, &lineIndex, commandOpType, IC,
                                   &jumpIsReg, &reg1, &reg2, &reg3, &immed, &lineError)){
                    /* todo add by type */
                }
            }
        }

        /* todo add label */

        /* todo check extra characters */
        /* todo check line too big */
    }

    *ICFPtr = IC;
    *DCFPtr = DC;
    return generalError;
}/* end firstPass */
