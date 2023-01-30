/*resultset.h*/

//
// Project: Result Set for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include "ast.h"
#include "database.h"


//
// A ResultSet is the result of a query, which conceptually
// is a table of rows and columns. In terms of implementation,
// we build a linked-list of columns, where each column stores
// its data in a dynamically-allocated array (that grows and
// shrinks as necessary).
//
struct ResultSet
{
  struct RSColumn* columns;  // a linked-list of columns (forming a table)
  int              numRows;  // number of rows
  int              numCols;  // number of columns
};

//
// This is one column in the result set, which conceptually
// forms a column in a table. The column is actually a
// dynamically-allocated array that is reallocated when it
// fills.
//
struct RSColumn
{
  char* tableName;  // table name
  char* colName;    // column name
  int   function;   // enum AST_COLUMN_FUNCTIONS (ast.h)
  int   coltype;    // enum ColumnType (database.h)

  struct RSColumn* next;  // columns form a LL
  struct RSValue*  data;  // column data form an array
  int    N;               // # of data values in array
  int    cursor;          // index of "current" data value
  int    size;            // # of array locations (used + unused)
};

//
// This is one value in the result set / table. We have 3 
// types of values: int, real, or string.
//
struct RSValue
{
  union
  {
    int    i; 
    double r;
    char*  s;
  } value;

  int valueType;  // enum ColumnType (database.h)
};


//
// Functions:
//

//
// resultset_create
//
// Creates and returns a new, empty result set.
//
struct ResultSet* resultset_create(void);

//
// resultset_destroy
//
// Frees all the memory associated with the result set.
//
void resultset_destroy(struct ResultSet* rs);

//
// resultset_insertColumn
//
// Inserts a new column into the result set such that
// the new column ends up at the requested position.
// Positions are 1-based, so pass 1 if you want the new
// column to be first, and pass N+1 if you want to 
// append the new column to the end. You can obtain the
// # of columns via rs->numCols.
//
// You also pass the table and column names, e.g. "Movies"
// and "Title". You can pass NO_FUNCTION to start, and the
// type must be one of COL_TYPE_INT, COL_TYPE_REAL, or
// COL_TYPE_STRING.
//
// Returns the position where the column was inserted; this
// will be the same as the position value you passed in.
//
int  resultset_insertColumn(struct ResultSet* rs, 
  int position /*1..N+1*/, char* tableName, char* columnName,
  int function /*enum AST_COLUMN_FUNCTIONS*/,
  int colType /*enum ColumnType*/);

//
// resultset_findColumn
//
// Starting from startPos (which is 1-based), searches for the first
// column with the matching table and column name --- case-insensitive.
// Returns -1 if not found, otherwise returns position P where found 
// such that startPos <= P <= rs->numCols.
//
int  resultset_findColumn(struct ResultSet* rs, int startPos /*1..N*/, char* tableName, char* columnName);

//
// resultset_deleteColumn
//
// Deletes the column at position P, where 1 <= P <= rs->numCols.
//
void resultset_deleteColumn(struct ResultSet* rs, int position /*1..N*/);

//
// resultset_moveColumn
//
// Moves the column at position fromPos (1 <= fromPos <= rs->numCols)
// to position toPos (1 <= toPos <= rs->numCols+1).
//
void resultset_moveColumn(struct ResultSet* rs, int fromPos /*1..N*/, int toPos /*1..N+1*/);

//
// resultset_addRow
//
// Adds a new row to the end of each column; the values will
// be set to default values (0, 0.0, or ""). Returns the row
// # of this new row, 1-based.
//
int  resultset_addRow(struct ResultSet* rs);

//
// resultset_deleteRow
//
// Deletes the given row from each column, where 
// 1 <= rowNum <= rs->numRows;
//
void resultset_deleteRow(struct ResultSet* rs, int rowNum /*1..N*/);

//
// resultset_applyFunction
//
// Applies the given function --- one of enum AST_COLUMN_FUNCTIONS --- to
// the specified colNum (1 <= colNum <= rs->numCols). Howeer, do not pass 
// NO_FUNCTION, this will trigger an error message.
// 
// NOTE: when a function is applied, the # of rows in the result set will 
// drop to 1. The data in other columns is NOT deleted however, so that 
// additional functions can be applied. But when printing the result set, 
// at most one row will be printed.
//
void resultset_applyFunction(struct ResultSet* rs, int function /*enum AST_COLUMN_FUNCTIONS*/, int colNum /*1..N*/);

//
// resultset_putInt, putReal, putString
//
// These functions store a value into the given row and column of
// the result set; row and col are 1-based. When a string is stored,
// it is duplicated so that a copy is stored.
//
void resultset_putInt(struct ResultSet* rs, int row, int col, int value);
void resultset_putReal(struct ResultSet* rs, int row, int col, double value);
void resultset_putString(struct ResultSet* rs, int row, int col, char* value);

//
// resultset_getInt, getReal, getString
//
// These functions retrieve a value from the given row and column of
// the result set; row and col are 1-based. When a string is retrieved,
// it is duplicated so that a copy is returned --- this preserves the
// original value in the result set.
// 
// HOWEVER, this implies that when a string is returned, it is the 
// CALLER's responsibility to free the memory when they are done with
// the returned value.
//
int resultset_getInt(struct ResultSet* rs, int row, int col);
double resultset_getReal(struct ResultSet* rs, int row, int col);
char* resultset_getString(struct ResultSet* rs, int row, int col);

//
// resultset_print
//
// Prints the contents of the resultset to the console window.
//
void resultset_print(struct ResultSet* rs);
