#include"fake_lex.h"
#include"flex/lex.yy.c"

void lex_init(context* con)
{
    con->token = yylex();
    con->attrib = strdup((yytext == NULL)?(""):(yytext)); 
    //printf("first:: %s | %s \n",token_str(con->token),con->attrib.data);
}

void lex_next(context* con)
{
    assert(con != NULL);
    if(con->token == EOS)return;
    //printf("pre:: %s | %s \n",token_str(con->token),con->attrib.data);
    free(con->attrib);
    con->token = yylex();
    con->attrib = strdup((yytext == NULL)?(""):(yytext)); 
    //printf("pos:: %s | %s \n",token_str(con->token),con->attrib.data);
}

void lex_destroy()
{
    yylex_destroy();
}
