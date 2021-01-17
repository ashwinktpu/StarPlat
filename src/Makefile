EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c parser/y.tab.h

CC = g++  

all: finalcode 

finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/y.tab.o bin/lex.yy.o  -ll -o finalcode

bin/MainContext.o: maincontext/MainContext.cpp  maincontext/MainContext.hpp
	$(CC) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp  ast/ASTNode.hpp
	$(CC) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTable.o: symbolutil/SymbolTable.cpp  
	$(CC) -c symbolutil/SymbolTable.cpp -o bin/SymbolTable.o

bin/Symbol.o: symbolutil/Symbol.cpp  symbolutil/Symbol.hpp
	$(CC) -c symbolutil/Symbol.cpp -o bin/Symbol.o

bin/y.tab.o: 
	$(MAKE) -C parser

bin/lex.yy.o: 
	$(MAKE) -C parser

clean: 
	rm -f $(EXPENDABLES)