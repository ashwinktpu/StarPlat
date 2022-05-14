EXPENDABLES_WINDOWS = bin\MainContext.o bin\ASTHelper.o bin\SymbolTableBuilder.o bin\SymbolTableNew.o bin\y.tab.o bin\lex.yy.o parser\y.tab.c parser\lex.yy.c bin\dsl_cpp_generator.o bin\deviceVarsAnalyser.o bin\analyserUtil.o bin\dv*.o
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/dsl_cpp_generator.o bin/deviceVarsAnalyser.o
ANALYSER = bin/dataRaceAnalyser.o

CC = g++  
CFLAGS  = -g 
CC = g++ -g -std=c++11 -Wall -Wreturn-type #-fsanitize=address

all: finalcode

PROGRAMS = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o
A_PROGRAM = bin/attachPropAnalyser.o bin/dataRaceAnalyser.o bin/deviceVarsAnalyser.o bin/analyserUtil.o
cuda: generator_cuda finalcode_cuda

finalcode_cuda: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o
	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o -o finalcode

finalcode: $(PROGRAMS) $(A_PROGRAM)
	$(CC) $(CFLAGS) $(PROGRAMS) $(A_PROGRAM) -o finalcode

bin/MainContext.o: maincontext/MainContext.cpp maincontext/MainContext.hpp
	$(CC) $(CFLAGS) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp ast/ASTNode.hpp 
	$(CC) $(CFLAGS) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp symbolutil/SymbolTableBuilder.h
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

bin/SymbolTableNew.o: symbolutil/SymbolTableNew.cpp  symbolutil/SymbolTable.h 
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableNew.cpp -o bin/SymbolTableNew.o

bin/dsl_dyn_cpp_generator.o:  backends/backend_omp/dsl_dyn_cpp_generator.cpp  backends/dslCodePad.h
	$(CC) -c  backends/backend_omp/dsl_dyn_cpp_generator.cpp -o bin/dsl_dyn_cpp_generator.o

bin/dsl_cpp_generator.o:  backends/backend_omp/dsl_cpp_generator.cpp  backends/dslCodePad.h
	$(CC) $(CFLAGS) -c  backends/backend_omp/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/attachPropAnalyser.o: analyser/attachProp/attachPropAnalyser.cpp analyser/attachProp/attachPropAnalyser.h
	$(CC) -c analyser/attachProp/attachPropAnalyser.cpp -o bin/attachPropAnalyser.o

bin/dataRaceAnalyser.o: analyser/dataRace/dataRaceAnalyser.cpp analyser/dataRace/dataRaceAnalyser.h
	$(CC) -c analyser/dataRace/dataRaceAnalyser.cpp -o bin/dataRaceAnalyser.o

bin/deviceVarsAnalyser.o: bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o
	ld -r bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o -o bin/deviceVarsAnalyser.o

bin/dv1.o: analyser/deviceVars/deviceVarsAnalyser.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsAnalyser.cpp -o bin/dv1.o

bin/dv2.o: analyser/deviceVars/deviceVarsInit.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsInit.cpp -o bin/dv2.o

bin/dv3.o: analyser/deviceVars/deviceVarsPrint.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsPrint.cpp -o bin/dv3.o

bin/dv4.o: analyser/deviceVars/deviceVarsTransfer.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsTransfer.cpp -o bin/dv4.o

bin/dv5.o: analyser/deviceVars/getUsedVars.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/getUsedVars.cpp -o bin/dv5.o

bin/analyserUtil.o: analyser/analyserUtil.cpp
	$(CC) -c analyser/analyserUtil.cpp -o bin/analyserUtil.o

#bin/dsl_cpp_generator.o: backends/backend_mpi/mpi_cpp_generator.cpp backends/dslCodePad.h
#	$(CC) -c backends/backend_mpi/mpi_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

bin/lex.yy.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

clean_windows:
	del $(EXPENDABLES_WINDOWS)

generator_cuda: backends/backend_cuda/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) -c backends/backend_cuda/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o

clean:
	rm -f $(EXPENDABLES)
