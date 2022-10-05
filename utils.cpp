#include "utils.h"

// TODO: figure out how to make these global properly
void* db_primary = NULL;
void* db_primary_curr = NULL;
void* db_primary_curr_end = NULL;
int db_primary_records = 0;
int db_primary_blocks = 0;

void* db_attr = NULL;
void* db_attr_curr = NULL;
void* db_attr_curr_end = NULL;
int db_attr_records = 0;
int db_attr_blocks = 0;

void initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end, db_attr_blocks);
}

void initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  currBlockCount = 0;
}

// already initialized block, root doesn't change
void addNewBlock(void*& curr, void*& currEnd, int& currBlockCount) {
  if (curr == NULL || currEnd == NULL) {
    // error
    cerr << "the root block has not been initialized yet" << endl;
    return;
  }
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                                  // allocate space for this new block
  *(long*)currEnd = (uintptr_t)toAdd;                                          // add pointer to new block to the end of this block
  curr = toAdd;                                                                // update curr to be the beginning of this new block
  currEnd = (void*)(((long*)((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
  *(long*)currEnd = NULL;                                                      // set currEnd to be NULL for now
  currBlockCount++;
}

// allocate new space if needed
void checkSpaceAdd(int spaceNeeded, void* curr, void* currEnd, int currBlockCount) {
  if ((uintptr_t)currEnd - (uintptr_t)curr < spaceNeeded) {
    addNewBlock(curr, currEnd, currBlockCount);
  }
}

bool checkSpaceSearch(int fixedLength, void* currRead, void* currReadEnd) {
  // if variable, then we have to search for the terminating character
  if (fixedLength == -1) {  // variable length

  } else {
    // how are the read heads going to work?
    // currReadEnd can be calculated when we set it at first
    // maybe we can check to see if it's NULL
    // maybe outside of the function we'll know if we are on the last block or not
    // and maybe we can return that
    if ((uintptr_t)currReadEnd - (uintptr_t)currRead < fixedLength) {
      // time to get to the next block
      currRead = (void*)(*(long*)currReadEnd);
      if (currRead == NULL) {  // end of storage
        return false;
      }
      currReadEnd = (void*)(((long*)((uintptr_t)currRead + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
    }
  }
  return true;
}

void addToDBPrimary(char* name, void* dbAttributes) {
  // make the size of name exact
  char buffer[TABLE_NAME_SIZE];
  memset(buffer, '\0', TABLE_NAME_SIZE);  // fill it with blanks
  if (strlen(name) > TABLE_NAME_SIZE) {
    strncpy(buffer, name, TABLE_NAME_SIZE);
  } else {
    strncpy(buffer, name, strlen(name));
  }
  int spaceNeeded = TABLE_NAME_SIZE + 4 * (sizeof(void*)) + sizeof(int);
  checkSpaceAdd(spaceNeeded, db_primary_curr, db_primary_curr_end, db_primary_blocks);

  // copy name in
  strncpy((char*)db_primary_curr, buffer, TABLE_NAME_SIZE);
  (char*&)db_primary_curr += TABLE_NAME_SIZE;

  // db_attr pointer
  *(long*)db_primary_curr = (uintptr_t)dbAttributes;
  ((long*&)db_primary_curr)++;

  // make a new block
  void* dataRoot = NULL;
  void* dataCurr = NULL;
  void* dataCurrEnd = NULL;
  int dataCurrBlockCount = 0;
  initializeNewBlock(dataRoot, dataCurr, dataCurrEnd, dataCurrBlockCount);
  *(long*)db_primary_curr = (uintptr_t)dataRoot;
  ((long*&)db_primary_curr)++;
  *(long*)db_primary_curr = (uintptr_t)dataCurr;
  ((long*&)db_primary_curr)++;
  *(long*)db_primary_curr = (uintptr_t)dataCurrEnd;
  ((long*&)db_primary_curr)++;
  *(int*)db_primary_curr = dataCurrBlockCount;
  ((int*&)db_primary_curr)++;

  db_primary_records++;
}

void printDBPrimary() {
  // read heads
  void* ptr = db_primary;
  void* ptrEnd = (void*)(((long*)((uintptr_t)ptr + (uintptr_t)BLOCK_SIZE)) - 1);
  cout << "name| void* db_attr| void* data_root | void* data_curr | void* data end | int numDataRecords" << endl;
  int recordSpace = TABLE_NAME_SIZE + 4 * (sizeof(void*)) + sizeof(int);
  for (int i = 0; i < db_primary_records; i++) {
    // edge case: make sure our pointers are in the right place according to the blocks
    checkSpaceSearch(recordSpace, ptr, ptrEnd);
    // 1 name
    char buffer[TABLE_NAME_SIZE];
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    cout << buffer << " | ";
    (char*&)ptr += TABLE_NAME_SIZE;

    // 4 void pointers
    for (int j = 0; j < 4; j++) {
      cout << (void*)*(long*)ptr << " | ";
      ((long*&)ptr)++;
    }

    // 1 int
    cout << *(int*)ptr << " ";
    cout << endl;
  }
  // eventually for binary search we'll have to know how many records it's holding
  // binary search might be hard because how will we know where the nth record will be in memory?
  // we'll have to do some fancy maths
}
bool writeDataTypeToDBAttr(char* typeStr) {
  // TODO: check for edge cases - what if the block doesn't have any more room?!
  // we will need to check this before getting here to make sure they aren't spanning
  bool variable = false;
  dataType type = getDataType(typeStr);
  *(dataType*)db_attr_curr = type;
  ((dataType*&)db_attr_curr)++;

  if (type == VARCHAR || type == CHAR) {
    if (type == VARCHAR)
      variable = true;
    int n = getN(typeStr);
    *(int*)db_attr_curr = n;
    ((int*&)db_attr_curr)++;
  }
  db_attr_records++;
  // how will we know where one record ends? add null terminating
  return variable;
}
void printDBAttr() {
  // all of DBAttr should be ints
  void* ptr = db_attr;
  while (ptr != db_attr_curr_end) {
    int curr = *(int*)ptr;

    cout << curr << " ";
    if (curr < 0) {
      cout << endl;
    }
    ((int*&)ptr)++;
  }
  cout << endl;
}

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
  open++;
  for (int i = 0; i < 4; i++) {
    if (*open == ')') {
      break;
    }
    toInt[i] = *open;
    open++;
  }
  return atoi(toInt);
}

void create_table(const char* table_name, const char* key, int length, ...) {
  va_list arg_list;
  va_start(arg_list, length);
  cout << "CREATE TABLE " << table_name << endl;

  bool variable = false;

  for (int i = 0; i < length / 2; i++) {
    char* name = va_arg(arg_list, char*);
    char* typeStr = va_arg(arg_list, char*);

    // add to db_attr
    // how will we know if we have enough space?
    variable = writeDataTypeToDBAttr(typeStr);

    printf(" %-20s %s, \n", name, typeStr);
  }

  // write variable to db_attr
  // the negative character will indicate the end of the record
  // if -1 we know its variable
  // if -2 we know its fixed
  if (variable) {
    *(int*)db_attr_curr = -1;
  } else {
    *(int*)db_attr_curr = -2;
  }

  ((int*&)db_attr_curr)++;

  printf(" %-20s %s, \n", "PRIMARY KEY", key);

  addToDBPrimary((char*)table_name, db_attr_curr);
  printDBPrimary();
  printDBAttr();

  va_end(arg_list);
}

void insert(const char* table_name, int length, ...) {
  // now we have to worry about the setting we are using
  va_list arg_list;
  va_start(arg_list, length);
  cout << "INSERT INTO " << table_name << "VALUES ";

  for (int i = 0; i < length; i++) {
    cout << va_arg(arg_list, char*) << " ";
  }
  // we don't know what the values are going to be

  // TODO: different insertion methods

  va_end(arg_list);
}

// TODO: do this one last
void update(const char* table_name, int length, ...) {
}

void select(const char* table_name, int length, ...) {
  // TODO: different search methods
  // unordered: linear
  // ordered: fixed - binary search, variable - linear
  // hashing : ???
}