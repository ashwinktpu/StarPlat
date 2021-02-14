
DIGIT       [0-9]
ALPHANUM    [a-zA-Z][a-zA-Z0-9_]*
WS  [ \t\v\n\f]

%{
    #include <stdlib.h>
    #include <stdio.h>  
    #include <string.h>
    #include "includeHeader.hpp"
    #include "y.tab.h"
	void yyerror(char *);
    extern char mytext[];
    extern FILE* yyin;
%}


/* Define Section */

%%
 /* Keywords */

"function" {strcpy(mytext, yytext); return T_FUNC;}
"int" 	  { strcpy(mytext, yytext); return T_INT;}
"long" 	  { strcpy(mytext, yytext); return T_LONG;}
"float"   {strcpy(mytext, yytext); return T_FLOAT;}
"double"  { strcpy(mytext, yytext); return T_DOUBLE;}
"bool"    { strcpy(mytext, yytext); return T_BOOL;}
"forall"  {strcpy(mytext, yytext); return T_FORALL;}
"for"     {strcpy(mytext, yytext); return T_FOR;}
"+INF"    {strcpy(mytext, yytext); return T_P_INF;}
"INF"     {strcpy(mytext, yytext); return T_INF;}
"-INF"    {strcpy(mytext, yytext); return T_N_INF;}

"True"    { strcpy(mytext, yytext); return BOOL_VAL;}
"False"   { strcpy(mytext, yytext); return BOOL_VAL;}
"if"      { strcpy(mytext, yytext); return T_IF;}
"else"    { strcpy(mytext, yytext); return T_ELSE;}
"while"   { strcpy(mytext, yytext); return T_WHILE;}
"Return"  { strcpy(mytext, yytext); return T_RETURN;}
"do"      { strcpy(mytext, yytext); return T_DO;}
"in"      { strcpy(mytext, yytext); return T_IN;}
"fixedPoint" { strcpy(mytext, yytext); return T_FIXEDPOINT;}
"until"      { strcpy(mytext, yytext); return T_UNTIL;}
"iterateInBFS"  { strcpy(mytext, yytext); return T_BFS;}
"iterateInReverse" { strcpy(mytext, yytext); return T_REVERSE;} 
"from"          { strcpy(mytext, yytext); return T_FROM;} 
"filter"        { strcpy(mytext, yytext); return T_FILTER;} 

"+="					{ strcpy(mytext, yytext); return T_ADD_ASSIGN;}
"-="					{ strcpy(mytext, yytext); return T_SUB_ASSIGN;}
"*="					{ strcpy(mytext, yytext); return T_MUL_ASSIGN;}
"/="					{ strcpy(mytext, yytext); return T_DIV_ASSIGN;}
"%="					{ strcpy(mytext, yytext); return T_MOD_ASSIGN;}
"&="					{ strcpy(mytext, yytext); return T_AND_ASSIGN;}
"^="					{ strcpy(mytext, yytext); return T_XOR_ASSIGN;}
"|="					{ strcpy(mytext, yytext); return T_OR_ASSIGN;}
">>"					{ strcpy(mytext, yytext); return T_RIGHT_OP;}
"<<"					{ strcpy(mytext, yytext); return T_LEFT_OP;}
"++"					{ strcpy(mytext, yytext); return T_INC_OP;}
"--"					{ strcpy(mytext, yytext); return T_DEC_OP;}
"->"					{ strcpy(mytext, yytext); return T_PTR_OP;}
"&&"					{ strcpy(mytext, yytext); return T_AND_OP;}
"||"					{ strcpy(mytext, yytext); return T_OR_OP;}
"<="					{ strcpy(mytext, yytext); return T_LE_OP;}
">="					{ strcpy(mytext, yytext); return T_GE_OP;}
"=="					{ strcpy(mytext, yytext); return T_EQ_OP;}
"!="					{ strcpy(mytext, yytext); return T_NE_OP;}
 
";"					{  strcpy(mytext, yytext); return ';' ;}
("{"|"<%")				{  strcpy(mytext, yytext); return '{' ;}
("}"|"%>")				{ strcpy(mytext, yytext); return '}';}
","					{  strcpy(mytext, yytext); return ',';}
":"					{  strcpy(mytext, yytext); return ':';}
"="					{  strcpy(mytext, yytext); return '=';}
"("					{  strcpy(mytext, yytext); return '(';}
")"					{  strcpy(mytext, yytext); return ')';}
("["|"<:")				{ strcpy(mytext, yytext); return '[';}
("]"|":>")				{ strcpy(mytext, yytext); return ']';}
"."					{ strcpy(mytext, yytext); return '.';}
"&"					{ strcpy(mytext, yytext); return '&';}
"!"					{ strcpy(mytext, yytext); return '!';}
"~"					{ strcpy(mytext, yytext); return '~';}
"-"					{ strcpy(mytext, yytext); return '-';}
"+"					{ strcpy(mytext, yytext); return '+';}
"*"					{ strcpy(mytext, yytext); return '*';} 
"/"					{ strcpy(mytext, yytext); return '/';}
"%"					{ strcpy(mytext, yytext); return '%';}
"<"					{ strcpy(mytext, yytext); return '<';}
">"					{ strcpy(mytext, yytext); return '>';}

"^"					{ strcpy(mytext, yytext); return '^';}
"|"					{ strcpy(mytext, yytext); return '|';}
"?"					{ strcpy(mytext, yytext); return '?';} 
 

"And"     { strcpy(mytext, yytext); return T_AND;}
"Or"      { strcpy(mytext, yytext); return T_OR;}

"Sum"     { strcpy(mytext, yytext); return T_SUM;}
"Count"   { strcpy(mytext, yytext); return T_COUNT;}
"Product" { strcpy(mytext, yytext); return T_PRODUCT;}
"Max"     { strcpy(mytext, yytext); return T_MAX;}
"Min"     { strcpy(mytext, yytext); return T_MIN;} 


 /* Graph Types */
"Graph" 	{ strcpy(mytext, yytext); return T_GRAPH;}
"dirGraph" 	{ strcpy(mytext, yytext); return T_DIR_GRAPH;}
"node" 		{ strcpy(mytext, yytext); return T_NODE;}
"edge" 		{ strcpy(mytext, yytext); return T_EDGE;}
"propNode" {strcpy(mytext, yytext); return T_NP;}
"propEdge" { strcpy(mytext, yytext); return T_EP;}
 
"add_edge"   { strcpy(mytext, yytext);return T_ADD_EDGES;}
"add_edges"  { strcpy(mytext, yytext);return T_ADD_EDGES;}
"rem_edge"   { strcpy(mytext, yytext);return T_REM_EDGES;}
"rem_edges"  { strcpy(mytext, yytext);return T_REM_EDGES;}

 /*Graph Properties*/
"neighbors" {strcpy(mytext, yytext);return T_NBHRS;};
"nodes_from" {strcpy(mytext, yytext);return T_NODES_FRM;};
"nodes_to" {strcpy(mytext, yytext);return T_NODES_TO;};
"edges_from" {strcpy(mytext, yytext);return T_EDGES_FRM;};
"edges_to" {strcpy(mytext, yytext);return T_EDGES_TO;};
"out_degree" {strcpy(mytext, yytext);return T_OUT_DEGREE;};
"in_degree" {strcpy(mytext, yytext);return T_IN_DEGREE;};
"count_outNbrs" {strcpy(mytext, yytext);return T_COUNT_OUT_NBRS;};
"count_inNbrs" {strcpy(mytext, yytext);return T_COUNT_IN_NBRS;};
"get_source" {strcpy(mytext, yytext);return T_GET_SRC;};
"get_destination" {strcpy(mytext, yytext);return T_GET_DST;};
"get_edge" {strcpy(mytext, yytext);return T_GET_EDGE;};
"get_neighbor" {strcpy(mytext, yytext);return T_GET_NBHR;};
"nodes" {strcpy(mytext, yytext);return T_NODES;};
"edges" {strcpy(mytext, yytext);return T_EDGES;};
"numNodes" {strcpy(mytext, yytext);return T_NUM_NODES;};
"numEdges" {strcpy(mytext, yytext);return T_NUM_EDGES;};
"contains" { strcpy(mytext, yytext); return T_CONTAINS;}
"totalElements" { strcpy(mytext, yytext); return T_TOT_ELEMS;}
"isEmpty" { strcpy(mytext, yytext); return T_ISEMP;}

    /* Collection Type*/
"SetN" 	{ strcpy(mytext, yytext); return T_SET_NODES;}
"SetE" 	{ strcpy(mytext, yytext); return T_SET_EDGES;}
"list" { strcpy(mytext, yytext); return T_LIST;}

 /*Built In - Set Operations(Both Nodes and Edges)*/
"elements" { strcpy(mytext, yytext); return T_ELEMENTS;}



 /* Numbers and Identifies */
{ALPHANUM}          { yylval.text=yytext;return ID;}
{DIGIT}+"."{DIGIT}* { yylval.fval=atof(yytext);return FLOAT_NUM;}
{DIGIT}{DIGIT}*     { yylval.ival=atoi(yytext);return INT_NUM;}

[ \n\t]	{ }  
{WS}+					{ /* whitespace separates tokens */ } 

.    {}


%%

int yywrap(void) {
    return 1;
}
