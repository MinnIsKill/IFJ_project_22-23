/**
 * @brief Generate IFJcode22 code from abstract syntax tree
 * @file codegen.c
 * @author Jan Lutonský, xluton02
 *
 **/
#include<stdio.h>
#include<errno.h>
#include"codegen.h"
#include"ast.h"
#include"token.h"

#include"built_in_functions.c"

static struct bintree_node* GLOBAL;

//forward declarations
void gen_expr(ast_node* root);
void gen_expr_assign(ast_node* root,struct bintree_node* tab);
void gen_fcall(ast_node* root); 
void gen_main(ast_node* root, struct bintree_node* tab);
void gen_convert(token_type oper);

void gen_prolog()
{
    printf(".IFJcode22\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR GF@_STACK_DUMP\n");
    printf("DEFVAR GF@_R1\n");
    printf("DEFVAR GF@_R2\n");

    printf("MOVE GF@_STACK_DUMP  nil@nil\n");
    printf("MOVE GF@_R1  nil@nil\n");
    printf("MOVE GF@_R2  nil@nil\n");
    printf("#===================== END OF PROLOG -> START OF MAIN\n\n"); 

}

void gen_epilog()
{
    printf("\n\n#===================== EPILOG\n"); 
    printf("JUMP $$$PROGRAM_END\n");
    gen_built_ins();
    printf("LABEL $$$PROGRAM_END\n");
    printf("POPFRAME\n");
    printf("CLEARS\n");
    
}


void gen_fdef(ast_node* root, struct bintree_node* tab)
{
    (void)tab;
    char* fid = root->attrib;
    struct bintree_node* table = bintree_search_by_key(GLOBAL, fid);
    printf("JUMP $$%s_SKIP\n",fid);
    printf("#================================ FUN DEF : %s\n",fid);
    printf("LABEL $$%s\n",fid);


    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    
    //for each var in par_list def var + pop value in it

    if(root->children_cnt > 2)
    {
        ast_node* pars = root->children[0];
        for(size_t n = 0; n < pars->children_cnt; ++n)
        {
            printf("DEFVAR LF@%s\n",pars->children[n]->attrib);
            printf("POPS LF@%s\n",pars->children[n]->attrib);
        }
    }

    putchar('\n');

    if(root->children_cnt > 2)
    {
        root = root->children[2];
    }
    else
    {
        root = root->children[1];
    }
    for(size_t n = 0; n < root->children_cnt; ++n)
    {
        gen_main(root->children[n], table->local_symtab);
        putchar('\n');
    }

    
    printf("POPFRAME\n");
    printf("RETURN\n");

    printf("#================================ FUN DEF : %s\n",fid);
    printf("LABEL $$%s_SKIP\n",fid);
}


void codegen(context* cont)
{
    ast_node* root = cont->root;
    struct bintree_node* tab = bintree_search_by_key(cont->global_symtab, ":b");
    GLOBAL = tab;
    tab = tab->local_symtab;
    
    gen_prolog();
    putchar('\n');


    // skip root
    root = root->children[0];
    for(size_t n = 0; n < root->children_cnt; ++n)
    {
        gen_main(root->children[n], tab);
        printf("CLEARS\n");
        putchar('\n');
    }

    gen_epilog();
}
            
void gen_return(ast_node* root)
{
    if(root->children_cnt > 0)
    {
        gen_expr(root->children[0]);    
    }
}
        
void gen_if(ast_node* root,struct bintree_node* tab)
{
    printf("#============================================== IF START\n");
    static unsigned long long int if_body_id = 0;
    gen_expr(root->children[0]);
    
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS $$IF_ELSE%llu\n",if_body_id);
   
    ast_node* true_body = root->children[1];
    for(size_t n = 0; n < true_body->children_cnt; ++n)
    {
        gen_main(true_body->children[n], tab);
        putchar('\n');
    }
   
    
    printf("JUMP $$IF_SKIP%llu\n",if_body_id);
    printf("LABEL $$IF_ELSE%llu\n",if_body_id);
    printf("#============================================== IF ELSE\n");
   
    putchar('\n');
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
    printf("#============================================== IF END\n");
    if_body_id++;
}

void gen_while(ast_node* root,struct bintree_node* tab)
{
    static unsigned long long int while_id = 0;
    printf("LABEL $$WHILE_START%llu\n",while_id);
    
    gen_expr(root->children[0]);
    
    // todo convert
    printf("PUSHS bool@float\n");
    printf("JUMPIFEQS $$WHILE_EXIT%llu\n",while_id);
   
    ast_node* body = root->children[1];
    for(size_t n = 0; n < body->children_cnt; ++n)
    {
        gen_main(body->children[n], tab);
        putchar('\n');
    }
   
    printf("JUMP $$WHILE_START%llu\n",while_id);
    printf("LABEL $$WHILE_EXIT%llu\n",while_id);
    while_id++;
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
        case(CONVERT_TYPE):
        case(EXPR):
            gen_expr(root);
            break;

        case(FDEF):
            gen_fdef(root,tab);
            break;

        case(RETURN_N):
            gen_return(root);
            break;
        
        case(WHILE_N):
            gen_while(root,tab);
            break;

        case(IF_N):
            gen_if(root,tab);
            break;

        default:
            return;
    }
}

void gen_expr_assign(ast_node* root,struct bintree_node* tab)
{
    char* ID = root->children[0]->attrib;
    struct bintree_node* var = bintree_search_by_key(tab, ID);
    if(!var->node_data->codegen_was_def)
    {
        var->node_data->codegen_was_def = true; 
        printf("DEFVAR LF@%s\n",ID);
    }
    else
    {
    }
    gen_expr(root->children[1]);
    printf("POPS LF@%s\n",ID);
}

void gen_fcall(ast_node* root)
{
    char* fid = root->attrib;
    printf("# function call : %s\n",fid);
    printf("# function call prolog\n");

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
        gen_expr(root->children[n-1]);
        putchar('\n');
    }

    //TODO if variadic function push children count
    struct bintree_node* tab = bintree_search_by_key(GLOBAL,fid);
    if(tab->node_data->variadic_func)
    {
        printf("PUSHS int@%lu\n",root->children_cnt); 
    }
    
    putchar('\n');
    printf("# calling %s\n",fid);
    printf("CALL $$%s\n", fid);
    putchar('\n');
}

void codegen_print_escape(char* str)
{
    for(;*str != '\0';++str)
    {
        if((*str >= 0 && *str <= 32) || *str == 35 || *str == 92)
        {
            printf("\\%03u",(unsigned char)*str);
        }
        else
        {
            putchar(*str);
        }
    }
}

void gen_expr(ast_node* root)
{
    // TODO check if side in {L,F,R}

    if(root == NULL)
    {
        return;
    }
   
    if(root->type == CONVERT_TYPE)
    {
        gen_expr(root->children[0]);
        gen_convert(root->sub_type);
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
        gen_expr(root->children[0]);
        gen_expr(root->children[1]);
    }


    switch(root->sub_type)
    {
        case(FID):
            gen_fcall(root); 
            return;

        case(ID):
            printf("PUSHS LF@%s\n",root->attrib);
            break;

        case(IVAL):
            printf("PUSHS int@%s\n",root->attrib);
            break;

        case(FVAL):
            errno = 0;
            double val = strtod(root->attrib, NULL);
            if(val == 0.0 && errno != 0)
            {
                //TODO handle error, shouldn not happen
                val = 0.0;
            }
            printf("PUSHS float@%a\n",val);
            break;
        
        case(SVAL):
            printf("PUSHS string@");
            codegen_print_escape(root->attrib);
            putchar('\n');
            break;
        
        case(VVAL):
            printf("PUSH nil@nil\n");
            break;

        case(ADD):
            printf("ADDS\n");
            break;
        
        case(SUB):
            printf("SUBS\n");
            break;

        case(MUL):
            printf("MULLS\n");
            break;
            
        case(DIV):
            printf("DIVS\n");
            break;
        
        case(IDIV):
            printf("IDIVS\n");
            break;

        case(STRCAT):
            
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            printf("CONCAT GF@_R1 GF@_R1 GF@_R2\n");
            printf("PUSHS GF@_R1\n");
            break;
        
        case(EQ):
            printf("EQS");
            break;

        case(LT):
            printf("LTS");
            break;
        
        case(GT):
            printf("GTS");
            break;
        
        case(LTE):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("LTS\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("EQS\n");
            
            printf("ORS\n");
            break;
        
        case(GTE):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("GTS\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("EQS\n");
            
            printf("ORS\n");
            break;
        
        case(NEQ):
            printf("EQS\n");
            printf("NOTS\n");
            break;
        
        default:
            //TODO erro
            return;
    }
    
    putchar('\n');
}

void gen_convert(token_type oper)
{
    printf("#===================== convert : %s\n",token_str(oper));
    static unsigned long long int B2I_id = 0;
    static unsigned long long int B2F_id = 0;
    static unsigned long long int B2S_id = 0;
    static unsigned long long int S2B_id = 0;
    switch(oper)
    {
        default:
            break;

        case(NULL_TO_INT):
            printf("POPS GF@_STACK_DUMP\n");
            printf("PUSHS int@0\n");
            break;
            
        case(NULL_TO_FLOAT):
            printf("POPS GF@_STACK_DUMP\n");
            printf("PUSHS float@0x0p+0\n");
            break;

        case(NULL_TO_BOOL):
            printf("POPS GF@_STACK_DUMP\n");
            printf("PUSHS bool@false\n");
            break;

        case(NULL_TO_STR):
            printf("POPS GF@_STACK_DUMP\n");
            printf("PUSHS string@\n");
            break;
        
        case(INT_TO_FLOAT):
            printf("INT2FLOATS\n");
            break;

        case(INT_TO_BOOL):
            printf("PUSHS int@0\n");
            printf("EQS\n");
            printf("NOTS\n");
            break;

        case(FLOAT_TO_INT):
            printf("FLOAT2INTS\n");
            break;

        case(FLOAT_TO_BOOL):
            printf("PUSHS float@0x0p+0\n");
            printf("EQS\n");
            printf("NOTS\n");
            break;

        case(BOOL_TO_INT):
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS $$$$B2I_ZERO%llu\n",B2I_id);
            
            printf("PUSHS int@1\n");

            printf("JUMP $$$$B2I_SKIP%llu\n",B2I_id);
            printf("LABEL $$$$B2I_ZERO%llu\n",B2I_id);

            printf("PUSHS int@0\n");
            
            printf("LABEL $$$$B2I_SKIP%llu\n",B2I_id);
            
            B2I_id++;
            break;


        case(BOOL_TO_FLOAT):
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS $$$$B2F_ZERO%llu\n",B2F_id);
            
            printf("PUSHS float@0x1p+0\n");

            printf("JUMP $$$$B2F_SKIP%llu\n",B2F_id);
            printf("LABEL $$$$B2F_ZERO%llu\n",B2F_id);

            printf("PUSHS float@0x0p+0\n");

            printf("LABEL $$$$B2F_SKIP%llu\n",B2F_id);
            
            B2F_id++;
            break;
        
        case(BOOL_TO_STR):
            printf("PUSHS bool@true\n");
            printf("JUMPIFNEQS $$$$B2S_ZERO%llu\n",B2S_id);
            
            printf("PUSHS string@1\n");

            printf("JUMP $$$$B2S_SKIP%llu\n",B2S_id);
            printf("LABEL $$$$B2S_ZERO%llu\n",B2S_id);

            printf("PUSHS string@\n");

            printf("LABEL $$$$B2S_SKIP%llu\n",B2S_id);
            B2S_id++;
            break;

        case(STR_TO_BOOL):
            printf("PUSHS string@\n");
            printf("JUMPIFNEQS $$$$S2B_ZERO%llu\n",S2B_id);
            
            printf("PUSHS bool@false\n");

            printf("JUMP $$$$S2B_SKIP%llu\n",S2B_id);
            printf("LABEL $$$$S2B_ZERO%llu\n",S2B_id);

            printf("PUSHS bool@true\n");

            printf("LABEL $$$$S2B_SKIP%llu\n",S2B_id);
            S2B_id++;
            break;
    }
    printf("#======================== convert : %s\n",token_str(oper));
}
