#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

//~ using namespace spsycl;
namespace spsycl{

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

    void dsl_cpp_generator::generation_begin() {
        char temp[1024];
        main.pushString("#ifndef GENCPP_");
        main.pushUpper(fileName);
        main.pushstr_newL("_H");
        main.pushString("#define GENCPP_");
        main.pushUpper(fileName);
        main.pushstr_newL("_H");
        main.pushString("#include ");
        addIncludeToFile("CL/sycl.hpp", main, true);
        main.pushString("#include ");
        addIncludeToFile("iostream", main, true);
        main.pushString("#include ");
        addIncludeToFile("fstream", main, true);
        main.pushString("#include ");
        addIncludeToFile("../graph.hpp", main, false);
        main.pushstr_newL("using namespace sycl");
        main.NewLine();
    }

    void dsl_cpp_generator::setCurrentFunc(Function* func) 
    {
        currentFunc = func;
    }

    const char* dsl_cpp_generator::convertToCppType(Type* type) 
    {
        if (type->isPrimitiveType()) {
            int typeId = type->gettypeId();
            switch (typeId) {
            case TYPE_INT:
                return "int";
            case TYPE_BOOL:
                return "bool";
            case TYPE_LONG:
                return "long";
            case TYPE_FLOAT:
                return "float";
            case TYPE_DOUBLE:
                return "double";
            case TYPE_NODE:
                return "int";
            case TYPE_EDGE:
                return "int";
            default:
                assert(false);
            }
        } else if (type->isPropType()) {
            Type* targetType = type->getInnerTargetType();
            if (targetType->isPrimitiveType()) {
            int typeId = targetType->gettypeId();
            //~ cout << "TYPEID IN CPP" << typeId << "\n";
            switch (typeId) {
                case TYPE_INT:
                return "int*";
                case TYPE_BOOL:
                return "bool*";
                case TYPE_LONG:
                return "long*";
                case TYPE_FLOAT:
                return "float*";
                case TYPE_DOUBLE:
                return "double*";
                default:
                assert(false);
            }
            }
        } else if (type->isNodeEdgeType()) {
            return "int";  // need to be modified.

        } else if (type->isGraphType()) {
            return "graph&";
        } else if (type->isCollectionType()) {
            int typeId = type->gettypeId();

            switch (typeId) {
            case TYPE_SETN:
                return "std::set<int>&";

            default:
                assert(false);
            }
        }

        return "NA";
    }

    void dsl_cpp_generator::generateCSRArrays(const char* gId) 
    {
        char strBuffer[1024];
        // assuming DSL does not contain variables V and E
        sprintf(strBuffer, "int V = %s.num_nodes();", gId); 
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "int E = %s.num_edges();", gId);
        main.pushstr_newL(strBuffer);
        main.NewLine();

        main.pushstr_newL("printf(\"#nodes:%d\\n\",V);");
        main.pushstr_newL("printf(\"#edges:\%d\\n\",E);");
        //~ main.pushstr_newL("printf(\"#srces:\%d\\n\",sourceSet.size()); /// TODO get from var");

        sprintf(strBuffer, "int* edgeLen = %s.getEdgeLen();",
                gId);  // assuming DSL  do not contain variables as V and E
        main.pushstr_newL(strBuffer);
        main.NewLine();

        // These H & D arrays of CSR do not change. Hence hardcoded!
        main.pushstr_newL("int *h_meta;");
        main.pushstr_newL("int *h_data;");
        main.pushstr_newL("int *h_src;");
        main.pushstr_newL("int *h_weight;");
        main.pushstr_newL("int *h_rev_meta;");  //done only to handle PR since other doesn't uses it
        main.NewLine();

        main.pushstr_newL("h_meta = (int *)malloc( (V+1)*sizeof(int));");
        main.pushstr_newL("h_data = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_src = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_weight = (int *)malloc( (E)*sizeof(int));");
        main.pushstr_newL("h_rev_meta = (int *)malloc( (V+1)*sizeof(int));");
        main.NewLine();

        main.pushstr_newL("for(int i=0; i<= V; i++) {");
        sprintf(strBuffer, "int temp = %s.indexofNodes[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_meta[i] = temp;");
        sprintf(strBuffer, "temp = %s.rev_indexofNodes[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_rev_meta[i] = temp;");
        main.pushstr_newL("}");
        main.NewLine();

        main.pushstr_newL("for(int i=0; i< E; i++) {");
        sprintf(strBuffer, "int temp = %s.edgeList[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_data[i] = temp;");
        sprintf(strBuffer, "temp = %s.srcList[i];", gId);
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("h_src[i] = temp;");
        main.pushstr_newL("temp = edgeLen[i];");
        main.pushstr_newL("h_weight[i] = temp;");
        main.pushstr_newL("}");
        main.NewLine();
    }

    void dsl_cpp_generator::generateFuncHeader(Function* proc) 
    {
        main.pushString("void ");
        main.pushString(proc->getIdentifier()->getIdentifier());

        main.push('(');
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam*> paramList = proc->getParamList();
        list<formalParam*>::iterator itr;

        for (itr = paramList.begin(); itr != paramList.end(); itr++) 
        {
            arg_currNo++;
            argumentTotal--;

            Type* type = (*itr)->getType();
            main.pushString(convertToCppType(type));
            char* parName = (*itr)->getIdentifier()->getIdentifier();

            main.pushString(" ");
            char str[80];
            strcpy(str, "d_");
            strcat(str, (*itr)->getIdentifier()->getIdentifier());
            cout << "param :" << parName << endl;
            cout << "paramD:" << str << endl;

            if (type->isGraphType())
            {
                std::cout << "========== SET TRUE" << '\n';
                genCSR = true;
            }

            main.pushString(parName);
            if(argumentTotal > 0) main.pushString(",");
            if (arg_currNo == maximum_arginline) 
            {
                main.NewLine();
                arg_currNo = 0;
            }
        }

        main.pushString(")");
        main.NewLine();
        return;        

    }

    void dsl_cpp_generator::generateMallocDeviceStr(const char* dVar, const char* typeStr, const char* sizeOfType) 
    {
        char strBuffer[1024];
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "%s=malloc_device<%s>(%s, Q);", dVar, typeStr, sizeOfType);
        main.pushstr_newL(strBuffer);
        //~ main.NewLine();
    }

    // Only for device variables
    void dsl_cpp_generator::generateMallocDevice(Type* type, const char* identifier) {
        char strBuffer[1024];
        //~ Type* targetType = type->getInnerTargetType();
        // cudaMalloc(&d_ nodeVal ,sizeof( int ) * V );
        //                   1             2      3
        // dVar=malloc_device<type>(V, Q);
        sprintf(strBuffer, "d_%s=malloc_device<%s>(%s, Q);", identifier,
                convertToCppType(type->getInnerTargetType()),
                (type->isPropNodeType())
                    ? "V"
                    : "E");  // this assumes PropNode type  IS PROPNODE? V : E //else
                            // might error later

        main.pushstr_newL(strBuffer);
        main.NewLine();
    }

    void dsl_cpp_generator::generateMallocDeviceParams(list<formalParam*> paramList) 
    {
        list<formalParam*>::iterator itr;
        for (itr = paramList.begin(); itr != paramList.end(); itr++) {
            Type* type = (*itr)->getType();
            if (type->isPropType()) 
            {
                Type* innerType = type->getInnerTargetType();
                main.pushString(convertToCppType(innerType)); 
                main.pushString("*");
                main.space();
                char str[80];
                strcpy(str, "d_");
                strcat(str, (*itr)->getIdentifier()->getIdentifier());
                main.pushString(str);
                main.pushstr_newL(";");
                generateMallocDevice(type, (*itr)->getIdentifier()->getIdentifier());
            }
        }
    }

    void dsl_cpp_generator::generateMemCpyStr(const char* sVarName, const char* tVarName, const char* type, const char* sizeV) 
    {
        //Q.submit([&](handler &h)
        //     { h.memcpy(d_data, h_data, sizeof(T) * N); })
        //.wait();
        char strBuffer[1024];
        sprintf(strBuffer, "Q.submit([&](handler &h)");
        main.pushstr_newL(strBuffer);

        
        sprintf(strBuffer, "{ h.memcpy(%8s, %8s, sizeof(%3s)*(%s)); })", sVarName, tVarName, type, sizeV);
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, ".wait();");
        main.pushstr_newL(strBuffer);
    }

    void dsl_cpp_generator::generateLaunchConfig() 
    {
        //int NUM_THREADS = 1048576;
        //int stride = NUM_THREADS;

        char strBuffer[1024];
        main.NewLine();

        sprintf(strBuffer, "int NUM_THREADS = 1048576;");
        main.pushstr_newL(strBuffer);

        sprintf(strBuffer, "int stride = NUM_THREADS;");
        main.pushstr_newL(strBuffer);
    }

    void dsl_cpp_generator::generateFuncBody(Function* proc)
    {
        char strBuffer[1024];
        int maximum_arginline = 4;
        int arg_currNo = 0;
        int argumentTotal = proc->getParamList().size();
        list<formalParam*> paramList = proc->getParamList();
        list<formalParam*>::iterator itr;

        const char* gId;

        for (itr = paramList.begin(); itr != paramList.end(); itr++) 
        {
            arg_currNo++;
            argumentTotal--;

            Type* type = (*itr)->getType();

            const char* parName = (*itr)->getIdentifier()->getIdentifier();
            cout << "param:" <<  parName << endl;
            if (type->isGraphType()) 
            {
                genCSR = true;
                gId = parName;
            }

            if (arg_currNo == maximum_arginline) 
            {
                arg_currNo = 0;
            }
        }

        sprintf(strBuffer, "default_selector d_selector;");
        main.pushstr_newL(strBuffer);
        sprintf(strBuffer, "queue Q(d_selector);");
        main.pushstr_newL(strBuffer);
        if(genCSR){
            main.pushstr_newL("// CSR BEGIN");
            generateCSRArrays(gId);

            main.NewLine();
            sprintf(strBuffer, "int* d_meta;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_data;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_src;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_weight;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "int* d_rev_meta;");
            main.pushstr_newL(strBuffer);
            sprintf(strBuffer, "bool* d_modified_next;");
            main.pushstr_newL(strBuffer);
            main.NewLine();

            generateMallocDeviceStr("d_meta", "int", "(1+V)");
            generateMallocDeviceStr("d_data", "int", "(E)");
            generateMallocDeviceStr("d_src", "int", "(E)");
            generateMallocDeviceStr("d_weight", "int", "(E)");
            generateMallocDeviceStr("d_rev_meta", "int", "(V+1)");
            generateMallocDeviceStr("d_modified_next", "bool", "(V)");

            main.NewLine();

            generateMemCpyStr("d_meta", "h_meta", "int", "V+1");
            generateMemCpyStr("d_data", "h_data", "int", "E");
            generateMemCpyStr("d_src", "h_src", "int", "E");
            generateMemCpyStr("d_weight", "h_weight", "int", "E");
            generateMemCpyStr("d_rev_meta", "h_rev_meta", "int", "(V+1)");
            main.NewLine();

            main.pushstr_newL("// CSR END");

            main.pushString("//LAUNCH CONFIG");

            generateLaunchConfig();

            main.NewLine();

            main.pushstr_newL("// TODO: TIMER START");
        }

    }

    void dsl_cpp_generator::generateFunc(ASTNode* proc) 
    {
        Function* func = (Function*)proc;
        generateFuncHeader(func);

        main.pushstr_newL("{");
        generateFuncBody(func);

        main.NewLine();

        main.pushstr_newL("//DECLAR DEVICE AND HOST vars in params");
        /* function for generation of cudamalloc for property type params*/
        generateMallocDeviceParams(func->getParamList());

    }

    void dsl_cpp_generator::generation_end() 
    {
        main.NewLine();
        main.pushstr_newL("#endif");
    }

    bool dsl_cpp_generator::openFileforOutput() 
    {
        char temp[1024];
        printf("fileName %s\n", fileName);
        sprintf(temp, "%s/%s.cpp", "../graphcode/generated_sycl", fileName);
        bodyFile = fopen(temp, "w");
        if (bodyFile == NULL) return false;
        main.setOutputFile(bodyFile);
        return true;
    }

    void dsl_cpp_generator::closeOutputFile() 
    {

        if (bodyFile != NULL) {
            main.outputToFile();
            fclose(bodyFile);
        }

        bodyFile = NULL;
    }

    bool dsl_cpp_generator::generate() 
    {
        cout<<"INSIDE SYCL GENERATOR"<<endl;
        cout<<"FRONTEND VALUES"<<frontEndContext.getFuncList().front()->getBlockStatement()->returnStatements().size()<<endl;
        // //openFileforOutput();
        if (!openFileforOutput()) return false;
        generation_begin();

        list<Function*> funcList = frontEndContext.getFuncList();
        for (Function* func : funcList) {
            setCurrentFunc(func);
            generateFunc(func);
        }

        generation_end();

        closeOutputFile();

        return true;
    }

    void dsl_cpp_generator::setFileName(char* f)
    {
        char* token = strtok(f, "/");
        char* prevtoken;

        while (token != NULL) {
            prevtoken = token;
            token = strtok(NULL, "/");
        }
        fileName = prevtoken;
    }
    }