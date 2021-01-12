EXPENDABLES = lex.yy.c lex.yy.o y.tab.c y.tab.h y.tab.o

CC = g++  

all : parse clean

parse:
	yacc -d playcopy.y
	$(CC) -c -x c++ y.tab.c
	lex clean_lex.l
	$(CC) -c -x c++ lex.yy.c
	c++ lex.yy.c y.tab.c -ll

clean:
	rm -f $(EXPENDABLES)