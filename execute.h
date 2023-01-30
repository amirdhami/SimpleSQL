/*execute.h*/

//
// Project: Execution of queries for SimpleSQL
//
// YOUR NAME
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

//
// #include header files needed to compile function declarations
//
#include "database.h"
#include "ast.h"
//
// function declarations:
//
void execute_query(struct Database *db, struct QUERY *query);