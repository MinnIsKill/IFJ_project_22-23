/**
 * @brief Collection of datastructures that need to be shared betwen 
 *        modules
 * @file context.h
 * @author Jan Lutonský, xluton02
 **/

#ifndef INCLUDE_CONTEXT_H
#define INCLUDE_CONTEXT_H

#include"symtable.h"
#include"ast.h"
#include"ast_stack.h"
#include"token.h"

// TODO implement
//#include"char_buffer.h"

typedef struct
{
    ast_node* root;
    struct bintree_node* global_symtab;
    ast_stack expr_stack;
    token_type token;
    char* attrib;
//    char_buffer attrib; this will replace line above
}context;

#endif
