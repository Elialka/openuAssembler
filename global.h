/* max supported sizes */
#define MAX_LABEL_LENGTH (31)
#define MAX_LINE (80)
#define CHARS_PER_LINE_MAX (MAX_LINE + 1)
#define MAX_FILENAME_LENGTH (250)
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
#define LABEL_ARRAY_SIZE (MAX_LABEL_LENGTH + 1) /* label length including '\0' character */
#define LINE_ARRAY_SIZE (MAX_LINE + 2)/* including '\n' and '\0' */
#define TOKEN_ARRAY_SIZE (MAX_LINE)
#define NUMBERS_ARRAY_SIZE (MAX_LINE / 2)
#define IMAGE_BLOCK_SIZE (50)


/* default values */
#define STARTING_ADDRESS (100)
#define CODE_AND_DATA_IMAGE_MARGIN (0)
#define EXTERN_LABEL_VALUE (0)


/* formats */
#define SOURCE_FILE_EXTENSION (".as")
#define OBJECT_FILE_EXTENSION (".ob")
#define ENTRY_FILE_EXTENSION (".ent")
#define EXTERN_FILE_EXTENSION (".ext")


typedef enum {
    FALSE = 0,
    TRUE = 1
}boolean;

/* error enums */

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
    EXCEEDING_MEMORY_LIMITS,
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
    ILLEGAL_REGISTER_ID,
    /* strings and numbers*/
    MISSING_QUOTE,
    NOT_PRINTABLE_CHAR,
    NOT_NUMBER,
    NOT_INTEGER,
    NOT_REGISTER,
    /* parsing */
    MISSING_COMMA,
    ILLEGAL_COMMA,
    /* other */
    EXTRANEOUS_TEXT,
    COULD_NOT_CREATE_FILE,
    COULD_NOT_OPEN_FILE,
    /* internal crisis */
    IMPOSSIBLE
}errorCodes;

typedef enum projectErrors{
    NOT_OCCURRED = 0,
    PROJECT_MEMORY_FAILURE,
    FILENAME_LENGTH_NOT_SUPPORTED,
    ILLEGAL_FILE_EXTENSION,
    NO_FILES_TO_COMPILE
}projectErrors;

typedef struct fileErrorStatus{
    char *sourceFileName;
    boolean errorOccurred;
}fileErrorStatus;


typedef enum{/* for code line commands - grouped by operand types */
    R_ARITHMETIC,
    R_COPY,
    I_ARITHMETIC,
    I_BRANCHING,
    I_MEMORY_LOAD,
    J_JMP,
    J_CALL_OR_LA,
    J_STOP
}operationClass;

typedef enum{/* for data line commands */
    ENTRY,
    EXTERN,
    DW,
    DH,
    DB,
    ASCIZ
}dataOps;


typedef struct lineID{/* input line identifiers */
    char line[LINE_ARRAY_SIZE];/* the line exactly as read from input file */
    long count;/* number of line in input file */
}lineID;

typedef struct labelID{/* label identifiers */
    char name[LABEL_ARRAY_SIZE];/* name of label */
    long address;/* based on context - address defined or address called as operand */
}labelID;


/* database pointer types */

typedef union codeImageDB *codeImageDBPtr;

typedef struct dataImageDB *dataImageDBPtr;

typedef struct operationsDB *operationsDBPtr;

typedef struct unit *databasePtr;

typedef struct databaseRouter *databaseRouterPtr;

typedef struct databaseRouter{/* structure holding pointers to all databases */
    codeImageDBPtr codeImageDB;
    dataImageDBPtr dataImageDB;
    databasePtr entryCallsDB;
    databasePtr externUsesDB;
    databasePtr labelCallsDB;
    databasePtr labelsDB;
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


typedef struct operandAttributes{
    union valuePointer{/* pointers to where to store relevant values */
        unsigned char*regPtr;
        long *immedPtr;
    }valuePointer;
    char labelName[LABEL_ARRAY_SIZE];
    boolean isLabel;
}operandAttributes;
