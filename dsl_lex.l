
DIGIT       [0-9]
ALPHANUM    [a-zA-Z][a-zA-Z0-9_]*
WS  [ \t\v\n\f]

%{
    #include <stdlib.h>
    #include <stdio.h>  
    #include <string.h>
%}

/* Define Section */

%%
 /* Keywords */

function {printf("Keyword function");}
"int" {printf("Type: %s", yytext);}
"long" {printf("Type: %s", yytext);}
"float" {printf("Type: %s", yytext);}
"double" {printf("Type: %s", yytext);}
"bool"  {printf("Type: %s", yytext);}
"forall" {printf("Keyword-forall: %s", yytext);}
"for"     {printf("For: %s", yytext);}
"+INF"     {printf("+Infinity: %s", yytext);}
"INF"     {printf("+Infinity: %s", yytext);}
"-INF"    {printf("-Infinity: %s", yytext);}

"True"    {printf("Type Bool: %s", yytext);}
"False"   {printf("Type Bool: %s", yytext);}
"If"      {printf("Keyword if: %s", yytext);}
"Else"    {printf("Keyword else: %s", yytext);}
"While"   {printf("Keyword while: %s", yytext);}
"Return"  {printf("Keyword return: %s", yytext);}
"Do"      {printf("Keyword do: %s", yytext);}

"+="					{ printf ("ADD_ASSIGN "); }
"-="					{ printf ("SUB_ASSIGN"); }
"*="					{ printf ( "MUL_ASSIGN"); }
"/="					{ printf ( "DIV_ASSIGN"); }
"%="					{ printf ( "MOD_ASSIGN"); }
"&="					{ printf ( "AND_ASSIGN"); }
"^="					{ printf ( "XOR_ASSIGN"); }
"|="					{ printf ( "OR_ASSIGN"); }
">>"					{ printf ( "RIGHT_OP"); }
"<<"					{ printf ( "LEFT_OP"); }
"++"					{ printf ( "INC_OP"); }
"--"					{ printf ( "DEC_OP"); }
"->"					{ printf ( "PTR_OP"); }
"&&"					{ printf ( "AND_OP"); }
"||"					{ printf ( "OR_OP"); }
"<="					{ printf ( "LE_OP"); }
">="					{ printf ( "GE_OP"); }
"=="					{ printf ( "EQ_OP"); }
"!="					{ printf ( "NE_OP"); }
";"					{ printf ( "';'");}
("{"|"<%")				{ printf ( "'{'") ;}
("}"|"%>")				{ printf ( "'}'");}
","					{ printf ( "','"); }
":"					{ printf ( "':'"); }
"="					{ printf ( "'='"); }
"("					{ printf ( "'('"); }
")"					{ printf ( "')'"); }
("["|"<:")				{ printf ( "'['"); }
("]"|":>")				{ printf ( "']'"); }
"."					{ printf ( "'.'"); }
"&"					{ printf ( "'&'"); }
"!"					{ printf ( "'!'"); }
"~"					{ printf ( "'~'"); }
"-"					{ printf ( "'-'"); }
"+"					{ printf ( "'+'"); }
"*"					{ printf ( "'*'"); }
"/"					{ printf ( "'/'"); }
"%"					{ printf ( "'%'"); }
"<"					{ printf ( "'<'"); }
">"					{ printf ( "'>'"); }
"^"					{ printf ( "'^'"); }
"|"					{ printf ( "'|'"); }
"?"					{ printf ( "'?'"); } 
"And"     { printf ( "And"); }
"Or"     { printf ( "Or"); }

"Sum"     { printf ( "Sum"); }
"Avg"     { printf ( "Average"); }
"Count"   { printf ( "Count"); }
"Product" { printf ( "Product"); }
"Max"     { printf ( "Max"); }
"Min"     { printf ( "Min"); } 


 /* Graph Types */
"Graph" {printf("Graph Type");}
"dirGraph" {printf("Directed Graph Type");}
"node" {printf("Node Type");}
"edge" {printf("Edge Type");}

"numNodes" {printf("Number of Nodes");}
"numEdges" {printf("Number of Edges");}
"makeUndirected" {printf("Make Undirected\n");}

"contains" {printf("Keyword-Contains");}

"nodes_from" {printf("Nodes From"); }
"nodes_to" {printf("Nodes To");}
"edges_from" {printf("Edges From");}
"edges_to" {printf("Edges From");}
"out_degree" {printf("Out degree");}
"in_degree" {printf("In Degree");}
"count_outNbrs" {printf("Count OutNbr");}
"count_inNbrs" {printf("Count InNbr");}

"get_source" {printf("Get Source");}
"get_destination" {printf("Get Destination");}
"get_edge" {printf("Get Edge");}
"get_neighbor" {printf("Get Nbr");}

"nodes" {printf("Nodes");}
"edges" {printf("Edges");}
"propNode" {printf("Node Property");}
"propEdge" {printf("Edge Property");}
"attachNodeProperty" {printf("Attach Node Property");}
"attachEdgeProperty" {printf("Attach Edge Property");}

 /* Collection Type */
"SetN" {printf("Set of Nodes");}
"SetE" {printf("Set of Edges");}
"elements" {printf("elements");}


 /* Numbers and Identifies */
{ALPHANUM}          {printf("Identifier");}
{DIGIT}+"."{DIGIT}* {printf("Float Number");}
{DIGIT}{DIGIT}*     {printf("Int Number");}

[ \n\t]	{ printf("\n");} 
{WS}+					{ /* whitespace separates tokens */ } 

.    {printf("invalid\n");}


%%

int main(int ac, char **av)
{

 /* yyin and yyout as pointer 
    of File type */

  
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

