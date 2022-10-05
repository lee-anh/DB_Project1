#ifndef TABLES
#define TABLES
// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

// #include "globals.h"

using namespace std;

void initializeDB();
void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount);
void addNewBlock(void* curr, void* currEnd);

// values have already been added to dbAttributes
void addToDBPrimary(char* name, void* dbAttributes);

void addToDBAttr();

// allocate new space if needed
void checkSpaceAdd(int spaceNeeded, void* curr, void* currEnd, int currBlockCount);

// check if we can read the next record safely?
// maybe we could also store the last pointer? really need to work this out
// especially for varchar ones we just won't know until we find

// TODO:
// curr is the write head
// we also need to have a curr read head
// when we're reading, we will also need a currReadEnd head
void checkSpaceSearch(bool variable, void* currRead, void* currReadEnd);
// maybe we could find end of record

// as we traverse, how do we keep track of what block we're on?
// pointer arithmetic? keep goini
// we could also keep track of how many blocks we have

void printDBPrimary();
void printDBAttr();

// TODO:
// fixed record length
// function that appends the records to db_primary
// function that reads records from  db_primary

// variable record length
// function that appends records to db_attr
// function that reads records in db_attr

// eventually it will be time to parse the strings as well

#endif