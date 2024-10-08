#include<stdio.h>
#include<stdbool.h>
#include"./../../src/error.h"
#include"./../../src/parser.h"
#include"./../../src/context.h"
#include"./../../src/fake_lex.h"
#include"./../../src/semantic.h"
#include"./../../src/symtable.h"
#include"./../../src/lex.h"


int main()
{
    context con;
    if(!context_new(&con))
    {
        return(INTERNAL_ERROR);
    }

    p_codes p_rc = parse(&con);
    INFORUN(parser_print_header(p_rc););
    switch(p_rc)
    { 

        //TODO other error types
        default:
            context_delete(&con); 
            return(SYNTAX_ERROR); 
        
        case(P_SUCCESS):
            tree_dot_print(stdout,con.root); 
            break; // continue
    } 

    
    semantic_retcodes s_rc = semantic(&con);
    infoprintt("!!!!!!!!!!\n    semantics ended with code:  [%d]\n!!!!!!!!!!\n", s_rc);

    context_delete(&con);
    return(s_rc);
}




