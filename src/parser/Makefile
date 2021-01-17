EXPENDABLES = lex.yy.c y.tab.c y.tab.h

CC = g++ 

all: parse 

parse: dsl_yacc_modified.y
	yacc -d dsl_yacc_modified.y
	lex dsl_lex_for_yacc.l
	$(CC) -c -x c++ y.tab.c -o ../bin/y.tab.o
	$(CC) -c -x c++ lex.yy.c -o ../bin/lex.yy.o

clean: 
	rm -f $(EXPENDABLES)