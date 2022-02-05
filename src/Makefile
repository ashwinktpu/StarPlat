EXPENDABLES = bin\MainContext.o bin\ASTHelper.o bin\SymbolTableBuilder.o bin\SymbolTableNew.o bin\y.tab.o bin\lex.yy.o parser\y.tab.c parser\lex.yy.c bin\dsl_cpp_generator.o bin\ASTAnalyser.o
ANALYSER = bin\ASTAnalyser.o

CC = g++  

all: finalcode clean

finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o bin/ASTAnalyser.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o bin/ASTAnalyser.o -o finalcode

bin/MainContext.o: maincontext/MainContext.cpp
	$(CC) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp
	$(CC) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp 
	$(CC) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

bin/SymbolTableNew.o: symbolutil/SymbolTableNew.cpp  symbolutil/SymbolTable.h
	$(CC) -c symbolutil/SymbolTableNew.cpp -o bin/SymbolTableNew.o



bin/dsl_cpp_generator.o: backends/backend_omp/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) -c backends/backend_omp/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/ASTAnalyser.o: analyser/ASTAnalyser.cpp analyser/ASTAnalyser.h
	$(CC) -c analyser/ASTAnalyser.cpp -o bin/ASTAnalyser.o

#bin/dsl_cpp_generator.o: backends/backend_mpi/mpi_cpp_generator.cpp backends/dslCodePad.h
#	$(CC) -c backends/backend_mpi/mpi_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o: 
	$(MAKE) -C parser

bin/lex.yy.o: 
	$(MAKE) -C parser

clean: 
	del $(ANALYSER)
