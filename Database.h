#ifndef UTILS
#define UTILS

// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

#include "macros.h"

using namespace std;

class Database {
 public:
  Database();
  Database(insertionMethod method);
  void create_table(const char* table_name, const char* key, int length, ...);
  void insert(const char* table_name, int length, ...);
  void update(const char* table_name, int length, ...);
  void select(const char* table_name, int length, ...);

  // TODO: make everything under here private - how does this affect the testing suite?
  // instance variables
  insertionMethod method;
  // pointers to important blocks and some useful information
  // db_primary format:
  // would we also want the number of records? yeah that's super useful for binary search
  // I think we'll just calculate the max size of the db_attr every time, that's ok
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

  void initializeDB();
  void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount);
  void addNewBlock(void*& curr, void*& currEnd, int& currBlockCount);

  // allocate new space if needed
  void checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount);
  // moves pointers to the next blocks if necessary
  bool checkSpaceSearch(int fixedLength, void*& currRead, void*& currReadEnd);

  // DBPrimary
  void addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum);
  void printDBPrimary();
  void* retrieveDBPrimaryRecord(char* table_name);
  // would it make sense to make a getter for all of these?

  // DBAttr
  int addToDBAttr(char* attrName, char* typeStr);
  void printDBAttr();

  // DataTable
  void addFixedToTable(void* bufferToWrite, int recordSize, void*& dbPrimaryPtr);
  void addUnorderedToTable(void* bufferToWrite, int recordSize, void* dataRoot, void*& dataCurr, void*& dataCurrEnd, int& dataCurrBlockCount);
  bool primaryKeyIsUnique();          // make sure that the primary key is unique in insertion
  void printTable(char* table_name);  // very closely aligned with select

  // other helper functions
  dataType getDataType(char* type);
  int getN(char* str);
  int calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes);
  bool compare(char* targetValue, char* comparator, void* candidate, int candidateLength, dataType type);
};

#endif