#include "lex.h"
#include "char_buffer.h"

int main(int argc, char** argv)
{
    (void) argc;
    (void) argv;

    context con = {};
    lex_init(&con);

    while (con.token != EOS)
        lex_next(&con);

    lex_destroy(&con);

    return 0;
}
