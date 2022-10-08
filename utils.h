#ifndef UTILS
#define UTILS

// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

#include "macros.h"

using namespace std;

// maybe we can still stick all of these guys in a class!
enum insertionMethod { ORDERED,
                       UNORDERED,
                       HASH };
enum dataType { VARCHAR,
                CHAR,
                SMALLINT,
                INTEGER,
                REAL,
                INVALID };

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

void addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum);
void printDBPrimary();
void* retrieveDBPrimaryRecord(char* table_name);

int addToDBAttr(char* attrName, char* typeStr);
void printDBAttr();

// should we make these references
void addFixedToTable(void* bufferToWrite, int recordSize, void* dbPrimaryPtr);
void addUnorderedToTable(void* bufferToWrite, int recordSize, void* dataRoot, void*& dataCurr, void*& dataCurrEnd, int& dataCurrBlockCount);
bool primaryKeyIsUnique();  // make sure that the primary key is unique in insertion
dataType getDataType(char* type);
int getN(char* str);
int calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes);

void printTable(char* table_name);  // very closely aligned with select

void create_table(const char* table_name, const char* key, int length, ...);
void insert(const char* table_name, int length, ...);
void update(const char* table_name, int length, ...);
void select(const char* table_name, int length, ...);

#endif