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
static unsigned long long int if_body_id = 0;
static unsigned long long int while_id = 0;
static unsigned long long int B2I_id = 0;
static unsigned long long int B2F_id = 0;
static unsigned long long int B2S_id = 0;
static unsigned long long int S2B_id = 0;

//forward declarations
void gen_expr(ast_node* root);
void gen_expr_assign(ast_node* root,struct bintree_node* tab);
void gen_fcall(ast_node* root); 
void gen_main(ast_node* root, struct bintree_node* tab);
void gen_convert(token_type oper);


// one parameter that will be converted
void gen_CONVERT_X_TO_BOOL()
{
    printf
    (
        "LABEL $$$_CONVERT_X_TO_BOOL\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_VAL\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_VAL\n"
        "TYPE LF@_TYPE LF@_VAL\n"
        "\n"
        "JUMPIFEQ $$$_INT_TO_BOOL LF@_TYPE string@int\n"
        "JUMPIFEQ $$$_FLOAT_TO_BOOL LF@_TYPE string@float\n"
        "JUMPIFEQ $$$_LOCAL_NIL_TO_BOOL LF@_TYPE string@nil\n"
        "JUMPIFEQ $$$_STRING_TO_BOOL LF@_TYPE string@string\n"
            "PUSHS LF@_VAL\n"
            "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "\n"
        "LABEL $$$_INT_TO_BOOL\n"
            "JUMPIFEQ $$$_INT_TO_BOOL_FALSE LF@_VAL int@0\n"
                "PUSHS bool@true\n"
                "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
            "LABEL $$$_INT_TO_BOOL_FALSE\n"
                "PUSHS bool@false\n"
            "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "\n"
        "LABEL $$$_FLOAT_TO_BOOL\n"
            "JUMPIFEQ $$$_FLOAT_TO_BOOL_FALSE LF@_VAL float@0x0p+0\n"
                "PUSHS bool@true\n"
                "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
            "LABEL $$$_FLOAT_TO_BOOL_FALSE\n"
                "PUSHS bool@false\n"
            "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "\n"
        "LABEL $$$_LOCAL_NIL_TO_BOOL\n"
            "PUSHS bool@false\n"
            "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "\n"
        "LABEL $$$_STRING_TO_BOOL\n"
            "JUMPIFEQ $$$_STRING_TO_BOOL_FALSE LF@_VAL string@\n"
                "PUSHS bool@true\n"
                "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
            "LABEL $$$_STRING_TO_BOOL_FALSE\n"
                "PUSHS bool@false\n"
            "JUMP $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_BOOL_SKIP\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

void gen_CONVERT_X_TO_STRING()
{
    printf
    (
        "LABEL $$$_CONVERT_X_TO_STRING\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_VAL\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_VAL\n"
        "TYPE LF@_TYPE LF@_VAL\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_STRING_NIL LF@_TYPE string@nil\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_STRING_BOOL LF@_TYPE string@bool\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_STRING_END LF@_TYPE string@string\n"
        "EXIT int@7\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_STRING_NIL\n"
        "MOVE LF@_VAL string@\n"
        "JUMP $$$_CONVERT_X_TO_STRING_END\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_STRING_BOOL\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_STRING_BOOL_FALSE LF@_VAL bool@false\n"
        "\n"
        "MOVE LF@_VAL string@1\n"
        "\n"
        "JUMP $$$_CONVERT_X_TO_STRING_END\n"
        "LABEL $$$_CONVERT_X_TO_STRING_BOOL_FALSE\n"
        "\n"
        "MOVE LF@_VAL string@\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_STRING_END\n"
        "PUSHS LF@_VAL\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
        );
}

void gen_CONVERT_X_TO_INT()
{
    printf
        (
        "LABEL $$$_CONVERT_X_TO_INT\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_VAL\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_VAL\n"
        "TYPE LF@_TYPE LF@_VAL\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_INT_NIL LF@_TYPE string@nil\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_INT_END LF@_TYPE string@int\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_INT_FLOAT LF@_TYPE string@float\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_INT_BOOL LF@_TYPE string@bool\n"
        "EXIT int@7\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_INT_NIL\n"
        "MOVE LF@_VAL int@0\n"
        "JUMP $$$_CONVERT_X_TO_INT_END\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_INT_FLOAT\n"
        "FLOAT2INT LF@_VAL LF@_VAL\n"
        "JUMP $$$_CONVERT_X_TO_INT_END\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_INT_BOOL\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_INT_BOOL_FALSE LF@_VAL bool@false\n"
        "MOVE LF@_VAL int@1\n"
        "\n"
        "JUMP $$$_CONVERT_X_TO_INT_END\n"
        "LABEL $$$_CONVERT_X_TO_INT_BOOL_FALSE\n"
        "MOVE LF@_VAL int@0\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_INT_END\n"
        "PUSHS LF@_VAL\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
        );
}

void gen_CONVERT_TO_NUMERIC()
{
    printf
        (
        "LABEL $$$_CONVERT_TO_NUMERIC\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_L\n"
        "DEFVAR LF@_R\n"
        "DEFVAR LF@_LT\n"
        "DEFVAR LF@_RT\n"
        "POPS LF@_L\n"
        "POPS LF@_R\n"
        "TYPE LF@_LT LF@_L\n"
        "TYPE LF@_RT LF@_R\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_TO_NUMERIC_RIGHT_FLOAT LF@_LT string@float\n"
        "JUMPIFEQ $$$_CONVERT_TO_NUMERIC_LEFT_FLOAT LF@_RT string@float\n"
        "JUMPIFEQ $$$_CONVERT_TO_NUMERIC_RIGHT_INT LF@_LT string@int\n"
        "JUMPIFEQ $$$_CONVERT_TO_NUMERIC_LEFT_INT LF@_RT string@int\n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_L\n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_R\n"
        "JUMP $$$_CONVERT_TO_NUMERIC_END\n"
        "\n"
        "\n"
        "LABEL $$$_CONVERT_TO_NUMERIC_RIGHT_INT\n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_INT\n"
        "POPS LF@_R\n"
        "JUMP $$$_CONVERT_TO_NUMERIC_END\n"
        "\n"
        "LABEL $$$_CONVERT_TO_NUMERIC_RIGHT_FLOAT\n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_R\n"
        "JUMP $$$_CONVERT_TO_NUMERIC_END\n"
        "\n"
        "LABEL $$$_CONVERT_TO_NUMERIC_LEFT_INT\n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_INT\n"
        "POPS LF@_L\n"
        "JUMP $$$_CONVERT_TO_NUMERIC_END\n"
        "\n"
        "LABEL $$$_CONVERT_TO_NUMERIC_LEFT_FLOAT\n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_L\n"
        "\n"
        "LABEL $$$_CONVERT_TO_NUMERIC_END\n"
        "PUSHS LF@_R\n"
        "PUSHS LF@_L\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}


void gen_CONVERT_X_TO_FLOAT()
{
    printf
        (
        "LABEL $$$_CONVERT_X_TO_FLOAT\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_VAL\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_VAL\n"
        "TYPE LF@_TYPE LF@_VAL\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_FLOAT_NIL LF@_TYPE string@nil\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_FLOAT_INT LF@_TYPE string@int\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_FLOAT_END LF@_TYPE string@float\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_FLOAT_BOOL LF@_TYPE string@bool\n"
        "EXIT int@7\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_FLOAT_NIL\n"
        "MOVE LF@_VAL float@0x0p+0\n"
        "JUMP $$$_CONVERT_X_TO_FLOAT_END\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_FLOAT_INT\n"
        "INT2FLOAT LF@_VAL LF@_VAL\n"
        "JUMP $$$_CONVERT_X_TO_FLOAT_END\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_FLOAT_BOOL\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_FLOAT_BOOL_FALSE LF@_VAL bool@false\n"
        "MOVE LF@_VAL float@0x1p+0\n"
        "\n"
        "JUMP $$$_CONVERT_X_TO_FLOAT_END\n"
        "LABEL $$$_CONVERT_X_TO_FLOAT_BOOL_FALSE\n"
        "MOVE LF@_VAL float@0x0p+0\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_FLOAT_END\n"
        "PUSHS LF@_VAL\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
        );
}

void gen_CONVERT_NIL_TO_X()
{
    printf
    (
        "LABEL $$$_CONVERT_NIL\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_TYPE\n"
        "\n"
        "JUMPIFEQ $$$_NIL_TO_INT LF@_TYPE string@int\n"
        "JUMPIFEQ $$$_NIL_TO_FLOAT LF@_TYPE string@float\n"
        "JUMPIFEQ $$$_NIL_TO_BOOL LF@_TYPE string@bool\n"
        "JUMPIFEQ $$$_NIL_TO_STRING LF@_TYPE string@string\n"
        "PUSHS nil@nil\n"
        "JUMP $$$_NIL_TO_SKIP\n"
        "\n"
        "LABEL $$$_NIL_TO_INT\n"
        "PUSHS int@0\n"
        "JUMP $$$_NIL_TO_SKIP\n"
        "\n"
        "LABEL $$$_NIL_TO_FLOAT\n"
        "PUSHS float@0x0p+0\n"
        "JUMP $$$_NIL_TO_SKIP\n"
        "\n"
        "LABEL $$$_NIL_TO_BOOL\n"
        "PUSHS bool@false\n"
        "JUMP $$$_NIL_TO_SKIP\n"
        "\n"
        "LABEL $$$_NIL_TO_STRING\n"
        "PUSHS string@\n"
        "JUMP $$$_NIL_TO_SKIP\n"
        "\n"
        "LABEL $$$_NIL_TO_SKIP\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

void gen_CONVERT_X_TO_NILL()
{
    printf
    (
        "LABEL $$$_CONVERT_X_TO_NIL\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_VAL\n"
        "DEFVAR LF@_TYPE\n"
        "POPS LF@_VAL\n"
        "TYPE LF@_TYPE LF@_TYPE\n"
        "\n"
        "JUMPIFEQ $$$_CONVERT_X_TO_NIL_RETURN LF@_TYPE string@nil\n"
        "EXIT int@7\n"
        "\n"
        "LABEL $$$_CONVERT_X_TO_NIL_RETURN\n"
        "PUSHS LF@_VAL\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}
void gen_EQEQ()
{
    printf
    (
        "LABEL $$$_EQEQ\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_LEFT\n"
        "DEFVAR LF@_RIGHT\n"
        "DEFVAR LF@_LEFT_TYPE\n"
        "DEFVAR LF@_RIGHT_TYPE\n"
        "POPS LF@_LEFT\n"
        "POPS LF@_RIGHT\n"
        "TYPE LF@_LEFT_TYPE LF@_LEFT\n"
        "TYPE LF@_RIGHT_TYPE LF@_RIGHT\n"
        "\n"
        "JUMPIFEQ $$$_EQEQ_LEFT_NIL LF@_LEFT_TYPE string@nil\n"
        "JUMPIFEQ $$$_EQEQ_RIGHT_NIL LF@_RIGHT_TYPE string@nil\n"
        "PUSHS LF@_RIGHT\n"
        "PUSHS LF@_LEFT\n"
        "CALL $$$_DYN_CONVERT\n"
        "POPS LF@_LEFT\n"
        "POPS LF@_RIGHT\n"
        "PUSHS LF@_LEFT\n"
        "PUSHS LF@_RIGHT\n"
        "EQS\n"
        "JUMP $$$_EQEQ_SKIP\n"
        "\n"
        "LABEL $$$_EQEQ_LEFT_NIL\n"
        "JUMPIFEQ $$$_EQEQ_BOTH_NIL LF@_RIGHT_TYPE string@nil\n"
        "PUSHS LF@_RIGHT_TYPE\n"
        "CALL $$$_CONVERT_NIL\n"
        "PUSHS LF@_RIGHT\n"
        "EQS\n"
        "JUMP $$$_EQEQ_SKIP\n"
        "\n"
        "LABEL $$$_EQEQ_RIGHT_NIL\n"
        "JUMPIFEQ $$$_EQEQ_BOTH_NIL LF@_LEFT_TYPE string@nil\n"
        "PUSHS LF@_LEFT_TYPE\n"
        "CALL $$$_CONVERT_NIL\n"
        "PUSHS LF@_LEFT\n"
        "EQS\n"
        "JUMP $$$_EQEQ_SKIP\n"
        "\n"
        "LABEL $$$_EQEQ_BOTH_NIL\n"
        "PUSHS bool@true\n"
        "\n"
        "LABEL $$$_EQEQ_SKIP\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

void gen_REL_LT()
{
    printf
    (
        "LABEL $$$_REL_LT\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "DEFVAR LF@_LEFT\n"
        "DEFVAR LF@_RIGHT\n"
        "DEFVAR LF@_RESAULT\n"
        "DEFVAR LF@_LEFT_TYPE\n"
        "DEFVAR LF@_RIGHT_TYPE\n"
        "POPS LF@_RIGHT\n"
        "POPS LF@_LEFT\n"
        "TYPE LF@_LEFT_TYPE LF@_LEFT\n"
        "TYPE LF@_RIGHT_TYPE LF@_RIGHT\n"
        "\n"
        "JUMPIFEQ $$$_REL_LT_FALSE LF@_LEFT_TYPE string@nil\n"
        "JUMPIFEQ $$$_REL_LT_FALSE LF@_RIGHT_TYPE string@nil\n"
        "PUSHS LF@_RIGHT\n"
        "PUSHS LF@_LEFT\n"
        "CALL $$$_DYN_CONVERT\n"
        "POPS LF@_LEFT\n"
        "POPS LF@_RIGHT\n"
        "LT LF@_RESAULT LF@_LEFT LF@_RIGHT\n"
        "JUMP $$$_REL_LT_SKIP\n"
        "\n"
        "LABEL $$$_REL_LT_FALSE\n"
        "MOVE LF@_RESAULT bool@false\n"
        "LABEL $$$_REL_LT_SKIP\n"
        "PUSHS LF@_RESAULT\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

// LEFT, RIGH
void gen_DYN_CONVERT()
{
    printf
        (
        "LABEL $$$_DYN_CONVERT\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_L\n"
        "DEFVAR LF@_R\n"
        "DEFVAR LF@_LT\n"
        "DEFVAR LF@_RT\n"
        "POPS LF@_L\n"
        "POPS LF@_R\n"
        "TYPE LF@_LT LF@_L\n"
        "TYPE LF@_RT LF@_R\n"
        "\n"
        "JUMPIFEQ $$$_DYN_CONVERT_RTS LF@_LT string@string\n"
        "JUMPIFEQ $$$_DYN_CONVERT_LTS LF@_RT string@string\n"
        "JUMPIFEQ $$$_DYN_CONVERT_RTF LF@_LT string@float\n"
        "JUMPIFEQ $$$_DYN_CONVERT_LTF LF@_RT string@float\n"
        "JUMPIFEQ $$$_DYN_CONVERT_RTI LF@_LT string@int\n"
        "JUMPIFEQ $$$_DYN_CONVERT_LTI LF@_RT string@int\n"
        "JUMPIFEQ $$$_DYN_CONVERT_RTB LF@_LT string@bool\n"
        "JUMPIFEQ $$$_DYN_CONVERT_LTB LF@_RT string@bool\n"
        "JUMPIFEQ $$$_DYN_CONVERT_RTN LF@_LT string@nil\n"
        "JUMPIFEQ $$$_DYN_CONVERT_LTN LF@_RT string@nil\n"
        "EXIT int@7\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_RTB \n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_BOOL\n"
        "POPS LF@_R\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_LTB \n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_BOOL\n"
        "POPS LF@_L\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_LTF \n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_L\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_RTF \n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_FLOAT\n"
        "POPS LF@_R\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_LTI \n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_INT\n"
        "POPS LF@_L\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_RTI \n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_INT\n"
        "POPS LF@_R\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_LTS \n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_STRING\n"
        "POPS LF@_L\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_RTS \n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_STRING\n"
        "POPS LF@_R\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_LTN \n"
        "PUSHS LF@_L\n"
        "CALL $$$_CONVERT_X_TO_NIL\n"
        "POPS LF@_L\n"
        "JUMP $$$_DYN_CONVERT_END\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_RTN \n"
        "PUSHS LF@_R\n"
        "CALL $$$_CONVERT_X_TO_NIL\n"
        "POPS LF@_R\n"
        "\n"
        "LABEL $$$_DYN_CONVERT_END\n"
        "PUSHS LF@_R\n"
        "PUSHS LF@_L\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
        );
}


void gen_REL_GT()
{
    printf
    (
        "LABEL $$$_REL_GT\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "DEFVAR LF@_LEFT\n"
        "DEFVAR LF@_RIGHT\n"
        "DEFVAR LF@_RESAULT\n"
        "DEFVAR LF@_LEFT_TYPE\n"
        "DEFVAR LF@_RIGHT_TYPE\n"
        "POPS LF@_RIGHT\n"
        "POPS LF@_LEFT\n"
        "TYPE LF@_LEFT_TYPE LF@_LEFT\n"
        "TYPE LF@_RIGHT_TYPE LF@_RIGHT\n"
        "\n"
        "JUMPIFEQ $$$_REL_GT_FALSE LF@_LEFT_TYPE string@nil\n"
        "JUMPIFEQ $$$_REL_GT_FALSE LF@_RIGHT_TYPE string@nil\n"
        "PUSHS LF@_RIGHT\n"
        "PUSHS LF@_LEFT\n"
        "CALL $$$_DYN_CONVERT\n"
        "POPS LF@_LEFT\n"
        "POPS LF@_RIGHT\n"
        "GT LF@_RESAULT LF@_LEFT LF@_RIGHT\n"
        "JUMP $$$_REL_GT_SKIP\n"
        "\n"
        "LABEL $$$_REL_GT_FALSE\n"
        "MOVE LF@_RESAULT bool@false\n"
        "LABEL $$$_REL_GT_SKIP\n"
        "PUSHS LF@_RESAULT\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

void gen_STRCAT()
{
    printf
    (
        "LABEL $$$_STRCATENATE\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_R\n"
        "DEFVAR LF@_L\n"
        "\n"
        "CALL $$$_CONVERT_X_TO_STRING\n"
        "POPS LF@_L\n"
        "\n"
        "CALL $$$_CONVERT_X_TO_STRING\n"
        "POPS LF@_R\n"
        "\n"
        "CONCAT LF@_L LF@_L LF@_R\n"
        "PUSHS LF@_L\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

void gen_DO_DIV()
{
printf
    (
        "LABEL $$$_DO_DIV\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_L\n"
        "DEFVAR LF@_R\n"
        "DEFVAR LF@_T\n"
        "POPS LF@_L\n"
        "POPS LF@_R\n"
        "TYPE LF@_T LF@_L\n"
        "\n"
        "JUMPIFEQ $$$_DO_DIV_INT LF@_T string@int\n"
        "JUMPIFEQ $$$_DO_DIV_FLOAT LF@_T string@float\n"
        "\n"
        "LABEL $$$_DO_DIV_INT\n"
        "JUMPIFEQ $$$_DO_DIV_DIV_BY_ZERO LF@_R int@0\n"
        "IDIV LF@_L LF@_L LF@_R\n"
        "JUMP $$$_DO_DIV_END\n"
        "\n"
        "LABEL $$$_DO_DIV_FLOAT\n"
        "JUMPIFEQ $$$_DO_DIV_DIV_BY_ZERO LF@_R float@0x0p+0\n"
        "DIV LF@_L LF@_L LF@_R\n"
        "\n"
        "LABEL $$$_DO_DIV_END\n"
        "PUSHS LF@_L\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
        "\n"
        "LABEL $$$_DO_DIV_DIV_BY_ZERO\n"
        "EXIT int@8\n"
    );

}

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
    gen_CONVERT_NIL_TO_X();
    gen_EQEQ();
    gen_REL_GT();
    gen_REL_LT();
    gen_CONVERT_X_TO_NILL();
    gen_CONVERT_X_TO_BOOL();
    gen_CONVERT_X_TO_STRING();
    gen_CONVERT_X_TO_INT();
    gen_CONVERT_X_TO_FLOAT();
    gen_DYN_CONVERT();
    gen_CONVERT_TO_NUMERIC();
    gen_DO_DIV();
    gen_STRCAT();
    gen_built_ins();
    printf("LABEL $$$PROGRAM_END\n");
    printf("POPFRAME\n");
    printf("CLEARS\n");
    
}

void gen_init_locals(struct bintree_node* tab)
{
    if(tab == NULL)
    {
        return;
    }
    
    for(size_t n = 0; n < tab->node_data->vars_cnt; ++n)
    {
        struct bintree_node* var = bintree_search_by_id(tab->local_symtab,n);
        char* ID = var->node_data->key;
        if(!var->node_data->codegen_was_def)
        {
            printf("DEFVAR LF@%s\n",ID);
            printf("MOVE LF@%s nil@nil\n",ID);
            var->node_data->codegen_was_def = true;
        }
    }

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
            char* ID = pars->children[n]->attrib;
            struct bintree_node* var = bintree_search_by_key(table->local_symtab, ID);
            var->node_data->codegen_was_def = true; 
            printf("DEFVAR LF@%s\n",ID);
            printf("POPS LF@%s\n",pars->children[n]->attrib);
        }
    }
    
    gen_init_locals(table);    

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

    gen_init_locals(GLOBAL);
    
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
    unsigned long long int if_body_id_sample = if_body_id++;
    printf("#============================================== IF START\n");
    gen_expr(root->children[0]);
    printf("CALL $$$_CONVERT_X_TO_BOOL\n");
    
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS $$IF_ELSE%llu\n",if_body_id_sample);
   
    ast_node* true_body = root->children[1];
    for(size_t n = 0; n < true_body->children_cnt; ++n)
    {
        gen_main(true_body->children[n], tab);
        putchar('\n');
    }
   
    
    printf("JUMP $$IF_SKIP%llu\n",if_body_id_sample);
    printf("LABEL $$IF_ELSE%llu\n",if_body_id_sample);
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
    
    printf("LABEL $$IF_SKIP%llu\n",if_body_id_sample);
    printf("#============================================== IF END\n");
}

void gen_while(ast_node* root,struct bintree_node* tab)
{
    unsigned long long int while_id_sample = while_id++;
    printf("LABEL $$WHILE_START%llu\n",while_id_sample);
    
    gen_expr(root->children[0]);
    printf("CALL $$$_CONVERT_X_TO_BOOL\n");
    
    // todo convert
    printf("PUSHS bool@false\n");
    printf("JUMPIFEQS $$WHILE_EXIT%llu\n",while_id_sample);
   
    ast_node* body = root->children[1];
    for(size_t n = 0; n < body->children_cnt; ++n)
    {
        gen_main(body->children[n], tab);
        putchar('\n');
    }
   
    printf("JUMP $$WHILE_START%llu\n",while_id_sample);
    printf("LABEL $$WHILE_EXIT%llu\n",while_id_sample);
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

    // if variadic function push children count
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

void gen_cmp()
{
    static unsigned long long int CMP_ID=0;

    unsigned long long int CMP_ID_SAMPLE=CMP_ID++;
    printf("POPS GF@_R2\n");
    printf("POPS GF@_R1\n");
    
    printf("PUSHS GF@_R1\n");
    printf("PUSHS GF@_R2\n");
   
    printf("TYPE GF@_R1 GF@_R1\n");
    printf("TYPE GF@_R2 GF@_R2\n");
   
    printf("JUMPIFEQ $$$_COMPARE%llu GF@_R1 GF@_R2\n",CMP_ID_SAMPLE);
    
    printf("POPS GF@_STACK_DUMP\n");
    printf("POPS GF@_STACK_DUMP\n");

    printf("PUSHS bool@false\n");

    printf("JUMP $$$_COMPARE_SKIP%llu\n",CMP_ID_SAMPLE);
    printf("LABEL $$$_COMPARE%llu\n",CMP_ID_SAMPLE);
    
    printf("EQS\n");
    
    printf("LABEL $$$_COMPARE_SKIP%llu\n",CMP_ID_SAMPLE);
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
        //gen_convert(root->sub_type);
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
            printf("PUSHS nil@nil\n");
            break;

        case(ADD):
            printf("CALL $$$_CONVERT_TO_NUMERIC\n");
            printf("ADDS\n");
            break;
        
        case(SUB):
            printf("CALL $$$_CONVERT_TO_NUMERIC\n");
            printf("SUBS\n");
            break;

        case(MUL):
            printf("CALL $$$_CONVERT_TO_NUMERIC\n");
            printf("MULS\n");
            break;
            
        case(DIV):
        case(IDIV):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("PUSHS GF@_R1\n");
            printf("CALL $$$_CONVERT_TO_NUMERIC\n");
            printf("CALL $$$_DO_DIV\n");
            break;

        case(STRCAT):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            printf("PUSHS GF@_R1\n");
            printf("CALL $$$_STRCATENATE\n");
            //printf("POPS GF@_R2\n");
            //printf("POPS GF@_R1\n");
            //printf("CONCAT GF@_R1 GF@_R1 GF@_R2\n");
            //printf("PUSHS GF@_R1\n");
            break;
        
        case(EQ):
            //printf("EQS");
            gen_cmp();
            break;

        case(LT):
            printf("CALL $$$_REL_LT\n");
            break;
        
        case(GT):
            printf("CALL $$$_REL_GT\n");
            break;
        
        case(LTE):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            
            printf("CALL $$$_REL_LT\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            
            printf("CALL $$$_EQEQ\n");
            
            printf("ORS\n");
            break;
        
        case(GTE):
            printf("POPS GF@_R2\n");
            printf("POPS GF@_R1\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            
            printf("CALL $$$_REL_GT\n");
            
            printf("PUSHS GF@_R1\n");
            printf("PUSHS GF@_R2\n");
            
            printf("CALL $$$_EQEQ\n");
            
            printf("ORS\n");
            break;
        
        case(NEQ):
            //printf("EQS\n");
            gen_cmp();
            printf("NOTS\n");
            break;
        
        default:
            printf("EXIT int@2\n");
            return;
    }
    
    putchar('\n');
}

void gen_convert(token_type oper)
{
    printf("#===================== convert : %s\n",token_str(oper));
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
