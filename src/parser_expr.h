/** 
 * @brief Expression parser
 * @file parser_expr.h
 * @author Jan Lutonský, xluton02
 * 
 * TODO remove support for unart +-
 * TODO add support for bool( || && !)
 *
 * compile with -DDEBUG -DINFO to get additional debugging
 * messages.
 **/
#ifndef INCLUDE_PAR_UTILS_H
#define INCLUDE_PAR_UTILS_H

#include<stdbool.h>
#include<stdio.h>
#include"ast.h"
#include"ast_stack.h"
#include"fake_lex.h"
#include"context.h"

// return codes of parser and its inner functions
typedef enum
{
    EP_SUCCESS,         // inner use, parse_expr() will not return this
    EP_EXPR,            // expr was parsed         |
    EP_EXPR_PAR,        // expr_par was parsed     |>-- expression was added to root
    EP_EXPR_FCALL,      // expr_fcall was parsed   |
    EP_EXPR_ASSIGN,     // expr_assign was parsed  |
    EP_SYNTAX_ERROR,    // syntax error
    EP_STACK_ERROR,     // error while manipulating with stack
    EP_AST_ERROR,       // error while manipulating with ast
    EP_UNKNOWN_ERROR,   // other kind of errors
}ep_codes;

/** 
 * @brief parse expression and append it to root
 * @param[in] root ast node where the parsed expression will be connected
 * @param[in] cont context
 * @retrun one of ep_codes 
 * @see ep_codes
 *
 *  TODO return type of expression thath was parsed
 *  maybe use root from cont instead of sepate pointer(root)
 **/
ep_codes parse_expr(ast_node* root, context* cont);

#endif
