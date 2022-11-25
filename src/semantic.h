/**
 * @file semantic.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief semantic analyzer implementation header
 *        receives an AST passed by parser as input, walks through it to evaluate semantic
 *        correctness, and if successful, passes the AST to code generator
 *
 * @date of last update:   22nd November 2022
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "context.h"
#include "symtable.h"
#include "dll.h"
#include "ast.h"
#include "vector.h"
#include "dbg.h"
#include "symtable_stack.h"

typedef enum{
    SEM_SUCCESS               = 0, //no error
    FUNC_DEF_ERR              = 3, //undefined function, attempt at function redefinition
    FUNC_CALL_OR_RETTYPE_ERR  = 4, //wrong number/type of parameters passed while calling a function, or wrong return type
    UNDEF_VAR_ERR             = 5, //attempt at using an undefined variable
    FUNC_RET_EXPR_ERR         = 6, //missing or excessive expression in return
    INCOMP_TYPES_ERR          = 7, //type incompatibility in arithmetic, string or relational expressions
    SEM_GENERAL_ERR           = 8, //others (division by zero, ...)
    ERR_INTERNAL              = 99 //internal error not to do with semantics (malloc fail, etc.)
}semantic_retcodes;

void AST_dotprint(ast_node* root);
void AST_dotprint_internal(ast_node* root);

const char* node_type_tostr(node_type type);
const char* node_subtype_tostr(token_type type);
arg_type token_type_to_arg_type(token_type token);
arg_type token_type_to_arg_type_forvals(token_type token);

bool is_it_predef_func_call(ast_node* node);

arg_type semantic_get_expr_type(ast_node* node, struct bintree_node* global_symtab);

struct bintree_node* global_symtab_funcinsert(ast_node* node, struct bintree_node* global_symtab);
struct bintree_node* AST_DF_firsttraversal(ast_node* node, struct bintree_node* global_symtab);

void handle_conversions(ast_node* parent, arg_type type_l, arg_type type_r);
void handle_conversions_conditionals(ast_node* parent, ast_node* child, struct bintree_node* global_symtab);

void semantic_check_assign(ast_node* node, struct bintree_node* global_symtab);
void semantic_check_fcall(ast_node* node, struct bintree_node* global_symtab);
void semantic_check_expr(ast_node* node, struct bintree_node* global_symtab);
void semantic_check_return(ast_node* node, struct bintree_node* global_symtab);
void semantic_check_conditionals(ast_node* node, struct bintree_node* global_symtab);

void AST_DF_traversal(ast_node* node, struct bintree_node* global_symtab);

int semantic(context* cont);

#endif