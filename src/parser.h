/* 
 * @brief Parser
 * @file parser.h
 * @author Jan Lutonský, xluton02
 * 
 * compile with -DDEBUG -DINFO to get additional debugging
 * messages.
 *
 * TODO unifi error codes with expression parser
 */
#ifndef INCLUDE_PARSER_H
#define INCLUDE_PARSER_H

#include<stdbool.h>

#include"stdio.h"
#include"fake_lex.h"
#include"ast.h"
#include"parser_expr.h"
#include"token.h"
#include"context.h"

typedef enum
{
    P_SUCCESS,      // succesfull parse, ast can be read from cont->root
    P_SYNTAX_ERROR, // syntax error
    P_CAN_SKIP,     // inner use parse() will not return this
    P_AST_ERROR,    // error while manipulating with stack
    P_STACK_ERROR,  // error while manipulating with ast
    P_PARAM_ERROR,  // wrong parameters
}p_codes;

/*
 * @brief Recursive descend parser
 * @param[in] cont - parser context
 * @return one of p_codes
 * @see p_codes
 */
p_codes parse(context* cont);

// === DEBUG functions ===
/*
 * @brief Converts p_codes to string
 * @param[in] p code that will be converted to string
 * @return string representation of p
 * @node Do not free returnded string
 */
const char* p_codes_str(const p_codes p);

/*
 * @brief print apripriet header 
 * for given return code of parse()
 * @param[in] p return code of parse()
 */
void parser_print_header(const p_codes p);

#endif
