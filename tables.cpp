#include "tables.h"

void initializeDB() {
  initializeNewBlock(db_primary, db_primary_curr, db_primary_curr_end);
  initializeNewBlock(db_attr, db_attr_curr, db_attr_curr_end);
}

// not tested
void initializeNewBlock(void* root, void* curr, void* currEnd) {
  root = (void*)calloc(BLOCK_SIZE, 1);
  curr = root;
  currEnd = (void*)(((long*)((uintptr_t)root + (uintptr_t)BLOCK_SIZE)) - 1);
  return;
}

// not tested
// not a new block, root doesn't change
void addNewBlock(void* curr, void* currEnd) {
  if (curr == NULL || currEnd == NULL) {
    // error
    cerr << "the root block has not been initialized yet" << endl;
    return;
  }
  void* toAdd = (void*)calloc(BLOCK_SIZE, 1);                                  // allocate space for this new block
  *(long*)currEnd = (uintptr_t)toAdd;                                          // add pointer to new block to the end of this block
  curr = toAdd;                                                                // update curr to be the beginning of this new block
  currEnd = (void*)(((long*)((uintptr_t)toAdd + (uintptr_t)BLOCK_SIZE)) - 1);  // update the end to be this new end
}
