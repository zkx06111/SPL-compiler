%option noyywrap
%{
#include "token.h"
#include <stdio.h>
int cur_line_num = 1;

void lex_error(char*, int);
%}

SYS_CON false|maxint|true
SYS_FUNCT abs|chr|odd|ord|pred|sqr|sqrt|succ
SYS_PROC write|writeln
SYS_TYPE boolean|char|integer|real
READ read
INTEGER [0-9]+
REAL [0-9]+.[0-9]+
CHAR "'.'"
KEYWORD and|array|begin|case|const|div|do|downto|else|end|for|function|goto|if|mod|not|of|or|packed|procedure|program|record|repeat|then|to|type|until|var|while
ID ([a-zA-Z][_a-zA-Z0-9]*)


%%
[\n] {++cur_line_num;}
[ \t\r\a]+ {};
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"." {return DOT;}
"," {return COMMA;}
":" {return COLON;}
"*" {return MUL;}
"/" {return DIV;}
"<>" {return UNEQUAL;}
"not" {return NOT;}
"+" {return PLUS;}
"-" {return MINUS;}
">=" {return GE;}
">" {return GT;}
"<=" {return LE;}
"<" {return LT;}
"=" {return EQUAL;}
":=" {return ASSIGN;}
"mod" {return MOD;}
".." {return DOTDOT;}
";" {return SEMI;}
{SYS_CON} {return SYS_CON;}
{SYS_FUNCT} {return SYS_FUNCT;}
{SYS_PROC} {return SYS_PROC;}
{SYS_TYPE} {return SYS_TYPE;}
{READ} {return READ;}
{INTEGER} {return INTEGER;}
{REAL} {return REAL;}
{CHAR} {return CHAR;}
{KEYWORD} {return KEYWORD;}
{ID} {return ID;}
<<EOF>> {return -1;}
. {lex_error("Unrecognized character", cur_line_num);}

%%
void lex_error(char *msg, int line) {
    printf("Unrecognized character at line %d\n\n", cur_line_num);
}

int main()
{
    int token;
    while ((token = yylex()) != -1) {
        printf("%s ", yytext); print_token(token);
    }
    return 0;
}