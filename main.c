/*main.c*/

//
// Program to open a database and "execute" select queries 
// against this database. For now we just output the database
// schema, the AST for each query, and the first 5 lines
// of the references table.

#include <assert.h>  // assert
#include <ctype.h>   // isdigit
#include <math.h>    // fabs
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

#include "gtest/gtest.h"


//
// print_schema
//
// prints the database meta-data to stdout (console).
//
/*static void print_schema(struct Database* db)
{
  if (db == NULL) panic("db is NULL (print_schema)");

  printf("**DATABASE SCHEMA**\n");

  //
  // debugging: dump all the meta-data:
  //
  printf("Database: %s\n", db->name);

  for (int t = 0; t < db->numTables; t++)
  {
    printf("Table: %s\n", db->tables[t].name);
    printf("  Record size: %d\n", db->tables[t].recordSize);

    for (int c = 0; c < db->tables[t].numColumns; c++)
    {
      printf("  Column: %s, ", db->tables[t].columns[c].name);

      if (db->tables[t].columns[c].colType == COL_TYPE_INT)
        printf("int, ");
      else if (db->tables[t].columns[c].colType == COL_TYPE_REAL)
        printf("real, ");
      else
        printf("string, ");

      if (db->tables[t].columns[c].indexType == COL_NON_INDEXED)
        printf("non-indexed\n");
      else if (db->tables[t].columns[c].indexType == COL_INDEXED)
        printf("indexed\n");
      else
        printf("unique indexed\n");
    }
  }

  printf("**END OF DATABASE SCHEMA**\n");
}*/


//
// print_ast
//
// Prints the query AST to stdout (console).
//
/*static void print_ast(struct QUERY* query)
{
  if (query == NULL) panic("query is NULL (print_ast)");

  if (query->queryType != SELECT_QUERY)
  {
    printf("**INTERNAL ERROR: analyzer_print() only supports SELECT queries.\n");
    return;
  }

  // 
  // select query:
  // 
  struct SELECT* select = query->q.select;  // alias for less typing:

  printf("**QUERY AST**\n");

  printf("Table: %s\n", select->table);

  //
  // First we'll walk down the linked-list of columns
  // and output tablename.columnname, along with any
  // function that might be applied:
  //
  struct COLUMN* cur = select->columns;

  while (cur != NULL)
  {
    printf("Select column: ");

    if (cur->function != NO_FUNCTION)
	{
      print_function(cur->function);
      printf("(");
    }

    printf("%s.%s", cur->table, cur->name);

    if (cur->function != NO_FUNCTION)  // closing ) if a function
      printf(")");

    printf("\n");

    cur = cur->next;
  }

  //
  // is there a join clause?  if so, print...
  //
  if (select->join == NULL)
  {
    printf("Join (NULL)\n");
  }
  else
  {
    printf("Join %s On ", select->join->table);
    printf("%s.%s = ", select->join->left->table, select->join->left->name);
    printf("%s.%s\n", select->join->right->table, select->join->right->name);
  }//join

  //
  // is there a where clause?  If so, print...
  //
  if (select->where == NULL)
  {
    printf("Where (NULL)\n");
  }
  else
  {
    printf("Where %s.%s ", select->where->expr->column->table, select->where->expr->column->name);

    //
	// which operator?
	//
	assert(EXPR_LT == 0);
	
	char* operators[] = { "<", "<=", ">", ">=", "=", "<>", "like" };
	
	printf("%s", operators[select->where->expr->operator]);
	
    //
	// String literals we have to surround with ' or ", 
	// otherwise we just print the literal as is:
	//
    if (select->where->expr->litType == STRING_LITERAL)
    {
      char* cp = strchr(select->where->expr->value, '\'');  // contain '?

      if (cp == NULL)  // does not contain '
        printf(" '%s'\n", select->where->expr->value);
      else
        printf(" \"%s\"\n", select->where->expr->value);
    }
    else // int or real
    {
      printf(" %s\n", select->where->expr->value);
    }
  }//where

  //
  // is there an orderby clause? IF so, print...
  //
  if (select->orderby == NULL)
  {
    printf("Order By (NULL)\n");
  }
  else
  {
    printf("Order By ");

    //
	// there can be a function on the order by column:
	//
	if (select->orderby->column->function != NO_FUNCTION)
	{
      print_function(select->orderby->column->function);
      printf("(");
    }

    printf("%s.%s",
      select->orderby->column->table,
      select->orderby->column->name);

    if (select->orderby->column->function != NO_FUNCTION)  // closing )
      printf(")");

    if (select->orderby->ascending)
      printf(" ASC\n");
    else
      printf(" DESC\n");
  }//orderby

  //
  // limit clause? If so...
  //
  if (select->limit == NULL)
  {
    printf("Limit (NULL)\n");
  }
  else
  {
    printf("Limit %d\n", select->limit->N);
  }

  //
  // into clause?
  //
  if (select->into == NULL)
  {
    printf("Into (NULL)\n");
  }
  else
  {
    printf("Into %s\n", select->into->table);;
  }

  printf("**END OF QUERY AST**\n");
}*/
TEST(execute_query, basic_select) {
  //
  // Write our test query to a file, then open that
  // file for parsing:
  //
  FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select * From Movies2 limit 3;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");

  //
  // open the database the query needs, and then
  // parse, analyze, and execute the query:
  //
  struct Database *db = database_open("MovieLens2");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  struct ResultSet *rs = execute_query(db, query);
  ASSERT_TRUE(rs != NULL);

  ASSERT_TRUE(rs->numCols == 4);
  ASSERT_TRUE(rs->numRows == 3);

  struct RSColumn* column1 = rs->columns;
  
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_INT);
  
  struct RSColumn* column2=column1->next;
  ASSERT_TRUE(column2 != NULL);
  ASSERT_TRUE(column2->coltype == COL_TYPE_STRING);
  
  struct RSColumn* column3=column2->next;
  ASSERT_TRUE(column3 != NULL);
  ASSERT_TRUE(column3->coltype == COL_TYPE_INT);
  
  struct RSColumn* column4=column3->next;
  ASSERT_TRUE(column4 != NULL);
  ASSERT_TRUE(column4->coltype == COL_TYPE_REAL);

  ASSERT_TRUE(column4->next == NULL);
  //
  // check the data that should be in row 2:
  //
  ASSERT_TRUE(resultset_getInt(rs, 2, 1) == 111);
  ASSERT_TRUE(strcmp(resultset_getString(rs, 2, 2), "Scarface") == 0);
  ASSERT_TRUE(resultset_getInt(rs, 2, 3) == 1983);
  ASSERT_TRUE(fabs(resultset_getReal(rs, 2, 4) - 65884703.0) < 0.00001);

  //
  // TODO: add more checks to this test?
  //
}

TEST(execute_query, test02_LTE){
  FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select ID, Title From Movies2 where ID <= 260;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");
  
  struct Database *db = database_open("MovieLens2");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  struct ResultSet *rs = execute_query(db, query);
  
  ASSERT_TRUE(rs != NULL);

  ASSERT_TRUE(rs->numCols == 2);
  ASSERT_TRUE(rs->numRows == 2);

  struct RSColumn* column1 = rs->columns;
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_INT);

  for(int i = rs->numRows -1; i>=0;i--){
    int rs_int = resultset_getInt(rs, i + 1, 1);
    

    if(i == 1){
      ASSERT_TRUE(rs_int == 260);
    }
    if(i == 0){
      ASSERT_TRUE(rs_int == 111);
    }
    
    
  }

  
  struct RSColumn* column2=column1->next;
  ASSERT_TRUE(column2 != NULL);
  ASSERT_TRUE(column2->coltype == COL_TYPE_STRING);

  for(int i = rs->numRows -1; i>=0;i--){
    char *rs_str = resultset_getString(rs, i + 1, 2);
    
    if(i == 1){
      ASSERT_TRUE(strcmp(rs_str, "The 39 Steps") == 0);
    }
    if(i == 0){
       ASSERT_TRUE(strcmp(rs_str, "Scarface") == 0);;
    }
    free(rs_str);
    
  }
  
}
TEST(execute_query, test02_LT){
  FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select ID, Title From Movies2 where ID < 293;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");
  
  struct Database *db = database_open("MovieLens2");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  struct ResultSet *rs = execute_query(db, query);
  ASSERT_TRUE(rs != NULL);

  ASSERT_TRUE(rs->numCols == 2);
  ASSERT_TRUE(rs->numRows == 2);

  struct RSColumn* column1 = rs->columns;
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_INT);

  for(int i = rs->numRows -1; i>=0;i--){
    int rs_int = resultset_getInt(rs, i + 1, 1);
    

    if(i == 1){
      ASSERT_TRUE(rs_int == 260);
    }
    if(i == 0){
      ASSERT_TRUE(rs_int == 111);
    }
    
    
  }

  
  struct RSColumn* column2=column1->next;
  ASSERT_TRUE(column2 != NULL);
  ASSERT_TRUE(column2->coltype == COL_TYPE_STRING);

  for(int i = rs->numRows -1; i>=0;i--){
    char *rs_str = resultset_getString(rs, i + 1, 2);
    
    if(i == 1){
      ASSERT_TRUE(strcmp(rs_str, "The 39 Steps") == 0);
    }
    if(i == 0){
       ASSERT_TRUE(strcmp(rs_str, "Scarface") == 0);;
    }
    free(rs_str);
    
  }

  
}
TEST(execute_query, test03_min){
  //
  // Write our test query to a file, then open that
  // file for parsing:
  //
  FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select min(Date) From Ridership;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");

  
  //
  // open the database the query needs, and then
  // parse, analyze, and execute the query:
  //
  struct Database *db = database_open("CTA");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  
  struct ResultSet *rs = execute_query(db, query);
  
  ASSERT_TRUE(rs != NULL);

  
  ASSERT_TRUE(rs->numCols == 1);
  ASSERT_TRUE(rs->numRows == 1);

  
  struct RSColumn* column1 = rs->columns;
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_STRING);

  char *rs_str = resultset_getString(rs, 1, 1); 
  ASSERT_TRUE(strcmp(rs_str, "2017-12-01") == 0);
  
  
  free(rs_str);
    
}
TEST(execute_query, test04_max){
  //
  // Write our test query to a file, then open that
  // file for parsing:
  //
  FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select max(Date) From Ridership;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");

  
  //
  // open the database the query needs, and then
  // parse, analyze, and execute the query:
  //
  struct Database *db = database_open("CTA");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  
  struct ResultSet *rs = execute_query(db, query);
  
  ASSERT_TRUE(rs != NULL);

  
  ASSERT_TRUE(rs->numCols == 1);
  ASSERT_TRUE(rs->numRows == 1);

  
  struct RSColumn* column1 = rs->columns;
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_STRING);

  char *rs_str = resultset_getString(rs, 1, 1); 
  ASSERT_TRUE(strcmp(rs_str, "2018-11-30") == 0);
 
  
  free(rs_str);
    

  
}

TEST(execute_query, test05_avg){
   FILE *output = fopen("input.txt", "w");
  fprintf(output, "Select Avg(Revenue), Title from Movies2;\n$\n");
  fclose(output);
  FILE *input = fopen("input.txt", "r");
  
  struct Database *db = database_open("MovieLens2");
  parser_init();

  struct TokenQueue *tq = parser_parse(input);
  ASSERT_TRUE(tq != NULL);

  struct QUERY *query = analyzer_build(db, tq);
  ASSERT_TRUE(query != NULL);

  struct ResultSet *rs = execute_query(db, query);
  
  ASSERT_TRUE(rs != NULL);

  ASSERT_TRUE(rs->numCols == 2);
  ASSERT_TRUE(rs->numRows == 1);

  struct RSColumn* column1 = rs->columns;
  ASSERT_TRUE(column1 != NULL);
  ASSERT_TRUE(column1->coltype == COL_TYPE_REAL);

  double rs_real = resultset_getReal(rs, 1, 1);
  ASSERT_TRUE(fabs(rs_real - 75495851.222222) < 0.00001);

  
  struct RSColumn* column2=column1->next;
  ASSERT_TRUE(column2 != NULL);
  ASSERT_TRUE(column2->coltype == COL_TYPE_STRING);

  char *rs_str = resultset_getString(rs, 1, 2);
  ASSERT_TRUE(strcmp(rs_str, "The Thomas Crown Affair") == 0 );

  free(rs_str);
    
  
}
  

//
// main
//
// Prompt for database, open, and then input and
// execute SimpleSQL queries...
//
int main()
{
  struct Database* db = NULL;

  //
  // first we need the database name, and then let's 
  // try to open it:
  //
  char database[DATABASE_MAX_ID_LENGTH+1];  // +1 for null terminator

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
  //print_schema(db);

  //
  // now let's start parsing and analyzing each query:
  //
  parser_init();

  while (true)
  {
    printf("query? ");

    struct TokenQueue* tokens = NULL;

    //
	// first we check for syntax errors / EOF:
	//
    tokens = parser_parse(stdin);

    if (tokens == NULL)
    {
      //
      // EOF, or syntax error (error msg already output by parser if so):
      //
      if (parser_eof())
        break;
      else  // syntax error, loop around and try another query;
        continue;
    }
    else // successful parse:
    {
      //
      // analyze the query for semantic errors, and build AST if successful:
      //
      struct QUERY* query = NULL;

      query = analyzer_build(db, tokens);
	  
	  tokenqueue_destroy(tokens);  // done with the tokens, free memory:

      if (query == NULL)  // semantic error, msg already output
      {
        //
        // nothing to do, ignore and loop around and try again:
        //
        continue;
      }
      else
      {
        //
        // print AST and output first 5 lines of the table
        // given in the FROM clause of the query:
        //
        //print_ast(query);

        execute_query(db, query);
        analyzer_destroy(query);
      }
    }//else
  }//while

  //
  // done!
  //
  
  database_close(db);

  ::testing::InitGoogleTest();

  //
  // run all the tests, returns 0 if they
  // all pass and 1 if any fail:
  //
  int result = RUN_ALL_TESTS();

  return result; // 0 => all passed
}
