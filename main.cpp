// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

// local
#include "utils.h"
using namespace std;

int main(int argc, char* argv[]) {
  cout << "SIZE OF FLOAT " << sizeof(float) << endl;
  initializeDB();
  printDBPrimary();
  create_table("movies", "id", 6,
               "id", "integer",
               "title", "char(32)",
               "length", "smallint");

  create_table("stars", "id", 6,
               "id", "integer",
               "fname", "char(32)",
               "lname", "char(32)");

  insert("movies", 3, 27, "Star Wars", 120);
  printDBPrimary();

  void* blk = calloc(BLOCK_SIZE, 1);

  void* ptr = blk;

  void* blkEnd = (void*)(((long*)((uintptr_t)blk + (uintptr_t)BLOCK_SIZE)) - 1);

  // blkEnd = (void*)(((long*)blkEnd) - 1);  // decrement so that we can point to the next block

  cout << (uintptr_t)blkEnd - (uintptr_t)blk << endl;  // check the block size
  cout << sizeof(void*) << endl;

  // can we fill in a string all at once given it's length?
  char buffer1[1024];
  memset(buffer1, '\0', 1024);
  cout << "buffer1 strlen: " << strlen(buffer1) << endl;

  // perfect!
  strncpy((char*)ptr, "hello", strlen("hello"));
  (char*&)ptr += strlen("hello");
  strncpy((char*)ptr, "world", strlen("world"));
  (char*&)ptr += strlen("world");

  *(dataType*)ptr = SMALLINT;
  cout << "dataType: " << *(dataType*)ptr << endl;
  ((dataType*&)ptr)++;

  /*
  void* ptr2 = blk;
  for (int i = 0; i < 6; i++) {
    cout << *(char*)ptr2 << endl;
    ((char*&)ptr2)++;
  }
  */

  cout << (char*)blk << endl;

  // now can we print out the whole word at once?
  // strncpy to a buffer and return that? or just strncpy?
  char buffer[256];
  memset(buffer, '\0', 256);
  strncpy(buffer, (char*)blk, 5);
  cout << buffer << endl;

  // store an int
  int a = 123;
  *(int*)ptr = a;  // put a in memory
  cout << (*(int*)ptr) << endl;
  ((int*&)ptr)++;  // increment the pointer for the correct amount

  // store a pointer
  // we have to use the long type to match the number of bytes a void pointer is
  void* test = malloc(sizeof(char));
  *(long*)ptr = (uintptr_t)test;

  void* testTemp = (void*)*(long*)ptr;  // get it back to void pointer form
  cout << testTemp << endl;
  // move out the size of a pointer
  ((char*&)ptr)++;
  void* test1 = malloc(sizeof(char));
  void* test2 = malloc(sizeof(char));
  *(long*)ptr = (uintptr_t)test2;

  // long* longptr = NULL;
  long longval = 1;
  cout << "long size: " << sizeof(longval) << endl;

  // print a pointer
  // cout << hex << *(long*)ptr << endl;

  void* testTemp2 = (void*)*(long*)ptr;  // get it back to void pointer form
  cout << testTemp2 << endl;
  ((long*&)ptr)++;

  // how can we print variable length ones?
  // we could do helper functions that look for the next terminating character

  // we are going to be doing a lot of pointer stuff and a lot of helper functions
  // since we can't abstract into struct land
  cout << "as int: " << *(int*)blk << endl;

  // for the system dump we need to be able to tell what type each variable
  // within the record is
  free(test);
  free(test1);
  free(test2);
  free(blk);

  return 0;
}