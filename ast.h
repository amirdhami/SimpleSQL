/*ast.h*/

//
// Project: AST for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include <stdbool.h>     // true, false
#include "tokenqueue.h"

//
// SQL Query
//
enum AST_QUERY_TYPES
{
  SELECT_QUERY = 0,
  INSERT_QUERY,
  UPDATE_QUERY,
  DELETE_QUERY
};

struct QUERY
{
  union
  {
    struct SELECT* select;
    struct INSERT* insert;
    struct UPDATE* update;
    struct DELETE* delete;
  } q;

  int queryType;  // enum AST_QUERY_TYPES
};


//
// SELECT
//
struct SELECT
{
  char* table;             // table name
  struct COLUMN* columns;  // Linked-list of 1 or more columns

  struct JOIN* join;       // OPTIONAL join clause
  struct WHERE* where;     // OPTIONAL where clause
  struct ORDERBY* orderby; // OPTIONAL order by clause
  struct LIMIT* limit;     // OPTIONAL limit clause
  struct INTO* into;       // OPTIONAL into clause
};

enum AST_COLUMN_FUNCTIONS
{
  NO_FUNCTION = -1,
  MIN_FUNCTION,
  MAX_FUNCTION,
  SUM_FUNCTION,
  AVG_FUNCTION,
  COUNT_FUNCTION
};

struct COLUMN
{
  char* table;         // table name
  char* name;          // column in that table

  int function;        // OPTIONAL: function to apply (enum AST_COLUMN_FUNCTIONS)
  struct COLUMN* next; // OPTIONAL: if part of a list
};

struct JOIN
{
  char* table;
  struct COLUMN* left;
  struct COLUMN* right;
};

struct WHERE
{
  struct EXPR* expr;
};

struct ORDERBY
{
  struct COLUMN* column;
  bool   ascending;  // true => ascending, false => descending
};

struct LIMIT
{
  int N;
};

struct INTO
{
  char* table;
};

enum AST_EXPR_OPERATORS
{
  EXPR_LT = 0,
  EXPR_LTE,
  EXPR_GT,
  EXPR_GTE,
  EXPR_EQUAL,
  EXPR_NOT_EQUAL,
  EXPR_LIKE
};

enum AST_LITERAL_TYPES
{
  INTEGER_LITERAL,
  REAL_LITERAL,
  STRING_LITERAL
};

struct EXPR
{
  struct COLUMN* column;
  int operator;  // enum AST_EXPR_OPERATORS
  int litType;   // enum AST_LITERAL_TYPES
  char* value;   // literal in string form, e.g. "123" or "The Matrix"
};


//
// Action queries: not yet supported
//
struct INSERT
{
  char* table;
};

struct UPDATE
{
  char* table;
};

struct DELETE
{
  char* table;
};
