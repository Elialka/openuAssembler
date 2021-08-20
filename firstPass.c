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
    char labelName[LABEL_ARRAY_SIZE];/* if a new label is defined, store new name */
    boolean labelIsUsed;/* track if a label definition or call occurred */
    labelClass labelType;
}labelAttributes;

typedef struct operationAttributes *operationAttributesPtr;

typedef struct operationAttributes{
    lineType currentLineClass;/* track if current line is command or data line */
    union{
        struct{
            opcodes opcode;
            functValues funct;
            operationClass class;
        }commandOpData;
        dataOps dataOpType;
    }operationID;
}operationAttributes;

typedef struct lineAttributes * lineAttributesPtr;

typedef struct lineAttributes{
    long lineCounter;
    char line[LINE_ARRAY_SIZE];
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
 * Read label definition (if present) and command name, set operation attributes,
 * and add label to database (if present)
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr pointer to operation attributes structure
 * @param IC code image counter
 * @param DC data image counter
 * @param databasesPtr pointer to databases structure
 * @return TRUE if no errors occurred, FALSE otherwise
 */
static errorCodes
handleLabelAndCommandName(char **currentPosPtr, operationAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
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
encodeCodeCommand(char **currentPosPtr, operationAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
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
encodeDataCommand(char **currentPosPtr, operationAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
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
 * Check if a label is defined in current line, update line attributes accordingly
 * @param currentPosPtr Pointer to position in line array
 * @param definedLabelDataPtr pointer to struct - current line attributes
 * @param operationsDB pointer to operations database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes checkLabelDefinition(char **currentPosPtr, labelAttributesPtr definedLabelDataPtr, operationPtr operationsDB);

/**
 * Read command name, set relevant attributes in line attributes structures.
 * Print error messages, if any encountered
 * @param currentPosPtr Pointer to position in line array
 * @param operationDataPtr  pointer to operation attributes structure
 * @param definedLabelDataPtr pointer to label attributes structure
 * @param operationsDB pointer to operations database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes getLineType(char **currentPosPtr, operationAttributesPtr operationDataPtr, labelAttributesPtr definedLabelDataPtr,
                              operationPtr operationsDB);

static void checkRedundantLabel(labelAttributesPtr definedLabelDataPtr, operationAttributesPtr operationDataPtr,
                                lineAttributesPtr lineDataPtr);

/**
 * Add new label to labels database
 * @param definedLabelDataPtr pointer to label attributes structure
 * @param IC image counter value
 * @param DC data counter value
 * @param labelsDB pointer to labels database
 * @return errorCodes enum value describing function success/failure
 */
static errorCodes addLabel(labelAttributesPtr definedLabelDataPtr, long IC, long DC, labelPtr labelsDB);

static errorCodes extractCodeOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                               lineAttributesPtr lineDataPtr, labelCallPtr labelCallsDB);

static void resetCodeLineAttributes(codeLineData *currentLineDataPtr, operationClass commandOpType);

/**
 * Check if first operand is needed, and extract relevant information from it
 * @param currentPosPtr Pointer to position in line array
 * @param commandOpType type of command
 * @param currentLineDataPtr pointer to structure to set operand attributes
 * @param needAnotherPtr flag turns on if current operand is needed
 * @param calledLabelPtr pointer to label structure to copy label name if used
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
 * @param calledLabelPtr pointer to label structure to copy label name if used
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
static void flushLine(FILE *sourceFile, char *line, long lineCounter);


boolean firstPass(FILE *sourceFile, long *ICFPtr, long *DCFPtr, databaseRouterPtr databasesPtr){

    long IC = STARTING_ADDRESS;/* next code line address */
    long DC = CODE_AND_DATA_IMAGE_MARGIN;/* next data line address */
    boolean result = encodeFile(sourceFile, &IC, &DC, databasesPtr);

    *ICFPtr = IC - STARTING_ADDRESS;
    *DCFPtr = DC;

    if(result && IC + DC > ADDRESS_MAX_VALUE){

        result = FALSE;
    }

    return result;
}


static boolean encodeFile(FILE *sourceFile, long *ICPtr, long *DCPtr, databaseRouterPtr databasesPtr){
    lineAttributes currentLineData;
    lineAttributesPtr lineDataPtr = &currentLineData;
    boolean result = TRUE;
    errorCodes encounteredError = NO_ERROR;

    lineDataPtr->ICPtr = ICPtr;
    lineDataPtr->DCPtr = DCPtr;

    for(currentLineData.lineCounter = 1;
    fgets(currentLineData.line, MAX_LINE, sourceFile);/* todo check if maxLine + 1 */
    currentLineData.lineCounter++){/* there is another line */

        if(needToReadLine(currentLineData.line)){/* line should be analysed */
            encounteredError = readLine(&currentLineData, databasesPtr);
        }

        flushLine(sourceFile, currentLineData.line, currentLineData.lineCounter);

        if(encounteredError){
            printErrorMessage(encounteredError, currentLineData.line, currentLineData.lineCounter);
            result = FALSE;
        }
    }

    return result;
}

/* todo change line to line data struct */
static errorCodes readLine(lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr) {
    char *currentPos = lineDataPtr->line;/* position in line array */
    operationAttributes operationData;/* current line attributes */
    errorCodes encounteredError;

    encounteredError = handleLabelAndCommandName(&currentPos, &operationData, lineDataPtr, databasesPtr);

    if(!encounteredError){
        if(operationData.currentLineClass == CODE_TYPE){
            encounteredError = encodeCodeCommand(&currentPos, &operationData, lineDataPtr, databasesPtr);
        }
        else if(operationData.currentLineClass == DATA_TYPE){
            encounteredError = encodeDataCommand(&currentPos, &operationData, lineDataPtr, databasesPtr);
        }
    }

    if(!encounteredError){
        encounteredError = checkLineTermination(&currentPos);
    }

    return encounteredError;
}


static errorCodes
handleLabelAndCommandName(char **currentPosPtr, operationAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                          databaseRouterPtr databasesPtr) {
    labelAttributes definedLabelData;
    errorCodes encounteredError = checkLabelDefinition(currentPosPtr, &definedLabelData, databasesPtr->operationsDB);

    if(!encounteredError){

        encounteredError = getLineType(currentPosPtr, operationDataPtr, &definedLabelData, databasesPtr->operationsDB);
    }

    if(!encounteredError){
        checkRedundantLabel(&definedLabelData, operationDataPtr, lineDataPtr);
    }

    if(!encounteredError){
        encounteredError = addLabel(&definedLabelData, *lineDataPtr->ICPtr, *lineDataPtr->DCPtr, databasesPtr->labelsDB);
    }

    return encounteredError;
}


static errorCodes encodeCodeCommand(char **currentPosPtr, operationAttributesPtr operationDataPtr,
                                    lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr) {
    operationClass commandOpType = operationDataPtr->operationID.commandOpData.class;
    codeLineData currentLineData;/* structure containing all operation data relevant for encoding */
    errorCodes encounteredError;

    resetCodeLineAttributes(&currentLineData, commandOpType);

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
encodeDataCommand(char **currentPosPtr, operationAttributesPtr operationDataPtr, lineAttributesPtr lineDataPtr,
                  databaseRouterPtr databasesPtr) {
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


static errorCodes
externOrEntry(char **currentPosPtr, dataOps dataOpType, lineAttributesPtr lineDataPtr, databaseRouterPtr databasesPtr) {
    labelAttributes definedLabelData;
    errorCodes encounteredError = getLabelFromLine(currentPosPtr, definedLabelData.labelName);

    if(!encounteredError){
        if(dataOpType == ENTRY){
            encounteredError = addEntryCall(databasesPtr->entryCallsDB, definedLabelData.labelName,
                                            lineDataPtr->line, lineDataPtr->lineCounter);
        }
        else{/* must be extern */
            definedLabelData.labelType = EXTERN_LABEL;
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


static errorCodes readString(char **currentPosPtr, long *DCPtr, dataImagePtr *dataImageDBPtr) {
    char string[LINE_ARRAY_SIZE];
    errorCodes encounteredError = getStringFromLine(currentPosPtr, string);

    if(!encounteredError){
        encounteredError = addString(dataImageDBPtr, DCPtr, string);
    }

    return encounteredError;
}


static errorCodes checkLabelDefinition(char **currentPosPtr, labelAttributesPtr definedLabelDataPtr, operationPtr operationsDB) {
    errorCodes encounteredError = NO_ERROR;

    if(isLabelDefinition(currentPosPtr, definedLabelDataPtr->labelName, &encounteredError)){/* label is defined */
        if(seekOp(operationsDB, definedLabelDataPtr->labelName) == NOT_FOUND){/* label name is not operation name */
            definedLabelDataPtr->labelIsUsed = TRUE;
        }
        else{/* label name is operation name */
            encounteredError = LABEL_IS_OPERATION;
        }
    }
    else{/* no label definition */
        definedLabelDataPtr->labelIsUsed = FALSE;
    }

    return encounteredError;
}

/* todo maybe split\organize */
static errorCodes getLineType(char **currentPosPtr, operationAttributesPtr operationDataPtr,
                              labelAttributesPtr definedLabelDataPtr, operationPtr operationsDB) {
    char command[COMMAND_ARRAY_SIZE];/* command name buffer */
    opcodes *opcodePtr = &operationDataPtr->operationID.commandOpData.opcode;/* address where to store opcode */
    functValues *functPtr = &operationDataPtr->operationID.commandOpData.funct;/* address where to store funct */
    operationClass *classPtr = &operationDataPtr->operationID.commandOpData.class;/* address where to store command class */
    errorCodes encounteredError = extractCommandName(currentPosPtr, command);

    if(!encounteredError){/* extracted command name */
        if(getOpcode(operationsDB, command, opcodePtr, functPtr, classPtr)){/* is code line */
            operationDataPtr->currentLineClass = CODE_TYPE;
            definedLabelDataPtr->labelType = CODE_LABEL;
        }
        else if(seekDataOp(command, &operationDataPtr->operationID.dataOpType)){/* is data line */
            operationDataPtr->currentLineClass = DATA_TYPE;
            definedLabelDataPtr->labelType = DATA_LABEL;
        }
        else{/* unidentified line type */
            operationDataPtr->currentLineClass = UNIDENTIFIED_COMMAND;
            definedLabelDataPtr->labelType = UNIDENTIFIED_LABEL_TYPE;
            encounteredError = UNIDENTIFIED_OPERATION_NAME;
        }
    }

    return encounteredError;
}


static void checkRedundantLabel(labelAttributesPtr definedLabelDataPtr, operationAttributesPtr operationDataPtr,
                                      lineAttributesPtr lineDataPtr){
    if(operationDataPtr->currentLineClass == DATA_TYPE){/* is data instruction line */

        if(definedLabelDataPtr->labelIsUsed){/* label definition present */

            if(operationDataPtr->operationID.dataOpType == ENTRY ||
            operationDataPtr->operationID.dataOpType == EXTERN){

                /* redundant label definition - ignore */
                printWarningMessage(DEFINED_LABEL_ENTRY_EXTERN,
                                    lineDataPtr->line, lineDataPtr->lineCounter);
                definedLabelDataPtr->labelIsUsed = FALSE;
            }
        }
    }
}


static errorCodes addLabel(labelAttributesPtr definedLabelDataPtr, long IC, long DC, labelPtr labelsDB){
    errorCodes encounteredError = NO_ERROR;
    long address = 0;/* definition address of the label */

    if(definedLabelDataPtr->labelIsUsed){
        if(definedLabelDataPtr->labelType == CODE_LABEL){
            address = IC;
        }
        else if(definedLabelDataPtr->labelType == DATA_LABEL){
            address = DC;
        }
        else if(definedLabelDataPtr->labelType == EXTERN_LABEL){
            address = EXTERN_LABEL_VALUE;
        }
        encounteredError = addNewLabel(labelsDB, definedLabelDataPtr->labelName,
                                       address, definedLabelDataPtr->labelType);
    }

    return encounteredError;
}


static errorCodes extractCodeOperands(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                               lineAttributesPtr lineDataPtr, labelCallPtr labelCallsDB) {
    errorCodes encounteredError;
    labelAttributes calledLabel;
    boolean mayNeedAnotherOperand;

    calledLabel.labelIsUsed = FALSE;/* reset flag */

    encounteredError = handleFirstOperand(currentPosPtr, commandOpType, currentLineDataPtr,
                                          &mayNeedAnotherOperand, &calledLabel);

    if(!encounteredError && mayNeedAnotherOperand){/* first operand read successfully */
        encounteredError = handleSecondOperand(currentPosPtr, commandOpType, currentLineDataPtr,
                                               &mayNeedAnotherOperand);
    }

    if(!encounteredError && mayNeedAnotherOperand){/* second operand read successfully */
        encounteredError = handleThirdOperand(currentPosPtr, commandOpType, currentLineDataPtr, &calledLabel);
    }

    /* mark label usage */
    if(!encounteredError && calledLabel.labelIsUsed){
        encounteredError = setLabelCall(labelCallsDB, *lineDataPtr->ICPtr,calledLabel.labelName,
                                        commandOpType, lineDataPtr->line, lineDataPtr->lineCounter);
    }

    return encounteredError;
}


static void resetCodeLineAttributes(codeLineData *currentLineDataPtr, operationClass commandOpType) {
    /* reset structure attributes */
    if(commandOpType == R_ARITHMETIC || commandOpType == R_COPY){
        /* is R type */
        currentLineDataPtr->rAttributes.opcode = 0;
        currentLineDataPtr->rAttributes.rt = 0;
        currentLineDataPtr->rAttributes.rs = 0;
        currentLineDataPtr->rAttributes.rd = 0;
        currentLineDataPtr->rAttributes.funct = 0;
    }
    else if(commandOpType == I_ARITHMETIC || commandOpType == I_BRANCHING || commandOpType == I_MEMORY_LOAD){
        /* is I type */
        currentLineDataPtr->iAttributes.immed = 0;
        currentLineDataPtr->iAttributes.rs = 0;
        currentLineDataPtr->iAttributes.rt = 0;
        currentLineDataPtr->iAttributes.opcode = 0;
    }
    else if(commandOpType == J_JMP || commandOpType == J_CALL_OR_LA || commandOpType == J_STOP){
        /* is J type */
        currentLineDataPtr->jAttributes.isReg = FALSE;
        currentLineDataPtr->jAttributes.opcode = 0;
        currentLineDataPtr->jAttributes.address = 0;
    }
}


static errorCodes handleFirstOperand(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                              boolean *needAnotherPtr, labelAttributes *calledLabelPtr){
    errorCodes encounteredError = NO_ERROR;
    operandAttributes currentOperand;
    boolean operandIsNeeded = firstOperandFormat(commandOpType, currentLineDataPtr, &currentOperand);

    currentOperand.isLabel = FALSE;/* reset parameter is label flag */

    if(operandIsNeeded){
        encounteredError = getFirstOperand(currentPosPtr, commandOpType, &currentOperand);
    }

    if((calledLabelPtr->labelIsUsed = currentOperand.isLabel)){/* label is used */
        strcpy(calledLabelPtr->labelName, currentOperand.labelName);
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
                              labelAttributes *calledLabelPtr) {
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

    if(currentOperand.isLabel){/* label is used */
        calledLabelPtr->labelIsUsed = TRUE;
        strcpy(calledLabelPtr->labelName, currentOperand.labelName);
    }

    return encounteredError;
}


/* todo check flushLine EOF*/
void flushLine(FILE *sourceFile, char *line, long lineCounter) {
    char *currentPos = line;
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
            printWarningMessage(LINE_TOO_LONG, line, lineCounter);
        }
    }
}




/* temp - delete - test */
errorCodes extractCodeOperandsTest(char **currentPosPtr, operationClass commandOpType, codeLineData *currentLineDataPtr,
                                   long *ICPtr, labelCallPtr labelCallsDB){
    lineAttributes lineData;
    lineData.ICPtr = ICPtr;
    return extractCodeOperands(currentPosPtr, commandOpType, currentLineDataPtr, &lineData,labelCallsDB);
}
