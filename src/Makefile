EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/dsl_cpp_generator.o

CC = g++ -std=c++14 -Wall -Wreturn-type #-fsanitize=address

all: finalcode clean

cuda: bin generator_cuda finalcode_cuda clean

finalcode_cuda: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o -o finalcode

#~ Earlier finalcode
finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o  -ll -o finalcode

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

generator_cuda: backends/backend_cuda/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) -c backends/backend_cuda/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o:
	$(MAKE) -C parser

bin/lex.yy.o:
	$(MAKE) -C parser

bin:
	mkdir -p bin
	mkdir -p ../graphcode/generated_cuda

clean:
	rm -f $(EXPENDABLES)
