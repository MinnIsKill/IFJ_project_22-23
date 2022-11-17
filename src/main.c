/**
 * @brief Operator Precedence Parser
 * @author Jan Lutonský, xluton02
 **/
#include<stdio.h>
#include<stdbool.h>
#include"error.h"
#include"parser.h"
#include"context.h"
#include"fake_lex.h"
#include"semantic.h"
#include"symtable.h"


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




