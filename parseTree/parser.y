%{
#include "tree.h"
//#include "token.h"
#include <stdio.h>
#include "lex.yy.c"
#define PARSER
struct TreeNode* p;
pTNode fa[233];
int top=0;
const int MAXLINE = 2147483647;
void yyerror(const char* msg) {
    if(yylval.t) printf("%s at line %d.\n", msg, yylval.t->lineNumber);
    else printf("%s\n", msg);
    p = nullptr;
}

int idx = 0;

void printNode(pTNode p) {
    if (p == NULL) printf("NULL");
    else printf("%s %d", p->type, p->lineNumber);
}

void dfs(pTNode cur, pTNode par, int faid) {
    printNode(par);
    printf(" ");
    int curid = ++idx;
    printNode(cur);
    printf("\n");
    if (cur->visit == 1) {
        exit(0);
    }
    cur->visit=1;
    for (pTNode t = cur->child; t; t = t->sibling) {
        dfs(t, cur, curid);
    }
}
void print(pTNode cur, int depth){
    if(cur == nullptr || cur->lineNumber == MAXLINE) return;
    for(int i = 0; i < depth; i += 1) printf("-");
    if(strcmp(cur->type, "ID") == 0) printf("%s(ID)", cur->vals);
    else if(strcmp(cur->type, "INTEGER") == 0) printf("%d", cur->vali);
    else if(strcmp(cur->type, "CHAR") == 0) printf("%c(CHAR)", cur->valc);
    else if(strcmp(cur->type, "REAL") == 0) printf("%lf", cur->valf);
    else if(strcmp(cur->type, "BOOLEAN") == 0) printf(cur->vali ? "true" : "false");
    else if(strcmp(cur->type, "LP") == 0) printf("(");
    else if(strcmp(cur->type, "RP") == 0) printf(")");
    else if(strcmp(cur->type, "LB") == 0) printf("[");
    else if(strcmp(cur->type, "RB") == 0) printf("]");
    else if(strcmp(cur->type, "DOT") == 0) printf(".");
    else if(strcmp(cur->type, "MUL") == 0) printf("*");
    else if(strcmp(cur->type, "DIV") == 0) printf("/");
    else if(strcmp(cur->type, "UNEQUAL") == 0) printf("<>");
    else if(strcmp(cur->type, "PLUS") == 0) printf("+");
    else if(strcmp(cur->type, "GE") == 0) printf(">=");
    else if(strcmp(cur->type, "GT") == 0) printf(">");
    else if(strcmp(cur->type, "LE") == 0) printf("<=");
    else if(strcmp(cur->type, "LT") == 0) printf("<");
    else if(strcmp(cur->type, "ASSIGN") == 0) printf(":=");
    else if(strcmp(cur->type, "DOTDOT") == 0) printf("..");
    else printf("%s", cur->type);
    printf("\n");
    for (pTNode t = cur->child; t; t = t->sibling) {
        print(t, depth + 1);
    }
}
%}
%union{
    struct TreeNode* t;
}
%token <t> LP RP LB RB DOT COMMA COLON MUL DIV UNEQUAL NOT PLUS MINUS GE GT LE LT EQUAL ASSIGN MOD DOTDOT SEMI FALSE MAXINT TRUE ABS CHR ODD PRED SQRT SUCC WRITE WRITELN BOOLEAN_TYPE CHAR_TYPE INTEGER_TYPE REAL_TYPE READ INTEGER REAL CHAR AND ARRAY BEG CASE CONST DO DOWNTO ELSE END FOR FUNCTION GOTO IF OF OR PACKED PROCEDURE PROGRAM RECORD REPEAT THEN TO TYPE UNTIL VAR WHILE ID SQR ORD LABEL
%type <t> program program_head routine sub_routine routine_head label_part const_part const_expr_list const_value type_part type_decl_list type_definition type_decl simple_type_decl sys_type array_type_decl record_type_decl field_decl_list field_decl name_list var_part var_decl_list var_decl routine_part function_decl function_head procedure_decl procedure_head parameters para_decl_list para_type_list var_para_list val_para_list routine_body compound_stmt stmt_list stmt non_label_stmt assign_stmt proc_stmt if_stmt else_clause repeat_stmt while_stmt for_stmt direction case_stmt case_expr_list case_expr goto_stmt expression_list expression expr term factor args_list sys_proc sys_funct label_list
%left PLUS MINUS
%left MUL DIV MOD
%%

program:
    program_head routine DOT {
        p = newNode("program", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

program_head:
    PROGRAM ID SEMI {
        p = newNode("program_head", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

routine:
    routine_head routine_body {
        p = newNode("routine", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

sub_routine:
    routine_head routine_body {
        p = newNode("sub_routine", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

routine_head:
    label_part const_part type_part var_part {fa[++top]=newNode("routine_part", MAXLINE, NULL, 0);}routine_part {
        p = newNode("routine_head", MAXLINE, NULL, 0);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

label_part:
    LABEL {fa[++top] = newNode("label_part", MAXLINE, NULL, 0);} label_list SEMI {
        $<t>$ = fa[top--];
    }
|   /* empty */ { $<t>$ = newNode("label_part", MAXLINE, NULL, 0); }
;

label_list:
    label_list COMMA INTEGER {
        addRightChild(fa[top], $3);
    }
|   INTEGER {
        addRightChild(fa[top], $1);
    }
;


const_part:
    CONST {fa[++top] = newNode("const_part", MAXLINE, NULL, 0);} const_expr_list {
        $<t>$ = fa[top--];
    }
|   { $<t>$ = newNode("const_part", MAXLINE, NULL, 0);}
;

const_expr_list:
    const_expr_list ID EQUAL const_value SEMI {
        addRightChild(fa[top], $2);
        addRightChild(fa[top], $4);
    }
|   ID EQUAL const_value SEMI {
        addRightChild(fa[top], $1);
        addRightChild(fa[top], $3);
    }
;

const_value:
    INTEGER {
        $<t>$ = $1;
    }
|   REAL{
        $<t>$ = $1;
    }
|   CHAR{
        $<t>$ = $1;
    }
|   FALSE{
        $<t>$ = $1;
    }
|   TRUE{
        $<t>$ = $1;
    }
|   MAXINT{
        $<t>$ = $1;
    }
;

type_part:
    TYPE {fa[++top] = newNode("type_part", MAXLINE, NULL, 0);} type_decl_list {
        $<t>$ = fa[top--];
    }
|   { $<t>$ = newNode("type_part", MAXLINE, NULL, 0);}
;

type_decl_list:
    type_decl_list type_definition {
        addRightChild(fa[top], $2);
    }
|   type_definition {
        addRightChild(fa[top], $1);
    }
;

type_definition:
    ID EQUAL type_decl SEMI {
        p = newNode("type_definition", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

type_decl:
    simple_type_decl {
        $<t>$ = $1;
   }
|   array_type_decl {
        $<t>$ = $1;
    }
|   record_type_decl {
        $<t>$ = $1;
    }
;

simple_type_decl:
    sys_type {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   LP { fa[++top] = newNode("name_list", MAXLINE, NULL, 0); } name_list RP {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, fa[top--]);
        $<t>$ = p;
    }
|   const_value DOTDOT const_value {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   MINUS const_value DOTDOT const_value {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   MINUS const_value DOTDOT MINUS const_value {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID DOTDOT ID {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

sys_type:
    INTEGER_TYPE {
        p = newNode("sys_type", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   CHAR_TYPE {
        p = newNode("sys_type", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   REAL_TYPE {
        p = newNode("sys_type", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   BOOLEAN_TYPE {
        p = newNode("sys_type", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

array_type_decl:
    ARRAY LB simple_type_decl RB OF type_decl {
        p = newNode("array_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $6);
        addLeftChild(p, $3);
        $<t>$ = p;
    }
;

record_type_decl:
    RECORD {fa[++top] = newNode("record_type_decl", MAXLINE, NULL, 0); fa[++top] = newNode("name_list", MAXLINE, NULL, 0);} field_decl_list END {
        --top; // delete namelist fa
        $<t>$ = fa[top--];
    }
;

field_decl_list:
    field_decl_list field_decl {
        //exit(0);
        addRightChild(fa[top - 1], $2);
        $<t>$ = fa[top - 1]->child;
    }
|   field_decl {
        addRightChild(fa[top - 1], $1);
        $<t>$ = fa[top - 1]->child;
    }
;

field_decl:
    name_list COLON type_decl SEMI {
        p = newNode("field_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, fa[top]);
        fa[top] = newNode("name_list", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

name_list:
    name_list COMMA ID {
        addRightChild(fa[top], $3);
        $<t>$ = fa[top]->child;
    }
|   ID {
        addRightChild(fa[top], $1);
        $<t>$ = fa[top]->child;
    }
;

var_part:
    VAR {fa[++top]=newNode("var_part", MAXLINE, NULL, 0); fa[++top]=newNode("name_list", MAXLINE, NULL, 0);} var_decl_list {
        --top;
        $<t>$ = fa[top--];
    }
|   /* empty */ {
        p = newNode("var_part", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

var_decl_list:
    var_decl_list var_decl {
        addRightChild(fa[top-1], $2);
        $<t>$ = fa[top-1]->child;
    }
|   var_decl {
        addRightChild(fa[top-1], $1);
        $<t>$ = fa[top-1]->child;
    }
;

var_decl:
    name_list COLON type_decl SEMI {
        p = newNode("var_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, fa[top]);
        fa[top]=newNode("name_list", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

routine_part:
    routine_part function_decl {
        addRightChild(fa[top], $2);
    }
|   routine_part procedure_decl {
        addLeftChild(fa[top], $2);
    }
|   function_decl {
        addLeftChild(fa[top], $1);
    }
|   procedure_decl {
        addLeftChild(fa[top], $1);
    }
|   /* empty */ {
    }
;

function_decl:
    function_head SEMI sub_routine SEMI {
        p = newNode("function_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

function_head:
    FUNCTION ID parameters COLON simple_type_decl {
        p = newNode("function_head", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

procedure_decl:
    procedure_head SEMI sub_routine SEMI {
        p = newNode("procedure_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

procedure_head:
    PROCEDURE ID parameters {
        p = newNode("procedure_head", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

parameters:
    LP {fa[++top] = newNode("para_decl_list", MAXLINE, NULL, 0);fa[++top] = newNode("name_list", MAXLINE, NULL, 0);} para_decl_list RP {
        p = newNode("parameters", MAXLINE, NULL, 0);
        --top;
        addLeftChild(p, fa[top--]);
        $<t>$ = p;
    }
|   /* empty */ {
        p = newNode("parameters", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

para_decl_list:
    para_decl_list SEMI para_type_list {
        addRightChild(fa[top-1], $3);
        // fa[top] = newNode("name_list", MAXLINE, NULL, 0);
    }
|   para_type_list {
        addRightChild(fa[top-1], $1);
    }
;

para_type_list:
    var_para_list COLON simple_type_decl {
        p = newNode("para_type_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   val_para_list COLON simple_type_decl {
        p = newNode("para_type_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

var_para_list: VAR name_list {
        p = newNode("var_para_list", MAXLINE, NULL, 0);
        addLeftChild(p, fa[top]);
        fa[top] = newNode("name_list", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

val_para_list:
    name_list {
        p = newNode("val_para_list", MAXLINE, NULL, 0);
        addLeftChild(p, fa[top]);
        fa[top] = newNode("name_list", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

routine_body:
    compound_stmt {
        p = newNode("routine_body", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

compound_stmt: BEG {fa[++top] = newNode("stmt_list", MAXLINE, NULL, 0);}stmt_list END {
        $<t>$ = newNode("compound_stmt", MAXLINE, NULL, 0);
        addLeftChild($<t>$, fa[top--]);
    }
;

stmt_list:
    stmt_list stmt SEMI {
        addRightChild(fa[top], $2);
    }
|   /* empty */ {
    }
;

stmt:
    INTEGER COLON non_label_stmt {
        p = newNode("stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   non_label_stmt {
        p = newNode("stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

non_label_stmt:
    assign_stmt {$<t>$ = $1;}
|   proc_stmt {$<t>$ = $1;}
|   compound_stmt {$<t>$ = $1;}
|   if_stmt {$<t>$ = $1;}
|   repeat_stmt {$<t>$ = $1;}
|   while_stmt {$<t>$ = $1;}
|   for_stmt {$<t>$ = $1;}
|   case_stmt {$<t>$ = $1;}
|   goto_stmt {$<t>$ = $1;}
;

assign_stmt:
    ID ASSIGN expression {
        p = newNode("assign_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID LB expression RB ASSIGN expression {
        p = newNode("assign_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $6);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID DOT ID ASSIGN expression {
        p = newNode("assign_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

proc_stmt:
    ID {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID LP {fa[++top] = newNode("args_list", MAXLINE, NULL, 0);}args_list RP {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_proc {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_proc LP {fa[++top] = newNode("expression_list", MAXLINE, NULL, 0);}expression_list RP {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        
        $<t>$ = p;
    }
|   READ LP factor RP {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

sys_proc:
    WRITE {
        p = newNode("sys_proc", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   WRITELN {
        p = newNode("sys_proc", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

if_stmt:
    IF expression THEN stmt else_clause {
        p = newNode("if_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        // addLeftChild(p, $3);
        addLeftChild(p, $2);
        // addLeftChild(p, $1);
        $<t>$ = p;
    }
;

else_clause:
    ELSE stmt {
        p = newNode("else_clause", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        // addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        $<t>$ = newNode("else_clause", MAXLINE, NULL, 0);
    }
;

repeat_stmt:
    REPEAT {fa[++top] = newNode("stmt_list", MAXLINE, NULL, 0); } stmt_list UNTIL expression {
        p = newNode("repeat_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, fa[top--]);
        $<t>$ = p;
    }
;

while_stmt:
    WHILE expression DO stmt {
        p = newNode("while_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

for_stmt:
    FOR ID ASSIGN expression direction expression DO stmt {
        p = newNode("for_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $8);
        addLeftChild(p, $6);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

direction:
    TO {
        p = newNode("direction", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   DOWNTO {
        p = newNode("direction", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

case_stmt:
    CASE expression OF {fa[++top] = newNode("case_expr_list", MAXLINE, NULL, 0); }case_expr_list END {
        p = newNode("case_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

case_expr_list:
    case_expr_list case_expr {
        addRightChild(fa[top], $2);
    }
|   case_expr {
        addRightChild(fa[top], $1);
    }
;

case_expr:
    const_value COLON stmt SEMI {
        p = newNode("case_expr", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID COLON stmt SEMI {
        p = newNode("case_expr", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

goto_stmt:
    GOTO INTEGER {
        p = newNode("goto_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
;

expression_list:
    expression_list COMMA expression {
        addRightChild(fa[top], $3);
    }
|   expression {
        addRightChild(fa[top], $1);
    }
;

expression: 
    expression GE expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expression GT expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expression LE expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expression LT expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expression EQUAL expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expression UNEQUAL expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild($2, $3);
        addLeftChild($2, $1->child);
        addLeftChild(p, $2);
        $<t>$ = p;
    }
|   expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;
expr:
    expr PLUS term {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   expr MINUS term {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   expr OR term {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   term {
        $<t>$ = $1;
    }
;

term:
    term MUL factor {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   term DIV factor {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   term MOD factor {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   term AND factor {
        addLeftChild($2, $3);
        addLeftChild($2, $1);
        $<t>$ = $2;
    }
|   factor {
        $<t>$ = $1;
    }
;

factor:
    ID {
        $<t>$ = $1;
    }
|
    ID LP {fa[++top] = newNode("args_list", MAXLINE, NULL,  0);}args_list RP {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_funct LP {fa[++top] = newNode("args_list", MAXLINE, NULL, 0);} args_list RP {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, fa[top--]);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   const_value {
        $<t>$ = $1;
    }
|   LP expression RP {
        $<t>$ = $2->child;
    }
|
    NOT factor {
        addLeftChild($1, $2);
        $<t>$ = $1;
    }
|   MINUS factor{
        addLeftChild($1, $2);
        $<t>$ = $1;
    }
|   ID LB expression RB {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3->child);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID DOT ID {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

sys_funct:
    ABS {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   CHR {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ODD {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ORD {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   PRED {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SQR {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SQRT {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SUCC {
        p = newNode("sys_funct", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }

args_list:
    args_list COMMA expression {
        addRightChild(fa[top], $3);
    }
|   expression {
        addRightChild(fa[top], $1);
    }
;
%%

pTNode buildTree()
{
    yyparse();
    return p;
}

/*
int main() {
    // freopen("../test.spl", "r", stdin);
    dfs(buildTree(), NULL, 0);
    return 0;
}
*/