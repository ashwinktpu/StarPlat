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
	Identifier* process_identifier(char *id_token){
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
%type<node> graphNodeOperations graphEdgeOperations edgelist vertexlist 
%type<node> prop_fns setops
%type <aList> arg_list
%type <ival> reduction_op
%type <temporary>  rightList
%type<node> node_id

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

function_data: T_FUNC node_id '(' paramList ')' { $$=Util::createFuncNode($2,$4->PList); };

paramList: param {$$=Util::createPList($1);};
               | param ',' paramList {$$=Util::addToPList($3,$1); };

param : type1 node_id {  $$=Util::createParamNode($1,$2); } ;
               | type2 node_id {  $$=Util::createParamNode($1,$2);};
			   | type2 node_id '(' node_id ')' {  
				                        Type* tempType=(Type*)$1;
			                            if(tempType->isNodeEdgeType())
										   tempType->addSourceGraph((Identifier*)$4);
				                         $$=Util::createParamNode(tempType,$2);
									 };


function_body : blockstatements {$$=$1;};


statements :  {};
	| statements statement { Util::addToBlock($2); };

statement: declaration ';'{$$=$1;};
	| assignment ';'{$$=$1;};
	| proc_call ';' {$$=Util::createNodeForProcCallStmt($1);};
	| control_flow {$$=$1;};
	| reduction ';'{$$=$1;};
	| bfs_abstraction {$$=$1; };
	| reverse_abstraction {$$=$1;};
	| blockstatements {$$=$1;};
	| node_id '.' graphNodeOperations '(' vertexlist ')' ';' {};
	| node_id '.' graphEdgeOperations '(' edgelist ')' ';'{};
	| node_id '.' setops '(' node_id ')' ';' {};
	| node_id '.' T_CLR '(' ')' ';' {};
	| node_id '.' T_MK_UNDRCTD '(' ')' ';' {};

edgelist: '[' expression ',' expression ']' {};
	| '[' expression ',' expression ']' ',' edgelist {};

vertexlist:	expression {};
	| expression ',' vertexlist {};

setops: T_ADD_SET_NODE {};
	| T_ADD_SET_EDGE {};
	| T_ADD_NODESET {};
	| T_ADD_EDGESET {};
	| T_DISCARD {};

graphNodeOperations: T_ADD_NODES {};
	| T_REM_NODES {};

graphEdgeOperations: T_ADD_EDGES {};
	| T_REM_EDGES {};

blockstatements : block_begin statements block_end { $$=Util::finishBlock();};

block_begin:'{' { Util::createNewBlock(); }

block_end:'}'


declaration : type1 node_id   {$$=Util::createNormalDeclNode($1,(Identifier*)$2);};
	| type1 node_id '=' rhs  {$$=Util::createAssignedDeclNode($1,(Identifier*)$2,$4);};
	| type2 node_id  { $$=Util::createNormalDeclNode($1,(Identifier*)$2); };
	| type2 node_id '=' rhs { $$=Util::createAssignedDeclNode($1,(Identifier*)$2,$4); };

type1: primitive {$$=$1; };
	| graph {$$=$1;};
	| collections { $$=$1;};


primitive: T_INT { $$=Util::createPrimitiveTypeNode(TYPE_INT);};
	| T_FLOAT { $$=Util::createPrimitiveTypeNode(TYPE_FLOAT);};
	| T_BOOL { $$=Util::createPrimitiveTypeNode(TYPE_BOOL);};
	| T_DOUBLE { $$=Util::createPrimitiveTypeNode(TYPE_DOUBLE); };
    | T_LONG {$$=$$=Util::createPrimitiveTypeNode(TYPE_LONG);};

graph : T_GRAPH { $$=Util::createGraphTypeNode(TYPE_GRAPH,NULL);};
	|T_DIR_GRAPH {$$=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);};

collections : T_LIST { $$=Util::createCollectionTypeNode(TYPE_LIST,NULL);};
		|T_SET_NODES '<' node_id '>' {$$=Util::createCollectionTypeNode(TYPE_SETN,(Identifier*)$3);};
                | T_SET_EDGES '<' node_id '>' { $$=Util::createCollectionTypeNode(TYPE_SETE,(Identifier*)$3);};

type2 : T_NODE {$$=Util::createNodeEdgeTypeNode(TYPE_NODE) ;};
       | T_EDGE {$$=Util::createNodeEdgeTypeNode(TYPE_EDGE);};
	   | property {$$=$1;};

property : T_NP '<' primitive '>' { $$=Util::createPropertyTypeNode(PROP_NODE,$3); };
              | T_EP '<' primitive '>' { $$=Util::createPropertyTypeNode(PROP_EDGE,$3); };
			  | T_NP '<' collections '>'{  $$=Util::createPropertyTypeNode(PROP_NODE,$3); };
			  | T_EP '<' collections '>' {$$=Util::createPropertyTypeNode(PROP_EDGE,$3);};

assignment :  leftSide '=' rhs  { $$=Util::createAssignmentNode($1,$3);};

rhs : expression { $$=$1;};

expression : proc_call { $$=$1;};
             | expression '+' expression { $$=Util::createNodeForArithmeticExpr($1,$3,OPERATOR_ADD);};
	         | expression '-' expression { $$=Util::createNodeForArithmeticExpr($1,$3,OPERATOR_SUB);};
	         | expression '*' expression {$$=Util::createNodeForArithmeticExpr($1,$3,OPERATOR_MUL);};
	         | expression'/' expression{$$=Util::createNodeForArithmeticExpr($1,$3,OPERATOR_DIV);};
             | expression T_AND_OP expression {$$=Util::createNodeForLogicalExpr($1,$3,OPERATOR_AND);};
	         | expression T_OR_OP  expression {$$=Util::createNodeForLogicalExpr($1,$3,OPERATOR_OR);};
	         | expression T_LE_OP expression {$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_LE);};
	         | expression T_GE_OP expression{$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_GE);};
			 | expression '<' expression{$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_LT);};
			 | expression '>' expression{$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_GT);};
			 | expression T_EQ_OP expression{$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_EQ);};
             | expression T_NE_OP expression{$$=Util::createNodeForRelationalExpr($1,$3,OPERATOR_NE);};
		| '(' expression ')' {$$=$2;};
	         | val {$$=$1;}; 
			 | leftSide { $$=$1 ;}; 
			 | prop_fns '(' arg_list ')' {};
			 | node_id '.' T_ELEMENTS

proc_call : leftSide '(' arg_list ')' { $$=Util::createNodeForProcCall($1,$3->AList); };

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

val : INT_NUM { 
                $$=Util::createNodeForIval($1); };
	| FLOAT_NUM {$$=Util::createNodeForFval($1);};
	| BOOL_VAL {$$=Util::createNodeForBval($1);};
	| T_INF {$$=Util::createNodeForINF(true);};
	| T_P_INF {$$=Util::createNodeForINF(true);};
	| T_N_INF {$$=Util::createNodeForINF(false);};

control_flow : selection_cf { $$=$1; };
              | iteration_cf { $$=$1; };

iteration_cf : T_FIXEDPOINT T_UNTIL '(' boolean_expr ')' blockstatements { $$=Util::createNodeForFixedPointStmt($4,$6);};
		   | T_WHILE '(' boolean_expr')' blockstatements {$$=Util::createNodeForWhileStmt($3,$5); };
		   | T_DO blockstatements T_WHILE '(' boolean_expr ')' {$$=Util::createNodeForDoWhileStmt($5,$2);  };
		| T_FORALL '(' node_id T_IN node_id '.' proc_call filterExpr')'  blockstatements {$$=Util::createNodeForForAllStmt($3,$5,$7,$8,$10,true);};
		| T_FORALL '(' node_id T_IN  node_id '.' T_ELEMENTS filterExpr')'  blockstatements {};
		| T_FORALL '(' node_id T_IN prop_fns '(' arg_list ')' filterExpr')'  blockstatements {};
		| T_FOR '(' node_id T_IN leftSide ')' blockstatements {$$=Util::createNodeForForStmt($3,$5,$7,false);};
		| T_FOR '(' node_id T_IN node_id '.' proc_call  filterExpr')' blockstatements {$$=Util::createNodeForForAllStmt($3,$5,$7,$8,$10,false);};

filterExpr  :         { $$=NULL;};
            |'.' T_FILTER '(' boolean_expr ')'{ $$=$4;};

boolean_expr : expression { $$=$1 ;};

selection_cf : T_IF '(' boolean_expr ')' blockstatements { $$=Util::createNodeForIfStmt($3,$5,NULL); };
	           | T_IF '(' boolean_expr ')' blockstatements T_ELSE blockstatements  {$$=Util::createNodeForIfStmt($3,$5,$7); };


reduction : leftSide '=' reductionCall { $$=Util::createNodeForReductionStmt($1,$3) ;}
		   |'<' leftList '>' '=' '<' rightList '>'  {$$=Util::createNodeForReductionStmtList($2->ASTNList,$6->reducCall,$6->exprVal);};

leftList :  leftSide ',' leftList { $$=Util::addToNList($3,$1);};
		 | leftSide { $$=Util::createNList($1);};

rightList : reductionCall ',' val { $$=new tempNode();
	                                $$->reducCall=(reductionCall*)$1;
                                    $$->exprVal=(Expression*)$3; };
          | reductionCall { 
			                $$->reducCall=(reductionCall*)$1;} ;

reductionCall : reduction_op '(' arg_list ')' {$$=Util::createNodeforReductionCall($1,$3->AList);} ;

reduction_op : T_SUM { $$=REDUCE_SUM;};
	         | T_COUNT {$$=REDUCE_COUNT;};
	         | T_PRODUCT {$$=REDUCE_PRODUCT;};
	         | T_MAX {$$=REDUCE_MAX;};
	         | T_MIN {$$=REDUCE_MIN;};

leftSide : node_id { $$=$1;};
         | oid { $$=$1; };
         | tid {$$ = $1; };

arg_list :    {argument* a1=new argument();
		                // a1->expression=(Expression*)$1;
				//		 a1->expressionflag=true;
						  $$=Util::createAList(a1);};
						  
		|assignment ',' arg_list {argument* a1=new argument();
		                 a1->assignExpr=(assignment*)$3;
						  a1->assign=true;
						 $$=Util::addToAList($3,a1);
                                 };


	       |   expression ',' arg_list   {argument* a1=new argument();
		                                a1->expression=(Expression*)$3;
						               
										a1->expressionflag=true;
										 $$=Util::addToAList($3,a1);
						                };
	       | expression {argument* a1=new argument();
		                 a1->expression=(Expression*)$1;
						 a1->expressionflag=true;
						  $$=Util::createAList(a1); };
	       | assignment { argument* a1=new argument();
		                  a1->assignExpr=(assignment*)$1;
						  a1->assign=true;
						   $$=Util::createAList(a1);};


bfs_abstraction	: T_BFS '(' node_id ':' T_FROM node_id ')' filterExpr blockstatements reverse_abstraction{$$=Util::createIterateInBFSNode($3,$6,$8,$9,$10) ;};
			| T_BFS '(' node_id ':' T_FROM node_id ')' filterExpr blockstatements {//$$=Util::createIterateInBFSNode($3,$6,$8,$9,$10) ;
			};



reverse_abstraction : T_REVERSE '(' boolean_expr ')' filterExpr blockstatements {$$=Util::createIterateInReverseBFSNode($3,$5,$6);};

oid : node_id '.' node_id { $$=Util::createPropIdNode($1,$3);};

tid : node_id '.' node_id '.' node_id { $$=Util::createPropIdNode($1,$3);};

node_id: ID {$$ = process_identifier($1);}

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