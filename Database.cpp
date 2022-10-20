#include "Database.h"
/// @brief standard constructor, initialize the database
/// method = UNORDERED
/// BLOCK_SIZE = 512
Database::Database() {
  this->method = UNORDERED;
  this->BLOCK_SIZE = 512;
  initializeDB();
}

/// @brief constructor, can specify insertion method, initialize the database
/// @param method insertionMethod
/// BLOCK_SIZE = 512
Database::Database(insertionMethod method) {
  this->method = method;
  this->BLOCK_SIZE = 512;
  initializeDB();
}

/// @brief constructor, cam specify insertion method and block size, initialize the database
/// @param method
/// @param blockSize
Database::Database(insertionMethod method, int blockSize) {
  this->method = method;
  BLOCK_SIZE = blockSize;
  initializeDB();
}

/// @brief create_table sql statement
/// @param table_name
/// @param key the name of the primary key attribute
/// @param length the number of variadic arguments
/// @param variadic arguments - attribute name followed by type
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

  if (method == HASHED) {
    addToDBPrimaryHashed((char*)table_name, dbAttrStart, length / 2, primKey);
  } else {
    addToDBPrimary((char*)table_name, dbAttrStart, length / 2, primKey);
  }

  va_end(arg_list);
}

/// @brief insert sql statement. supports unordered, ordered, and hashed inserts
/// @param table_name table to insert into
/// @param length number of attributes associated with the table
/// @param  values of attributes to add in
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
    *(char*)tempBufferPtr = END_RECORD_CHAR;
    ((char*&)tempBufferPtr)++;
    addVariableToTable(pk, tempBuffer, maxDataRecordSize, record);
  }

  // printTable((char*)table_name);
  va_end(arg_list);
}

/// @brief update sql statement, prints out the number of records affected by the change
/// @param table_name table to update
/// @param length should be 3, a limitation of the program :(
/// @param  first should be name of attribute to update, second is value to update to, last is a comparison
void Database::update(const char* table_name, int length, ...) {
  // make an array of the attribute names that we're looking for
  va_list arg_list;
  va_start(arg_list, length);

  cout << "UPDATE " << table_name;

  // get the attribute to update

  // get the attribute to set
  char* attrToUpdate = strdup(va_arg(arg_list, char*));
  // TODO:  we need to look up the type of value first, for now passing it as a char* is going to be fine
  // change this to void

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

  int numUpdated = 0;
  if (method == HASHED) {
    numUpdated = updateTableGivenHashed((char*)table_name, attrToUpdate, value, attrName, comparator, target);
  } else {
    numUpdated = updateTableGiven((char*)table_name, attrToUpdate, value, attrName, comparator, target);
  }

  cout << " WHERE " << sentence;

  cout << endl;
  cout << "You have made changes to the database. Rows affected: " << numUpdated << endl;
  va_end(arg_list);
}

/// @brief select sql statement, prints out the table with all the relevant information
/// @param table_name table to select from
/// @param length
/// @param  attributes to print out given a condition
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
  if (method == HASHED) {
    printTableGivenHashed((char*)table_name, fields, attrName, comparator, target);
  } else {
    printTableGiven((char*)table_name, fields, attrName, comparator, target);
  }

  for (int i = 0; i < (int)fields.size(); i++) {
    free(fields[i]);
  }
  va_end(arg_list);
}

/// @brief initialize db_primary and db_attr blocks
void Database::initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end, db_attr_blocks);
  if (method == HASHED) {
    int availableSpace = BLOCK_SIZE - (sizeof(long));  // last space is saved for the pointer to next
    num_hash_buckets = availableSpace / hashed_size;
  }
}

/// @brief malloc space for a new block
/// @param root the root block
/// @param curr current write head
/// @param currEnd end of current write head
/// @param currBlockCount number of blocks associated with root
void Database::initializeNewBlock(void*& root, void*& curr, void*& currEnd, int& currBlockCount) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  currBlockCount = 1;
}

/// @brief addNewBlock add a new dbPrimary block or dbAttr block
/// @param curr the current write head
/// @param currEnd end of current block for curr write head
/// @param currBlockCount the number of blocks associated with this table
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

/// @brief addNew dataBlock to dbPrimary
/// @param dbPrimaryPtr the record for the table to add a new block to
void Database::addNewDataBlock(void*& dbPrimaryPtr) {
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);
  **(long**)((uintptr_t)dbPrimaryPtr + data_curr_end_offset) = (uintptr_t)toAdd;                                       // *dataCurrEnd = toAdd
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset) = (uintptr_t)toAdd;                                             // dataCurr = toadd
  *(int*)((uintptr_t)dbPrimaryPtr + data_block_count_offset) += 1;                                                     // blockCount++;
  *(long*)((uintptr_t)dbPrimaryPtr + data_curr_end_offset) = (uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE - sizeof(long);  // put dataCurrEnd in the right place
}

/// @brief add a new dataBlock to a hashed bucket
/// @param bucketPtr the bucket to add the new block to
void Database::addNewDataBlockHashed(void*& bucketPtr) {
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);
  **(long**)((uintptr_t)bucketPtr + hash_curr_end_offset) = (uintptr_t)toAdd;                                       // *dataCurrEnd = toAdd
  *(long*)((uintptr_t)bucketPtr + hash_curr_offset) = (uintptr_t)toAdd;                                             // dataCurr = toadd
  *(int*)((uintptr_t)bucketPtr + hash_block_count_offset) += 1;                                                     // blockCount++;
  *(long*)((uintptr_t)bucketPtr + hash_curr_end_offset) = (uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE - sizeof(long);  // put dataCurrEnd in the right place
}

/// @brief check to see if there is space on the current block to add another record
/// allocate new space if needed
/// @param spaceNeeded record size
/// @param curr current block write head
/// @param currEnd end of current block
/// @param currBlockCount the number of current block
void Database::checkSpaceAdd(int spaceNeeded, void*& curr, void*& currEnd, int& currBlockCount) {
  if ((uintptr_t)currEnd - (uintptr_t)curr < spaceNeeded) {
    addNewBlock(curr, currEnd, currBlockCount);
  }
}

/// @brief see if there is space on the current block to add data to
/// @param spaceNeeded record size
/// @param dBPrimaryRecord the associated record for the table
void Database::checkSpaceAddData(int spaceNeeded, void* dBPrimaryRecord) {
  void* currData = (void*)*(long*)((uintptr_t)dBPrimaryRecord + data_curr_offset);
  void* currDataEnd = (void*)*(long*)((uintptr_t)dBPrimaryRecord + data_curr_end_offset);

  if ((long)currDataEnd - (long)currData < spaceNeeded) {
    addNewDataBlock(dBPrimaryRecord);
  }
}

/// @brief see if there is space in the current bucket
/// @param spaceNeeded record size
/// @param bucketPtr the associated bucket to add to
void Database::checkSpaceAddDataHashed(int spaceNeeded, void* bucketPtr) {
  // cout << "bucketPtr " << bucketPtr << endl;
  // cout << "currData Root" << (void*)*(long*)bucketPtr << endl;
  void* currData = (void*)*(long*)((uintptr_t)bucketPtr + hash_curr_offset);
  void* currDataEnd = (void*)*(long*)((uintptr_t)bucketPtr + hash_curr_end_offset);

  // cout << "checkSpaceAddDataHashed(): currData" << currData << endl;
  // cout << "checkSpaceAddDataHashed(): currDataEnd" << currDataEnd << endl;
  if ((long)currDataEnd - (long)currData < spaceNeeded) {
    addNewDataBlockHashed(bucketPtr);
  }
}

/// @brief check to see if we can continue searching on the current block, move pointers to next block if necessary
/// @param fixedLength space we want to search
/// @param currRead the current read head
/// @param currReadEnd end of the block of the current read head
/// @return
// call before incrementing
// caller asks: is it safe to increment? redirects pointers to right places if it's not
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

/// @brief add a new table to db primary
/// @param name the name of the table to add
/// @param dbAttributes pointer to the first attribute in dbAttributes
/// @param numAttributes number of attributes associated with the table
/// @param primaryKeyNum which attribute is the primaryKeyNum
/// @return
void* Database::addToDBPrimary(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum) {
  // make the size of name exact
  char buffer[TABLE_NAME_SIZE];
  memset(buffer, '\0', TABLE_NAME_SIZE);  // fill it with blanks
  if (strlen(name) > TABLE_NAME_SIZE) {
    strncpy(buffer, name, TABLE_NAME_SIZE);
  } else {
    strncpy(buffer, name, strlen(name));
  }

  checkSpaceAdd(db_primary_record_size, db_primary_curr, db_primary_curr_end, db_primary_blocks);
  void* beginningOfRecord = db_primary_curr;
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
  return beginningOfRecord;
}

/// @brief add a new table to db primary hash table
/// @param name the name of the table to add
/// @param dbAttributes pointer to the first attribute in dbAttributes
/// @param numAttributes number of attributes associated with the table
/// @param primaryKeyNum which attribute is the primaryKeyNum
void Database::addToDBPrimaryHashed(char* name, void* dbAttributes, int numAttributes, int primaryKeyNum) {
  void* dbPrimaryPtr = addToDBPrimary(name, dbAttributes, numAttributes, primaryKeyNum);
  void* bucketBlock = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);
  // now do the hashing part
  // cout << "num_hash_bucket: " << num_hash_buckets << endl;

  // get each bucket ready to accept records
  for (int i = 0; i < num_hash_buckets; i++) {
    // we don't have to check the add because we know that it's going to all fit in one block
    // make a new block
    void* dataRoot = NULL;
    void* dataCurr = NULL;
    void* dataCurrEnd = NULL;
    int dataCurrBlockCount = 0;  // printing this stuff out
    initializeNewBlock(dataRoot, dataCurr, dataCurrEnd, dataCurrBlockCount);
    // cout << "i: " << i << endl;
    // cout << "bucketBlock " << bucketBlock << endl;
    // cout << "dataRoot " << dataRoot << endl;  // why didn't this work?
    // cout << "dataCurr " << dataCurr << endl;

    *(long*)bucketBlock = (uintptr_t)dataRoot;
    *(long*)((uintptr_t)bucketBlock + hash_curr_offset) = (uintptr_t)dataCurr;
    *(long*)((uintptr_t)bucketBlock + hash_curr_end_offset) = (uintptr_t)dataCurrEnd;
    *(int*)((uintptr_t)bucketBlock + hash_block_count_offset) = dataCurrBlockCount;

    // cout << (void*)*(long*)((uintptr_t)bucketBlock + hash_curr_offset) << endl;
    // cout << (void*)*(long*)((uintptr_t)bucketBlock + hash_curr_end_offset) << endl;
    bucketBlock = (void*)((uintptr_t)bucketBlock + hashed_size);  // increment the pointer
  }

  // void* dataCurr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_curr_offset);

  // did we not finish this function?
}

/// @brief print out the contents of db primary, system dump
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

/// @brief retrieve the right record from db primary given a table name, linear search
/// @param table_name
/// @return
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

// format of db attr record: char(TABLE_NAME_SIZE) attrName | (int) type | (int) length, the length if it's variable, -1 if not
/// @brief add a new attribute to db attribute
/// @param attrName the name of the attribute
/// @param typeStr string form of the type
/// @return
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

/// @brief print out the contents of db attr, only valid, assigned records
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
/// @brief  find the end of the block given a db attr pointer
/// @param currRecord the current pointer to find the end of the block for
/// @return a pointer to the end of the block
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

/// @brief add a fixed record to the table, depending on the insertion method
/// @param primaryKey the primary key to add
/// @param bufferToWrite
/// @param recordSize number of bytes of the record
/// @param dbPrimaryPtr table to add this record to
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
  } else if (method == HASHED) {
    // cout << "insert(): insert hashed" << endl;
    if (findPrimaryKeyHashedFixed(dbPrimaryPtr, primaryKey)) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }
    addHashedToTable(bufferToWrite, recordSize, primaryKey, dbPrimaryPtr);
  }

  free(primaryKey);
  free(bufferToWrite);
}

/// @brief add a variable record to the table, depending on the insertion method
/// @param primaryKey the primary key to add
/// @param bufferToWrite
/// @param recordSize number of bytes of the record
/// @param dbPrimaryPtr table to add this record to
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
  } else if (method == HASHED) {
    if (findPrimaryKeyHashedVariable(dbPrimaryPtr, primaryKey)) {
      free(primaryKey);
      cerr << "insertion error: duplicate primary key" << endl;
      return;
    }
    addHashedToTable(bufferToWrite, actualRecordSize, primaryKey, dbPrimaryPtr);
  }

  free(primaryKey);
  free(bufferToWrite);
}

/// @brief unordered insert
/// @param bufferToWrite the copy of the record to add
/// @param recordSize the size of the record to add in bytes
/// @param dbPrimaryPtr pointer to the record to add to
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

/// @brief unordered insert for hash tables, basically just copies the data over
/// @param bufferToWrite copy of the record to add
/// @param recordSize the size of the record to add in bytes
/// @param bucketPtr the bucket to add the table
void Database::addUnorderedToTableHashed(void* bufferToWrite, int recordSize, void*& bucketPtr) {
  // cout << "addUnorderedToTableHashed(): got here" << endl;
  // cout << "addUnorderedToTableHashed() : bucketPtr " << bucketPtr << endl;
  // cout << *(long*)((uintptr_t)bucketPtr) << endl;
  // cout << *(long*)((uintptr_t)bucketPtr + hash_curr_offset) << endl;
  // cout << *(long*)((uintptr_t)bucketPtr + hash_curr_end_offset) << endl;
  checkSpaceAddDataHashed(recordSize, bucketPtr);
  // cout << "addUnorderedToTableHashed(): got here 2" << endl;
  void* dataCurr = (void*)*(long*)((uintptr_t)bucketPtr + hash_curr_offset);
  int dataRecordCount = *(int*)((uintptr_t)bucketPtr + hash_record_count_offset);

  // copy into correct place
  memcpy(dataCurr, bufferToWrite, recordSize);

  // increment dataCurr (write head)
  *(long*)((uintptr_t)bucketPtr + hash_curr_offset) = (uintptr_t)dataCurr + recordSize;
  // increment number of dataRecords
  *(int*)((uintptr_t)bucketPtr + hash_record_count_offset) = dataRecordCount + 1;
}

/// @brief ordered insert for fixed records, basically copies all values in the table over to a new table, inserted record
/// @param bufferToWrite copy of the record to add
/// @param primaryKey
/// @param recordSize size of the record to add in bytes
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param insertionPoint where to add the record to
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

/// @brief ordered insert for variable records, basically copies all values in the table over to a new table, inserted record
/// @param bufferToWrite copy of the record to add
/// @param primaryKey
/// @param recordSize size of the record to add in bytes
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param insertionPoint where to add the record to
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

/// @brief hashed insert for records. find the proper bucket to add into and add to the unordered list
/// @param bufferToWrite copy of the record to add
/// @param recordSize size of the record to add in bytes
/// @param primaryKey
/// @param dbPrimaryPtr the pointer to the record associated with the table
void Database::addHashedToTable(void* bufferToWrite, int recordSize, void* primaryKey, void*& dbPrimaryPtr) {
  void* dbAttrPtr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + db_primary_db_attr_offset);
  int numDbAttr = *(int*)((uintptr_t)dbPrimaryPtr + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)dbPrimaryPtr + primary_key_offset);
  dataType type = getPrimaryKeyType(dbAttrPtr, numDbAttr, primaryKeyNumber);
  int pkLength = getPrimaryKeyLength(dbAttrPtr, numDbAttr, primaryKeyNumber);
  int bucket = getBucketNumber(primaryKey, pkLength, type);
  // cout << "bucket #" << bucket << endl;

  void* dataRoot = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);  // go to root of hash table
  // cout << "dataRoot " << dataRoot << endl;
  /// is this guy right? I think so
  void* bucketPtr = (void*)((uintptr_t)dataRoot + (bucket * hashed_size));  // increment to the right bucket
  // now we could just treat that as a regular unordered insert of sorts
  // cout << "bucketPtr " << bucketPtr << endl;
  // cout << "addHashedToTable: got here" << endl;

  // cout << *(long*)((uintptr_t)bucketPtr) << endl;
  // cout << *(long*)((uintptr_t)bucketPtr + hash_curr_offset) << endl;
  // cout << *(long*)((uintptr_t)bucketPtr + hash_curr_end_offset) << endl;
  addUnorderedToTableHashed(bufferToWrite, recordSize, bucketPtr);
}
/// @brief find the pointer to the record with the given primary key for fixed records
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param pkToFind the primary key to find
/// @return the pointer to the record w/given primary key or an insertion point to where it should be (negative)
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

/// @brief find the record with the given primary key for fixed records when hashing is used
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param pkToFind the primary key to find
/// @return true if primary key was found, false if not
bool Database::findPrimaryKeyHashedFixed(void* dbPrimaryPtr, void* pkToFind) {
  void* dbAttrPtr = (void*)*(long*)((uintptr_t)dbPrimaryPtr + db_primary_db_attr_offset);
  int numDbAttr = *(int*)((uintptr_t)dbPrimaryPtr + db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)dbPrimaryPtr + primary_key_offset);
  void* readDbAttr = dbAttrPtr;
  void* readDbAttrEnd = findEndOfBlock(readDbAttr);

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
  // now go to that bucket and search for it
  int bucket = getBucketNumber(pkToFind, pkSize, pkType);  // where it would be
  void* bucketBlock = (void*)*(long*)((uintptr_t)dbPrimaryPtr + data_root_offset);
  bucketBlock = (void*)((uintptr_t)bucketBlock + (bucket * hashed_size));  // get to the right bucket
  // cout << "the right bucket: " << bucketBlock << endl;

  int fixedSize = calculateMaxDataRecordSize(dbAttrPtr, numDbAttr);
  void* dataRoot = (void*)*(long*)((uintptr_t)bucketBlock);

  int dataCurrRecordCount = *(int*)((uintptr_t)bucketBlock + hash_record_count_offset);
  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);

  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedSize, dataRead, dataReadEnd);
    void* pk = (void*)((uintptr_t)dataRead + offset);

    // haven't checked if separating them all out will be ok

    if (pkType == CHAR) {
      if (strncmp((char*)pkToFind, (char*)pk, pkSize) == 0) {
        return true;
      }

    } else if (pkType == SMALLINT) {
      if (*(short*)pkToFind == *(short*)pk) {
        return true;
      }
    } else if (pkType == INTEGER) {
      if (*(int*)pkToFind == *(int*)pk) {
        return true;
      }
    } else if (pkType == REAL) {
      if (*(float*)pkToFind == *(float*)pk) {
        return true;
      }
    }

    // increment to the next record
    dataRead = (void*)((intptr_t)dataRead + fixedSize);
  }
  return false;  // we didn't find a record with matching primary key
}

/// @brief find the pointer to the record with the given primary key for variable records
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param pkToFind the primary key to find
/// @return the pointer to the record w/given primary key or an insertion point to where it should be (negative)
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
  // cout << "nullptr :(" << endl;
  return nullptr;
}

/// @brief find the record with the given primary key for variable records when hashing is used
/// @param dbPrimaryPtr the pointer to the record associated with the table
/// @param pkToFind the primary key to find
/// @return true if primary key was found, false if not
bool Database::findPrimaryKeyHashedVariable(void* dbPrimaryPtr, void* pkToFind) {
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

  // we just need the attribute types so we know what to cast it into
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(-1, dataRead, dataReadEnd);
    void* recordBegin = dataRead;

    for (int j = 0; j < numAttr; j++) {
      dataType type = attrTypes[j];
      if (type == VARCHAR) {
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

        if (j == primaryKeyNumber) {
          if (strcmp((char*)pkToFind, (char*)buff) == 0) return true;
        }
      }

      else if (type == CHAR) {
        // get char length, set up a buffer
        int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
        char buff[buffSize];
        memset(buff, '\0', buffSize);

        strncpy(buff, (char*)dataRead, buffSize);

        if (j == primaryKeyNumber) {
          if (strncmp((char*)pkToFind, (char*)buff, pkSize) == 0) return true;
        }
        ((char*&)dataRead) += buffSize;
      } else if (type == SMALLINT) {
        if (j == primaryKeyNumber) {
          if (*(short*)pkToFind == *(short*)dataRead) return true;
          ((short*&)dataRead)++;
        }
      } else if (type == INTEGER) {
        if (j == primaryKeyNumber) {
          if (*(int*)pkToFind == *(int*)dataRead) return true;
        }
        ((int*&)dataRead)++;
      } else if (type == REAL) {
        if (j == primaryKeyNumber) {
          if (*(float*)pkToFind == *(float*)dataRead) return true;
        }
        ((float*&)dataRead)++;
      }
    }
    ((char*&)dataRead)++;  // end record character
  }
  return false;
}

/// @brief print the contents of a table
/// @param table_name
void Database::printTable(char* table_name) {
  if (method == HASHED) {
    printTableHashed(table_name);
    return;
  }
  // printDBPrimary();
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

/// @brief print the contents of a hashed table
/// @param table_name
void Database::printTableHashed(char* table_name) {
  // printDBPrimary();
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

  int fixedLength = -1;
  if (!variable) {
    fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  }

  // for each of the hash buckets
  void* bucketBlock = (void*)*(long*)((uintptr_t)record + data_root_offset);

  for (int k = 0; k < num_hash_buckets; k++) {
    void* dataRead = (void*)(*(long*)bucketBlock);  // data root
    void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
    int recordsInBlock = *(int*)((uintptr_t)bucketBlock + hash_record_count_offset);

    // we just need the attribute types so we know what to cast it into
    for (int i = 0; i < recordsInBlock; i++) {
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

    bucketBlock = (void*)((uintptr_t)bucketBlock + hashed_size);
  }
}

/// @brief print a table given certain attributes and a condition
/// @param table_name
/// @param fieldsToPrint attributes to print
/// @param specialName the name of the attribute that we are trying to compare
/// @param comparator a string of a comparator ex. >, =, <
/// @param target the target value that we want the specialName attribute to be compared with
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
    if (specialName == nullptr) {
      isTarget[i] = true;  // if we don't have a special, by default we're going to want to print everything out
    } else {
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
          if (j == special) isTarget[i] = compare(target, comparator, (void*)buff, strlen(buff), VARCHAR);

          // this strlen is a bit risky but should be ok
          //

        } else if (type == CHAR) {
          // get char length, set up a buffer
          int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
          char buff[buffSize];
          memset(buff, '\0', buffSize);
          // copy
          strncpy(buff, (char*)dataRead, buffSize);

          if (j == special) isTarget[i] = compare(target, comparator, (void*)buff, strlen(buff), CHAR);
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
    }
    //  cout << "target" << isTarget[i] << " ";
    //  cout << endl;
  }
  printTableGivenHelper(variable, numAttr, numToLookFor, dataRoot, dataCurrRecordCount, fixedLength, attrRecords, attrTypes, interest, isTarget);
}

/// @brief print a table given certain attributes and a condition, for hashed records
/// @param table_name table to update
/// @param fieldsToPrint attributes to print
/// @param specialName the name of the attribute that we are trying to compare
/// @param comparator a string of a comparator ex. >, =, <
/// @param target the target value that we want the specialName attribute to be compared with
void Database::printTableGivenHashed(char* table_name, vector<char*> fieldsToPrint, char* specialName, char* comparator, char* target) {
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

  // go through each bucket
  void* bucketBlock = (void*)*(long*)((uintptr_t)record + data_root_offset);
  // void* dataRead = bucketBlock;
  // void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  if (variable) {
    fixedLength = -1;
  }
  // we just need the attribute types so we know what to cast it into

  for (int k = 0; k < num_hash_buckets; k++) {
    //  cout << "k: " << k << endl;
    void* dataRead = (void*)(*(long*)bucketBlock);  // data root
    void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
    int recordsInBlock = *(int*)((uintptr_t)bucketBlock + hash_record_count_offset);
    bool isTarget[recordsInBlock];
    for (int i = 0; i < recordsInBlock; i++) {
      checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

      if (specialName == nullptr) isTarget[i] = true;  // if we don't have a special, by default we're going to want to print everything out

      for (int j = 0; j < numAttr; j++) {
        dataType type = attrTypes[j];
        if (type == VARCHAR) {
          // get max, set up a buffer
          int buffSize = *(int*)((uintptr_t)attrRecords[j] + attr_length_offset);
          char buff[buffSize];
          memset(buff, '\0', buffSize);

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
      // cout << "isTarget[i]: " << isTarget[i] << endl;
      if (variable) {
        ((char*&)dataRead)++;  // END_RECORD_CHAR
      }
    }
    void* bucketBlockRoot = (void*)*(long*)bucketBlock;

    printTableGivenHelper(variable, numAttr, numToLookFor, bucketBlockRoot, recordsInBlock, fixedLength, attrRecords, attrTypes, interest, isTarget);
    bucketBlock = (void*)((uintptr_t)bucketBlock + hashed_size);
  }
}

/// @brief a helper for printing tables
/// @param variable whether the record is variable or fixed
/// @param numAttr number of attributes associated with this table
/// @param numToLookFor the number of attributes to look for
/// @param dataRoot the root to the table with the data
/// @param dataCurrRecordCount the number of data records
/// @param fixedLength the length of the records
/// @param attrRecords an array of pointers to each attribute record
/// @param attrTypes an array of the types of each attribute
/// @param interest an array if the attribute is of interest to print
/// @param isTarget an array if the record meets the condition to print
void Database::printTableGivenHelper(bool variable, int numAttr, int numToLookFor, void* dataRoot, int dataCurrRecordCount, int fixedLength, void* attrRecords[], dataType attrTypes[], bool interest[], bool isTarget[]) {
  void* dataRead = dataRoot;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < dataCurrRecordCount; i++) {
    checkSpaceSearch(fixedLength, dataRead, dataReadEnd);

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

        for (int k = 0; k < buffSize; k++) {
          char a = *(char*)dataRead;
          ((char*&)dataRead)++;  // move on to next byte regardless
          if (a == END_FIELD_CHAR) {
            break;  // don't copy the end field char
          }
          buff[k] = a;
        }

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

/// @brief update the table given an attribute to update and a condition
/// @param table_name table to update
/// @param attrToUpdate the attribute to update
/// @param value to update the attribute to
/// @param specialName the name of the attribute to compare
/// @param comparator a string of a comparator ex. >, =, <
/// @param target the target value that we want the specialName attribute to be compared with
/// @return the number of records updated
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

  if (variable) {
    updateTableGivenVariableHelper(record, numAttr, attrRecords, isTarget, attrTypes, fixedLength, dataCurrRecordCount, dataRead, dataReadEnd, toUpdate, value);
    return numUpdate;
  }
  // update table fixed
  updateTableGivenFixedHelper(dataCurrRecordCount, fixedLength, dataRead, dataReadEnd, numAttr, attrRecords, attrTypes, isTarget, toUpdate, value);
  return numUpdate;
}

/// @brief update the table given an attribute to update and a condition for hashed insertion method
/// @param table_name table to update
/// @param attrToUpdate the attribute to update
/// @param value to update the attribute to
/// @param specialName the name of the attribute to compare
/// @param comparator a string of a comparator ex. >, =, <
/// @param target the target value that we want the specialName attribute to be compared with
/// @return the number of records updated
int Database::updateTableGivenHashed(char* table_name, char* attrToUpdate, char* value, char* specialName, char* comparator, char* target) {
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
  // void* dataRead = dataRoot;
  // void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  int fixedLength = -1;
  if (!variable) {
    fixedLength = calculateMaxDataRecordSize(attrRecords[0], numAttr);  // hopefully that works!
  }
  // we just need the attribute types so we know what to cast it into

  void* bucketBlock = (void*)*(long*)((uintptr_t)record + data_root_offset);

  for (int b = 0; b < num_hash_buckets; b++) {
    void* dataRead = (void*)(*(long*)bucketBlock);  // data root
    void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
    int recordsInBlock = *(int*)((uintptr_t)bucketBlock + hash_record_count_offset);

    bool isTarget[recordsInBlock];

    for (int i = 0; i < recordsInBlock; i++) {
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
          cout << "integer" << endl;
          if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, INTEGER);
          cout << "done" << endl;
          ((int*&)dataRead)++;
        } else if (type == REAL) {
          if (j == special) isTarget[i] = compare(target, comparator, dataRead, -1, REAL);
          ((float*&)dataRead)++;
        }
      }
      if (isTarget[i]) numUpdate++;
      if (variable) ((char*&)dataRead)++;

      // cout << "target" << isTarget[i] << " ";
      // cout << endl;
    }

    if (variable) {
      // cout << "before variable" << endl;
      updateTableGivenVariableHelperHashed(bucketBlock, numAttr, attrRecords, isTarget, attrTypes, fixedLength, recordsInBlock, toUpdate, value);
      // cout << "after variable" << endl;
    } else {
      // reset reads
      dataRead = (void*)(*(long*)bucketBlock);  // data root
      dataReadEnd = (void*)(((long*)((uintptr_t)bucketBlock + (uintptr_t)BLOCK_SIZE)) - 1);
      updateTableGivenFixedHelper(recordsInBlock, fixedLength, dataRead, dataReadEnd, numAttr, attrRecords, attrTypes, isTarget, toUpdate, value);
    }
    bucketBlock = (void*)((uintptr_t)bucketBlock + hashed_size);
  }
  return numUpdate;
}

/// @brief helper to updateTableGiven, changes fixed records
/// @param dataCurrRecordCount the number of records associated with the table
/// @param fixedLength the length in bytes of the record
/// @param dataRead current read head
/// @param dataReadEnd end of current read head's block
/// @param numAttr number of attributes associated with record
/// @param attrRecords an array of pointers to each attribute record
/// @param attrTypes an array of the types of each attribute
/// @param interest an array if the attribute is of interest to print
/// @param isTarget an array if the record meets the condition to print
/// @param value the target value that we want the specialName attribute to be compared with
void Database::updateTableGivenFixedHelper(int dataCurrRecordCount, int fixedLength, void* dataRead, void* dataReadEnd, int numAttr, void* attrRecords[], dataType attrTypes[], bool isTarget[], int toUpdate, char* value) {
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
}

/// @brief a duplicate of update() but returns number of values update, used for testing
/// @param table_name the table to update
/// @param length should be 3
/// @param  first should be the attribute to update, second is the conditional
/// @return number of records updated
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
/// @brief helper to updateTableGiven, changes variable records
/// @param record to update
/// @param numAttr number of attributes associated with record
/// @param attrRecords an array of pointers to each attribute record
/// @param isTarget an array if the record meets the condition to print
/// @param attrTypes an array of the types of each attribute
/// @param fixedLength the length in bytes of the record
/// @param dataCurrRecordCount the number of records associated with the table
/// @param dataRead current read head
/// @param dataReadEnd end of current read head's block
/// @param toUpdate the number of the attribute to update
/// @param value the target value that we want the specialName attribute to be compared with
void Database::updateTableGivenVariableHelper(void*& record, int numAttr, void* attrRecords[], bool isTarget[], dataType attrTypes[], int fixedLength, int dataCurrRecordCount, void* dataRead, void* dataReadEnd, int toUpdate, char* value) {
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
}

// TODO: fix
/// @brief helper to updateTableGivenHashed, changes variable records
/// @param bucketBlock the bucket number to update
/// @param numAttr number of attributes associated with record
/// @param attrRecords an array of pointers to each attribute record
/// @param isTarget an array if the record meets the condition to print
/// @param attrTypes an array of the types of each attribute
/// @param fixedLength the length in bytes of the record
/// @param recordsInBlock the number of records associated with the current bucket
/// @param toUpdate the number of the attribute to update
/// @param value the target value that we want the specialName attribute to be compared with
void Database::updateTableGivenVariableHelperHashed(void*& bucketBlock, int numAttr, void* attrRecords[], bool isTarget[], dataType attrTypes[], int fixedLength, int recordsInBlock, int toUpdate, char* value) {
  // get each bucket ready to accept records
  void* oldBucketBlock = bucketBlock;  // keep track of the old bucket
  cout << "bucketBlock " << bucketBlock << endl;
  // we only have to do this for one bucket
  void* dataRoot = NULL;
  void* dataCurr = NULL;
  void* dataCurrEnd = NULL;
  int dataCurrBlockCount = 0;  // printing this stuff out
  initializeNewBlock(dataRoot, dataCurr, dataCurrEnd, dataCurrBlockCount);
  // cout << "i: " << i << endl;
  // cout << "bucketBlock " << bucketBlock << endl;
  cout << "dataRoot " << dataRoot << endl;
  cout << "dataCurr " << dataCurr << endl;

  *(long*)bucketBlock = (uintptr_t)dataRoot;
  *(long*)((uintptr_t)bucketBlock + hash_curr_offset) = (uintptr_t)dataCurr;
  *(long*)((uintptr_t)bucketBlock + hash_curr_end_offset) = (uintptr_t)dataCurrEnd;
  *(int*)((uintptr_t)bucketBlock + hash_block_count_offset) = dataCurrBlockCount;

  // so annoying

  int maxRecordSize = calculateMaxDataRecordSize(attrRecords[0], numAttr);
  // cout << "got here" << endl;

  void* dataRead = bucketBlock;
  void* dataReadEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)BLOCK_SIZE)) - 1);
  for (int i = 0; i < recordsInBlock; i++) {
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
    // cout << "before unordered add" << endl;
    addUnorderedToTableHashed(bufferToSend, actualRecordSize, bucketBlock);
    // cout << "after unordered add" << endl;
    //  cout << "after add" << endl;
    //  printTable(table_name);
    //  TODO: free the old tables
  }
}

/// @brief get the data type given a string describing the type
/// @param type a string
/// @return the dataType
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

/// @brief get the type of the primary key
/// @param dbAttributes pointer to dbAttributes, first record associated with table
/// @param numAttributes number of attributes associated with the table
/// @param primaryKeyNum the number attribute that the primary key is
/// @return the type of the primary key
dataType Database::getPrimaryKeyType(void* dbAttributes, int numAttributes, int primaryKeyNum) {
  void* readDbAttr = dbAttributes;
  void* readDbAttrEnd = findEndOfBlock(readDbAttr);
  for (int i = 0; i < numAttributes; i++) {
    checkSpaceSearch(db_attr_record_size, readDbAttr, readDbAttrEnd);
    if (i == primaryKeyNum) {
      return *(dataType*)((uintptr_t)readDbAttr + attr_type_offset);
    }
  }
  return INVALID;
}

/// @brief get the length of the primary key, useful if primary key is VARCHAR or CHAR
/// @param dbAttributes pointer to dbAttributes, first record associated with table
/// @param numAttributes  number of attributes associated with the table
/// @param primaryKeyNum the number attribute t
int Database::getPrimaryKeyLength(void* dbAttributes, int numAttributes, int primaryKeyNum) {
  void* readDbAttr = dbAttributes;
  void* readDbAttrEnd = findEndOfBlock(readDbAttr);
  for (int i = 0; i < numAttributes; i++) {
    checkSpaceSearch(db_attr_record_size, readDbAttr, readDbAttrEnd);
    if (i == primaryKeyNum) {
      return *(int*)((uintptr_t)readDbAttr + attr_length_offset);
    }
  }
  return -2;
}

/// @brief computes the bucket number based on hash code
/// @param pk primary key to hash
/// @param pkLength length of primary key if VARCHAR or CHAR
/// @param pkType the dataType of the primary key
/// @return
int Database::getBucketNumber(void* pk, int pkLength, dataType pkType) {
  int hashCode = -1;
  if (pkType == VARCHAR || pkType == CHAR) {
    char buffer[pkLength];
    strncpy(buffer, (char*)pk, pkLength);
    // taken from https://stackoverflow.com/questions/2624192/good-hash-function-for-strings
    hashCode = 7;
    for (int i = 0; i < pkLength; i++) {
      hashCode = hashCode * 31 + buffer[i];
    }
  } else if (pkType == SMALLINT) {
    hashCode = (*(short*)pk * 100) % num_hash_buckets;
  } else if (pkType == INTEGER) {
    hashCode = (*(int*)pk * 100) % num_hash_buckets;
  } else if (pkType == REAL) {
    hashCode = ((int)*(float*)pk * 100) % num_hash_buckets;
  }
  return hashCode;
}

/// @brief get the n from a create table for CHAR and VARCHAR
/// @param str string to parse
/// @return the n in the string
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

/// @brief calculate the maximum that the record size could be based on the attribute types
/// @param ptrToFirstAttribute
/// @param numberOfAttributes associated with the record
/// @return max number of bytes the record could be
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

/// @brief return the value of a comparison
/// @param targetValue value to compare to
/// @param comparator ex. >, =, <
/// @param candidate value to compare
/// @param candidateLength length of value (useful if VARCHAR or CHAR)
/// @param type candidate/targetValue dataType
/// @return
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
    cout << "hello int" << endl;
    int tar = atoi(targetValue);
    cout << "mid" << endl;
    int num = *(int*)candidate;
    cout << "target " << tar << " num " << num << endl;
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

/// @brief free the malloc'd space for a chain of data blocks, given the root
/// @param dataRoot
/// @param dataCurrBlockCount
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