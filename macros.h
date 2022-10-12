#ifndef MACROS
#define MACROS

#define BLOCK_SIZE 512
#define TABLE_NAME_SIZE 64
#define TEXT_MAX_SIZE 256
#define END_FIELD_CHAR '`'
#define END_RECORD_CHAR '~'

enum insertionMethod { UNORDERED,
                       ORDERED,
                       HASH };
enum dataType { VARCHAR,
                CHAR,
                SMALLINT,
                INTEGER,
                REAL,
                INVALID };

#endif