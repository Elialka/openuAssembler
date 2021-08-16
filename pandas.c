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


int copyNextToken(char *source, char *buffer){
    int i;

    for(i = 0; *source && !isspace(*source) && *source != ','; source++, i++){
        buffer[i] = *source;
    }
    buffer[i] = '\0';
    return i;
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


boolean isLabelDefinition(char **currentPosPtr, char *currentLabel, errorCodes *lineErrorPtr) {
    boolean result = FALSE;
    char *currentChar = *currentPosPtr;/* tracks progress in strings */
    char *definitionEnd;/* point to the end of the definition */
    char token[TOKEN_ARRAY_SIZE];/* store token to examine it */
    int nameLength;/* how many characters are in the name being defined */

    /* skip white characters and count progress */
    SKIP_WHITES(currentChar);

    /* extract first token */
    currentChar += copyNextToken(currentChar, token);

    if((definitionEnd = strchr(token, ':'))){/* a label is defined */
        nameLength = countLabelNameCharacters(token);
        if(nameLength > MAX_LABEL_LENGTH){
            *lineErrorPtr = LABEL_DEFINITION_TOO_LONG;
        }
        else if(nameLength != definitionEnd - token){/* illegal characters before ':' */
            *lineErrorPtr = ILLEGAL_LABEL_NAME;
        }
        else{/* legal length and characters */
            if(isspace(*(definitionEnd + 1))){/* no white character after label definition */
                *lineErrorPtr = NO_SPACE_AFTER_LABEL;
            }
            else{
                *currentPosPtr = currentChar;/* update line position */
                *definitionEnd = '\0';/* mark end of name string */
                strcpy(currentLabel, token);/* save label name */
                result = TRUE;
            }
        }
    }

    return result;
}


errorCodes extractCommandName(char **currentPosPtr, char *commandName) {
    errorCodes encounteredError = NO_ERROR;
    char *currentCharacter = *currentPosPtr;
    char buffer[TOKEN_ARRAY_SIZE];
    int tokenLength;

    SKIP_WHITES(currentCharacter);

    /* get next token */
    tokenLength = copyNextToken(currentCharacter, buffer);
    currentCharacter += tokenLength;

    if(!tokenLength){/* no characters read */
        encounteredError = MISSING_OPERATION_NAME;
    }
    else if(tokenLength > MAX_COMMAND_LENGTH){/* impossible length */
        encounteredError = UNIDENTIFIED_OPERATION_NAME;
    }
    else{/* possible command name */
        *currentPosPtr = currentCharacter;
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
    errorCodes encounteredError = NO_ERROR;
    int i;
    char *current = *currentPosPtr;

    SKIP_WHITES(current);

    /* start of string */
    if(*current=='"')
    {
        current++;
    }
    else{
        encounteredError = MISSING_QUOTE;
    }

    /* copy string to string buffer */
    for (i=0; !encounteredError && *current && *current != '"'; i++, current++)
    {
        if(isprint(*current)){
            destination[i] = *current;
        }
        else{/* only printable characters are allowed */
            encounteredError = ILLEGAL_CHARACTER;
        }
    }

    /* validate closing quotes */
    if (!encounteredError && *current=='"')
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
        SKIP_WHITES(current);

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
            numberLength = copyNextToken(current, number);
            if(!numberLength){/* end of line */
                if(i){/* already read some numbers */
                    *lineErrorPtr = ILLEGAL_COMMA;
                }
                else{
                    *lineErrorPtr = EXPECTED_NUMBER;
                }
            }
            current += numberLength;

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


boolean idRegister(char *token, int *regPtr, errorCodes *lineErrorPtr) {


    char *current; /* this token current position*/
    int i; /*index for loop*/
    boolean isInt = TRUE; /*flag to integer number*/
    int reg; /*register value*/

    current = token;

    /*check if the first char is $ */
    if (*current == '$') {
        current++;

        if (isdigit(*current)) {

            for (i = 0 ; current[i]!='\0' ; i++) /*checks if current does not include illegal chars, only digits*/
            {
                if (!(isdigit(current[i]))) {
                    isInt = FALSE;
                    break;
                }
            }

            if (isInt) {
                reg = atoi(current); /*make int out of current token string*/
                if (reg >= REGISTER_MIN_INDEX && reg <= REGISTER_MAX_INDEX) { /*in registers range*/
                    *regPtr = reg;
                    return TRUE;
                }
            }

        }
    }
    /*if token is not following the conditions this is not a register*/
    *lineErrorPtr = NOT_REG;
    return FALSE;
}


boolean extractOperands(char **currentPosPtr, operationClass commandOpType, long IC, boolean *jIsRegPtr, int *reg1Ptr,
                        int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr, void *labelCallsHead) {
    char *current;/* current character */
    char buffer[TOKEN_ARRAY_SIZE];/* next token */
    int bufferLength;/* token length */
    int *lastRegPtr;/* when analysing third operand, point to first unused register buffer */
    boolean generalError;/* flag turns on when error occurred*/
    boolean finished;/* flag turns on when we have enough operands for current operation type \ error occurred */

    /* reset line operand buffers */
    *reg1Ptr = 0;
    *reg2Ptr = 0;
    *reg3Ptr = 0;
    *immedPtr = 0;

    /* reset flags */
    generalError = FALSE;
    finished = FALSE;

    /* next unread character */
    current = *currentPosPtr;
    SKIP_WHITES(current);

    /* get next token */
    bufferLength = copyNextToken(current, buffer);
    current += bufferLength;

    /* analyse first operator */
    if(!getFirstOperand(buffer, bufferLength, commandOpType, IC, jIsRegPtr,
                        reg1Ptr, lineErrorPtr, labelCallsHead)){/* error */
        generalError =  TRUE;
        finished = TRUE;
    }
    else{/* legal first operator */
        /* check if only one operator needed */
        if(commandOpType == J_JUMP || commandOpType == J_CALL_OR_LA || commandOpType == J_STOP){
            finished = TRUE;
        }
    }

    if(!finished){ /* need second operator */
        /* read comma */
        if(!readComma(&current, lineErrorPtr)){/* missing */
            *lineErrorPtr = MISSING_COMMA;
            generalError = TRUE;
            finished = TRUE;
        }
        else{/* comma as expected */
            /* get next token */
            bufferLength = copyNextToken(current, buffer);
            current += bufferLength;

            /* analyse second operator */
            if(!getSecondOperand(buffer, commandOpType, reg2Ptr, immedPtr, lineErrorPtr)){/* error */
                generalError =  TRUE;
                finished = TRUE;
            }
            else{/* legal second operator */
                /* check if only two operators needed */
                if(commandOpType == R_COPY){
                    finished = TRUE;
                }
            }
        }
    }

    if(!finished){/* need third operator */
        /* read comma */
        if(!readComma(&current, lineErrorPtr)){/* missing */
            *lineErrorPtr = MISSING_COMMA;
            generalError = TRUE;
        }
        else{/* comma as expected */
            /* get next token */
            bufferLength = copyNextToken(current, buffer);
            current += bufferLength;

            /* determine which register buffer is next unused */
            if(commandOpType == R_ARITHMETIC){/* two registers already read */
                lastRegPtr = reg3Ptr;
            }
            else{/* one register already read */
                lastRegPtr = reg2Ptr;
            }

            /* analyse third operand */
            if(!getThirdOperand(buffer, bufferLength, IC, commandOpType, lastRegPtr,
                                immedPtr, lineErrorPtr, labelCallsHead)){/* error */
                generalError = TRUE;
            }
        }
    }

    *currentPosPtr = current;

    return !generalError;
}


boolean tokenIsLabel(char *token, int tokenLength, errorCodes *lineErrorPtr){
    boolean result = FALSE;
    int nameLength;

    /* count how many legal characters are in token */
    nameLength = countLabelNameCharacters(token);

    if(nameLength > MAX_LABEL_LENGTH){/* impossible length */
        *lineErrorPtr = LABEL_TOO_LONG;
    }
    else if(nameLength < tokenLength){/* illegal characters present */
        *lineErrorPtr = ILLEGAL_LABEL_NAME;
    }
    else{/* possible label name */
        result = TRUE;
    }

    return result;
}


boolean readComma(char **currentPtr, errorCodes *lineErrorPtr) {
    boolean result;

    SKIP_WHITES(*currentPtr);

    if(**currentPtr != ','){
        *lineErrorPtr = MISSING_COMMA;
        result = FALSE;
    }
    else{/* is a comma */
        (*currentPtr)++;
        SKIP_WHITES(*currentPtr);
        result = TRUE;
    }

    return result;
}


boolean getFirstOperand(char *token, int tokenLength, operationClass commandOpType, long IC, boolean *jIsRegPtr,
                        int *regPtr, errorCodes *lineErrorPtr, void *labelCallsHead) {
    boolean result;

    if(idRegister(token, regPtr, lineErrorPtr)){/* token is register */
        *jIsRegPtr = TRUE;
        result = TRUE;
    }
    else if(commandOpType == J_JUMP){
        *jIsRegPtr = FALSE;

        if(tokenIsLabel(token, tokenLength, lineErrorPtr)){/* token is label call */
            /* add label call to database */
            if(!setLabelCall(labelCallsHead, IC, token, commandOpType, lineErrorPtr)){
                *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
                result = FALSE;
            }
            else{
                result = TRUE;
            }
        }
        else{/* token is not a possible label name */
            *lineErrorPtr = EXPECTED_LABEL_OR_REGISTER;
            result = FALSE;
        }
    }
    else{
        *lineErrorPtr = EXPECTED_REGISTER;
        result = FALSE;
    }

    return result;
}


boolean getSecondOperand(char *token, operationClass commandOpType, int *regPtr, long *immedPtr,
                         errorCodes *lineErrorPtr) {
    boolean result;
    char *endPtr;

    if(idRegister(token, regPtr, lineErrorPtr)){/* token is register */
        if(commandOpType == I_ARITHMETIC || commandOpType == I_MEMORY_LOAD){/* should be number */
            *lineErrorPtr = EXPECTED_NUMBER;
            result = FALSE;
        }
        else{/* is register as expected */
            result = TRUE;
        }
    }
    else{/* token is not register */
        if(commandOpType != I_ARITHMETIC && commandOpType != I_MEMORY_LOAD){/* should be register */
            *lineErrorPtr = EXPECTED_REGISTER;
            result = FALSE;
        }
        else if (stringToLong(token, immedPtr, &endPtr, HALF_WORD_MAX_VALUE)){ /* can be legal immediate value */
            if(*endPtr == '.'){/* decimal point */
                *lineErrorPtr = NOT_INTEGER;
                result = FALSE;
            }
            else if(*endPtr){/* mixed number and other characters */
                *lineErrorPtr = NOT_NUMBER;
                result = FALSE;
            }
            else{/* legal immediate value */
                result = TRUE;
            }
        }
        else{/* should be immediate value - number */
            *lineErrorPtr = EXPECTED_NUMBER;
            result = FALSE;
        }
    }

    return result;
}


boolean getThirdOperand(char *token, int tokenLength, long IC, operationClass commandOpType, int *regPtr, long *immedPtr,
                        errorCodes *lineErrorPtr, void *labelCallsHead) {
    boolean result;

    if(idRegister(token, regPtr, lineErrorPtr)){/* token is register */
        if(commandOpType != I_BRANCHING){/* viable operator */
            result = TRUE;
        }
        else{/* operator should be a label */
            *lineErrorPtr = EXPECTED_LABEL;
            result = FALSE;
        }
    }
    else{/* token is not register*/
        if(commandOpType != I_BRANCHING){/* should be register */
            *lineErrorPtr = EXPECTED_REGISTER;
            result = FALSE;
        }
        else{/* can be label call */
            if(tokenIsLabel(token, tokenLength, lineErrorPtr)){/* possible label name */
                *immedPtr = -IC;/* label address will be added at second pass */

                /* add label call to database */
                if(!setLabelCall(labelCallsHead, IC, token, commandOpType, lineErrorPtr)){/* memory failure */
                    *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
                    result = FALSE;
                }
                else{
                    result = TRUE;
                }
            }
            else{/* impossible label name */
                *lineErrorPtr = EXPECTED_LABEL;
                result = FALSE;
            }
        }
    }

    return result;
}


boolean getLabel(char **currentPosPtr, char *labelName, errorCodes *lineErrorPtr){
    boolean result = FALSE;
    char buffer[TOKEN_ARRAY_SIZE];
    char *current = *currentPosPtr;
    int tokenLength = 0;


    SKIP_WHITES(current);

    tokenLength += copyNextToken(current, buffer);
    current += tokenLength;

    if(!tokenLength){/* missing token */
        *lineErrorPtr = MISSING_PARAMETER;
    }
    else if(tokenIsLabel(buffer, tokenLength, lineErrorPtr)){/* legal label name */
        copyNextToken(buffer, labelName);
        *currentPosPtr = current;
        result = TRUE;
    }

    return result;
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
