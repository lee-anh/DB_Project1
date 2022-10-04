#ifndef TABLES
#define TABLES
// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

#include "globals.h"

using namespace std;

void initializeDB();
void initializeNewBlock(void* root, void* curr, void* currEnd);
void addNewBlock(void* curr, void* currEnd);

// TODO:
// fixed record length
// function that appends the records to db_primary
// function that reads records in db_primary

// variable record length
// function that appends records to db_attr
// function that reads records in db_attr

// eventually it will be time to parse the strings as well

#endif