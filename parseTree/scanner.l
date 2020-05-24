%option noyywrap
%{
#include <math.h>
#include <string.h>
#include "y.tab.hpp"
#include <stdio.h>
#include "tree.h"
#include "token.h"
#define SETLVAL(x) yylval.t = newNode(token_names[x], curLineNum, NULL, 0)
int curLineNum = 1;

void lex_error(char*, int);
int yylex();
%}

LABEL label
FALSE false
MAXINT maxint
TRUE true
ABS abs
CHR chr
ODD odd
ORD ord
PRED pred
SQR sqr
SQRT sqrt
SUCC succ
WRITE write
WRITELN writeln
BOOLEAN_TYPE boolean
CHAR_TYPE char
INTEGER_TYPE integer
REAL_TYPE real
READ read
INTEGER [0-9]+
REAL [0-9]+(".")[0-9]+
CHAR \'.\'
AND and
ARRAY array
BEG begin
CASE case
CONST const
DIV div
DO do
DOWNTO downto
ELSE else
END end
FOR for
FUNCTION function
GOTO goto
IF if
OF of
OR or
PACKED packed
PROCEDURE procedure
PROGRAM program
RECORD record
REPEAT repeat
THEN then
TO to
TYPE type
UNTIL until
VAR var
WHILE while
ID ([a-zA-Z][_a-zA-Z0-9]*)


%%
[\n] {++curLineNum;}
[ \t\r\a]+ {};
"(" {SETLVAL(T_LP); return LP;}
")" {SETLVAL(T_RP); return RP;}
"[" {SETLVAL(T_LB); return LB;}
"]" {SETLVAL(T_RB); return RB;}
"." {SETLVAL(T_DOT); return DOT;}
"," {SETLVAL(T_COMMA); return COMMA;}
":" {SETLVAL(T_COLON); return COLON;}
"*" {SETLVAL(T_MUL); return MUL;}
"/" {SETLVAL(T_DIV); return DIV;}
"<>" {SETLVAL(T_UNEQUAL); return UNEQUAL;}
"not" {SETLVAL(T_NOT); return NOT;}
"+" {SETLVAL(T_PLUS); return PLUS;}
"-" {SETLVAL(T_MINUS); return MINUS;}
">=" {SETLVAL(T_GE); return GE;}
">" {SETLVAL(T_GT); return GT;}
"<=" {SETLVAL(T_LE); return LE;}
"<" {SETLVAL(T_LT); return LT;}
"=" {SETLVAL(T_EQUAL); return EQUAL;}
":=" {SETLVAL(T_ASSIGN); return ASSIGN;}
"mod" {SETLVAL(T_MOD); return MOD;}
".." {SETLVAL(T_DOTDOT); return DOTDOT;}
";" {SETLVAL(T_SEMI); return SEMI;}
{LABEL} {SETLVAL(T_LABEL); return LABEL;}
{FALSE} {int val = 0; yylval.t = newNode("BOOLEAN", curLineNum, (void*)&val, 0); return FALSE;}
{MAXINT} {int val = 2147483647; yylval.t = newNode("INTEGER", curLineNum, (void*)&val, 0); return MAXINT;}
{TRUE} {int val = 1; yylval.t = newNode("BOOLEAN", curLineNum, (void*)&val, 0); return TRUE;}
{ABS} {SETLVAL(T_ABS); return ABS;}
{CHR} {SETLVAL(T_CHR); return CHR;}
{ODD} {SETLVAL(T_ODD); return ODD;}
{PRED} {SETLVAL(T_PRED); return PRED;}
{SQRT} {SETLVAL(T_SQRT); return SQRT;}
{SUCC} {SETLVAL(T_SUCC); return SUCC;}
{ORD} {SETLVAL(T_SUCC); return SUCC;}
{SQR} {SETLVAL(T_SUCC); return SUCC;}
{WRITE} {SETLVAL(T_WRITE); return WRITE;}
{WRITELN} {SETLVAL(T_WRITELN); return WRITELN;}
{BOOLEAN_TYPE} {SETLVAL(T_BOOLEAN_TYPE); return BOOLEAN_TYPE;}
{CHAR_TYPE} {SETLVAL(T_CHAR_TYPE); return CHAR_TYPE;}
{INTEGER_TYPE} {SETLVAL(T_INTEGER_TYPE); return INTEGER_TYPE;}
{REAL_TYPE} {SETLVAL(T_REAL_TYPE); return REAL_TYPE;}
{READ} {SETLVAL(T_READ); return READ;}
{REAL} {
    double vald = atof(yytext);
    yylval.t = newNode("REAL", curLineNum, (void*)&vald, 1);
    return REAL;
}
{INTEGER} {
    int val = atoi(yytext);
    yylval.t = newNode("INTEGER", curLineNum, (void*)&val, 0);
    return INTEGER;
}
{CHAR} {
    yylval.t = newNode("CHAR", curLineNum, (void*)&yytext[1], 2);
    return CHAR;
}
{AND} {SETLVAL(T_AND); return AND;}
{ARRAY} {SETLVAL(T_ARRAY); return ARRAY;}
{BEG} {SETLVAL(T_BEG); return BEG;}
{CASE} {SETLVAL(T_CASE); return CASE;}
{CONST} {SETLVAL(T_CONST); return CONST;}
{DIV} {SETLVAL(T_DIV); return DIV;}
{DO} {SETLVAL(T_DO); return DO;}
{DOWNTO} {SETLVAL(T_DOWNTO); return DOWNTO;}
{ELSE} {SETLVAL(T_ELSE); return ELSE;}
{END} {SETLVAL(T_END); return END;}
{FOR} {SETLVAL(T_FOR); return FOR;}
{FUNCTION} {SETLVAL(T_FUNCTION); return FUNCTION;}
{GOTO} {SETLVAL(T_GOTO); return GOTO;}
{IF} {SETLVAL(T_IF); return IF;}
{OF} {SETLVAL(T_OF); return OF;}
{OR} {SETLVAL(T_OR); return OR;}
{PACKED} {SETLVAL(T_PACKED); return PACKED;}
{PROCEDURE} {SETLVAL(T_PROCEDURE); return PROCEDURE;}
{PROGRAM} {SETLVAL(T_PROGRAM); return PROGRAM;}
{RECORD} {SETLVAL(T_RECORD); return RECORD;}
{REPEAT} {SETLVAL(T_REPEAT); return REPEAT;}
{THEN} {SETLVAL(T_THEN); return THEN;}
{TO} {SETLVAL(T_TO); return TO;}
{TYPE} {SETLVAL(T_TYPE); return TYPE;}
{UNTIL} {SETLVAL(T_UNTIL); return UNTIL;}
{VAR} {SETLVAL(T_VAR); return VAR;}
{WHILE} {SETLVAL(T_WHILE); return WHILE;}
{ID} {yylval.t = newNode("ID", curLineNum, yytext, 3); return ID;}
<<EOF>> {return -1;}
. {lex_error("Unrecognized character", curLineNum);}

%%
void lex_error(char *msg, int line) {
    printf("Unrecognized character at line %d\n\n", curLineNum);
}