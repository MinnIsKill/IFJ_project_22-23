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


void clean_up(context con){
    lex_destroy();
    node_delete(&(con.root));
    bintree_dispose(con.global_symtab);
    ast_stack_destroy(&(con.expr_stack));
    free(con.attrib);
}

int main()
{
    int retcode = SUCCESS;

    context con;
    con.root = node_new(NTERM,PS_MARK,"ROOT");
    ast_stack_init(&(con.expr_stack),64);
    lex_init(&con);

    p_codes p_rc = parse(&con);
    infoprintt("!!!!!!!!!!\n    parser ended with code:  [%d]\n!!!!!!!!!!\n", p_rc); 
    switch(p_rc)
    { 

        //TODO other error types
        default:
            clean_up(con); 
            return(SYNTAX_ERROR); 
        
        case(P_SUCCESS):
            tree_dot_print(stdout,con.root); 
            break; // continue
    } 

    
    semantic_retcodes s_rc = semantic(&con);
    infoprintt("!!!!!!!!!!\n    semantics ended with code:  [%d]\n!!!!!!!!!!\n", s_rc);

    clean_up(con);
    return(s_rc);
}




