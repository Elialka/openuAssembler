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
    char *current;
    int i, progress;

    /* set current to next unread character and reset progress counter*/
    current = line;
    progress = 0;

    /* skip white characters and count progress */
    SKIP_WHITES(current);
    progress += (int)(current - line);

    /* extract first token */
    progress += extractToken(current, currentLabel);

    /* validate characters */
    if(isalpha(*currentLabel)){
        for (i = 0; (isalpha(*currentLabel) || isdigit(*currentLabel)) && i < MAX_LABEL_LENGTH;
                  currentLabel++)/* legal char */
           ;

        if ((*currentLabel)==':'){/* end of label definition */
            *currentLabel = '\0';/* remove ':' from label name */
            *lineIndexPtr += progress;
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
            extractToken(current, regBuffer);

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



boolean extractOperands(char *line, int *lineIndexPtr, operationClass commandOpType, int IC, boolean *jumpIsRegPtr,
                        int *reg1Ptr, int *reg2Ptr, int *reg3Ptr, long *immedPtr, errorCodes *lineErrorPtr) {
    char *current;/* current character */
    char buffer[TOKEN_ARRAY_SIZE];/* next token */
    int bufferLength;/* token length */
    char *endPtr;/* may be deleted */

    /* next unread character */
    current = line + *lineIndexPtr;

    SKIP_WHITES(current);

    /* expecting a register */
    bufferLength = extractToken(current, buffer);
    current += bufferLength;
    if (!(idRegister(buffer, reg1Ptr, lineErrorPtr)))/*if first token is not a register*/
    {
        if (commandOpType == J_JUMP){/* JUMP - only operation with label as possible first parameter */
            if(tokenIsLabel(buffer, bufferLength, lineErrorPtr)){/* is possible label name */
                *jumpIsRegPtr = FALSE;
                if(!addLabelCall(IC, buffer, commandOpType, lineErrorPtr)){
                    *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
                    return FALSE;
                }
                else{
                    return TRUE;/* only expected one operand */
                }
            }
            else{
                *lineErrorPtr = EXPECTED_LABEL_OR_REGISTER;
                return FALSE;
            }
        }
        else{/* operation is not jump - first parameter has to be register */
            *lineErrorPtr = EXPECTED_REGISTER;
            return FALSE;
        }
    }
    else{ /*if first token is register*/

        /* one single register needed */
        if (commandOpType == J_CALL_OR_LA) {
            return TRUE;
        }

        /* read comma and proceed */
        if(!validateComma(&current, lineErrorPtr)){/* missing comma */
            return FALSE;
        }

        bufferLength = extractToken(current, buffer);
        current += bufferLength;


        if (!(idRegister(buffer, reg2Ptr, lineErrorPtr))) /* if second token is not a register*/
        {
            if (!(stringToLong(buffer, immedPtr, &endPtr, HALF_WORD_MAX_VALUE))) /* check if not number*/
                {
                    *lineErrorPtr = EXPECTED_NUMBER;/* todo number\register*/
                    return FALSE;
                }
            else /*if number*/
            {
                /* read comma and proceed */
                if(!validateComma(&current, lineErrorPtr)){/* missing comma */
                    return FALSE;
                }
                else{/* third parameter must be register */
                    /* get next token */
                    bufferLength = extractToken(current, buffer);
                    current += bufferLength;

                    if (!(idRegister(buffer, reg2Ptr, lineErrorPtr)))/* check if third token is not register*/
                    {
                        *lineErrorPtr = EXPECTED_REGISTER;
                        return FALSE;
                    }
                    else if (commandOpType == I_ARITHMETIC || commandOpType == I_MEMORY_LOAD)
                    {
                        return TRUE;
                    }
                }
            }
        }
        else if (commandOpType == R_COPY) { /*if both tokens are registers and R_COPY*/
            return TRUE;
        }
        else{/* after two registers, check the third token*/

            /* read comma and proceed */
            if(!validateComma(&current, lineErrorPtr)){/* missing comma */
                return FALSE;
            }
            else{
                /* get next token */
                bufferLength = extractToken(current, buffer);
                current += bufferLength;

                if (!(idRegister(buffer, reg3Ptr, lineErrorPtr)))/*third is not register*/
                {
                    if (commandOpType == I_BRANCHING) /* I_BRANCHING or FALSE */
                    {
                        if(tokenIsLabel(buffer, bufferLength, lineErrorPtr)){/* third parameter is label */
                            /* write label call position */
                            if(!addLabelCall(IC, buffer, commandOpType, lineErrorPtr)){
                                *lineErrorPtr = MEMORY_ALLOCATION_FAILURE;
                                return FALSE;
                            }
                            else{
                                return TRUE;/* finished */
                            }
                        }
                        else{
                            *lineErrorPtr = EXPECTED_LABEL;
                            return FALSE;
                        }
                    }
                    else{
                        *lineErrorPtr = EXPECTED_REGISTER;
                        return FALSE;
                    }
                }
                else if (commandOpType == R_ARITHMETIC) {/* three tokens are registers and R_ARITHMETICS */
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
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

boolean validateComma(char **currentPtr, errorCodes *lineErrorPtr) {
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
