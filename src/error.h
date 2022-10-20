/**
 * @brief Errors and return codes
 * @file error.h
 * @author Jan Lutonský, xluton02
 **/

#ifndef INCLUDE_ERROR_H
#define INCLUDE_ERROR_H

// !!! THIS WILL PROBABLY CHANGE !!!

#define SUCCESS 0
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define FUNC_MISSING_OR_REDEFINED 3
#define FUNC_TYPE_ERROR  4
#define UNDEFINED_VAR  5
#define FUNC_RETURN_ERROR  6
#define EXPR_TYPE_ERROR  7
#define OTHER_SEMANTIC_ERROR  8
#define INTERNAL_ERROR  99
#define AST_ERROR 100
#define AST_STACK_ERROR 101
#define CHAR_BUFFER_ERROR 102
#define SYMTAB_ERROR 103
#define EXPR_PARSER_SYNTAX_ERROR 104
#define PARSER_SYNTAX_ERROR 105


#endif
