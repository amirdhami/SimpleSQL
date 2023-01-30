/*parser.h*/

//
// Project: Parser for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include <stdio.h>
#include <stdbool.h>  // true, false

#include "tokenqueue.h"


//
// parser_init
//
// Call this once before you start calling parser_parse().
//
void parser_init(void);

//
// parser_eof
//
// Returns true if a call to parser_parse() encountered EOF, 
// otherwise false is returned. 
//
bool parser_eof(void);

//
// parser_parse
//
// Given an input stream, uses the scanner to obtain the tokens
// and then checks the syntax of the input against the BNF rules
// for the subset of SQL we are supporting. 
//
// Returns NULL if a syntax error was found; in this case 
// an error message was output. Returns a pointer to a list
// of tokens -- a Token Queue -- if no syntax errors were 
// detected. This queue contains the complete input in token
// form for further analysis.
//
// NOTE: it is the callers responsibility to free the resources
// used by the Token Queue.
//
struct TokenQueue* parser_parse(FILE* input);
