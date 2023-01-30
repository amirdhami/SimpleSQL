/*main.c*/

//
// Program to open a database and "execute" select queries
// against this database. For now we only support select
// with where clauses, limit, and aggregate functions
// min, max, avg, sum and count.
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#include <assert.h>  // assert
#include <ctype.h>   // isdigit
#include <stdbool.h> // true, false
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcpy, strcat

#include "analyzer.h"
#include "ast.h"
#include "database.h"
#include "execute.h"
#include "parser.h"
#include "resultset.h"
#include "scanner.h"
#include "tokenqueue.h"
#include "util.h"

//
// main
//
// main function for the SimpleSQL execution environment.
// Inputs the database name, and then one or more queries.
// Each query must end with ; and execution ends when $
// is input by itself.
//
int main()
{
  struct Database *db = NULL;

  //
  // first we need the database name, and then let's
  // try to open it:
  //
  char database[DATABASE_MAX_ID_LENGTH + 1]; // +1 for null terminator

  printf("database? ");
  scanf("%s", database);

  db = database_open(database);

  //
  // Did the database open successfully?
  //
  if (db == NULL) // no
  {
    printf("**Error: unable to open database '%s'\n", database);
    exit(-1);
  }

  //
  // print the schema:
  //
  // print_schema(db);

  //
  // now let's start parsing and analyzing each query:
  //
  parser_init();

  while (true) {
    printf("query? ");

    struct TokenQueue *tokens = NULL;

    tokens = parser_parse(stdin);

    if (tokens == NULL) {
      //
      // EOF, or syntax error (error msg already output by parser if so):
      //
      if (parser_eof())
        break;
      else // syntax error, loop around and try another query;
        continue;
    } 
    else // successful parse:
    {
      //
      // analyze the query and build the AST:
      //
      struct QUERY *query = NULL;

      query = analyzer_build(db, tokens);

      tokenqueue_destroy(tokens); // we are done, free memory:

      if (query == NULL) // semantic error, msg already output
      {
        //
        // nothing to do, ignore and loop around and try again:
        //
        continue;
      } 
      else {
        //
        // print AST and output first 5 lines of the table
        // given in the FROM clause of the query:
        //
        // print_ast(query);

        //
        // TODO:
        //
        // execute_query(???;

        analyzer_destroy(query); // done with query, free memory
      }
    }// else
  }// while

  //
  // done!
  //
  database_close(db);

  return 0;
}
