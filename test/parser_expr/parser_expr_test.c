/**
 * @brief Operator Precedence Parser test
 * @author Jan Lutonský, xluton02
 **/
#include<stdio.h>
#include<stdbool.h>
#include"./../../src/parser_expr.h"
#include"./../../src/ast_stack.h"
#include"./../../src/fake_lex.h"
#include"./../../src/ast.h"
#include"./../../src/error.h"
#include"./../../src/context.h"

void context_clean(context* con)
{
    if(con == NULL)
    {
        return;
    }
    node_delete(&con->root);
    ast_stack_destroy(&(con->expr_stack));
    free(con->attrib);
}

int main()
{
    context con;
    con.root = node_new(NTERM,PS_MARK,"");
    ast_stack_init(&(con.expr_stack),64);
    lex_init(&con);
    
    int rc;
    switch(parse_expr(con.root,&con))
    {
        case(EP_EXPR):
        case(EP_EXPR_PAR):
        case(EP_EXPR_FCALL):
            tree_dot_print(stdout,con.root);
            rc = SUCCESS;
            break;

        case(EP_SYNTAX_ERROR):
            fprintf(stderr,"Expression parser returned syntax error.\n");
            rc = SYNTAX_ERROR;
            break;
        
        case(EP_STACK_ERROR):
            fprintf(stderr,"Expression parser failed due to stack.\n");
            rc = INTERNAL_ERROR;
            break;

        case(EP_AST_ERROR):
            fprintf(stderr,"Expression parser failed due to abstract syntaxtic tree.\n");
            rc = INTERNAL_ERROR;
            break;
       
        default:
            fprintf(stderr,"Expression parser returned unknown value.\n");
            rc = INTERNAL_ERROR;
            break;
    }
    
    context_clean(&con);
    lex_destroy();
    return(rc);
}




