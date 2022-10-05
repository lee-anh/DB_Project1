#include "tables.h"
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