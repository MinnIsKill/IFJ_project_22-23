#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include"token.h"
#include"context.h"
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>


void lex_next(context* cont);
void lex_init(context* cont);
void lex_destroy();

#endif // LEX_H_INCLUDED
