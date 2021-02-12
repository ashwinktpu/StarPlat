%{
	
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <iostream>
	#include <stdbool.h>
    #include "includeHeader.hpp"
	//#include "y.tab.h"
     
	void yyerror(char *);
	int yylex(void);
    extern FILE* yyin;

	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;
	extern SymbolTable* symbTab;
	FrontEndContext *FrontEndContext::instance=0;
    //symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());

	//Modularised Rule Functions
	Identifier* on_seeing_identifier(char *id_token){
		string s(id_token);
		Identifier* funcId=(Identifier*)Util::createIdentifierNode(id_token);
		return funcId;
	}

%}

/* This is the yacc file in use for the DSL. The action part needs to be modified completely*/

%union {
    int  info;
    long ival;
	bool bval;
    double fval;
    char* text;
	ASTNode* node;
	Identifier* idnode;
	paramList* pList;
	argList* aList;
	ASTNodeList* nodeList;
    tempNode* temporary;
     }
%token T_INT T_FLOAT T_BOOL T_DOUBLE  T_LONG
%token T_FORALL T_FOR  T_P_INF  T_INF T_N_INF
%token T_FUNC T_IF T_ELSE T_WHILE T_RETURN T_DO T_IN T_FIXEDPOINT T_UNTIL T_FILTER
%token T_ADD_ASSIGN T_SUB_ASSIGN T_MUL_ASSIGN T_DIV_ASSIGN T_MOD_ASSIGN T_AND_ASSIGN T_XOR_ASSIGN
%token T_OR_ASSIGN T_RIGHT_OP T_LEFT_OP T_INC_OP T_DEC_OP T_PTR_OP T_AND_OP T_OR_OP T_LE_OP T_GE_OP T_EQ_OP T_NE_OP
%token T_AND T_OR T_SUM T_AVG T_COUNT T_PRODUCT T_MAX T_MIN
%token T_GRAPH T_DIR_GRAPH  T_NODE T_EDGE
%token T_NP  T_EP
%token T_LIST T_SET_NODES T_SET_EDGES T_ELEMENTS T_FROM
%token T_BFS T_REVERSE
%token T_ADD_NODES T_ADD_EDGES
%token T_REM_NODES T_REM_EDGES
%token T_NBHRS T_NODES_FRM T_NODES_TO T_EDGES_FRM T_EDGES_TO T_OUT_DEGREE T_IN_DEGREE
%token T_COUNT_OUT_NBRS T_COUNT_IN_NBRS T_GET_SRC T_GET_DST T_GET_EDGE T_GET_NBHR 
%token T_NODES T_EDGES T_NUM_NODES T_NUM_EDGES
%token T_CONTAINS T_TOT_ELEMS T_ISEMP
%token T_ADD_SET_NODE T_ADD_SET_EDGE T_ADD_NODESET T_ADD_EDGESET T_DISCARD T_CLR
%token T_MK_UNDRCTD

%token <text> ID
%token <ival> INT_NUM
%token <fval> FLOAT_NUM
%token <bval> BOOL_VAL

%type <node> function_def function_data function_body param
%type <pList> paramList
%type <node> statement blockstatements assignment declaration proc_call control_flow reduction
%type <node> type1 type2
%type <node> primitive graph collections property
%type <node> leftSide rhs expression oid val boolean_expr tid
%type <node> bfs_abstraction filterExpr reverse_abstraction
%type <nodeList> leftList
%type <node> iteration_cf selection_cf
%type <node> reductionCall
%type<node>  edgelist vertexlist 
%type<node> prop_fns 
%type<info> setops graphNodeOperations graphEdgeOperations
%type <aList> arg_list
%type <ival> reduction_op
%type <temporary>  rightList
%type<idnode> node_id

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
program: function_def {printf("Program Parsed Successfully\n");};

function_def: function_data  function_body  { };

function_data: T_FUNC node_id '(' paramList ')' {  };

paramList: param { };
               | param ',' paramList { };

param : type1 node_id { } ;
               | type2 node_id { };
			   | type2 node_id '(' node_id ')' { };


function_body : blockstatements { };


statements :  {};
	| statements statement {  };

statement: declaration ';'{ };
	| assignment ';'{ };
	| proc_call ';' { };
	| control_flow { };
	| reduction ';'{ };
	| bfs_abstraction { };
	| reverse_abstraction {};
	| blockstatements {};
	| node_id '.' graphNodeOperations '(' vertexlist ')' ';' {};
	| node_id '.' graphEdgeOperations '(' edgelist ')' ';'{};
	| node_id '.' setops '(' node_id ')' ';' {  };
	| node_id '.' T_CLR '(' ')' ';' {};
	| node_id '.' T_MK_UNDRCTD '(' ')' ';' {};

edgelist: '[' expression ',' expression ']' {};
	| '[' expression ',' expression ']' ',' edgelist {};

vertexlist:	expression {};
	| expression ',' vertexlist {};

setops: T_ADD_SET_NODE { $$ = SETOPS_ADD_SET_NODE; };
	| T_ADD_SET_EDGE { $$ = SETOPS_ADD_SET_EDGE; };
	| T_ADD_NODESET { $$ = SETOPS_ADD_NODESET; };
	| T_ADD_EDGESET { $$ = SETOPS_ADD_EDGESET; };
	| T_DISCARD { $$ = SETOPS_DISCARD; };

graphNodeOperations: T_ADD_NODES { $$ = GRAPH_ADD_NODES; };
	| T_REM_NODES {$$ = GRAPH_REM_NODES; };

graphEdgeOperations: T_ADD_EDGES {$$ = GRAPH_ADD_EDGES; };
	| T_REM_EDGES { $$ = GRAPH_REM_EDGES; };

blockstatements : block_begin statements block_end { };

block_begin:'{' {  }

block_end:'}'


declaration : type1 node_id   {};
	| type1 node_id '=' rhs  {};
	| type2 node_id  { };
	| type2 node_id '=' rhs {};

type1: primitive { };
	| graph { };
	| collections { };


node_id: ID { }

primitive: T_INT { };
	| T_FLOAT { };
	| T_BOOL { };
	| T_DOUBLE {  };
    | T_LONG { };

graph : T_GRAPH { };
	|T_DIR_GRAPH { };

collections : T_LIST {};
		|T_SET_NODES '<' node_id '>' { };
                | T_SET_EDGES '<' node_id '>' { };

type2 : T_NODE { };
       | T_EDGE { };
	   | property { };

property : T_NP '<' primitive '>' {  };
              | T_EP '<' primitive '>' {  };
			  | T_NP '<' collections '>'{  };
			  | T_EP '<' collections '>' { };

assignment :  leftSide '=' rhs  { };

rhs : expression { };

expression : proc_call { };
             | expression '+' expression { };
	         | expression '-' expression { };
	         | expression '*' expression {};
	         | expression'/' expression{};
             | expression T_AND_OP expression {};
	         | expression T_OR_OP  expression {};
	         | expression T_LE_OP expression {};
	         | expression T_GE_OP expression{};
			 | expression '<' expression{};
			 | expression '>' expression{};
			 | expression T_EQ_OP expression{};
             | expression T_NE_OP expression{};
		| '(' expression ')' {};
	         | val {}; 
			 | leftSide {  }; 
			 | prop_fns '(' arg_list ')' {};
			 | node_id '.' T_ELEMENTS {};

proc_call : leftSide '(' arg_list ')' {  };

prop_fns: node_id '.'  T_NBHRS {};
	| node_id '.'  T_NODES_FRM {};
	| node_id '.'  T_NODES_TO {};
	| node_id '.'  T_EDGES_FRM {};
	| node_id '.'  T_EDGES_TO {};
	| node_id '.'  T_OUT_DEGREE {};
	| node_id '.'  T_IN_DEGREE {};
	| node_id '.'  T_COUNT_OUT_NBRS {};
	| node_id '.'  T_COUNT_IN_NBRS {};
	| node_id '.'  T_GET_SRC {};
	| node_id '.'  T_GET_DST {};
	| node_id '.'  T_GET_EDGE {};
	| node_id '.'  T_GET_NBHR {};
	| node_id '.'  T_NODES {};
	| node_id '.'  T_EDGES {};
	| node_id '.'  T_NUM_NODES {};
	| node_id '.'  T_NUM_EDGES {};
	| node_id '.'  T_CONTAINS {};
	| node_id '.'  T_TOT_ELEMS {};
	| node_id '.'  T_ISEMP {};

val : INT_NUM { };
	| FLOAT_NUM { };
	| BOOL_VAL { };
	| T_INF { };
	| T_P_INF { };
	| T_N_INF { };

control_flow : selection_cf {  };
              | iteration_cf {  };

iteration_cf : T_FIXEDPOINT T_UNTIL '(' boolean_expr ')' blockstatements { };
		   | T_WHILE '(' boolean_expr')' blockstatements {  };
		   | T_DO blockstatements T_WHILE '(' boolean_expr ')' { };
		| T_FORALL '(' node_id T_IN node_id '.' proc_call filterExpr')'  blockstatements { };
		| T_FORALL '(' node_id T_IN  node_id '.' T_ELEMENTS filterExpr')'  blockstatements {};
		| T_FORALL '(' node_id T_IN prop_fns '(' arg_list ')' filterExpr')'  blockstatements {};
		| T_FOR '(' node_id T_IN leftSide ')' blockstatements {};
		| T_FOR '(' node_id T_IN node_id '.' proc_call  filterExpr')' blockstatements { };

filterExpr  :         { };
            |'.' T_FILTER '(' boolean_expr ')'{ };

boolean_expr : expression { };

selection_cf : T_IF '(' boolean_expr ')' blockstatements {  };
	           | T_IF '(' boolean_expr ')' blockstatements T_ELSE blockstatements  { };


reduction : leftSide '=' reductionCall { }
		   |'<' leftList '>' '=' '<' rightList '>'  { };

leftList :  leftSide ',' leftList { };
		 | leftSide { };

rightList : reductionCall ',' val {  };
          | reductionCall { } ;

reductionCall : reduction_op '(' arg_list ')' { } ;

reduction_op : T_SUM { };
	         | T_COUNT { };
	         | T_PRODUCT {};
	         | T_MAX {};
	         | T_MIN { };

leftSide : node_id { };
         | oid { };
         | tid { };

arg_list :    { };
    |assignment ',' arg_list { };
    |   expression ',' arg_list   {};
    | expression { };
    | assignment { };


bfs_abstraction	: T_BFS '(' node_id ':' T_FROM node_id ')' filterExpr blockstatements reverse_abstraction{ };
			| T_BFS '(' node_id ':' T_FROM node_id ')' filterExpr blockstatements { };



reverse_abstraction : T_REVERSE '(' boolean_expr ')' filterExpr blockstatements { };


oid : node_id '.' node_id {  };

tid : node_id '.' node_id '.' node_id {  };

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(int argc,char **argv) {
	
	//Parser running in Debug mode
	/*#ifdef YYDEBUG
		yydebug = 1;
	#endif*/
  
   FILE    *fd;

    if (argc>1)
     yyin= fopen(argv[1],"r");
	else 
	  yyin=stdin;
	yyparse();

	return 0;   
	 
}