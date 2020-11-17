
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
"Avg"     { printf ( "Average"); strcpy(mytext, yytext); return T_AVG;}
"Count"   { printf ( "Count"); strcpy(mytext, yytext); return T_COUNT;}
"Product" { printf ( "Product"); strcpy(mytext, yytext); return T_PRODUCT;}
"Max"     { printf ( "Max"); strcpy(mytext, yytext); return T_MAX;}
"Min"     { printf ( "Min"); strcpy(mytext, yytext); return T_MIN;} 


 /* Graph Types */
"Graph" 	{printf("Graph Type"); strcpy(mytext, yytext); return T_GRAPH;}
"dirGraph" 	{printf("Directed Graph Type"); strcpy(mytext, yytext); return T_DIR_GRAPH;}
"node" 		{printf("Node Type"); strcpy(mytext, yytext); return T_NODE;}
"edge" 		{printf("Edge Type"); strcpy(mytext, yytext); return T_EDGE;}

 /*
"numNodes" {printf("Number of Nodes"); strcpy(mytext, yytext); return T_NUM_NODES;}
"numEdges" {printf("Number of Edges"); strcpy(mytext, yytext); return T_NUM_EDGES;}
"makeUndirected" {printf("Make Undirected\n"); strcpy(mytext, yytext); return T_MUND;}

"contains" {printf("Keyword-Contains"); strcpy(mytext, yytext); return T_CONTAINS;}

"neighbors"  {printf("Neighbors"); strcpy(mytext, yytext); return T_NBR;}
"nodes_from" {printf("Nodes From"); strcpy(mytext, yytext); return T_OUT_NBR;}
"nodes_to" {printf("Nodes To"); strcpy(mytext, yytext); return T_IN_NBR;}
"edges_from" {printf("Edges From"); strcpy(mytext, yytext); return T_EDGES_FROM;}
"edges_to" {printf("Edges tO"); strcpy(mytext, yytext); return T_EDGES_TO;}
"out_degree" {printf("Out degree"); strcpy(mytext, yytext); return T_OUT_DEGREE;}
"in_degree" {printf("In Degree"); strcpy(mytext, yytext); return T_IN_DEGREE;}
"count_outNbrs" {printf("Count OutNbr"); strcpy(mytext, yytext); return T_C_ONBR;}
"count_inNbrs" {printf("Count InNbr"); strcpy(mytext, yytext); return T_C_INBR;}

"get_source" {printf("Get Source"); strcpy(mytext, yytext); return T_GET_SRC;}
"get_destination" {printf("Get Destination"); strcpy(mytext, yytext); return T_GET_DST;}
"get_edge" {printf("Get Edge"); strcpy(mytext, yytext); return T_GET_EDGE;}
"get_neighbor" {printf("Get Nbr"); strcpy(mytext, yytext); return T_GET_NBR;}

"nodes" {printf("Nodes"); strcpy(mytext, yytext); return T_NODES;}
"edges" {printf("Edges"); strcpy(mytext, yytext); return T_EDGES;}
*/

"propNode" {printf("Node Property"); strcpy(mytext, yytext); return T_NP;}
"propEdge" {printf("Edge Property"); strcpy(mytext, yytext); return T_EP;}
 /*
"attachNodeProperty" {printf("Attach Node Property"); strcpy(mytext, yytext); return T_ATTACH_NP;}
"attachEdgeProperty" {printf("Attach Edge Property"); strcpy(mytext, yytext); return T_ATTACH_EP;}
*/
 /* Collection Type */
"SetN" 	{printf("Set of Nodes"); strcpy(mytext, yytext); return T_SET_NODES;}
"SetE" 	{printf("Set of Edges");strcpy(mytext, yytext); return T_SET_EDGES;}
"elements" {printf("elements"); strcpy(mytext, yytext); return T_ELEMENTS;}
"list" {printf("list type"); strcpy(mytext, yytext); return T_LIST;}


 /* Numbers and Identifies */
{ALPHANUM}          {printf("Identifier \n"); strcpy(mytext, yytext); return ID;}
{DIGIT}+"."{DIGIT}* {printf("Float Number"); strcpy(mytext, yytext); return FLOAT_NUM;}
{DIGIT}{DIGIT}*     {printf("Int Number"); strcpy(mytext, yytext); return INT_NUM;}

[ \n\t]	{ printf("\n");}  
{WS}+					{ /* whitespace separates tokens */ } 

.    {printf("invalid\n");}


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
        printf("Usage: a.out filename\n");
    return (0);
//yylex();
//return 0;
}

int yywrap(void) {
    return 1;
}

*/
