#include<stdio.h>

// function reads() : ?string
void gen_reads()
{
    printf
    (
        "LABEL $$reads\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_out\n"
        "READ LF@_out string\n"
        "PUSHS LF@_out\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}


// function readi() : ?int
void gen_readi()
{
    printf
    (
        "LABEL $$readi\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_out\n"
        "READ LF@_out int\n"
        "PUSHS LF@_out\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );

}


// function write(<n>, term1, term2, ..., termn) : void
void gen_write()
{
    printf
    (
        "LABEL $$write\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_n\n"
        "DEFVAR LF@_i\n"
        "DEFVAR LF@_current\n"
        "\n"
        "POPS LF@_n\n"
        "MOVE LF@_i int@0\n"
        "\n"
        "LABEL $$write!loop\n"
        "JUMPIFEQ $$write!success LF@_i LF@_n\n"
        "\n"
        "POPS LF@_current\n"
        "WRITE LF@_current\n"
        "ADD LF@_i LF@_i int@1\n"
        "\n"
        "JUMP $$write!loop\n"
        "\n"
        "LABEL $$write!success\n"
        "POPFRAME\n"
        "PUSHS nil@nil\n"
        "RETURN\n"
    );
}

// function substring(string $str, int $i, int $j) : ?string
void gen_substring()
{
    printf
    (
        "LABEL $$substring\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_str\n"
        "DEFVAR LF@_i\n"
        "DEFVAR LF@_j\n"
        "\n"
        "POPS LF@_str\n"
        "POPS LF@_i\n"
        "POPS LF@_j\n"
        "\n"
        "DEFVAR LF@_cmp\n"
        "LT LF@_cmp LF@_i int@0\n"
        "JUMPIFEQ $$substring!error LF@_cmp bool@true\n"
        "\n"
        "LT LF@_cmp LF@_j int@0\n"
        "JUMPIFEQ $$substring!error LF@_cmp bool@true\n"
        "\n"
        "GT LF@_cmp LF@_i LF@_j\n"
        "JUMPIFEQ $$substring!error LF@_cmp bool@true\n"
        "\n"
        "DEFVAR LF@_len\n"
        "STRLEN LF@_len LF@_str\n"
        "\n"
        "LT LF@_cmp LF@_i LF@_len\n"
        "NOT LF@_cmp LF@_cmp\n"
        "JUMPIFEQ $$substring!error LF@_cmp bool@true\n"
        "\n"
        "LT LF@_cmp LF@_j LF@_len\n"
        "NOT LF@_cmp LF@_cmp\n"
        "JUMPIFEQ $$substring!error LF@_cmp bool@true\n"
        "\n"
        "DEFVAR LF@_out\n"
        "DEFVAR LF@_current\n"
        "\n"
        "MOVE LF@_out string@\n"
        "\n"
        "LABEL $$substring!loop\n"
        "JUMPIFEQ $$substring!success LF@_i LF@_j\n"
        "\n"
        "GETCHAR LF@_current LF@_str LF@_i\n"
        "CONCAT LF@_out LF@_out LF@_current\n"
        "ADD LF@_i LF@_i int@1\n"
        "\n"
        "JUMP $$substring!loop\n"
        "\n"
        "LABEL $$substring!success\n"
        "PUSHS LF@_out\n"
        "JUMP $$substring!exit\n"
        "\n"
        "LABEL $$substring!error\n"
        "PUSHS nil@nil\n"
        "\n"
        "LABEL $$substring!exit\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}

// function ord(string $c) : int
void gen_ord()
{
    printf
    (
        "LABEL $$ord\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_c\n"
        "POPS LF@_c\n"
        "\n"
        "DEFVAR LF@_len\n"
        "STRLEN LF@_len LF@_c\n"
        "\n"
        "DEFVAR LF@_cmp\n"
        "EQ LF@_cmp LF@_len int@0\n"
        "JUMPIFEQ $$ord!error LF@_cmp bool@true\n"
        "\n"
        "STRI2INT LF@_c LF@_c int@0\n"
        "PUSHS LF@_c\n"
        "\n"
        "JUMP $$ord!exit\n"
        "\n"
        "LABEL $$ord!error\n"
        "PUSHS int@0\n"
        "\n"
        "LABEL $$ord!exit\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}




// function chr(int $i) : string
void gen_chr()
{
    printf
    (
        "LABEL $$chr\n"
        "CREATEFRAME\n"
        "PUSHFRAME\n"
        "\n"
        "DEFVAR LF@_i\n"
        "POPS LF@_i\n"
        "\n"
        "INT2CHAR LF@_i LF@_i\n"
        "PUSHS LF@_i\n"
        "\n"
        "POPFRAME\n"
        "RETURN\n"
    );
}


void gen_built_ins()
{
    gen_chr();
    gen_ord();
    gen_substring();
    gen_write();
    gen_readi();
    gen_reads();
}
