// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

// local
#include "globals.h"
#include "tables.h"
using namespace std;

int main(int argc, char* argv[]) {
  // another trick is (int*&) to get the job done

  // since we don't need to use
  void* blk = calloc(BLOCK_SIZE, 1);

  void* ptr = blk;

  void* blkEnd = (void*)(((long*)((uintptr_t)blk + (uintptr_t)BLOCK_SIZE)) - 1);

  // blkEnd = (void*)(((long*)blkEnd) - 1);  // decrement so that we can point to the next block

  cout << (uintptr_t)blkEnd - (uintptr_t)blk << endl;  // check the block size
  cout << sizeof(void*) << endl;
  // need some way to check the end - helper function
  // fill it with chars
  /*
  *(char*)ptr = 'h';
  ((char*&)ptr)++;
  *(char*)ptr = 'e';
  ((char*&)ptr)++;
  *(char*)ptr = 'l';
  ((char*&)ptr)++;
  *(char*)ptr = 'l';
  ((char*&)ptr)++;
  *(char*)ptr = 'o';
  ((char*&)ptr)++;
  *(char*)ptr = '\0';
  */

  // can we fill in a string all at once given it's length?

  // perfect!
  strncpy((char*)ptr, "hello", strlen("hello"));
  (char*&)ptr += strlen("hello");
  strncpy((char*)ptr, "world", strlen("world"));
  (char*&)ptr += strlen("world");

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
  cout << test << endl;
  *(long*)ptr = (uintptr_t)test;

  // cout << hex << *(long*)ptr << endl;
  void* testTemp = (void*)*(long*)ptr;  // get it back to void pointer form
  cout << testTemp << endl;

  // how can we print variable length ones?
  // we could do helper functions that look for the next terminating character

  // we are going to be doing a lot of pointer stuff and a lot of helper functions
  // since we can't abstract into struct land
  cout << "as int: " << *(int*)blk << endl;

  // for the system dump we need to be able to tell what type each variable
  // within the record is

  free(blk);

  return 0;
}