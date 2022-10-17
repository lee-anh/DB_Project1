#include "Database.h"

Database::Database() {
  this->method = UNORDERED;
  this->BLOCK_SIZE = 512;
  initializeDB();
}

Database::Database(insertionMethod method) {
  this->method = method;
  this->BLOCK_SIZE = 512;
  initializeDB();
}

Database::Database(insertionMethod method, int blockSize) {
  this->method = method;
  BLOCK_SIZE = blockSize;
  initializeDB();
}

void Database::create_table(const char* table_name, const char* key, int length, ...) {
  void* test = retrieveDBPrimaryRecord((char*)table_name);
  if (test != nullptr) {
    cerr << "Table of same name has already been created" << endl;
    return;
  }
  va_list arg_list;
  va_start(arg_list, length);

  int primKey = 0;
  // missed putting this here earlier! 🥲
  checkSpaceAdd(db_attr_record_size, db_attr_curr, db_attr_curr_end, db_attr_blocks);
  void* dbAttrStart = db_attr_curr;

  // using a vectors, but it's just for an intermediate step 🙏🏼
  vector<char*> attrNames;
  vector<char*> types;

  // TODO: make sure attribute names are short enough

  for (int i = 0; i < length / 2; i++) {
    char* name = va_arg(arg_list, char*);
    // check for unique attribute names
    for (char* n : attrNames) {
      if (strcmp(n, name) == 0) {
        cerr << "Error creating " << table_name << ". Attributes cannot have the same name." << endl;
        return;
      }
    }
    attrNames.push_back(name);

    char* typeStr = va_arg(arg_list, char*);

    types.push_back(typeStr);
    if (strcmp(name, key) == 0) {
      primKey = i;
    }
  }

  cout << "CREATE TABLE " << table_name << endl;

  for (int i = 0; i < (int)attrNames.size(); i++) {
    addToDBAttr(attrNames[i], types[i]);

    printf(" %-20s %s, \n", attrNames[i], types[i]);
  }

  printf(" %-20s %s, \n", "PRIMARY KEY", key);

  addToDBPrimary((char*)table_name, dbAttrStart, length / 2, primKey);

  va_end(arg_list);
}

void Database::insert(const char* table_name, int length, ...) {
  va_list arg_list;
  va_start(arg_list, length);
  cout << "INSERT INTO " << table_name << " VALUES ";
  // first find the table that we want to insert into

  void* record = retrieveDBPrimaryRecord((char*)table_name);
  if (record == NULL) {
    cerr << "cannot find specified table" << endl;
    return;
  }

  // cout << "found record" << endl;
  char buffer[TABLE_NAME_SIZE];
  memset(buffer, '\0', TABLE_NAME_SIZE);
  strncpy(buffer, (char*)record, TABLE_NAME_SIZE);

  void* dbAttrRecord = (void*)*(long*)((char*&)record + db_primary_db_attr_offset);
  void* dbAttrRecordEnd = findEndOfBlock(dbAttrRecord);
  int numAttributes = *(int*)((uintptr_t)record + db_primary_num_db_attr_offset);
  int pkNumber = *(int*)((uintptr_t)record + primary_key_offset);
  int maxDataRecordSize = calculateMaxDataRecordSize(dbAttrRecord, numAttributes);
  void* tempBuffer = calloc(maxDataRecordSize, 1);
  void* tempBufferPtr = tempBuffer;

  if (length != numAttributes) {
    cerr << "Number of attributes does not match" << endl;
  }
  bool variable = false;
  void* pk = nullptr;

  for (int i = 0; i < length; i++) {
    // dummy me forgot the check space search?
    checkSpaceSearch(db_attr_record_size, dbAttrRecord, dbAttrRecordEnd);

    // we have to know the dataType that's coming in
    dbAttrRecord = (void*)((uintptr_t)dbAttrRecord + TABLE_NAME_SIZE);
    dataType type = *(dataType*)dbAttrRecord;
    // cout << "dataType " << type << endl;
    ((dataType*&)dbAttrRecord)++;

    if (type == VARCHAR) {
      char* at = va_arg(arg_list, char*);
      cout << at;
      int n = *(int*)dbAttrRecord;

      // copy as is, trim down if necessary
      if (strlen(at) > n - 2) {
        char* temp = (char*)tempBufferPtr;
        strncpy((char*)tempBufferPtr, at, n - 2);
        (char*&)tempBufferPtr += (n - 2);
        ((char*&)tempBufferPtr)++;               // empty space
        *(char*)tempBufferPtr = END_FIELD_CHAR;  // add end of field char
        ((char*&)tempBufferPtr)++;

        if (i == pkNumber) {
          pk = calloc(n - 1, sizeof(char));

          strncpy((char*)pk, temp, n - 1);
        }

      } else {
        char* temp = (char*)tempBufferPtr;
        strncpy((char*)tempBufferPtr, at, strlen(at));
        (char*&)tempBufferPtr += strlen(at);
        *(char*)tempBufferPtr = END_FIELD_CHAR;  // add end of field char
        ((char*&)tempBufferPtr)++;

        if (i == pkNumber) {
          pk = calloc(strlen(at) + 1, sizeof(char));

          strncpy((char*)pk, temp, strlen(at));
        }
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

      if (i == pkNumber) {
        pk = calloc(n, sizeof(char));
        strncpy((char*)pk, bufferN, n);
      }

    } else if (type == SMALLINT) {
      // cout << "smallint ";
      short at = (short)va_arg(arg_list, int);
      cout << at;
      *(short*)tempBufferPtr = at;
      ((short*&)tempBufferPtr)++;

      if (i == pkNumber) {
        pk = malloc(sizeof(short));
        *(short*)pk = at;
      }

    } else if (type == INTEGER) {
      // cout << "integer ";
      int at = va_arg(arg_list, int);
      cout << at;
      *(int*)tempBufferPtr = at;
      ((int*&)tempBufferPtr)++;

      if (i == pkNumber) {
        pk = malloc(sizeof(int));
        *(int*)pk = at;
      }

    } else if (type == REAL) {
      // cout << "real ";
      float at = va_arg(arg_list, double);  // we have to read in as double, but can cast to a float
      cout << at;
      *(float*)tempBufferPtr = (float)at;
      ((float*&)tempBufferPtr)++;

      if (i == pkNumber) {
        pk = malloc(sizeof(float));
        *(float*)pk = at;
      }
    }
    ((int*&)dbAttrRecord)++;
    if (i == length - 1) {
      cout << endl;
    } else {
      cout << ", ";
    }
  }

  if (!variable) {  // fixed
    addFixedToTable(pk, tempBuffer, maxDataRecordSize, record);
  } else {  // variable
    // TODO: variable
    *(char*)tempBufferPtr = END_RECORD_CHAR;
    ((char*&)tempBufferPtr)++;
    addVariableToTable(pk, tempBuffer, maxDataRecordSize, record);
  }

  // I was thinking of using a try catch for something... what was it?
  // printTable((char*)table_name);
  va_end(arg_list);
}

void Database::update(const char* table_name, int length, ...) {
  // make an array of the attribute names that we're looking for
  va_list arg_list;
  va_start(arg_list, length);

  cout << "UPDATE " << table_name;

  // get the attribute to update

  // get the attribute to set
  char* attrToUpdate = strdup(va_arg(arg_list, char*));
  // TODO:  we need to look up the type of value first, for now passing it as a char* is going to be fine
  char* value = strdup(va_arg(arg_list, char*));
  cout << " SET " << attrToUpdate << " = " << value;

  // For update we MUST have a WHERE clause
  // parse the WHERE
  char* sentence = va_arg(arg_list, char*);
  char* cpy = strdup(sentence);
  char* attrName = nullptr;
  char* comparator = nullptr;
  char* target = nullptr;

  int counter = 0;

  char* token = strtok(cpy, " ");
  while (token) {
    if (counter == 0) {
      attrName = token;
    } else if (counter == 1) {
      comparator = token;
    } else if (counter == 2) {
      target = token;
    }

    token = strtok(NULL, " ");

    counter++;
  }

  /*
  cout << "attrName " << attrName << endl;
  cout << "comparator " << comparator << endl;
  cout << "target " << target << endl;
  */
  int numUpdated = updateTableGiven((char*)table_name, attrToUpdate, value, attrName, comparator, target);

  cout << " WHERE " << sentence;

  cout << endl;
  cout << "You have made changes to the database. Rows affected: " << numUpdated << endl;
  va_end(arg_list);
}

void Database::select(const char* table_name, int length, ...) {
  // make an array of the attribute names that we're looking for
  va_list arg_list;
  va_start(arg_list, length);

  char* fieldsToParse = va_arg(arg_list, char*);

  // using a vector, but it's just for an intermediate step 🙏🏼
  vector<char*> fields;

  if (strcmp(fieldsToParse, "*") == 0) {
    fields.push_back(strdup(fieldsToParse));

  } else {
    // figure out how many attributes we're looking for and remove white space

    char local[strlen(fieldsToParse) + 1];
    memset(local, '\0', strlen(fieldsToParse) + 1);
    strcpy(local, fieldsToParse);

    int index = 0;
    char* token = strtok(local, " ,");
    while (token) {
      fields.push_back(strdup(token));
      token = strtok(NULL, " ,");
      index++;
    }
  }

  // TODO: do the where clause
  // conditional
  // how do we pass in a comparator operator?  <, <=, ==, =>, >
  // https://stackoverflow.com/questions/4530588/passing-operator-as-a-parameter
  // 😬
  cout << "SELECT ";

  for (int i = 0; i < (int)fields.size(); i++) {
    cout << fields[i];
    if (i != (int)fields.size() - 1) {
      cout << ", ";
    }
  }
  cout << " FROM " << table_name;

  char* sentence;
  char* attrName = nullptr;
  char* comparator = nullptr;
  char* target = nullptr;
  if (length == 2) {  // we also have a WHERE clause

    sentence = va_arg(arg_list, char*);
    char* cpy = strdup(sentence);

    int counter = 0;

    char* token = strtok(cpy, " ");
    while (token) {
      if (counter == 0) {
        attrName = token;
      } else if (counter == 1) {
        comparator = token;
      } else if (counter == 2) {
        target = token;
      }

      token = strtok(NULL, " ");

      counter++;
    }

    /*
    cout << "attrName " << attrName << endl;
    cout << "comparator " << comparator << endl;
    cout << "target " << target << endl;
    */

    cout << " WHERE " << sentence;
  }

  // unordered: linear
  // ordered: fixed - binary search, variable - linear
  // hashing : ???
  cout << endl;
  printTableGiven((char*)table_name, fields, attrName, comparator, target);
  for (int i = 0; i < (int)fields.size(); i++) {
    free(fields[i]);
  }
  va_end(arg_list);
}

void Database::initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end, db_attr_blocks);
}

void Database::initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  currBlockCount = 1;
}

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
}

void Database::addNewDataBlock(void*& dbPrimaryPtr) {
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);
  **(long**)((uintptr_t)dbPrimaryPtr + data_curr_end_offset) = (uintptr_t)toAdd;                                       // *dataCurrEnd = toAdd
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset) = (uintptr_t)toAdd;                                             // dataCurr = toadd
  *(int*)((uintptr_t)dbPrimaryPtr + data_block_count_offset) += 1;                                                     // blockCount++;
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_end_offset) = (uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE - sizeof(long);  // put dataCurrEnd in the right place
}

void Database::checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount) {
  if ((uintptr_t)currEnd - (uintptr_t)curr < spaceNeeded) {
    addNewBlock(curr, currEnd, currBlockCount);
  }
}

// call before incrementing
// is it safe to increment? redirects pointers to right places if it's not
bool Database::checkSpaceSearch(int fixedLength, void*& currRead, void*& currReadEnd) {
  if (fixedLength == -1) {  // variable length
    // search in the current block for a record terminating character
    // if we can't find it, then
    int spaceToCheck = (uintptr_t)currReadEnd - (uintptr_t)currRead;
    // cout << "currReadEnd: " << currReadEnd << endl;
    bool found = false;
    char* currReadPtr = (char*)currRead;
    for (int i = 0; i < spaceToCheck; i++) {
      // cout << "*currReadPtr " << *currReadPtr << endl;
      if (*currReadPtr == END_RECORD_CHAR) {
        found = true;
        // cout << "dont need new" << endl;
        break;
      }
      currReadPtr++;
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

void Database::checkSpaceAddData(int spaceNeeded, void* dBPrimaryRecord) {
  void* currData = (void*)*(long*)((uintptr_t)dBPrimaryRecord + data_curr_offset);
  void* currDataEnd = (void*)*(long*)((uintptr_t)dBPrimaryRecord + data_curr_end_offset);

  if ((long)currDataEnd - (long)currData < spaceNeeded) {
    addNewDataBlock(dBPrimaryRecord);
  }
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
  cout << "name | void* db_attr | int num_db_attr | int primary_key_num | void* data_root | void* data_curr | void* data end | int numDataBlocks| int numDataRecords" << endl;

  for (int i = 0; i < db_primary_records; i++) {
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
    ptr = (void*)((uintptr_t)ptr + db_primary_record_size);  // add the recordSizeOffset
  }
  return nullptr;  // the record was not found
}

// char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
int Database::addToDBAttr(char* attrName, char* typeStr) {
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

  cout << "void* | char(TABLE_NAME_SIZE) attrName | (int) type | (int) length" << endl;
  for (int i = 0; i < db_attr_records; i++) {
    void* before = ptr;
    checkSpaceSearch(db_attr_record_size, ptr, ptrEnd);
    if (before != ptr) {
      cout << "__________________________________________________________________" << endl;
    }
    cout << ptr << " | ";
    // attr name
    char buffer[TABLE_NAME_SIZE];
    memset(buffer, '\0', TABLE_NAME_SIZE);
    strncpy(buffer, (char*)ptr, TABLE_NAME_SIZE);
    cout << buffer << " | ";
    (char*&)ptr += TABLE_NAME_SIZE;

    // type
    cout << *(dataType*)ptr << " | ";
    ((dataType*&)ptr)++;

    // length
    cout << *(int*)ptr << endl;
    ((int*&)ptr)++;
  }
}

void* Database::findEndOfBlock(void* currRecord) {
  void* currRead = db_attr;
  void* currReadEnd = (void*)(((long*)((uintptr_t)currRead + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < db_attr_records; i++) {
    checkSpaceSearch(db_attr_records, currRead, currReadEnd);
    if (currRead == currRecord) {
      return currReadEnd;
    }
    currRead = (void*)((uintptr_t)currRead + db_attr_record_size);  // go on to the next record
  }
  return nullptr;
}

void Database::addFixedToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr) {
  if (method == UNORDERED) {
    if ((long)findPrimaryKeyFixed(dbPrimaryPtr, primaryKey) > 0) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }
    addUnorderedToTable(bufferToWrite, recordSize, dbPrimaryPtr);
  } else if (method == ORDERED) {
    // insertion point
    // TODO: change to Binary Search
    void* insertionPoint = (void*)(long)findPrimaryKeyFixed(dbPrimaryPtr, primaryKey);
    if ((long)insertionPoint > 0) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }
    addOrderedToTableFixed(bufferToWrite, primaryKey, recordSize, dbPrimaryPtr, insertionPoint);
  }

  free(primaryKey);
  free(bufferToWrite);
}

void Database::addVariableToTable(void* primaryKey, void* bufferToWrite, int recordSize, void*& dbPrimaryPtr) {
  // figure out the actual recordSize
  int actualRecordSize = 0;
  char* bufferPtr = (char*)bufferToWrite;
  for (int i = 0; i < recordSize; i++) {
    if (*bufferPtr == END_RECORD_CHAR) {
      break;
    }
    ((char*&)bufferPtr)++;  // dumb girl
    actualRecordSize++;
  }
  actualRecordSize++;

  if (method == UNORDERED) {
    if ((long)findPrimaryKeyVariable(dbPrimaryPtr, primaryKey) > 0) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }

    addUnorderedToTable(bufferToWrite, actualRecordSize, dbPrimaryPtr);
  } else if (method == ORDERED) {
    void* insertionPoint = (void*)(long)findPrimaryKeyVariable(dbPrimaryPtr, primaryKey);
    if ((long)insertionPoint > 0) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }
    addOrderedToTableVariable(bufferToWrite, primaryKey, actualRecordSize, dbPrimaryPtr, insertionPoint);
  }

  free(primaryKey);
  free(bufferToWrite);
}

void Database::addUnorderedToTable(void* bufferToWrite, int recordSize, void*& dbPrimaryPtr) {
  checkSpaceAddData(recordSize, dbPrimaryPtr);
  void* dataCurr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset);
  int dataRecordCount = *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset);

  // copy into correct place
  memcpy(dataCurr, bufferToWrite, recordSize);

  // increment dataCurr (write head)
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset) = (uintptr_t)dataCurr + recordSize;
  // increment number of dataRecords
  *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset) = dataRecordCount + 1;
}

// maybe this is what's broken
void Database::addOrderedToTableFixed(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint) {
  void* dataRoot = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);
  void* dataCurr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset);

  insertionPoint = (void*)(-1 * (long)insertionPoint);  // flip the bits back

  // empty insert or end of record insert
  if ((insertionPoint == nullptr) || (insertionPoint == dataCurr)) {
    // checkSpaceAdd(recordSize, dataCurr, dataEnd, dataCurrBlockCount);

    addUnorderedToTable(bufferToWrite, recordSize, dbPrimaryPtr);
    return;
  }

  void* penultimateRead = dataRoot;
  void* penultimateReadEnd = (void*)(((long*)((uintptr_t)dataRoot + (uintptr_t)BLOCK_SIZE)) - 1);
  void* penult = nullptr;  // find the last element

  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  bool foundInsertionPoint = false;
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(recordSize, penultimateRead, penultimateReadEnd);
    checkSpaceSearch(recordSize, dataRead, dataReadEnd);
    if (i == dataCurrRecordCount - 1) {  // last element
      penult = penultimateRead;
    }
    penultimateRead = (void*)((uintptr_t)penultimateRead + recordSize);
    if (!foundInsertionPoint) {
      if (dataRead == insertionPoint) {
        foundInsertionPoint = true;
      }
      dataRead = (void*)((uintptr_t)dataRead + recordSize);
    }
  }

  // add to the end of the table
  addUnorderedToTable(penult, recordSize, dbPrimaryPtr);

  // shift everyone down
  // would be really helpful if we could traverse backwards, but kind of hard
  void* last[recordSize];

  memcpy(last, insertionPoint, recordSize);           // store the last
  memcpy(insertionPoint, bufferToWrite, recordSize);  // insert the new record

  while (dataRead != dataCurr) {
    checkSpaceSearch(recordSize, dataRead, dataReadEnd);
    void* temp[recordSize];
    memcpy(temp, dataRead, recordSize);
    memcpy(dataRead, last, recordSize);
    memcpy(last, temp, recordSize);
    dataRead = (void*)((uintptr_t)dataRead + recordSize);  // onto the next;
  }
}

void Database::addOrderedToTableVariable(void* bufferToWrite, void* primaryKey, int recordSize, void*& dbPrimaryPtr, void* insertionPoint) {
  // we hold on to the memory here
  void* dataRoot = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);
  void* dataCurr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset);
  int dataCurrBlockCount = *(int*)((uintptr_t)dbPrimaryPtr + data_block_count_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset);

  insertionPoint = (void*)(-1 * (long)insertionPoint);  // flip the bits back

  // insertion empty or insertion at the end]
  //  "insertionPoint " << insertionPoint << endl;
  // cout << "dataCurr " << dataCurr << endl;
  if ((insertionPoint == nullptr) || (insertionPoint == dataCurr)) {
    // cout << "empty or last insertion" << endl;
    addUnorderedToTable(bufferToWrite, recordSize, dbPrimaryPtr);
    return;
  }
  // cleanest, but maybe not the optimal solution
  void* dataTempRoot = NULL;
  void* dataTempCurr = NULL;
  void* dataTempCurrEnd = NULL;
  int dataTempCurrBlockCount = 0;
  int dataTempCurrRecordCount = 0;  // I think this stuff will be taken care of

  initializeNewBlock(dataTempRoot, dataTempCurr, dataTempCurrEnd, dataTempCurrBlockCount);

  // reassignment of dbPrimaryStuff
  *(long*)((uintptr_t)dbPrimaryPtr + data_root_offset) = (uintptr_t)dataTempRoot;
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset) = (uintptr_t)dataTempCurr;         // dataCurr = toadd
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_end_offset) = (uintptr_t)dataTempCurrEnd;  // put dataCurrEnd in the right
  *(int*)((uintptr_t)dbPrimaryPtr + data_block_count_offset) = dataTempCurrBlockCount;    // blockCount++;
  *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset) = dataTempCurrRecordCount;  // blockCount++; place

  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);

  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(-1, dataRead, dataReadEnd);

    if (dataRead == insertionPoint) {
      // insert the new data record
      addUnorderedToTable(bufferToWrite, recordSize, dbPrimaryPtr);
    }

    int thisRecordCount = 0;
    void* recordBeginning = dataRead;

    // go to the end of this record

    while (*(char*)dataRead != END_RECORD_CHAR) {
      ((char*&)dataRead)++;
      thisRecordCount++;
    }
    ((char*&)dataRead)++;
    thisRecordCount++;

    char buffer[thisRecordCount];
    memcpy(buffer, recordBeginning, thisRecordCount);
    addUnorderedToTable(buffer, thisRecordCount, dbPrimaryPtr);
  }

  freeDataTable(dataRoot, dataCurrBlockCount);
}

// primary key can be modified to be a search for a certain attribute?
void* Database::findPrimaryKeyFixed(void* dbPrimaryPtr, void* pkToFind) {
  // find out the type of primary key
  void* dbAttrPtr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + db_primary_db_attr_offset);
  int numDbAttr = *(int*)((uintptr_t)dbPrimaryPtr + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)dbPrimaryPtr + primary_key_offset);

  void* readDbAttr = dbAttrPtr;
  void* readDbAttrEnd = findEndOfBlock(readDbAttr);
  // find the type of primary key and calculate the offset
  int offset = 0;
  dataType pkType = INVALID;
  int pkSize = 0;
  for (int i = 0; i < numDbAttr; i++) {
    checkSpaceSearch(db_attr_record_size, readDbAttr, readDbAttrEnd);

    dataType type = *(dataType*)((uintptr_t)readDbAttr + attr_type_offset);
    if (i == primaryKeyNumber) {
      pkType = type;
      break;
    }
    if (type == CHAR) {  // technically we shouldn't be able to get a varchar here
      int size = *(int*)((uintptr_t)readDbAttr + attr_length_offset);
      offset += size;
      pkSize = size;
    } else if (type == SMALLINT) {
      offset += sizeof(short);
    } else if (type == INTEGER || type == REAL) {
      offset += sizeof(int);
    }
    // on to the next record
    readDbAttr = (void*)((uintptr_t)readDbAttr + db_attr_record_size);
  }
  int fixedSize = calculateMaxDataRecordSize(dbAttrPtr, numDbAttr);
  void* dataRoot = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);

  int dataCurrRecordCount = *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset);

  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  void* dataReadNext = dataRoot;
  void* dataReadNextEnd = dataReadEnd;
  // checkSpaceSearch(fixedSize, dataReadNext, dataReadNextEnd);  // no harm done if there's only one record here?
  dataReadNext = (void*)((intptr_t)dataReadNext + fixedSize);
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedSize, dataRead, dataReadEnd);
    checkSpaceSearch(fixedSize, dataReadNext, dataReadNextEnd);
    void* pk = (void*)((uintptr_t)dataRead + offset);
    void* pkNext = (void*)((uintptr_t)dataReadNext + offset);

    // haven't checked if separating them all out will be ok
    if (i == 0) {
      if (pkType == CHAR) {
        if (strncmp((char*)pkToFind, (char*)pk, pkSize) == 0) {
          return dataRead;
        } else if (strncmp((char*)pkToFind, (char*)pk, pkSize) < 0) {
          return (void*)(-1 * (uintptr_t)dataRead);  // flip the bits for the insertion point
        }

      } else if (pkType == SMALLINT) {
        if (*(short*)pkToFind == *(short*)pk) {
          return dataRead;
        } else if ((*(short*)pkToFind < *(short*)pk)) {
          return (void*)(-1 * (uintptr_t)dataRead);  // flip the bits for the insertion point
        }
      } else if (pkType == INTEGER) {
        if (*(int*)pkToFind == *(int*)pk) {
          return dataRead;
        } else if ((*(int*)pkToFind < *(int*)pk)) {
          return (void*)(-1 * (uintptr_t)dataRead);  // flip the bits for the insertion point
        }
      } else if (pkType == REAL) {
        if (*(float*)pkToFind == *(float*)pk) {
          return dataRead;
        } else if ((*(float*)pkToFind < *(float*)pk)) {
          return (void*)(-1 * (uintptr_t)dataRead);  // flip the bits for the insertion point
        }
      }
    }

    if (i != dataCurrRecordCount - 1) {
      if (pkType == CHAR) {
        if (strncmp((char*)pkToFind, (char*)pk, pkSize) == 0) {
          return dataRead;
        } else if (strncmp((char*)pkToFind, (char*)pk, pkSize) > 0 && strncmp((char*)pkToFind, (char*)pkNext, pkSize) < 0) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }

      } else if (pkType == SMALLINT) {
        if (*(short*)pkToFind == *(short*)pk) {
          return dataRead;
        } else if ((*(short*)pkToFind > *(short*)pk) && (*(short*)pkToFind < *(short*)pkNext)) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      } else if (pkType == INTEGER) {
        if (*(int*)pkToFind == *(int*)pk) {
          return dataRead;
        } else if ((*(int*)pkToFind > *(int*)pk) && (*(int*)pkToFind < *(int*)pkNext)) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      } else if (pkType == REAL) {
        if (*(float*)pkToFind == *(float*)pk) {
          return dataRead;
        } else if ((*(float*)pkToFind > *(float*)pk) && (*(float*)pkToFind < *(float*)pkNext)) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      }
    }
    if (i == dataCurrRecordCount - 1) {  // check the last index
      if (pkType == CHAR) {
        if (strncmp((char*)pk, (char*)pkToFind, pkSize) == 0) {
          return dataRead;
        } else if (strncmp((char*)pk, (char*)pkToFind, pkSize) < 0) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }

      } else if (pkType == SMALLINT) {
        if (*(short*)pkToFind == *(short*)pk) {
          return dataRead;
        } else if (*(short*)pkToFind > *(short*)pk) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      } else if (pkType == INTEGER) {
        if (*(int*)pkToFind == *(int*)pk) {
          return dataRead;
        } else if (*(int*)pkToFind > *(int*)pk) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      } else if (pkType == REAL) {
        if (*(float*)pkToFind == *(float*)pk) {
          return dataRead;
        } else if (*(float*)pkToFind > *(float*)pk) {
          return (void*)(-1 * (uintptr_t)dataReadNext);  // flip the bits for the insertion point
        }
      }
    }
    // increment to the next record
    dataRead = (void*)((intptr_t)dataRead + fixedSize);
    dataReadNext = (void*)((intptr_t)dataReadNext + fixedSize);
  }
  return nullptr;  // empty insert
}

void* Database::findPrimaryKeyVariable(void* dbPrimaryPtr, void* pkToFind) {
  int pkSize = 0;
  void* dbAttrRecord = (void*)*(long*)((uintptr_t)dbPrimaryPtr + db_primary_db_attr_offset);
  int numAttr = *(int*)((uintptr_t)dbPrimaryPtr + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)dbPrimaryPtr + primary_key_offset);
  void* dataRoot = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)dbPrimaryPtr + data_record_count_offset);

  dataType attrTypes[numAttr];
  void* attrRecords[numAttr];  // pointer to each attribute
  void* readDBAttr = dbAttrRecord;
  void* readDBAttrEnd = findEndOfBlock(dbAttrRecord);

  // get db_attr information
  for (int i = 0; i < numAttr; i++) {
    checkSpaceSearch(db_attr_record_size, readDBAttr, readDBAttrEnd);

    attrRecords[i] = readDBAttr;

    dataType type = *(dataType*)((uintptr_t)attrRecords[i] + attr_type_offset);
    attrTypes[i] = type;
    if (i == primaryKeyNumber) {
      if (type == VARCHAR || type == CHAR) {
        pkSize = *(int*)((uintptr_t)attrRecords[i] + attr_length_offset);
      }
    }

    // the next is our next record
    readDBAttr = (void*)((uintptr_t)readDBAttr + db_attr_record_size);
  }

  // set up read heads
  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);

  void* dataReadNext = dataRead;
  void* dataReadNextEnd = (void*)(((long*)((uintptr_t)dataReadNext + (uintptr_t)BLOCK_SIZE)) - 1);

  // need one that's one record ahead, just go find the next end of record character
  if (dataCurrRecordCount > 0) {
    // cout << "here" << endl;                               // I think this might be the issue!
    checkSpaceSearch(-1, dataReadNext, dataReadNextEnd);  // correction for finding the next record
    int spaceToCheck = (uintptr_t)dataReadNextEnd - (uintptr_t)dataReadNext;
    //  cout << "here 2" << endl;
    for (int i = 0; i < spaceToCheck; i++) {
      if (*(char*)dataReadNext == END_RECORD_CHAR) {
        ((char*&)dataReadNext)++;
        break;
      }
      ((char*&)dataReadNext)++;
    }
  }

  // we just need the attribute types so we know what to cast it into
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(-1, dataRead, dataReadEnd);
    checkSpaceSearch(-1, dataReadNext, dataReadNextEnd);
    void* recordBegin = dataRead;
    void* nextRecordBegin = dataReadNext;
    // cout << "record Begin " << *(int*)recordBegin << endl;
    // cout << "nextRecord Begin " << *(int*)nextRecordBegin << endl;
    //  cout << "dataReadHead 2: " << dataRead << endl;
    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);

        char buff[buffSize];
        memset(buff, '\0', buffSize);

        char buffNext[buffSize];
        memset(buffNext, '\0', buffSize);

        // copy
        // go character by character

        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;
          }
          buff[k] = a;
        }

        if (dataCurrRecordCount > 1) {
          for (int k = 0; k < buffSize; k++) {
            char a = *(char*)dataReadNext;
            ((char*&)dataReadNext)++;  // move on to next byte regardless
            if (a == END_FIELD_CHAR) {
              break;
            }
            buffNext[k] = a;
          }
        }

        if (j == primaryKeyNumber) {
          if (strcmp((char*)pkToFind, (char*)buff) == 0) {
            return recordBegin;
          }

          if (i == 0) {
            if (strcmp((char*)pkToFind, (char*)buff) < 0) {
              return (void*)(-1 * (uintptr_t)recordBegin);
            }
          }
          if (i == dataCurrRecordCount - 1) {
            if (strcmp((char*)pkToFind, (char*)buffNext) > 0) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }

          if (i != dataCurrRecordCount - 1) {
            if (strcmp((char*)pkToFind, (char*)buff) > 0 && strcmp((char*)pkToFind, (char*)buffNext) < 0) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
        }
      }

      else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        char buffNext[buffSize];
        memset(buffNext, '\0', buffSize);

        // copy

        strncpy(buff, (char*)dataRead, buffSize);
        // cout << "pkToFind " << (char*)pkToFind << endl;

        // cout
        if (j == primaryKeyNumber) {
          if (strncmp((char*)pkToFind, (char*)buff, pkSize) == 0) {
            return recordBegin;
          }
          if (i == 0) {
            if (strcmp((char*)pkToFind, (char*)buff) < 0) {
              return (void*)(-1 * (uintptr_t)recordBegin);
            }
          }

          if (i == dataCurrRecordCount - 1) {
            if (strcmp((char*)pkToFind, (char*)buff) > 0) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
          if (i != dataCurrRecordCount - 1) {
            strncpy(buffNext, (char*)dataReadNext, buffSize);
            if (strcmp((char*)pkToFind, buff) > 0 && strcmp((char*)pkToFind, buffNext) < 0) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
        }
        ((char*&)dataRead) += buffSize;
        ((char*&)dataReadNext) += buffSize;
      } else if (type == SMALLINT) {
        if (j == primaryKeyNumber) {
          if (*(short*)pkToFind == *(short*)dataRead) {
            return recordBegin;
          }
          if (i == 0) {
            if (*(short*)pkToFind < *(short*)dataRead) {
              return (void*)(-1 * (uintptr_t)recordBegin);
            }
          }
          if (i == dataCurrRecordCount - 1) {
            if (*(short*)pkToFind > *(short*)dataRead) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }

          if (i != dataCurrRecordCount - 1) {
            if ((*(short*)pkToFind > *(short*)dataRead) && (*(short*)pkToFind < *(short*)dataReadNext)) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
        }
        ((short*&)dataRead)++;
        ((short*&)dataReadNext)++;
      } else if (type == INTEGER) {
        if (j == primaryKeyNumber) {
          // cout << "dataReadNext: " << *(int*)dataReadNext << endl;
          // I know what's wrong
          if (*(int*)pkToFind == *(int*)dataRead) {
            return recordBegin;
          }
          if (i == 0) {
            if (*(int*)pkToFind < *(int*)dataRead) {
              // cout << "first element" << endl;
              // cout << "dataRead " << *(int*)dataRead << endl;
              // cout << "PktoFind: " << *(int*)pkToFind << endl;
              return (void*)(-1 * (uintptr_t)recordBegin);
            }
          }
          if (i == dataCurrRecordCount - 1) {
            if (*(int*)pkToFind > *(int*)dataRead) {
              //  cout << "last element" << endl;
              //  cout << "dataRead " << *(int*)dataRead << endl;
              // cout << "PktoFind: " << *(int*)pkToFind << endl;
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }

          if (i != dataCurrRecordCount - 1) {
            if ((*(int*)pkToFind > *(int*)dataRead) && (*(int*)pkToFind < *(int*)dataReadNext)) {
              // cout << "mid element" << endl;
              // cout << "dataRead " << *(int*)dataRead << endl;
              // cout << "dataRead Next " << *(int*)dataReadNext << endl;
              // cout << "PktoFind: " << *(int*)pkToFind << endl;

              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
        }
        ((int*&)dataRead)++;
        ((int*&)dataReadNext)++;
      } else if (type == REAL) {
        if (j == primaryKeyNumber) {
          if (*(float*)pkToFind == *(float*)dataRead) {
            return recordBegin;
          }
          if (i == 0) {
            if (*(float*)pkToFind < *(float*)dataRead) {
              return (void*)(-1 * (uintptr_t)recordBegin);
            }
          }
          if (i == dataCurrRecordCount - 1) {
            if (*(float*)pkToFind > *(float*)dataRead) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }

          if (i != dataCurrRecordCount - 1) {
            if ((*(float*)pkToFind > *(float*)dataRead) && (*(float*)pkToFind < *(float*)dataReadNext)) {
              return (void*)(-1 * (uintptr_t)nextRecordBegin);
            }
          }
        }

        ((float*&)dataRead)++;
        ((float*&)dataReadNext)++;
      }
    }
    ((char*&)dataRead)++;  // end record character
    ((char*&)dataReadNext)++;
  }
  cout << "nullptr :(" << endl;
  return nullptr;
}

void Database::printTable(char* table_name) {
  printDBPrimary();
  // just do a select all from table? maybe do a select helper
  void* record = retrieveDBPrimaryRecord(table_name);

  void* dbAttrRecord = (void*)*(long*)((uintptr_t)record + db_primary_db_attr_offset);
  int numAttr = *(int*)((uintptr_t)record + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)record + primary_key_offset);
  void* dataRoot = (void*)*(long*)((uintptr_t)record + data_root_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)record + data_record_count_offset);

  // print out all of the attributes
  bool variable = false;  // useful to know I guess... how are we going to do variable records? special ending for a varchar and special ending for a record?
  dataType attrTypes[numAttr];
  void* attrRecords[numAttr];  // pointer to each attribute
  void* readDBAttr = dbAttrRecord;
  void* readDBAttrEnd = findEndOfBlock(dbAttrRecord);

  // print out the headers
  for (int i = 0; i < numAttr; i++) {
    checkSpaceSearch(db_attr_record_size, readDBAttr, readDBAttrEnd);

    attrRecords[i] = readDBAttr;

    char attrName[TABLE_NAME_SIZE];
    strncpy(attrName, (char*)attrRecords[i], TABLE_NAME_SIZE);

    dataType type = *(dataType*)((uintptr_t)attrRecords[i] + attr_type_offset);
    attrTypes[i] = type;
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

    // the next is our next record
    readDBAttr = (void*)((uintptr_t)readDBAttr + db_attr_record_size);
  }

  // set up read heads
  void* dataRead = dataRoot;
  // cout << "dataRead Head: " << dataRead << endl;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = -1;
  if (!variable) {
    fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  }
  // we just need the attribute types so we know what to cast it into
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

    // cout << "dataReadHead 2: " << dataRead << endl;
    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        // get max, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        // go character by character
        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;
          }
          buff[k] = a;
        }
        // cout
        cout << buff;

      } else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
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
    if (variable) ((char*&)dataRead)++;
  }
}

void Database::printTableGiven(char* table_name, vector<char*> fieldsToPrint, char* specialName, char* comparator, char* target) {
  void* record = retrieveDBPrimaryRecord(table_name);

  void* dbAttrRecord = (void*)*(long*)((uintptr_t)record + db_primary_db_attr_offset);
  int numAttr = *(int*)((uintptr_t)record + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)record + primary_key_offset);
  void* dataRoot = (void*)*(long*)((uintptr_t)record + data_root_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)record + data_record_count_offset);

  // print out all of the attributes
  bool variable = false;
  char* attrNames[numAttr];
  dataType attrTypes[numAttr];
  void* attrRecords[numAttr];  // pointer to each attribute
  bool interest[numAttr];

  int numToLookFor = (int)fieldsToPrint.size();
  if (strcmp(fieldsToPrint.at(0), "*") == 0) {
    numToLookFor = numAttr;
  }
  void* readDBAttr = dbAttrRecord;
  void* readDBAttrEnd = findEndOfBlock(dbAttrRecord);

  // print out the headers
  int attrCounter1 = 0;
  int special = -1;
  for (int i = 0; i < numAttr; i++) {
    checkSpaceSearch(db_attr_record_size, readDBAttr, readDBAttrEnd);

    attrRecords[i] = readDBAttr;

    char attrName[TABLE_NAME_SIZE];
    strncpy(attrName, (char*)attrRecords[i], TABLE_NAME_SIZE);
    if (specialName != nullptr && strcmp(attrName, specialName) == 0) {
      special = i;
    }

    dataType type = *(dataType*)((uintptr_t)attrRecords[i] + attr_type_offset);
    attrTypes[i] = type;
    if (type == VARCHAR) {
      variable = true;
    }

    attrNames[i] = attrName;
    bool need = false;
    if (strcmp(fieldsToPrint[0], "*") == 0) {
      cout << attrName;
      need = true;
    } else {
      // print out if it's an attribute we're interested in
      for (char* n : fieldsToPrint) {
        if (strstr(attrName, n)) {
          cout << attrName;
          need = true;
          break;
        }
      }
    }
    interest[i] = need;
    if (i == primaryKeyNumber && need) {
      cout << "*";
    }
    if (need) {
      if (attrCounter1 == numToLookFor - 1) {
        cout << endl;

      } else {
        cout << " | ";
      }
      attrCounter1++;
    }

    // the next is our next record
    readDBAttr = (void*)((uintptr_t)readDBAttr + db_attr_record_size);
  }

  // set up read heads
  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  if (variable) {
    fixedLength = -1;
  }
  // we just need the attribute types so we know what to cast it into
  bool isTarget[dataCurrRecordCount];

  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

    if (specialName == nullptr) isTarget[i] = true;  // if we don't have a special, by default we're going to want to print everything out

    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        // get max, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        // go character by character

        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;
          }
          buff[k] = a;
        }
        if (j == special) isTarget[i] = compare(target, comparator, buff, strlen(buff), VARCHAR);

        // this strlen is a bit risky but should be ok
        //

      } else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);
        // copy
        strncpy(buff, (char*)dataRead, buffSize);

        if (j == special) isTarget[i] = compare(target, comparator, buff, strlen(buff), CHAR);
        ((char*&)dataRead) += buffSize;

      } else if (type == SMALLINT) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, SMALLINT);
        ((short*&)dataRead)++;
      } else if (type == INTEGER) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, INTEGER);
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, REAL);
        ((float*&)dataRead)++;
      }
    }
    if (variable) {
      ((char*&)dataRead)++;  // END_RECORD_CHAR
    }
    //  cout << "target" << isTarget[i] << " ";
    //  cout << endl;
  }

  // TODO: good place to separate the function for reuse and for testing?
  // reset the dataReads
  dataRead = dataRoot;
  dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

    // we know which attribute is special... how can we figure out if we need to print it before we print out the record partially?
    // we could iterate through twice, which isn't ideal but isn't horrible - we could keep a count of all the records to print out
    // this in turn could be useful for the update function, which will mostly be written

    int attrCounter2 = 0;
    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        // get max, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        // go character by character
        // int counter = 0;
        // cout << "i: " << i << " j: " << j << " ";
        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;  // don't copy the end field char
          }
          buff[k] = a;
          /*
          if (a != '\0') {  // don't copy blanks... so weird that they would be here
            buff[counter] = a;
            counter++;
          }
          */
        }
        // cout << " buff: " << buff;  // why is there random stuff in the buffer sometimes? this causes us to miss the last
        // cout << endl;
        //  cout
        if (interest[j] && isTarget[i]) {
          cout << buff;  // why would the buffer sometimes be empty?
        }

      } else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        strncpy(buff, (char*)dataRead, buffSize);
        // cout
        if (interest[j] && isTarget[i]) {
          cout << buff;
        }
        ((char*&)dataRead) += buffSize;

      } else if (type == SMALLINT) {
        if (interest[j] && isTarget[i]) cout << *(short*)dataRead;
        ((short*&)dataRead)++;
      } else if (type == INTEGER) {
        if (interest[j] && isTarget[i]) cout << *(int*)dataRead;
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        if (interest[j] && isTarget[i]) cout << *(float*)dataRead;
        ((float*&)dataRead)++;
      }
      if (interest[j] && isTarget[i]) {
        if (attrCounter2 == numToLookFor - 1) {
          cout << endl;
        } else {
          cout << " | ";
        }
        attrCounter2++;
      }
    }
    if (variable) ((char*&)dataRead)++;
  }
}

int Database::updateTableGiven(char* table_name, char* attrToUpdate, char* value, char* specialName, char* comparator, char* target) {
  void* record = retrieveDBPrimaryRecord(table_name);
  void* dbAttrRecord = (void*)*(long*)((uintptr_t)record + db_primary_db_attr_offset);
  int numAttr = *(int*)((uintptr_t)record + db_primary_num_db_attr_offset);
  void* dataRoot = (void*)*(long*)((uintptr_t)record + data_root_offset);
  int dataCurrRecordCount = *(int*)((uintptr_t)record + data_record_count_offset);

  // print out all of the attributes
  bool variable = false;
  char* attrNames[numAttr];
  dataType attrTypes[numAttr];
  void* attrRecords[numAttr];  // pointer to each attribute

  void* readDBAttr = dbAttrRecord;
  void* readDBAttrEnd = findEndOfBlock(dbAttrRecord);

  int special = -1;
  int toUpdate = -1;

  int numUpdate = 0;
  // find the number of the attribute that we want to update
  for (int i = 0; i < numAttr; i++) {
    checkSpaceSearch(db_attr_record_size, readDBAttr, readDBAttrEnd);

    attrRecords[i] = readDBAttr;

    char attrName[TABLE_NAME_SIZE];
    strncpy(attrName, (char*)attrRecords[i], TABLE_NAME_SIZE);

    if (strcmp(attrName, attrToUpdate) == 0) toUpdate = i;
    if (specialName != nullptr && strcmp(attrName, specialName) == 0) special = i;

    dataType type = *(dataType*)((uintptr_t)attrRecords[i] + attr_type_offset);
    attrTypes[i] = type;
    if (type == VARCHAR) variable = true;

    attrNames[i] = attrName;

    // the next is our next record
    readDBAttr = (void*)((uintptr_t)readDBAttr + db_attr_record_size);
  }

  // see if the record fits the condition
  // set up read heads
  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = -1;
  if (!variable) {
    fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  }
  // we just need the attribute types so we know what to cast it into
  bool isTarget[dataCurrRecordCount];

  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);
    if (specialName == nullptr) isTarget[i] = true;  // if we don't have a special, by default we're going to want to print everything out

    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
        // get max, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        // copy
        // go character by character

        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;
          }
          buff[k] = a;
        }
        if (j == special) isTarget[i] = compare(target, comparator, buff, strlen(buff), VARCHAR);

      } else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);
        // copy
        strncpy(buff, (char*)dataRead, buffSize);

        if (j == special) isTarget[i] = compare(target, comparator, buff, strlen(buff), CHAR);
        ((char*&)dataRead) += buffSize;

      } else if (type == SMALLINT) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, SMALLINT);
        ((short*&)dataRead)++;
      } else if (type == INTEGER) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, INTEGER);
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, REAL);
        ((float*&)dataRead)++;
      }
    }
    if (isTarget[i]) numUpdate++;
    if (variable) ((char*&)dataRead)++;

    //  cout << "target" << isTarget[i] << " ";
    //  cout << endl;
  }

  // go update the isTarget records
  // reset the dataReads
  dataRead = dataRoot;
  dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);

  if (variable) {  // update table variable 🥲
    // cout << "variable" << endl;
    // we have to combine the printing and the inserting
    // cleanest, but maybe not the optimal solution
    void* dataTempRoot = NULL;
    void* dataTempCurr = NULL;
    void* dataTempCurrEnd = NULL;
    int dataTempCurrBlockCount = 0;
    int dataTempCurrRecordCount = 0;  // I think this stuff will be taken care of

    initializeNewBlock(dataTempRoot, dataTempCurr, dataTempCurrEnd, dataTempCurrBlockCount);

    // reassignment of dbPrimaryStuff
    *(long*)((uintptr_t)record + data_root_offset) = (uintptr_t)dataTempRoot;
    *(long*)((uintptr_t)record + data_curr_offset) = (uintptr_t)dataTempCurr;         // dataCurr = toadd
    *(long*)((uintptr_t)record + data_curr_end_offset) = (uintptr_t)dataTempCurrEnd;  // put dataCurrEnd in the right
    *(int*)((uintptr_t)record + data_block_count_offset) = dataTempCurrBlockCount;    // blockCount++;
    *(int*)((uintptr_t)record + data_record_count_offset) = dataTempCurrRecordCount;  // blockCount++; place

    int maxRecordSize = calculateMaxDataRecordSize(attrRecords[0], numAttr);
    // cout << "got here" << endl;
    for (int i = 0; i < dataCurrRecordCount; i++) {
      // cout << "ping " << i << endl;
      checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

      // we need to make a buffer here and then add that to the new copy
      void* bufferToSend = calloc(maxRecordSize, 1);
      void* bufferPtr = bufferToSend;
      for (int j = 0; j < numAttr; j++) {
        dataType type = attrTypes[j];

        if (type == VARCHAR) {
          int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
          char buff[buffSize];
          memset(buff, '\0', buffSize);

          int actual = 0;
          for (int k = 0; k < buffSize; k++) {
            char a = *(char*)dataRead;
            ((char*&)dataRead)++;  // move on to next byte regardless
            actual++;
            buff[k] = a;
            if (a == END_FIELD_CHAR) break;  // DO copy the end of field char
          }

          if (isTarget[i] && j == toUpdate) {
            strncpy((char*)bufferPtr, value, strlen(value));
            ((char*&)bufferPtr += strlen(value));
            *(char*)bufferPtr = END_FIELD_CHAR;
            ((char*&)bufferPtr)++;  // END FIELD CHAR
          } else {
            // just send the regular one
            strncpy((char*)bufferPtr, buff, actual);
            ((char*&)bufferPtr += actual);
          }

        } else if (type == CHAR) {
          // cout
          int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
          if (isTarget[i] && j == toUpdate) {
            char buff[buffSize];
            memset(buff, '\0', buffSize);
            // copy
            strncpy(buff, value, buffSize);
            strncpy((char*)dataRead, buff, buffSize);
          }
          // copy over to new buffer
          strncpy((char*)bufferPtr, (char*)dataRead, buffSize);
          ((char*&)bufferPtr) += buffSize;
          ((char*&)dataRead) += buffSize;
        } else if (type == SMALLINT) {
          if (isTarget[i] && j == toUpdate) *(short*)dataRead = (short)atoi(value);
          // copy over to new buffer
          *(short*)bufferPtr = *(short*)dataRead;
          ((short*&)bufferPtr)++;
          ((short*&)dataRead)++;
        } else if (type == INTEGER) {
          if (isTarget[i] && j == toUpdate) *(int*)dataRead = atoi(value);
          *(int*)bufferPtr = *(int*)dataRead;
          ((int*&)bufferPtr)++;
          ((int*&)dataRead)++;
        } else if (type == REAL) {
          if (isTarget[i] && j == toUpdate) *(float*)dataRead = (float)atof(value);
          *(float*)bufferPtr = *(float*)dataRead;
          ((float*&)bufferPtr)++;
          ((float*&)dataRead)++;
        }
      }
      *(char*)bufferPtr = END_RECORD_CHAR;
      ((char*&)bufferPtr)++;  // END_RECORD_CHAR
      ((char*&)dataRead)++;   // END_RECORD_CHAR
                              // cout << "before add" << endl;
      int actualRecordSize = (uintptr_t)bufferPtr - (uintptr_t)bufferToSend;
      addUnorderedToTable(bufferToSend, actualRecordSize, record);
      // cout << "after add" << endl;
      // printTable(table_name);
    }

    return numUpdate;
  }

  // update table fixed
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == CHAR) {
        // cout
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        if (isTarget[i] && j == toUpdate) {
          // get char length, set up a buffer
          char buff[buffSize];
          memset(buff, '\0', buffSize);

          // copy
          strncpy(buff, value, buffSize);
          strncpy((char*)dataRead, buff, buffSize);
        }
        ((char*&)dataRead) += buffSize;

      } else if (type == SMALLINT) {
        if (isTarget[i] && j == toUpdate) *(short*)dataRead = (short)atoi(value);
        ((short*&)dataRead)++;
      } else if (type == INTEGER) {
        if (isTarget[i] && j == toUpdate) *(int*)dataRead = atoi(value);
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        if (isTarget[i] && j == toUpdate) *(float*)dataRead = (float)atof(value);
        ((float*&)dataRead)++;
      }
    }
  }

  return numUpdate;
}

int Database::updateTest(const char* table_name, int length, ...) {
  // make an array of the attribute names that we're looking for
  va_list arg_list;
  va_start(arg_list, length);

  cout << "UPDATE " << table_name;

  // get the attribute to update

  // get the attribute to set
  char* attrToUpdate = strdup(va_arg(arg_list, char*));
  // TODO:  we need to look up the type of value first, for now passing it as a char* is going to be fine
  char* value = strdup(va_arg(arg_list, char*));
  cout << " SET " << attrToUpdate << " = " << value;

  // For update we MUST have a WHERE clause
  // parse the WHERE
  char* sentence = va_arg(arg_list, char*);
  char* cpy = strdup(sentence);
  char* attrName = nullptr;
  char* comparator = nullptr;
  char* target = nullptr;

  int counter = 0;

  char* token = strtok(cpy, " ");
  while (token) {
    if (counter == 0) {
      attrName = token;
    } else if (counter == 1) {
      comparator = token;
    } else if (counter == 2) {
      target = token;
    }

    token = strtok(NULL, " ");

    counter++;
  }

  /*
  cout << "attrName " << attrName << endl;
  cout << "comparator " << comparator << endl;
  cout << "target " << target << endl;
  */
  int numUpdated = updateTableGiven((char*)table_name, attrToUpdate, value, attrName, comparator, target);

  cout << " WHERE " << sentence;

  cout << endl;
  cout << "You have made changes to the database. Rows affected: " << numUpdated << endl;
  va_end(arg_list);
  return numUpdated;
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
  void* ptrEnd = findEndOfBlock(ptr);
  int toReturn = 0;
  bool variable = false;
  for (int i = 0; i < numberOfAttributes; i++) {
    checkSpaceSearch(db_attr_record_size, ptr, ptrEnd);
    ptr = (void*)((uintptr_t)ptr + TABLE_NAME_SIZE);
    dataType dt = *(dataType*)ptr;
    if (dt == VARCHAR || dt == CHAR) {
      if (dt == VARCHAR) {
        variable = true;
      }
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
  // if variable, we have to add 1 for the END_RECORD_CHAR
  if (variable) toReturn++;

  return toReturn;
}

// we would need to parse the sentence before and just hand off the operator
bool Database::compare(char* targetValue, char* comparator, void* candidate, int candidateLength, dataType type) {
  // need to parse the sentence
  // we can strstr to find
  // would be good to parse the before, the after and know the type (only varchar and char will be different)

  if (type == VARCHAR || type == CHAR) {
    char str[candidateLength];
    memset(str, '\0', candidateLength);  // for good measure
    strncpy(str, (char*)candidate, candidateLength);

    int res = strcmp(str, targetValue);  // order matters
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

    if (strstr(comparator, "!=")) {
      return num != tar;
    }
    if (strstr(comparator, "<=")) {
      return num <= tar;
    }
    if (strstr(comparator, ">=")) {
      return num >= tar;
    }
    if (strstr(comparator, "=")) {
      return num == tar;
    }
    if (strstr(comparator, "<")) {
      return num < tar;
    }
    if (strstr(comparator, ">")) {
      return num > tar;
    }
  } else if (type == INTEGER) {
    int tar = atoi(targetValue);
    int num = *(int*)candidate;
    // cout << "target " << tar << " num " << num << endl;
    if (strstr(comparator, "!=")) {
      return num != tar;
    }
    if (strstr(comparator, "<=")) {
      return num <= tar;
    }
    if (strstr(comparator, ">=")) {
      return num >= tar;
    }
    if (strstr(comparator, "=")) {
      return num == tar;
    }
    if (strstr(comparator, "<")) {
      return num < tar;
    }
    if (strstr(comparator, ">")) {
      return num > tar;
    }
  } else if (type == REAL) {
    float tar = (float)atof(targetValue);
    float num = *(float*)candidate;
    if (strstr(comparator, "!=")) {
      return num != tar;
    }
    if (strstr(comparator, "<=")) {
      return num <= tar;
    }
    if (strstr(comparator, ">=")) {
      return num >= tar;
    }
    if (strstr(comparator, "=")) {
      return num == tar;
    }
    if (strstr(comparator, "<")) {
      return num < tar;
    }
    if (strstr(comparator, ">")) {
      return num > tar;
    }
  }
  return false;
}

void Database::freeDataTable(void* dataRoot, int dataCurrBlockCount) {
  void* currBlock = dataRoot;
  // cout << dataCurrBlockCount << endl;
  for (int i = 0; i < dataCurrBlockCount; i++) {
    // find the next block
    void* nextBlock = nullptr;
    if (i != dataCurrBlockCount - 1) {
      void* temp = (void*)(((long*)((uintptr_t)currBlock + (uintptr_t)BLOCK_SIZE)) - 1);
      nextBlock = (void*)*(long*)temp;
    }
    free(currBlock);
    currBlock = nextBlock;
  }
}