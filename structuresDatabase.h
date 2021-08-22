
/**
 * Initialize a database
 * @return pointer to database
 */
databasePtr initDatabase();

/**
 * Check if a given database has been used since initialized
 * @param head pointer to database
 * @return
 */
boolean isDBEmpty(databasePtr head);

/**
 * Run through a database, get address of last entry
 * @param head pointer to database
 * @return pointer to last unit
 */
databasePtr seekLastDatabaseEntry(databasePtr head);

/**
 * Add an entry to a database
 * @param lastUnitAddress address of last entry
 * @param sizeOfData size of data unit being added
 * @return pointer to new entry's data
 */
void * addNewDatabaseEntry(databasePtr lastUnitAddress, int sizeOfData);

/**
 * Get pointer to current entry data
 * @param unitAddress address of current entry in database
 * @return pointer to the data
 */
void *getEntryDataPtr(databasePtr unitAddress);

/**
 * Get address of next entry in database
 * @param unitAddress address of current entry in database
 * @return Address of next entry
 */
databasePtr getNextEntryAddress(databasePtr unitAddress);

/**
 * Free any memory allocated by given database
 * @param head pointer to database
 */
void clearDatabase(databasePtr head);
