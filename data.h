/* max supported sizes */
#define MAX_LABEL_LENGTH (31)
#define MAX_LINE (80)
#define MAX_FILENAME_LENGTH (250)
#define MAX_COMMAND_LENGTH (7)
#define SIZE_OF_BYTE (1)
#define SIZE_OF_HALF_WORD (2)
#define SIZE_OF_WORD (4)
#define I_TYPE_IMMED_MAX_VALUE_SIGNED (32767)
#define I_TYPE_IMMED_MIN_VALUE (-32768)
#define I_TYPE_IMMED_MAX_VALUE_UNSIGNED (65536)
#define BYTE_MAX_VALUE (127)
#define HALF_WORD_MAX_VALUE (I_TYPE_IMMED_MAX_VALUE_SIGNED)
#define WORD_MAX_VALUE (2147483647)
#define ADDRESS_MAX_VALUE (33554432)
#define REGISTER_MIN_INDEX (0)
#define REGISTER_MAX_INDEX (31)


/* array types sizes */
#define LABEL_ARRAY_SIZE (MAX_LABEL_LENGTH + 1) /* labelsDB length including '\0' character */
#define LINE_ARRAY_SIZE (MAX_LINE + 2)/* including '\n' and '\0' */
#define TOKEN_ARRAY_SIZE (MAX_LINE)
#define NUMBERS_ARRAY_SIZE (MAX_LINE / 2)
#define IMAGE_BLOCK_SIZE (50)
#define COMMAND_ARRAY_SIZE (MAX_COMMAND_LENGTH + 1)

/* default values */
#define STARTING_ADDRESS (100)
#define CODE_AND_DATA_IMAGE_MARGIN (0)
#define EXTERN_LABEL_VALUE (0)
#define NOT_FOUND (-1)


/* formats */
#define DECIMAL_BASE (10)
#define SOURCE_FILE_EXTENSION (".as")
#define OBJECT_FILE_EXTENSION (".ob")
#define ENTRY_FILE_EXTENSION (".ent")
#define EXTERN_FILE_EXTENSION (".ext")


typedef enum {
    FALSE = 0,
    TRUE = 1
}boolean;

typedef enum{
    LINE_TOO_LONG,
    DEFINED_LABEL_ENTRY_EXTERN
}warningCodes;

typedef enum{
    NO_ERROR = 0,
    /* labels */
    DOUBLE_LABEL_DEFINITION,
    LABEL_LOCAL_AND_EXTERN,
    LABEL_NOT_FOUND,
    NO_SPACE_AFTER_LABEL,
    LABEL_IS_OPERATION,
    LABEL_TOO_LONG,
    ILLEGAL_LABEL_NAME,
    ADDRESS_DISTANCE_OVER_LIMITS,
    ENTRY_IS_EXTERN,
    ENTRY_NOT_DEFINED,
    CANNOT_BE_EXTERN,
    /* memory */
    MEMORY_ALLOCATION_FAILURE,
    /* operation names */
    UNIDENTIFIED_OPERATION_NAME,
    MISSING_OPERATION_NAME,
    /* parameters */
    MISSING_PARAMETER,
    EXPECTED_REGISTER_FIRST,
    EXPECTED_REGISTER_SECOND,
    EXPECTED_REGISTER_THIRD,
    EXPECTED_NUMBER_SECOND,
    EXPECTED_LABEL_FIRST,
    REGISTER_ILLEGAL_CHAR,
    REGISTER_OUT_OF_RANGE,
    /* strings and numbers*/
    MISSING_QUOTE,
    NOT_PRINTABLE_CHAR,
    NOT_NUMBER,
    NOT_INTEGER,
    NOT_REGISTER,
    /* parsing */
    MISSING_COMMA,
    ILLEGAL_COMMA,
    /* source file name */
    FILENAME_LENGTH_NOT_SUPPORTED,
    ILLEGAL_FILE_EXTENSION,
    NO_FILES_TO_COMPILE,
    COULD_NOT_OPEN_FILE,
    /* other */
    EXTRANEOUS_TEXT,
    COULD_NOT_CREATE_FILE,
    /* internal crisis */
    IMPOSSIBLE_UPDATE_CODE_IMAGE,/* todo change back */
    IMPOSSIBLE_ENCODE_CODE,
    IMPOSSIBLE_ENCODE_DATA,
    IMPOSSIBLE_SET_LABEL_CALL
}errorCodes;

typedef enum{
    R_ARITHMETIC,
    R_COPY,
    I_ARITHMETIC,
    I_BRANCHING,
    I_MEMORY_LOAD,
    J_JMP,
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
}opcodes;

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
    UNIDENTIFIED_LABEL_TYPE,
    CODE_LABEL,
    DATA_LABEL,
    EXTERN_LABEL
}labelType;

typedef enum{
    UNIDENTIFIED_COMMAND = 0,
    OPERATION_LINE,
    INSTRUCTION_LINE
}commandType;


typedef struct lineID{
    char line[LINE_ARRAY_SIZE];/* the line exactly as read from input file */
    long count;/* number of line in input file */
}lineID;

typedef struct labelID{
    char name[LABEL_ARRAY_SIZE];/* name of labelsDB */
    long address;/* based on context - address defined or address called as operand */
}labelID;


/* database pointer types */

typedef union codeImageDB *codeImageDBPtr;

typedef char *dataImagePtr;/* todo change terminology */

typedef struct entryCallsDB *entryCallsDBPtr;

typedef struct externUsesDB *externUsesDBPtr;

typedef struct labelCallsDB *labelCallsDBPtr;

typedef struct labelsDB *labelsDBPtr;

typedef struct operationsDB *operationsDBPtr;

typedef struct unit *databasePtr;

typedef struct databaseRouter *databaseRouterPtr;

typedef struct databaseRouter{
    codeImageDBPtr codeImageDB;
    dataImagePtr dataImageDB;
    entryCallsDBPtr entryCallsDB;
    databasePtr externUsesDB;
    labelCallsDBPtr labelCallsDB;
    labelsDBPtr labelsDB;
    operationsDBPtr operationsDB;
}databaseRouter;


/* operand managing type definitions */

typedef struct rTypeData{
    unsigned char opcode;
    unsigned char rs;
    unsigned char rt;
    unsigned char rd;
    unsigned char funct;
}rTypeData;

typedef struct iTypeData{
    unsigned char opcode;
    unsigned char rs;
    unsigned char rt;
    long immed;
}iTypeData;

typedef struct jTypeData{
    unsigned char opcode;
    boolean isReg;
    unsigned char address;
}jTypeData;


typedef union codeLineData{
    rTypeData rAttributes;
    iTypeData iAttributes;
    jTypeData jAttributes;
}codeLineData;




typedef struct operandAttributes{
    union valuePointer{
        unsigned char*regPtr;
        long *immedPtr;
    }valuePointer;
    char labelName[LABEL_ARRAY_SIZE];
    boolean isLabel;
}operandAttributes;



void printWarningMessage(warningCodes encounteredWarning, lineID *lineIdPtr);

void printErrorMessage(errorCodes encounteredError, lineID *lineIdPtr);
