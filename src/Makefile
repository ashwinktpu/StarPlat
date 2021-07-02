<<<<<<< HEAD
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/gen.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c
=======
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/dsl_cpp_generator.o
>>>>>>> da5dbf845f87d4dee325ace2fe92861c865cd224

CC = g++  

all: finalcode clean

<<<<<<< HEAD
finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/gen.o bin/y.tab.o bin/lex.yy.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTable.o bin/Symbol.o bin/gen.o bin/y.tab.o bin/lex.yy.o  -ll -o finalcode
=======
finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o  -ll -o finalcode
>>>>>>> da5dbf845f87d4dee325ace2fe92861c865cd224

bin/MainContext.o: maincontext/MainContext.cpp
	$(CC) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp
	$(CC) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp 
	$(CC) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

bin/SymbolTableNew.o: symbolutil/SymbolTableNew.cpp  symbolutil/SymbolTable.h
	$(CC) -c symbolutil/SymbolTableNew.cpp -o bin/SymbolTableNew.o

<<<<<<< HEAD
bin/gen.o: backends/dsl_cpp_generator.cpp
	$(CC) -c backends/dsl_cpp_generator.cpp -o bin/gen.o

=======


bin/dsl_cpp_generator.o: backends/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) -c backends/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o
    	
>>>>>>> da5dbf845f87d4dee325ace2fe92861c865cd224
bin/y.tab.o: 
	$(MAKE) -C parser

bin/lex.yy.o: 
	$(MAKE) -C parser

clean: 
	rm -f $(EXPENDABLES)