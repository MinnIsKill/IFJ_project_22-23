#ifndef LEX_H_INCLUDED
#define LEX_H_INCLUDED

#include <errno.h>
#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "context.h"

/* Determines how to scanner will interact with incoming code */
typedef enum lex_mode
{
    /* Loading only '<?php' */
    LEX_MODE_START,

    /* Loading optionally '\s+declare(strict_types=1);' */
    LEX_MODE_CONTINUE,
    
    /* Loading rest of the tokens */
    LEX_MODE_NORMAL
} lex_mode;

/* Internal states used for token analyzation */
typedef enum lex_state
{
    /* The initial state */
    LEX_STATE_START,

    /* Intermediate states */
    LEX_STATE_COM_0,
    LEX_STATE_LCOM_0,
    LEX_STATE_BCOM_0,
    LEX_STATE_BCOM_1,

    LEX_STATE_ID_0,
    LEX_STATE_ID_1,
    LEX_STATE_FID_0,
    LEX_STATE_NID_0,

    LEX_STATE_IVAL_0,
    LEX_STATE_FVAL_0,
    LEX_STATE_FVAL_1,
    LEX_STATE_FVAL_2,
    LEX_STATE_FVAL_3,
    LEX_STATE_FVAL_4,
    LEX_STATE_SVAL_0,
    LEX_STATE_SVAL_1,
    LEX_STATE_SVAL_X0,
    LEX_STATE_SVAL_X1,
    LEX_STATE_SVAL_O0,
    LEX_STATE_SVAL_O1,

    LEX_STATE_ASSIG_0,
    LEX_STATE_LT_0,
    LEX_STATE_GT_0,
    LEX_STATE_EQ_0,
    LEX_STATE_NEQ_0,
    LEX_STATE_NEQ_1,

    LEX_STATE_PS_MARK_0,
    LEX_STATE_PC_MARK_0,
    LEX_STATE_PC_MARK_1,
    LEX_STATE_PC_MARK_2,
    LEX_STATE_PE_MARK_0,
    LEX_STATE_PE_MARK_1,
    LEX_STATE_PE_MARK_2,

    /* An error occured state */
    LEX_STATE_ERROR,

    /* End states that return the token */
    LEX_STATE_END,

    LEX_STATE_FUNC,
    LEX_STATE_IF,
    LEX_STATE_ELSE,
    LEX_STATE_WHILE,
    LEX_STATE_RETURN,

    LEX_STATE_ITYPE,
    LEX_STATE_FTYPE,
    LEX_STATE_STYPE,
    LEX_STATE_VTYPE,

    LEX_STATE_NITYPE,
    LEX_STATE_NFTYPE,
    LEX_STATE_NSTYPE,

    LEX_STATE_IVAL,
    LEX_STATE_FVAL,
    LEX_STATE_SVAL,
    LEX_STATE_VVAL,

    LEX_STATE_ID,
    LEX_STATE_FID,

    LEX_STATE_ADD,
    LEX_STATE_SUB,
    LEX_STATE_MUL,
    LEX_STATE_DIV,

    LEX_STATE_ASSIG,
    LEX_STATE_LT,
    LEX_STATE_LTE,
    LEX_STATE_GT,
    LEX_STATE_GTE,
    LEX_STATE_EQ,
    LEX_STATE_NEQ,

    LEX_STATE_LPAR,
    LEX_STATE_RPAR,
    LEX_STATE_LBRC,
    LEX_STATE_RBRC,

    LEX_STATE_COMMA,
    LEX_STATE_TYPE,
    LEX_STATE_SEMIC,
    LEX_STATE_STRCAT,
    
    LEX_STATE_PS_MARK,
    LEX_STATE_PC_MARK,
    LEX_STATE_PE_MARK,
    LEX_STATE_EOS
} lex_state;

/**
 * @brief A function pointer to a scanner state implementation
 * 
 * @param[in] context Compiler context
 * @param[in] input The input stream
 * @return Next state to enter
 */
typedef lex_state (*lex_state_function)(context* context, FILE* input);

/**
 * @brief Initializes the scanner, loads first token and stores it to context
 * 
 * @param[in] context Compiler context
 * @param[in] input The input stream
 * @return true if operation succeeded
 */
bool lex_init(context* context, FILE* input);

/**
 * @brief Loads next token and stores it to context
 * 
 * @param[in] context Compiler context
 * @param[in] input The input stream
 * @return true if operation succeeded
 */
bool lex_next(context* context, FILE* input);

/**
 * @brief Frees all scanner used memory
 * 
 * @param context Compiler context
 */
void lex_destroy(context* context);

#endif