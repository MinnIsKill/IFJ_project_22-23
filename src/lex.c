#include "lex.h"

#define LEX_KEYWORD_FUNCTION        "function"
#define LEX_KEYWORD_IF              "if"
#define LEX_KEYWORD_ELSE            "else"
#define LEX_KEYWORD_WHILE           "while"
#define LEX_KEYWORD_RETURN          "return"
#define LEX_KEYWORD_NULL            "null"

#define LEX_TYPE_INT                "int"
#define LEX_TYPE_FLOAT              "float"
#define LEX_TYPE_STRING             "string"
#define LEX_TYPE_VOID               "void"

#define LEX_TYPE_NINT               "?int"
#define LEX_TYPE_NFLOAT             "?float"
#define LEX_TYPE_NSTRING            "?string"

#define LEX_PHP_PS_MARK             "<?php"
#define LEX_PHP_PC_MARK             "declare(strict_types=1);"
#define LEX_PHP_PC_MARK_START       "declare"
#define LEX_PHP_PE_MARK             "?>"

#define LEX_ESC_HEX_LEN             2
#define LEX_ESC_OCT_LEN             3

#define LEX_KEYWORD_CHECK(ctx, value, result, state)        \
        if (char_buffer_equals(&ctx->attrib, value))        \
        {                                                   \
            ctx->token = result;                            \
            return state;                                   \
        }

#define CHAR_BUFFER_ADD(ctx, c)                             \
        if (!char_buffer_add(&ctx->attrib, c))              \
        {                                                   \
            dbgprint("Out of memory!")                      \
            return LEX_STATE_ERROR;                         \
        }

static char lex_esc[LEX_ESC_OCT_LEN + 1]; // Make sure HEX and OCT fit here!
static int lex_esc_len;

lex_state lex_state_start(context* context, FILE* input);

lex_state lex_state_com_0(context* context, FILE* input);
lex_state lex_state_lcom_0(context* context, FILE* input);
lex_state lex_state_bcom_0(context* context, FILE* input);
lex_state lex_state_bcom_1(context* context, FILE* input);

lex_state lex_state_id_0(context* context, FILE* input);
lex_state lex_state_id_1(context* context, FILE* input);
lex_state lex_state_fid_0(context* context, FILE* input);
lex_state lex_state_nid_0(context* context, FILE* input);

lex_state lex_state_ival_0(context* context, FILE* input);
lex_state lex_state_fval_0(context* context, FILE* input);
lex_state lex_state_fval_1(context* context, FILE* input);
lex_state lex_state_fval_2(context* context, FILE* input);
lex_state lex_state_fval_3(context* context, FILE* input);
lex_state lex_state_fval_4(context* context, FILE* input);
lex_state lex_state_sval_0(context* context, FILE* input);
lex_state lex_state_sval_1(context* context, FILE* input);
lex_state lex_state_sval_x(context* context, FILE* input);
lex_state lex_state_sval_o(context* context, FILE* input);

lex_state lex_state_assig_0(context* context, FILE* input);
lex_state lex_state_lt_0(context* context, FILE* input);
lex_state lex_state_gt_0(context* context, FILE* input);
lex_state lex_state_eq_0(context* context, FILE* input);
lex_state lex_state_neq_0(context* context, FILE* input);
lex_state lex_state_neq_1(context* context, FILE* input);

lex_state lex_state_ps_mark_0(context* context, FILE* input);
lex_state lex_state_ps_mark_1(context* context, FILE* input);
lex_state lex_state_pc_mark_0(context* context, FILE* input);
lex_state lex_state_pe_mark_0(context* context, FILE* input);
lex_state lex_state_pe_mark_1(context* context, FILE* input);
lex_state lex_state_pe_mark_2(context* context, FILE* input);

const lex_state_function LEX_TABLE[] =
{
    /* LEX_STATE_START */ lex_state_start,

    /* LEX_STATE_COM_0 */ lex_state_com_0,
    /* LEX_STATE_LCOM_0 */ lex_state_lcom_0,
    /* LEX_STATE_BCOM_0 */ lex_state_bcom_0,
    /* LEX_STATE_BCOM_1 */ lex_state_bcom_1,

    /* LEX_STATE_ID_0 */ lex_state_id_0,
    /* LEX_STATE_ID_1 */ lex_state_id_1,
    /* LEX_STATE_FID_0 */ lex_state_fid_0,
    /* LEX_STATE_NID_0 */ lex_state_nid_0,

    /* LEX_STATE_IVAL_0 */ lex_state_ival_0,
    /* LEX_STATE_FVAL_0 */ lex_state_fval_0,
    /* LEX_STATE_FVAL_1 */ lex_state_fval_1,
    /* LEX_STATE_FVAL_2 */ lex_state_fval_2,
    /* LEX_STATE_FVAL_3 */ lex_state_fval_3,
    /* LEX_STATE_FVAL_4 */ lex_state_fval_4,
    /* LEX_STATE_SVAL_0 */ lex_state_sval_0,
    /* LEX_STATE_SVAL_1 */ lex_state_sval_1,
    /* LEX_STATE_SVAL_X */ lex_state_sval_x,
    /* LEX_STATE_SVAL_O */ lex_state_sval_o,

    /* LEX_STATE_ASSIG_0 */ lex_state_assig_0,
    /* LEX_STATE_LT_0 */ lex_state_lt_0,
    /* LEX_STATE_GT_0 */ lex_state_gt_0,
    /* LEX_STATE_EQ_0 */ lex_state_eq_0,
    /* LEX_STATE_NEQ_0 */ lex_state_neq_0,
    /* LEX_STATE_NEQ_1 */ lex_state_neq_1,

    /* LEX_STATE_PS_MARK_0 */ lex_state_ps_mark_0,
    /* LEX_STATE_PS_MARK_1 */ lex_state_ps_mark_1,
    /* LEX_STATE_PC_MARK_0 */ lex_state_pc_mark_0,
    /* LEX_STATE_PE_MARK_0 */ lex_state_pe_mark_0,
    /* LEX_STATE_PE_MARK_1 */ lex_state_pe_mark_1,
    /* LEX_STATE_PE_MARK_2 */ lex_state_pe_mark_2
};

bool lex_is_whitespace(int c)
{
    return c == ' ' || c == '\t' || c == '\n';
}

bool lex_is_letter_uppercase(int c)
{
    return c >= 'A' && c <= 'Z';
}

bool lex_is_letter_lowercase(int c)
{
    return c >= 'a' && c <= 'z';
}

bool lex_is_letter(int c)
{
    return lex_is_letter_uppercase(c) || lex_is_letter_lowercase(c);
}

bool lex_is_digit(int c)
{
    return c >= '0' && c <= '9';
}

bool lex_is_letter_or_digit(int c)
{
    return lex_is_letter(c) || lex_is_digit(c);
}

bool lex_is_first_identifier(int c)
{
    return lex_is_letter(c) || c == '_';
}

bool lex_is_identifier(int c)
{
    return lex_is_letter_or_digit(c) || c == '_';
}

bool lex_is_hex(int c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
}

bool lex_is_octal(int c)
{
    return c >= '0' && c <= '7';
}

lex_state lex_validate_int(context* context)
{
    const char* content = char_buffer_cstr(&context->attrib);
    long value = strtol(content, NULL, 10);

    if (errno != 0 || value < 0 || value > INT_MAX)
    {
        dbgprint("Invalid int value '%s'!", content);
        return LEX_STATE_ERROR;
    }

    return LEX_STATE_IVAL;
}

lex_state lex_validate_float(context* context)
{
    const char* content = char_buffer_cstr(&context->attrib);
    strtod(content, NULL);

    if (errno != 0 && errno != ERANGE)
    {
        dbgprint("Invalid float value '%s'!", content);
        return LEX_STATE_ERROR;
    }

    return LEX_STATE_FVAL;
}

bool lex_init(context* context, FILE* input)
{
    if (!char_buffer_init(&context->attrib))
        return false;

    return lex_next(context, input);
}

bool lex_next(context* context, FILE* input)
{
    lex_state state = LEX_STATE_START;
    char_buffer_clear(&context->attrib);

    while (state < LEX_STATE_END)
    {
        lex_state_function fun = LEX_TABLE[state];
        state = fun(context, input);

        if (state == LEX_STATE_ERROR)
        {
            dbgprint("Lexical analysis failed!");
            context->token = INVALID;
            return false;
        }
    }

    const char* content = char_buffer_cstr(&context->attrib);
    const char* token = token_str(context->token);
    
    if (strcmp(content, token) == 0 || context->token == EOS)
    {
        infoprint("Token %s", token);
    }

    else
    {
        infoprint("Token %s: '%s'", token, content);
    }

    return true;
}

void lex_destroy(context* context)
{
    char_buffer_destroy(&context->attrib);
}

lex_state lex_state_start(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_whitespace(current))
        return LEX_STATE_START;

    switch (current)
    {
        case '$':
            return LEX_STATE_ID_0;

        case '"':
            return LEX_STATE_SVAL_0;
    }

    CHAR_BUFFER_ADD(context, current);

    if (lex_is_first_identifier(current))
        return LEX_STATE_FID_0;

    if (lex_is_digit(current))
        return LEX_STATE_IVAL_0;

    switch (current)
    {
        case '+':
            context->token = ADD;
            return LEX_STATE_ADD;

        case '-':
            context->token = SUB;
            return LEX_STATE_SUB;

        case '*':
            context->token = MUL;
            return LEX_STATE_MUL;

        case '/':
            return LEX_STATE_COM_0;

        case '<':
            return LEX_STATE_LT_0;

        case '>':
            return LEX_STATE_GT_0;

        case '=':
            return LEX_STATE_ASSIG_0;

        case '!':
            return LEX_STATE_NEQ_0;

        case '?':
            return LEX_STATE_PE_MARK_0;

        case '(':
            context->token = LPAR;
            return LEX_STATE_LPAR;

        case ')':
            context->token = RPAR;
            return LEX_STATE_RPAR;

        case '{':
            context->token = LBRC;
            return LEX_STATE_LBRC;

        case '}':
            context->token = RBRC;
            return LEX_STATE_RBRC;

        case ',':
            context->token = COMMA;
            return LEX_STATE_COMMA;

        case ':':
            context->token = TYPE;
            return LEX_STATE_TYPE;

        case ';':
            context->token = SEMIC;
            return LEX_STATE_SEMIC;

        case '.':
            context->token = STRCAT;
            return LEX_STATE_STRCAT;

        case EOF:
            context->token = EOS;
            return LEX_STATE_EOS;

        default:
            dbgprint("Unexpected symbol '%c'!", current);
            return LEX_STATE_ERROR;
    }
}

lex_state lex_state_com_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '/')
        return LEX_STATE_LCOM_0;

    if (current == '*')
        return LEX_STATE_BCOM_0;

    ungetc(current, input);

    context->token = DIV;
    return LEX_STATE_DIV;
}

lex_state lex_state_lcom_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '\n' || current == EOF)
    {
        ungetc(current, input);

        char_buffer_clear(&context->attrib);
        return LEX_STATE_START;
    }

    return LEX_STATE_LCOM_0;
}

lex_state lex_state_bcom_0(context* context, FILE* input)
{
    (void) context;
    int current = fgetc(input);

    if (current == EOF)
    {
        dbgprint("Unexpected symbol '%c'!", current);
        return LEX_STATE_ERROR;
    }

    if (current == '*')
        return LEX_STATE_BCOM_1;

    return LEX_STATE_BCOM_0;
}

lex_state lex_state_bcom_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '*')
        return LEX_STATE_BCOM_1;

    if (current == '/')
    {
        char_buffer_clear(&context->attrib);
        return LEX_STATE_START;
    }

    return LEX_STATE_BCOM_0;
}

lex_state lex_state_id_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_first_identifier(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_ID_1;
    }
    
    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_id_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_identifier(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_ID_1;
    }

    ungetc(current, input);

    context->token = ID;
    return LEX_STATE_ID;
}

lex_state lex_state_fid_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_identifier(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FID_0;
    }

    if (char_buffer_equals(&context->attrib, LEX_PHP_PC_MARK_START))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_PC_MARK_0;
    }

    ungetc(current, input);

    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_FUNCTION, FUNC, LEX_STATE_FUNC);
    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_IF, IF, LEX_STATE_IF);
    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_ELSE, ELSE, LEX_STATE_ELSE);
    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_WHILE, WHILE, LEX_STATE_WHILE);
    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_RETURN, RETURN, LEX_STATE_RETURN);
    LEX_KEYWORD_CHECK(context, LEX_KEYWORD_NULL, VVAL, LEX_STATE_VVAL);

    LEX_KEYWORD_CHECK(context, LEX_TYPE_INT, ITYPE, LEX_STATE_ITYPE);
    LEX_KEYWORD_CHECK(context, LEX_TYPE_FLOAT, FTYPE, LEX_STATE_FTYPE);
    LEX_KEYWORD_CHECK(context, LEX_TYPE_STRING, STYPE, LEX_STATE_STYPE);
    LEX_KEYWORD_CHECK(context, LEX_TYPE_VOID, VTYPE, LEX_STATE_VTYPE);

    context->token = FID;
    return LEX_STATE_FID;
}

lex_state lex_state_nid_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_identifier(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_NID_0;
    }

    ungetc(current, input);

    LEX_KEYWORD_CHECK(context, LEX_TYPE_NINT, NITYPE, LEX_STATE_NITYPE);
    LEX_KEYWORD_CHECK(context, LEX_TYPE_NFLOAT, NFTYPE, LEX_STATE_NFTYPE);
    LEX_KEYWORD_CHECK(context, LEX_TYPE_NSTRING, NSTYPE, LEX_STATE_NSTYPE);

    dbgprint("Unexpected token '%s'!", char_buffer_cstr(&context->attrib));

    return LEX_STATE_ERROR;
}

lex_state lex_state_ival_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_IVAL_0;
    }

    if (current == '.')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_0;
    }

    if (current == 'e' || current == 'E')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_2;
    }

    ungetc(current, input);

    context->token = IVAL;
    return lex_validate_int(context);
}

lex_state lex_state_fval_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_1;
    }
    
    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_fval_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_1;
    }

    if (current == 'e' || current == 'E')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_2;
    }

    ungetc(current, input);

    context->token = FVAL;
    return lex_validate_float(context);
}

lex_state lex_state_fval_2(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_4;
    }

    if (current == '+' || current == '-')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_3;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_fval_3(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_4;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_fval_4(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_digit(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_FVAL_4;
    }

    ungetc(current, input);

    context->token = FVAL;
    return lex_validate_float(context);
}

lex_state lex_state_sval_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '"')
    {
        context->token = SVAL;
        return LEX_STATE_SVAL;
    }

    if (current == '\\')
        return LEX_STATE_SVAL_1;

    if (current == '$')
    {
        dbgprint("Unexpected symbol '%c'!", current);
        return LEX_STATE_ERROR;
    }

    if (current < 32) // Catches even EOF
    {
        dbgprint("Unexpected symbol '%c'!", current);
        return LEX_STATE_ERROR;
    }

    CHAR_BUFFER_ADD(context, current);
    return LEX_STATE_SVAL_0;
}

lex_state lex_state_sval_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '"' || current == '$' || current == '\\')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_SVAL_0;
    }

    if (current == 'n')
    {
        CHAR_BUFFER_ADD(context, '\n');
        return LEX_STATE_SVAL_0;
    }

    if (current == 't')
    {
        CHAR_BUFFER_ADD(context, '\t');
        return LEX_STATE_SVAL_0;
    }

    if (current == 'x')
    {
        memset(lex_esc, 0, sizeof(lex_esc));
        lex_esc_len = 0;

        return LEX_STATE_SVAL_X;
    }

    if (lex_is_digit(current))
    {
        memset(lex_esc, 0, sizeof(lex_esc));
        lex_esc_len = 0;

        ungetc(current, input);
        return LEX_STATE_SVAL_O;
    }

    ungetc(current, input);
    CHAR_BUFFER_ADD(context, '\\');

    return LEX_STATE_SVAL_0;
}

lex_state lex_state_sval_x(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current != EOF && lex_is_hex(current))
    {
        lex_esc[lex_esc_len] = current;
        lex_esc_len++;

        if (lex_esc_len == LEX_ESC_HEX_LEN)
        {
            long symbol = strtol(lex_esc, NULL, 16);

            if (errno == 0 && symbol >= 0x01 && symbol <= 0xFF)
            {
                CHAR_BUFFER_ADD(context, symbol);
                return LEX_STATE_SVAL_0;
            }
        }
        else
        {
            return LEX_STATE_SVAL_X;
        }
    }
    else
    {
        ungetc(current, input);
    }

    for (int i = lex_esc_len - 1; i >= 0; i--)
        ungetc(lex_esc[i], input);

    ungetc('x', input);

    CHAR_BUFFER_ADD(context, '\\');
    return LEX_STATE_SVAL_0;
}

lex_state lex_state_sval_o(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current != EOF && lex_is_octal(current))
    {
        lex_esc[lex_esc_len] = current;
        lex_esc_len++;

        if (lex_esc_len == LEX_ESC_OCT_LEN)
        {
            long symbol = strtol(lex_esc, NULL, 8);

            if (errno == 0 && symbol >= 0001 && symbol <= 0377)
            {
                CHAR_BUFFER_ADD(context, symbol);
                return LEX_STATE_SVAL_0;
            }
        }
        else
        {
            return LEX_STATE_SVAL_O;
        }
    }
    else
    {
        ungetc(current, input);
    }

    for (int i = lex_esc_len - 1; i >= 0; i--)
        ungetc(lex_esc[i], input);

    CHAR_BUFFER_ADD(context, '\\');
    return LEX_STATE_SVAL_0;
}

lex_state lex_state_assig_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_EQ_0;
    }

    ungetc(current, input);

    context->token = ASSIG;
    return LEX_STATE_ASSIG;
}

lex_state lex_state_lt_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);

        context->token = LTE;
        return LEX_STATE_LTE;
    }

    if (current == '?')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_PS_MARK_0;
    }

    ungetc(current, input);

    context->token = LT;
    return LEX_STATE_LT;
}

lex_state lex_state_gt_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);

        context->token = GTE;
        return LEX_STATE_GTE;
    }

    ungetc(current, input);

    context->token = GT;
    return LEX_STATE_GT;
}

lex_state lex_state_eq_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);
        
        context->token = EQ;
        return LEX_STATE_EQ;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_neq_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_NEQ_1;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_neq_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '=')
    {
        CHAR_BUFFER_ADD(context, current);

        context->token = NEQ;
        return LEX_STATE_NEQ;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_ps_mark_0(context* context, FILE* input)
{
    int current = fgetc(input);
    size_t len = char_buffer_len(&context->attrib);

    if (current == LEX_PHP_PS_MARK[len])
    {
        CHAR_BUFFER_ADD(context, current);

        if (char_buffer_equals(&context->attrib, LEX_PHP_PS_MARK))
            return LEX_STATE_PS_MARK_1;

        return LEX_STATE_PS_MARK_0;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_ps_mark_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (lex_is_whitespace(current))
    {
        context->token = PS_MARK;
        return LEX_STATE_PS_MARK;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_pc_mark_0(context* context, FILE* input)
{
    int current = fgetc(input);
    size_t len = char_buffer_len(&context->attrib);

    if (current == LEX_PHP_PC_MARK[len])
    {
        CHAR_BUFFER_ADD(context, current);

        if (char_buffer_equals(&context->attrib, LEX_PHP_PC_MARK))
        {
            context->token = PC_MARK;
            return LEX_STATE_PC_MARK;
        }

        return LEX_STATE_PC_MARK_0;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_pe_mark_0(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == '>')
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_PE_MARK_1;
    }

    if (lex_is_first_identifier(current))
    {
        CHAR_BUFFER_ADD(context, current);
        return LEX_STATE_NID_0;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_pe_mark_1(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == EOF)
    {
        context->token = PE_MARK;
        return LEX_STATE_PE_MARK;
    }

    if (current == '\n')
        return LEX_STATE_PE_MARK_2;

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}

lex_state lex_state_pe_mark_2(context* context, FILE* input)
{
    int current = fgetc(input);

    if (current == EOF)
    {
        context->token = PE_MARK;
        return LEX_STATE_PE_MARK;
    }

    dbgprint("Unexpected symbol '%c'!", current);
    return LEX_STATE_ERROR;
}
