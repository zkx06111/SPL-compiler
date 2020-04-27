%option noyywrap
%{
#include <stdio.h>
#include <ctype.h>
%}
LP "("
RP ")"
LB "]"
RB "]"
DOT "."
COMMA ","
COLON ":"
MUL "*"
DIV "/"
UNEQUAL "<>"
NOT "NOT"
PLUS "+"
MINUS "-"
GE ">="
GT ">"
LE "<="


%%
{comment} {printf("%s", yytext);}
{keyword} {
    for (char *p = yytext; *p; ++p) {
        if (islower(*p)) {
            *p = *p - 'a' + 'A'; 
        }
    }
    printf("%s",yytext);
}
{identifier} {printf("%s", yytext);}
%%
int main()
{ yylex(); return 0; }