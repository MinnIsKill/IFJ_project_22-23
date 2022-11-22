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


int main()
{
    context con;
    context_new(&con);
    
    int rc;
    switch(parse_expr(con.root,&con))
    {
        case(EP_EXPR):
        case(EP_EXPR_PAR):
        case(EP_EXPR_FCALL):
        case(EP_EXPR_ASSIGN):
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
    
    context_delete(&con);
    return(rc);
}




