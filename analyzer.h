/*analyzer.h*/

//
// Project: Analyzer for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include "ast.h"
#include "database.h"
#include "tokenqueue.h"


//
// analyzer_build
//
// Given a database schema and a list of syntactically-valid
// tokens, analyzes the tokens for a semantically-valid SQL
// program and returns an AST (abstract syntax tree) of the
// program.
//
// Returns NULL if a semantic error was found; in this case 
// an error message was output. Returns a pointer to an AST
// if successful --- no semantic errors were detected.
//
// NOTE: it is the callers responsibility to free the resources
// used by the Token Queue, and the AST.
//
struct QUERY* analyzer_build(struct Database* db, struct TokenQueue* tokens);

//
// analyzer_destroy
//
// Frees the memory associated with the query; call this
// when you are done with the AST.
//
void analyzer_destroy(struct QUERY* query);
