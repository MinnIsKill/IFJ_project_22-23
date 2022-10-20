%{
#include"./../token.h"
%}

%option noyywrap

ID [$][_a-zA-Z][_a-zA-Z0-9]*
FID [_a-zA-Z][_a-zA-Z0-9]*
IVAL ([1-9][0-9]*)|0
SVAL \".*\"
FVAL [1-9][0-9]*[.][0-9]*

%%
[ \n\t]+ { ; }
"declare(strict_types=1);" { return(PC_MARK); } 
"while" { return(WHILE); }
"if" { return(IF); }
"else" { return(ELSE); }
"function" { return(FUNC); }
"return" { return(RETURN); }
"int" { return(ITYPE); }
"float" { return(FTYPE); }
"string" { return(STYPE); }
"?int" { return(NITYPE); }
"?float" { return(NFTYPE); }
"?string" { return(NSTYPE); }
"void" { return(VTYPE); }
{ID} { return(ID); }
{FID} { return(FID); }
{IVAL} { return(IVAL); }
{FVAL} { return(FVAL); }
{SVAL} { return(SVAL); }
"+" { return(ADD); }
"-" { return(SUB); }
"." { return(STRCAT); }
"*" { return(MUL); }
"/" { return(DIV); }
"<" { return(LT); }
"<?php" { return(PS_MARK); }
">" { return(GT); }
"<="  { return(LTE); }
">="  { return(GTE); }
"===" { return(EQ); }
"!==" { return(NEQ); }
";" { return(SEMIC); }
":" { return(TYPE); }
"(" { return(LPAR); }
")" { return(RPAR); }
"{" { return(LBRC); }
"}" { return(RBRC); }
"=" { return(ASSIG); }
"," { return(COMMA); }
"?>" { return(PE_MARK); }
. { return(INVALID); }
<<EOF>> { return(EOS); }
%%


void yyerror(char* msg)
{
    puts("LEX ERROR");
    puts(msg);
}
