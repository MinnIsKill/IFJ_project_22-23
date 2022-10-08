#ifndef INCLUDE_CONTEXT_H
#define INCLUDE_CONTEXT_H



#include"symtable.h"
#include"ast.h"
#include"ast_stack.h"
#include"token.h"

// this may change
#include"char_buffer.h"

typedef struct
{
    ast_node*   root;
    symtable*   global_symtab;
    ast_stack   expr_stack;
    token_type  token;
    char_buffer atrib_buffer;
}context;

#endif
