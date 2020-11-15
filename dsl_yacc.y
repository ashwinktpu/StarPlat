%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
	void yyerror(char *);
	int yylex(void);
	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;
%}

%union {
    int  info;
    long ival;
    double fval;
    char* text;
    //bool bval;
    //ast_node* ptr;
    //expr_list* e_list;  // list of expressions
    //lhs_list* l_list;   // list of lhs
    //struct parse_temp_t pair;
}

%token T_INT T_FLOAT T_BOOL T_DOUBLE  T_LONG
%token T_FORALL T_FOR  T_P_INF  T_INF T_N_INF
%token T_IF T_ELSE T_WHILE T_RETURN T_DO T_IN T_FIXEDPOINT T_UNTIL
%token T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN
%token T_OR_ASSIGN T_RIGHT_OP T_LEFT_OP T_INC_OP T_DEC_OP T_PTR_OP T_AND_OP T_OR_OP T_LE_OP T_GE_OP T_EQ_OP T_NE_OP
%token T_AND T_OR T_SUM T_AVG T_COUNT T_PRODUCT T_MAX T_MIN
%token T_GRAPH T_DIR_GRAPH  T_NODE T_EDGE
%token T_NP  T_EP
%token T_SET_NODES T_SET_EDGES T_ELEMENTS

%token <text> ID
%token <ival> INT_NUM
%token <fval> FLOAT_NUM
%token <bval> BOOL_VAL

 /* operator precedence
  * Lower is higher
  */
%left '?'
%left ':'
%left T_OR
%left T_AND 
%left T_EQ T_NEQ
%left '<' '>'  T_LE T_GE
%left '+' '-'
%left '*' '/' '%'


%%
program: lines {printf("program.\n");};

lines: oneline {printf("oneline\n");};
	| oneline lines {printf("oneline lines\n");};

oneline: declaration {printf("declaration\n");};
	|expression {printf("Expression\n");};
	|assignment {printf("assignment\n");};
	|proc_call '{' proc_body '}' {printf("proc call \n");};
	|control_flow {printf("control flow\n");};
	|reduction {printf ("Reduction\n");};

declaration: type ID ';' {printf("declaration\n");};
	| type ID '=' val ';'{};
	| type ID '=' proc_call ';' {};
	| oid '=' val ';'{};
	| proc_call ';' {};
	| T_NP '<' primitive '>' ID ';' {};
	| T_EP '<' primitive '>' ID ';' {};

assignment: ID '=' val {};
	| oid '=' val{};
	| ID '=' expression ';' {};
	| oid '=' expression ';' {};
	| ID '=' proc_call ';'{};
	| oid '=' proc_call ';' {};

expression: ID '+' ID {};
	| ID '-' ID {};
	| ID '*' ID {};
	| ID '/' ID {};
	| oid '+' oid {};
	| oid '-' oid {};
	| oid '*' oid {};
	| oid '/' oid {};


proc_call: ID '(' arg_list ')' { printf("proccall\n");};
	| oid '(' arg_list ')' {};
	| ID '(' arg_list ')' '.' proc_call {};
	| oid '(' arg_list ')' '.' proc_call {};

proc_body : lines {printf("proc_body\n");};

control_flow: T_FORALL '(' ID T_IN proc_call ')' '{' lines '}' {};
	| T_FIXEDPOINT T_UNTIL '(' arg_list ')' '{' lines '}' {};


reduction: '<' lhs '>' '=' '<' rhs '>' ';' {};

lhs: arg_list {};

rhs: op '(' arg_list ')' {};
	| op '(' arg_list ')' ',' val {};

op: T_SUM {};
	| T_AVG {};
	| T_COUNT {};
	| T_PRODUCT {};
	| T_MAX {};
	| T_MIN {};

val : INT_NUM {};
	| FLOAT_NUM {};
	| BOOL_VAL {};
	| T_INF {};
	| T_P_INF {};
	| T_N_INF {};

type : primitive {};
	| graph {};

primitive: T_INT {};
	| T_FLOAT {};
	| T_BOOL {};
	| T_DOUBLE {};
	| T_LONG {};

graph : T_GRAPH {};
	|T_DIR_GRAPH {};
	|T_NODE {};
	|T_EDGE {};


oid: ID '.' ID {printf("oid\n");};

arg_list: 
	| ID ',' arg_list {};
	| oid ',' arg_list{};
	| type ID ',' arg_list {};
	| declaration ',' arg_list {};
	| assignment ',' arg_list {};
	| expression ',' arg_list {};
	| type ID {};
	| ID {};
	| declaration {};
	| expression {};
	| assignment {};
	| oid {};


%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}

