#ifndef DATABASE
#define DATABASE

// c

#include <stdint.h>
#include <string.h>

// cpp

#include <iostream>
#include <vector>

#include "macros.h"

using namespace std;

class Database {
 public:
  Database();
  Database(insertionMethod method);
  Database(insertionMethod method, int blockSize);
  void create_table(const char* table_name, const char* key, int length, ...);
  void insert(const char* table_name, int length, ...);
  void update(const char* table_name, int length, ...);
  void select(const char* table_name, int length, ...);

  // TODO: make everything under here private - how does this affect the testing suite?
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

  // some useful sizes
  int db_primary_record_size = TABLE_NAME_SIZE + 4 * (sizeof(void*)) + 4 * sizeof(int);
  int db_attr_record_size = TABLE_NAME_SIZE + 2 * sizeof(int);

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

  void initializeDB();
  void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount);
  void addNewBlock(void*& curr, void*& currEnd, int& currBlockCount);
  void addNewDataBlock(void*& dbPrimaryPtr);
  // allocate new space if needed
  void checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount);
  // moves pointers to the next blocks if necessary
  bool checkSpaceSearch(int fixedLength, void*& currRead, void*& currReadEnd);  // maybe this will work b/c of scoping

  void checkSpaceAddData(int spaceNeeded, void* dBPrimaryRecord);

  // DBPrimary
  void addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum);
  void printDBPrimary();
  void* retrieveDBPrimaryRecord(char* table_name);
  // would it make sense to make a getter for all of these?

  // DBAttr
  int addToDBAttr(char* attrName, char* typeStr);
  void printDBAttr();
  void* findEndOfBlock(void* currRecord);

  // DataTable
  void addFixedToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);
  void addVariableToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);
  void addUnorderedToTable(void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);

  void addOrderedToTableFixed(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint);
  void addOrderedToTableVariable(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint);

  void* findPrimaryKeyFixed(void* dbPrimaryPtr, void* pkToFind);
  void* findPrimaryKeyVariable(void* dbPrimaryPtr, void* pkToFind);
  void printTable(char* table_name);  // very closely aligned with select
  void printTableGiven(char* table_name, vector<char*> fieldsToPrint, char* condition);
  // other helper functions
  dataType getDataType(char* type);
  int getN(char* str);
  int calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes);
  vector<char*> parseOnDelim(char* toParse, char* delim);
  bool compare(char* targetValue, char* comparator, void* candidate, int candidateLength, dataType type);
  void freeDataTable(void* dataRoot, int dataCurrBlockCount);
};

#endif