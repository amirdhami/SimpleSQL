/*main.c*/

//
// Project 02
//
// Amir Dhami
// Northwestern University
// CS 211, Winter 2023
//
// Contributing author: Prof. Joe Hummel
//

// #include files
#include <ctype.h>   // isspace, isdigit, isalpha
#include <stdbool.h> // true, false
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // stricmp
#include <strings.h>

#include "analyzer.h"
#include "ast.h"
#include "database.h"
#include "parser.h"
#include "util.h"
// your functions

// prints the first 5 lines of the relevant data file
void execute_query(char *db_name, struct TableMeta *tbl, char *tbl_name,
                   int db_len, struct QUERY *query) {

  // opens the file where the input table matches the actual table
  for (int i = 0; i < db_len; i++) {
    char *acc_tbl_name = tbl[i].name;
    // opens the relvant data file
    if (!strcasecmp(acc_tbl_name, tbl_name)) {

      char file_name[DATABASE_MAX_ID_LENGTH];
      snprintf(file_name, DATABASE_MAX_ID_LENGTH, "%s/%s.data", db_name,
               acc_tbl_name);

      FILE *input = NULL;

      input = fopen(file_name, "r");

      if (!input) {
        printf("**ERROR: unable to open file '%s' for input.\n", file_name);
        exit(-1);
      }

      int recordSize = tbl[i].recordSize + 3;

      // dynamically sized buffer
      char *buffer = (char *)malloc(sizeof(char) * recordSize);
      if (buffer == NULL) {
        panic("out of memory\n");
      }

      // prints out the first 5 lines
      for (int j = 0; j < 5; j++) {
        fgets(buffer, recordSize, input);
        printf("%s\n", buffer);
      }

      break;
    }
  }
  analyzer_destroy(query);
}

// prints out the AST
void print_ast(struct QUERY *query, char *db_name, struct TableMeta *tbl,
               int db_len) {
  printf("**QUERY AST**\n");
  struct SELECT *select = query->q.select;
  char *tbl_name = select->table;
  struct COLUMN *cur;
  cur = select->columns;

  printf("Table: %s\n", tbl_name);

  // iterates through SLL of columns
  while (cur) {
    char *col_name = cur->name;
    int func = cur->function;

    // prints out the select column with the relevant function
    if (func == -1) {
      printf("Select column: %s.%s\n", tbl_name, col_name);
    } else {
      char func_name[6];
      if (func == 0) {
        strcpy(func_name, "MIN");
      } else if (func == 1) {
        strcpy(func_name, "MAX");
      } else if (func == 2) {
        strcpy(func_name, "SUM");
      } else if (func == 3) {
        strcpy(func_name, "AVG");
      } else if (func == 4) {
        strcpy(func_name, "COUNT");
      }
      printf("Select column: %s(%s.%s)\n", func_name, tbl_name, col_name);
    }
    cur = cur->next;
  }

  // Prints the clauses
  struct JOIN *join = select->join;
  struct WHERE *where = select->where;
  struct ORDERBY *order_by = select->orderby;
  struct LIMIT *limit = select->limit;
  struct INTO *into = select->into;

  // JOIN Clause
  if (!join) {
    printf("Join (NULL)\n");
  } else {
    char *join_table = join->table;

    struct COLUMN *left = join->left;
    char *left_table = left->table;
    char *left_name = left->name;

    struct COLUMN *right = join->right;
    char *right_table = right->table;
    char *right_name = right->name;

    printf("Join %s On %s.%s = %s.%s\n", join_table, left_table, left_name,
           right_table, right_name);
  }

  // WHERE Clause
  if (!where) {
    printf("Where (NULL)\n");
  } else {
    struct EXPR *expr = where->expr;
    struct COLUMN *expr_col = expr->column;

    char *expr_table = expr_col->table;
    char *expr_name = expr_col->name;
    int op = expr->operator;
    int litType = expr->litType;
    char op_val[5];
    char *value = expr->value;

    // gets the relevant operator
    if (op == 0) {
      strcpy(op_val, "<");
    } else if (op == 1) {
      strcpy(op_val, "<=");
    } else if (op == 2) {
      strcpy(op_val, ">");
    } else if (op == 3) {
      strcpy(op_val, ">=");
    } else if (op == 4) {
      strcpy(op_val, "=");
    } else if (op == 5) {
      strcpy(op_val, "<>");
    } else if (op == 6) {
      strcpy(op_val, "like");
    }

    // checks if string literal already has single quotes in it
    if (litType == 2) {
      if (!strchr(value, '\'')) {
        printf("Where %s.%s %s '%s'\n", expr_table, expr_name, op_val, value);
      } else {
        printf("Where %s.%s %s \"%s\"\n", expr_table, expr_name, op_val, value);
      }
    }
    // otherwise, prints out int/real literal
    else {
      printf("Where %s.%s %s %s\n", expr_table, expr_name, op_val, value);
    }
  }

  // ORDER BY Clause
  if (!order_by) {
    printf("Order By (NULL)\n");
  } else {
    struct COLUMN *order_col = order_by->column;
    char *order_tbl = order_col->table;
    char *order_name = order_col->name;
    int order_func = order_col->function;
    bool asc = order_by->ascending;
    char up_down[5];

    if (asc) {
      strcpy(up_down, "ASC");
    } else {
      strcpy(up_down, "DESC");
    }

    // gets the relevant function
    if (order_func == -1) {
      printf("Order By %s.%s %s\n", order_tbl, order_name, up_down);
    } else {
      char func_name[6];
      if (order_func == 0) {
        strcpy(func_name, "MIN");
      } else if (order_func == 1) {
        strcpy(func_name, "MAX");
      } else if (order_func == 2) {
        strcpy(func_name, "SUM");
      } else if (order_func == 3) {
        strcpy(func_name, "AVG");
      } else if (order_func == 4) {
        strcpy(func_name, "COUNT");
      }
      printf("Order By %s(%s.%s) %s\n", func_name, order_tbl, order_name,
             up_down);
    }
  }

  // LIMIT Clause
  if (!limit) {
    printf("Limit (NULL)\n");
  } else {
    int n = limit->N;
    printf("Limit %d\n", n);
  }

  // INTO Clause
  if (!into) {
    printf("Into (NULL)\n");
  } else {
    char *into_table = into->table;
    printf("Into %s\n", into_table);
  }

  printf("**END OF QUERY AST**\n");

  execute_query(db_name, tbl, tbl_name, db_len, query);
}

// asks the user for an input query and parses it
void parse_db(struct Database *db, char *db_name, struct TableMeta *tbl,
              int db_len) {
  parser_init();

  // while the user input is valid and not EOF, builds a query and AST
  while (true) {
    printf("query? ");
    struct TokenQueue *tkn = parser_parse(stdin);

    if (tkn != NULL) {
      struct QUERY *query = analyzer_build(db, tkn);

      if (query != NULL) {
        print_ast(query, db_name, tbl, db_len);
      }

    } else {
      if (parser_eof()) {

        break;
      }
    }
  }
  database_close(db);
}

// prints the database schema
void print_schema(struct Database *db, char *db_name) {

  struct TableMeta *tbl = db->tables;
  int db_len = db->numTables;

  printf("**DATABASE SCHEMA**\n");
  printf("Database: %s\n", db_name);

  // iterates through tables to print information
  for (int i = 0; i < db_len; i++) {

    printf("Table: %s\n", tbl[i].name);
    printf("\tRecord size: %d\n", tbl[i].recordSize);

    struct ColumnMeta *cols = tbl[i].columns;
    int num_cols = tbl[i].numColumns;

    // iterates through each column in tables
    for (int j = 0; j < num_cols; j++) {
      char index_type[15];
      char col_type[7];

      int col_index = cols[j].indexType;
      int col_data_type = cols[j].colType;

      // determines the index type
      if (col_index == 0) {
        strcpy(index_type, "non-indexed");
      } else if (col_index == 1) {
        strcpy(index_type, "indexed");
      } else if (col_index == 2) {
        strcpy(index_type, "unique indexed");
      }

      // determines the data type
      if (col_data_type == 1) {
        strcpy(col_type, "int");
      } else if (col_data_type == 2) {
        strcpy(col_type, "real");
      } else if (col_data_type == 3) {
        strcpy(col_type, "string");
      }
      printf("\tColumn: %s, %s, %s\n", cols[j].name, col_type, index_type);
    }
  }
  printf("**END OF DATABASE SCHEMA**\n");

  parse_db(db, db_name, tbl, db_len);
}

// opens a user-inputted database
void open_database() {
  char db_name[32];

  printf("database? ");
  scanf("%s", db_name);

  struct Database *db = database_open(db_name);

  // if database is invalid, print error msg
  if (!db) {
    printf("**Error: unable to open database ‘%s’\n", db_name);
    exit(-1);
  } else {
    print_schema(db, db_name);
  }
}

int main() { open_database(); }
