#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


#include "pandas.h"
#include "operationsDB.h"

#define SKIP_WHITES(x) for(; isspace(*(x)); (x)++)
#define DECIMAL_BASE (10)



/**
 * Count legal label's characters
 * @param token Name string
 * @return How many characters read until string ended, or until illegal character encountered
 */
static int countLabelNameCharacters(char *token);

static long calculateMaxValue(dataOps dataOpType);


boolean needToReadLine(char *line){
    boolean result = FALSE;

    if(line){
        SKIP_WHITES(line);

        /* evaluate first non-white character */
        if(*line){/* line is not empty */
            if(*line != ';'){/* not comment line */
                result =  TRUE;
            }
        }
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


static long calculateMaxValue(dataOps dataOpType){
    long maxValue = 0;

    if(dataOpType == DB){
        maxValue = BYTE_MAX_VALUE;
    }
    else if(dataOpType == DH){
        maxValue = HALF_WORD_MAX_VALUE;
    }
    else if(dataOpType == DW){
        maxValue = WORD_MAX_VALUE;
    }

    return maxValue;
}


boolean isLabelDefinition(char **currentPosPtr, char *currentLabel, errorCodes *errorPtr){
    errorCodes encounteredError = NO_ERROR;
    boolean result = FALSE;
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
                result = TRUE;
            }
            else{/* no white character after label definition */
                encounteredError = NO_SPACE_AFTER_LABEL;
            }
        }
    }

    if(encounteredError){
        *errorPtr = encounteredError;
    }

    return result;
}


errorCodes extractCommandName(char **currentPosPtr, char *commandName){
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


boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue){
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


errorCodes getLabelFromLine(char **currentPosPtr, char *destination){
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(currentPosPtr, token);
    errorCodes encounteredError = NO_ERROR;

    if(!tokenLength){
        encounteredError = MISSING_PARAMETER;
    }

    if(!encounteredError){
        encounteredError = getLabelOperand(token, tokenLength, destination);
    }

    return encounteredError;
}


errorCodes getStringFromLine(char **currentPosPtr, char *destination){
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
            encounteredError = NOT_PRINTABLE_CHAR;
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


int getNumbersFromLine(char **currentPosPtr, long *numbersArray, dataOps dataOpType, errorCodes *lineErrorPtr){
    errorCodes encounteredError = NO_ERROR;
    int i;/* will help keep track if we are expecting a comma or a number */
    long value;/* store each numbers value */
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength;
    int numbersCounter = 0;/* how many numbers read */
    long maxValue = calculateMaxValue(dataOpType);/* max positive value */
    boolean finished = FALSE;

    for (i = 0 ; !encounteredError && !finished; i++){
        if (i % 2){/* expecting a comma */
            if(readComma(currentPosPtr) != NO_ERROR){/* missing comma */
                if(needToReadLine(*currentPosPtr)){/* line is terminated *//* temp */
                    encounteredError = NO_ERROR;
                }
                finished = TRUE;
            }
        }
        else{/* expecting a number */
            tokenLength = extractNextToken(currentPosPtr, token);
            if(tokenLength){
                /* read number */
                encounteredError = getNumberOperand(token, &value, maxValue);
                numbersArray[i / 2] = value;
                numbersCounter++;
            }
            else{/* end of line */
                encounteredError = i ? ILLEGAL_COMMA : MISSING_PARAMETER;
            }
        }
    }

    if(encounteredError){
        numbersCounter = 0;
        *lineErrorPtr = encounteredError;
    }

    return numbersCounter;
}


errorCodes tokenIsLabel(char *token, int tokenLength){
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


errorCodes readComma(char **currentPosPtr){
    errorCodes encounteredError = NO_ERROR;

    SKIP_WHITES(*currentPosPtr);

    if(**currentPosPtr != ','){
        encounteredError = MISSING_COMMA;
    }
    else{
        (*currentPosPtr)++;
    }

    return encounteredError;
}


errorCodes getFirstOperand(char **currentPosPtr, operationClass commandOpType, operandAttributes *operandAttributesPtr){
    char token[TOKEN_ARRAY_SIZE];
    int tokenLength = extractNextToken(currentPosPtr, token);
    errorCodes encounteredError = getRegisterOperand(token, operandAttributesPtr->valuePointer.regPtr);

    if(!tokenLength){/* end of the line */
        encounteredError = MISSING_PARAMETER;
    }
    else if(encounteredError == NOT_REGISTER){/* not a register */
        if (commandOpType == J_JMP || commandOpType == J_CALL_OR_LA){/* is possibly a label */
            encounteredError = getLabelOperand(token, tokenLength, operandAttributesPtr->labelName);
            operandAttributesPtr->isLabel = TRUE;
        }
        else{/* should be a register */
            encounteredError = EXPECTED_REGISTER_FIRST;
        }
    }

    if(commandOpType == J_CALL_OR_LA && !operandAttributesPtr->isLabel){
        /* this command type does not support register operands */
        encounteredError = EXPECTED_LABEL_FIRST;
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
        encounteredError = getRegisterOperand(token, operandAttributesPtr->valuePointer.regPtr);
        if(encounteredError == NOT_REGISTER){
            encounteredError = EXPECTED_REGISTER_SECOND;
        }
    }
    else{/* need an immediate value */
        encounteredError = getNumberOperand(token, operandAttributesPtr->valuePointer.immedPtr, HALF_WORD_MAX_VALUE);
        if(encounteredError == NOT_NUMBER){
            encounteredError = EXPECTED_NUMBER_SECOND;
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
        encounteredError = getRegisterOperand(token, operandAttributesPtr->valuePointer.regPtr);
        if(encounteredError == NOT_REGISTER){
            encounteredError = EXPECTED_REGISTER_THIRD;
        }
    }
    else{/* need a label */
        encounteredError = getLabelOperand(token, tokenLength, operandAttributesPtr->labelName);
        operandAttributesPtr->isLabel = TRUE;
    }

    return encounteredError;
}


errorCodes getRegisterOperand(char *token, unsigned char *regPtr){
    long registerValue; /*register value*/
    errorCodes encounteredError = NO_ERROR;
    char *currentPos = token;

    if (*currentPos++ != '$'){/* register token must begin with '$' */
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
                    encounteredError = ILLEGAL_REGISTER_ID;
                }
            }
            else{/* out of range */
                encounteredError = ILLEGAL_REGISTER_ID;
            }
        }
        else{/* register name is not a number */
            encounteredError = NOT_REGISTER;
        }
    }

    return encounteredError;
}


errorCodes getLabelOperand(char *token, int tokenLength, char *destination){
    errorCodes encounteredError = tokenIsLabel(token, tokenLength);

    if(!encounteredError){/* legal label name */
        strcpy(destination, token);
    }

    return encounteredError;
}


errorCodes getNumberOperand(char *token, long *destination, long maxValue){
    long value;
    char *nextChar;/* will point to first character after the integer in token */
    errorCodes encounteredError = NO_ERROR;

    if(stringToLong(token, &value, &nextChar, maxValue)){/* number operand is in range */
        /* validate operand is a pure integer */
        if(*nextChar == '.'){/* decimal point */
            encounteredError = NOT_INTEGER;
        }
        else if(*nextChar){/* mixed number and other characters */
            encounteredError = NOT_NUMBER;
        }
        else{/* valid operand */
            *destination = (int)value;
        }
    }
    else{/* first character is not a digit */
        encounteredError = NOT_NUMBER;
    }

    return encounteredError;
}


errorCodes checkLineTermination(char **currentPosPtr){
    char *current = *currentPosPtr;
    errorCodes encounteredError = NO_ERROR;

    for(; !encounteredError && *current; current++){
        if(!isspace(*current)){
            if(*current == ','){
                encounteredError = ILLEGAL_COMMA;
            }
            else{
                encounteredError = EXTRANEOUS_TEXT;
            }
        }
    }

    *currentPosPtr = --current;/* set pointer to last character */

    return encounteredError;
}
