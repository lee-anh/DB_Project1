#include "Database.h"

Database::Database() {
  this->method = UNORDERED;
  initializeDB();
}

Database::Database(insertionMethod method) {
  this->method = method;
  initializeDB();
}

void Database::create_table(const char* table_name, const char* key, int length, ...) {
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

  addToDBPrimary((char*)table_name, dbAttrStart, length / 2, primKey);
  printDBPrimary();
  printDBAttr();

  va_end(arg_list);
}

void Database::insert(const char* table_name, int length, ...) {
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
  // cout << "dbAttrRecord " << dbAttrRecord << endl;
  int numAttributes = *(int*)((uintptr_t)temp + TABLE_NAME_SIZE + sizeof(void*));
  int maxDataRecordSize = calculateMaxDataRecordSize(dbAttrRecord, numAttributes);
  void* tempBuffer = calloc(maxDataRecordSize, 1);
  void* tempBufferPtr = tempBuffer;

  if (length != numAttributes) {
    cerr << "Number of attributes does not match" << endl;
  }
  bool variable = false;
  // can't separate out b/c of the va_arg
  for (int i = 0; i < length; i++) {
    // we have to know the dataType that's coming in
    dbAttrRecord = (void*)((uintptr_t)dbAttrRecord + TABLE_NAME_SIZE);
    dataType type = *(dataType*)dbAttrRecord;
    ((dataType*&)dbAttrRecord)++;

    if (type == VARCHAR) {
      // cout << "varchar";
      char* at = va_arg(arg_list, char*);
      cout << at;
      int n = *(int*)dbAttrRecord;
      // copy as is, trim down if necessary
      if (strlen(at) > n) {
        strncpy((char*)tempBufferPtr, at, n);
        (char*&)tempBufferPtr += n;
      } else {
        strncpy((char*)tempBufferPtr, at, strlen(at));
        (char*&)tempBufferPtr += strlen(at);
      }
      variable = true;
    } else if (type == CHAR) {
      // cout << "char ";
      char* at = va_arg(arg_list, char*);
      cout << at;
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
      // cout << "smallint ";
      short at = (short)va_arg(arg_list, int);
      cout << at;
      *(short*)tempBufferPtr = at;
      ((short*&)tempBufferPtr)++;

    } else if (type == INTEGER) {
      // cout << "integer ";
      int at = va_arg(arg_list, int);
      cout << at;
      *(int*)tempBufferPtr = at;
      ((int*&)tempBufferPtr)++;

    } else if (type == REAL) {
      // cout << "real ";
      // maybe an int would be better?
      float at = va_arg(arg_list, double);  // we have to read in as double, but can cast to a float
      cout << at;
      *(float*)tempBufferPtr = (float)at;
      ((float*&)tempBufferPtr)++;
    }
    ((int*&)dbAttrRecord)++;
    if (i == length - 1) {
      cout << endl;
    } else {
      cout << ", ";
    }
  }

  if (!variable) {  // fixed
    addFixedToTable(tempBuffer, maxDataRecordSize, record);
  } else {  // variable
    // TODO: variable
  }

  // I was thinking of using a try catch for something... what was it?
  printTable((char*)table_name);

  va_end(arg_list);
}

// TODO: do this one last
void Database::update(const char* table_name, int length, ...) {
}

void Database::select(const char* table_name, int length, ...) {
  // TODO: different search methods
  // TODO: do we have to do *
  // AND OR
  void* recordPtr = retrieveDBPrimaryRecord((char*)table_name);

  // have to read in the attribute names that we're interested in
  // have to account for the wild card

  // we'll want to store the db_primary

  // the last string is the one that we'll have to parse

  // we have to look up in db_attributes for the right offset

  // will have to parse the WHERE, could have AND OR this is not fun

  // unordered: linear
  // ordered: fixed - binary search, variable - linear
  // hashing : ???
}

void Database::initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end, db_attr_blocks);
}

// TODO: change level of these pointers? I'm not sure if it's going to work how we think
void Database::initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  currBlockCount = 1;
}

// already initialized block, root doesn't change

// curr and currEnd need to be pointers to that part of db_primary
void Database::addNewBlock(void*& curr, void*& currEnd, int& currBlockCount) {
  if (curr == NULL || currEnd == NULL) {
    // error
    cerr << "the root block has not been initialized yet" << endl;
    return;
  }
  // old

  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                                  // allocate space for this new block
  *(long*)currEnd = (uintptr_t)toAdd;                                          // add pointer to new block to the end of this block
  curr = toAdd;                                                                // update curr to be the beginning of this new block
  currEnd = (void*)(((long*)((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
  *(long*)currEnd = NULL;                                                      // set currEnd to be NULL for now
  currBlockCount++;

  // new - with curr and currEnd being pointers
  /*
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                         // allocate space for this new block
  *(long*)(*(long*)currEnd) = (uintptr_t)toAdd;                       // add pointer to new block to the end of this block
  *(long*)curr = (uintptr_t)toAdd;                                    // update curr to be the beginning of this new block
  currEnd = (void*)(((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE) - 1);  // update the end to be this new end
  *(long*)(*(long*)currEnd) = NULL;                                   // set currEnd to be NULL for now
  currBlockCount++;
  */

  /*
    // maybe we can switch out and in for good measure
    // 🚨 now everything is zeroed out, great
    void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                                      // allocate space for this new block
    **(long**)(&currEnd) = (uintptr_t)toAdd;                                         // add pointer to new block to the end of this block
    *(&curr) = toAdd;                                                                // update curr to be the beginning of this new block
    *(&currEnd) = (void*)(((long*)((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
    **(long**)(&currEnd) = NULL;                                                     // set currEnd to be NULL for now
    currBlockCount++;
    */
}

// TODO: check this
// curr and currEnd are the actual pointers to the beginning and end
// where are we using these/ are values preserved how we think they are?
void Database::checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount) {
  if ((uintptr_t)currEnd - (uintptr_t)curr < spaceNeeded) {
    addNewBlock(curr, currEnd, currBlockCount);
  }

  /*
  if (*(long*)currEnd - *(long*)curr < spaceNeeded) {
    addNewBlock(curr, currEnd, currBlockCount);
  }
  */

  // weird things going on, check the trace
}

// TODO: agh these also need to be changed to pointers I think 😬
bool Database::checkSpaceSearch(int fixedLength, void*& currRead, void*& currReadEnd) {
  if (fixedLength == -1) {  // variable length
    // search in the current block for a record terminating character
    // if we can't find it, then
    int spaceToCheck = (uintptr_t)currReadEnd - (uintptr_t)currRead;
    bool found = false;
    for (int i = 0; i < spaceToCheck; i++) {
      if (*(char*)currRead == END_RECORD_CHAR) {
        found = true;
        break;
      }
    }
    if (!found) {
      // time to get to the next block
      currRead = (void*)(*(long*)currReadEnd);
      if (currRead == NULL) {  // end of storage
        return false;
      }
      currReadEnd = (void*)(((long*)((uintptr_t)currRead + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
    }
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

void Database::addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum) {
  // make the size of name exact
  char buffer[TABLE_NAME_SIZE];
  memset(buffer, '\0', TABLE_NAME_SIZE);  // fill it with blanks
  if (strlen(name) > TABLE_NAME_SIZE) {
    strncpy(buffer, name, TABLE_NAME_SIZE);
  } else {
    strncpy(buffer, name, strlen(name));
  }
  void* db_primary_curr_address = &(db_primary_curr);
  void* db_primary_curr_end_address = &(db_primary_curr_end);

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

void Database::printDBPrimary() {
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
void* Database::retrieveDBPrimaryRecord(char* table_name) {
  void* ptr = db_primary;
  void* ptrEnd = (void*)(((long*)((uintptr_t)ptr + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < db_primary_records; i++) {
    checkSpaceSearch(db_primary_record_size, ptr, ptrEnd);
    char buffer[TABLE_NAME_SIZE];
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    if (strcmp(buffer, table_name) == 0) {
      // cout << "found db_primary record for " << buffer << endl;
      return ptr;
    }
    ptr = (void*)((uintptr_t)ptr + db_primary_records);  // add the recordSizeOffset
  }
  return NULL;  // the record was not found
}

// char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
int Database::addToDBAttr(char* attrName, char* typeStr) {
  // void* db_attr_curr_address = &db_attr_curr;  // so risky!
  // void* db_attr_curr_end_address = &db_attr_curr_end;
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
void Database::printDBAttr() {
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

void Database::addFixedToTable(void* bufferToWrite, int recordSize, void*& dbPrimaryPtr) {
  // figure out all the pointer arithmetic here
  void* ptr = (void*)((uintptr_t)dbPrimaryPtr + TABLE_NAME_SIZE + sizeof(void*) + sizeof(int));  // for an extra level of protection
  int primaryKeyNumber = *(int*)ptr;
  ((int*&)ptr)++;
  void* dataRoot = (void*)*(long*)ptr;
  ((long*&)ptr)++;
  cout << "MOM " << ptr << endl;  // 🚨 this is the reason! MOM != mom, makes switching in and out not work
  void* dataCurrPtr = ptr;        // (void*)*(long*)ptr;  // the actual pointer
  ((long*&)ptr)++;
  void* dataCurrEndPtr = ptr;  // (void*)*(long*)ptr;  // the actual pointer
  ((long*&)ptr)++;
  int dataCurrBlockCount = *(int*)ptr;
  ((int*&)ptr)++;
  cout << "got here" << endl;

  // TODO: FIGURE OUT IF THE PRIMARY KEY IS UNIQUE

  if (method == UNORDERED) {
    cout << "mom " << (dataCurrPtr) << endl;
    addUnorderedToTable(bufferToWrite, recordSize, dataRoot, dataCurrPtr, dataCurrEndPtr, dataCurrBlockCount);
  }

  cout << "got here 2" << endl;

  // increment the number of dataRecords
  (*(int*)ptr)++;
  // free the bufferToWrite
  free(bufferToWrite);
}

// ugh we are getting all confused with the pointers here
void Database::addUnorderedToTable(void* bufferToWrite, int recordSize, void* dataRoot, void*& dataCurr, void*& dataCurrEnd, int& dataCurrBlockCount) {
  void* dataCurrActual = (void*)*(long*)dataCurr;
  void* dataCurrEndActual = (void*)*(long*)dataCurrEnd;
  checkSpaceAdd(recordSize, dataCurrActual, dataCurrEndActual, dataCurrBlockCount);  // TODO: fix this
  cout << "ping" << endl;

  memcpy(dataCurrActual, bufferToWrite, recordSize);
  /*
  cout << "record Size: " << recordSize << endl;
  cout << "before: " << dataCurr << endl;
  cout << "switcheroo " << (void*)*(long*)(&dataCurr) << endl;
  cout << "mom " << (void*)(dataCurr) << endl;
  */
  *(long*)dataCurr = (uintptr_t)dataCurrActual + recordSize;

  //*(long*)(&dataCurr) = (long)((uintptr_t)dataCurr + recordSize);  // we need to be able to actually change this, otherwise we just keep overwriting the same old thing
}

// TODO: improve formatting to be like CREATE_TABLE
void Database::printTable(char* table_name) {
  // just do a select all from table? maybe do a select helper
  void* record = retrieveDBPrimaryRecord(table_name);
  void* ptr = (void*)((uintptr_t)record + TABLE_NAME_SIZE);  // for an extra level of protection
  void* dbAttrRecord = (void*)*(long*)ptr;
  // cout << dbAttrRecord << endl;
  ((long*&)ptr)++;
  int numAttr = *(int*)ptr;
  ((int*&)ptr)++;
  int primaryKeyNumber = *(int*)ptr;
  ((int*&)ptr)++;
  void* dataRoot = (void*)*(long*)ptr;
  ((long*&)ptr)++;
  // we don't really care about the write heads
  ((long*&)ptr)++;  // dataCurr
  ((long*&)ptr)++;  // dataCurrEnd
  int dataCurrBlockCount = *(int*)ptr;
  ((int*&)ptr)++;
  int dataCurrRecordCount = *(int*)ptr;
  ((int*&)ptr)++;

  // print out all of the attributes
  bool variable = false;  // useful to know I guess... how are we going to do variable records? special ending for a varchar and special ending for a record?
  dataType attrTypes[numAttr];
  void* attrRecords[numAttr];  // pointer to each attribute
  // print out the headers
  for (int i = 0; i < numAttr; i++) {
    // what are we doing here?
    attrRecords[i] = dbAttrRecord;

    char attrName[TABLE_NAME_SIZE];
    strncpy(attrName, (char*)dbAttrRecord, TABLE_NAME_SIZE);
    dbAttrRecord = (void*)((uintptr_t)dbAttrRecord + TABLE_NAME_SIZE);

    dataType type = *(dataType*)dbAttrRecord;
    attrTypes[i] = type;
    ((dataType*&)dbAttrRecord)++;
    if (type == VARCHAR) {
      variable = true;
    }

    cout << attrName;
    if (i == primaryKeyNumber) {
      cout << "*";
    }
    if (i == numAttr - 1) {
      cout << endl;
    } else {
      cout << " | ";
    }

    ((int*&)dbAttrRecord)++;  // skip over the size too
    // the next is our next record
  }

  // set up read heads
  void* dataRead = dataRoot;
  // cout << "dataRead Head: " << dataRead << endl;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  if (variable) {
    fixedLength = -1;
  }
  // we just need the attribute types so we know what to cast it into
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);
    // cout << "dataReadHead 2: " << dataRead << endl;
    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        // get max, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + TABLE_NAME_SIZE + sizeof(dataType));
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        // go character by character
        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_RECORD_CHAR) {
            break;
          }
          buff[k] = a;
        }
        // cout
        cout << buff;
        // while loop to append - go fix
        // cout

      } else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + TABLE_NAME_SIZE + sizeof(dataType));
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        strncpy(buff, (char*)dataRead, buffSize);
        // cout
        cout << buff;
        ((char*&)dataRead) += buffSize;

      } else if (type == SMALLINT) {
        cout << *(short*)dataRead;
        ((short*&)dataRead)++;
      } else if (type == INTEGER) {
        cout << *(int*)dataRead;
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        cout << *(float*)dataRead;
        ((float*&)dataRead)++;
      }
      if (j == numAttr - 1) {
        cout << endl;
      } else {
        cout << " | ";
      }
    }
  }

  // TODO: depending on fixed/variable we might have an end record we might have a record ending char or not that we will have to skip over

  // TODO: make a good method for retrieving fixed records and variable records
  // dbAttr
  // for each of them, how do we select the attributes that we want?
  // char** of the attributes we want
  // we still need the dbAttr to figure out what the offsets are
  // in the select we might also have a conditional -- how are we going to handle the conditional?

  // conditional
  // how do we pass in a comparator operator?  <, <=, ==, =>, >
  // https://stackoverflow.com/questions/4530588/passing-operator-as-a-parameter
  // 😬

  // we have to iterate through db_primary
  // get the attributes for the header
  // then iterate through and print out each data record
  // it matters whether or not we have a spanning block
  // might have been useful to store if the record is variable or not
  // rather than try to calculate it every time (even though we have enough info, each time we do it we have an increased chance of mistake)
  // indicate the primary key with a * when printing it out
}

dataType Database::getDataType(char* type) {
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

int Database::getN(char* str) {
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

int Database::calculateMaxDataRecordSize(void* ptrToFirstAttribute, int numberOfAttributes) {
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

// we would need to parse the sentence before and just hand off the operator
bool Database::compare(char* targetValue, char* comparator, void* candidate, int candidateLength, dataType type) {
  // need to parse the sentence
  // we can strstr to find
  // would be good to parse the before, the after and know the type (only varchar and char will be different)

  // how will we know the length of the VARCHAR? 😢
  if (type == VARCHAR || type == CHAR) {
    char str[candidateLength];
    memset(str, '\0', candidateLength);  // for good measure
    strncpy(str, (char*)candidate, candidateLength);

    int res = strcmp(targetValue, str);  // order matters
    // I think it might be better to return this
    if (strstr(comparator, "!=")) {
      if (res != 0) return true;
      return false;
    }
    if (strstr(comparator, "<=")) {
      if (res == 0 | res < 0) return true;
      return false;
    }
    if (strstr(comparator, ">=")) {
      if (res == 0 | res > 0) return true;
      return false;
    }
    if (strstr(comparator, "=")) {
      if (res == 0) return true;
      return false;
    }
    if (strstr(comparator, "<")) {
      if (res < 0) return true;
      return false;
    }
    if (strstr(comparator, ">")) {
      if (res > 0) return true;
      return false;
    }
  } else if (type == SMALLINT) {
    short tar = (short)atoi(targetValue);
    short num = *(short*)candidate;
    // we might have some scoping issues if we try to make this fancier

    // sadly we need to know the types to compare tar and num, so we can't optimize
    if (strstr(comparator, "!=")) {
      return tar != num;
    }
    if (strstr(comparator, "<=")) {
      return tar <= num;
    }
    if (strstr(comparator, ">=")) {
      return tar >= num;
    }
    if (strstr(comparator, "=")) {
      return tar >= num;
    }
    if (strstr(comparator, "<")) {
      return tar < num;
    }
    if (strstr(comparator, ">")) {
      return tar > num;
    }
  } else if (type == INTEGER) {
    int tar = atoi(targetValue);
    int num = *(int*)candidate;
    if (strstr(comparator, "!=")) {
      return tar != num;
    }
    if (strstr(comparator, "<=")) {
      return tar <= num;
    }
    if (strstr(comparator, ">=")) {
      return tar >= num;
    }
    if (strstr(comparator, "=")) {
      return tar >= num;
    }
    if (strstr(comparator, "<")) {
      return tar < num;
    }
    if (strstr(comparator, ">")) {
      return tar > num;
    }
  } else if (type == REAL) {
    float tar = (float)atof(targetValue);
    float num = *(float*)candidate;
    if (strstr(comparator, "!=")) {
      return tar != num;
    }
    if (strstr(comparator, "<=")) {
      return tar <= num;
    }
    if (strstr(comparator, ">=")) {
      return tar >= num;
    }
    if (strstr(comparator, "=")) {
      return tar >= num;
    }
    if (strstr(comparator, "<")) {
      return tar < num;
    }
    if (strstr(comparator, ">")) {
      return tar > num;
    }
  }
  return false;
}