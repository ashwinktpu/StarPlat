 #include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

namespace spmpi {

    /* generate is the main entrypoint function of the dsl_cpp_generator which generates the cpp code for GraphDSL*/
    bool dsl_cpp_generator::generate() 
    {   
        cout<<"FRONTEND VALUES "<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size();    //openFileforOutput();
        if (!openFileforOutput())
            return false;
        generation_begin();

        list<Function*> funcList = frontEndContext.getFuncList();

        std::cout<< "Funclist siz" << funcList.size() << '\n';
        
        for (Function* func : funcList) {
            generateFunc(func);
            std::cout<< "in FUN" << '\n';
        }

        generation_end();

        closeOutputFile();

        return true;
    }

    /* generation_begin function generates the code corresponding to including various cpp libraries. It also 
    generates code which includes the graph_mpi.hpp file */
    void dsl_cpp_generator::generation_begin() 
    {   
        char temp[1024];
        
        header.pushString("#ifndef GENCPP_");
        header.pushUpper(fileName);
        
        header.pushstr_newL("_H");
        header.pushString("#define GENCPP_");
        header.pushUpper(fileName);
        header.pushstr_newL("_H");
        header.pushString("#include");
        addIncludeToFile("stdio.h", header, true);
        
        // Add other include files later if required
        header.pushString("#include");
        addIncludeToFile("stdlib.h", header, true);
        header.pushString("#include");
        addIncludeToFile("limits.h", header, true);
        header.pushString("#include");
        addIncludeToFile("atomic", header, true);
        header.pushString("#include");
        addIncludeToFile("set", header, true);
        header.pushString("#include");
        addIncludeToFile("vector", header, true);

        header.pushString("#include");
        addIncludeToFile("../mpi_header/graph_mpi.h", header, false);
        header.NewLine();
        main.pushString("#include");
        sprintf(temp, "%s.h", fileName);
        addIncludeToFile(temp, main, false);
        main.NewLine();
    }

    /* generation_end completes the cpp code generation part */
    void dsl_cpp_generator::generation_end() {
        header.NewLine();
        header.pushstr_newL("#endif");
    }
    
    /* generateFunc generates the code for one complete function including its header and the body */
    void dsl_cpp_generator::generateFunc(ASTNode* proc) 
    {   
        Function* func = (Function*)proc;
        generateFuncHeader(func, false);
        generateFuncHeader(func, true);
        curFuncType = func->getFuncType();
        currentFunc = func;
        main.pushstr_newL("{");
        generateBlock(func->getBlockStatement(), false);
        main.NewLine();
        std::cout<<"inside"<<std::endl;
        main.pushstr_newL("}");
        incFuncCount(func->getFuncType());
    }

    void dsl_cpp_generator::generateBlock(blockStatement* blockStmt, bool includeBrace) 
    {
        list<statement*> stmtList = blockStmt->returnStatements();
        list<statement*>::iterator itr;
        if (includeBrace) 
            main.pushstr_newL("{");
  
        for (itr = stmtList.begin(); itr != stmtList.end(); itr++) 
        {
            statement* stmt = *itr;

            generateStatement(stmt);
        }

        if (includeBrace) 
            main.pushstr_newL("}");
  
    }

    /* generateFuncHeader genereates the function header i.e it generates the definiton of the function along
    with its arguments list. If isMainFile is True it generates the header for the cpp file otherwise it generates the
    definition of the function for the header file. */
    void dsl_cpp_generator::generateFuncHeader(Function* proc, bool isMainFile) 
    {
        dslCodePad& targetFile = isMainFile ? main : header;

        if (proc->containsReturn())
            targetFile.pushString("auto ");
        else
            targetFile.pushString("void ");

        targetFile.pushString(proc->getIdentifier()->getIdentifier());
        targetFile.push('(');

        generateParamList(proc->getParamList(), targetFile);

        targetFile.pushString(")");
        if (!isMainFile)
            targetFile.pushString(";");
        targetFile.NewLine();

        return;
    }

    /* generateParamList generates comma separated list of parameters(type and the identifier pair)*/
    void dsl_cpp_generator::generateParamList(list<formalParam*> paramList, dslCodePad& targetFile) 
    {
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = paramList.size();
        list<formalParam*>::iterator itr;
        for (itr = paramList.begin(); itr != paramList.end(); itr++) 
        {
            arg_currNo++;
            argumentTotal--;

            Type* type = (*itr)->getType();

            targetFile.pushString(convertToCppType(type,true));
            
            targetFile.pushString(" ");
            
            targetFile.pushString((*itr)->getIdentifier()->getIdentifier());

            if (argumentTotal > 0)
                targetFile.pushString(", ");

            if (arg_currNo == maximum_arginline) 
            {
                targetFile.NewLine();
                arg_currNo = 0;
            }
        }
        targetFile.pushString(", boost::mpi::communicator world ");
    }

    /* setFileName sets the file name in which the code will be generated. This function is called before the
    main generate function is called*/ 
    void dsl_cpp_generator::setFileName(char* f)  // to be changed to make it more universal.
    {   
        //printf("%s \n", f);
        char* token = strtok(f, "/");
        char* prevtoken;

        while (token != NULL) {
        prevtoken = token;
        token = strtok(NULL, "/");
        }
    fileName = prevtoken;
    printf("OutFile: %s \n", fileName);
    }

    /* openFileforOutput opens the header and cc files in which the cpp code generated will be pushed*/
    bool dsl_cpp_generator::openFileforOutput() 
    {
        char temp[1024];
        printf("fileName %s\n", fileName);
        sprintf(temp, "%s/%s.h", "../graphcode/generated_mpi", fileName);
        headerFile = fopen(temp, "w");
        if (headerFile == NULL)
            return false;
        header.setOutputFile(headerFile);

        sprintf(temp, "%s/%s.cc", "../graphcode/generated_mpi", fileName);
        bodyFile = fopen(temp, "w");
        if (bodyFile == NULL)
            return false;
        main.setOutputFile(bodyFile);

        return true;
    }

    /* closeOutputFile closes the header and cc files after all the code generation is complete*/
    void dsl_cpp_generator::closeOutputFile() 
    {
        if (headerFile != NULL) 
        {
            header.outputToFile();
        
            fclose(headerFile);
        }
        headerFile = NULL;

        if (bodyFile != NULL) 
        {
            main.outputToFile();
            fclose(bodyFile);
        }

        bodyFile = NULL;
    }

    /* addIncludeToFile generates the cpp format code for including files*/
    void dsl_cpp_generator::addIncludeToFile(const char* includeName, dslCodePad& file, bool isCppLib) 
    {  
        if (!isCppLib)
            file.push('"');
        else
            file.push('<');

        file.pushString(includeName);
        if (!isCppLib)
            file.push('"');
        else
            file.push('>');
        file.NewLine();
    }


}

