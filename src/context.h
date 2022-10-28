/**
 * @brief Collection of datastructures that need to be shared betwen 
 *        modules
 * @file context.h
 * @author Jan Lutonský, xluton02
 **/

#ifndef INCLUDE_CONTEXT_H
#define INCLUDE_CONTEXT_H

#include"char_buffer.h"
#include"ast.h"
#include"ast_stack.h"
#include"symtable.h"
#include"token.h"

typedef struct
{
    ast_node* root;
    struct _binTree_node* global_symtab;
    ast_stack expr_stack;
    token_type token;
    char_buffer attrib;
}context;

#endif
