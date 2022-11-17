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
#include"error.h"

typedef struct
{
    ast_node* root;
    struct bintree_node* global_symtab;
    ast_stack expr_stack;
    token_type token;
    char_buffer attrib;
}context;

/*
 * @brief Allocate data for and initialize compiler context
 * @param[in] c context to be initialized
 * @return true if context was initialized correctly
 *              else return false
 */
bool context_new(context* c);

/*
 * @brief Free data alocated for compiler context
 * @param[in] c context to be freed
 */
void context_delete(context* c);

#endif
