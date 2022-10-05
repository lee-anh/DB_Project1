#ifndef GLOBALS
#define GLOBALS

// it's ok that these are inline
const int BLOCK_SIZE = 1024;
const int TABLE_NAME_SIZE = 64;
const int TEXT_MAX_SIZE = 256;
const char END_RECORD_CHAR = '~';

enum dataType { VARCHAR,
                CHAR,
                SMALLINT,
                INTEGER,
                REAL,
                INVALID };

/*
typedef struct Pointers {
  void* root;
  void* curr;
  void* currEnd;
  int records;
  int blocks;

} Pointers;


inline Pointers db_primary_1;
*/

// TODO: figure out how to make these global properly
void* db_primary = NULL;
void* db_primary_curr = NULL;
void* db_primary_curr_end = NULL;
int db_primary_records = 0;
int db_primary_blocks = 0;  // useful data information

void* db_attr = NULL;
void* db_attr_curr = NULL;
void* db_attr_curr_end = NULL;
int db_attr_records = 0;
int db_attr_blocks = 0;

#endif