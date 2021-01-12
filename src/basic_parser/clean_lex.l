
DIGIT       [0-9]
ALPHANUM    [a-zA-Z][a-zA-Z0-9_]*
WS  [ \t\v\n\f]

%{
    #include <stdlib.h>
    #include <stdio.h>  
    #include <string.h>
    #include "y.tab.h"
	void yyerror(char *);
	extern char mytext[];
%}


/* Define Section */

%%
 /* Keywords */

"function" { strcpy(mytext, yytext); return T_FUNC;}
"int" 	  { strcpy(mytext, yytext); return T_INT;}
"long" 	  { strcpy(mytext, yytext); return T_LONG;}
"float"   { strcpy(mytext, yytext); return T_FLOAT;}
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

"+="					{  strcpy(mytext, yytext); return T_ADD_ASSIGN;}
"-="					{  strcpy(mytext, yytext); return T_SUB_ASSIGN;}
"*="					{  strcpy(mytext, yytext); return T_MUL_ASSIGN;}
"/="					{  strcpy(mytext, yytext); return T_DIV_ASSIGN;}
"%="					{  strcpy(mytext, yytext); return T_MOD_ASSIGN;}
"&="					{  strcpy(mytext, yytext); return T_AND_ASSIGN;}
"^="					{  strcpy(mytext, yytext); return T_XOR_ASSIGN;}
"|="					{  strcpy(mytext, yytext); return T_OR_ASSIGN;}
">>"					{  strcpy(mytext, yytext); return T_RIGHT_OP;}
"<<"					{  strcpy(mytext, yytext); return T_LEFT_OP;}
"++"					{  strcpy(mytext, yytext); return T_INC_OP;}
"--"					{  strcpy(mytext, yytext); return T_DEC_OP;}
"->"					{  strcpy(mytext, yytext); return T_PTR_OP;}
"&&"					{  strcpy(mytext, yytext); return T_AND_OP;}
"||"					{  strcpy(mytext, yytext); return T_OR_OP;}
"<="					{  strcpy(mytext, yytext); return T_LE_OP;}
">="					{  strcpy(mytext, yytext); return T_GE_OP;}
"=="					{  strcpy(mytext, yytext); return T_EQ_OP;}
"!="					{  strcpy(mytext, yytext); return T_NE_OP;}
 
";"					{  strcpy(mytext, yytext); return ';' ;}
("{"|"<%")				{  strcpy(mytext, yytext); return '{' ;}
("}"|"%>")				{ strcpy(mytext, yytext); return '}';}
","					{  strcpy(mytext, yytext); return ',';}
":"					{  strcpy(mytext, yytext); return ':';}
"="					{  strcpy(mytext, yytext); return '=';}
"("					{  strcpy(mytext, yytext); return '(';}
")"					{  strcpy(mytext, yytext); return ')';}
("["|"<:")				{  strcpy(mytext, yytext); return '[';}
("]"|":>")				{  strcpy(mytext, yytext); return ']';}
"."					{  strcpy(mytext, yytext); return '.';}
"&"					{  strcpy(mytext, yytext); return '&';}
"!"					{  strcpy(mytext, yytext); return '!';}
"~"					{  strcpy(mytext, yytext); return '~';}
"^"					{  strcpy(mytext, yytext); return '^';}
"|"					{  strcpy(mytext, yytext); return '|';}
"?"					{  strcpy(mytext, yytext); return '?';} 
 

"And"     {  strcpy(mytext, yytext); return T_AND;}
"Or"      {  strcpy(mytext, yytext); return T_OR;}

"Sum"     {  strcpy(mytext, yytext); return T_SUM;}
"Count"   {  strcpy(mytext, yytext); return T_COUNT;}
"Product" {  strcpy(mytext, yytext); return T_PRODUCT;}
"Max"     {  strcpy(mytext, yytext); return T_MAX;}
"Min"     {  strcpy(mytext, yytext); return T_MIN;} 


 /* Graph Types */
"Graph" 	{ strcpy(mytext, yytext); return T_GRAPH;}
"dirGraph" 	{ strcpy(mytext, yytext); return T_DIR_GRAPH;}
"node" 		{ strcpy(mytext, yytext); return T_NODE;}
"edge" 		{ strcpy(mytext, yytext); return T_EDGE;}
"propNode" { strcpy(mytext, yytext); return T_NP;}
"propEdge" { strcpy(mytext, yytext); return T_EP;}
 
 /* Collection Type */
"SetN" 	{ strcpy(mytext, yytext); return T_SET_NODES;}
"SetE" 	{strcpy(mytext, yytext); return T_SET_EDGES;}
"elements" { strcpy(mytext, yytext); return T_ELEMENTS;}
"list" { strcpy(mytext, yytext); return T_LIST;}
"filter" { strcpy(mytext, yytext); return T_FLTR;}

 /* Numbers and Identifies */
{ALPHANUM}          { strcpy(mytext, yytext); return ID;}
{DIGIT}+"."{DIGIT}* { strcpy(mytext, yytext); return FLOAT_NUM;}
{DIGIT}{DIGIT}*     { strcpy(mytext, yytext); return INT_NUM;}

[ \n\t]	{ }  
{WS}+					{ /* whitespace separates tokens */ } 

.    {printf("NonSensible Token%s",yytext);}


%%

int yywrap(void) {
    return 1;
}

/*

int main(int ac, char **av)
{
*/

 /* yyin and yyout as pointer 
    of File type */
 /* 
   FILE    *fd;

    if (ac == 2)
    {
        if (!(fd = fopen(av[1], "r")))
        {
            perror("Error: ");
            return (-1);
        }
        yyset_in(fd);
        yylex();
        fclose(fd);
    }
    else
        
    return (0);
//yylex();
//return 0;
}

int yywrap(void) {
    return 1;
}

*/
