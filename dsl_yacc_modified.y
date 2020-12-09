
  
%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <SymbolTable.h>
	#include<Context.cpp>
	#include<FrontEndContext.h>
	#include <list>

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
	| blockstatements {};
	
	
blockstatements : block_begin statements block_end { printf("block of statements\n");};

block_begin:'{' { }

block_end:'}'
	
	
declaration : type1 ID  { 
	                       Symbol* symbol=symbTab->LookUp($2);
						   if(symbol!=NULL)
						     {
								 cerr<<"Error:Symbol"<<$2<<"is already declared";
								 exit(1);
							 }
	                         symbTab->insertSymbol($2,$1);
	                  
						//	symTab->getSymbol(s1)->type=$1->type;
						//	symTab->getSymbol(s1)->enclosedType=$1->enclosedType;
						 //    $$=new genContent();
						//	 $$->type=$1->type;
						//	 $$->place=s1; 
                          

                         };
	| type1 ID '=' rhs {};
	| type2 ID  {         Symbol* symbol=symbTab->LookUp($2);
						   if(symbol!=NULL)
						     {
								 cerr<<"Error:Symbol"<<$2<<"is already declared";
								 exit(1);
							 }
	                         symbTab->insertSymbol($2,$1);
							//symTab->getSymbol(s1)->type=$1->type;
						   //	symTab->getSymbol(s1)->enclosedType=$1->enclosedType;
						  //   $$=new genContent();
							// $$->type=$1->type;
							// $$->place=s1;
							// $$->enclosedType=$1->enclosedType;
							// };
	| type2 ID '=' rhs {};

type1: primitive {$$=$1};
	| graph {$$=$1};
	| collections { $$=$1};
	
primitive: T_INT { genContext* context=new genContext();
                    context->place="int";
					context->type="int";
					$$=context;
                      }; 
	| T_FLOAT { genContext* context=new genContext();
                    context->place="float";
					context->type="float";
					$$=context;
				};
	| T_BOOL {
		          genContext* context=new genContext();
                    context->place="bool";
					context->type="bool";
					$$=context;};
		  
	           };
	| T_DOUBLE { genContext* context=new genContext();
                    context->place="double";
					context->type="double";
					$$=context;};

	| T_LONG {genContext* context=new genContext();
                    context->place="long";
					context->type="long";
					$$=context;};

graph : T_GRAPH {genContext* context=new genContext();
                    context->place="Graph";
					context->type="Graph";
					$$=context;};
	|T_DIR_GRAPH {genContext* context=new genContext();
                    context->place="dirGraph";
					context->type="dirGraph";
					$$=context;};
	
collections : T_LIST {genContext* context=new genContext();
                    context->place="list";
					context->type="list";
					$$=context;};
		|T_SET_NODES '<' ID '>' { 
			         Symbol* symbol=symbTab->LookUp($3);
						   if(symbol!=NULL)
						     {
								 cerr<<"Error:Symbol"<<$3<<"is already declared";
								 exit(1);
							 }
					if(symbol->type!="Graph")
					{
						cerr<<"Type "<<symbol->type<<"not allowed!";
						exit(1);
					}
			      // genContext* context=new genContext();
                   // context->place="SetN";
					//context->type="SetN";
				//	$$=context;};
                | T_SET_EDGES '<' ID '>' {
 Symbol* symbol=symbTab->LookUp($3);
						   if(symbol!=NULL)
						     {
								 cerr<<"Error:Symbol"<<$3<<"is already declared";
								 exit(1);
							 }
					if(symbol->type!="Graph")
					{
						cerr<<"Type "<<symbol->type<<"not allowed!";
						exit(1);
					}
					genContext* context=new genContext();
                    context->place="SetE";
					context->type="SetE";
					$$=context;
				 };
	
type2 : T_NODE { genContext* context=new genContext();
                    context->place="node";
					context->type="node";
					$$=context;};
       | T_EDGE { genContext* context=new genContext();
                    context->place="edge";
					context->type="edge";
					$$=context;};
	   | property {$$=$1;};

property : T_NP '<' primitive '>' { genContext* context=new genContext();
                                    context->place="nodeProperty";
					                context->type="nodeProperty";
									context->enclosedType=$3->type;
	                                $$=context;
};
              | T_EP '<' primitive '>' {genContext* context=new genContext();
                                    context->place="edgeProperty";
					                context->type="edgeProperty";
									context->enclosedType=$3->type;
	                                $$=context; };
			  | T_NP '<' collections '>'{ genContext* context=new genContext();
                                    context->place="nodeProperty";
					                context->type="nodeProperty";
									context->enclosedType=$3->type;
	                                $$=context;};
			  | T_EP '<' collections '>' {genContext* context=new genContext();
                                    context->place="edgeProperty";
					                context->type="edgeProperty";
									context->enclosedType=$3->type;
	                                $$=context; };
	
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
		| T_FORALL '(' ID T_IN proc_call filterExpr')'  blockstatements {};
		| T_FOR '(' ID T_IN leftSide ')' blockstatements {};
		| T_FOR '(' ID T_IN proc_call ')' blockstatements {};
		   
filterExpr  : '.filter('boolean_expr ')'{ };

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

