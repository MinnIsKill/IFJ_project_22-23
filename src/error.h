/**
 * @brief Errors and return codes
 * @file error.h
 * @author Jan Lutonský, xluton02
 **/

#ifndef INCLUDE_ERROR_H
#define INCLUDE_ERROR_H

//----------------  MAIN return codes --------------------------

// NO error
#define SUCCESS 0

// generic LEX error
#define LEX_ERROR 1

// generic SYNTAX error
#define SYNTAX_ERROR 2

// redeffinition of function
// or function is missing
#define SEMANTIC_FUNC 3

// function was called with invalid number of arguemnts
// function was called with invalid argument type/s
#define SEMANTIC_TYPE  4

// use of undefined variable
#define SEMANTIC_VAR  5

// return has more or less arguments
#define SEMANTIC_RETURN  6

// expression has invalid type
#define SEMANTIC_EXPR  7

// othe semantic errors
#define SEMANTIC_ERROR  8

// internal error, failed memory allocation
#define INTERNAL_ERROR  99


//------------------------------------------

// these two error code exists just for debugging purposes
// and should be romoved or translated to INTERNAL_ERROR in main !
#define AST_ERROR 100
#define AST_STACK_ERROR 101

// internaly used in parser
#define CAN_SKIP 1

#endif
