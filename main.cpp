#include <stdint.h>
#include <string.h>

#include <iostream>

#include "Database.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (atoi(argv[1]) == 1) {
    cout << "____________________ORDERED________________________" << endl;

    Database* db1 = new Database(UNORDERED);
    db1->printDBPrimary();
    db1->create_table("movies", "id", 6,
                      "id", "integer",
                      "title", "char(32)",
                      "rating", "real");

    db1->create_table("test", "id", 8,
                      "id", "integer",
                      "director", "char(32)",
                      "title", "varchar(32)",
                      "rating", "real");

    db1->insert("test", 4, 12, "hello", "world", 2.0);
    db1->insert("test", 4, 12, "hello", "world", 4.1);

    db1->create_table("stars", "id", 6,
                      "id", "integer",
                      "fname", "char(32)",
                      "lname", "char(32)");

    db1->create_table("critics", "id", 6,
                      "id", "integer",
                      "fname", "varchar(32)",
                      "lname", "varchar(32)");

    db1->create_table("movies1", "director1", 8,
                      "id1", "smallint",
                      "title1", "varchar(32)",
                      "director1", "char(32)",
                      "rating1", "real");
    db1->insert("movies1", 4, 27, "Star Wars", "George Lucas", 4.2);
    db1->insert("movies1", 4, 27, "Star Wars", "George Lucas", 4.2);

    db1->insert("stars", 3, 13, "Harrison", "Ford");

    db1->insert("critics", 3, 27, "Mario", "Bros");
    db1->insert("critics", 3, 27, "Mario", "Bros");
    db1->insert("critics", 3, 28, "Princess", "Peach");
    db1->insert("critics", 3, 25, "Luigi", "Bros");
    db1->insert("critics", 3, 26, "Toad", "Mushroom");
    db1->insert("critics", 3, 145, "Donkey", "Kong");
    db1->insert("critics", 3, 180, "Bowser", "Monster");
    db1->insert("critics", 3, 5, "Boo", "Ghost");
    db1->insert("critics", 3, 113, "Princess", "Daisy");
    db1->insert("critics", 3, 114, "Princess", "Zelda");
    db1->insert("critics", 3, 115, "Princessy", "Rosalina");
    db1->insert("critics", 3, 116, "Evil", "Waluigi");
    db1->insert("critics", 3, 117, "Toadette", "Mushroom");
    db1->insert("critics", 3, 118, "Evil", "Wario");
    db1->insert("critics", 3, 1100, "Goomba", "Fiend");
    db1->insert("critics", 3, 1125, "Koopa", "Troopa");
    db1->insert("critics", 3, 1124, "Shy", "Guy");
    db1->insert("critics", 3, 1145, "Yoshi", "Kong");
    db1->insert("critics", 3, 1180, "Bowser", "Dragon");
    db1->insert("critics", 3, 15, "Baby", "Mario");
    db1->insert("critics", 3, 1113, "Baby", "Luigi");
    db1->insert("critics", 3, 1114, "Baby", "Zelda");
    db1->insert("critics", 3, 1115, "Baby", "Daisy");
    db1->insert("critics", 3, 1116, "Kirby", "Pink");
    db1->insert("critics", 3, 1117, "Diddy", "Kong");
    db1->insert("critics", 3, 1118, "Princess", "Daisy");
    db1->insert("critics", 3, 2123, "Mario", "Bros");
    db1->insert("critics", 3, 1223, "Mario", "Bros");
    db1->insert("critics", 3, 1200, "Princess", "Peach");
    db1->insert("critics", 3, 1225, "Luigi", "Bros");
    db1->insert("critics", 3, 1224, "Toad", "Factory");
    db1->insert("critics", 3, 1245, "Donkey", "Kong");
    db1->insert("critics", 3, 1280, "Bowser", "Monster");
    db1->insert("critics", 3, 52, "Boo", "Ghost");
    db1->insert("critics", 3, 1213, "Princess1", "Daisy");
    db1->insert("critics", 3, 1214, "Princess2", "Zelda");
    db1->insert("critics", 3, 1215, "Princess3", "Daisy");
    db1->insert("critics", 3, 1216, "Princess4", "Daisy");
    db1->insert("critics", 3, 1217, "Princess5", "Daisy");
    db1->insert("critics", 3, 1218, "Princess6", "Daisy");
    db1->insert("critics", 3, 12100, "Princess", "Peach");
    db1->insert("critics", 3, 12125, "Luigi", "Bros");
    db1->insert("critics", 3, 12124, "Toad", "Factory");
    db1->insert("critics", 3, 12145, "Donkey", "Kong");
    db1->insert("critics", 3, 12180, "Bowser", "Monster");
    db1->insert("critics", 3, 125, "Boo", "Ghost");
    db1->insert("critics", 3, 12113, "Princess7", "Daisy");
    db1->insert("critics", 3, 122114, "Princess", "Zelda");
    db1->insert("critics", 3, 11125, "Princess8", "Daisy");
    db1->insert("critics", 3, 12116, "Princess9", "Daisy");
    db1->insert("critics", 3, 12117, "Princess10", "Daisy");
    db1->insert("critics", 3, 12118, "Princess11", "Daisy");

    db1->insert("movies", 3, 27, "Star Wars", 4.2);
    db1->insert("movies", 3, 28, "Rouge One", 3.0);
    db1->insert("movies", 3, 25, "The Last Starfighter", 3.0);
    db1->insert("movies", 3, 26, "Princess Diaries", 5.0);
    db1->insert("movies", 3, 127, "Frozen", 4.2);
    db1->insert("movies", 3, 228, "Big Hero 6", 3.0);
    db1->insert("movies", 3, 237, "Cars", 4.2);
    db1->insert("movies", 3, 248, "Cars 2", 3.0);
    db1->insert("movies", 3, 257, "Toy Story", 4.2);
    db1->insert("movies", 3, 268, "Tarzan", 3.0);
    db1->insert("movies", 3, 277, "Little Shop of Horrors", 4.2);
    db1->insert("movies", 3, 288, "Cinderella", 3.0);
    db1->insert("movies", 3, 297, "Sleeping Beauty", 4.2);
    db1->insert("movies", 3, 208, "Tangled", 3.0);
    db1->insert("movies", 3, 217, "Honey I Shrunk the Kids", 4.2);
    db1->insert("movies", 3, 281, "Back to the Future", 3.0);
    db1->insert("movies", 3, 272, "Father of the Bride", 4.2);
    db1->insert("movies", 3, 283, "Cheaper by the Dozen", 3.0);
    db1->insert("movies", 3, 274, "Avengers", 4.2);
    db1->insert("movies", 3, 285, "Joy Luck Club", 3.0);
    db1->insert("movies", 3, 276, "Crazy Rich Asians", 4.2);
    db1->insert("movies", 3, 287, "Solo", 3.0);
    db1->insert("movies", 3, 278, "Captain America", 4.2);
    db1->insert("movies", 3, 280, "Iron Man", 3.0);
    db1->insert("movies", 3, 272, "Thor", 4.2);
    db1->insert("movies", 3, 284, "Antman", 3.0);

    db1->select("movies", 1, "title, rating");
    db1->select("movies", 2, "id, title", "id = 27");
    db1->update("movies", 3, "rating", "5.0", "id > 27");
    db1->update("movies", 3, "rating", "4.0", "title = Frozen");
    db1->select("movies", 1, "*");

    db1->select("critics", 2, "fname, lname", "id = 115");
    db1->update("critics", 3, "lname", "Rosy", "id = 115");

    // db1 system dump
    db1->printTable("critics");
    db1->printTable("movies");
    db1->printDBPrimary();
    db1->printDBAttr();
    db1->closeDB();
  }
  if (atoi(argv[1]) == 2) {
    cout << "____________________ORDERED________________________" << endl;
    Database* db = new Database(ORDERED);
    db->printDBPrimary();
    db->create_table("movies", "id", 6,
                     "id", "integer",
                     "title", "char(32)",
                     "rating", "real");

    db->create_table("test", "id", 8,
                     "id", "integer",
                     "director", "char(32)",
                     "title", "varchar(32)",
                     "rating", "real");

    db->insert("test", 4, 12, "hello", "world", 2.0);
    db->insert("test", 4, 12, "hello", "world", 4.1);

    db->create_table("stars", "id", 6,
                     "id", "integer",
                     "fname", "char(32)",
                     "lname", "char(32)");
    db->printDBPrimary();

    db->create_table("critics", "id", 6,
                     "id", "integer",
                     "fname", "varchar(32)",
                     "lname", "varchar(32)");

    db->create_table("movies1", "director1", 8,
                     "id1", "smallint",
                     "title1", "varchar(32)",
                     "director1", "char(32)",
                     "rating1", "real");
    db->insert("movies1", 4, 27, "Star Wars", "George Lucas", 4.2);
    db->insert("movies1", 4, 27, "Star Wars", "George Lucas", 4.2);

    db->insert("stars", 3, 13, "Harrison", "Ford");

    db->insert("critics", 3, 27, "Mario", "Bros");
    db->insert("critics", 3, 27, "Mario", "Bros");
    db->insert("critics", 3, 28, "Princess", "Peach");
    db->insert("critics", 3, 25, "Luigi", "Bros");
    db->insert("critics", 3, 26, "Toad", "Mushroom");
    db->insert("critics", 3, 145, "Donkey", "Kong");
    db->insert("critics", 3, 180, "Bowser", "Monster");
    db->insert("critics", 3, 5, "Boo", "Ghost");
    db->insert("critics", 3, 113, "Princess", "Daisy");
    db->insert("critics", 3, 114, "Princess", "Zelda");
    db->insert("critics", 3, 115, "Princessy", "Rosalina");
    db->insert("critics", 3, 116, "Evil", "Waluigi");
    db->insert("critics", 3, 117, "Toadette", "Mushroom");
    db->insert("critics", 3, 118, "Evil", "Wario");
    db->insert("critics", 3, 1100, "Goomba", "Fiend");
    db->insert("critics", 3, 1125, "Koopa", "Troopa");
    db->insert("critics", 3, 1124, "Shy", "Guy");
    db->insert("critics", 3, 1145, "Yoshi", "Kong");
    db->insert("critics", 3, 1180, "Bowser", "Dragon");
    db->insert("critics", 3, 15, "Baby", "Mario");
    db->insert("critics", 3, 1113, "Baby", "Luigi");
    db->insert("critics", 3, 1114, "Baby", "Zelda");
    db->insert("critics", 3, 1115, "Baby", "Daisy");
    db->insert("critics", 3, 1116, "Kirby", "Pink");
    db->insert("critics", 3, 1117, "Diddy", "Kong");
    db->insert("critics", 3, 1118, "Princess", "Daisy");
    db->insert("critics", 3, 2123, "Mario", "Bros");
    db->insert("critics", 3, 1223, "Mario", "Bros");
    db->insert("critics", 3, 1200, "Princess", "Peach");
    db->insert("critics", 3, 1225, "Luigi", "Bros");
    db->insert("critics", 3, 1224, "Toad", "Factory");
    db->insert("critics", 3, 1245, "Donkey", "Kong");
    db->insert("critics", 3, 1280, "Bowser", "Monster");
    db->insert("critics", 3, 52, "Boo", "Ghost");
    db->insert("critics", 3, 1213, "Princess1", "Daisy");
    db->insert("critics", 3, 1214, "Princess2", "Zelda");
    db->insert("critics", 3, 1215, "Princess3", "Daisy");
    db->insert("critics", 3, 1216, "Princess4", "Daisy");
    db->insert("critics", 3, 1217, "Princess5", "Daisy");
    db->insert("critics", 3, 1218, "Princess6", "Daisy");
    db->insert("critics", 3, 12100, "Princess", "Peach");
    db->insert("critics", 3, 12125, "Luigi", "Bros");
    db->insert("critics", 3, 12124, "Toad", "Factory");
    db->insert("critics", 3, 12145, "Donkey", "Kong");
    db->insert("critics", 3, 12180, "Bowser", "Monster");
    db->insert("critics", 3, 125, "Boo", "Ghost");
    db->insert("critics", 3, 12113, "Princess7", "Daisy");
    db->insert("critics", 3, 122114, "Princess", "Zelda");
    db->insert("critics", 3, 11125, "Princess8", "Daisy");
    db->insert("critics", 3, 12116, "Princess9", "Daisy");
    db->insert("critics", 3, 12117, "Princess10", "Daisy");
    db->insert("critics", 3, 12118, "Princess11", "Daisy");

    db->insert("movies", 3, 27, "Star Wars", 4.2);
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
    db->insert("movies", 3, 275, "Thor", 4.2);
    db->insert("movies", 3, 284, "Antman", 3.0);

    db->select("movies", 1, "title, rating");
    db->select("movies", 2, "id, title", "id = 27");
    db->update("movies", 3, "rating", "5.0", "id > 27");
    db->update("movies", 3, "rating", "4.0", "title = Frozen");
    db->select("movies", 1, "*");

    db->select("critics", 2, "fname, lname", "id = 115");
    db->update("critics", 3, "lname", "Rosy", "id = 115");

    // system dump
    db->printTable("critics");
    db->printTable("movies");
    db->printDBPrimary();
    db->printDBAttr();
    db->closeDB();
  }
  if (atoi(argv[1]) == 3) {
    cout << "____________________HASHED________________________" << endl;
    Database* h = new Database(HASHED);

    h->create_table("movies", "id", 6,
                    "id", "integer",
                    "title", "char(32)",
                    "rating", "real");
    h->printDBPrimary();
    h->insert("movies", 3, 27, "Star Wars", 4.2);
    h->insert("movies", 3, 28, "Rouge One", 3.0);
    h->insert("movies", 3, 25, "The Last Starfighter", 3.0);
    h->insert("movies", 3, 26, "Princess Diaries", 5.0);
    h->insert("movies", 3, 127, "Frozen", 4.2);
    h->insert("movies", 3, 228, "Big Hero 6", 3.0);
    h->insert("movies", 3, 237, "Cars", 4.2);
    h->insert("movies", 3, 248, "Cars 2", 3.0);
    h->insert("movies", 3, 257, "Toy Story", 4.2);
    h->insert("movies", 3, 268, "Tarzan", 3.0);
    h->insert("movies", 3, 277, "Little Shop of Horrors", 4.2);
    h->insert("movies", 3, 288, "Cinderella", 3.0);
    h->insert("movies", 3, 297, "Sleeping Beauty", 4.2);
    h->insert("movies", 3, 208, "Tangled", 3.0);
    h->insert("movies", 3, 217, "Honey I Shrunk the Kids", 4.2);
    h->insert("movies", 3, 281, "Back to the Future", 3.0);
    h->insert("movies", 3, 272, "Father of the Bride", 4.2);
    h->insert("movies", 3, 283, "Cheaper by the Dozen", 3.0);
    h->insert("movies", 3, 274, "Avengers", 4.2);
    h->insert("movies", 3, 285, "Joy Luck Club", 3.0);
    h->insert("movies", 3, 276, "Crazy Rich Asians", 4.2);
    h->insert("movies", 3, 287, "Solo", 3.0);
    h->insert("movies", 3, 278, "Captain America", 4.2);
    h->insert("movies", 3, 280, "Iron Man", 3.0);
    h->insert("movies", 3, 272, "Thor", 4.2);
    h->insert("movies", 3, 283, "Antman", 3.0);

    h->select("movies", 1, "*");
    h->update("movies", 3, "rating", "5.0", "id > 27");
    h->select("movies", 1, "*");
    h->select("movies", 2, "id, title", "id = 27");

    h->create_table("critics", "id", 6,
                    "id", "integer",
                    "fname", "varchar(32)",
                    "lname", "varchar(32)");

    h->insert("critics", 3, 27, "Mario", "Bros");
    h->insert("critics", 3, 27, "Mario", "Bros");
    h->insert("critics", 3, 28, "Princess", "Peach");
    h->insert("critics", 3, 25, "Luigi", "Bros");
    h->insert("critics", 3, 26, "Toad", "Mushroom");
    h->insert("critics", 3, 145, "Donkey", "Kong");
    h->insert("critics", 3, 180, "Bowser", "Monster");
    h->insert("critics", 3, 5, "Boo", "Ghost");
    h->insert("critics", 3, 113, "Princess", "Daisy");
    h->insert("critics", 3, 114, "Princess", "Zelda");
    h->insert("critics", 3, 115, "Princessy", "Rosalina");
    h->insert("critics", 3, 116, "Evil", "Waluigi");
    h->insert("critics", 3, 117, "Toadette", "Mushroom");
    h->insert("critics", 3, 118, "Evil", "Wario");
    h->insert("critics", 3, 1100, "Goomba", "Fiend");
    h->insert("critics", 3, 1125, "Koopa", "Troopa");
    h->insert("critics", 3, 1124, "Shy", "Guy");
    h->insert("critics", 3, 1145, "Yoshi", "Kong");
    h->insert("critics", 3, 1180, "Bowser", "Dragon");
    h->insert("critics", 3, 15, "Baby", "Mario");
    h->insert("critics", 3, 1113, "Baby", "Luigi");
    h->insert("critics", 3, 1114, "Baby", "Zelda");
    h->insert("critics", 3, 1115, "Baby", "Daisy");
    h->insert("critics", 3, 1116, "Kirby", "Pink");
    h->insert("critics", 3, 1117, "Diddy", "Kong");
    h->insert("critics", 3, 1118, "Princess", "Daisy");
    h->insert("critics", 3, 2123, "Mario", "Bros");
    h->insert("critics", 3, 1223, "Mario", "Bros");
    h->insert("critics", 3, 1200, "Princess", "Peach");
    h->insert("critics", 3, 1225, "Luigi", "Bros");
    h->insert("critics", 3, 1224, "Toad", "Factory");
    h->insert("critics", 3, 1245, "Donkey", "Kong");
    h->insert("critics", 3, 1280, "Bowser", "Monster");
    h->insert("critics", 3, 52, "Boo", "Ghost");
    h->insert("critics", 3, 1213, "Princess1", "Daisy");
    h->insert("critics", 3, 1214, "Princess2", "Zelda");
    h->insert("critics", 3, 1215, "Princess3", "Daisy");
    h->insert("critics", 3, 1216, "Princess4", "Daisy");
    h->insert("critics", 3, 1217, "Princess5", "Daisy");
    h->insert("critics", 3, 1218, "Princess6", "Daisy");
    h->insert("critics", 3, 12100, "Princess", "Peach");
    h->insert("critics", 3, 12125, "Luigi", "Bros");
    h->insert("critics", 3, 12124, "Toad", "Factory");
    h->insert("critics", 3, 12145, "Donkey", "Kong");
    h->insert("critics", 3, 12180, "Bowser", "Monster");
    h->insert("critics", 3, 125, "Boo", "Ghost");
    h->insert("critics", 3, 12113, "Princess7", "Daisy");
    h->insert("critics", 3, 122114, "Princess", "Zelda");
    h->insert("critics", 3, 11125, "Princess8", "Daisy");
    h->insert("critics", 3, 12116, "Princess9", "Daisy");
    h->insert("critics", 3, 12117, "Princess10", "Daisy");
    h->insert("critics", 3, 12118, "Princess11", "Daisy");

    h->select("critics", 1, "*");
    h->update("critics", 3, "lname", "Rosy", "id < 28");
    h->select("critics", 1, "*");
    h->select("critics", 2, "id, fname, lname", "id <= 1000");

    // system dump
    h->printTable("movies");
    h->printTable("critics");
    h->printDBPrimary();
    h->printDBAttr();
    h->closeDB();
  }

  // Figuring stuff out
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