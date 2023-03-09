EXPENDABLES_WINDOWS = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c bin/deviceVarsAnalyser.o bin/analyserUtil.o bin/dv*.o
EXPENDABLES = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o parser/y.tab.c parser/lex.yy.c  bin/deviceVarsAnalyser.o
ANALYSER = bin/dataRaceAnalyser.o
PROGRAMS = bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/cu_dsl_cpp_generator.o  bin/ac_dsl_cpp_generator.o bin/om_dsl_cpp_generator.o bin/mpi_dsl_cpp_generator.o
#bin/ac_dsl_cpp_generator.o 
#bin/om_dsl_cpp_generator.o
A_PROGRAM = bin/attachPropAnalyser.o bin/dataRaceAnalyser.o bin/deviceVarsAnalyser.o bin/blockVarsAnalyser.o bin/analyserUtil.o

CFLAGS  = -g 
CC = g++ -std=c++11 #-Wall # -Wno-unused-variable #-fsanitize=address

all: starplat

#~ openACC: generator_openACC starplat_openACC clean

#~ starplat_openACC: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o $(A_PROGRAM)
#~ 	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o $(A_PROGRAM) -o starplat



#~ cuda: generator_cuda starplat_cuda clean_windows

#~ starplat_cuda: bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o $(A_PROGRAM)
#~ 	$(CC) bin/MainContext.o bin/ASTHelper.o bin/SymbolTableBuilder.o bin/SymbolTableNew.o bin/y.tab.o bin/lex.yy.o bin/dsl_cpp_generator.o $(A_PROGRAM) -o starplat

#CUDA
bin/cu_dsl_cpp_generator.o: backends/backend_cuda/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_cuda/dsl_cpp_generator.cpp -o bin/cu_dsl_cpp_generator.o

#openACC
bin/ac_dsl_cpp_generator.o: backends/backend_openACC/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_openACC/dsl_cpp_generator.cpp -o bin/ac_dsl_cpp_generator.o
	
#omp
bin/om_dsl_cpp_generator.o: backends/backend_omp/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_omp/dsl_cpp_generator.cpp -o bin/om_dsl_cpp_generator.o

#mpi
bin/mpi_dsl_cpp_generator.o: backends/backend_mpi/dsl_cpp_generator.cpp backends/dslCodePad.h
	$(CC) $(CFLAGS) -c backends/backend_mpi/dsl_cpp_generator.cpp -o bin/mpi_dsl_cpp_generator.o

starplat: bin $(PROGRAMS) $(A_PROGRAM) 
	$(CC)  $(PROGRAMS) $(A_PROGRAM) -o StarPlat


#~ bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp
#~ 	$(CC) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

bin/MainContext.o: maincontext/MainContext.cpp maincontext/MainContext.hpp
	$(CC) $(CFLAGS) -c maincontext/MainContext.cpp -o bin/MainContext.o

bin/ASTHelper.o: ast/ASTHelper.cpp ast/ASTNode.hpp 
	$(CC) $(CFLAGS) -c ast/ASTHelper.cpp -o bin/ASTHelper.o

bin/SymbolTableBuilder.o: symbolutil/SymbolTableBuilder.cpp symbolutil/SymbolTableBuilder.h
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableBuilder.cpp -o bin/SymbolTableBuilder.o

#~ bin/dsl_cpp_generator.o: backends/backend_omp/dsl_cpp_generator.cpp backends/dslCodePad.h
#~ 	$(CC) -c backends/backend_omp/dsl_cpp_generator.cpp -o bin/dsl_cpp_generator.o


bin/SymbolTableNew.o: symbolutil/SymbolTableNew.cpp  symbolutil/SymbolTable.h 
	$(CC) $(CFLAGS) -c symbolutil/SymbolTableNew.cpp -o bin/SymbolTableNew.o


bin/attachPropAnalyser.o: analyser/attachProp/attachPropAnalyser.cpp analyser/attachProp/attachPropAnalyser.h
	$(CC) $(CFLAGS) -c analyser/attachProp/attachPropAnalyser.cpp -o bin/attachPropAnalyser.o

bin/dataRaceAnalyser.o: analyser/dataRace/dataRaceAnalyser.cpp analyser/dataRace/dataRaceAnalyser.h
	$(CC) $(CFLAGS) -c analyser/dataRace/dataRaceAnalyser.cpp -o bin/dataRaceAnalyser.o                       

bin/deviceVarsAnalyser.o: bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o
	ld -r bin/dv1.o bin/dv2.o bin/dv3.o bin/dv4.o bin/dv5.o -o bin/deviceVarsAnalyser.o 

bin/dv1.o: analyser/deviceVars/deviceVarsAnalyser.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsAnalyser.cpp -o bin/dv1.o

bin/dv2.o: analyser/deviceVars/deviceVarsInit.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsInit.cpp -o bin/dv2.o

bin/dv3.o: analyser/deviceVars/deviceVarsPrint.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/deviceVarsPrint.cpp -o bin/dv3.o

bin/dv4.o: analyser/deviceVars/deviceVarsTransfer.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC)  -c analyser/deviceVars/deviceVarsTransfer.cpp -o bin/dv4.o

bin/dv5.o: analyser/deviceVars/getUsedVars.cpp analyser/deviceVars/deviceVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/deviceVars/getUsedVars.cpp -o bin/dv5.o

bin/blockVarsAnalyser.o: bin/bv1.o bin/bv2.o bin/bv3.o
	ld -r bin/bv1.o bin/bv2.o bin/bv3.o -o bin/blockVarsAnalyser.o

bin/bv1.o: analyser/blockVars/blockVarsAnalyser.cpp analyser/blockVars/blockVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/blockVars/blockVarsAnalyser.cpp -o bin/bv1.o

bin/bv2.o: analyser/blockVars/blockVarsInit.cpp analyser/blockVars/blockVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/blockVars/blockVarsInit.cpp -o bin/bv2.o

bin/bv3.o: analyser/blockVars/getUsedVars.cpp analyser/blockVars/blockVarsAnalyser.h analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/blockVars/getUsedVars.cpp -o bin/bv3.o

bin/analyserUtil.o: analyser/analyserUtil.cpp
	$(CC) $(CFLAGS) -c analyser/analyserUtil.cpp -o bin/analyserUtil.o

#bin/dsl_cpp_generator.o: backends/backend_mpi/mpi_cpp_generator.cpp backends/dslCodePad.h
#	$(CC) -c backends/backend_mpi/mpi_cpp_generator.cpp -o bin/dsl_cpp_generator.o

bin/y.tab.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

bin/lex.yy.o: parser/lexer.l parser/lrparser.y
	$(MAKE) -C parser

bin:
	mkdir -p bin

clean_windows:
	rm -f $(EXPENDABLES_WINDOWS)

clean:
#~ 	rm -f $(EXPENDABLES)
	rm bin/*.o StarPlat
	rmdir bin
