#include "utils.h"

dataType getDataType(char* type) {
  if (strstr(type, "varchar") != NULL) {
    return VARCHAR;
  }  // always check varchar before checking char, substring issues
  if (strstr(type, "char") != NULL) {
    return CHAR;
  }
  if (strstr(type, "smallint") != NULL) {
    return SMALLINT;
  }
  if (strstr(type, "integer") != NULL) {
    return INTEGER;
  }
  if (strstr(type, "real") != NULL) {
    return REAL;
  }
  return INVALID;
}

int getN(char* str) {
  // we can find the opening ( and closing ) and then return between the two
  char* open = strstr(str, "(");
  char toInt[4];
  // brute force
  for (int i = 0; i < 4; i++) {
    if (*open == ')') {
      break;
    }
    toInt[i] = *open;
    open++;
  }
  return atoi(toInt);
}

void initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end, db_attr_blocks);

  db_attr = (void*)calloc(BLOCK_SIZE, 1);  // how could this happen?
  db_attr_curr = db_attr;
  db_attr_curr_end = (void*)(((long*)((uintptr_t)db_attr + (uintptr_t)BLOCK_SIZE)) - 1);
  db_attr_blocks++;
  // printDBPrimary();
  // printDBAttr();
}

// not tested
void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  currBlockCount++;
  return;
}

// not tested
// not a new block, root doesn't change
void addNewBlock(void* curr, void* currEnd, int currBlockCount) {
  if (curr == NULL || currEnd == NULL) {
    // error
    cerr << "the root block has not been initialized yet" << endl;
    return;
  }
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                                  // allocate space for this new block
  *(long*)currEnd = (uintptr_t)toAdd;                                          // add pointer to new block to the end of this block
  curr = toAdd;                                                                // update curr to be the beginning of this new block
  currEnd = (void*)(((long*)((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
  currBlockCount++;
}

void addToDBPrimary(char* name, void* dbAttributes) {
  // TODO: need to check the length of name
  // TODO: need to check for the edge case where we need more space in the block
}

// allocate new space if needed
// would be much better if we could pass around a struct :(
void checkSpaceAdd(int spaceNeeded, void* curr, void* currEnd, int currBlockCount) {
}

// not tested
void printDBPrimary() {
  // TODO: handle edge cases
  void* ptr = db_primary;
  // name | void* db_attr | void* data_root | void* data_curr | void* data end | int numDataRecords
  cout << "name\t| void* db_attr\t| void* data_root\t| void* data_curr\t| void* data end\t| int numDataRecords" << endl;
  for (int i = 0; i < db_attr_records; i++) {
    // time for some pointer stuff
    char buffer[TABLE_NAME_SIZE];
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    cout << buffer << "\t";
    (char*&)ptr += TABLE_NAME_SIZE;

    // 4 void pointers
    for (int j = 0; j < 4; j++) {
      cout << (void*)*(long*)ptr << "\t";
      ((long*&)ptr)++;
    }

    // 1 int
    cout << *(int*)ptr << "\t";
    cout << endl;
  }

  // how to navigate across the blocks that span???
  // we could just print the records

  // eventually for binary search we'll have to know how many records it's holding
}

void printDBAttr() {
  // this is pretty in comprehensible stuff but it should be all ints
  void* ptr = db_attr;
  while (ptr != db_attr_curr_end) {
    cout << *(int*)ptr << " ";
  }
  cout << endl;
}

void create_table(const char* table_name, const char* key, int length, ...) {
  va_list arg_list;
  va_start(arg_list, length);
  cout << "CREATE TABLE " << table_name << endl;

  void* ptr = db_attr_curr;

  bool variable = false;

  char* name = va_arg(arg_list, char*);
  char* typeStr = va_arg(arg_list, char*);
  dataType type = getDataType(typeStr);

  if (db_attr == NULL) {
    cout << "db_attr is NULL! " << endl;
  }

  // add to db_attributes
  // TODO: check for edge cases - what if the block doesn't have any more room?!
  // we're getting a seg fault... maybe something to do with the globals

  /*
    strncpy((char*)ptr, "hello", strlen("hello"));
    (char*&)ptr += strlen("hello");
    strncpy((char*)ptr, "world", strlen("world"));
    (char*&)ptr += strlen("world");
    */
  /*

    for (int i = 0; i < length / 2; i++) {
      char* name = va_arg(arg_list, char*);
      char* typeStr = va_arg(arg_list, char*);
      dataType type = getDataType(typeStr);

      // add to db_attributes
      // TODO: check for edge cases - what if the block doesn't have any more room?!
      // we're getting a seg fault. maybe checking for this will help

      *(dataType*)ptr = SMALLINT;
      cout << "dataType: " << *(dataType*)ptr << endl;
      ((dataType*&)ptr)++;

      *(dataType*)ptr = type;
      ((dataType*&)ptr)++;

      if (type == VARCHAR || type == CHAR) {
        if (type == VARCHAR)
          variable = true;

        int n = 1;  // getN(typeStr);
        *(int*)ptr = n;
        ((int*&)ptr)++;
      }
      db_attr_records++;


      // print to terminal
      printf(" %-20s %s, \n", name, typeStr);
    }
    */

  // *(bool*)db_attr_curr = variable;
  // ((bool*&)db_attr_curr)++;

  printf(" %-20s %s, \n", "PRIMARY KEY", key);

  // addToDBPrimary((char*)table_name, db_attr_curr_ptr);

  va_end(arg_list);
}

void insert(const char* table_name, int length, ...) {
}

void update(const char* table_name, int length, ...) {
}

void select(const char* table_name, int length, ...) {
}