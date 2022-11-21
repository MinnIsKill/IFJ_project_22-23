/**
 * @brief Generate IFJcode22 code from abstract syntax tree
 * @file codegen_tmp.c
 * @author Jan Lutonský, xluton02
 *
 **/
#include"codegen_tmp.h"

bool gen_ADD(context* cont,char* where, char* left, char* right)
{
    // TODO type conversions
    // TODO gen code
    return(true);
}

char* gen_expr(context* cont, ast_node* root)
{
    if(root == NULL)
    {
        return(NULL);
    }
   
    // skip EXPR_PAR
    if(root->type == EXPR_PAR)
    {
        if(root->children[0] == NULL)
        {
            return(NULL); 
        }

        root = root->children[0];
    }

    char* left = gen_expr(cont,root->children[0]);
    char* right = gen_expr(cont,root->children[1]);

    char* where = NULL; // TODO gen name + malloc

    switch(root->sub_type)
    {
        //TODO other opers
        case(ADD):
            gen_ADD(cont,where,left,right);
            break;
        default:
            break;
    
    }

    free(left);
    free(right);
    return(where);
}

