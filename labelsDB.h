
typedef enum{
    UNIDENTIFIED_LABEL_TYPE,
    CODE_LABEL,
    DATA_LABEL,
    EXTERN_LABEL
}labelType;


typedef struct definedLabel{
    labelID labelId;
    labelType type;
}definedLabel;


/**
 * Initialize labels database
 * @return pointer to the database
 */
databasePtr initLabelsDB();

errorCodes addNewLabel(databasePtr head, definedLabel *labelDataPtr);

errorCodes getLabelAttributes(databasePtr head, char *name, definedLabel **destinationPtr);

void updateDataLabels(databasePtr head, long offset);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearLabels(databasePtr head);

