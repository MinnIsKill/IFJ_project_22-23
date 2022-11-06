#include <dirent.h>
#include <stdio.h>
#include <stdbool.h>

#include "../../src/context.h"
#include "../../src/lex.h"
#include "../../src/token.h"
#include "../../src/vector.h"

typedef struct test_pair
{
    const char* input;
    const char* output;
} test_pair;

#define TEST_DIR_IN             "in"
#define TEST_DIR_OUT            "out"
#define TEST_FILE(name)         { "./" TEST_DIR_IN "/" name ".php", "./" TEST_DIR_OUT "/" name ".lex" }

const test_pair FILES[] =
{
    TEST_FILE("tokens"),
    TEST_FILE("float"),
    TEST_FILE("string"),
    TEST_FILE("factorial"),
    TEST_FILE("ps_mark_invalid"),
    TEST_FILE("ps_mark_valid")
};

#define FILE_COUNT              6

void fgets_but_better(char* target, size_t size, FILE* stream)
{
    memset(target, 0, size);
    fgets(target, size, stream);
    target[strlen(target) - 1] = '\0';
}

bool has_value(token_type type)
{
    return type == IVAL ||
           type == FVAL ||
           type == SVAL ||
           type == ID ||
           type == FID;
}

void test(test_pair file)
{
    printf("Testing '%s' against '%s' ... ", file.input, file.output);
    int fail = false;

    FILE* input = fopen(file.input, "r");
    FILE* output = fopen(file.output, "r");

    const int max = 256;
    char cmp[max];
    int counter = 0;

    context context = {0};
    lex_init(&context, input);

    while (context.token != EOS)
    {
        counter++;
        
        memset(cmp, 0, max);
        fgets(cmp, max, output);
        cmp[strlen(cmp) - 1] = '\0';
        
        const char* token = token_str(context.token);

        if (context.token == INVALID && strcmp(cmp, "ERROR") == 0)
        {
            goto exit;
        }

        if (strcmp(token, cmp) != 0)
        {
            printf("FAIL\n%d: Unexpected token '%s'. Expected '%s'!\n", counter, token, cmp);
            fail = true;
            goto exit;
        }

        if (has_value(context.token))
        {
            counter++;

            for (size_t i = 0; i < context.attrib.len; i++)
            {
                int c = fgetc(output);

                if (char_buffer_get(&context.attrib, i) != c)
                {
                    printf("FAIL\n%d: Unexpected symbol '%c'. Expected '%c'!\n", counter, char_buffer_get(&context.attrib, i), c);
                    fail = true;
                    goto exit;
                }

                if (c == '\n')
                    counter++;
            }

            int cc = fgetc(output);

            if (cc != '\n')
            {
                printf("FAIL\n%d: Unexpected end of value. Expected '%c'!\n", counter, cc);
                fail = true;
                goto exit;
            }
        }

        lex_next(&context, input);
    }

    if (!fail)
    {
        counter++;
        fgets_but_better(cmp, max, output);
        if (strlen(cmp) != 0)
        {
            printf("FAIL\n%d: Unexpected EOF. Expected '%s'!\n", counter, cmp);
            fail = true;
        }
    }

    exit:
    lex_destroy(&context);

    fclose(output);
    fclose(input);

    if (!fail)
        printf("OK\n");
}

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;
    
    for (size_t i = 0; i < FILE_COUNT; i++)
        test(FILES[i]);

    return 0;
}
