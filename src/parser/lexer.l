DIGIT       [0-9]
ALPHANUM    [a-zA-Z][a-zA-Z0-9_]*
WS  [ \t\v\n\f]
NEGATE [-]

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

"function" {printf("Type: %s", yytext);  return T_FUNC;}
"int" 	  {printf("Type: %s", yytext);  return T_INT;}
"long" 	  {printf("Type: %s", yytext);  return T_LONG;}
"float"   {printf("Type: %s", yytext);  return T_FLOAT;}
"double"  {printf("Type: %s", yytext);  return T_DOUBLE;}
"bool"    {printf("Type: %s", yytext);  return T_BOOL;}

"forall"  {printf("Keyword-forall: %s", yytext); return T_FORALL;}
"for"     {printf("For: %s", yytext); return T_FOR;}
"return"  {printf("Return: %s", yytext); return T_RETURN;}
"+INF"    {printf("+Infinity: %s", yytext); return T_P_INF;}
"INF"     {printf("+Infinity: %s", yytext); return T_INF;}
"-INF"    {printf("-Infinity: %s", yytext); return T_N_INF;}

"True"    {printf("Type Bool: %s", yytext); yylval.bval=true; return BOOL_VAL;}
"False"   {printf("Type Bool: %s", yytext);  yylval.bval=false; return BOOL_VAL;}
"if"      {printf("Keyword if: %s", yytext);  return T_IF;}
"else"    {printf("Keyword else: %s", yytext); return T_ELSE;}
"while"   {printf("Keyword while: %s", yytext);  return T_WHILE;}
"do"      {printf("Keyword do: %s", yytext);  return T_DO;}
"in"      {printf("Keyword in ");  return T_IN;}
"fixedPoint" {printf("Keyword fixedPoint"); return T_FIXEDPOINT;}
"until"      {printf("Keyword until");  return T_UNTIL;}
"iterateInBFS"  {printf("iterateInBFS");  return T_BFS;}
"iterateInReverse" {printf("iterateInReverse");  return T_REVERSE;} 
"from"          {printf("from"); return T_FROM;} 
"filter"        {printf("filter");  return T_FILTER;} 
"Incremental"   {printf("incremental"); return T_INCREMENTAL;}
"Decremental"   {printf("decremental"); return T_DECREMENTAL;}
"Static"        {printf("static"); return T_STATIC;}
"Dynamic"       {printf("dynamic"); return T_DYNAMIC ;}
"Batch"         {printf("batch"); return T_BATCH ;} 
"OnDelete"      {printf("Ondelete") ; return T_ONDELETE;}
"OnAdd"      {printf("Onadd") ; return T_ONADD;}
 


"+="					{ printf ("ADD_ASSIGN "); return T_ADD_ASSIGN;}
"-="					{ printf ("SUB_ASSIGN"); return T_SUB_ASSIGN;}
"*="					{ printf ( "MUL_ASSIGN"); return T_MUL_ASSIGN;}
"/="					{ printf ( "DIV_ASSIGN");  return T_DIV_ASSIGN;}
"%="					{ printf ( "MOD_ASSIGN");  return T_MOD_ASSIGN;}
"&="					{ printf ( "AND_ASSIGN");  return T_AND_ASSIGN;}
"^="					{ printf ( "XOR_ASSIGN"); return T_XOR_ASSIGN;}
"|="					{ printf ( "OR_ASSIGN"); return T_OR_ASSIGN;}
"++"					{ printf ( "INC_OP");  return T_INC_OP;}
"--"					{ printf ( "DEC_OP");  return T_DEC_OP;}
"->"					{ printf ( "PTR_OP");  return T_PTR_OP;}
"&&"					{ printf ( "AND_OP");  return T_AND_OP;}
"||"					{ printf ( "OR_OP");  return T_OR_OP;}
"<="					{ printf ( "LE_OP");  return T_LE_OP;}
">="					{ printf ( "GE_OP");  return T_GE_OP;}
"=="					{ printf ( "EQ_OP");  return T_EQ_OP;}
"!="					{ printf ( "NE_OP");  return T_NE_OP;}

";"					{ printf ( "';'"); return ';' ;}
("{"|"<%")				{ printf ( "'{'") ;  return '{' ;}
("}"|"%>")				{ printf ( "'}'"); return '}';}
","					{ printf ( "','"); return ',';}
":"					{ printf ( "':'");  return ':';}
"="					{ printf ( "'='");  return '=';}
"("					{ printf ( "'('");  return '(';}
")"					{ printf ( "')'");  return ')';}
("["|"<:")				{ printf ( "'['");  return '[';}
("]"|":>")				{ printf ( "']'");  return ']';}
"."					{ printf ( "'.'");  return '.';}
"&"					{ printf ( "'&'");  return '&';}
"!"					{ printf ( "'!'");  return '!';}
"~"					{ printf ( "'~'");  return '~';}
"-"					{ printf ( "'-'");  return '-';}
"+"					{ printf ( "'+'");  return '+';}
"*"					{ printf ( "'*'");  return '*';}
"/"					{ printf ( "'/'"); return '/';}
"//"[^\n]*"\n"                { printf ( "COMMENT" ); }
"%"					{ printf ( "'%%'");  return '%';}
"<"					{ printf ( "'<'");  return '<';}
">"					{ printf ( "'>'");  return '>';}

"^"					{ printf ( "'^'"); return '^';}
"|"					{ printf ( "'|'"); return '|';}
"?"					{ printf ( "'?'");  return '?';}


"And"     { /* printf ( "And"); */  return T_AND;}
"Or"      { /* printf ( "Or"); */  return T_OR;}

"Sum"     { printf ( "Sum");  return T_SUM;}
"Count"   { printf ( "Count");  return T_COUNT;}
"Product" { printf ( "Product"); return T_PRODUCT;}
"Max"     { printf ( "Max");  return T_MAX;}
"Min"     { printf ( "Min"); return T_MIN;}


 /* Graph Types */
"Graph" 	{printf("Graph Type"); return T_GRAPH;}
"dirGraph" 	{printf("Directed Graph Type");  return T_DIR_GRAPH;}
"node" 		{printf("Node Type");  return T_NODE;}
"edge" 		{printf("Edge Type");return T_EDGE;}
"propNode" {printf("Node Property");  return T_NP;}
"propEdge" {printf("Edge Property"); return T_EP;}

 /* Collection Type */
"SetN" 	{printf("Set of Nodes");  return T_SET_NODES;}
"SetE" 	{printf("Set of Edges"); return T_SET_EDGES;}
"list" {printf("list type"); return T_LIST;}
"updates" {printf("updates type"); return T_UPDATES;}
"container" {printf("container type"); return T_CONTAINER;}
"std::vector" {printf ("vector type"); return T_VECTOR;}
"nodeMap"  {printf("nodeMap type"); return T_NODEMAP;}


 /* Numbers and Identifies */
"std::min"          { yylval.text = yytext; return ID; }
{ALPHANUM}          { yylval.text=yytext; /*printf("HELLO ID %s",yytext); /* printf("\n");*/ return ID;}
{DIGIT}+"."{DIGIT}* { yylval.fval=atof(yytext);
                        return FLOAT_NUM;}
{NEGATE}*{DIGIT}{DIGIT}*     {  yylval.ival=atoi(yytext);
                            return INT_NUM;}

[ \n\t]	{ printf("\n");}
{WS}+					{ /* whitespace separates tokens */ }

.    {/* printf("invalid\n"); */}


%%

int yywrap(void) {
    return 1;
}

