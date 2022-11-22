/**
 * @brief Generate IFJcode22 code from abstract syntax tree
 * @file codegen.c
 * @author Jan Lutonský, xluton02
 *
 **/
#include"codegen.h"
#include"ast.h"
#include"stdio.h"

//forward declarations
void gen_expr(ast_node* root, char side);
void gen_expr_assign(ast_node* root);
void gen_main(ast_node* root/*, struct bintree_node* global_symtab*/);

void gen_prolog()
{
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR GF@_EF\n");
    printf("DEFVAR GF@_EL\n");
    printf("DEFVAR GF@_ER\n");
    printf("DEFVAR GF@_EBL\n");
    printf("DEFVAR GF@_EBR\n");

    //TODO generate all the function calls
}

void gen_epilog()
{
    // for gen_expr_purposes
    printf("WRITE GF@_EF\n");
}


void codegen(context* cont)
{
    ast_node* root = cont->root;
    gen_prolog();

    // skip root
    root = root->children[0];
    for(size_t n = 0; n < root->children_cnt; n++)
    {
        gen_main(root->children[n]);
    }

    gen_epilog();
}

// basicly router to the correct generator gunction
void gen_main(ast_node* root/*, struct bintree_node* global_symtab*/)
{
    switch(root->type)
    {
        case(EXPR_ASSIGN):
            gen_expr_assign(root);
            break;

        case(EXPR):
            gen_expr(root,'F');
            break;

        default:
            return;
    }
}

void gen_expr_assign(ast_node* root)
{
    char* ID = root->children[0]->attrib;

    // TODO only if was not already defined
    // TODO zaznam v tabulce symbolu
    printf("DEFVAR LF@%s\n",ID);
    gen_expr(root->children[1],'F');
    printf("MOVE LF@%s GF@_EF\n",ID);

}

void gen_expr(ast_node* root, char side)
{
    // TODO check if side in {L,F,R}

    if(root == NULL)
    {
        return;
    }
   
    // skip EXPR_PAR
    if(root->type == EXPR_PAR)
    {
        if(root->children[0] == NULL)
        {
            return;
        }

        root = root->children[0];
    }

    if(root->children_cnt > 0)
    {
        gen_expr(root->children[0],'L');
        gen_expr(root->children[1],'R');
    }

    switch(root->sub_type)
    {
        case(ID):
            printf("MOVE GF@_E%c LF@%s\n",side,root->attrib);
            return;

        case(IVAL):
            printf("MOVE GF@_E%c int@%s\n",side,root->attrib);
            return;

        case(FVAL):
            printf("MOVE GF@_E%c float@%s\n",side,root->attrib);
            return;
        
        case(SVAL):
            printf("MOVE GF@_E%c string@%s\n",side,root->attrib);
            return;
        
        case(VVAL):
            printf("MOVE GF@_E%c nil@nil\n",side);
            return;

        case(ADD):
            printf("ADD");
            break;
        
        case(SUB):
            printf("SUB");
            break;

        case(MUL):
            printf("MULL");
            break;
            
        case(DIV):
            // TODO IDIV
            printf("DIV");
            break;

        case(STRCAT):
            printf("CONCAT");
            break;
        
        case(EQ):
            printf("EQ");
            break;

        case(LT):
            printf("LT");
            break;
        
        case(GT):
            printf("GT");
            break;
        
        case(LTE):
            printf("LT GF@_EBL GF@_EL GF@_ER\n");
            printf("EQ GF@_EBR GF@_EL GF@_ER\n");
            printf("OR GF@_E%c GF@_EBL GF@_EBR\n",side);
            return;
        
        case(GTE):
            printf("GT GF@_EBL GF@_EL GF@_ER\n");
            printf("EQ GF@_EBR GF@_EL GF@_ER\n");
            printf("OR GF@_E%c GF@_EBL GF@_EBR\n",side);
            return;
        
        case(NEQ):
            printf("EQ GF@_EBL GF@_EL GF@_ER\n");
            printf("NOT GF@_E%c GF@_EBL\n",side);
            return;
        
        default:
            break;
    
    }
    
    printf(" GF@_E%c GF@_EL GF@_ER\n",side);
}