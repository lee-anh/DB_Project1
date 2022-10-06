#ifndef UTILS
#define UTILS

// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

using namespace std;
enum dataType { VARCHAR,
                CHAR,
                SMALLINT,
                INTEGER,
                REAL,
                INVALID };

inline const int BLOCK_SIZE = 1024;
inline const int TABLE_NAME_SIZE = 64;
inline const int TEXT_MAX_SIZE = 256;
inline const char END_RECORD_CHAR = '~';

void initializeDB();
void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount);
void addNewBlock(void*& curr, void*& currEnd, int& currBlockCount);
// allocate new space if needed
void checkSpaceAdd(int spaceNeeded, void* curr, void* currEnd, int currBlockCount);

// check if we can read the next record safely?
// maybe we could also store the last pointer? really need to work this out
// especially for varchar ones we just won't know until we find

// TODO:
// curr is the write head
// we also need to have a curr read head
// when we're reading, we will also need a currReadEnd head
bool checkSpaceSearch(int fixedLength, void* currRead, void* currReadEnd);
// maybe we could find end of record

// as we traverse, how do we keep track of what block we're on?
// pointer arithmetic? keep going
// we could also keep track of how many blocks we have

// dbPrimary is not spanning
void addToDBPrimary(char* name, void* dbAttributes);
void printDBPrimary();

// dbAttr is going to be spanning
bool writeDataTypeToDBAttr(char* typeStr);
void printDBAttr();

// should we make these references
void addUnorderedToTable(void* root);

dataType getDataType(char* type);
int getN(char* str);
int calculateMaxRecordSize(int* dbAttrArr);

void* retrieveDBPrimaryRecord(char* table_name);
int* retrieveDBAttrRecord(void* dbAttrPtr);

void create_table(const char* table_name, const char* key, int length, ...);
void insert(const char* table_name, int length, ...);
void update(const char* table_name, int length, ...);
void select(const char* table_name, int length, ...);

#endif