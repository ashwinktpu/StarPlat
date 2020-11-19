
  
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
%token T_IF T_ELSE T_WHILE T_RETURN T_DO T_IN T_FIXEDPOINT T_UNTIL T_FUNC
%token T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN
%token T_OR_ASSIGN T_RIGHT_OP T_LEFT_OP T_INC_OP T_DEC_OP T_PTR_OP T_AND_OP T_OR_OP T_LE_OP T_GE_OP T_EQ_OP T_NE_OP
%token T_AND T_OR T_SUM T_AVG T_COUNT T_PRODUCT T_MAX T_MIN
%token T_GRAPH T_DIR_GRAPH  T_NODE T_EDGE
%token T_NP  T_EP 
%token T_SET_NODES T_SET_EDGES T_ELEMENTS T_LIST T_FROM 
%token T_BFS T_REVERSE

%token <text> ID
%token <ival> INT_NUM
%token <fval> FLOAT_NUM
%token <bval> BOOL_VAL

 /* operator precedence
  * Lower is higher
  */
%left '?'
%left ':'
%left T_OR_OP
%left T_AND_OP 
%left T_EQ_OP  T_NE_OP
%left '<' '>'  T_LE_OP T_GE_OP
%left '+' '-'
%left '*' '/' '%'


%%
program: function_def {printf("program.\n");};

function_def: function_data  function_body  { };

function_data: T_FUNC ID '(' arg_list ')' { };

//paramList: param { }| paramList ',' param { };

//param : type1 ID | type2 ID '(' ID ')' {};


function_body : blockstatements {printf("program.\n");};


statements : statement statements {printf("Statement\n");};
	| statement {printf("statement\n");};

statement: declaration ';'{printf("declaration\n");};
	|assignment ';'{printf("assignment\n");};
	|proc_call ';' {printf("proc call \n");};
	|control_flow {printf("control flow\n");};
	|reduction ';'{printf ("Reduction\n");};
	|bfs_abstraction { };
	| reverse_abstraction {};
	
	
blockstatements :'{' statements '}' { printf("block of statements\n");};
		| statement {};
	
declaration : type1 ID  {};
	| type1 ID '=' rhs {};
	| type2 ID  {};
	| type2 ID '=' rhs {};

type1: primitive {};
	| graph {};
	| collections { };
	
primitive: T_INT {};
	| T_FLOAT {};
	| T_BOOL {};
	| T_DOUBLE {};
	| T_LONG {};

graph : T_GRAPH {};
	|T_DIR_GRAPH {};
	
collections : T_LIST {};
		|T_SET_NODES '<' ID '>' { };
                | T_SET_EDGES '<' ID '>' { };
	
type2 : T_NODE { };
       | T_EDGE { };
	   | property { };

property : T_NP '<' primitive '>' { printf("node property\n");};
              | T_EP '<' primitive '>' { };
			  | T_NP '<' collections '>'{ };
			  | T_EP '<' collections '>' { };
	
assignment :  leftSide '=' rhs  {};

rhs : expression { };

expression :  proc_call { };
             | expression '+' expression {};
	         | expression '-' expression {};
	         | expression '*' expression {};
	         | expression'/' expression{};
			 | expression T_AND_OP expression {};
	         | expression T_OR_OP  expression {};
	         | expression T_LE_OP ID {};
	         | expression T_GE_OP expression{};
			 | expression T_EQ_OP expression{};
             | expression T_NE_OP expression{};	
		| '(' expression ')' {};		 
	         | val {};
			 | leftSide { };
			 
proc_call : leftSide '(' arg_list ')' { };
           |leftSide '(' arg_list ')' '.' proc_call {}; 
		| oid '.' ID '(' arg_list ')' {};

			 
val : INT_NUM {};
	| FLOAT_NUM {};
	| BOOL_VAL {};
	| T_INF {};
	| T_P_INF {};
	| T_N_INF {};
	
control_flow : selection_cf { };
              | iteration_cf { };

iteration_cf : T_FIXEDPOINT T_UNTIL '(' arg_list ')' blockstatements {};
		   | T_WHILE '(' boolean_expr')' blockstatements { };
		   | T_DO blockstatements T_WHILE '(' boolean_expr ')' { };
		   | T_FORALL '(' ID T_IN proc_call ')' blockstatements {};
		| T_FORALL '(' ID T_IN proc_call ')' filterExpr blockstatements {};
		| T_FOR '(' ID T_IN leftSide ')' blockstatements {};
		| T_FOR '(' ID T_IN proc_call ')' blockstatements {};
		   
filterExpr  : '.' proc_call { };

boolean_expr : expression { };

selection_cf : T_IF '(' boolean_expr ')' statements { };
              | T_IF '(' boolean_expr ')' statements T_ELSE statements  { };
			  

reduction : leftSide '=' reductionCall { } 
		   |'<' leftList '>' '=' '<' rightList '>'  {}; 

leftList : leftSide ',' leftList { };
		 | leftSide { };

rightList : reductionCall ',' val { };
          | reductionCall ;
		  
reductionCall : reduction_op '(' arg_list ')' {} ;

reduction_op : T_SUM {};
	         | T_COUNT {};
	         | T_PRODUCT {};
	         | T_MAX {};
	         | T_MIN {};
			 
leftSide : ID { };
         | oid { };

arg_list : 
           | leftSide ',' arg_list {};
	        
           | assignment ',' arg_list {};
	       | expression ',' arg_list {};
		| declaration ',' arg_list {};
	       | declaration {};
	       | expression {};
	       | assignment {};
	
		   
bfs_abstraction	: T_BFS '(' ID ':' T_FROM ID ')'  blockstatements {};
                 | T_BFS '(' ID ':' T_FROM ID ')' filterExpr blockstatements { };

 

reverse_abstraction : T_REVERSE '(' arg_list ')' blockstatements {};
                    | T_REVERSE filterExpr blockstatements {};

oid : ID '.' ID {printf("oid\n");};
 
%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}

