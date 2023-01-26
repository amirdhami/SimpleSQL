/*database.h*/

//
// Project: Database schema for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

//
// meta-data like table and column names have a max size 
// of 31, so this value is plenty safe for reading any of
// the meta-data, building filenames, etc.
//
#define DATABASE_MAX_ID_LENGTH 31

struct Database
{
  char*  name;
  int    numTables;
  struct TableMeta* tables;  // pointer to ARRAY of table meta-data
};

struct TableMeta
{
  char*  name;
  int    recordSize;
  int    numColumns;
  struct ColumnMeta* columns;  // pointer to ARRAY of column meta-data
};

struct ColumnMeta
{
  char* name;
  int   colType;    // int, real or string
  int   indexType;  // none, indexed, unique+indexed (aka Primary Key)
};

enum ColumnType
{
  COL_TYPE_INT = 1,
  COL_TYPE_REAL,
  COL_TYPE_STRING
};

enum IndexType
{
  COL_NON_INDEXED = 0,
  COL_INDEXED,         // indexed, could have duplicates
  COL_UNIQUE_INDEXED   // indexed, no duplicates
};


//
// functions:
//

//
// database_open
//
// Given a database name, tries to open the underlying database
// meta files and retrive the meta-data (i.e. database schema).
//
// Returns NULL if the database does not exist. Returns a pointer
// to a data structure if the database was successfully opened
// and the schema retrieved.
//
// NOTE: it is the callers responsibility to free the resources
// used by the data structure by calling database_close().
//
struct Database* database_open(char* database);

//
// database_close
//
// Frees the memory associated with the query; call this
// when you are done with the data structure.
//
void database_close(struct Database* db);
