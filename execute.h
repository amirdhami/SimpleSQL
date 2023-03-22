/*execute.h*/

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
