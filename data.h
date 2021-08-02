/* max supported sizes */
#define MAX_LABEL_LENGTH (31)
#define MAX_LINE (80)
#define SIZE_OF_BYTE (1)
#define SIZE_OF_HALF_WORD (2)
#define SIZE_OF_WORD (4)
#define BYTE_MAX_VALUE (127)
#define HALF_WORD_MAX_VALUE (32767)
#define WORD_MAX_VALUE (2147483647)
#define ADDRESS_MAX_VALUE (33554432)
#define REGISTER_MIN_INDEX (0)
#define REGISTER_MAX_INDEX (31)


/* array types sizes */
#define LABEL_ARRAY_SIZE (MAX_LABEL_LENGTH + 1) /* label length including '\0' character */
#define LINE_ARRAY_SIZE (MAX_LINE + 2)/* including '\n' and '\0' */
#define TOKEN_ARRAY_SIZE (MAX_LINE)
#define NUMBERS_ARRAY_SIZE (MAX_LINE / 2)
#define IMAGE_BLOCK_SIZE (50)
#define DATABASE_POINTER_ARRAY_SIZE (5)

/* default values */
#define STARTING_ADDRESS (100)


/* formats */
#define DECIMAL_BASE (10)



typedef enum {
    FALSE = 0,
    TRUE
}boolean;

typedef enum{
    /* labels */
    DOUBLE_LABEL_DEFINITION,
    LABEL_NOT_FOUND,
    NO_SPACE_AFTER_LABEL,
    LABEL_IS_OPERATION,
    LABEL_TOO_LONG,
    ILLEGAL_LABEL_NAME,
    /* memory */
    MEMORY_ALLOCATION_FAILURE,
    /* operation names */
    UNIDENTIFIED_OPERATION_NAME,
    MISSING_OPERATION_NAME,
    /* parameters */
    MISSING_PARAMETER,
    TOO_MANY_PARAMETERS,
    EXPECTED_REGISTER,
    EXPECTED_NUMBER,
    EXPECTED_LABEL,
    EXPECTED_LABEL_OR_REGISTER,
    /* strings and numbers*/
    MISSING_QUOTE,
    ILLEGAL_EXPRESSION,
    ILLEGAL_CHARACTER,
    NOT_NUMBER,
    NOT_INTEGER,
    VALUE_OUT_OF_RANGE,
    NOT_REG,
    /* parsing */
    MISSING_COMMA,
    ILLEGAL_COMMA,
    /* other */
    LINE_TOO_BIG,
    NO_ERROR,
    /* internal crisis */
    IMPOSSIBLE
}errorCodes;

typedef enum{
    R_ARITHMETIC,
    R_COPY,
    I_ARITHMETIC,
    I_BRANCHING,
    I_MEMORY_LOAD,
    J_JUMP,
    J_CALL_OR_LA,
    J_STOP
}operationClass;

typedef enum{
    ENTRY,
    EXTERN,
    DW,
    DH,
    DB,
    ASCIZ
}dataOps;

typedef enum{
    ADD = 0,
    SUB = 0,
    AND = 0,
    OR = 0,
    NOR = 0,
    MOVE = 1,
    MVHI = 1,
    MVLO = 1,
    ADDI = 10,
    SUBI = 11,
    ANDI = 12,
    ORI = 13,
    NORI = 14,
    BNE = 15,
    BEQ = 16,
    BLT = 17,
    BGT = 18,
    LB = 19,
    SB = 20,
    LW = 21,
    SW = 22,
    LH = 23,
    SH = 24,
    JMP = 30,
    LA = 31,
    CALL = 32,
    STOP = 63
}commandOps;

typedef enum{
    ADD_FUNCT = 1,
    SUB_FUNCT = 2,
    AND_FUNCT = 3,
    OR_FUNCT = 4,
    NOR_FUNCT = 5,
    MOVE_FUNCT = 1,
    MVHI_FUNCT = 2,
    MVLO_FUNCT = 3
}functValues;

typedef enum{
    DATA_LINE,
    CODE_LINE
}labelType;

typedef enum{
    LABELS_POINTER,
    OPERATIONS_POINTER,
    DATA_IMAGE_POINTER,
    CODE_IMAGE_POINTER,
    LABEL_CALLS_POINTER
}pointerArrayIndex;


