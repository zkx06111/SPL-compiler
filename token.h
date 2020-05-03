#ifndef TOKEN_H
#define TOKEN_H
typedef enum
{
    LP,
    RP,
    LB,
    RB,
    DOT,
    COMMA,
    COLON,
    MUL,
    DIV,
    UNEQUAL,
    NOT,
    PLUS,
    MINUS,
    GE,
    GT,
    LE,
    LT,
    EQUAL,
    ASSIGN,
    MOD,
    DOTDOT,
    SEMI,
    SYS_CON,
    SYS_FUNCT,
    SYS_PROC,
    SYS_TYPE,
    READ,
    INTEGER,
    REAL,
    CHAR,
    KEYWORD,
    ID
} TokenType;

void print_token(int token)
{
    static char *token_names[] = {
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
        "SYS_CON",
        "SYS_FUNC",
        "SYS_PROC",
        "SYS_TYPE",
        "READ",
        "INTEGER",
        "REAL",
        "CHAR",
        "KEYWORD",
        "ID"};
    printf("%s\n", token_names[token]);
}

#endif