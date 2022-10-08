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
    INVALID, // this token signify error
    STOP          // $ used by operator precedence parser
}token_type;

const char* token_str(token_type t);

#endif
