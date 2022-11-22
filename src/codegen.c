/**
 * @brief Generate IFJcode22 code from abstract syntax tree
 * @file codegen.c
 * @author Jan Lutonský, xluton02
 *
 **/
#include"codegen.h"
#include"ast.h"
#include<stdio.h>
#include"token.h"

//forward declarations
void gen_expr(ast_node* root, char side);
void gen_expr_assign(ast_node* root,struct bintree_node* tab);
void gen_fcall(ast_node* root); 
void gen_main(ast_node* root, struct bintree_node* tab);

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
    printf("DEFVAR GF@_FF\n");

    printf("MOVE GF@_EF  nil@nil\n");
    printf("MOVE GF@_EL  nil@nil\n");
    printf("MOVE GF@_ER  nil@nil\n");
    printf("MOVE GF@_EBL nil@nil\n");
    printf("MOVE GF@_EBR nil@nil\n");
    printf("MOVE GF@_FF  nil@nil\n");

}

void gen_epilog()
{
    // for gen_expr_purposes
    printf("WRITE GF@_EF\n");
}


void gen_fdef(ast_node* root, struct bintree_node* tab)
{
    char* fid = root->attrib;
    struct bintree_node* table = bintree_delete_by_key(tab, fid);
    printf("JUMP $$%s_SKIP\n",fid);
    printf("#================================ FUN DEF : %s\n",fid);
    printf("LABEL $$%s\n",fid);


    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("MOVE GF@_FF nil@nil\n");
    
    //for each var in par_list def var + pop value in it

    ast_node* pars = root->children[0];
    for(size_t n = 0; n < pars->children_cnt; ++n)
    {
        printf("DEFVAR LF@%s\n",pars->children[n]->attrib);
        printf("POPS LF@%s\n",pars->children[n]->attrib);
        
    }

    putchar('\n');

    root = root->children[2];
    for(size_t n = 0; n < root->children_cnt; ++n)
    {
        gen_main(root->children[n], table->local_symtab);
        putchar('\n');
    }

    
    printf("PUSHS GF@_FF\n");
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("#================================ FUN DEF : %s\n",fid);
    printf("LABEL $$%s_SKIP\n",fid);
}


void codegen(context* cont)
{
    ast_node* root = cont->root;
    struct bintree_node* tab = cont->global_symtab;
    
    gen_prolog();
    putchar('\n');


    // skip root
    root = root->children[0];
    for(size_t n = 0; n < root->children_cnt; ++n)
    {
        gen_main(root->children[n], tab);
        putchar('\n');
    }

    gen_epilog();
}
            
void gen_return(ast_node* root)
{
    gen_expr(root->children[0],'F');    
    printf("MOVE GF@_FF GF@_EF\n");
}
        
void gen_if(ast_node* root,struct bintree_node* tab)
{
    static unsigned long long int if_body_id = 0;
    gen_expr(root->children[0],'F');
    
    // todo convert
    printf("JUMPIFEQ $$IF_ELSE%llu GF@_EF bool@false\n",if_body_id);
   
    ast_node* true_body = root->children[1];
    for(size_t n = 0; n < true_body->children_cnt; ++n)
    {
        gen_main(true_body->children[n], tab);
        putchar('\n');
    }
   
    
    printf("JUMP $$IF_SKIP%llu\n",if_body_id);
    printf("LABEL $$IF_ELSE%llu\n",if_body_id);
   
    if(root->children_cnt > 2)
    {
        ast_node* else_body = root->children[2];
        for(size_t n = 0; n < else_body->children_cnt; ++n)
        {
            gen_main(else_body->children[n], tab);
            putchar('\n');
        }
    }
    
    printf("LABEL $$IF_SKIP%llu\n",if_body_id);
    if_body_id++;
}

// basicly router to the correct generator gunction
void gen_main(ast_node* root, struct bintree_node* tab)
{
    switch(root->type)
    {
        case(EXPR_ASSIGN):
            gen_expr_assign(root,tab);
            break;

        case(EXPR_FCALL):
        case(EXPR_PAR):
        case(EXPR):
            gen_expr(root,'F');
            break;

        case(FDEF):
            gen_fdef(root,tab);
            break;

        case(RETURN_N):
            gen_return(root);
            break;

        case(IF_N):
            gen_if(root,tab);
        default:
            return;
    }
}

void gen_expr_assign(ast_node* root,struct bintree_node* tab)
{
    char* ID = root->children[0]->attrib;
    struct bintree_node* var = bintree_delete_by_key(tab, ID);
    if(!var->node_data->codegen_was_def)
    {
        var->node_data->codegen_was_def = 1; 
        printf("DEFVAR LF@%s\n",ID);
    }
    gen_expr(root->children[1],'F');
    printf("MOVE LF@%s GF@_EF\n",ID);

}

void gen_fcall(ast_node* root)
{
    char* fid = root->attrib;
    printf("# function call : %s\n",fid);
    printf("# function call prolog\n");
    printf("PUSHS GF@_EF\n");
    printf("PUSHS GF@_EL\n");
    printf("PUSHS GF@_ER\n");
    printf("PUSHS GF@_EBL\n");
    printf("PUSHS GF@_EBR\n");

    if(root->children_cnt > 0 )
    {
        if(root->children[0]->type == EXPR_LIST)
        {
            root = root->children[0];
        }
    }

    putchar('\n');
    for(size_t n = root->children_cnt; n > 0; --n)
    {
        printf("# generating resault of expr for arg[%lu]\n",n);
        gen_expr(root->children[n-1],'F');
        putchar('\n');
        printf("# pushing arg[%lu]\n",n);
        printf("PUSHS GF@_EF\n"); 
    }
    
    putchar('\n');
    printf("# calling %s\n",fid);
    printf("CALL $$%s\n", fid);
    printf("# extracting resault into GF@_FF\n");
    printf("POPS GF@_FF\n");
    putchar('\n');
    
    printf("# function call epilog\n");
    printf("POPS GF@_EBR\n");
    printf("POPS GF@_EBL\n");
    printf("POPS GF@_ER\n");
    printf("POPS GF@_EL\n");
    printf("POPS GF@_EF\n");
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

    if(root->children_cnt > 0 && root->type != EXPR_FCALL)
    {
        gen_expr(root->children[0],'L');
        gen_expr(root->children[1],'R');
    }

    switch(root->sub_type)
    {
        case(FID):
            gen_fcall(root); 
            printf("# move resault of function call in correct register\n");
            printf("MOVE GF@_E%c GF@_FF\n",side);
            return;

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
            printf("\n\n??? %s:%s ???\n\n",token_str(root->sub_type),root->attrib);
            return;
    
    }
    
    printf(" GF@_E%c GF@_EL GF@_ER\n",side);
    putchar('\n');
}
==== BASE ====
