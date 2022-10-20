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


int main()
{
    context con;
    con.root = node_new(NTERM,PS_MARK,"ROOT");
    ast_stack_init(&(con.expr_stack),64);
    lex_init(&con);

    if(parse(&con) != P_SUCCESS) return(SYNTAX_ERROR);
    tree_dot_print(stdout,con.root);

    lex_destroy();
    node_delete(&(con.root));
    ast_stack_destroy(&(con.expr_stack));
    free(con.attrib);
    return(SUCCESS);
}




