
  %{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include "ASTNodeTypes.h"
	#include "SymbolTable.h"
	#include "MainContext.h"
	#include "ASTNode.h"
    #include "enum_def.h"
    
    
	//#include <list>

	void yyerror(char *);
	int yylex(void);
	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;
	extern SymbolTable* symbTab;
	
	symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());
%}

/* This is the yacc file in use for the DSL. The action part needs to be modified completely*/

%union {
    int  info;
    long ival;
    double fval;
    char* text;
	ASTNode* node;
	argList* aList;
	ASTNodeList* nodeList;
    
    struct tempNode temporary;
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


%token <text> ID
%token <ival> INT_NUM
%token <fval> FLOAT_NUM
%token <bval> BOOL_VAL

%type <node> function_def function_data function_body param
%type <pList> paramList
%type <node> statement blockstatements assignment declaration proc_call control_flow reduction 
%type <node> type1 type2
%type <node> primitive graph collections property
%type <node> id leftSide rhs expression oid val boolean_expr
%type <node> bfs_abstraction filterExpr reverse_abstraction
%type <nodeList> leftList
%type <node> iteration_cf selection_cf
%type <node> reductionCall
%type <aList> arg_list
%type <ival> reduction_op
%type <temporary>  rightList




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

function_data: T_FUNC ID '(' paramList ')' {Identifier* funcId=createIdentifierNode($2);
                                           $$=createFunctionNode(funcId,$4); };

paramList: param {$$=addToPList($$,$1);};
               | paramList ',' param {$$=addToPList($$,$3); };

param : type1 ID { Identifier* id=createIdentifierNode($2);
                           $$=createParamNode($1,id); } ;
               | type2 ID {  Identifier* id=createIdentifierNode($2);
                             $$=createParamNode($1,id);};
			   | type2 ID '(' ID ')' {  Identifier* id1=createIdentifierNode($4);
			                            Identifier* id=createIdentifierNode($2);
				                        Type* tempType=(Type*)$1;
			                            if(tempType->isNodeEdgeType())
										   (Type*)$1->addSourceGraph(id1);
				                         $$=createParamNode($1,id);
									 }; 


function_body : blockstatements {$$=$1;};


statements :  {};
	| statements statement { addToBlock($2); };

statement: declaration ';'{$$=$1};
	|assignment ';'{$$=$1;};
	|proc_call ';' {$$=createNodeForProcCallStmt($1);};
	|control_flow {$$=$1;};
	|reduction ';'{$$=$1;};
	| bfs_abstraction {$$=$1; };
	| blockstatements {$$=$1;};
	
	
blockstatements : block_begin statements block_end { $$=finishBlock();};

block_begin:'{' { createNewBlock(); }

block_end:'}'
	
	
declaration : type1 ID  {Identifier* id=createIdentifierNode($2);
                         $$=createNormalDeclNode($1,id);};
	| type1 ID '=' rhs {Identifier* id=createIdentifierNode($2);
	                    $$=createAssignedDeclNode($1,id,$4);};
	| type2 ID  {Identifier* id=createIdentifierNode($2);
                         $$=createNormalDeclNode($1,id); };
	| type2 ID '=' rhs {Identifier* id=createIdentifierNode($2);
	                    $$=createAssignedDeclNode($1,id,$4);};

type1: primitive {$$=$1}; 
	| graph {$$=$1};
	| collections { $$=$1};

	
primitive: T_INT { $$=createPrimitiveTypeNode(TYPE_INT);}; 
	| T_FLOAT { $$=createPrimitiveTypeNode(TYPE_FLOAT);};
	| T_BOOL { $$=createPrimitiveTypeNode(TYPE_BOOL);};
	| T_DOUBLE { $$=createPrimitiveTypeNode(TYPE_DOUBLE); };
    | T_LONG {$$=$$=createPrimitiveTypeNode(TYPE_LONG);};

graph : T_GRAPH { $$=createGraphTypeNode(TYPE_GRAPH,NULL);};
	|T_DIR_GRAPH {$$=createGraphTypeNode(TYPE_DIRGRAPH,NULL);};
	
collections : T_LIST { $$=createCollectionTypeNode(TYPE_LIST,NULL);};
		|T_SET_NODES '<' ID '>' { $$=createCollectionTypeNode(TYPE_SETN,$3);};
                | T_SET_EDGES '<' ID '>' { $$=createCollectionTypeNode(TYPE_SETE,$3);};
	
type2 : T_NODE {$$=createNodeEdgeTypeNode(TYPE_NODE) };
       | T_EDGE {$$=createNodeEdgeTypeNode(TYPE_EDGE)};
	   | property {$$=$1;};

property : T_NP '<' primitive '>' { $$=createPropertyTypeNode(PROP_NODE,$3); };
              | T_EP '<' primitive '>' { $$=createPropertyTypeNode(PROP_EDGE,$3); };
			  | T_NP '<' collections '>'{  $$=createPropertyTypeNode(PROP_NODE,$3); };
			  | T_EP '<' collections '>' {$$=createPropertyTypeNode(PROP_EDGE,$3);};
	
assignment :  leftSide '=' rhs  { $$=createAssignmentNode($1,$3);};

rhs : expression { $$=$1;};

expression : proc_call { $$=$1};
             | expression '+' expression { $$=createNodeForArithmeticExpr($1,$3,OPERATOR_ADD);};
	         | expression '-' expression { $$=createNodeForArithmeticExpr($1,$3,OPERATOR_SUB);};
	         | expression '*' expression {$$=createNodeForArithmeticExpr($1,$3,OPERATOR_MUL);};
	         | expression'/' expression{$$=createNodeForArithmeticExpr($1,$3,OPERATOR_DIV);};
             | expression T_AND_OP expression {$$=createNodeForLogicalExpr($1,$3,OPERATOR_AND);};
	         | expression T_OR_OP  expression {$$=createNodeForLogicalExpr($1,$3,OPERATOR_OR);};
	         | expression T_LE_OP expression {$$=createNodeForRelationalExpr($1,$3,OPERATOR_LE);};
	         | expression T_GE_OP expression{$$=createNodeForRelationalExpr($1,$3,OPERATOR_GE);};
			 | expression '<' expression{$$=createNodeForRelationalExpr($1,$3,OPERATOR_LT);};
			 | expression '>' expression{$$=createNodeForRelationalExpr($1,$3,OPERATOR_GT);};
			 | expression T_EQ_OP expression{$$=createNodeForRelationalExpr($1,$3,OPERATOR_EQ);};
             | expression T_NE_OP expression{$$=$$=createNodeForRelationalExpr($1,$3,OPERATOR_NE);};	
		| '(' expression ')' {$$=$2;};		 
	         | val {$$=$1;};
			 | leftSide { $$=$1 ;};
			 
proc_call : leftSide '(' arg_list ')' { $$=createNodeForProcCall($1,$3); };
	

			 
val : INT_NUM { $$=createNodeForIval($1); };
	| FLOAT_NUM {$$=createNodeForFval($1);};
	| BOOL_VAL {$$=createNodeForBval($1);};
	| T_INF {$$=createNodeForINF(true);};
	| T_P_INF {$$=createNodeForINF(true);};
	| T_N_INF {$$=createNodeForINF(false);};
	
control_flow : selection_cf { $$=$1; };
              | iteration_cf { $$=$1; };

iteration_cf : T_FIXEDPOINT T_UNTIL '(' boolean_expr ')' blockstatements { $$=createNodeForFixedPointStmt($4,$6);};
		   | T_WHILE '(' boolean_expr')' blockstatements {$$=createNodeForWhileStmt($3,$5); };
		   | T_DO blockstatements T_WHILE '(' boolean_expr ')' {$$=createNodeForDoWhileStmt($5,$2);  };
		| T_FORALL '(' ID T_IN ID '.' proc_call filterExpr')'  blockstatements {$$=createNodeForForAllStmt($3,$5,$7,$8,$10,true);};
		| T_FOR '(' ID T_IN leftSide ')' blockstatements {$$=createNodeForForStmt($3,$5,$7,false);};
		| T_FOR '(' ID T_IN ID '.' proc_call  filterExpr')' blockstatements {$$=createNodeForForAllStmt($3,$5,$7,$8,$10,false);};
		   
filterExpr  :         { $$=NULL;};
            |'.' T_FILTER '(' boolean_expr ')'{ $$=$4;};

boolean_expr : expression { $$=$1 ;};

selection_cf : T_IF '(' boolean_expr ')' blockstatements { $$=createNodeForIfStmt($3,$5,NULL); };
              | T_IF '(' boolean_expr ')' blockstatements T_ELSE blockstatements  {$$=createNodeForIfStmt($3,$5,$7); };
			  

reduction : leftSide '=' reductionCall { $$=createNodeForReductionStmt($1,$3) } 
		   |'<' leftList '>' '=' '<' rightList '>'  {$$=createNodeForReductionStmtList($2,$6.reducCall,$6.exprVal);}; 

leftList :  leftList ',' leftSide { $$=addToNList($$,$3);};
		 | leftSide { $$=addToNList($$,$1);};

rightList : reductionCall ',' val { $$.reducCall=$1;
                                    $$.exprVal=$3; };
          | reductionCall {$$.reducCall=$1;} ;
		  
reductionCall : reduction_op '(' arg_list ')' {$$=createNodeforReductionCall($1,$3);} ;

reduction_op : T_SUM { $$=REDUCE_SUM;};
	         | T_COUNT {$$=REDUCE_COUNT;};
	         | T_PRODUCT {$$=REDUCE_PRODUCT;};
	         | T_MAX {$$=REDUCE_MAX;};
	         | T_MIN {$$=REDUCE_MIN;};
			 
leftSide : ID { $$=createIdentifierNode($1);};
         | oid { $$=$1; };

arg_list :   arg_list ',' assignment {argument a1;
		                 a1.expression=$1;
						 a1.expressionflag=true;
						 $$=addToAList($$,a1);
                                 };


	       |  arg_list ',' expression  {argument a1;
		                                a1.assignExpr=$1;
						                a1.assign=true;
										 $$=addToAList($$,a1);
						                };
	       | expression {argument a1;
		                 a1.expression=$1;
						 a1.expressionflag=true;
						  $$=addToAList($$,a1); };
	       | assignment { argument a1;
		                  a1.assignExpr=$1;
						  a1.assign=true;
						   $$=addToAList($$,a1);};
	
		   
bfs_abstraction	: T_BFS '(' ID ':' T_FROM ID ')' filterExpr blockstatements reverse_abstraction{$$=createIterateInBFSNode($3,$6,$8,$9,$10) ;};

 

reverse_abstraction : T_REVERSE '(' boolean_expr ')' filterExpr blockstatements {$$=createIterateInReverseBFSNode($3,$5,$6)};
                    

oid : ID '.' ID { Identifier* id1=createIdentifierNode($1);
                   Identifier* id12=createIdentifierNode($1);
				   $$=createPropIdNode(id1,id2);
				    };
 
%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}

