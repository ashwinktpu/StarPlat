
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

"function" {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_FUNC;}
"int" 	  {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_INT;}
"long" 	  {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_LONG;}
"float"   {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_FLOAT;}
"double"  {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_DOUBLE;}
"bool"    {printf("Type: %s", yytext); strcpy(mytext, yytext); return T_BOOL;}
"forall"  {printf("Keyword-forall: %s", yytext);strcpy(mytext, yytext); return T_FORALL;}
"for"     {printf("For: %s", yytext);strcpy(mytext, yytext); return T_FOR;}
"+INF"    {printf("+Infinity: %s", yytext);strcpy(mytext, yytext); return T_P_INF;}
"INF"     {printf("+Infinity: %s", yytext);strcpy(mytext, yytext); return T_INF;}
"-INF"    {printf("-Infinity: %s", yytext);strcpy(mytext, yytext); return T_N_INF;}

"True"    {printf("Type Bool: %s", yytext); strcpy(mytext, yytext); return BOOL_VAL;}
"False"   {printf("Type Bool: %s", yytext); strcpy(mytext, yytext); return BOOL_VAL;}
"if"      {printf("Keyword if: %s", yytext); strcpy(mytext, yytext); return T_IF;}
"else"    {printf("Keyword else: %s", yytext); strcpy(mytext, yytext); return T_ELSE;}
"while"   {printf("Keyword while: %s", yytext); strcpy(mytext, yytext); return T_WHILE;}
"Return"  {printf("Keyword return: %s", yytext); strcpy(mytext, yytext); return T_RETURN;}
"do"      {printf("Keyword do: %s", yytext); strcpy(mytext, yytext); return T_DO;}
"in"      {printf("Keyword in "); strcpy(mytext, yytext); return T_IN;}
"fixedPoint" {printf("Keyword fixedPoint"); strcpy(mytext, yytext); return T_FIXEDPOINT;}
"until"      {printf("Keyword until"); strcpy(mytext, yytext); return T_UNTIL;}
"iterateInBFS"  {printf("iterateInBFS"); strcpy(mytext, yytext); return T_BFS;}
"iterateInReverse" {printf("iterateInReverse"); strcpy(mytext, yytext); return T_REVERSE;} 
"from"          {printf("from"); strcpy(mytext, yytext); return T_FROM;} 
"filter"        {printf("from"); strcpy(mytext, yytext); return T_FILTER;} 

"+="					{ printf ("ADD_ASSIGN "); strcpy(mytext, yytext); return T_ADD_ASSIGN;}
"-="					{ printf ("SUB_ASSIGN"); strcpy(mytext, yytext); return T_SUB_ASSIGN;}
"*="					{ printf ( "MUL_ASSIGN"); strcpy(mytext, yytext); return T_MUL_ASSIGN;}
"/="					{ printf ( "DIV_ASSIGN"); strcpy(mytext, yytext); return T_DIV_ASSIGN;}
"%="					{ printf ( "MOD_ASSIGN"); strcpy(mytext, yytext); return T_MOD_ASSIGN;}
"&="					{ printf ( "AND_ASSIGN"); strcpy(mytext, yytext); return T_AND_ASSIGN;}
"^="					{ printf ( "XOR_ASSIGN"); strcpy(mytext, yytext); return T_XOR_ASSIGN;}
"|="					{ printf ( "OR_ASSIGN"); strcpy(mytext, yytext); return T_OR_ASSIGN;}
">>"					{ printf ( "RIGHT_OP"); strcpy(mytext, yytext); return T_RIGHT_OP;}
"<<"					{ printf ( "LEFT_OP"); strcpy(mytext, yytext); return T_LEFT_OP;}
"++"					{ printf ( "INC_OP"); strcpy(mytext, yytext); return T_INC_OP;}
"--"					{ printf ( "DEC_OP"); strcpy(mytext, yytext); return T_DEC_OP;}
"->"					{ printf ( "PTR_OP"); strcpy(mytext, yytext); return T_PTR_OP;}
"&&"					{ printf ( "AND_OP"); strcpy(mytext, yytext); return T_AND_OP;}
"||"					{ printf ( "OR_OP"); strcpy(mytext, yytext); return T_OR_OP;}
"<="					{ printf ( "LE_OP"); strcpy(mytext, yytext); return T_LE_OP;}
">="					{ printf ( "GE_OP"); strcpy(mytext, yytext); return T_GE_OP;}
"=="					{ printf ( "EQ_OP"); strcpy(mytext, yytext); return T_EQ_OP;}
"!="					{ printf ( "NE_OP"); strcpy(mytext, yytext); return T_NE_OP;}
 
";"					{ printf ( "';'"); strcpy(mytext, yytext); return ';' ;}
("{"|"<%")				{ printf ( "'{'") ; strcpy(mytext, yytext); return '{' ;}
("}"|"%>")				{ printf ( "'}'");strcpy(mytext, yytext); return '}';}
","					{ printf ( "','"); strcpy(mytext, yytext); return ',';}
":"					{ printf ( "':'"); strcpy(mytext, yytext); return ':';}
"="					{ printf ( "'='"); strcpy(mytext, yytext); return '=';}
"("					{ printf ( "'('"); strcpy(mytext, yytext); return '(';}
")"					{ printf ( "')'"); strcpy(mytext, yytext); return ')';}
("["|"<:")				{ printf ( "'['"); strcpy(mytext, yytext); return '[';}
("]"|":>")				{ printf ( "']'"); strcpy(mytext, yytext); return ']';}
"."					{ printf ( "'.'"); strcpy(mytext, yytext); return '.';}
"&"					{ printf ( "'&'"); strcpy(mytext, yytext); return '&';}
"!"					{ printf ( "'!'"); strcpy(mytext, yytext); return '!';}
"~"					{ printf ( "'~'"); strcpy(mytext, yytext); return '~';}
"-"					{ printf ( "'-'"); strcpy(mytext, yytext); return '-';}
"+"					{ printf ( "'+'"); strcpy(mytext, yytext); return '+';}
"*"					{ printf ( "'*'"); strcpy(mytext, yytext); return '*';} 
"/"					{ printf ( "'/'"); strcpy(mytext, yytext); return '/';}
"%"					{ printf ( "'%'"); strcpy(mytext, yytext); return '%';}
"<"					{ printf ( "'<'"); strcpy(mytext, yytext); return '<';}
">"					{ printf ( "'>'"); strcpy(mytext, yytext); return '>';}

"^"					{ printf ( "'^'"); strcpy(mytext, yytext); return '^';}
"|"					{ printf ( "'|'"); strcpy(mytext, yytext); return '|';}
"?"					{ printf ( "'?'"); strcpy(mytext, yytext); return '?';} 
 

"And"     { printf ( "And"); strcpy(mytext, yytext); return T_AND;}
"Or"      { printf ( "Or"); strcpy(mytext, yytext); return T_OR;}

"Sum"     { printf ( "Sum"); strcpy(mytext, yytext); return T_SUM;}
"Count"   { printf ( "Count"); strcpy(mytext, yytext); return T_COUNT;}
"Product" { printf ( "Product"); strcpy(mytext, yytext); return T_PRODUCT;}
"Max"     { printf ( "Max"); strcpy(mytext, yytext); return T_MAX;}
"Min"     { printf ( "Min"); strcpy(mytext, yytext); return T_MIN;} 


 /* Graph Types */
"Graph" 	{printf("Graph Type"); strcpy(mytext, yytext); return T_GRAPH;}
"dirGraph" 	{printf("Directed Graph Type"); strcpy(mytext, yytext); return T_DIR_GRAPH;}
"node" 		{printf("Node Type"); strcpy(mytext, yytext); return T_NODE;}
"edge" 		{printf("Edge Type"); strcpy(mytext, yytext); return T_EDGE;}
"propNode" {printf("Node Property"); strcpy(mytext, yytext); return T_NP;}
"propEdge" {printf("Edge Property"); strcpy(mytext, yytext); return T_EP;}
 
 /*Built In - Dynamic Instructions*/
"add_node"   {printf("add node");strcpy(mytext, yytext);return T_ADD_NODE;}
"add_nodes"  {printf("add nodes");strcpy(mytext, yytext);return T_ADD_NODES;}
"add_edge"   {printf("add edge");strcpy(mytext, yytext);return T_ADD_EDGE;}
"add_edges"  {printf("add edges");strcpy(mytext, yytext);return T_ADD_EDGES;}
"rem_node"   {printf("rem node");strcpy(mytext, yytext);return T_REM_NODE;}
"rem_nodes"  {printf("rem nodes");strcpy(mytext, yytext);return T_REM_NODES;}
"rem_edge"   {printf("rem edge");strcpy(mytext, yytext);return T_REM_EDGE;}
"rem_edges"  {printf("rem_edges");strcpy(mytext, yytext);return T_REM_EDGES;}

 /* Collection Type */
"SetN" 	{printf("Set of Nodes"); strcpy(mytext, yytext); return T_SET_NODES;}
"SetE" 	{printf("Set of Edges");strcpy(mytext, yytext); return T_SET_EDGES;}
"list" {printf("list type"); strcpy(mytext, yytext); return T_LIST;}

 /*Built In - Set Operations(Both Nodes and Edges)*/
"elements" {printf("elements"); strcpy(mytext, yytext); return T_ELEMENTS;}
"contains" {printf("contains"); strcpy(mytext, yytext); return T_CONTAINS;}
"discard" {printf("discard"); strcpy(mytext, yytext); return T_DISCARD;}
"totalElements" {printf("totalElements"); strcpy(mytext, yytext); return T_TOT_ELEMS;}
"isEmpty" {printf("isEmpty"); strcpy(mytext, yytext); return T_ISEMP;}
"clear" {printf("clear"); strcpy(mytext, yytext); return T_CLR;}

 /*Built In - Set Operations(Nodes)*/
"addNode" {printf("Add Node");strcpy(mytext, yytext);return T_ADD_NODE;}
"addSetN" {printf("Add Node Set");strcpy(mytext, yytext);return T_ADD_NODESET;}

 /*Built In - Set Operations(Edges)*/
"addEdge" {printf("Add Edge");strcpy(mytext, yytext);return T_ADD_EDGE;}
"addSetE" {printf("Add Edge Set");strcpy(mytext, yytext);return T_ADD_EDGESET;}

 /* Built In*/
"makeUndirected" {printf("Make Undirected");strcpy(mytext, yytext);return T_MK_UNDRCTD;}

 /* Numbers and Identifies */
{ALPHANUM}          {printf("Identifier \n"); 
                           yylval.text=yytext;
                           
                        return ID;}
{DIGIT}+"."{DIGIT}* {printf("Float Number");
                       yylval.fval=atof(yytext);
                         return FLOAT_NUM;}
{DIGIT}{DIGIT}*     {printf("Int Number");
                           yylval.ival=atoi(yytext);
                            return INT_NUM;}

[ \n\t]	{ printf("\n");}  
{WS}+					{ /* whitespace separates tokens */ } 

.    {printf("invalid\n");}


%%

int yywrap(void) {
    return 1;
}
