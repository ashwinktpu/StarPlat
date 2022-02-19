EXPENDABLES_WINDOWS = bin\MainContext.o bin\ASTHelper.o bin\SymbolTableBuilder.o bin\SymbolTableNew.o bin\y.tab.o bin\lex.yy.o parser\y.tab.c parser\lex.yy.c bin\dsl_cpp_generator.o bin\attachPropAnalyser.o
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/dsl_cpp_generator.o bin/dataRaceAnalyser.o
ANALYSER = bin/dataRaceAnalyser.o

CC = g++  

all: finalcode clean
windows: finalcode clean_windows

finalcode: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o bin/dataRaceAnalyser.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o bin/dataRaceAnalyser.o -o finalcode

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

bin/attachPropAnalyser.o: analyser/attachPropAnalyser.cpp analyser/attachPropAnalyser.h
	$(CC) -c analyser/attachPropAnalyser.cpp -o bin/attachPropAnalyser.o

bin/dataRaceAnalyser.o: analyser/dataRaceAnalyser.cpp analyser/dataRaceAnalyser.h
	$(CC) -c analyser/dataRaceAnalyser.cpp -o bin/dataRaceAnalyser.o

bin/analyserUtil.o: analyser/analyserUtil.hpp
	$(CC) -c analyser/analyserUtil.hpp -o bin/analyserUtil.o

#bin/dsl_cpp_generator.o: backends/backend_mpi/mpi_cpp_generator.cpp backends/dslCodePad.h
#	$(CC) -c backends/backend_mpi/mpi_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o: 
	$(MAKE) -C parser

bin/lex.yy.o: 
	$(MAKE) -C parser

clean: 
	rm $(EXPENDABLES)

clean_windows:
	del $(EXPENDABLES_WINDOWS)
