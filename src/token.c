/**
 * @brief Token data type + functions
 * @file token.c
 * @author Jan Lutonský, xluton02
 **/

#include"token.h"

const char* token_str(token_type t)
{
    static const char* s[]=
    {
        "<EOS>",    // end of source == EOF
        "function",  // function
        "if",
        "else",
        "while",
        "return", 

        // types
        "int",  // int
        "float",  // float
        "string",  // string
        "void",  // void
        
        // nullable types
        "?int", // ?int
        "?float", // ?float
        "?string", // ?string
        
        // type op
        ":",  //:

        // consts
        "ival",  // int
        "fval",  // float
        "sval",  // (?:string@)?"(?:[^\$]*)|(?:\x(?:[0-9a-fa-f][0-9a-fa-f])|(?:\[\t"n$])|(?:\[0-9][0-9][0-9]))"
        "null",  // null
        
        // identifier
        "ID",
        "FID",

        "+",    // +
        "-",    // -
        "*",    // *
        "/",    // /
        ".", // .
        "<",     // <
        ">",     // >
        "<=",    // <=
        ">=",    // >=
        "===",     // ===
        "!==",    // !==
        ",",  // ,

        "(",   // (
        ")",   // )
        "=",  // =

        // scopes
        "{",    // {
        "}",    // }
        ";",     // ;

        // other
        "<?php", // <?php
        "declare(strict_types=1);", // declare(strict_types=1); 
        "?>", // ?>, optional

        // special 
        "not used",
        "you shoul not see dis", // this token signify error
        "$",          // $ used by operator precedence parser
        "NULL -> INT",   // 0, return"0";
        "NULL -> FLOAT", // 0.0, return "0.0";
        "NULL -> BOOL",  // false, return "false";
        "NULL -> STR",   // "", return "";
        "INT -> FLOAT",  // INT2FLOAT
        "INT -> BOOL",   // if(val != 0 ) return TRUE else return FALSE
        "FLOAT -> INT",  // FLOAT2IN
        "FLOAT -> BOOL", // if(val != 0.0) return TRUE else RETURN FALSE 
        "BOOL -> INT",   // TRUE -> 1 , FALSE -> 0
        "BOOL -> FLOAT", // TRUE -> 1.0 , FALSE -> 0.0
        "BOOL -> STR",   // TRUE -> "1", FALSE -> ""
        "STR -> BOOL",   // "" -> FALSE else return true
        };
        return(s[t]);
}
