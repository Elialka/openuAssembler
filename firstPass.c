#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "firstPass.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "pandas.h"
#include "dataImageDB.h"
#include "codeImageDB.h"
#include "labelCallsDB.h"
#include "entryCallsDB.h"


typedef struct labelAttributes *labelAttributesPtr;

typedef struct labelAttributes{/* todo maybe reset flag each line */
    union{
        definedLabel defined;
        labelCall called;
    }data;
    boolean labelIsUsed;/* based on context, track if a labelsDB definition or call occurred */
}labelAttributes;

typedef struct commandAttributes *commandAttributesPtr;

typedef struct commandAttributes{
    commandType lineType;/* track if current line is code or data line */
    union{
        struct{
            opcodes opcode;
            functValues funct;
            operationClass class;
        }commandOpData;
        dataOps dataOpType;
    }operationID;
}commandAttributes;

typedef struct lineAttributes * lineAttributesPtr;

typedef struct lineAttributes{
    lineID lineId;
    long *ICPtr;
    long *DCPtr;
}lineAttributes;


/**
 * Read file, encode command and add relevant data to databases
 * @param sourceFile pointer to file
 * @param ICPtr pointer to final code image counter value
 * @param DCPtr pointer to final data image counter value
 * @param databasesPtr pointer to databases structure
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static boolean encodeFile(FILE *sourceFile, long *ICPtr, long *DCPtr, databaseRouterPtr databasesPtr);

static errorCodes readLine(lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr);

/**
 * Read labelsDB definition (if present) and command name, set operation attributes,
 * and add labelsDB to database (if present)
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr pointer to operation attributes structure
 * @param IC code image counter
 * @param DC data image counter
 * @param databasesPtr pointer to databases structure
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static errorCodes
handleLabelAndCommandName(char **currentPosPtr, commandAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                          databaseRouterPtr databasesPtr);
/**
 * Handle encoding for code operation commands,  print relevant error message if encountered
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr pointer to operation attributes structure
 * @param ICPtr pointer to data image counter
 * @param databasesPtr pointer to databases structure
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static errorCodes
encodeCodeCommand(char **currentPosPtr, commandAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                  databaseRouterPtr databasesPtr);
/**
 * Handle instructions that are not code operations, print relevant error message if encountered
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr pointer to operation attributes structure
 * @param DCPtr pointer to data image counter
 * @param databasesPtr pointer to databases structure
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static errorCodes
encodeDataCommand(char **currentPosPtr, commandAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                  databaseRouterPtr databasesPtr);
/**
 * Handle extern and entry instructions
 * @param currentPosPtr Pointer to position in line array
 * @param dataOpType type of instruction
 * @param DCPtr pointer to data image counter
 * @param databasesPtr pointer to databases structure
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes
externOrEntry(char **currentPosPtr, dataOps dataOpType, lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr);
/**
 * Handle dw, dh, and db instructions
 * @param currentPosPtr Pointer to position in line array
 * @param dataOpType type of instruction
 * @param DCPtr pointer to data image counter
 * @param dataImageDBPtr pointer to data image database pointer
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes readNumbers(char **currentPosPtr, dataOps dataOpType, long *DCPtr, dataImagePtr *dataImageDBPtr);

/**
 * Handle asciz instruction
 * @param currentPosPtr Pointer to position in line array
 * @param DCPtr pointer to data image counter
 * @param dataImageDBPtr pointer to data image database pointer
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes readString(char **currentPosPtr, long *DCPtr, dataImagePtr *dataImageDBPtr);

/**
 * Check if a labelsDB is defined in current line, update line attributes accordingly
 * @param currentPosPtr Pointer to position in line array
 * @param definedLabelDataPtr pointer to struct - current line attributes
 * @param operationsDB pointer to operations database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes checkLabelDefinition(char **currentPosPtr, labelAttributesPtr definedLabelDataPtr, operationsDBPtr operationsDB);

/**
 * Read command name, set relevant attributes in line attributes structures.
 * Print error messages, if any encountered
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr  pointer to operation attributes structure
 * @param labelTypePtr pointer to labelsDB attributes structure
 * @param operationsDB pointer to operations database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes getLineType(char **currentPosPtr, commandAttributesPtr operationDataPtr, labelType *labelTypePtr,
                              operationsDBPtr operationsDB);

static void checkRedundantLabel(labelAttributesPtr definedLabelDataPtr, commandAttributesPtr operationDataPtr,
                                lineAttributesPtr lineDataPtr);

/**
 * Add new labelsDB to labels database
 * @param definedLabelDataPtr pointer to labelsDB attributes structure
 * @param IC image counter value
 * @param DC data counter value
 * @param labelsDB pointer to labels database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes addLabel(labelAttributesPtr definedLabelDataPtr, long IC, long DC, labelsDBPtr labelsDB);

static errorCodes extractCodeOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                                      lineAttributesPtr lineDataPtr, labelCallsDBPtr labelCallsDB);

/**
 * Check if first operand is needed, and extract relevant information from it
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of command
 * @param currentLineDataPtr pointer to structure to set operand attributes
 * @param needAnotherPtr flag turns on if current operand is needed
 * @param calledLabelPtr pointer to labelsDB structure to copy labelsDB name if used
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes handleFirstOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                              boolean *needAnotherPtr, labelAttributes *calledLabelPtr);
/**
 * Check if second operand is needed, and extract relevant information from it
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of command
 * @param currentLineDataPtr pointer to structure to set operand attributes
 * @param needAnotherPtr flag turns on if current operand is needed
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes handleSecondOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                               boolean *needAnotherPtr);
/**
 * Check if third operand is needed, and extract relevant information from it
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of command
 * @param currentLineDataPtr pointer to structure to set operand attributes
 * @param calledLabelPtr pointer to labelsDB structure to copy labelsDB name if used
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes handleThirdOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                              labelAttributes *calledLabelPtr);

/**
 * Check for extraneous text in the end of the line, and ensure line was not longer than max supported length
 * @param currentLineData Position in line array
 * @param sourceFile pointer to file
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static void flushLine(FILE *sourceFile, lineID *lineIdPtr);


boolean firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr){
    /* reset image counters */
    long IC = STARTING_ADDRESS;
    long DC = CODE_AND_DATA_IMAGE_MARGIN;

    boolean result = encodeFile(sourceFile, &IC, &DC, databasesPtr);

    *ICFPtr = IC - STARTING_ADDRESS;
    *DCFPtr = DC;

    if(result && IC + DC > ADDRESS_MAX_VALUE){/* total memory image size exceeds machine capabilities */
        /* todo check if error is printed */
        result = FALSE;
    }

    return result;
}


static boolean encodeFile(FILE *sourceFile, long *ICPtr, long *DCPtr, databaseRouterPtr databasesPtr){
    lineAttributes currentLineData;/* structure containing line array + counters */
    boolean result = TRUE;
    errorCodes encounteredError = NO_ERROR;

    currentLineData.ICPtr = ICPtr;
    currentLineData.DCPtr = DCPtr;

    for(currentLineData.lineId.count = 1;
    fgets(currentLineData.lineId.line, MAX_LINE, sourceFile);/* todo check if maxLine + 1 */
    currentLineData.lineId.count++){/* there is another line */

        if(needToReadLine(currentLineData.lineId.line)){/* line should be analysed */
            encounteredError = readLine(&currentLineData, databasesPtr);
        }

        flushLine(sourceFile, &currentLineData.lineId);

        if(encounteredError){
            printErrorMessage(encounteredError, &currentLineData.lineId);
            result = FALSE;
        }
    }

    return result;
}


static errorCodes readLine(lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr){
    char *currentPos = lineDataPtr->lineId.line;/* position in line array */
    commandAttributes currCommandAttributes;/* current operation identifiers*/
    errorCodes encounteredError;

    encounteredError = handleLabelAndCommandName(&currentPos, &currCommandAttributes, lineDataPtr, databasesPtr);

    if(!encounteredError){
        if(currCommandAttributes.lineType == OPERATION_LINE){
            encounteredError = encodeCodeCommand(&currentPos, &currCommandAttributes, lineDataPtr, databasesPtr);
        }
        else if(currCommandAttributes.lineType == INSTRUCTION_LINE){
            encounteredError = encodeDataCommand(&currentPos, &currCommandAttributes, lineDataPtr, databasesPtr);
        }
    }

    if(!encounteredError){
        encounteredError = checkLineTermination(&currentPos);
    }

    return encounteredError;
}


static errorCodes
handleLabelAndCommandName(char **currentPosPtr, commandAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                          databaseRouterPtr databasesPtr){
    labelAttributes definedLabelData;/* structure to store defined label attributes, if present */
    errorCodes encounteredError = checkLabelDefinition(currentPosPtr, &definedLabelData, databasesPtr->operationsDB);

    if(!encounteredError){
        encounteredError = getLineType(currentPosPtr, operationDataPtr, &definedLabelData.data.defined.type, databasesPtr->operationsDB);
    }

    if(!encounteredError){
        checkRedundantLabel(&definedLabelData, operationDataPtr, lineDataPtr);
    }

    if(!encounteredError){
        encounteredError = addLabel(&definedLabelData, *lineDataPtr->ICPtr, *lineDataPtr->DCPtr, databasesPtr->labelsDB);
    }

    return encounteredError;
}


static errorCodes encodeCodeCommand(char **currentPosPtr, commandAttributesPtr operationDataPtr,
                                    lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr){
    operationClass commandOpType = operationDataPtr->operationID.commandOpData.class;
    codeLineData currentLineData = {0};/* structure containing all operation data relevant for encoding */
    errorCodes encounteredError;

    encounteredError = extractCodeOperands(currentPosPtr, commandOpType, &currentLineData, lineDataPtr,
                                           databasesPtr->labelCallsDB);

    if(!encounteredError){
        if(commandOpType == R_ARITHMETIC || commandOpType == R_COPY){
            /* is R type */
            currentLineData.rAttributes.opcode = operationDataPtr->operationID.commandOpData.opcode;
            currentLineData.rAttributes.funct = operationDataPtr->operationID.commandOpData.funct;
            encounteredError = addRCommand(&databasesPtr->codeImageDB, lineDataPtr->ICPtr, currentLineData.rAttributes);
        }
        else if(commandOpType == I_BRANCHING || commandOpType == I_MEMORY_LOAD || commandOpType == I_ARITHMETIC){
            /* is I type */
            currentLineData.iAttributes.opcode = operationDataPtr->operationID.commandOpData.opcode;
            encounteredError = addICommand(&databasesPtr->codeImageDB, lineDataPtr->ICPtr, currentLineData.iAttributes);
        }
        else if(commandOpType == J_JMP || commandOpType == J_CALL_OR_LA || commandOpType == J_STOP){
            /* is J type */
            currentLineData.jAttributes.opcode = operationDataPtr->operationID.commandOpData.opcode;
            encounteredError = addJCommand(&databasesPtr->codeImageDB, lineDataPtr->ICPtr, currentLineData.jAttributes);
        }
        else{/* impossible scenario */
            encounteredError = IMPOSSIBLE_ENCODE_CODE;
        }
    }

    return encounteredError;
}



static errorCodes
encodeDataCommand(char **currentPosPtr, commandAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                  databaseRouterPtr databasesPtr){
    errorCodes encounteredError;
    dataOps dataOpType = operationDataPtr->operationID.dataOpType;

    if(dataOpType == EXTERN || dataOpType == ENTRY){
        encounteredError = externOrEntry(currentPosPtr, dataOpType, lineDataPtr, databasesPtr);
    }
    else if(dataOpType == DW || dataOpType == DH || dataOpType == DB){
        encounteredError = readNumbers(currentPosPtr, dataOpType, lineDataPtr->DCPtr, &databasesPtr->dataImageDB);
    }
    else if(dataOpType == ASCIZ){
        encounteredError = readString(currentPosPtr, lineDataPtr->DCPtr, &databasesPtr->dataImageDB);
    }
    else{
        encounteredError = IMPOSSIBLE_ENCODE_DATA;
    }

    return encounteredError;
}


/* todo check function */
static errorCodes
externOrEntry(char **currentPosPtr, dataOps dataOpType, lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr){
    labelAttributes definedLabelData;
    errorCodes encounteredError = getLabelFromLine(currentPosPtr, definedLabelData.data.defined.labelId.name);

    if(!encounteredError){
        if(dataOpType == ENTRY){
            encounteredError = addEntryCall(databasesPtr->entryCallsDB, definedLabelData.data.defined.labelId.name,
                                            lineDataPtr->lineId);
        }
        else{/* must be extern */
            definedLabelData.data.defined.type = EXTERN_LABEL;
            encounteredError = addLabel(&definedLabelData, 888, *lineDataPtr->DCPtr, databasesPtr->labelsDB);
        }
    }

    return encounteredError;
}


static errorCodes readNumbers(char **currentPosPtr, dataOps dataOpType, long *DCPtr, dataImagePtr *dataImageDBPtr){
    int amountOfNumbers;
    long numbers[NUMBERS_ARRAY_SIZE];
    errorCodes encounteredError = NO_ERROR;

    amountOfNumbers = getNumbersFromLine(currentPosPtr, numbers, dataOpType, &encounteredError);
    if(amountOfNumbers){
        encounteredError = addNumberArray(dataImageDBPtr, DCPtr, numbers, amountOfNumbers, dataOpType);
    }

    return encounteredError;
}


static errorCodes readString(char **currentPosPtr, long *DCPtr, dataImagePtr *dataImageDBPtr){
    char string[LINE_ARRAY_SIZE];
    errorCodes encounteredError = getStringFromLine(currentPosPtr, string);

    if(!encounteredError){
        encounteredError = addString(dataImageDBPtr, DCPtr, string);
    }

    return encounteredError;
}


static errorCodes checkLabelDefinition(char **currentPosPtr, labelAttributesPtr definedLabelDataPtr, operationsDBPtr operationsDB){
    errorCodes encounteredError = NO_ERROR;

    if(isLabelDefinition(currentPosPtr, definedLabelDataPtr->data.defined.labelId.name, &encounteredError)){/* labels is defined */
        if(seekOp(operationsDB, definedLabelDataPtr->data.defined.labelId.name) == NOT_FOUND){/* labels name is not operation name */
            definedLabelDataPtr->labelIsUsed = TRUE;
        }
        else{/* labelsDB name is operation name */
            encounteredError = LABEL_IS_OPERATION;
        }
    }
    else{/* no labelsDB definition */
        definedLabelDataPtr->labelIsUsed = FALSE;
    }

    return encounteredError;
}

/* todo maybe split\organize */
static errorCodes getLineType(char **currentPosPtr, commandAttributesPtr operationDataPtr,
                              labelType *labelTypePtr, operationsDBPtr operationsDB){
    char command[COMMAND_ARRAY_SIZE];/* command name buffer */
    opcodes *opcodePtr = &operationDataPtr->operationID.commandOpData.opcode;/* address where to store opcode */
    functValues *functPtr = &operationDataPtr->operationID.commandOpData.funct;/* address where to store funct */
    operationClass *classPtr = &operationDataPtr->operationID.commandOpData.class;/* address where to store operation class */
    errorCodes encounteredError = extractCommandName(currentPosPtr, command);

    if(!encounteredError){/* extracted command name */
        if(getOpcode(operationsDB, command, opcodePtr, functPtr, classPtr)){/* is code line */
            operationDataPtr->lineType = OPERATION_LINE;
            *labelTypePtr = CODE_LABEL;
        }
        else if(seekDataOp(command, &operationDataPtr->operationID.dataOpType)){/* is data line */
            operationDataPtr->lineType = INSTRUCTION_LINE;
            *labelTypePtr = DATA_LABEL;
        }
        else{/* unidentified line type */
            operationDataPtr->lineType = UNIDENTIFIED_COMMAND;
            *labelTypePtr = UNIDENTIFIED_LABEL_TYPE;
            encounteredError = UNIDENTIFIED_OPERATION_NAME;
        }
    }

    return encounteredError;
}


static void checkRedundantLabel(labelAttributesPtr definedLabelDataPtr, commandAttributesPtr operationDataPtr,
                                lineAttributesPtr lineDataPtr){
    if(operationDataPtr->lineType == INSTRUCTION_LINE){/* is data instruction line */

        if(definedLabelDataPtr->labelIsUsed){/* label definition is present */

            if(operationDataPtr->operationID.dataOpType == ENTRY ||
            operationDataPtr->operationID.dataOpType == EXTERN){

                /* redundant labelsDB definition - ignore */
                printWarningMessage(DEFINED_LABEL_ENTRY_EXTERN, &lineDataPtr->lineId);
                definedLabelDataPtr->labelIsUsed = FALSE;
            }
        }
    }
}


static errorCodes addLabel(labelAttributesPtr definedLabelDataPtr, long IC, long DC, labelsDBPtr labelsDB){
    errorCodes encounteredError = NO_ERROR;

    if(definedLabelDataPtr->labelIsUsed){
        /* determine definition address */
        if(definedLabelDataPtr->data.defined.type == CODE_LABEL){
            definedLabelDataPtr->data.defined.labelId.address = IC;
        }
        else if(definedLabelDataPtr->data.defined.type == DATA_LABEL){
            definedLabelDataPtr->data.defined.labelId.address = DC;
        }
        else if(definedLabelDataPtr->data.defined.type == EXTERN_LABEL){
            definedLabelDataPtr->data.defined.labelId.address = EXTERN_LABEL_VALUE;
        }

        /* add label to database */
        encounteredError = addNewLabel(labelsDB, &definedLabelDataPtr->data.defined);
    }

    return encounteredError;
}


static errorCodes extractCodeOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                                      lineAttributesPtr lineDataPtr, labelCallsDBPtr labelCallsDB){
    errorCodes encounteredError;
    labelAttributes calledLabel;/* struct holding information about label operand if was present */
    boolean mayNeedAnotherOperand;/* flag to keep track whether we should check for another operand */

    /* set called label attributes, to be used when adding label call to database if present */
    calledLabel.labelIsUsed = FALSE;/* reset flag */
    calledLabel.data.called.lineId = lineDataPtr->lineId;/* copy input line data */
    calledLabel.data.called.type = commandOpType;
    calledLabel.data.called.labelId.address = *lineDataPtr->ICPtr;

    encounteredError = handleFirstOperand(currentPosPtr, commandOpType, currentLineDataPtr,
                                          &mayNeedAnotherOperand, &calledLabel);

    if(!encounteredError && mayNeedAnotherOperand){/* first operand read successfully */
        encounteredError = handleSecondOperand(currentPosPtr, commandOpType, currentLineDataPtr,
                                               &mayNeedAnotherOperand);
    }

    if(!encounteredError && mayNeedAnotherOperand){/* second operand read successfully */
        encounteredError = handleThirdOperand(currentPosPtr, commandOpType, currentLineDataPtr, &calledLabel);
    }

    /* mark labelsDB usage */
    if(!encounteredError && calledLabel.labelIsUsed){
        encounteredError = addLabelCall(labelCallsDB, &calledLabel.data.called);
    }

    return encounteredError;
}


static errorCodes handleFirstOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                              boolean *needAnotherPtr, labelAttributes *calledLabelPtr){
    errorCodes encounteredError = NO_ERROR;
    operandAttributes currentOperand;
    boolean operandIsNeeded = firstOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);

    currentOperand.isLabel = FALSE;/* reset parameter is labels flag */

    if(operandIsNeeded){
        encounteredError = getFirstOperand(currentPosPtr, commandOpType, &currentOperand);
    }

    if((calledLabelPtr->labelIsUsed = currentOperand.isLabel)){/* labels is used */
        strcpy(calledLabelPtr->data.called.labelId.name, currentOperand.labelName);
    }
    else if(commandOpType == J_JMP){/* register is used with jmp command */
        currentLineDataPtr->jAttributes.isReg = TRUE;
    }

    *needAnotherPtr = operandIsNeeded;
    return encounteredError;
}

static errorCodes handleSecondOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                               boolean *needAnotherPtr){
    errorCodes encounteredError = NO_ERROR;
    operandAttributes currentOperand;
    boolean operandIsNeeded = secondOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);

    if(operandIsNeeded){
        encounteredError = readComma(currentPosPtr);
        if(!encounteredError){/* comma read successfully */
            encounteredError = getSecondOperand(currentPosPtr, commandOpType, &currentOperand);
        }
    }

    *needAnotherPtr = operandIsNeeded;
    return encounteredError;
}

static errorCodes handleThirdOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                              labelAttributes *calledLabelPtr){
    errorCodes encounteredError = NO_ERROR;
    operandAttributes currentOperand;
    boolean operandIsNeeded = thirdOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);

    currentOperand.isLabel = FALSE;/* reset flag */

    if(operandIsNeeded){
        encounteredError = readComma(currentPosPtr);
        if(!encounteredError){/* comma read successfully */
            encounteredError = getThirdOperand(currentPosPtr, commandOpType, &currentOperand);
        }
    }

    if(currentOperand.isLabel){/* labelsDB is used */
        calledLabelPtr->labelIsUsed = TRUE;
        strcpy(calledLabelPtr->data.called.labelId.name, currentOperand.labelName);
    }

    return encounteredError;
}


/* todo check flushLine EOF*/
void flushLine(FILE *sourceFile, lineID *lineIdPtr) {
    char *currentPos = lineIdPtr->line;
    int currentChar;
    boolean missedCharacters = FALSE;/* turns on if any non-white characters are found past supported line limits */

    /* find last character */
    for(; *currentPos; currentPos++)
        ;

    if(*--currentPos != '\n'){/* newline character is not present in line */
        /* flush line by reading characters until newline character or EOF */
        for(; (currentChar = fgetc(sourceFile)) != EOF && currentChar != '\n' ; currentPos++){
            if(!isspace(currentChar)){
                missedCharacters = TRUE;
            }
        }

        if(missedCharacters){/* was not end of file */
            printWarningMessage(LINE_TOO_LONG, lineIdPtr);
        }
    }
}
