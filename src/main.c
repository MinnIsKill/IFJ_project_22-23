/**
 * @brief main function of compiler
 * @author Jan Lutonský, xluton02
 **/
#include<stdio.h>
#include<stdbool.h>
#include"error.h"
#include"parser.h"
#include"context.h"
#include"semantic.h"
#include"symtable.h"
#include"lex.h"
#include"codegen.h"


int main()
{
    context con;
    int con_rc = SUCCESS;
    if((con_rc = context_new(&con)) != SUCCESS)
    {
        return(con_rc);
    }

    p_codes p_rc = parse(&con);
    INFORUN(parser_print_header(p_rc););
    switch(p_rc)
    { 
        default:
            context_delete(&con); 
            return(INTERNAL_ERROR);

        case(P_SYNTAX_ERROR):
            context_delete(&con); 
            return(SYNTAX_ERROR);

        case(P_LEX_ERROR):
            context_delete(&con); 
            return(LEX_ERROR);
        
        case(P_SUCCESS):
            break; // continue
    } 

#ifdef DEBUG
    tree_dot_print(stdout, con.root);
#endif
    
    semantic_retcodes s_rc = semantic(&con);
    infoprintt("\n    semantics ended with code:  [%d]\n\n", s_rc);
    if(s_rc != SEM_SUCCESS)
    {
        context_delete(&con);
        return(s_rc);
    }

#ifdef DEBUG
    tree_dot_print(stdout, con.root);
#endif

    codegen(&con);

    context_delete(&con);
    return(SUCCESS);
}




