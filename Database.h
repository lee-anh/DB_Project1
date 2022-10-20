#ifndef DATABASE
#define DATABASE

#include <stdint.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "macros.h"

using namespace std;

class Database {
 public:
  /// @brief standard constructor
  /// method = UNORDERED
  /// BLOCK_SIZE = 512
  Database();

  /// @brief constructor, can specify insertion method
  /// @param method insertionMethod
  /// BLOCK_SIZE = 512
  Database(insertionMethod method);

  /// @brief constructor, cam specify insertion method and block size
  /// @param method
  /// @param blockSize
  Database(insertionMethod method, int blockSize);

  /// @brief create_table sql statement
  /// @param table_name table to create
  /// @param key the name of the primary key attribute
  /// @param length the number of variadic arguments
  /// @param variadic arguments - attribute name followed by type
  void create_table(const char* table_name, const char* key, int length, ...);

  /// @brief insert sql statement
  /// @param table_name table to insert into
  /// @param length number of attributes associated with the table
  /// @param  values of attributes to add in
  void insert(const char* table_name, int length, ...);

  /// @brief update sql statement
  /// @param table_name table to update
  /// @param length should be 3
  /// @param  first should be name of attribute to update, second is value to update to, last is a comparison
  void update(const char* table_name, int length, ...);

  /// @brief select sql statement
  /// @param table_name table to select from
  /// @param length
  /// @param  attributes to print out given a condition
  void select(const char* table_name, int length, ...);

  /// @brief free all the allocated memory associated with the database
  void closeDB();

  /*********ideally would make everything below this private, but exposing it for the sake of testing **********/
  // instance variables
  insertionMethod method;
  int BLOCK_SIZE;

  // db_primary format:
  // char(TABLE_NAME_SIZE) table_name | (void*) db_attr (first) | (int) num_db_attr | (int) primary_key_num  | (void*) data_root | (void*) dataCurr | (void*) dataEnd  | (int) numCurrBlockCount| (int) currRecordCount
  void* db_primary = NULL;
  void* db_primary_curr = NULL;
  void* db_primary_curr_end = NULL;
  int db_primary_records = 0;
  int db_primary_blocks = 0;
  // maybe we could also return an array of all the exact pointers to the attributes. this would let us work with it pretty easily in multiple places

  // char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
  void* db_attr = NULL;
  void* db_attr_curr = NULL;
  void* db_attr_curr_end = NULL;
  int db_attr_records = 0;
  int db_attr_blocks = 0;

  // hashed record
  // (void*) dataRoot | (void*) dataCurr | (void*) dataCurrEnd | (int) dataNumBlocks | (int) dataNumRecords

  // some useful sizes
  int db_primary_record_size = TABLE_NAME_SIZE + 4 * (sizeof(void*)) + 4 * sizeof(int);
  int db_attr_record_size = TABLE_NAME_SIZE + 2 * sizeof(int);
  int hashed_size = 3 * sizeof(void*) + 2 * sizeof(int);

  int db_primary_db_attr_offset = TABLE_NAME_SIZE;
  int db_primary_num_db_attr_offset = TABLE_NAME_SIZE + sizeof(void*);
  int primary_key_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int);
  int data_root_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int) * 2;
  int data_curr_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int) * 2 + sizeof(void*);
  int data_curr_end_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int) * 2 + sizeof(void*) * 2;
  int data_block_count_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int) * 2 + sizeof(void*) * 3;
  int data_record_count_offset = TABLE_NAME_SIZE + sizeof(void*) + sizeof(int) * 2 + sizeof(void*) * 3 + sizeof(int);

  int attr_type_offset = TABLE_NAME_SIZE;
  int attr_length_offset = TABLE_NAME_SIZE + sizeof(dataType);

  int hash_curr_offset = sizeof(void*);
  int hash_curr_end_offset = sizeof(void*) * 2;
  int hash_block_count_offset = sizeof(void*) * 3;
  int hash_record_count_offset = sizeof(void*) * 3 + sizeof(int);
  int num_hash_buckets;

  /// @brief initialize db_primary and db_attr
  void initializeDB();

  /// @brief malloc space for a new block
  /// @param root the root block
  /// @param curr current write head
  /// @param currEnd end of current write head
  /// @param currBlockCount number of blocks associated with root
  void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount);

  /// @brief addNewBlock add a new dbPrimary block or dbAttr block
  /// @param curr the current write head
  /// @param currEnd end of current block for curr write head
  /// @param currBlockCount the number of blocks associated with this table
  void addNewBlock(void*& curr, void*& currEnd, int& currBlockCount);

  /// @brief addNew dataBlock to dbPrimary
  /// @param dbPrimaryPtr the record for the table to add a new block to
  void addNewDataBlock(void*& dbPrimaryPtr);

  /// @brief add a new dataBlock to a hashed bucket
  /// @param bucketPtr the bucket to add the new block to
  void addNewDataBlockHashed(void*& bucketPtr);

  /// @brief check to see if there is space on the current block to add another record
  /// allocate new space if needed
  /// @param spaceNeeded record size
  /// @param curr current block write head
  /// @param currEnd end of current block
  /// @param currBlockCount the number of current block
  void checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount);

  /// @brief see if there is space on the current block to add data to
  /// @param spaceNeeded record size
  /// @param dBPrimaryRecord the associated record for the table
  void checkSpaceAddData(int spaceNeeded, void* dBPrimaryRecord);

  /// @brief see if there is space in the current bucket
  /// @param spaceNeeded record size
  /// @param bucketPtr the associated bucket to add to
  void checkSpaceAddDataHashed(int spaceNeeded, void* bucketPtr);

  /// @brief check to see if we can continue searching on the current block, move pointers to next block if necessary
  /// @param fixedLength space we want to search
  /// @param currRead the current read head
  /// @param currReadEnd end of the block of the current read head
  /// @return
  bool checkSpaceSearch(int fixedLength, void*& currRead, void*& currReadEnd);  // maybe this will work b/c of scoping

  // DBPrimary

  /// @brief add a new table to db primary
  /// @param name the name of the table to add
  /// @param dbAttributes pointer to the first attribute in dbAttributes
  /// @param numAttributes number of attributes associated with the table
  /// @param primaryKeyNum which attribute is the primaryKeyNum
  /// @return
  void* addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum);

  /// @brief add a new table to db primary hash table
  /// @param name the name of the table to add
  /// @param dbAttributes pointer to the first attribute in dbAttributes
  /// @param numAttributes number of attributes associated with the table
  /// @param primaryKeyNum which attribute is the primaryKeyNum
  void addToDBPrimaryHashed(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum);

  /// @brief print out the contents of db primary
  void printDBPrimary();

  /// @brief retrieve the right record from db primary given a table name
  /// @param table_name
  /// @return
  void* retrieveDBPrimaryRecord(char* table_name);

  /// @brief add a new attribute to db attribute
  /// @param attrName the name of the attribute
  /// @param typeStr string form of the type
  /// @return
  int addToDBAttr(char* attrName, char* typeStr);

  /// @brief print out the contents of db attr
  void printDBAttr();
  /// @brief  find the end of the block given a db attr pointer
  /// @param currRecord the current pointer to find the end of the block for
  /// @return a pointer to the end of the block
  void* findEndOfBlock(void* currRecord);

  // DataTable functions

  /// @brief add a fixed record to the table
  /// @param primaryKey the primary key to add
  /// @param bufferToWrite
  /// @param recordSize number of bytes of the record
  /// @param dbPrimaryPtr table to add this record to
  void addFixedToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);

  /// @brief add a variable record to the table
  /// @param primaryKey the primary key to add
  /// @param bufferToWrite
  /// @param recordSize number of bytes of the record
  /// @param dbPrimaryPtr table to add this record to
  void addVariableToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);

  /// @brief unordered insert
  /// @param bufferToWrite the copy of the record to add
  /// @param recordSize the size of the record to add in bytes
  /// @param dbPrimaryPtr pointer to the record to add to
  void addUnorderedToTable(void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);

  /// @brief unordered insert for hash tables
  /// @param bufferToWrite copy of the record to add
  /// @param recordSize the size of the record to add in bytes
  /// @param bucketPtr the bucket to add the table
  void addUnorderedToTableHashed(void* bufferToWrite, int recordSize, void*& bucketPtr);

  /// @brief ordered insert for fixed records
  /// @param bufferToWrite copy of the record to add
  /// @param primaryKey
  /// @param recordSize size of the record to add in bytes
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param insertionPoint where to add the record to
  void addOrderedToTableFixed(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint);

  /// @brief ordered insert for variable records
  /// @param bufferToWrite copy of the record to add
  /// @param primaryKey
  /// @param recordSize size of the record to add in bytes
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param insertionPoint where to add the record to
  void addOrderedToTableVariable(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint);

  /// @brief hashed insert for records
  /// @param bufferToWrite copy of the record to add
  /// @param recordSize size of the record to add in bytes
  /// @param primaryKey
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  void addHashedToTable(void* bufferToWrite, int recordSize, void* primaryKey, void*& dbPrimaryPtr);

  /// @brief find the pointer to the record with the given primary key for fixed records
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param pkToFind the primary key to find
  /// @return the pointer to the record w/given primary key or an insertion point to where it should be (negative)
  void* findPrimaryKeyFixed(void* dbPrimaryPtr, void* pkToFind);
  /// @brief find the record with the given primary key for fixed records when hashing is used
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param pkToFind the primary key to find
  /// @return true if primary key was found, false if not
  bool findPrimaryKeyHashedFixed(void* dbPrimaryPtr, void* pkToFind);
  /// @brief find the pointer to the record with the given primary key for variable records
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param pkToFind the primary key to find
  /// @return the pointer to the record w/given primary key or an insertion point to where it should be (negative)
  void* findPrimaryKeyVariable(void* dbPrimaryPtr, void* pkToFind);

  /// @brief find the record with the given primary key for variable records when hashing is used
  /// @param dbPrimaryPtr the pointer to the record associated with the table
  /// @param pkToFind the primary key to find
  /// @return true if primary key was found, false if not
  bool findPrimaryKeyHashedVariable(void* dbPrimaryPtr, void* pkToFind);

  /// @brief print the contents of a table
  /// @param table_name
  void printTable(char* table_name);

  /// @brief print the contents of a hashed table
  /// @param table_name
  void printTableHashed(char* table_name);

  /// @brief print a table given certain attributes and a condition
  /// @param table_name
  /// @param fieldsToPrint attributes to print
  /// @param specialName the name of the attribute that we are trying to compare
  /// @param comparator a string of a comparator ex. >, =, <
  /// @param target the target value that we want the specialName attribute to be compared with
  void printTableGiven(char* table_name, vector<char*> fieldsToPrint, char* specialName, char* comparator, char* target);

  /// @brief print a table given certain attributes and a condition, for hashed records
  /// @param table_name table to update
  /// @param fieldsToPrint attributes to print
  /// @param specialName the name of the attribute that we are trying to compare
  /// @param comparator a string of a comparator ex. >, =, <
  /// @param target the target value that we want the specialName attribute to be compared with
  void printTableGivenHashed(char* table_name, vector<char*> fieldsToPrint, char* specialName, char* comparator, char* target);

  /// @brief a helper for printing tables
  /// @param variable whether the record is variable or fixed
  /// @param numAttr number of attributes associated with this table
  /// @param numToLookFor the number of attributes to look for
  /// @param dataRoot the root to the table with the data
  /// @param dataCurrRecordCount the number of data records
  /// @param fixedLength the length of the records
  /// @param attrRecords an array of pointers to each attribute record
  /// @param attrTypes an array of the types of each attribute
  /// @param interest an array if the attribute is of interest to print
  /// @param isTarget an array if the record meets the condition to print
  void printTableGivenHelper(bool variable, int numAttr, int numToLookFor, void* dataRoot, int dataCurrRecordCount, int fixedLength, void* attrRecords[], dataType attrTypes[], bool interest[], bool isTarget[]);

  /// @brief update the table given an attribute to update and a condition
  /// @param table_name table to update
  /// @param attrToUpdate the attribute to update
  /// @param value to update the attribute to
  /// @param specialName the name of the attribute to compare
  /// @param comparator a string of a comparator ex. >, =, <
  /// @param target the target value that we want the specialName attribute to be compared with
  /// @return the number of records updated
  int updateTableGiven(char* table_name, char* attrToUpdate, char* value, char* specialName, char* comparator, char* target);

  /// @brief update the table given an attribute to update and a condition for hashed insertion method
  /// @param table_name table to update
  /// @param attrToUpdate the attribute to update
  /// @param value to update the attribute to
  /// @param specialName the name of the attribute to compare
  /// @param comparator a string of a comparator ex. >, =, <
  /// @param target the target value that we want the specialName attribute to be compared with
  /// @return the number of records updated
  int updateTableGivenHashed(char* table_name, char* attrToUpdate, char* value, char* specialName, char* comparator, char* target);

  /// @brief helper to updateTableGiven, changes fixed records
  /// @param dataCurrRecordCount the number of records associated with the table
  /// @param fixedLength the length in bytes of the record
  /// @param dataRead current read head
  /// @param dataReadEnd end of current read head's block
  /// @param numAttr number of attributes associated with record
  /// @param attrRecords an array of pointers to each attribute record
  /// @param attrTypes an array of the types of each attribute
  /// @param interest an array if the attribute is of interest to print
  /// @param isTarget an array if the record meets the condition to print
  /// @param value the target value that we want the specialName attribute to be compared with
  void updateTableGivenFixedHelper(int dataCurrRecordCount, int fixedLength, void* dataRead, void* dataReadEnd, int numAttr, void* attrRecords[], dataType attrTypes[], bool isTarget[], int toUpdate, char* value);

  /// @brief a duplicate of update() but returns number of values update, used for testing
  /// @param table_name the table to update
  /// @param length should be 3
  /// @param  first should be the attribute to update, second is the conditional
  /// @return number of records updated
  int updateTest(const char* table_name, int length, ...);

  /// @brief helper to updateTableGiven, changes variable records
  /// @param record to update
  /// @param numAttr number of attributes associated with record
  /// @param attrRecords an array of pointers to each attribute record
  /// @param isTarget an array if the record meets the condition to print
  /// @param attrTypes an array of the types of each attribute
  /// @param fixedLength the length in bytes of the record
  /// @param dataCurrRecordCount the number of records associated with the table
  /// @param dataRead current read head
  /// @param dataReadEnd end of current read head's block
  /// @param toUpdate the number of the attribute to update
  /// @param value the target value that we want the specialName attribute to be compared with
  void updateTableGivenVariableHelper(void*& record, int numAttr, void* attrRecords[], bool isTarget[], dataType attrTypes[], int fixedLength, int dataCurrRecordCount, void* dataRead, void* dataReadEnd, int toUpdate, char* value);

  /// @brief helper to updateTableGivenHashed, changes variable records
  /// @param bucketBlock the bucket number to update
  /// @param numAttr number of attributes associated with record
  /// @param attrRecords an array of pointers to each attribute record
  /// @param isTarget an array if the record meets the condition to print
  /// @param attrTypes an array of the types of each attribute
  /// @param fixedLength the length in bytes of the record
  /// @param recordsInBlock the number of records associated with the current bucket
  /// @param toUpdate the number of the attribute to update
  /// @param value the target value that we want the specialName attribute to be compared with
  void updateTableGivenVariableHelperHashed(void*& bucketBlock, int numAttr, void* attrRecords[], bool isTarget[], dataType attrTypes[], int fixedLength, int recordsInBlock, int toUpdate, char* value);

  // void updateTableFixed(char* table_name, char* attrToUpdate, char* value, bool* isTarget);
  // other helper functions

  /// @brief get the data type given a string describing the type
  /// @param type a string
  /// @return the dataType
  dataType getDataType(char* type);

  /// @brief get the type of the primary key
  /// @param dbAttributes pointer to dbAttributes, first record associated with table
  /// @param numAttributes number of attributes associated with the table
  /// @param primaryKeyNum the number attribute that the primary key is
  /// @return the type of the primary key
  dataType getPrimaryKeyType(void* dbAttributes, int numAttributes, int primaryKeyNum);

  /// @brief get the length of the primary key, useful if primary key is VARCHAR or CHAR
  /// @param dbAttributes pointer to dbAttributes, first record associated with table
  /// @param numAttributes  number of attributes associated with the table
  /// @param primaryKeyNum the number attribute that the primary key is
  /// @return the length of the primary key
  int getPrimaryKeyLength(void* dbAttributes, int numAttributes, int primaryKeyNum);

  /// @brief computes the bucket number based on hash code
  /// @param pk primary key to hash
  /// @param pkLength length of primary key if VARCHAR or CHAR
  /// @param pkType the dataType of the primary key
  /// @return
  int getBucketNumber(void* pk, int pkLength, dataType pkType);

  /// @brief get the n from a create table for CHAR and VARCHAR
  /// @param str string to parse
  /// @return the n in the string
  int getN(char* str);

  /// @brief calculate the maximum that the record size could be based on the attribute types
  /// @param ptrToFirstAttribute
  /// @param numberOfAttributes associated with the record
  /// @return max number of bytes the record could be
  int calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes);

  /// @brief return the value of a comparison
  /// @param targetValue value to compare to
  /// @param comparator ex. >, =, <
  /// @param candidate value to compare
  /// @param candidateLength length of value (useful if VARCHAR or CHAR)
  /// @param type candidate/targetValue dataType
  /// @return
  bool compare(char* targetValue, char* comparator, void* candidate, int candidateLength, dataType type);

  /// @brief free the malloc'd space for a chain of data blocks, given the root
  /// @param dataRoot
  /// @param dataCurrBlockCount
  void freeDataTable(void* dataRoot, int dataCurrBlockCount);
};

#endif