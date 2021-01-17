EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c

CC = g++  

all: finalcode clean

finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o  -ll -o finalcode

bin/MainContext.o: maincontext/MainContext.cpp
	$(CC) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp
	$(CC) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTable.o: symbolutil/SymbolTable.cpp
	$(CC) -c symbolutil/SymbolTable.cpp -o bin/SymbolTable.o

bin/Symbol.o: symbolutil/Symbol.cpp
	$(CC) -c symbolutil/Symbol.cpp -o bin/Symbol.o

bin/y.tab.o: 
	$(MAKE) -C parser

bin/lex.yy.o: 
	$(MAKE) -C parser

clean: 
	rm -f $(EXPENDABLES)