%{
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	void yyerror(char *);
	int yylex(void);
	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;
%}

%token T_INT T_FLOAT T_BOOL T_DOUBLE  T_LONG


 /* operator precedence
  * Lower is higher
  */
%left '?'
%left ':'
%left T_OR
%left T_AND 
%left T_EQ T_NEQ
%left '<' '>'  T_LE T_GE
%left '+' '-'
%left '*' '/' '%'


%%
program: lines {
       		printf("program.\n");
	};


%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}


