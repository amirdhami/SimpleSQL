/*tokenqueue*/

//
// Project: Token Queue for SimpleSQL
//
// Prof. Joe Hummel
// Northwestern University
// CS 211, Winter 2023
//

#pragma once

#include <stdbool.h>  // true, false
#include "token.h"


struct TokenNode
{
  struct Token token;
  char* value;
  struct TokenNode* next;
};

struct TokenQueue
{
  struct TokenNode* head;
  struct TokenNode* tail;
};

//
// functions
//
struct TokenQueue* tokenqueue_create(void);
void               tokenqueue_destroy(struct TokenQueue* tokens);

void tokenqueue_enqueue(struct TokenQueue* tokens, struct Token token, char* value);
void tokenqueue_dequeue(struct TokenQueue* tokens);
bool tokenqueue_empty(struct TokenQueue* tokens);
struct Token tokenqueue_peekToken(struct TokenQueue* tokens);
char* tokenqueue_peekValue(struct TokenQueue* tokens);
void tokenqueue_print(struct TokenQueue* tokens);

struct TokenQueue* tokenqueue_duplicate(struct TokenQueue* tokens);
