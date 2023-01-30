/*token.h*/

//
// Token definitions for SimpleSQL programming language
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once


//
// Token
// 
// A token in the SimpleSQL programming language
//
struct Token
{
  int id;    // token id (see enum below)
  int line;  // line containing the token (1-based)
  int col;   // column where the token starts (1-based)
};


//
// TokenID
//
// Every token in SimpleSQL has a unique ID number
//
enum TokenID
{
  SQL_UNKNOWN = -1, // a character that is not part of SimpleSQL
  SQL_EOS,          // $ or EOF
  SQL_SEMI_COLON,   // ;
  SQL_LEFT_PAREN,   // (
  SQL_RIGHT_PAREN,  // )
  SQL_ASTERISK,     // *
  SQL_DOT,          // .
  SQL_HASH,         // #
  SQL_COMMA,        // ,
  SQL_EQUAL,        // =
  SQL_GT,           // >
  SQL_GTE,          // >=
  SQL_LT,           // <
  SQL_LTE,          // <=
  SQL_NOT_EQUAL,    // <>
  SQL_INT_LITERAL,  // e.g. 123
  SQL_REAL_LITERAL, // e.g. 12. or 3.14159
  SQL_STR_LITERAL,  // e.g. "hello cs211" or 'hello cs211'
  SQL_IDENTIFIER,   // e.g. title or Count_2 or X
  //
  // keywords:
  //
  SQL_KEYW_ASC,    // case insensitive, e.g. "ASC" or "Asc" or "asc"
  SQL_KEYW_AVG,
  SQL_KEYW_BY,
  SQL_KEYW_COUNT,
  SQL_KEYW_DELETE,
  SQL_KEYW_DESC,
  SQL_KEYW_FROM,
  SQL_KEYW_INNER,
  SQL_KEYW_INSERT,
  SQL_KEYW_INTERSECT,
  SQL_KEYW_INTO,
  SQL_KEYW_JOIN,
  SQL_KEYW_LIKE,
  SQL_KEYW_LIMIT,
  SQL_KEYW_MAX,
  SQL_KEYW_MIN,
  SQL_KEYW_ON,
  SQL_KEYW_ORDER,
  SQL_KEYW_SELECT,
  SQL_KEYW_SET,
  SQL_KEYW_SUM,
  SQL_KEYW_UNION,
  SQL_KEYW_UPDATE,
  SQL_KEYW_VALUES,
  SQL_KEYW_WHERE
};
