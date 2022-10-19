#include "Database.h"

#include <gtest/gtest.h>

namespace {
// To use a test fixture, derive a class from testing::Test.

class DatabaseTest : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the variables.
  // Otherwise, this can be skipped.
  void SetUp() override {
  }

  // Declares the variables your tests want to use.
  Database* db = new Database(UNORDERED);  // doesn't matter if it's UNORDERED, ORDERED, OR HASHED here
  void createSomeMovies() {
    db->create_table("movies", "id", 6,
                     "id", "integer",
                     "title", "char(32)",
                     "rating", "real");

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
    db->insert("movies", 3, 272, "Thor", 4.2);
    db->insert("movies", 3, 283, "Antman", 3.0);
  };

  void createSomeCritics() {
    db->create_table("critics", "id", 6,
                     "id", "integer",
                     "fname", "varchar(32)",
                     "lname", "varchar(32)");

    db->insert("critics", 3, 123, "Mario", "Bros");
    db->insert("critics", 3, 100, "Princess", "Peach");
    db->insert("critics", 3, 125, "Luigi", "Bros");
    db->insert("critics", 3, 124, "Toad", "Mushroom");
    db->insert("critics", 3, 145, "Donkey", "Kong");
    db->insert("critics", 3, 180, "Bowser", "Monster");
    db->insert("critics", 3, 5, "Boo", "Ghost");
    db->insert("critics", 3, 113, "Princess", "Daisy");
    db->insert("critics", 3, 114, "Princess", "Zelda");
    db->insert("critics", 3, 115, "Princess", "Rosalina");
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
  }
};

class DatabaseTestUnordered : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the variables.
  // Otherwise, this can be skipped.
  void SetUp() override {
  }

  // Declares the variables your tests want to use.
  Database* db = new Database(UNORDERED);
};

class DatabaseTestOrdered : public testing::Test {
 protected:  // You should make the members protected s.t. they can be
             // accessed from sub-classes.
  // virtual void SetUp() will be called before each test is run.  You
  // should define it if you need to initialize the variables.
  // Otherwise, this can be skipped.
  void SetUp() override {
  }

  // Declares the variables your tests want to use.
  Database* db = new Database(ORDERED);
};

// create_table
// initializeNewBlock wis included
TEST_F(DatabaseTest, CreateTable) {
  // You can access data in the test fixture here.
  // Database* db1 = new Database(UNORDERED);
  EXPECT_EQ(db->db_primary_records, 0);
  EXPECT_EQ(db->db_primary_blocks, 1);

  EXPECT_EQ(db->db_attr_records, 0);
  EXPECT_EQ(db->db_attr_blocks, 1);

  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  // a primary record was created
  EXPECT_EQ(db->db_primary_records, 1);
  EXPECT_EQ(db->db_primary_blocks, 1);

  // attribute records were inserted correctly
  EXPECT_EQ(db->db_attr_records, 3);
  EXPECT_EQ(db->db_attr_blocks, 1);
}

TEST_F(DatabaseTest, RetrieveDBPrimaryRecordOnly) {
  // You can access data in the test fixture here.
  //  Database* db1 = new Database(UNORDERED);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);
  ASSERT_EQ(record, nullptr);
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  record = db->retrieveDBPrimaryRecord(movies);

  // make sure that the table details were set correctly in db primary
  ASSERT_NE(record, nullptr);
  EXPECT_STREQ((char*)(record), "movies");
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->db_primary_num_db_attr_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->primary_key_offset), 0);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_root_offset), nullptr);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_curr_offset), nullptr);
  EXPECT_EQ((void*)*(long*)((uintptr_t)(record) + db->data_curr_end_offset), (void*)((*(long*)((uintptr_t)(record) + db->data_curr_offset)) + db->BLOCK_SIZE - sizeof(long)));
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 0);

  free(movies);
}
TEST_F(DatabaseTest, CheckGetPrimaryKeyType) {
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);
  void* dbAttrRecord = (void*)*(long*)((uintptr_t)record + db->db_primary_db_attr_offset);
  int numAttr = *(int*)((uintptr_t)record + db->db_primary_num_db_attr_offset);
  int primaryKeyNumber = *(int*)((uintptr_t)record + db->primary_key_offset);
  EXPECT_EQ(db->getPrimaryKeyType(dbAttrRecord, numAttr, primaryKeyNumber), INTEGER);
}

TEST_F(DatabaseTest, RetrieveDBPrimaryRecordFirst) {
  // You can access data in the test fixture here.
  //  Database* db1 = new Database(UNORDERED);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);
  ASSERT_EQ(record, nullptr);
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");
  db->create_table("astars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");
  db->create_table("bstars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");

  record = db->retrieveDBPrimaryRecord(movies);

  // make sure that the table details were set correctly in db primary
  ASSERT_NE(record, nullptr);
  EXPECT_STREQ((char*)(record), "movies");
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->db_primary_num_db_attr_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->primary_key_offset), 0);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_root_offset), nullptr);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_curr_offset), nullptr);
  EXPECT_EQ((void*)*(long*)((uintptr_t)(record) + db->data_curr_end_offset), (void*)((*(long*)((uintptr_t)(record) + db->data_curr_offset)) + db->BLOCK_SIZE - sizeof(long)));
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 0);

  // check the db attributes as well?

  free(movies);
}

TEST_F(DatabaseTest, RetrieveDBPrimaryRecordMiddle) {
  // You can access data in the test fixture here.
  //  Database* db1 = new Database(UNORDERED);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);
  ASSERT_EQ(record, nullptr);
  db->create_table("astars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");
  db->create_table("bstars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");

  record = db->retrieveDBPrimaryRecord(movies);

  // make sure that the table details were set correctly in db primary
  ASSERT_NE(record, nullptr);
  EXPECT_STREQ((char*)(record), "movies");
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->db_primary_num_db_attr_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->primary_key_offset), 0);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_root_offset), nullptr);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_curr_offset), nullptr);
  EXPECT_EQ((void*)*(long*)((uintptr_t)(record) + db->data_curr_end_offset), (void*)((*(long*)((uintptr_t)(record) + db->data_curr_offset)) + db->BLOCK_SIZE - sizeof(long)));
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 0);

  // check the db attributes as well?

  free(movies);
}

TEST_F(DatabaseTest, RetrieveDBPrimaryRecordEnd) {
  // You can access data in the test fixture here.
  //  Database* db1 = new Database(UNORDERED);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);
  ASSERT_EQ(record, nullptr);
  db->create_table("astars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");
  db->create_table("bstars", "id", 6,
                   "id", "integer",
                   "fname", "char(32)",
                   "lname", "char(32)");
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  record = db->retrieveDBPrimaryRecord(movies);

  // make sure that the table details were set correctly in db primary
  ASSERT_NE(record, nullptr);
  EXPECT_STREQ((char*)(record), "movies");
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->db_primary_num_db_attr_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)(record) + db->primary_key_offset), 0);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_root_offset), nullptr);
  EXPECT_NE((void*)*(long*)((uintptr_t)(record) + db->data_curr_offset), nullptr);
  EXPECT_EQ((void*)*(long*)((uintptr_t)(record) + db->data_curr_end_offset), (void*)((*(long*)((uintptr_t)(record) + db->data_curr_offset)) + db->BLOCK_SIZE - sizeof(long)));
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 0);

  // check the db attributes as well?

  free(movies);
}
TEST_F(DatabaseTest, NoDuplicateTableNames) {
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");
  db->create_table("movies", "id", 8,
                   "id", "smallint",
                   "title", "char(32)",
                   "rating", "real",
                   "revenue", "integer");

  EXPECT_EQ(db->db_primary_records, 1);  // only one record should have been created
}

TEST_F(DatabaseTest, NoDuplicateTableAttributes) {
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "title", "real");

  EXPECT_EQ(db->db_primary_records, 0);  // only one record should have been created
}

TEST_F(DatabaseTest, LoadTestForDBPrimaryAndDBAttributes) {
  // indirectly tests checkSpaceAdd()
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");
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

  // number of primary records and blocks created
  EXPECT_EQ(db->db_primary_records, 15);
  EXPECT_EQ(db->db_primary_blocks, 4);

  // number of db attr records and blocks created
  EXPECT_EQ(db->db_attr_records, 45);
  EXPECT_EQ(db->db_attr_blocks, 7);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyFixedInteger) {
  // indirectly tests findPrimaryKeyFixed
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->insert("movies", 3, 28, "Rouge One", 3.0);
  db->insert("movies", 3, 25, "The Last Starfighter", 3.0);
  db->insert("movies", 3, 26, "Princess Diaries", 5.0);
  db->insert("movies", 3, 127, "Frozen", 4.2);
  db->insert("movies", 3, 27, "Star Wars", 4.2);     // duplicate primary key
  db->insert("movies", 3, 27, "Say Anything", 4.2);  // duplicate primary key

  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 5);
}
TEST_F(DatabaseTest, NoDuplicatePrimaryKeyFixedSmallInt) {
  db->create_table("movies", "id", 6,
                   "id", "smallint",
                   "title", "char(32)",
                   "rating", "real");
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyFixedFloat) {
  db->create_table("movies", "rating", 6,
                   "id", "smallint",
                   "title", "char(32)",
                   "rating", "real");
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyFixedChar) {
  db->create_table("movies", "title", 6,
                   "id", "smallint",
                   "title", "char(32)",
                   "rating", "real");
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->insert("movies", 3, 27, "Star Wars", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyVariableInteger) {
  // indirectly tests findPrimaryKeyVariable
  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");

  db->insert("critics", 3, 123, "Mario", "Bros");
  db->insert("critics", 3, 123, "Mario", "Bros");  // duplicate primary key

  char* critics = (char*)calloc(strlen("critics") + 1, sizeof(char));
  strcpy(critics, "critics");
  void* record = db->retrieveDBPrimaryRecord(critics);
  free(critics);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyVariableSmallInt) {
  db->create_table("movies", "id", 8,
                   "id", "smallint",
                   "title", "varchar(32)",
                   "director", "char(32)",
                   "rating", "real");
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyVariableFloat) {
  db->create_table("movies", "rating", 8,
                   "id", "smallint",
                   "director", "char(32)",
                   "rating", "real");
  db->insert("movies", 4, 27, "George Lucas", 4.2);
  db->insert("movies", 4, 27, "George Lucas", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyVariableChar) {
  db->create_table("movies", "director", 8,
                   "id", "smallint",
                   "title", "varchar(32)",
                   "director", "char(32)",
                   "rating", "real");
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, NoDuplicatePrimaryKeyVariableVarChar) {
  db->create_table("movies", "title", 8,
                   "id", "smallint",
                   "title", "varchar(32)",
                   "director", "char(32)",
                   "rating", "real");
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  db->insert("movies", 4, 27, "Star Wars", "George Lucas", 4.2);
  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 1);
}

TEST_F(DatabaseTest, UpdateFixedEqual) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id = 27"), 1);
}
TEST_F(DatabaseTest, UpdateFixedNotEqual) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id != 27"), 25);
}
TEST_F(DatabaseTest, UpdateFixedGreaterThan) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id > 27"), 23);
}
TEST_F(DatabaseTest, UpdateFixedGreaterThanOrEqualTo) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id >= 27"), 24);
}
TEST_F(DatabaseTest, UpdateFixedLessThan) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id < 27"), 2);
}
TEST_F(DatabaseTest, UpdateFixedLessThanOrEqualTo) {
  createSomeMovies();
  EXPECT_EQ(db->updateTest("movies", 3, "rating", "5.0", "id <= 27"), 3);
}

TEST_F(DatabaseTest, UpdateVariableEqual) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id = 115"), 1);
}
TEST_F(DatabaseTest, UpdateVariableNotEqual) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id != 115"), 49);
}
TEST_F(DatabaseTest, UpdateVariableGreaterThan) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id > 115"), 43);
}
TEST_F(DatabaseTest, UpdateVariableGreaterThanOrEqualTo) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id >= 115"), 44);
}
TEST_F(DatabaseTest, UpdateVariableLessThan) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id < 115"), 6);
}
TEST_F(DatabaseTest, UpdateVariableLessThanOrEqualTo) {
  createSomeCritics();
  EXPECT_EQ(db->updateTest("critics", 3, "lname", "Rosy", "id <= 115"), 7);
}
TEST_F(DatabaseTestUnordered, InsertIntoTableFixed) {
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  db->insert("movies", 3, 27, "Star Wars", 4.2);
  db->insert("movies", 3, 28, "Rouge One", 3.0);
  db->insert("movies", 3, 25, "The Last Starfighter", 3.0);
  db->insert("movies", 3, 26, "Princess Diaries", 5.0);
  db->insert("movies", 3, 127, "Frozen", 4.2);

  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 5);

  free(movies);
}

TEST_F(DatabaseTestUnordered, InsertIntoTableFixedLoadTest) {
  // indirectly tests findPrimaryKeyFixed
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

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
  db->insert("movies", 3, 272, "Thor", 4.2);
  db->insert("movies", 3, 283, "Antman", 3.0);

  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 26);
}

TEST_F(DatabaseTestUnordered, InsertIntoTableVariable) {
  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");

  db->insert("critics", 3, 100, "Princess", "Peach");
  db->insert("critics", 3, 125, "Luigi", "Bros");
  db->insert("critics", 3, 124, "Toad", "Factory");
  db->insert("critics", 3, 145, "Donkey", "Kong");
  db->insert("critics", 3, 180, "Bowser", "Monster");

  char* critics = (char*)calloc(strlen("critics") + 1, sizeof(char));
  strcpy(critics, "critics");
  void* record = db->retrieveDBPrimaryRecord(critics);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 5);

  free(critics);
}

TEST_F(DatabaseTestUnordered, InsertIntoTableVariableLoadTest) {
  // indirectly tests findPrimaryKeyFixed
  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");

  db->insert("critics", 3, 123, "Mario", "Bros");
  db->insert("critics", 3, 100, "Princess", "Peach");
  db->insert("critics", 3, 125, "Luigi", "Bros");
  db->insert("critics", 3, 124, "Toad", "Mushroom");
  db->insert("critics", 3, 145, "Donkey", "Kong");
  db->insert("critics", 3, 180, "Bowser", "Monster");
  db->insert("critics", 3, 5, "Boo", "Ghost");
  db->insert("critics", 3, 113, "Princess", "Daisy");
  db->insert("critics", 3, 114, "Princess", "Zelda");
  db->insert("critics", 3, 115, "Princess", "Rosalina");
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

  char* critics = (char*)calloc(strlen("critics") + 1, sizeof(char));
  strcpy(critics, "critics");
  void* record = db->retrieveDBPrimaryRecord(critics);
  free(critics);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 2);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 50);
}

TEST_F(DatabaseTestOrdered, InsertIntoTableFixed) {
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

  db->insert("movies", 3, 27, "Star Wars", 4.2);             // insert to empty
  db->insert("movies", 3, 28, "Rouge One", 3.0);             // insert to end
  db->insert("movies", 3, 25, "The Last Starfighter", 3.0);  // insert to front
  db->insert("movies", 3, 26, "Princess Diaries", 5.0);      // insert to middle
  db->insert("movies", 3, 127, "Frozen", 4.2);               // insert to end

  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 5);

  // we have to make sure that the primary key order is correct
  // (indirectly tests findPrimaryKeyFixed as well)
  // what's the easiest way?

  void* dataRead = (void*)*(long*)((uintptr_t)record + db->data_root_offset);
  void* dataEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)db->BLOCK_SIZE)) - 1);

  int size = db->calculateMaxDataRecordSize((void*)*(long*)((uintptr_t)record + db->db_primary_db_attr_offset), 3);
  EXPECT_EQ(size, 40);
  int prevPk = -1;

  for (int i = 0; i < 5; i++) {
    db->checkSpaceSearch(size, dataRead, dataEnd);

    int currPk = *(int*)dataRead;  // we know for a fact that in this test case the first element is going to be the primary key

    EXPECT_LT(prevPk, currPk);
    prevPk = currPk;
    dataRead = (void*)((uintptr_t)dataRead + size);
  }

  free(movies);
}

TEST_F(DatabaseTestOrdered, InsertIntoTableFixedLoadTest) {
  // indirectly tests findPrimaryKeyFixed
  db->create_table("movies", "id", 6,
                   "id", "integer",
                   "title", "char(32)",
                   "rating", "real");

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
  db->insert("movies", 3, 572, "Thor", 4.2);
  db->insert("movies", 3, 583, "Antman", 3.0);

  char* movies = (char*)calloc(strlen("movies") + 1, sizeof(char));
  strcpy(movies, "movies");
  void* record = db->retrieveDBPrimaryRecord(movies);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 3);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 26);

  void* dataRead = (void*)*(long*)((uintptr_t)record + db->data_root_offset);
  void* dataEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)db->BLOCK_SIZE)) - 1);

  int size = db->calculateMaxDataRecordSize((void*)*(long*)((uintptr_t)record + db->db_primary_db_attr_offset), 3);
  EXPECT_EQ(size, 40);
  int prevPk = -1;

  for (int i = 0; i < 26; i++) {
    db->checkSpaceSearch(size, dataRead, dataEnd);

    // sad this is causing a seg fault
    cout << "dataRead " << dataRead;
    int currPk = *(int*)dataRead;  // we know for a fact that in this test case the first element is going to be the primary key

    EXPECT_LT(prevPk, currPk);
    prevPk = currPk;
    dataRead = (void*)((uintptr_t)dataRead + size);
  }

  free(movies);
}

TEST_F(DatabaseTestOrdered, InsertIntoTableVariable) {
  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");

  db->insert("critics", 3, 100, "Princess", "Peach");
  db->insert("critics", 3, 125, "Luigi", "Bros");
  db->insert("critics", 3, 124, "Toad", "Factory");
  db->insert("critics", 3, 145, "Donkey", "Kong");
  db->insert("critics", 3, 180, "Bowser", "Monster");

  char* critics = (char*)calloc(strlen("critics") + 1, sizeof(char));
  strcpy(critics, "critics");
  void* record = db->retrieveDBPrimaryRecord(critics);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 1);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 5);

  void* dataRead = (void*)*(long*)((uintptr_t)record + db->data_root_offset);
  void* dataEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)db->BLOCK_SIZE)) - 1);

  int size = db->calculateMaxDataRecordSize((void*)*(long*)((uintptr_t)record + db->db_primary_db_attr_offset), 3);
  EXPECT_EQ(size, 69);
  int prevPk = -1;

  for (int i = 0; i < 5; i++) {
    db->checkSpaceSearch(-1, dataRead, dataEnd);

    cout << "dataRead " << dataRead;
    int currPk = *(int*)dataRead;  // we know for a fact that in this test case the first element is going to be the primary key

    EXPECT_LT(prevPk, currPk);
    prevPk = currPk;

    // go to the next record
    if (i != 5 - 1) {
      for (int j = 0; j < size; j++) {
        // cout << "*currReadPtr " << *currReadPtr << endl;
        if (*(char*)dataRead == '~') {
          ((char*&)dataRead)++;
          break;
        }
        ((char*&)dataRead)++;
      }
    }
  }

  free(critics);
}

TEST_F(DatabaseTestOrdered, InsertIntoTableVariableLoadTest) {
  db->create_table("critics", "id", 6,
                   "id", "integer",
                   "fname", "varchar(32)",
                   "lname", "varchar(32)");

  db->insert("critics", 3, 123, "Mario", "Bros");
  db->insert("critics", 3, 100, "Princess", "Peach");
  db->insert("critics", 3, 125, "Luigi", "Bros");
  db->insert("critics", 3, 124, "Toad", "Mushroom");
  db->insert("critics", 3, 145, "Donkey", "Kong");
  db->insert("critics", 3, 180, "Bowser", "Monster");
  db->insert("critics", 3, 5, "Boo", "Ghost");
  db->insert("critics", 3, 113, "Princess", "Daisy");
  db->insert("critics", 3, 114, "Princess", "Zelda");
  db->insert("critics", 3, 115, "Princess", "Rosalina");
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

  char* critics = (char*)calloc(strlen("critics") + 1, sizeof(char));
  strcpy(critics, "critics");
  void* record = db->retrieveDBPrimaryRecord(critics);

  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_block_count_offset), 2);
  EXPECT_EQ(*(int*)((uintptr_t)record + db->data_record_count_offset), 50);

  void* dataRead = (void*)*(long*)((uintptr_t)record + db->data_root_offset);
  void* dataEnd = (void*)(((long*)((uintptr_t)dataRead + (uintptr_t)db->BLOCK_SIZE)) - 1);

  int size = db->calculateMaxDataRecordSize((void*)*(long*)((uintptr_t)record + db->db_primary_db_attr_offset), 3);
  EXPECT_EQ(size, 69);
  int prevPk = -1;

  for (int i = 0; i < 50; i++) {
    db->checkSpaceSearch(-1, dataRead, dataEnd);
    int currPk = *(int*)dataRead;  // we know for a fact that in this test case the first element is going to be the primary key

    EXPECT_LT(prevPk, currPk);
    prevPk = currPk;

    // go to the next record
    if (i != 50 - 1) {
      for (int j = 0; j < size; j++) {
        if (*(char*)dataRead == '~') {
          ((char*&)dataRead)++;
          break;
        }
        ((char*&)dataRead)++;
      }
    }
  }

  free(critics);
}
// TODO:
// error inserts where the attributes don't match? what kind of error checking do we have in place?
// more tests focusing on the primary keys
// primary key based on each data type
// ordered will have more tests since we'll have to pay attention to the different
// select/update - test out all of the conditionals
}  // namespace
