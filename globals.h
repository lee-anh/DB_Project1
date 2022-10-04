#ifndef GLOBALS
#define GLOBALS

const int BLOCK_SIZE = 1024;

// maybe this would be an appropriate use for a struct?
// ehh we could do without it

void* db_primary = NULL;
void* db_primary_curr = NULL;
void* db_primary_curr_end = NULL;

void* db_attr = NULL;
void* db_attr_curr = NULL;
void* db_attr_curr_end = NULL;

// TODO: create enums for the different datatypes
// having these trios is useful for writing
// having the beginning of the block is useful for searching

// for each root we would also have to store this kind of information
// perhaps it can be become part of the record for db_primary

// won't we need a lot of things to be global?
// like the beginning
// end pointer of current block
// how would we calculate spanning offsets?  should be ok because it's just all addresses
// current pointer (so we can add new stuff)

#endif