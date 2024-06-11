#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"


namespace spmpi { 


    /* convertToCppType converts GraphDSL type to the corresponding cpp type */
    const char* dsl_cpp_generator::convertToCppType(Type* type, bool is_reference) 
    {   
        char * cppType = new char [1024];
        string cppString;
        bool isUpdatesType = false;
        if (type->isPrimitiveType()) 
        {
            int typeId = type->gettypeId();
            switch (typeId) {
                case TYPE_INT:
                    cppString = "int";
                    break;
                case TYPE_BOOL:
                    cppString = "bool";
                    break;
                case TYPE_LONG:
                    cppString = "long";
                    break;
                case TYPE_FLOAT:
                    cppString = "float";
                    break;
                case TYPE_DOUBLE:
                    cppString = "double";
                    break;
                case TYPE_NODE:
                    cppString = "int";
                    break;
                case TYPE_EDGE:
                    cppString = "int";
                    break;
				case TYPE_AUTOREF:
                    cppString = "auto&";
                    break;
                default:
                    assert(false);
            }
        } 
        else if (type->isPropType()) 
        {
            Type* targetType = type->getInnerTargetType();
            
            if (targetType->isPrimitiveType()) 
            {
                int typeId = targetType->gettypeId();
                cout << "TYPEID IN CPP" << typeId << "\n";
                switch (typeId) 
                {
                    case TYPE_INT:
                        cppString = type->gettypeId()==TYPE_PROPNODE ? "NodeProperty<int>" : "EdgeProperty<int>";
                        break;
                    case TYPE_BOOL:
                        cppString = type->gettypeId()==TYPE_PROPNODE ? "NodeProperty<bool>" : "EdgeProperty<bool>";
                        break;
                    case TYPE_LONG:
                        cppString = type->gettypeId()==TYPE_PROPNODE ? "NodeProperty<long>" : "EdgeProperty<long>";
                        break;
                    case TYPE_FLOAT:
                        cppString = type->gettypeId()==TYPE_PROPNODE ? "NodeProperty<float>" : "EdgeProperty<float>";
                        break;
                    case TYPE_DOUBLE:
                        cppString = type->gettypeId()==TYPE_PROPNODE ? "NodeProperty<double>" : "EdgeProperty<double>";
                        break;
                    default:
                        assert(false);
                }
            }
            else if(targetType->isNodeType())
            {
                cppString = "NodeProperty<int>";
            }
            else    
                assert(false);
        } 
        // TODO : (Atharva) Correct this for new mpi header 
        else if (type->isNodeEdgeType()) 
        {
            if (type->isEdgeType() )
                cppString = "Edge";
            else
                cppString = "int";  //need to be modified.
        } 
        else if (type->isGraphType()) 
        {
            cppString = "Graph";
        } 
        else if (type->isCollectionType()) 
        {
            printf("Collection Type \n");
            int typeId = type->gettypeId();

            switch (typeId) 
            {
                case TYPE_SETN:
                    cppString = "std::set<int>";
                    break;
                case TYPE_UPDATES:
                    isUpdatesType = true;
                    cppString = "Updates *";
                    break;
                case TYPE_NODEMAP:
                {  
                    char* newS = new char[1024];
                    string mapString = "std::map<int, ";   

                    char* valType = (char*)convertToCppType(type->getInnerTargetType() , false);
                    string keyString = valType;
                    mapString = mapString + keyString;
                    mapString = mapString + ">";
                    cppString = mapString;
                    break;
                 }
                case TYPE_CONTAINER:
                {
                    string vecString = "Container<";   

                    char* valType = (char*)convertToCppType(type->getInnerTargetType() , false);
                    string innerString = valType;
                    vecString = vecString + innerString;
                    vecString = vecString + ">";
                    cppString = vecString;
                    break;
                }
                case TYPE_VECTOR:
                {
                    string vecString = "std::vector<";   

                    char* valType = (char*)convertToCppType(type->getInnerTargetType() , false);
                    string innerString = valType;
                    vecString = vecString + innerString;
                    vecString = vecString + ">";
                    cppString = vecString;
                    break;
                }
				case TYPE_HASHMAP:
                {
                    string vecString = "HashMap<";   

                    char* keyType = (char*)convertToCppType(type->getInnerTargetType() , false);
					char* valType = (char*)convertToCppType(type->getOuterTargetType() , false);
                    string keyString = keyType;
					string valString = valType;
					string innerString = keyString+','+valString;
                    vecString = vecString + innerString;
                    vecString = vecString + ">";
                    cppString = vecString;
                    break;
                }
				case TYPE_HASHSET:
                {
                    string vecString = "HashSet<";   

                    char* keyType = (char*)convertToCppType(type->getInnerTargetType() , false);
                    string innerString = keyType;
                    vecString = vecString + innerString;
                    vecString = vecString + ">";
                    cppString = vecString;
                    break;
                }
                default:
                    assert(false);
            }
        }
        else{
            assert(false);
        }

        if(is_reference && !type->isPrimitiveType() && !type->isNodeEdgeType() && !isUpdatesType )
        {
            cppString+="&";
        }

        copy(cppString.begin(),cppString.end(),cppType);
        cppType[cppString.size()]='\0';
        return cppType;
    }



    void dsl_cpp_generator::castIfRequired(Type* type, Identifier* methodID, dslCodePad& main) 
    {

    /* This needs to be made generalized, extended for all predefined function,
      made available by the DSL*/
        string predefinedFunc("num_nodes");
        if (predefinedFunc.compare(methodID->getIdentifier()) == 0) 
        {
            if (type->gettypeId() != TYPE_INT) 
            {
                char strBuffer[1024];
                sprintf(strBuffer, "(%s)", convertToCppType(type));
                main.pushString(strBuffer);
            }
        }
    }



    void dsl_cpp_generator::getDefaultValueforTypes(int type) 
    {
      // Barenya : to debug the type issue : 
      printf ("type = %d\n", type) ;
        switch (type) {
            case TYPE_INT:
            case TYPE_LONG:
                main.pushstr_space("0");
                break;
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
                main.pushstr_space("0.0");
                break;
            case TYPE_BOOL:
                main.pushstr_space("false");
                break ;
            default:
                assert(false);
            return;
        }
    }

    const char* dsl_cpp_generator::getReductionOperatorString(int reductionType)
    {
        switch (reductionType)
        {
            case REDUCE_SUM:
                return "MPI_SUM";
            case REDUCE_COUNT:
                return "MPI_INC";
            case REDUCE_PRODUCT:
                return "MPI_PROD";
            case REDUCE_MAX:
                return "MPI_MAX";
            case REDUCE_MIN:
                return "MPI_MIN";

            default:
                assert(false);
                break;
        }
    }
    const char* dsl_cpp_generator::getOperatorString(int operatorId) {
        switch (operatorId) {
            case OPERATOR_ADD:
                return "+";
            case OPERATOR_DIV:
                return "/";
            case OPERATOR_MUL:
                return "*";
            case OPERATOR_MOD:
                return "%";
            case OPERATOR_SUB:
                return "-";
            case OPERATOR_EQ:
                return "==";
            case OPERATOR_NE:
                return "!=";
            case OPERATOR_LT:
                return "<";
            case OPERATOR_LE:
                return "<=";
            case OPERATOR_GT:
                return ">";
            case OPERATOR_GE:
                return ">=";
            case OPERATOR_AND:
                return "&&";
            case OPERATOR_OR:
                return "||";
            case OPERATOR_INC:
                return "++";
            case OPERATOR_DEC:
                return "--";
            case OPERATOR_ADDASSIGN:
                return "+";
            case OPERATOR_ANDASSIGN:
                return "&&";
            case OPERATOR_ORASSIGN:
                return "||";
            case OPERATOR_MULASSIGN:
                return "*";
            case OPERATOR_SUBASSIGN:
                return "-";
            case OPERATOR_NOT:
                return "!";
            default:
                return "NA";
        }
    }

    bool dsl_cpp_generator::startsWith (char * actual, char * prefix) {
      int endPointer = 0 ;
      while (prefix[endPointer++] != '\0') ;
      endPointer-- ;
      for (int startPointer = 0 ; startPointer < endPointer ; startPointer++) {
        if (actual[startPointer] == '\0') return false ;
        if (prefix[startPointer] != actual[startPointer]) {
          printf ("failed\n") ;
          return false ;
        }
      }
      printf ("matched\n") ;
      return true ;
    }

    bool dsl_cpp_generator::allGraphIteration(char* methodId) {
        string methodString(methodId);

        return (methodString == "nodes" || methodString == "edges");
    }

    bool dsl_cpp_generator::neighbourIteration(char* methodId) {
        string methodString(methodId);
        return (methodString == "neighbors" || methodString == "nodes_to");
    }

    bool dsl_cpp_generator::elementsIteration(char* extractId) {
        string extractString(extractId);
        return (extractString == "elements");
    }

    const char * dsl_cpp_generator::getMPIreduction(int operatorId)
    {
        switch (operatorId) {
            case OPERATOR_ADDASSIGN:
                return "MPI_SUM";
            case OPERATOR_ANDASSIGN:
                return "MPI_LAND";
            case OPERATOR_ORASSIGN:
                return "MPI_LOR";
            case OPERATOR_MULASSIGN:
                return "MPI_PROD";
            //TODO :(ATHARVA) what to do for subassign?
            case OPERATOR_SUBASSIGN:
                return "-";
            default:
                return "NA";
        }
    }

    const char * dsl_cpp_generator::getMPItype(Type * type)
    {
        if (type->isPrimitiveType()) 
        {
            int typeId = type->gettypeId();
            switch (typeId) {
                case TYPE_INT:
                    return "MPI_INT";
                case TYPE_BOOL:
                    return "MPI_C_BOOL";
                case TYPE_LONG:
                    return "MPI_LONG";
                case TYPE_FLOAT:
                    return "MPI_FLOAT";
                case TYPE_DOUBLE:
                    return "MPI_DOUBLE";
                default:
                    assert(false);
            }
        }
        else if(type->isNodeEdgeType())
        {
            int typeId = type->gettypeId();
            switch (typeId) {
                case TYPE_NODE:
                    return "MPI_INT";
                default :
                    assert(false); // add for other types if required    
            }        
        }
         
        else 
        {
            assert(false); 
        } 

    }

    void dsl_cpp_generator::generateArgList(list<argument*> argList) {
        char strBuffer[1024];
    
        int argListSize = argList.size();
        int commaCounts = 0;
        list<argument*>::iterator itr;
        for (itr = argList.begin(); itr != argList.end(); itr++) {
            commaCounts++;
            argument* arg = *itr;
            if(arg->isAssignExpr())
            {
                // arguments supplied in the type  arg = value; 
                // TO BE ADDED GENERATION WHEN REQUIRED
                assert(false);
            }
            else if(arg->isExpr())
            {
                generateExpr((Expression*) arg->getExpr());
            }
            

        if (commaCounts < argListSize)
            main.pushString(",");
        }

    }

    int dsl_cpp_generator::curFuncCount() {
        int count;
        if (curFuncType == GEN_FUNC)
            count = genFuncCount;

        else if (curFuncType == STATIC_FUNC)
            count = staticFuncCount;

        else if (curFuncType == INCREMENTAL_FUNC)
            count = inFuncCount;

        else if (curFuncType == DECREMENTAL_FUNC)
            count = decFuncCount;

        else if (curFuncType == DYNAMIC_FUNC)
            count = dynFuncCount;

        return count;
    }

    void dsl_cpp_generator::incFuncCount(int funcType) {
        if (funcType == GEN_FUNC)
            genFuncCount++;
        else if (funcType == STATIC_FUNC)
            staticFuncCount++;
        else if (funcType == INCREMENTAL_FUNC)
            inFuncCount++;
        else if (funcType == DECREMENTAL_FUNC)
            decFuncCount++;
        else
            dynFuncCount++;
    }

    // transfer this function to other relvant file

    bool dsl_cpp_generator::checkFixedPointAssociation(PropAccess* propId) {
        char strBuffer[1024];
        Identifier* id2 = propId->getIdentifier2();
        Expression* fpExpr = id2->getSymbolInfo()->getId()->get_dependentExpr();
        Identifier* depPropId = NULL;
        if (fpExpr->getExpressionFamily() == EXPR_UNARY) {
            if (fpExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
                depPropId = fpExpr->getUnaryExpr()->getId();
            }
        } else if (fpExpr->getExpressionFamily() == EXPR_ID) {
            depPropId = fpExpr->getId();
        }

        if (fixedPointEnv != NULL) {
            Expression* dependentPropExpr = fixedPointEnv->getDependentProp();
            Identifier* dependentPropId = NULL;
            if (dependentPropExpr->getExpressionFamily() == EXPR_UNARY) {
                if (dependentPropExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) {
                    dependentPropId = dependentPropExpr->getUnaryExpr()->getId();
                }
            } else if (dependentPropExpr->getExpressionFamily() == EXPR_ID) {
                dependentPropId = dependentPropExpr->getId();
            }
            string fpStmtProp(depPropId->getIdentifier());
            string dependentPropString(dependentPropId->getIdentifier());
            if (fpStmtProp == dependentPropString) {
                return true;
            }
        }

        return false;
    }
}
