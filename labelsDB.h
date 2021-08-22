
typedef enum{
    UNIDENTIFIED_LABEL_TYPE,
    CODE_LABEL,
    DATA_LABEL,
    EXTERN_LABEL
}labelType;


typedef struct definedLabel{
    labelID labelId;/* defined in global.h */
    labelType type;
}definedLabel;


/**
 * Initialize labels database
 * @return pointer to the database
 */
databasePtr initLabelsDB();

/**
 * Add new label definition
 * @param head pointer to database
 * @param labelDataPtr pointer to structure holding label attributes
 * @return errorCodes enum value describing function success/failure
 */
errorCodes addNewLabel(databasePtr head, definedLabel *labelDataPtr);

/**
 * Get pointer to label data structure using label name
 * @param head pointer to database
 * @param name label name
 * @param destinationPtr address where to store pointer to the database
 * @return errorCodes enum value describing function success/failure
 */
errorCodes getLabelAttributes(databasePtr head, char *name, definedLabel **destinationPtr);

/**
 * After first pass, add offset to addresses of data labels
 * @param head pointer to database
 * @param offset how much to increment each address
 */
void updateDataLabels(databasePtr head, long offset);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearLabels(databasePtr head);

