#ifndef TOKEN_H
#define TOKEN_H
#include <stdio.h>
typedef enum
{
    T_LP = 0,
    T_RP,
    T_LB,
    T_RB,
    T_DOT,
    T_COMMA,
    T_COLON,
    T_MUL,
    T_DIV,
    T_UNEQUAL,
    T_NOT,
    T_PLUS,
    T_MINUS,
    T_GE,
    T_GT,
    T_LE,
    T_LT,
    T_EQUAL,
    T_ASSIGN,
    T_MOD,
    T_DOTDOT,
    T_SEMI,
    T_FALSE,
    T_MAXINT,
    T_TRUE,
    T_ABS,
    T_CHR,
    T_ODD,
    T_PRED,
    T_SQRT,
    T_SUCC,
    T_WRITE,
    T_WRITELN,
    T_BOOLEAN_TYPE,
    T_CHAR_TYPE,
    T_INTEGER_TYPE,
    T_REAL_TYPE,
    T_READ,
    T_INTEGER,
    T_REAL,
    T_CHAR,
    T_AND,
    T_ARRAY,
    T_BEG,
    T_CASE,
    T_CONST,
    T_DO,
    T_DOWNTO,
    T_ELSE,
    T_END,
    T_FOR,
    T_FUNCTION,
    T_GOTO,
    T_IF,
    T_OF,
    T_OR,
    T_PACKED,
    T_PROCEDURE,
    T_PROGRAM,
    T_RECORD,
    T_REPEAT,
    T_THEN,
    T_TO,
    T_TYPE,
    T_UNTIL,
    T_VAR,
    T_WHILE,
    T_ID,
    T_LABEL
} TokenType;

const char *token_names[] = {
    "LP",
    "RP",
    "LB",
    "RB",
    "DOT",
    "COMMA",
    "COLON",
    "MUL",
    "DIV",
    "UNEQUAL",
    "NOT",
    "PLUS",
    "MINUS",
    "GE",
    "GT",
    "LE",
    "LT",
    "EQUAL",
    "ASSIGN",
    "MOD",
    "DOTDOT",
    "SEMI",
    "FALSE",
    "MAXINT",
    "TRUE",
    "ABS",
    "CHR",
    "ODD",
    "PRED",
    "SQRT",
    "SUCC",
    "WRITE",
    "WRITELN",
    "BOOLEAN_TYPE",
    "CHAR_TYPE",
    "INTEGER_TYPE",
    "REAL_TYPE",
    "READ",
    "INTEGER",
    "REAL",
    "CHAR",
    "AND",
    "ARRAY",
    "BEG",
    "CASE",
    "CONST",
    "DO",
    "DOWNTO",
    "ELSE",
    "END",
    "FOR",
    "FUNCTION",
    "GOTO",
    "IF",
    "OF",
    "OR",
    "PACKED",
    "PROCEDURE",
    "PROGRAM",
    "RECORD",
    "REPEAT",
    "THEN",
    "TO",
    "TYPE",
    "UNTIL",
    "VAR",
    "WHILE",
    "ID",
    "LABEL"};

void print_token(int token)
{
    printf("%s\n", token_names[token]);
}

#endif