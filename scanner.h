/*scanner.h*/

//
// Scanner for SimpleSQL programming language. The scanner reads the input
// stream and turns the characters into language Tokens, such as identifiers,
// keywords, and punctuation.
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include <stdio.h>
#include "token.h"


//
// scanner_init
//
// Initializes line number, column number, and value before
// the start of the next input sequence. 
//
void scanner_init(int* lineNumber, int* colNumber, char* value);

//
// scanner_nextToken
//
// Returns the next token in the given input stream, advancing the line
// number and column number as appropriate. The token's string-based 
// value is returned via the "value" parameter. For example, if the 
// token returned is an integer literal, then the value returned is
// the actual literal in string form, e.g. "123". For an identifer,
// the value is the identifer itself, e.g. "ID" or "title". For a 
// string literal, the value is the contents of the string literal
// without the quotes.
//
struct Token scanner_nextToken(FILE* input, int* lineNumber, int* colNumber, char* value);
