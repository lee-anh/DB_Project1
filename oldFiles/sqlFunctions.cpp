#include "sqlFunctions.h"

void create_table(const char* table_name, const char* key, int length, ...) {
  va_list arg_list;
  va_start(arg_list, length);
  cout << "CREATE TABLE " << table_name << endl;

  void* ptr = db_attr_curr;

  bool variable = false;

  char* name = va_arg(arg_list, char*);
  char* typeStr = va_arg(arg_list, char*);
  dataType type = getDataType(typeStr);

  if (db_attr == NULL) {
    cout << "db_attr is NULL! " << endl;
  }

  // add to db_attributes
  // TODO: check for edge cases - what if the block doesn't have any more room?!
  // we're getting a seg fault... maybe something to do with the globals

  /*
    strncpy((char*)ptr, "hello", strlen("hello"));
    (char*&)ptr += strlen("hello");
    strncpy((char*)ptr, "world", strlen("world"));
    (char*&)ptr += strlen("world");
    */
  /*

    for (int i = 0; i < length / 2; i++) {
      char* name = va_arg(arg_list, char*);
      char* typeStr = va_arg(arg_list, char*);
      dataType type = getDataType(typeStr);

      // add to db_attributes
      // TODO: check for edge cases - what if the block doesn't have any more room?!
      // we're getting a seg fault. maybe checking for this will help

      *(dataType*)ptr = SMALLINT;
      cout << "dataType: " << *(dataType*)ptr << endl;
      ((dataType*&)ptr)++;

      *(dataType*)ptr = type;
      ((dataType*&)ptr)++;

      if (type == VARCHAR || type == CHAR) {
        if (type == VARCHAR)
          variable = true;

        int n = 1;  // getN(typeStr);
        *(int*)ptr = n;
        ((int*&)ptr)++;
      }
      db_attr_records++;


      // print to terminal
      printf(" %-20s %s, \n", name, typeStr);
    }
    */

  // *(bool*)db_attr_curr = variable;
  // ((bool*&)db_attr_curr)++;

  printf(" %-20s %s, \n", "PRIMARY KEY", key);

  // addToDBPrimary((char*)table_name, db_attr_curr_ptr);

  va_end(arg_list);
}

void insert(const char* table_name, int length, ...) {
}

void update(const char* table_name, int length, ...) {
}

void select(const char* table_name, int length, ...) {
}