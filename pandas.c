#include <ctype.h>
#include <stdint.h>/* experimental */
#include <stdlib.h>
#include <errno.h>


#include "pandas.h"
#include "labelsDB.h"
#include "operationsDB.h"
#include "dataImageDB.h"
#include "labelCallsDB.h"

#define SKIP_WHITES(x) for(; isspace(*x); x++)


/*
 * return TRUE if empty or comment line
 */
boolean ignoreLine(char *line){
    char *current;

    current = line;

    SKIP_WHITES(current);

    if(*current){
        if(*current == ';'){/* comment line */
            return TRUE;
        }
        else{/* line is not empty */
            return FALSE;
        }
    }
    else{/* line is empty */
        return TRUE;
    }
}


int extractToken(char *current, char *buffer){
    int i;

    for(i = 0; *current && !isspace(*current) && *current != ','; current++, i++){
        buffer[i] = *current;
    }
    buffer[i] = '\0';
    return i;
}


/*
 * return TRUE if line starts with label definition
 */
boolean isLabelDefinition(char *line, int *lineIndexPtr, char *currentLabel){
    char *current;/* tracks progress in checks */
    char buffer[TOKEN_ARRAY_SIZE];/* store token to examine it */
    int i;
    int progress;/* counts how many characters read from line */

    /* set current to next unread character and reset progress counter*/
    current = line;
    progress = 0;

    /* skip white characters and count progress */
    SKIP_WHITES(current);
    progress += (int)(current - line);

    /* extract first token */
    progress += extractToken(current, buffer);

    current = buffer;

    /* validate characters */
    if(isalpha(*current)){
        for (i = 0; (isalpha(*current) || isdigit(*current)) && i < MAX_LABEL_LENGTH;
                  current++)/* legal char */
           ;

        if ((*current)==':'){/* end of label definition */
            *current = '\0';/* remove ':' from label name */
            *lineIndexPtr += progress;
            /* copy to current label */
            extractToken(current, currentLabel);
            return TRUE;
        }
    }

    return FALSE;
}


/*
 * extract command name from line
 * return TRUE if no errors occurred, FALSE otherwise
 */
boolean extractCommandName(char *line, int *lineIndexPtr, char *commandName,
                           boolean labelDefinition, errorCodes *lineErrorPtr)
{
    char *current;/* current character */
    char *start;/* first character to track progress in line array */
    int tokenLength;

    /* reset current to first unread character */
    current = line + *lineIndexPtr;
    start = current;

    if(labelDefinition){/* check white character after label definition */
        if(isspace(*current)){
            current++;
        }
        else{
            *lineErrorPtr = NO_SPACE_AFTER_LABEL;
            return FALSE;
        }
    }

    SKIP_WHITES(current);

    tokenLength = extractToken(current, commandName);

    if(!tokenLength){/* no characters read */
        *lineErrorPtr = MISSING_OPERATION_NAME;
        return FALSE;
    }

    /* count how many characters read, add to counter */
    *lineIndexPtr += (int)(current - start);

    return TRUE;
}


boolean stringToLong(char *token, long *valuePtr, char **endPtrPtr, long maxValue) {
    long value;
    long minValue;

    /* reset global error flag */
    errno = 0;

    /* calculate minimum value */
    minValue = maxValue * (-1);
    minValue--;

    value = strtol(token, endPtrPtr, DECIMAL_BASE);

    /* is value in range */
    if(token != *endPtrPtr && !errno && value <= maxValue && value >= minValue){
        *valuePtr = value;
        return TRUE;
    }
    else{
        return FALSE;
    }

}


boolean getStringFromLine(char *line, int *indexPtr, char *buffer, errorCodes *lineErrorPtr){
    int i;
    char *current;
    char *start;

    current = line + *indexPtr;/* next unread character */
    start = current;

    SKIP_WHITES(current);

    /* start of string */
    if(*current=='"')
    {
        current++;
        /* copy string to string buffer */
        for (i=0; *current && *current != '"' && i < TOKEN_ARRAY_SIZE; i++, current++)
        {
            if(isprint(*current)){
                buffer[i] = *current;
            }
            else{
                *lineErrorPtr = ILLEGAL_CHARACTER;
                return FALSE;
            }
        }

        /* validate closing quotes */
        if (*current=='"')
        {
            buffer[i]='\0';
            *indexPtr += (int)(current - start);
        }
        else
        {
            *lineErrorPtr = MISSING_QUOTE;
            return FALSE;
        }
    }
    else{
        return FALSE;
    }

    return TRUE;
}


int getNumbersFromLine(char *line, int *indexPtr, long *buffer, dataOps dataOpType, errorCodes *lineErrorPtr) {
    int i;
    long value;
    char number[MAX_LINE];
    int numberLength;
    int numberCounter;
    char *current;
    char *endPtr;
    int maxValue;

    current = line + *indexPtr;
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

            if (!*current) /*end of line*/
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
            numberLength = extractToken(current, number);
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

    char *current;
    char regBuffer[MAX_LINE];
    int i;
    boolean isInt = TRUE;
    int reg;

    current = token;

    if (*current == '$') {
        current++;

        if (isdigit(*current)) {
            extractToken(current, regBuffer);/* todo remove redundant extract token */

            for (i = 0 ; regBuffer[i] != '\0' ; i++) /*checks if regBuffer does not include illegal chars*/
            {
                if (!(isdigit(regBuffer[i]))) {
                    isInt = FALSE;
                    break;
                }
            }

            if (isInt) {
                reg = atoi(regBuffer); /*make int out of regBuffer string*/
                if (reg >= REGISTER_MIN_INDEX && reg <= REGISTER_MAX_INDEX) { /*in range*/
                    *regPtr = reg;
                    return TRUE;
                }
            }
        }
    }

    *lineErrorPtr = NOT_REG;
    return FALSE;
}


boolean extractOperands(char *line, int *lineIndexPtr, operationClass commandOpType, int IC, boolean *jIsRegPtr,
                        int *reg1Ptr, int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr) {
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
    current = line + *lineIndexPtr;
    SKIP_WHITES(current);

    /* get next token */
    bufferLength = extractToken(current, buffer);
    current += bufferLength;

    /* analyse first operator */
    if(!getFirstOperand(buffer, bufferLength, commandOpType, IC, jIsRegPtr, reg1Ptr, lineErrorPtr)){/* error */
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
            bufferLength = extractToken(current, buffer);
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
            bufferLength = extractToken(current, buffer);
            current += bufferLength;

            /* determine which register buffer is next unused */
            if(commandOpType == R_ARITHMETIC){/* two registers already read */
                lastRegPtr = reg3Ptr;
            }
            else{/* one register already read */
                lastRegPtr = reg2Ptr;
            }

            /* analyse third operand */
            if(!getThirdOperand(buffer, bufferLength, IC, commandOpType, lastRegPtr, immedPtr, lineErrorPtr)){/* error */
                generalError = TRUE;
            }
        }
    }

    return !generalError;
}


boolean tokenIsLabel(char *token, int tokenLength, errorCodes *lineErrorPtr){
    boolean result;
    char *current;

    result = TRUE;
    current = token;

    if(tokenLength > MAX_LABEL_LENGTH){/* impossible length */
        *lineErrorPtr = LABEL_TOO_LONG;
        result = FALSE;
    }
    else if(!isalpha(*current)){/* does not start with letter */
        *lineErrorPtr = ILLEGAL_LABEL_NAME;
        result = FALSE;
    }
    else{/* starts with a letter && possible length */
        for(current++; *current; current++){
            if(!isalnum(*current)){/* illegal character */
                *lineErrorPtr = ILLEGAL_LABEL_NAME;
                result = FALSE;
            }
        }
    }

    return result;
}


boolean readComma(char **currentPtr, errorCodes *lineErrorPtr) {
    boolean result;

    SKIP_WHITES((*currentPtr));

    if(**currentPtr != ','){
        *lineErrorPtr = MISSING_COMMA;
        result = FALSE;
    }
    else{/* is a comma */
        (*currentPtr)++;
        SKIP_WHITES((*currentPtr));
        result = TRUE;
    }

    return result;
}


boolean getFirstOperand(char *token, int tokenLength, operationClass commandOpType, int IC, boolean *jIsRegPtr,
                        int *regPtr, errorCodes *lineErrorPtr) {
    boolean result;

    if(idRegister(token, regPtr, lineErrorPtr)){/* token is register */
        *jIsRegPtr = TRUE;
        result = TRUE;
    }
    else if(commandOpType == J_JUMP){
        *jIsRegPtr = FALSE;

        if(tokenIsLabel(token, tokenLength, lineErrorPtr)){/* token is label call */
            /* add label call to database */
            if(!addLabelCall(IC, token, commandOpType, lineErrorPtr)){
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


boolean getThirdOperand(char *token, int tokenLength, int IC, operationClass commandOpType,
                        int *regPtr, long *immedPtr, errorCodes *lineErrorPtr){
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
                if(!addLabelCall(IC, token, commandOpType, lineErrorPtr)){/* memory failure */
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

