/*execute.c*/

//
// Project: Execution of queries for SimpleSQL
//
// Amir Dhami
// Northwestern University
// CS 211, Winter 2023
//

#include <assert.h>  // assert
#include <ctype.h>   // isdigit
#include <stdbool.h> // true, false
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strcpy, strcat
#include <strings.h>

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
// #include any other of our ".h" files?
//

//
// implementation of function(s), both private and public
//

//
// print_function
//
// Given an enum value, prints the function name.
//
/*static void print_function(int function) {
  switch (function) {
  case NO_FUNCTION:

    break;

  case MIN_FUNCTION:
    printf("MIN");
    break;

  case MAX_FUNCTION:
    printf("MAX");
    break;

  case SUM_FUNCTION:
    printf("SUM");
    break;

  case AVG_FUNCTION:
    printf("AVG");
    break;

  case COUNT_FUNCTION:
    printf("COUNT");
    break;

  default:
    panic("unknown function in print_function");
  }
}*/

//
// execute_query
//
// execute a select query, which for now means open the underlying
// .data file and output the first 5 lines.
//
void execute_query(struct Database *db, struct QUERY *query) {
  if (db == NULL)
    panic("db is NULL (execute)");
  if (query == NULL)
    panic("query is NULL (execute)");

  if (query->queryType != SELECT_QUERY) {
    printf("**INTERNAL ERROR: execute() only supports SELECT queries.\n");
    return;
  }

  struct SELECT *select = query->q.select; // alias for less typing:

  //
  // the query has been analyzed and so we know it's correct: the
  // database exists, the table(s) exist, the column(s) exist, etc.
  //

  //
  // (1) we need a pointer to the table meta data, so find it:
  //
  struct TableMeta *tablemeta = NULL;

  for (int t = 0; t < db->numTables; t++) {
    if (icmpStrings(db->tables[t].name, select->table) == 0) // found it:
    {
      tablemeta = &db->tables[t];
      break;
    }
  }

  assert(tablemeta != NULL);

  // (2) create a resultset from the table's cols
  struct ResultSet *rs = resultset_create();
  struct RSColumn *rs_col = rs->columns;
  char *table_name = tablemeta->name;
  int num_cols = tablemeta->numColumns;

  struct ColumnMeta *col_meta = tablemeta->columns;
  for (int i = 0; i < num_cols; i++) {
    char *col_name = col_meta[i].name;
    int colType = col_meta[i].colType;

    resultset_insertColumn(rs, i + 1, table_name, col_name, NO_FUNCTION,
                           colType);
  }

  //
  // (3) open the table's data file
  //
  // the table exists within a sub-directory under the executable
  // where the directory has the same name as the database, and with
  // a "TABLE-NAME.data" filename within that sub-directory:
  //
  char path[(2 * DATABASE_MAX_ID_LENGTH) + 10];

  strcpy(path, db->name); // name/name.data
  strcat(path, "/");
  strcat(path, tablemeta->name);
  strcat(path, ".data");

  FILE *datafile = fopen(path, "r");
  if (datafile == NULL) // unable to open:
  {
    printf("**INTERNAL ERROR: table's data file '%s' not found.\n", path);
    exit(-1);
  }

  //
  // (3) allocate a buffer for input, and start reading the data:
  //
  int dataBufferSize =
      tablemeta->recordSize + 3; // ends with $\n + null terminator
  char *dataBuffer = (char *)malloc(sizeof(char) * dataBufferSize);
  if (dataBuffer == NULL)
    panic("out of memory");

  int rowCount = 1;
  while (true) {
    fgets(dataBuffer, dataBufferSize, datafile);

    if (feof(datafile)) // end of the data file, we're done
      break;

    char quoteType;
    int quoteCount = 0;
    int data_length = strlen(dataBuffer);

    // adds null string character at beg/end of quotes or at empty spaces
    char *cp = dataBuffer;
    while (*cp != '\0') {
      char c = *cp;
      if (c == '\'' || c == '"') {
        quoteType = c;
        cp++;
        while (true) {
          char new_c = *cp;
          if (new_c == quoteType) {
            break;
          }
          cp++;
        }
      }
      if (isspace(c)) {
        *cp = '\0';
      }

      cp++;
    }

    // adds data to resultset row
    resultset_addRow(rs);
    cp = dataBuffer;
    for (int i = 1; i < num_cols + 1; i++) {
      int len_cp = strlen(cp);
      // checks if string literal
      if (strchr(cp, '"') || strchr(cp, '\'')) {

        int count = 0;
        char noQuotes[len_cp - 2];
        for (int j = 1; j < len_cp - 1; j++) {
          noQuotes[j - 1] = cp[j];
          count++;
        }
        noQuotes[count] = '\0';
        resultset_putString(rs, rowCount, i, noQuotes);

      }
      // checks if int or real
      else if (isdigit(cp[0])) {
        if (strchr(cp, '.')) {
          resultset_putReal(rs, rowCount, i, atof(cp));

        } else {

          resultset_putInt(rs, rowCount, i, atoi(cp));
        }
      }

      cp += len_cp + 1; // advance to start of next value
    }

    rowCount++;
  }

  free(dataBuffer);
  fclose(datafile);

  // Delete rows from resultSet if a "where" clause is present
  if (select->where != NULL) {

    // iterates through the columns until col matches where col's name
    char *where_col = select->where->expr->column->name;
    struct RSColumn *cur = rs->columns;
    int colNum = 1;
    while (true) {
      if (strcasecmp(cur->colName, where_col) == 0) {
        break;
      }
      cur = cur->next;
      colNum++;
    }
    
    struct RSValue *rsVal = cur->data;
    int valType = rsVal[colNum - 1].valueType;
    char *expr_val = select->where->expr->value;
    int op = select->where->expr->operator;
    
    // iterate through the rows for each column
    for (int i = rs->numRows - 1; i >= 0; i--) {


      bool trueExpr = true;
      if (valType == 1) {
        // check whether each expression is True
        int rs_int = resultset_getInt(rs, i + 1, colNum);
        if (op == 0) {
          trueExpr = rs_int < atoi(expr_val);
        } else if (op == 1) {
          trueExpr = rs_int <= atoi(expr_val);
        } else if (op == 2) {
          trueExpr = rs_int > atoi(expr_val);
        } else if (op == 3) {
          trueExpr = rs_int >= atoi(expr_val);
        } else if (op == 4) {
          trueExpr = rs_int == atoi(expr_val);
        } else if (op == 5) {
          trueExpr = rs_int != atoi(expr_val);
        }

      } else if (valType == 2) {
        double rs_real = resultset_getReal(rs, i + 1, colNum);
        if (op == 0) {
          trueExpr = rs_real < atof(expr_val);
        } else if (op == 1) {
          trueExpr = rs_real <= atof(expr_val);
        } else if (op == 2) {
          trueExpr = rs_real > atof(expr_val);
        } else if (op == 3) {
          trueExpr = rs_real >= atof(expr_val);
        } else if (op == 4) {
          trueExpr = rs_real == atof(expr_val);
        } else if (op == 5) {
          trueExpr = rs_real != atof(expr_val);
        }

      } else {
        char *rs_str = resultset_getString(rs, i + 1, colNum);

        if (op == 0) {
          trueExpr = strcmp(rs_str, expr_val) < 0;
        } else if (op == 1) {
          trueExpr = strcmp(rs_str, expr_val) <= 0;
        } else if (op == 2) {
          trueExpr = strcmp(rs_str, expr_val) > 0;
        } else if (op == 3) {
          trueExpr = strcmp(rs_str, expr_val) >= 0;
        } else if (op == 4) {
          trueExpr = strcmp(rs_str, expr_val) == 0;
        } else if (op == 5) {
          trueExpr = strcmp(rs_str, expr_val) != 0;
        }

        free(rs_str);
      }
      if (!trueExpr) {
        resultset_deleteRow(rs, i + 1);
      }
    }
  }

  // delete cols not in ast
  for (int i = num_cols - 1; i >= 0; i--) {
    char *col_name = col_meta[i].name;
    bool found = false;
    struct COLUMN *cur = select->columns;

    while (cur != NULL) {
      // printf("Col Name: %s | AST: %s", col_name, cur->name);

      if (strcasecmp(col_name, cur->name) == 0) {
        found = true;
        break;
      }
      cur = cur->next;
    }

    if (!found) {

      resultset_deleteColumn(rs, i + 1);
      // count--;
    }
    // count++;
  }

  // reorder the columns to match the query and applies function to columns
  struct COLUMN *cur = select->columns;
  int to_pos = 1;

  while (cur != NULL) {
    int function = cur->function;

    int from_pos = resultset_findColumn(rs, 1, table_name, cur->name);

    if (function != NO_FUNCTION) {
      resultset_applyFunction(rs, function, from_pos);
    }

    resultset_moveColumn(rs, from_pos, to_pos);

    cur = cur->next;
    to_pos++;
  }

  // delete all rows past the limit

  if (select->limit != NULL) {
    int limit = select->limit->N;
    for (int i = rs->numRows; i > 0; i--) {
      if (i > limit) {
        resultset_deleteRow(rs, i);
      }
    }
  }

  resultset_print(rs);

  resultset_destroy(rs);
  //
  // done!
  //
}
