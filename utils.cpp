#include "utils.h"

int insertionMethod = ORDERED;
// pointers to important blocks and some useful information
// db_primary format:
// would we also want the number of records? yeah that's super useful for binary search
// I think we'll just calculate the max size of the db_attr every time, that's ok
// char(TABLE_NAME_SIZE) table_name | (void*) db_attr (first) | (int) num_db_attr | (int) primary_key_num  | (void*) data_root | (void*) dataCurr | (void*) dataEnd  | (int) numCurrBlockCount| (int) currRecordCount
void* db_primary = NULL;
void* db_primary_curr = NULL;
void* db_primary_curr_end = NULL;
int db_primary_records = 0;
int db_primary_blocks = 0;
// maybe we could also return an array of all the exact pointers to the attributes. this would let us work with it pretty easily in multiple places

// char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
void* db_attr = NULL;
void* db_attr_curr = NULL;
void* db_attr_curr_end = NULL;
int db_attr_records = 0;
int db_attr_blocks = 0;

// some useful sizes
int db_primary_record_size = TABLE_NAME_SIZE + 4 * (sizeof(void*)) + 4 * sizeof(int);
int db_attr_record_size = TABLE_NAME_SIZE + 2 * sizeof(int);

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

void addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum) {
  // make the size of name exact
  char buffer[TABLE_NAME_SIZE];
  memset(buffer, '\0', TABLE_NAME_SIZE);  // fill it with blanks
  if (strlen(name) > TABLE_NAME_SIZE) {
    strncpy(buffer, name, TABLE_NAME_SIZE);
  } else {
    strncpy(buffer, name, strlen(name));
  }

  checkSpaceAdd(db_primary_record_size, db_primary_curr, db_primary_curr_end, db_primary_blocks);

  // copy name in
  strncpy((char*)db_primary_curr, buffer, TABLE_NAME_SIZE);
  (char*&)db_primary_curr += TABLE_NAME_SIZE;

  // db_attr pointer
  *(long*)db_primary_curr = (uintptr_t)dbAttributes;
  ((long*&)db_primary_curr)++;

  // num dbAttr
  *(int*)db_primary_curr = numAttributes;
  ((int*&)db_primary_curr)++;

  // primaryKeyNum
  *(int*)db_primary_curr = primaryKeyNum;
  ((int*&)db_primary_curr)++;

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

  *(int*)db_primary_curr = 0;  // dataCurrRecordCount
  ((int*&)db_primary_curr)++;

  db_primary_records++;
}

void printDBPrimary() {
  // read heads
  void* ptr = db_primary;
  void* ptrEnd = (void*)(((long*)((uintptr_t)ptr + (uintptr_t)BLOCK_SIZE)) - 1);
  cout << "name| void* db_attr | int num_db_attr | int primary_key_num | void* data_root | void* data_curr | void* data end | int numDataBlocks| int numDataRecords" << endl;

  for (int i = 0; i < db_primary_records; i++) {
    // edge case: make sure our pointers are in the right place according to the blocks
    checkSpaceSearch(db_primary_record_size, ptr, ptrEnd);
    // 1 name
    char buffer[TABLE_NAME_SIZE];
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    cout << buffer << " | ";
    (char*&)ptr += TABLE_NAME_SIZE;

    // db_attribute stuff
    cout << (void*)*(long*)ptr << " | ";
    ((long*&)ptr)++;

    cout << *(int*)ptr << " | ";
    ((int*&)ptr)++;

    cout << *(int*)ptr << " | ";
    ((int*&)ptr)++;

    // 3 void pointers for data block
    for (int j = 0; j < 3; j++) {
      cout << (void*)*(long*)ptr << " | ";
      ((long*&)ptr)++;
    }

    // 2 ints
    cout << *(int*)ptr << " | ";
    ((int*&)ptr)++;

    cout << *(int*)ptr << " ";
    ((int*&)ptr)++;
    cout << endl;
  }
}

// can adapt to a linear search
void* retrieveDBPrimaryRecord(char* table_name) {
  void* ptr = db_primary;
  void* ptrEnd = (void*)(((long*)((uintptr_t)ptr + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < db_primary_records; i++) {
    checkSpaceSearch(db_primary_record_size, ptr, ptrEnd);
    char buffer[TABLE_NAME_SIZE];
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    if (strcmp(buffer, table_name) == 0) {
      cout << "found db_primary record for " << buffer << endl;
      return ptr;
    }
    ptr = (void*)((uintptr_t)ptr + db_primary_records);  // add the recordSizeOffset
  }
  return NULL;  // the record was not found
}

// char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
int addToDBAttr(char* attrName, char* typeStr) {
  checkSpaceAdd(db_attr_record_size, db_attr_curr, db_attr_curr_end, db_attr_blocks);
  strncpy((char*)db_attr_curr, attrName, TABLE_NAME_SIZE);
  (char*&)db_attr_curr += TABLE_NAME_SIZE;

  int variable = -1;

  dataType type = getDataType(typeStr);
  *(dataType*)db_attr_curr = type;
  ((dataType*&)db_attr_curr)++;

  if (type == VARCHAR || type == CHAR) {
    variable = getN(typeStr);
  }
  *(int*)db_attr_curr = variable;
  ((int*&)db_attr_curr)++;

  db_attr_records++;
  return variable;
}

// print out all of the valid records
void printDBAttr() {
  void* ptr = db_attr;
  void* ptrEnd = (void*)(((long*)((uintptr_t)ptr + (uintptr_t)BLOCK_SIZE)) - 1);

  cout << "char(TABLE_NAME_SIZE) attrName | (int) type | (int) length" << endl;
  for (int i = 0; i < db_attr_records; i++) {
    checkSpaceSearch(db_attr_record_size, ptr, ptrEnd);

    // attr name
    char buffer[TABLE_NAME_SIZE];
    memset(buffer, '\0', TABLE_NAME_SIZE);
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    cout << buffer << " | ";
    (char*&)ptr += TABLE_NAME_SIZE;

    // type
    cout << *(int*)ptr << " | ";
    ((int*&)ptr)++;

    // length
    cout << *(int*)ptr << endl;
    ((int*&)ptr)++;
  }
}

void addFixedToTable(void* bufferToWrite, int recordSize, void* dbPrimaryPtr) {
  // figure out all the pointer arithmetic here
  void* ptr = (void*)((uintptr_t)dbPrimaryPtr + TABLE_NAME_SIZE + sizeof(void*) + sizeof(int));  // for an extra level of protection
  int primaryKeyNumber = *(int*)ptr;
  ((int*&)ptr)++;
  void* dataRoot = (void*)*(long*)ptr;
  ((long*&)ptr)++;
  void* dataCurr = (void*)*(long*)ptr;
  ((long*&)ptr)++;
  void* dataCurrEnd = (void*)*(long*)ptr;
  ((long*&)ptr)++;
  int dataCurrBlockCount = *(int*)ptr;
  ((int*&)ptr)++;

  // TODO: FIGURE OUT IF THE PRIMARY KEY IS UNIQUE

  if (insertionMethod == ORDERED) {
    addUnorderedToTable(bufferToWrite, recordSize, dataRoot, dataCurr, dataCurrEnd, dataCurrBlockCount);
  }

  // increment the number of dataRecords
  (*(int*)ptr)++;
}

void addUnorderedToTable(void* bufferToWrite, int recordSize, void* dataRoot, void*& dataCurr, void*& dataCurrEnd, int& dataCurrBlockCount) {
  checkSpaceAdd(recordSize, dataCurr, dataCurrEnd, dataCurrBlockCount);
  memcpy(dataCurr, bufferToWrite, recordSize);
  // just like that!
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

int calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes) {
  void* ptr = ptrToFirstAttribute;  // just precautionary
  int toReturn = 0;
  for (int i = 0; i < numberOfAttributes; i++) {
    ptr = (void*)((uintptr_t)ptr + TABLE_NAME_SIZE);
    dataType dt = *(dataType*)ptr;
    if (dt == VARCHAR || dt == CHAR) {
      ((dataType*&)ptr)++;
      toReturn += *(int*)ptr;
      ((int*&)ptr)++;
    } else if (dt == SMALLINT) {
      toReturn += sizeof(short);
      ((dataType*&)ptr)++;
      ((int*&)ptr)++;
    } else if (dt == INTEGER || dt == REAL) {
      toReturn += sizeof(int);
      ((dataType*&)ptr)++;
      ((int*&)ptr)++;
    }
  }

  return toReturn;
}

// TODO: a bunch of things changed here
void create_table(const char* table_name, const char* key, int length, ...) {
  va_list arg_list;
  va_start(arg_list, length);
  cout << "CREATE TABLE " << table_name << endl;

  int primKey = 0;
  void* dbAttrStart = db_attr_curr;

  for (int i = 0; i < length / 2; i++) {
    char* name = va_arg(arg_list, char*);
    char* typeStr = va_arg(arg_list, char*);
    if (strcmp(name, key) == 0) {
      primKey = i;
    }

    addToDBAttr(name, typeStr);

    printf(" %-20s %s, \n", name, typeStr);
  }

  printf(" %-20s %s, \n", "PRIMARY KEY", key);

  // oh this is where the bug was coming from
  addToDBPrimary((char*)table_name, dbAttrStart, length / 2, primKey);
  printDBPrimary();
  printDBAttr();

  va_end(arg_list);
}

void insert(const char* table_name, int length, ...) {
  // now we have to worry about the setting we are using
  va_list arg_list;
  va_start(arg_list, length);
  cout << "INSERT INTO " << table_name << " VALUES ";
  // first find the table that we want to insert into
  void* record = retrieveDBPrimaryRecord((char*)table_name);
  if (record == NULL) {
    cerr << "cannot find specified table" << endl;
    return;
  }

  void* temp = record;  // level of protection
  char buffer[TABLE_NAME_SIZE];
  strncpy(buffer, (char*)temp, TABLE_NAME_SIZE);

  // HOW TO GET A POINTER OUT THE RIGHT WAY:
  void* dbAttrRecord = (void*)*(long*)((char*&)temp + TABLE_NAME_SIZE);
  cout << "dbAttrRecord " << dbAttrRecord << endl;
  int numAttributes = *(int*)((uintptr_t)temp + TABLE_NAME_SIZE + sizeof(void*));
  int maxDataRecordSize = calculateMaxDataRecordSize(dbAttrRecord, numAttributes);
  void* tempBuffer = calloc(maxDataRecordSize, 1);
  void* tempBufferPtr = tempBuffer;

  cout << numAttributes << " " << length << endl;
  if (length != numAttributes) {
    cerr << "Number of attributes does not match" << endl;
  }

  // can't separate out b/c of the va_arg
  for (int i = 0; i < length; i++) {
    // we have to know the dataType that's coming in
    dbAttrRecord = (void*)((uintptr_t)dbAttrRecord + TABLE_NAME_SIZE);
    dataType type = *(dataType*)dbAttrRecord;
    ((dataType*&)dbAttrRecord)++;

    if (type == VARCHAR) {
      cout << "varchar";
      char* at = va_arg(arg_list, char*);
      cout << at << " " << endl;
      int n = *(int*)dbAttrRecord;
      // copy as is, trim down if necessary
      if (strlen(at) > n) {
        strncpy((char*)tempBufferPtr, at, n);
        (char*&)tempBufferPtr += n;
      } else {
        strncpy((char*)tempBufferPtr, at, strlen(at));
        (char*&)tempBufferPtr += strlen(at);
      }
    } else if (type == CHAR) {
      cout << "char ";
      char* at = va_arg(arg_list, char*);
      cout << at << endl;
      int n = *(int*)dbAttrRecord;
      // copy with filler, trim down if necessary
      char bufferN[n];
      memset(bufferN, '\0', n);
      if (strlen(at) > n) {
        strncpy(bufferN, at, n);
      } else {
        strncpy(bufferN, at, strlen(at));
      }
      strncpy((char*)tempBufferPtr, bufferN, n);
      (char*&)tempBufferPtr += n;

    } else if (type == SMALLINT) {
      cout << "smallint ";
      short at = (short)va_arg(arg_list, int);
      cout << at << endl;
      *(short*)tempBufferPtr = at;
      ((short*&)tempBufferPtr)++;

    } else if (type == INTEGER) {
      cout << "integer ";
      int at = va_arg(arg_list, int);
      cout << at << endl;
      *(int*)tempBufferPtr = at;
      ((int*&)tempBufferPtr)++;

    } else if (type == REAL) {
      cout << "real";
      double at = va_arg(arg_list, double);
      cout << at << endl;
      *(double*)tempBufferPtr = at;
      ((double*&)tempBufferPtr)++;
    }
    ((int*&)dbAttrRecord)++;
  }

  int checkRecordSize = (uintptr_t)tempBufferPtr - (uintptr_t)tempBuffer;
  cout << "check record size: " << checkRecordSize << endl;
  if (checkRecordSize == 0) {  // fixed
    addFixedToTable(tempBuffer, maxDataRecordSize, record);
  } else {  // variable
  }

  // I was thinking of using a try catch for something... what was it?

  va_end(arg_list);
}

// TODO: do this one last
void update(const char* table_name, int length, ...) {
}

void select(const char* table_name, int length, ...) {
  // TODO: different search methods

  // will have to parse the WHERE

  // unordered: linear
  // ordered: fixed - binary search, variable - linear
  // hashing : ???
}