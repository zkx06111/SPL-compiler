%{
#include "tree.h"
//#include "token.h"
#include <stdio.h>
#include "lex.yy.c"
#define PARSER
struct TreeNode* p;
struct TreeNode* fa;
struct TreeNode* stk[100];
int idx;
const int MAXLINE = 2147483647;
void yyerror(const char* msg) {
    printf("%s\n", msg);
}
%}
%union{
    struct TreeNode* t;
}
%token <t> LP RP LB RB DOT COMMA COLON MUL DIV UNEQUAL NOT PLUS MINUS GE GT LE LT EQUAL ASSIGN MOD DOTDOT SEMI FALSE MAXINT TRUE ABS CHR ODD PRED SQRT SUCC WRITE WRITELN BOOLEAN_TYPE CHAR_TYPE INTEGER_TYPE REAL_TYPE READ INTEGER REAL CHAR AND ARRAY BEG CASE CONST DO DOWNTO ELSE END FOR FUNCTION GOTO IF OF OR PACKED PROCEDURE PROGRAM RECORD REPEAT THEN TO TYPE UNTIL VAR WHILE ID SQR ORD LABEL
%type <t> program program_head routine sub_routine routine_head label_part const_part const_expr_list const_value type_part type_decl_list type_definition type_decl simple_type_decl sys_type array_type_decl record_type_decl field_decl_list field_decl name_list var_part var_decl_list var_decl routine_part function_decl function_head procedure_decl procedure_head parameters para_decl_list para_type_list var_para_list val_para_list routine_body compound_stmt stmt_list stmt non_label_stmt assign_stmt proc_stmt if_stmt else_clause repeat_stmt while_stmt for_stmt direction case_stmt case_expr_list case_expr goto_stmt expression_list expression expr term factor args_list sys_proc sys_funct label_list
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
        p = newNode("subroutine", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

routine_head:
    label_part const_part type_part var_part routine_part {
        p = newNode("routine_head", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

label_part:
    LABEL {fa = newNode("label_part", MAXLINE, NULL, 0);} label_list SEMI {
        $<t>$ = fa;
    }
|   /* empty */ { $<t>$ = newNode("label_part", MAXLINE, NULL, 0); }
;

label_list:
    label_list COMMA INTEGER {
        addRightChild(fa, $3);
        $<t>$ = fa->child;
    }
|   INTEGER {
        addRightChild(fa, $1);
        $<t>$ = fa->child;
    }
;


const_part:
    CONST {fa = newNode("const_part", MAXLINE, NULL, 0);} const_expr_list {
        $<t>$ = fa;
    }
|   { $<t>$ = newNode("const_part", MAXLINE, NULL, 0);}
;

const_expr_list:
    const_expr_list ID EQUAL const_value SEMI {
        addRightChild(fa, $2);
        addRightChild(fa, $4);
        $<t>$ = fa->child;
    }
|   ID EQUAL const_value SEMI {
        addRightChild(fa, $1);
        addRightChild(fa, $3);
        $<t>$ = fa->child;
    }
;

const_value:
    INTEGER {
        p = newNode("int_const_value", MAXLINE, NULL, 0);
        p->vali = $1->vali;
        $<t>$ = p;
    }
|   REAL{
        p = newNode("real_const_value", MAXLINE, NULL, 0);
        p->valf = $1->valf;
        $<t>$ = p;
    }
|   CHAR{
        p = newNode("char_const_value", MAXLINE, NULL, 0);
        p->valc = $1->valc;
        $<t>$ = p;
    }
|   FALSE{
        p = newNode("bool_const_value", MAXLINE, NULL, 0);
        p->vali = $1->vali;
        $<t>$ = p;
    }
|   TRUE{
        p = newNode("bool_const_value", MAXLINE, NULL, 0);
        p->vali = $1->vali;
        $<t>$ = p;
    }
|   MAXINT{
        p = newNode("int_const_value", MAXLINE, NULL, 0);
        p->vali = $1->vali;
        $<t>$ = p;
    }
;

type_part:
    TYPE {fa = newNode("type_part", MAXLINE, NULL, 0);} type_decl_list {
        $<t>$ = fa;
    }
|   { $<t>$ = newNode("type_part", MAXLINE, NULL, 0);}
;

type_decl_list:
    type_decl_list type_definition {
        printf("QAQ!");
        addRightChild(fa, $2);
        $<t>$ = fa->child;
    }
|   type_definition {
        addRightChild(fa, $1);
        $<t>$ = fa->child;
        printf("FUCK!\n");
    }
;

type_definition:
    ID EQUAL type_decl SEMI {
        printf("QWQQQ!\n");
        p = newNode("type_definition", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

type_decl:
    simple_type_decl {
        p = newNode("type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
   }
|   array_type_decl {
        p = newNode("type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   record_type_decl {
        p = newNode("type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
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
|   LP name_list RP {
        p = newNode("simple_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
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
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

record_type_decl:
    RECORD field_decl_list END {
        p = newNode("record_type_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

field_decl_list:
    field_decl_list field_decl {
        p = newNode("field_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   field_decl {
        p = newNode("field_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

field_decl:
    name_list COLON type_decl SEMI {
        p = newNode("field_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

name_list:
    name_list COMMA ID {
        p = newNode("name_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID {
        p = newNode("name_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

var_part:
    VAR var_decl_list {
        p = newNode("var_part", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        p = newNode("var_part", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

var_decl_list:
    var_decl_list var_decl {
        p = newNode("var_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   var_decl {
        p = newNode("var_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

var_decl:
    name_list COLON type_decl SEMI {
        p = newNode("var_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

routine_part:
    routine_part function_decl {
        p = newNode("routine_part", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   routine_part procedure_decl {
        p = newNode("routine_part", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   function_decl {
        p = newNode("routine_part", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   procedure_decl {
        p = newNode("routine_part", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        p = newNode("routine_part", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

function_decl:
    function_head SEMI sub_routine SEMI {
        p = newNode("function_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

function_head:
    FUNCTION ID parameters COLON simple_type_decl {
        p = newNode("function_head", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

procedure_decl:
    procedure_head SEMI sub_routine SEMI {
        p = newNode("procedure_decl", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

procedure_head:
    PROCEDURE ID parameters {
        p = newNode("procedure_head", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

parameters:
    LP para_decl_list RP {
        p = newNode("parameters", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        p = newNode("parameters", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

para_decl_list:
    para_decl_list SEMI para_type_list {
        p = newNode("para_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   para_type_list {
        p = newNode("para_decl_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

para_type_list:
    var_para_list COLON simple_type_decl {
        p = newNode("para_type_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   val_para_list COLON simple_type_decl {
        p = newNode("para_type_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

var_para_list: VAR name_list {
        p = newNode("var_para_list", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

val_para_list:
    name_list {
        p = newNode("val_para_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
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

compound_stmt: BEG stmt_list END {
        p = newNode("compound_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

stmt_list:
    stmt_list stmt SEMI {
        p = newNode("stmt_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        p = newNode("stmt_list", MAXLINE, NULL, 0);
        $<t>$ = p;
    }
;

stmt:
    INTEGER COLON non_label_stmt {
        p = newNode("stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
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
    assign_stmt {$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   proc_stmt {$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   compound_stmt {$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   if_stmt {$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   repeat_stmt{$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   while_stmt{$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   for_stmt{$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   case_stmt{$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
|   goto_stmt{$<t>$ = newNode("non_label_stmt", MAXLINE, NULL, 0);}
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
|   ID LP args_list RP {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_proc {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_proc LP expression_list RP {
        p = newNode("proc_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
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
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

else_clause:
    ELSE stmt {
        p = newNode("else_clause", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   /* empty */ {
        $<t>$ = newNode("else_clause", MAXLINE, NULL, 0);
    }
;

repeat_stmt:
    REPEAT stmt_list UNTIL expression {
        p = newNode("repeat_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

while_stmt:
    WHILE expression DO stmt {
        p = newNode("while_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

for_stmt:
    FOR ID ASSIGN expression direction expression DO stmt {
        p = newNode("for_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $8);
        addLeftChild(p, $7);
        addLeftChild(p, $6);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
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
    CASE expression OF case_expr_list END {
        p = newNode("case_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $5);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

case_expr_list:
    case_expr_list case_expr {
        p = newNode("case_expr_list", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   case_expr {
        p = newNode("case_expr_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

case_expr:
    const_value COLON stmt SEMI {
        p = newNode("case_expr", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID COLON stmt SEMI {
        p = newNode("case_expr", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

goto_stmt:
    GOTO INTEGER {
        p = newNode("goto_stmt", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

expression_list:
    expression_list COMMA expression {
        p = newNode("expression_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression {
        p = newNode("expression_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

expression: 
    expression GE expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression GT expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression LE expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression LT expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression EQUAL expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression UNEQUAL expr {
        p = newNode("expression", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
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
        p = newNode("expr", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expr MINUS term {
        p = newNode("expr", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expr OR term {
        p = newNode("expr", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   term {
        p = newNode("expr", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

term:
    term MUL factor {
        p = newNode("term", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   term DIV factor {
        p = newNode("term", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   term MOD factor {
        p = newNode("term", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   term AND factor {
        p = newNode("term", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   factor {
        p = newNode("term", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;

factor:
    ID {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|
    ID LP args_list RP {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_funct {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   sys_funct LP args_list RP {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   const_value {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   LP expression RP {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|
    NOT factor {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   MINUS factor{
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   ID LB expression RB {
        p = newNode("factor", MAXLINE, NULL, 0);
        addLeftChild(p, $4);
        addLeftChild(p, $3);
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
        p = newNode("sys_func", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   PRED {
        p = newNode("sys_func", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SQR {
        p = newNode("sys_func", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SQRT {
        p = newNode("sys_func", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   SUCC {
        p = newNode("sys_func", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }

args_list:
    args_list COMMA expression {
        p = newNode("args_list", MAXLINE, NULL, 0);
        addLeftChild(p, $3);
        addLeftChild(p, $2);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
|   expression {
        p = newNode("args_list", MAXLINE, NULL, 0);
        addLeftChild(p, $1);
        $<t>$ = p;
    }
;
%%

int idx = 0;

void printNode(pTNode p) {
    if (p == NULL) printf("NULL");
    else printf("%s", p->type);
}

void dfs(pTNode cur, pTNode par, int faid) {
    printNode(par); printf(" %d", faid);
    printf(" ");
    int curid = ++idx;
    printNode(cur); printf(" %d", curid);
    printf("\n");
    for (pTNode t = cur->child; t; t = t->sibling) {
        dfs(t, cur, curid);
    }
}

pTNode buildTree()
{
    yyparse();
    return p;
}

int main() {
    freopen("../test.spl", "r", stdin);
    dfs(buildTree(), NULL, 0);
    return 0;
}