#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "pandas.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "dataImageDB.h"
#include "labelCallsDB.h"

#define SKIP_WHITES(x) for(; isspace(*(x)); (x)++)



/**
 * Count legal label characters
 * @param token Name string
 * @return How many characters read until string ended, or until illegal character encountered
 */
static int countLabelNameCharacters(char *token);


boolean ignoreLine(char *line){
    boolean result;
    char *current;

    current = line;

    SKIP_WHITES(current);

    if(*current){
        if(*current == ';'){/* comment line */
            result =  TRUE;
        }
        else{/* line is not empty */
            result =  FALSE;
        }
    }
    else{/* line is empty */
        result =  TRUE;
    }

    return result;
}


int extractNextToken(char **sourcePtr, char *buffer){
    int length;
    char *currentPos = *sourcePtr;

    SKIP_WHITES(currentPos);
    /* copy token  */
    for(length = 0; *currentPos && !isspace(*currentPos) && *currentPos != ','; currentPos++, length++){
        buffer[length] = *currentPos;
    }

    buffer[length] = '\0';
    *sourcePtr = currentPos;

    return length;
}


static int countLabelNameCharacters(char *token){
    int counter = 0;/* how many consecutive legal name characters */
    char *currentChar = token;/* position in token string */

    if(isalpha(*currentChar)){
        for (; (isalpha(*currentChar) || isdigit(*currentChar));
            currentChar++, counter++)/* legal char */
            ;
    }

    return counter;
}


/* todo maybe change return type to errorCodes */
errorCodes isLabelDefinition(char **currentPosPtr, char *currentLabel) {
    errorCodes encounteredError = NO_ERROR;
    char *currentPos = *currentPosPtr;
    char *definitionEnd;/* point to the end of the definition */
    char token[TOKEN_ARRAY_SIZE];/* store token to examine it */
    int tokenLength = extractNextToken(&currentPos, token);


    if((definitionEnd = strchr(token, ':'))){/* a label is defined */
        *definitionEnd = '\0';/* delete ':' from token */
        tokenLength--;/* update new token length */
        encounteredError = tokenIsLabel(token, tokenLength);/* check if possibly legal name */

        if(!encounteredError){/* possibly legal */
            if(isspace(*currentPos)){/* legal label declaration */
                strcpy(currentLabel, token);/* save label name */
                *currentPosPtr = currentPos;/* update line position */
            }
            else{/* no white character after label definition */
                encounteredError = NO_SPACE_AFTER_LABEL;
            }
        }
    }

    return encounteredError;
}


errorCodes extractCommandName(char **currentPosPtr, char *commandName) {
    errorCodes encounteredError = NO_ERROR;
    char *currentPos = *currentPosPtr;
    char buffer[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(&currentPos, buffer);

    if(!tokenLength){/* no characters read */
        encounteredError = MISSING_OPERATION_NAME;
    }
    else if(tokenLength > MAX_COMMAND_LENGTH){/* impossible length */
        encounteredError = UNIDENTIFIED_OPERATION_NAME;
    }
    else{/* possible command name */
        *currentPosPtr = currentPos;
        strcpy(commandName, buffer);
    }

    return encounteredError;
}


boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue) {
    boolean result = TRUE;
    long value;
    long minValue;/* negative border for number of bits used */

    /* reset global error flag */
    errno = 0;

    /* calculate minimum value */
    minValue = maxValue * (-1);
    minValue--;

    value = strtol(token, endPtrPtr, DECIMAL_BASE);

    /* is value in range */
    if(token != *endPtrPtr && !errno && value <= maxValue && value >= minValue){
        *valuePtr = value;
    }
    else{
        errno = 0;
        result = FALSE;
    }

    return result;
}


errorCodes getStringFromLine(char **currentPosPtr, char *destination) {
    errorCodes encounteredError;
    int i;
    char *current = *currentPosPtr;

    SKIP_WHITES(current);

    /* start of string */
    encounteredError = *current++ == '"' ? NO_ERROR : MISSING_QUOTE;

    /* copy string to string buffer */
    for (i = 0; !encounteredError && *current && *current != '"'; i++, current++)
    {
        if(isprint(*current)){
            destination[i] = *current;
        }
        else{/* only printable characters are allowed */
            encounteredError = ILLEGAL_CHARACTER;
        }
    }

    /* validate closing quotes */
    if (!encounteredError && *current++=='"')
    {
        destination[i]='\0';
        *currentPosPtr = current;
    }
    else
    {
        encounteredError = MISSING_QUOTE;
    }

    return encounteredError;
}


/* todo refactor to shorter function */
int getNumbersFromLine(char **currentPosPtr, long *buffer, dataOps dataOpType, errorCodes *lineErrorPtr) {
    int i;
    long value;
    char number[MAX_LINE];
    int numberLength;
    int numberCounter;
    char *current = *currentPosPtr;
    char *endPtr;
    int maxValue;

    numberCounter = 0;

    /* check maxValue */
    if(dataOpType == DB){
        maxValue = BYTE_MAX_VALUE;
    }
    else if(dataOpType == DH){
        maxValue = HALF_WORD_MAX_VALUE;
    }
    else if(dataOpType == DW){
        maxValue = WORD_MAX_VALUE;
    }
    else{
        *lineErrorPtr = IMPOSSIBLE;
        return FALSE;
    }


    for (i = 0 ; *current; i++) {
        SKIP_WHITES(current);/* todo remove after readComma refactor */

        if (i % 2) {/* expecting a comma */

            if (!*current) /*end of line*//* todo refactor to use readComma */
                {break;}
                if (*current != ',') {
                    *lineErrorPtr = ILLEGAL_EXPRESSION;
                    return FALSE;
                }
                else
                {
                    current++;
                }

        }
        else {/* expecting a number */
            numberLength = extractNextToken(&current, number);
            if(!numberLength){/* end of line */
                if(i){/* already read some numbers */
                    *lineErrorPtr = ILLEGAL_COMMA;
                }
                else{
                    *lineErrorPtr = EXPECTED_NUMBER;
                }
            }

            if (!stringToLong(number, &value, &endPtr, maxValue)) { /*searching for illegal chars in line */
                *lineErrorPtr = ILLEGAL_EXPRESSION;
                return FALSE;
            }
            else if (*endPtr == '.') {
                *lineErrorPtr = NOT_INTEGER;
                return FALSE;
            }
            else if (*endPtr && !(isspace(*endPtr)) && *endPtr != ',' && !(isdigit(*endPtr))){
                *lineErrorPtr = NOT_NUMBER;
                return FALSE;
            }
            else {
                buffer[numberCounter] = value;
                numberCounter++;
            }
        }
    }

    return numberCounter;
}


errorCodes tokenIsLabel(char *token, int tokenLength) {
    errorCodes encounteredError = NO_ERROR;
    int nameLength;

    /* count how many legal characters are in token */
    nameLength = countLabelNameCharacters(token);

    if(nameLength > MAX_LABEL_LENGTH){/* impossible length */
        encounteredError = LABEL_TOO_LONG;
    }
    else if(nameLength < tokenLength){/* illegal characters present */
        encounteredError = ILLEGAL_LABEL_NAME;
    }

    return encounteredError;
}


errorCodes readComma(char **currentPtr) {
    errorCodes encounteredError = NO_ERROR;

    SKIP_WHITES(*currentPtr);

    if(**currentPtr != ','){
        encounteredError = MISSING_COMMA;
    }
    else{/* is a comma */
        (*currentPtr)++;
        SKIP_WHITES(*currentPtr);/* todo maybe remove line */
    }

    return encounteredError;
}


errorCodes getFirstOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr){
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(currentPosPtr, token);
    errorCodes encounteredError = getRegisterOperand(token, operandAttributesPtr->operandData.regPtr);

    if(!tokenLength){
        encounteredError = MISSING_PARAMETER;
    }
    else if(!encounteredError){/* token is register */
        operandAttributesPtr->isLabel = FALSE;
    }
    else if(encounteredError == NOT_REGISTER){
        if (commandOpType == J_JUMP || commandOpType == J_CALL_OR_LA){/* is possibly a label */
            encounteredError = getLabelOperand(token, tokenLength, operandAttributesPtr->labelName);
            operandAttributesPtr->isLabel = TRUE;
        }
        else{
            encounteredError = EXPECTED_REGISTER;
        }
    }

    if(commandOpType == J_CALL_OR_LA && !operandAttributesPtr->isLabel){
        /* this command type does not support register operands */
        encounteredError = EXPECTED_LABEL;
    }

    return encounteredError;
}


errorCodes getSecondOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr){
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(currentPosPtr, token);
    errorCodes encounteredError;

    if(!tokenLength){
        encounteredError = MISSING_PARAMETER;
    }
    else if(commandOpType != I_ARITHMETIC && commandOpType != I_MEMORY_LOAD){/* need a register */
        encounteredError = getRegisterOperand(token, operandAttributesPtr->operandData.regPtr);
        if(encounteredError == NOT_REGISTER){
            encounteredError = EXPECTED_REGISTER;
        }
    }
    else{/* need an immediate value */
        encounteredError = getNumberOperand(token, operandAttributesPtr->operandData.immedPtr);
        if(encounteredError == NOT_NUMBER){
            encounteredError = EXPECTED_NUMBER;
        }
    }

    return encounteredError;
}


errorCodes getThirdOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr){
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(currentPosPtr, token);
    errorCodes encounteredError;

    if(!tokenLength){
        encounteredError = MISSING_PARAMETER;
    }
    else if(commandOpType != I_BRANCHING){/* need a register */
        encounteredError = getRegisterOperand(token, operandAttributesPtr->operandData.regPtr);
        if(encounteredError == NOT_REGISTER){
            encounteredError = EXPECTED_REGISTER;
        }
    }
    else{/* need a label */
        encounteredError = getLabelOperand(token, tokenLength, operandAttributesPtr->labelName);
    }

    return encounteredError;
}


errorCodes getRegisterOperand(char *token, unsigned char *regPtr){
    long registerValue; /*register value*/
    errorCodes encounteredError = NO_ERROR;
    char *currentPos = token;

    if (*currentPos++ != '$') {/* register token must begin with '$' */
        encounteredError = NOT_REGISTER;
    }

    if(!encounteredError){
        /* convert register number to long */
        if(stringToLong(currentPos, &registerValue, &currentPos, WORD_MAX_VALUE)){/* is number */
            if(registerValue >= REGISTER_MIN_INDEX && registerValue <= REGISTER_MAX_INDEX){/* in range */
                if(!*currentPos){/* register id is only digits */
                    *regPtr = (unsigned char)registerValue;
                }
                else{/* mixed digits and other characters */
                    encounteredError = REGISTER_ILLEGAL_CHAR;
                }
            }
            else{/* out of range */
                encounteredError = REGISTER_OUT_OF_RANGE;
            }
        }
        else{/* register name is not a number */
            encounteredError = NOT_REGISTER;
        }
    }

    return encounteredError;
}


errorCodes getLabelOperand(char *token, int tokenLength, char *destination) {
    errorCodes encounteredError = tokenIsLabel(token, tokenLength);

    if(!encounteredError){/* legal label name */
        strcpy(destination, token);
    }

    return encounteredError;
}


errorCodes getNumberOperand(char *token, int *destination){
    long value;
    char *nextChar;/* will point to first character after the integer in token */
    errorCodes encounteredError = NO_ERROR;

    if(stringToLong(token, &value, &nextChar, HALF_WORD_MAX_VALUE)){/* number operand is in range */
        /* validate operand is a pure integer */
        if(*nextChar == '.'){/* decimal point */
            encounteredError = NOT_INTEGER;
        }
        else if(*nextChar){/* mixed number and other characters */
            encounteredError = NOT_NUMBER;
        }
        else{/* valid operand */
            *destination = (unsigned int)value;
        }
    }
    else{/* first character is not a digit */
        encounteredError = NOT_NUMBER;
    }

    return encounteredError;
}


boolean checkLineTermination(char **currentPtr, errorCodes *lineErrorPtr){
    char *current = *currentPtr;
    boolean extraneousText = FALSE;

    for(; *current; current++){
        if(!isspace(*current)){
            extraneousText = TRUE;
        }
    }

    if(extraneousText){
        *lineErrorPtr = EXTRANEOUS_TEXT;
    }

    *currentPtr = current - 1;/* mark last character */

    return !extraneousText;
}
