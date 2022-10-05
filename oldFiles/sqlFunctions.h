#ifndef SQL_FUNCTIONS
#define SQL_FUNCTIONS

// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

// #include "globals.h"
#include "tables.h"
#include "utils.h"

void create_table(const char* table_name, const char* key, int length, ...);
void insert(const char* table_name, int length, ...);
void update(const char* table_name, int length, ...);
void select(const char* table_name, int length, ...);

#endif