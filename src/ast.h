/**
 * @brief Abstract syntax tree datastructure
 * @file ast.h
 * @author Jan Lutonský, xluton02
 *
 * AST nodes are used by both parsers.
 * compile with -DDEBUG to get additional debugging
 * messages.
 **/

#ifndef INCLUDED_AST_H
#define INCLUDED_AST_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

#include"token.h"
#include"dbg.h"

// type of node in ast
typedef enum
{
    TERM,        // terminal/leaf node
    NTERM,       // non terminal generic rule, will change
    EXPR,        // expression
    EXPR_PAR,    // ( expression ) ; expression surrounded with parenthesis 
    EXPR_FCALL,  // FID ( expr or expr_list ) ; function call
    EXPR_LIST,   // expr,expr,... ; list of at least two expressions.
                 //  can only be found under EXPR_FCALL node
    EXPR_ASSIGN, // ID = EXPR
    BODY,
    FUN_BODY,
    PROG_BODY, 
    ASSIGNMENT,
    WHILE_N,
    IF_N,
    ELSE_N,
    RETURN_N,
    RET_TYPE,
    TYPE_N,
    FDEF,
    PAR_LIST,
}node_type;

struct ast_node
{
    unsigned long long id; // used for tree_dot_print()
    node_type type;        // type of node
    token_type sub_type;   // type of token if needed
    char* attrib;          // token attrib if needed
    size_t children_cnt;   // number of children
    struct ast_node** children; // array of children pointers
};
typedef struct ast_node ast_node; // remove annoyning struct keyword from type

/**
 * @brief Allocate new node
 * @param[in] type - type of node
 * @param[in] sub_type - type of token
 * @param[in] attrib - token attrib, new buffer will be allocated
 *                     and string cloned. NULL == ""
 * @return NULL if allocation failed else valid pointer
 **/
ast_node* node_new(node_type type, token_type sub_type, char* attrib);

/**
 * @brief Free memort allocated for node and all its children
 * @warning this will free while tree
 * @warning recursive
 * @param n - node which will be deleted
 **/
void node_delete(ast_node** n);

/**
 * @brief Add children to a node
 * @warning added src node must NOT be deleted(node_delete)
 *          this will do dst node when it is deleted!
 * @param[in] dst - src node will be added to this node
 * @param[in] src - this node will be added to dst
 * @return true when node could be added or false otherwise
 **/
bool node_add(ast_node* dst,ast_node* src);


//========== DEBUG AND PRINT FUNCTIONS ==========
/**
 * @brief return string representation of node_type enum
 * @warning DO NOT FREE
 * @param[in] t - value to be converted
 * @return pointer into static table of strings
 **/
const char* node_type_str(node_type t);

/**
 * @brief Print string representation of one node into f
 * @param[in] f - output file
 * @param[in] n - node to be printed
 **/
void node_print(FILE* f, ast_node* n);

/**
 * @brief Print tree structure in dot format into f
 * @warning recursive
 * @param[in] f - output file
 * @param[in] n - node(tree) to be printed
 **/
void tree_dot_print(FILE* f, ast_node* n);

#endif
