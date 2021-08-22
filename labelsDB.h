#ifndef MAX_LINE
#include "data.h"
#endif

typedef struct definedLabel{
    labelID labelId;
    labelType type;
}definedLabel;


/**
 * Initialize labels database
 * @return pointer to the database
 */
labelsDBPtr initLabelsDB();

errorCodes addNewLabel(labelsDBPtr head, definedLabel *labelDataPtr);

errorCodes getLabelAttributes(labelsDBPtr head, char *name, definedLabel **destinationPtr);

void updateDataLabels(labelsDBPtr head, long offset);

/**
 * Free any memory allocated by the database
 * @param head pointer to the database
 */
void clearLabels(labelsDBPtr head);

