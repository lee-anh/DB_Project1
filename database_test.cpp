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
  Database* db1 = new Database(ORDERED);
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

  // check the db attributes as well?

  free(movies);
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

TEST_F(DatabaseTest, InsertIntoTableUnorderedFixed) {
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

TEST_F(DatabaseTest, noDuplicatePrimaryKeyUnorderedFixed) {
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

TEST_F(DatabaseTest, noDuplicatePrimaryKeyUnorderedFixedLoadTest) {
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

}  // namespace