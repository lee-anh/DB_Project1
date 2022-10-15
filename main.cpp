// c
#include <stdint.h>
#include <string.h>

// cpp
#include <iostream>

// local
#include "Database.h"

using namespace std;

int main(int argc, char* argv[]) {
  cout << "SIZE OF FLOAT " << sizeof(float) << endl;
  Database* db = new Database(UNORDERED);
  // Database* db = new Database(ORDERED);
  db->printDBPrimary();
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  db->create_table("stars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");
  db->printDBPrimary();

  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");
  db->printDBPrimary();
  db->printDBAttr();

  db->insert("stars", 3, 13, "Harrison", "Ford");

  db->insert("critics", 3, 123, "Mario", "Bros");
  db->insert("critics", 3, 123, "Mario", "Bros");
  db->insert("critics", 3, 100, "Princess", "Peach");
  db->insert("critics", 3, 125, "Luigi", "Bros");
  db->insert("critics", 3, 124, "Toad", "Factory");
  db->insert("critics", 3, 145, "Donkey", "Kong");
  db->insert("critics", 3, 180, "Bowser", "Monster");
  db->insert("critics", 3, 5, "Boo", "Ghost");
  db->insert("critics", 3, 113, "Princess", "Daisy");
  db->insert("critics", 3, 114, "Princess", "Zelda");
  db->insert("critics", 3, 115, "Princess", "Daisy");
  db->insert("critics", 3, 116, "Princess", "Daisy");
  db->insert("critics", 3, 117, "Princess", "Daisy");
  db->insert("critics", 3, 118, "Princess", "Daisy");
  db->insert("critics", 3, 1100, "Princess", "Peach");
  db->insert("critics", 3, 1125, "Luigi", "Bros");
  db->insert("critics", 3, 1124, "Toad", "Factory");
  db->insert("critics", 3, 1145, "Donkey", "Kong");
  db->insert("critics", 3, 1180, "Bowser", "Monster");
  db->insert("critics", 3, 15, "Boo", "Ghost");
  db->insert("critics", 3, 1113, "Princess", "Daisy");
  db->insert("critics", 3, 1114, "Princess", "Zelda");
  db->insert("critics", 3, 1115, "Princess", "Daisy");
  db->insert("critics", 3, 1116, "Princess", "Daisy");
  db->insert("critics", 3, 1117, "Princess", "Daisy");
  db->insert("critics", 3, 1118, "Princess", "Daisy");
  db->insert("critics", 3, 2123, "Mario", "Bros");
  db->insert("critics", 3, 1223, "Mario", "Bros");
  db->insert("critics", 3, 1200, "Princess", "Peach");
  db->insert("critics", 3, 1225, "Luigi", "Bros");
  db->insert("critics", 3, 1224, "Toad", "Factory");
  db->insert("critics", 3, 1245, "Donkey", "Kong");
  db->insert("critics", 3, 1280, "Bowser", "Monster");
  db->insert("critics", 3, 52, "Boo", "Ghost");
  db->insert("critics", 3, 1213, "Princess", "Daisy");
  db->insert("critics", 3, 1214, "Princess", "Zelda");
  db->insert("critics", 3, 1215, "Princess", "Daisy");
  db->insert("critics", 3, 1216, "Princess", "Daisy");
  db->insert("critics", 3, 1217, "Princess", "Daisy");
  db->insert("critics", 3, 1218, "Princess", "Daisy");
  db->insert("critics", 3, 12100, "Princess", "Peach");
  db->insert("critics", 3, 12125, "Luigi", "Bros");
  db->insert("critics", 3, 12124, "Toad", "Factory");
  db->insert("critics", 3, 12145, "Donkey", "Kong");
  db->insert("critics", 3, 12180, "Bowser", "Monster");
  db->insert("critics", 3, 125, "Boo", "Ghost");
  db->insert("critics", 3, 12113, "Princess", "Daisy");
  db->insert("critics", 3, 122114, "Princess", "Zelda");
  db->insert("critics", 3, 11125, "Princess", "Daisy");
  db->insert("critics", 3, 12116, "Princess", "Daisy");
  db->insert("critics", 3, 12117, "Princess", "Daisy");
  db->insert("critics", 3, 12118, "Princess", "Daisy");

  // load test

  /*
    db->create_table("astars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("bstars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");

    db->create_table("cstars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("dstars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("estars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("fstars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("gstars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("aastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("bastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");

    db->create_table("castars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("dastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("eastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("fastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->create_table("gastars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    */

  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->printDBPrimary();
  db->insert("movies", 3, 28, "Rouge One", 3.0);
  db->insert("movies", 3, 25, "The Last Starfighter", 3.0);
  db->insert("movies", 3, 26, "Princess Diaries", 5.0);

  db->insert("movies", 3, 127, "Frozen", 4.2);
  db->insert("movies", 3, 228, "Big Hero 6", 3.0);
  db->insert("movies", 3, 237, "Cars", 4.2);
  db->insert("movies", 3, 248, "Cars 2", 3.0);
  db->insert("movies", 3, 257, "Toy Story", 4.2);
  db->insert("movies", 3, 268, "Tarzan", 3.0);
  db->insert("movies", 3, 277, "Little Shop of Horrors", 4.2);
  db->insert("movies", 3, 288, "Cinderella", 3.0);
  db->insert("movies", 3, 297, "Sleeping Beauty", 4.2);
  db->insert("movies", 3, 208, "Tangled", 3.0);
  db->insert("movies", 3, 217, "Honey I Shrunk the Kids", 4.2);
  db->insert("movies", 3, 281, "Back to the Future", 3.0);
  db->insert("movies", 3, 272, "Father of the Bride", 4.2);
  db->insert("movies", 3, 283, "Cheaper by the Dozen", 3.0);
  db->insert("movies", 3, 274, "Avengers", 4.2);
  db->insert("movies", 3, 285, "Joy Luck Club", 3.0);
  db->insert("movies", 3, 276, "Crazy Rich Asians", 4.2);
  db->insert("movies", 3, 287, "Solo", 3.0);
  db->insert("movies", 3, 278, "Captain America", 4.2);
  db->insert("movies", 3, 280, "Iron Man", 3.0);
  db->insert("movies", 3, 272, "Thor", 4.2);
  db->insert("movies", 3, 283, "Antman", 3.0);
  db->select("movies", 1, "title, rating");
  db->select("movies", 2, "id, title", "id = 27 OR title = Star Wars");

  db->printDBPrimary();
  cout << "Num db primary blocks " << db->db_primary_blocks << endl;

  cout << "Num db attribute blocks " << db->db_attr_blocks << endl;
  cout << "db_attr " << db->db_attr << endl;
  cout << "db_attr_curr_end " << db->db_attr_curr_end << endl;

  /*
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

  void* ptr2 = blk;
  for (int i = 0; i < 6; i++) {
    cout << *(char*)ptr2 << endl;
    ((char*&)ptr2)++;
  }


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
  */

  return 0;
}