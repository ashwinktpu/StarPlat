EXPENDABLES = lex.yy.c y.tab.c y.tab.h

CC = g++ 

all: parse 

parse: lrparser.y
	yacc -d lrparser.y
	lex lexer.l
	$(CC) -c -x c++ y.tab.c -o ../bin/y.tab.o
	$(CC) -c -x c++ lex.yy.c -o ../bin/lex.yy.o

debug: 
	yacc --debug -d lrparser.y
	lex lexer.l
	$(CC) -c -x c++ y.tab.c -o ../bin/y.tab.o
	$(CC) -c -x c++ lex.yy.c -o ../bin/lex.yy.o
	
clean: 
	rm -f $(EXPENDABLES)