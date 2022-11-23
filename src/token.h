/**
 * @brief Token data type + functions
 * @file token.h
 * @author Jan Lutonský, xluton02
 **/

#ifndef INCLUDE_TOKEN_H
#define INCLUDE_TOKEN_H

typedef enum{
    EOS = 0,    // end of source == EOF
    // keywords
    FUNC,  // function
    IF,
    ELSE,
    WHILE,
    RETURN, 

    // types
    ITYPE,  // int
    FTYPE,  // float
    STYPE,  // string
    VTYPE,  // void
    
    // nullable types
    NITYPE, // ?int
    NFTYPE, // ?float
    NSTYPE, // ?string
    
    // type op
    TYPE,  //:

    // consts
    IVAL,  // int
    FVAL,  // float
    SVAL,  // (?:string@)?"(?:[^\$]*)|(?:\x(?:[0-9A-Fa-f][0-9A-Fa-f])|(?:\[\t"n$])|(?:\[0-9][0-9][0-9]))"
    VVAL,  // null
    
    // identifier
    ID,     // must start with $
    FID,    // mus not start with $

    ADD,    // +
    SUB,    // -
    MUL,    // *
    DIV,    // /
    STRCAT, // .
    LT,     // <
    GT,     // >
    LTE,    // <=
    GTE,    // >=
    EQ,     // ===
    NEQ,    // !==
    COMMA,  // ,

    LPAR,   // (
    RPAR,   // )
    ASSIG,  // =

    // scopes
    LBRC,    // {
    RBRC,    // }
    SEMIC,     // ;

    // other
    PS_MARK, // <?php
    PC_MARK, // declare(strict_types=1); 
    PE_MARK, // ?>, optional
    

    // special 
    NOT_USED, // used in syntax tree for node that does't need subtype
    INVALID,  // this token signify error
    STOP,      // $ used by operator precedence parser
    
    // used in type conversions
    NULL_TO_INT,   // 0, return"0";
    NULL_TO_FLOAT, // 0.0, return "0.0";
    NULL_TO_BOOL,  // false, return "false";
    NULL_TO_STR,   // "", return "";
    INT_TO_FLOAT,  // INT2FLOAT
    INT_TO_BOOL,   // if(val != 0 ) return TRUE else return FALSE
    FLOAT_TO_INT,  // FLOAT2IN
    FLOAT_TO_BOOL, // if(val != 0.0) return TRUE else RETURN FALSE 
    BOOL_TO_INT,   // TRUE -> 1 , FALSE -> 0
    BOOL_TO_FLOAT, // TRUE -> 1.0 , FALSE -> 0.0
    BOOL_TO_STR,   // TRUE -> "1", FALSE -> ""
    STR_TO_BOOL,   // "" -> FALSE else return true
    
    IDIV,    // semantic converts DIV to IDIV when both operands are INT

}token_type;

const char* token_str(token_type t);

#endif
