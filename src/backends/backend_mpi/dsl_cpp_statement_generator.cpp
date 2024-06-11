#include <string.h>

#include <cassert>

#include "../../ast/ASTHelper.cpp"
#include "dsl_cpp_generator.h"

namespace spmpi {

    void dsl_cpp_generator::generateBFSAbstraction(iterateBFS* bfsAbstraction) 
    {   


        vector<Identifier*> graphIds = graphId[curFuncType][curFuncCount()];
        insideParallelConstruct.push_back(bfsAbstraction);
        char strBuffer[1024];
        sprintf(strBuffer,"%s.create_bfs_dag(%s);",graphIds[0]->getIdentifier(),bfsAbstraction->getRootNode()->getIdentifier());
        main.pushstr_newL(strBuffer);
        
        sprintf(strBuffer,"for(int phase=0;phase<%s.num_bfs_phases();phase++)",graphIds[0]->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");
        
        // TODO (Atharva) : similar to forall to add logic for when reduction operator is present inside iterBFS
        // and reverse IterBFS. And also to add logic for when global variable is modified inside a prallel region,
        // again similar to forall


        sprintf(strBuffer,"for( int %s : %s.get_bfs_nodes_for_phase(phase) )",bfsAbstraction->getIteratorNode()->getIdentifier(),graphIds[0]->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");
        statement* body = bfsAbstraction->getBody();
        assert(body->getTypeofNode() == NODE_BLOCKSTMT);
        blockStatement* block = (blockStatement*)body;
        list<statement*> statementList = block->returnStatements();
        for (statement* stmt : statementList) {
            generateStatement(stmt);
        }
        main.pushstr_newL("}");
        main.pushstr_newL("world.barrier();");
        main.pushstr_newL("}");
        
        sprintf(strBuffer,"for(int phase=%s.num_bfs_phases()-1;phase>0;phase--)",graphIds[0]->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");

        sprintf(strBuffer,"for(int %s : %s.get_bfs_nodes_for_phase(phase))",bfsAbstraction->getIteratorNode()->getIdentifier(),graphIds[0]->getIdentifier());
        main.pushstr_newL(strBuffer);
        main.pushstr_newL("{");
        blockStatement* revBlock = (blockStatement*)bfsAbstraction->getRBFS()->getBody();
        list<statement*> revStmtList = revBlock->returnStatements();

        for (statement* stmt : revStmtList) {
            generateStatement(stmt);
        }

        main.pushstr_newL("}");
        main.pushstr_newL("world.barrier();");
        main.pushstr_newL("}");
        insideParallelConstruct.pop_back();
    }
 
    void dsl_cpp_generator::generateReductionCallStmt(reductionCallStmt* stmt) 
    {
        vector<Identifier*> graphIds = graphId[curFuncType][curFuncCount()];

        reductionCall* reduceCall = stmt->getReducCall();
        char strBuffer[1024];

        // Only REDUCE_MIN supported as of now
        sprintf(strBuffer,"%s.queue_for_reduction(",graphIds[0]->getIdentifier());
        main.pushString(strBuffer);
        list<argument*> argList = reduceCall->getargList();
        list<ASTNode*> leftList = stmt->getLeftList();
        int i = 0;
        list<ASTNode*> rightList = stmt->getRightList();


        list<ASTNode*>::iterator itrLeftList = leftList.begin();
        list<ASTNode*>::iterator itrRightList = rightList.begin();
        if ((*itrLeftList)->getTypeofNode()!=NODE_PROPACCESS) 
        {
            // only the cases where all identifiers inside the reduction statement are prop type is supported right now
            assert(false);
        }
        PropAccess * assignedId = (PropAccess *)(*itrLeftList);
        sprintf(strBuffer,"std::make_pair(%s,",assignedId->getIdentifier1()->getIdentifier());
        main.pushString(strBuffer);        
        list<argument*>::iterator argItr = argList.begin();
        argItr++;
        generateExpr((*argItr)->getExpr());
        main.pushString(")");
        itrLeftList++;

        while(itrLeftList!=leftList.end())
        {
            if ((*itrLeftList)->getTypeofNode()!=NODE_PROPACCESS) 
            {   
                // only the cases where all identifiers inside the reduction statement are prop type is supported
                assert(false);
            }
            PropAccess * propId = (PropAccess *)(*itrLeftList);
            main.pushString(", ");
            sprintf(strBuffer,"std::make_pair(%s,",propId->getIdentifier1()->getIdentifier());
            main.pushString(strBuffer);
            generateExpr((Expression *)(*itrRightList));
            main.pushString(")");
            itrLeftList++;
            itrRightList++;
        }

        main.pushstr_newL(");");

        /*if (reduceCall->getReductionType() == REDUCE_MIN) 
        {
            // TODO : ONLY This case handled as of now (list not involved case not handled)
            if (stmt->isListInvolved()) 
            {
            //cout<<"INSIDE THIS OF LIST PRESENT"<<"\n";
                list<argument*> argList = reduceCall->getargList();
                list<ASTNode*> leftList = stmt->getLeftList();
                int i = 0;
                list<ASTNode*> rightList = stmt->getRightList();
            //printf("LEFT LIST SIZE %d \n",leftList.size());

                main.space();
                if (stmt->getAssignedId()->getSymbolInfo()->getType()->isPropType()) 
                {
                    Type* type = stmt->getAssignedId()->getSymbolInfo()->getType();

                    main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                }
                else {
                    // for case when it is normal identifier, not yet implemented
                    assert(false);
                }
                
                sprintf(strBuffer, "%s_new", stmt->getAssignedId()->getIdentifier());
                main.pushString(strBuffer);
                list<argument*>::iterator argItr;
                argItr = argList.begin();
                argItr++;
                main.pushString(" = ");
                generateExpr((*argItr)->getExpr());
                main.pushstr_newL(";");
                list<ASTNode*>::iterator itr1;
                list<ASTNode*>::iterator itr2;
                itr2 = rightList.begin();
                itr1 = leftList.begin();
                itr1++;
                for (; itr1 != leftList.end(); itr1++) 
                {
                    ASTNode* node = *itr1;
                    ASTNode* node1 = *itr2;

                    if (node->getTypeofNode() == NODE_ID) 
                    {
                        main.pushstr_space(convertToCppType(((Identifier*)node)->getSymbolInfo()->getType()));
                        sprintf(strBuffer, "%s_new", ((Identifier*)node)->getIdentifier());
                        main.pushString(strBuffer);
                        main.pushString(" = ");
                        generateExpr((Expression*)node1);
                    }
                    if (node->getTypeofNode() == NODE_PROPACCESS) 
                    {
                        PropAccess* p = (PropAccess*)node;
                        Type* type = p->getIdentifier2()->getSymbolInfo()->getType();
                        if (type->isPropType()) 
                        {
                            main.pushstr_space(convertToCppType(type->getInnerTargetType()));
                        }

                        sprintf(strBuffer, "%s_new", p->getIdentifier2()->getIdentifier());
                        main.pushString(strBuffer);
                        main.pushString(" = ");
                        Expression* expr = (Expression*)node1;
                        generateExpr((Expression*)node1);
                        main.pushstr_newL(";");
                    }
                    itr2++;
                }
                main.pushString("if(");
                generate_exprPropId(stmt->getTargetPropId());
                sprintf(strBuffer, " > %s_new)", stmt->getAssignedId()->getIdentifier());
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("{");  //added for testing then doing atomic min.
                // storing the old value before doing a atomic operation on the node property 
                sprintf(strBuffer, "%s.%s();",graphIds[0]->getIdentifier(), "get_lock_for_reduction_statement");
                main.pushstr_newL(strBuffer);
                main.pushString("if(");
                generate_exprPropId(stmt->getTargetPropId());
                sprintf(strBuffer, " > %s_new)", stmt->getAssignedId()->getIdentifier());
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("{");

                
                itr1 = leftList.begin();
                for (; itr1 != leftList.end(); itr1++) 
                {
                    ASTNode* node = *itr1;
                    Identifier* affected_Id = NULL;
                    if (node->getTypeofNode() == NODE_ID) 
                    {
                        affected_Id = (Identifier*)node;
                        sprintf(strBuffer,"%s = %s;", affected_Id->getIdentifier(),affected_Id->getIdentifier());
                    }
                    if (node->getTypeofNode() == NODE_PROPACCESS) 
                    {
                        Identifier * id1 = ((PropAccess*)node)->getIdentifier1();
                        Identifier * id2 = ((PropAccess*)node)->getIdentifier2();
                        sprintf(strBuffer,"%s.setValue(%s, %s_new);", id2->getIdentifier(),id1->getIdentifier(),id2->getIdentifier());   
                    }

                    main.pushstr_newL(strBuffer);

                }
                main.pushstr_newL("}");

                sprintf(strBuffer, "%s.%s();",graphIds[0]->getIdentifier(), "unlock_for_reduction_statement");
                main.pushstr_newL(strBuffer);
                main.pushstr_newL("}");  //added for testing condition..then atomicmin.
            }
            else 
                assert(false);
        }
        else {
            assert(false);
        }*/
    }

    void dsl_cpp_generator::generateFixedPoint(fixedPointStmt* fixedPointConstruct) 
    {
        char strBuffer[1024];
        Expression* convergeExpr = fixedPointConstruct->getDependentProp();
        Identifier* fixedPointId = fixedPointConstruct->getFixedPointId();
        statement* blockStmt = fixedPointConstruct->getBody();
        fixedPointEnv = fixedPointConstruct;

        // Right now only support for UNARY or ID type expression is supported inside fixed point filter.
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY || convergeExpr->getExpressionFamily() == EXPR_ID);
        main.pushString("while ( ");
        main.push('!');
        main.pushString(fixedPointId->getIdentifier());
        main.pushstr_newL(" )");
        main.pushstr_newL("{");
        
        if (fixedPointConstruct->getBody()->getTypeofNode() != NODE_BLOCKSTMT)
            generateStatement(fixedPointConstruct->getBody());
        else
            generateBlock((blockStatement*)fixedPointConstruct->getBody(), false);
        Identifier* dependentId = NULL;
        bool isNot = false;
        assert(convergeExpr->getExpressionFamily() == EXPR_UNARY || convergeExpr->getExpressionFamily() == EXPR_ID);
        if (convergeExpr->getExpressionFamily() == EXPR_UNARY) 
        {
            if (convergeExpr->getUnaryExpr()->getExpressionFamily() == EXPR_ID) 
            {
                dependentId = convergeExpr->getUnaryExpr()->getId();
                isNot = true;
            }
        }
        if (convergeExpr->getExpressionFamily() == EXPR_ID)
            dependentId = convergeExpr->getId();

        if(dependentId == NULL)
            assert(false);

        if(dependentId->getSymbolInfo()->getType()->isPropType())
        {
            sprintf(strBuffer, "%s = %s.%s(%s);", fixedPointId->getIdentifier(),dependentId->getIdentifier(), "aggregateValue",isNot ? "NOT" : "NO_OP" );
            main.pushstr_newL(strBuffer);
        }
        else
        {
            sprintf(strBuffer, "%s = ", fixedPointId->getIdentifier());
            main.pushString(strBuffer);
            generateExpr(convergeExpr);
            main.pushstr_newL(";");    
        }
        
        main.pushstr_newL("}");
        fixedPointEnv = NULL;
    }

    void dsl_cpp_generator::checkAndGenerateFixedPtFilter(forallStmt* forAll) 
    {
        Expression* filterExpr = NULL;
        bool filter_generated = false;
        if (forAll->hasFilterExpr())
        {
            filterExpr = forAll->getfilterExpr();
            if (fixedPointEnv != NULL)
                generatefixedpt_filter(filterExpr, filter_generated);
        }    
        else if(forAll->hasFilterExprAssoc())
        {
            filterExpr = forAll->getAssocExpr();
            if (fixedPointEnv != NULL)
                generatefixedpt_filter(filterExpr, filter_generated);
        }
    }

    void dsl_cpp_generator::generatefixedpt_filter(Expression* filterExpr, bool & filter_generated) 
    {
        if(filter_generated == true)
            return;

        Expression* lhs = filterExpr->getLeft();
        char strBuffer[1024];

        vector<Identifier*> graphIds = graphId[curFuncType][curFuncCount()];
        /*printf("curFuncType %d curFuncCount() %d \n",curFuncType,curFuncCount());
        printf("size %d\n",graphIds.size());*/

        if (lhs->isIdentifierExpr()) 
        {
            Identifier* filterId = lhs->getId();
            TableEntry* tableEntry = filterId->getSymbolInfo();
            if (tableEntry->getId()->get_fp_association()) 
            {
                if(tableEntry->getType()->gettypeId() == TYPE_PROPNODE)
                {
                    sprintf(strBuffer, "for (%s %s = 0; %s < %s.%s(); %s ++) ", "int", "v", "v", graphIds[0]->getIdentifier(), "num_local_nodes", "v");
                    main.pushstr_newL(strBuffer);
                    main.pushstr_space("{");
                    sprintf(strBuffer, "%s[%s] =  %s_nxt[%s] ;", filterId->getIdentifier(), "v", filterId->getIdentifier(), "v");
                    main.pushstr_newL(strBuffer);
      /*  Expression* initializer = filterId->getSymbolInfo()->getId()->get_assignedExpr();
            if(initializer!=NULL)
              assert(initializer->isBooleanLiteral());*/

                    // TODO : (Atharva) Understand this part
                    bool initializer = false;
                    Expression* fixedPtDependentExpr = filterId->getSymbolInfo()->getId()->get_dependentExpr();
                    if (fixedPtDependentExpr->isUnary())
                        initializer = false;
                    if (fixedPtDependentExpr->isIdentifierExpr())
                        initializer = true;
                    sprintf(strBuffer, "%s_nxt[%s] = %s ;", filterId->getIdentifier(), "v", initializer ? "true" : "false");
                    main.pushstr_newL(strBuffer);
                    main.pushstr_newL("}");
        
                }
                else if(tableEntry->getType()->gettypeId() == TYPE_PROPEDGE)
                {
                    sprintf(strBuffer, "for (%s %s = 0; %s < %s.%s(); %s ++) ", "int", "e", "e", graphIds[0]->getIdentifier(), "num_local_edges", "e");
                    main.pushstr_newL(strBuffer);
                    main.pushstr_space("{");
                    sprintf(strBuffer, "%s[%s] =  %s_nxt[%s] ;", filterId->getIdentifier(), "e", filterId->getIdentifier(), "e");
                    main.pushstr_newL(strBuffer);
      /*  Expression* initializer = filterId->getSymbolInfo()->getId()->get_assignedExpr();
            if(initializer!=NULL)
              assert(initializer->isBooleanLiteral());*/
                    bool initializer = false;
                    Expression* fixedPtDependentExpr = filterId->getSymbolInfo()->getId()->get_dependentExpr();
                    if (fixedPtDependentExpr->isUnary())
                        initializer = false;
                    if (fixedPtDependentExpr->isIdentifierExpr())
                        initializer = true;
                    sprintf(strBuffer, "%s_nxt[%s] = %s ;", filterId->getIdentifier(), "e", initializer ? "true" : "false");
                    main.pushstr_newL(strBuffer);
                    main.pushstr_newL("}");
                
                }
                filter_generated = true;
            }
        }
    }


    void dsl_cpp_generator::generateProcCall(proc_callStmt* procCall) 
    {
        proc_callExpr* procedure = procCall->getProcCallExpr();
        // cout<<"FUNCTION NAME"<<procedure->getMethodId()->getIdentifier();
        string methodID(procedure->getMethodId()->getIdentifier());
        string nodeCall("attachNodeProperty");
        string edgeCall("attachEdgeProperty");

        if (methodID.compare(nodeCall) == 0 || methodID.compare(edgeCall)==0)  
        {
            char strBuffer[1024];
            list<argument*> argList = procedure->getArgList();
            list<argument*>::iterator itr;
            
            for (itr = argList.begin(); itr != argList.end(); itr++) 
            {
                assignment* assign = (*itr)->getAssignExpr();
                Identifier* lhsId = assign->getId();
                Expression* exprAssigned = assign->getExpr();
                sprintf(strBuffer, "%s.attachToGraph(&%s, (%s)", lhsId->getIdentifier(), procedure->getId1()->getIdentifier(),  convertToCppType(lhsId->getSymbolInfo()->getType()->getInnerTargetType(), false));
                main.pushString(strBuffer);
                generateExpr(exprAssigned);
                main.pushString(")");
                main.pushstr_newL(";");

            }

        }  
        else 
        {
            generate_exprProcCall(procedure);
            main.pushstr_newL(";");
            main.NewLine();
        }
    }
   

    void dsl_cpp_generator::generateFixedPointUpdate(PropAccess* propId) 
    {
        char strBuffer[1024];
        Identifier* id2 = propId->getIdentifier2();
        if (checkFixedPointAssociation(propId)) {
            sprintf(strBuffer, "%s = %s ;", id2->getSymbolInfo()->getId()->get_fpId(), "false");
        main.pushstr_newL(strBuffer);
        }
    }
    
    void dsl_cpp_generator::generateAssignmentStmt(assignment* assignStmt)
    {
        char strBuffer[1024];
        Expression* exprAssigned = assignStmt->getExpr();
        vector<Identifier*> graphIds = graphId[curFuncType][curFuncCount()];

        if (assignStmt->lhs_isIdentifier()) {
            Identifier* id = assignStmt->getId();
            if (assignStmt->hasPropCopy())  // prop_copy is of the form (propId = propId)
            {
                if(insideParallelConstruct.size()>0)
                {
                    // cant assign prop copy inside a parallel region?
                    assert(false);
                }
                Type * type = id->getSymbolInfo()->getType();
                char strBuffer[1024];
                //Identifier* rhsPropId2 = exprAssigned->getId();
                    main.pushString(id->getIdentifier());
                    main.pushString(" = ");
                    generateExpr(exprAssigned);
                    main.pushstr_newL(";");
            } 
            else
            {
                // we can directly generate as we cant have conflicting assignment for global variable inside
                // non-parallel region
                if(insideParallelConstruct.size()==0)
                {
                    main.pushString(id->getIdentifier());
                    main.pushString(" = ");
                    generateExpr(assignStmt->getExpr());
                    main.pushstr_newL(";");
                }
                else
                {
                    if(id->getSymbolInfo()->isGlobalVariable())
                    {   

                        // Barenya : Commenting out some code generation that makes thigns incorrect.
                        sprintf(strBuffer,"%s_leader_rank",id->getIdentifier());
                        main.pushString(strBuffer);
                        main.pushString(" = world.rank()");
                        main.pushstr_newL(";");

                        main.pushString(id->getIdentifier());
                        main.pushString(" = ");
                        generateExpr(assignStmt->getExpr());
                        main.pushstr_newL(";");
                    }
                    else 
                    {
                        main.pushString(id->getIdentifier());
                        main.pushString(" = ");
                        generateExpr(assignStmt->getExpr());
                        main.pushstr_newL(";");
                    }
                }
            }    
        } 
        else if (assignStmt->lhs_isProp())  //the check for node and edge property to be carried out.
        {
            PropAccess* propId = assignStmt->getPropId();
            //if (assignStmt->getAtomicSignal()) {
      /*if(asmt->getParent()->getParent()->getParent()->getParent()->getTypeofNode()==NODE_ITRBFS)
           if(asmt->getExpr()->isArithmetic()||asmt->getExpr()->isLogical())*/
            //main.pushstr_newL("#pragma omp atomic");
            //}
            if(!propId->isPropertyExpression())
            {
                if(insideParallelConstruct.size()==0)
                {   
                    // if(propId->getIdentifier2()->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPNODE) 
                        // sprintf(strBuffer, "if(world.rank() == %s.%s(%s))", graphIds[0]->getIdentifier(),"get_node_owner", propId->getIdentifier1()->getIdentifier());
                    // else if(propId->getIdentifier2()->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPEDGE)
                        // sprintf(strBuffer, "if(world.rank() == %s.%s(%s))", graphIds[0]->getIdentifier(),"get_edge_owner", propId->getIdentifier1()->getIdentifier());
                    // main.pushstr_newL(strBuffer);
                    // main.pushstr_newL("{");
                }    

                if(assignStmt->getExpr()->isArithmetic() && assignStmt->getExpr()->getLeft()->isPropIdExpr() )
                {
                    PropAccess* propIdExpr = assignStmt->getExpr()->getLeft()->getPropId();
                    string propId1(propId->getIdentifier1()->getIdentifier());
                    string propId2(propId->getIdentifier2()->getIdentifier());
                    string propIdExpr1(propIdExpr->getIdentifier1()->getIdentifier());
                    string propIdExpr2(propIdExpr->getIdentifier2()->getIdentifier());
                
                    // Find out whether there is a better way to do this
                    if(propId1.compare(propIdExpr1)==0 && propId2.compare(propIdExpr2)==0)
                    {
                        if(assignStmt->getExpr()->getOperatorType() == OPERATOR_ADD)
                        {
                            sprintf(strBuffer, "%s.atomicAdd(%s,",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
                        }
                        else
                        {
                            // Add similar atomics for other types of similar operators
                            assert(false);
                        }
                        main.pushString(strBuffer);
                        generateExpr(assignStmt->getExpr()->getRight());
                        main.pushstr_newL(");");
                    }
                    else
                    {
                        sprintf(strBuffer,"%s.setValue(%s,",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
                        main.pushString(strBuffer);
                        generateExpr(assignStmt->getExpr());
                        main.pushstr_newL(");");        
                    }
                }
                else{
                    sprintf(strBuffer,"%s.setValue(%s,",propId->getIdentifier2()->getIdentifier(),propId->getIdentifier1()->getIdentifier());
                    main.pushString(strBuffer);
                    generateExpr(assignStmt->getExpr());
                    main.pushstr_newL(");");
                }

                // if(insideParallelConstruct.size()==0)    
                    // main.pushstr_newL("}");

                Identifier* id2 = propId->getIdentifier2();
                if (id2->getSymbolInfo() != NULL && id2->getSymbolInfo()->getId()->get_fp_association()) {
                    generateFixedPointUpdate(propId);
                }
            }
            else{
                if(insideParallelConstruct.size()==0)
                {   
                    if(propId->getIdentifier2()->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPNODE) 
                        sprintf(strBuffer, "if(world.rank() == %s.%s(%s))", graphIds[0]->getIdentifier(),"get_node_owner", propId->getIdentifier1()->getIdentifier());
                    else if(propId->getIdentifier2()->getSymbolInfo()->getType()->gettypeId()==TYPE_PROPEDGE)
                        sprintf(strBuffer, "if(world.rank() == %s.%s(%s))", graphIds[0]->getIdentifier(),"get_edge_owner", propId->getIdentifier1()->getIdentifier());
                    main.pushstr_newL(strBuffer);
                    main.pushstr_newL("{");
                }
                Expression * indexExpr = propId->getPropExpr();
                generate_exprIndexExpr(indexExpr);
                sprintf(strBuffer,".setValue(%s,", propId->getIdentifier1()->getIdentifier());
                main.pushString(strBuffer);
                generateExpr(assignStmt->getExpr());
                main.pushstr_newL(");");
                if(insideParallelConstruct.size()==0)    
                    main.pushstr_newL("}");

            }    
        }
        else if(assignStmt->lhs_isIndexAccess()){
            Expression * indexAccess = assignStmt->getIndexAccess();
            generate_exprIndexExpr(indexAccess);
            main.pushString(" = ");
            generateExpr(assignStmt->getExpr());
            main.pushstr_newL(";");
        }

           
    }

    void dsl_cpp_generator::generateReductionOpStmt(reductionCallStmt* stmt) {
        if (stmt->isLeftIdentifier()) {
            Identifier* id = stmt->getLeftId();
            main.pushString(id->getIdentifier());
            main.pushString(" = ( ");
            main.pushstr_space(id->getIdentifier());
            const char* operatorString = getOperatorString(stmt->reduction_op());
            main.pushstr_space(operatorString);
            generateExpr(stmt->getRightSide());
            main.pushString (") ") ;
            main.pushstr_newL(";");

        } 
        // TODO : Yet to update this
        else if (stmt->isContainerReduc() ) {
          auto containerName = stmt->getMapExprReduc()->getId()->getIdentifier() ;
          main.pushString (containerName) ;
          main.pushString (".") ;
          // TODO : Barenya : make this a little more operation agnostic
          main.pushString ("atomicAdd (") ;
          generateExpr (stmt->getIndexExprReduc ()) ;
          main.pushString (", ") ;
          generateExpr (stmt->getRightSide()) ;
          main.pushString (")") ;
          main.pushstr_newL(";") ;
        }
        else {
            // generate_exprPropId(stmt->getPropAccess());
            auto propId = stmt->getPropAccess () ;
            auto index = propId->getIdentifier1 () -> getIdentifier ()  ;
            auto propertyName = propId->getIdentifier2 () -> getIdentifier ();
            main.pushString (propertyName) ;
            // main.pushString(" = ");
            // generate_exprPropId(stmt->getPropAccess());
            // main.pushString (stmt->getLeftId()->getIdentifier()) ;
            main.pushString (".") ;
            const char* operatorString = getOperatorString(stmt->reduction_op());
            if (!strcmp(operatorString ,(const char*) "+")) main.pushString ("atomicAdd (") ;
            if (!strcmp(operatorString ,(const char*) "-")) main.pushString ("atomicAdd (") ;
            main.pushString (index) ;
            main.pushString (", ") ;
            if (!strcmp(operatorString ,(const char*) "-")) main.pushString ("-") ;
            // main.pushstr_space(operatorString);
            generateExpr(stmt->getRightSide());
            main.pushString (")") ;
            main.pushstr_newL(";");
        }
    }

    void dsl_cpp_generator::generateReductionStmt(reductionCallStmt* stmt) {
        if (stmt->is_reducCall()) {
            generateReductionCallStmt(stmt);
        } else {
            generateReductionOpStmt(stmt);
        }
    }

    void dsl_cpp_generator::generateIfStmt(ifStmt* ifstmt)
    {
        cout << "INSIDE IF STMT"
       << "\n";
        Expression* condition = ifstmt->getCondition();
        main.pushString("if (");
        cout << "TYPE OF IFSTMT" << condition->getTypeofExpr() << "\n";
        generateExpr(condition);
        main.pushstr_newL(" )");
        
        if(ifstmt->getIfBody()->getTypeofNode()==NODE_BLOCKSTMT)
            generateStatement(ifstmt->getIfBody());
        else
        {
            main.pushstr_newL("{");
            generateStatement(ifstmt->getIfBody());
            main.pushstr_newL("}");
        }   

        if (ifstmt->getElseBody() == NULL)
            return;
        main.pushstr_newL("else");
        generateStatement(ifstmt->getElseBody());
    }

    void dsl_cpp_generator::generateStatement(statement* stmt) 
    {
        
        if (stmt->getTypeofNode() == NODE_BLOCKSTMT) 
            {std::cout<<"inside1"<<std::endl;generateBlock((blockStatement*)stmt);std::cout<<"inside1"<<std::endl;}
        
        if (stmt->getTypeofNode() == NODE_DECL) 
            {std::cout<<"inside2"<<std::endl;generateVariableDecl((declaration*)stmt);
        std::cout<<"inside2"<<std::endl;}
        if (stmt->getTypeofNode() == NODE_ASSIGN) 
            {std::cout<<"inside3"<<std::endl;generateAssignmentStmt((assignment*)stmt);
  std::cout<<"inside3"<<std::endl;}

        if (stmt->getTypeofNode() == NODE_WHILESTMT) 
            {std::cout<<"inside4"<<std::endl;generateWhileStmt((whileStmt*)stmt);
        std::cout<<"inside4"<<std::endl;}

        if (stmt->getTypeofNode() == NODE_IFSTMT) 
            {std::cout<<"inside5"<<std::endl;generateIfStmt((ifStmt*)stmt);
  std::cout<<"inside5"<<std::endl;}

        if (stmt->getTypeofNode() == NODE_DOWHILESTMT) 
            {std::cout<<"inside6"<<std::endl;generateDoWhileStmt((dowhileStmt*)stmt);
  std::cout<<"inside6"<<std::endl;}

        if (stmt->getTypeofNode() == NODE_FORALLSTMT) 
            {std::cout<<"inside7"<<std::endl;generateForAll((forallStmt*)stmt);
  std::cout<<"inside7"<<std::endl;}

        if (stmt->getTypeofNode() == NODE_FIXEDPTSTMT) 
            {std::cout<<"inside8"<<std::endl;generateFixedPoint((fixedPointStmt*)stmt);
        std::cout<<"inside8"<<std::endl;}
        if (stmt->getTypeofNode() == NODE_REDUCTIONCALLSTMT) 
            {std::cout<<"inside9"<<std::endl;generateReductionStmt((reductionCallStmt*)stmt);
        std::cout<<"inside9"<<std::endl;}
        if (stmt->getTypeofNode() == NODE_ITRBFS) 
            {std::cout<<"inside10"<<std::endl;generateBFSAbstraction((iterateBFS*)stmt);
  std::cout<<"inside10"<<std::endl;}
        if (stmt->getTypeofNode() == NODE_PROCCALLSTMT) 
            {std::cout<<"inside11"<<std::endl;generateProcCall((proc_callStmt*)stmt);
  std::cout<<"inside11"<<std::endl;}
        if (stmt->getTypeofNode() == NODE_UNARYSTMT) 
        {
            unary_stmt* unaryStmt = (unary_stmt*)stmt;
            generateExpr(unaryStmt->getUnaryExpr());
            main.pushstr_newL(";");
        }
        if (stmt->getTypeofNode() == NODE_RETURN) 
        {
            returnStmt* returnStmtNode = (returnStmt*)stmt;
            main.pushstr_space("return");
            generateExpr(returnStmtNode->getReturnExpression());
            main.pushstr_newL(";");
        }
    }

    void dsl_cpp_generator::generateForAll(forallStmt* forAll) 
    {
        char strBuffer[1024];
        if (forAll->isForall()) {

            insideParallelConstruct.push_back(forAll);
            
        
            if (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc()) {
                bool already_remebered = false;
                Expression* filterExpr = NULL;
                
                
                if (forAll->hasFilterExpr())
                {    
                    filterExpr = forAll->getfilterExpr();

                    Expression* lhs = filterExpr->getLeft();
                    Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
                    TableEntry* tableEntry = filterId != NULL ? filterId->getSymbolInfo() : NULL;
                    if (tableEntry != NULL && tableEntry->getId()->get_used_inside_forall_filter_and_changed_inside_forall_body()) 
                    {
                        sprintf(strBuffer,"%s.rememberHistory();", filterId->getIdentifier());
                        already_remebered = true;
                        main.pushstr_newL(strBuffer);

                        filterId->set_forall_filter_association();
                    } 
                }
                else if(forAll->hasFilterExprAssoc())
                {
                    filterExpr = forAll->getAssocExpr();

                    Expression* lhs = filterExpr->getLeft();
                    Identifier* filterId = lhs->isIdentifierExpr() ? lhs->getId() : NULL;
                    TableEntry* tableEntry = filterId != NULL ? filterId->getSymbolInfo() : NULL;
                    if (tableEntry != NULL && tableEntry->getId()->get_fp_association()) 
                    {
                        sprintf(strBuffer,"%s.remeberHistory()", filterId->getIdentifier());
                        if(!already_remebered)
                            main.pushstr_newL(strBuffer);

                        filterId->set_forall_filter_association();
                    }
                }    
            } 
            
            if(forAll->getModifiedGlobalVariables().size() > 0)
            {
                for(TableEntry * te : forAll->getModifiedGlobalVariables())
                {
                    sprintf(strBuffer,"int %s_leader_rank = -1 ;", te->getId()->getIdentifier());
                    main.pushstr_newL(strBuffer);
                }
                main.NewLine();
            }

            std::cout<<"here befor\n";
            if(forAll->containsReductionStatement())
            {std::cout<<"here insider\n";
                reductionCallStmt * reductionStatement = (reductionCallStmt *)forAll->getReductionStatement();
                main.pushString("std::vector<Property *> temp_properties = {");
                std::list<ASTNode* > leftlist = reductionStatement->getLeftList();
                std::list<ASTNode* >::iterator itr = leftlist.begin();
                itr++;
                while(itr!=leftlist.end())
                {
                    // only propacess nodes allowed inside reduction statement for now for mpi
                    if((*itr)->getTypeofNode()!=NODE_PROPACCESS)
                        assert(false);

                    PropAccess * prop = (PropAccess*) (*itr);
                    sprintf(strBuffer,"(Property*)&%s",prop->getIdentifier2()->getIdentifier());
                    main.pushString(strBuffer);
                    itr++;
                    if(itr!=leftlist.end())
                        main.pushString(", ");
                }
                main.pushstr_newL("};");
                reductionCall* reduceCall = reductionStatement->getReducCall();
                
                if(leftlist.front()->getTypeofNode()!=NODE_PROPACCESS)
                        assert(false);
                PropAccess * reductionProperty = (PropAccess*) leftlist.front();       
                sprintf(strBuffer,"%s.initialise_reduction(%s, (Property*)&%s, temp_properties);",forAll->getSourceGraph()->getIdentifier(),getReductionOperatorString(reduceCall->getReductionType()),reductionProperty->getIdentifier2()->getIdentifier());
                main.pushstr_newL(strBuffer);
            }
            std::cout<<"here after\n";

            main.pushstr_newL("world.barrier();");
        }


        proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
        PropAccess* sourceField = forAll->getPropSource();
        Identifier* sourceId = forAll->getSource();
        Identifier* collectionId;
        
        if (sourceField != NULL) {
            collectionId = sourceField->getIdentifier1();
            //main.pushString("here1\n");
        }
        if (sourceId != NULL) {
            collectionId = sourceId;
            //main.pushString("here2\n");
        }

        Identifier* iteratorMethodId;
        if (extractElemFunc != NULL)
        {
            iteratorMethodId = extractElemFunc->getMethodId();
            //main.pushString("here3\n");
        }

        statement* body = forAll->getBody();
        
        analysisForAll = new bAnalyzer () ;

        analysisForAll->analyzeForAllStmt (forAll) ;  
        
        generateForAllSignature(forAll);

        if (analysisForAll->getAnalysisStatus() > 0) {
          blockStatement * newBody = analysisForAll->getNewBody () ; 
          forAll->setBody (newBody) ;
        }

        if (forAll->hasFilterExpr()) {
          analysisForAll->evaluateFilter (forAll, (blockStatement *)forAll->getBody (), forAll->getfilterExpr()) ;
          if (analysisForAll->getFilterAnalysisStatus()) {
            blockStatement * addToQueueBody = analysisForAll-> getStatementForLoop () ;
            // Barenya : Could cause potential deletion of the loop body.
            forAll->setBody (addToQueueBody) ;
          }
          blockStatement* changedBody = includeIfToBlock(forAll);
          forAll->setBody (changedBody);
        }

         

        if (extractElemFunc != NULL) {
            forallStack.push_back(make_pair(forAll->getIterator(), forAll->getExtractElementFunc()));
            callStackForAnalyzer.push (analysisForAll) ;
            if (neighbourIteration(iteratorMethodId->getIdentifier())) 
            {
                generateStatement(forAll->getBody());
            } 
            else 
            {
                generateStatement(forAll->getBody());
            }

            if (forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc())) {
                //main.pushstr_newL("world.barrier();");
                //checkAndGenerateFixedPtFilter(forAll);
            }

            forallStack.pop_back();
            analysisForAll = callStackForAnalyzer.top () ;
            callStackForAnalyzer.pop () ;
        } else {
            // If Forall is iterating through a set 
            if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) 
            {
                if (body->getTypeofNode() == NODE_BLOCKSTMT)
                    main.pushstr_newL("{");
                
                sprintf(strBuffer, "int %s = *itr;", forAll->getIterator()->getIdentifier());
                main.pushstr_newL(strBuffer);
                if (body->getTypeofNode() == NODE_BLOCKSTMT) 
                {
                    generateBlock((blockStatement*)body, false);
                    main.pushstr_newL("}");
                } 
                else
                    generateStatement(forAll->getBody());

            } 
            else if (collectionId->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES) 
            {
                generateStatement(body);
            } 
            else 
            {
                cout << iteratorMethodId->getIdentifier() << "\n";
                generateStatement(forAll->getBody());
            }

            if (forAll->isForall() && (forAll->hasFilterExpr() || forAll->hasFilterExprAssoc())) 
            {
                //main.pushstr_newL("world.barrier();");
                //checkAndGenerateFixedPtFilter(forAll);
            }
        }
        if(forAll->isForall() && forAll->containsReductionStatement())
        {
            sprintf(strBuffer,"%s.sync_reduction();",forAll->getSourceGraph()->getIdentifier());
            main.pushstr_newL(strBuffer);
        }
        if(forAll->isForall() && !ifStatementInForAll)
            main.pushstr_newL("world.barrier();");
        // Genereate code related to reduction at the end of for all

        if(forAll->isForall() &&  forAll->getModifiedGlobalVariables().size() > 0)
            {
                for(TableEntry * te : forAll->getModifiedGlobalVariables())
                {
                    sprintf(strBuffer,"int %s_leader_rank_temp = %s_leader_rank;", te->getId()->getIdentifier(), te->getId()->getIdentifier());
                    main.pushstr_newL(strBuffer);
                    sprintf(strBuffer, "MPI_Allreduce(&%s_leader_rank_temp,&%s_leader_rank,1,%s,%s,MPI_COMM_WORLD);",te->getId()->getIdentifier(),te->getId()->getIdentifier() ,"MPI_INT","MPI_MAX");
                    main.pushstr_newL(strBuffer);
                    sprintf(strBuffer,"MPI_Bcast(&%s,1,%s,%s_leader_rank,MPI_COMM_WORLD);",te->getId()->getIdentifier(),getMPItype(te->getType()),te->getId()->getIdentifier());
                    main.pushstr_newL(strBuffer);
                    main.NewLine();
                }
                main.NewLine();
            }

        if (forAll->isForall() && forAll->get_reduceKeys().size() > 0) {
            printf("INSIDE GENERATE FOR ALL FOR KEYS\n");

            set<int> reduce_Keys = forAll->get_reduceKeys();
            assert(reduce_Keys.size() == 1);
            
            set<int>::iterator it;
            for(it = reduce_Keys.begin(); it != reduce_Keys.end();it++)
            {
                list<Identifier*> op_List = forAll->get_reduceIds(*it);
                list<Identifier*>::iterator list_itr;
                main.NewLine();
            
            
            //main.pushString(strBuffer);
                for (list_itr = op_List.begin(); list_itr != op_List.end(); list_itr++) {
                    Identifier* id = *list_itr;
                    //main.pushString(id->getIdentifier());
                    char strBuffer[1024];
                    sprintf(strBuffer, "%s %s_temp = %s;",convertToCppType(id->getSymbolInfo()->getType(),false), id->getIdentifier(),id->getIdentifier());
                    main.pushstr_newL(strBuffer);
                    sprintf(strBuffer, "MPI_Allreduce(&%s_temp,&%s,1,%s,%s,MPI_COMM_WORLD);",id->getIdentifier(),id->getIdentifier() ,getMPItype(id->getSymbolInfo()->getType()),getMPIreduction(*it));
                
                    main.pushstr_newL(strBuffer);
                }
            }
                //main.pushString(")");
        }
        main.NewLine();

        if(forAll->isForall())
            insideParallelConstruct.pop_back();
        
        // Barenya :
        if (forAll->isForall() && ifStatementInForAll) {
            main.pushstr_newL ("}") ;
            ifStatementInForAll = true ;
            main.pushstr_newL ("world.barrier () ;") ;
        }
        main.NewLine () ;
        if (analysisForAll->getFilterAnalysisStatus() == true)
          generateStatement ((statement*)analysisForAll->getStatementWithinWhileLoop () ) ;
        analysisForAll->clearAllAnalysis () ;
    }

    void dsl_cpp_generator::generateForAllSignature(forallStmt* forAll) {
        char strBuffer[1024];
        Identifier* iterator = forAll->getIterator();
        if (forAll->isSourceProcCall()) {
            Identifier* sourceGraph = forAll->getSourceGraph();
            proc_callExpr* extractElemFunc = forAll->getExtractElementFunc();
            Identifier* iteratorMethodId = extractElemFunc->getMethodId();
            if (allGraphIteration(iteratorMethodId->getIdentifier())) {
                char* graphId = sourceGraph->getIdentifier();
                char* methodId = iteratorMethodId->getIdentifier();
                string s(methodId);
                if (s.compare("nodes") == 0) {
                    cout << "INSIDE NODES VALUE"
                    << "\n";
                    if(forAll->isForall()) {
                        sprintf(strBuffer, "for (%s %s = %s.%s(); %s <= %s.%s(); %s ++) ", "int", iterator->getIdentifier(),graphId,"start_node" ,iterator->getIdentifier(), graphId, "end_node", iterator->getIdentifier());
                    // ifStatementInForAll = true ;
                    } else 
                        sprintf(strBuffer, "for (%s %s = 0; %s < %s.%s(); %s ++) ", "int", iterator->getIdentifier() ,iterator->getIdentifier(), graphId, "num_nodes", iterator->getIdentifier());
                } else
                    //TODO :(Atharva) start_edge and end_edge is not yet supported in header file
                    if(forAll->isForall())
                        sprintf(strBuffer, "for (%s %s = %s.%s(); %s <= %s.%s(); %s ++) ", "int", iterator->getIdentifier(),graphId,"start_edge" ,iterator->getIdentifier(), graphId, "end_edge", iterator->getIdentifier());
                    else
                        sprintf(strBuffer, "for (%s %s = 0; %s < %s.%s(); %s ++) ", "int", iterator->getIdentifier(), iterator->getIdentifier(), graphId, "num_edges", iterator->getIdentifier());

                main.pushstr_newL(strBuffer);

            } else if (neighbourIteration(iteratorMethodId->getIdentifier())) {
                char* graphId = sourceGraph->getIdentifier();
                char* methodId = iteratorMethodId->getIdentifier();
                string s(methodId);
                strBuffer[0]='\0';
                
                if (s.compare("neighbors") == 0) {
                    // int analysisStatus = analysisForAll->analyzeForAllStmt (forAll) ;
                    // printf ("analysis returned %d\n", analysisStatus) ;
                    list<argument*> argList = extractElemFunc->getArgList();
                    assert(argList.size() == 1);
                    Identifier* nodeNbr = argList.front()->getExpr()->getId();
                    int analysisStatus = analysisForAll->getAnalysisStatus () ;
                    printf ("analysisStats = %d\n", analysisStatus) ;
                    if (analysisStatus) {
                      char * variableIter = analysisForAll->getIteratorVar () ;
                      printf ("LOG : variableIter = %s\n", variableIter) ;
                      sprintf (strBuffer, "int %s = 0 ;\n", variableIter) ;
                      main.pushstr_newL (strBuffer) ;
                    }
                    if(forAll->isForall()){
                        sprintf (strBuffer, "if ( %s != -1 && world.rank () == g.get_node_owner (%s) )\n { for (%s %s:%s.%s(%s))", nodeNbr->getIdentifier(), nodeNbr->getIdentifier(), "int", iterator->getIdentifier(), graphId, "getNeighbors", nodeNbr->getIdentifier()) ;
                        ifStatementInForAll = true ;
                        main.pushstr_newL (strBuffer) ;
                    }
                    //This is hardcoded here, need to change     
                    // Barenya : Hardcoding here is causing analysis to fail. TO -> Issue with nested forAlls only.
                    // else if(forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRBFS || forAll->getParent()->getParent()->getTypeofNode() == NODE_ITRRBFS){
                        // sprintf(strBuffer, "for (%s %s :%s.%s(%s))","int",iterator->getIdentifier(), graphId, "get_bfs_children", nodeNbr->getIdentifier());
                    // }
                    else {
                        sprintf(strBuffer, "for (%s %s : %s.%s(%s)) ", "int", iterator->getIdentifier(), graphId, "getNeighbors", nodeNbr->getIdentifier());
                        main.pushstr_newL(strBuffer);
                    }
                }
                if (s.compare("nodes_to") == 0) {
                    list<argument*> argList = extractElemFunc->getArgList();
                    assert(argList.size() == 1);
                    Identifier* nodeNbr = argList.front()->getExpr()->getId();
                    if(forAll->isForall())
                        //TODO : Yet to add, add for loop and add another if condition so that proc will consider 
                        // only those neighbors which the process owns.

                        assert(false);
                    else
                        sprintf(strBuffer, "for (%s %s : %s.%s(%s)) ", "int", iterator->getIdentifier(), graphId, "getInNeighbors", nodeNbr->getIdentifier());
                    main.pushstr_newL(strBuffer);
                    
                }  
            }
        } else if (forAll->isSourceField()) {
            // TODO : Yet to add code for this case
            

        } else {
            Identifier* sourceId = forAll->getSource();
            if (sourceId != NULL) {
                if (sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_SETN) {    
                        main.pushstr_newL("std::set<int>::iterator itr;");
                        sprintf(strBuffer, "for(itr=%s.begin();itr!=%s.end();itr++)", sourceId->getIdentifier(), sourceId->getIdentifier());
                        main.pushstr_newL(strBuffer);
                }
                else if(sourceId->getSymbolInfo()->getType()->gettypeId() == TYPE_UPDATES)
                {
                    sprintf(strBuffer, "for(Update %s : %s->getUpdates())", forAll->getIterator()->getIdentifier(), sourceId->getIdentifier());
                    main.pushstr_newL(strBuffer);
                }
            }
        }
    }    


        blockStatement* dsl_cpp_generator::includeIfToBlock(forallStmt* forAll) {
        Expression* filterExpr = forAll->getfilterExpr();
        statement* body = forAll->getBody();
        Expression* modifiedFilterExpr = filterExpr;
        

        //TODO : (ATharva) This is not yet supported, code might be invalid
        if (filterExpr->getExpressionFamily() == EXPR_RELATIONAL) {
            Expression* expr1 = filterExpr->getLeft();
            Expression* expr2 = filterExpr->getRight();
            if (expr1->isIdentifierExpr()) {
      /*if it is a nodeproperty, the filter is on the nodes that are iterated on
    One more check can be applied to check if the iterating type is a neigbor iteration
    or allgraph iterations.
   */
                if (expr1->getId()->getSymbolInfo() != NULL) {
                    if (expr1->getId()->getSymbolInfo()->getType()->isPropNodeType()) {
                        Identifier* iterNode = forAll->getIterator();
                        Identifier* nodeProp = expr1->getId();
                        PropAccess* propIdNode = (PropAccess*)Util::createPropIdNode(iterNode, nodeProp);
                        Expression* propIdExpr = Expression::nodeForPropAccess(propIdNode);
                        modifiedFilterExpr = (Expression*)Util::createNodeForRelationalExpr(propIdExpr, expr2, filterExpr->getOperatorType());
                    }
                }
            }
    /* if(expr1->isPropIdExpr()&&expr2->isBooleanLiteral()) //specific to sssp. Need to revisit again to change it.
    {   PropAccess* propId=expr1->getPropId();
        bool value=expr2->getBooleanConstant();
        Expression* exprBool=(Expression*)Util::createNodeForBval(!value);
        assignment* assign=(assignment*)Util::createAssignmentNode(propId,exprBool);
        if(body->getTypeofNode()==NODE_BLOCKSTMT)
        {
          blockStatement* newbody=(blockStatement*)body;
          newbody->addToFront(assign);
          body=newbody;
        }

        modifiedFilterExpr = filterExpr;
    }
  */
        }
        ifStmt* ifNode = (ifStmt*)Util::createNodeForIfStmt(modifiedFilterExpr, forAll->getBody(), NULL);
        blockStatement* newBlock = new blockStatement();
        newBlock->setTypeofNode(NODE_BLOCKSTMT);
        newBlock->addStmtToBlock(ifNode);
        return newBlock;
    }

    void dsl_cpp_generator::generateVariableDecl(declaration* decl)
    {
        Type* type = decl->getType();
        char strBuffer[1024];

        printf ("inside variable declation but outside propType") ;

        if (type->isPropType()) 
        {
          printf ("inside variable declation\n") ;
            if (type->getInnerTargetType()->isPrimitiveType()) 
            {
                Type* innerType = type->getInnerTargetType();
                
                main.pushString(convertToCppType(type));  
                main.space();
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized())
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");
                    
            }
            else {
                // NOT yet supported
                assert(false);
            }

        
        } 
        else if (type->isPrimitiveType()) 
        {
            printf ("get_edge maps to primitive ?? Don't think so\n") ;
            main.pushstr_space(convertToCppType(type));
            main.pushString(decl->getdeclId()->getIdentifier());
            if (decl->isInitialized()) 
            {
                main.pushString(" = ");
            /* the following if conditions is for cases where the 
                predefined functions are used as initializers
                but the variable's type doesnot match*/
                if (decl->getExpressionAssigned()->getExpressionFamily() == EXPR_PROCCALL) {
                    proc_callExpr* pExpr = (proc_callExpr*)decl->getExpressionAssigned();
                    Identifier* methodId = pExpr->getMethodId();
                    castIfRequired(type, methodId, main);
                }
                generateExpr(decl->getExpressionAssigned());
                main.pushstr_newL(";");
            } 
            else 
            {
                main.pushString(" = ");
                getDefaultValueforTypes(type->gettypeId());
                main.pushstr_newL(";");
            }

        } 
        else if (type->isNodeEdgeType()) 
        {
            printf ("get edge maps to isNodeEdgeType () \n") ;
            main.pushstr_space(convertToCppType(type));
            main.pushString(decl->getdeclId()->getIdentifier());
            if (decl->isInitialized()) 
            {
                main.pushString(" = ");
                generateExpr(decl->getExpressionAssigned());
                
            }
            main.pushstr_newL(";");

        } 
        else if (type->isCollectionType()) 
        {
            if (type->gettypeId() == TYPE_UPDATES) 
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");

            }
            else if(type->gettypeId() == TYPE_NODEMAP)
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");                
            }
            else if(type->gettypeId() == TYPE_CONTAINER)
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");
            }
            else if(type->gettypeId() == TYPE_VECTOR) 
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");
            }
			else if(type->gettypeId() == TYPE_HASHMAP)
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");
            }
			else if(type->gettypeId() == TYPE_HASHSET)
            {
                main.pushstr_space(convertToCppType(type));
                main.pushString(decl->getdeclId()->getIdentifier());
                if (decl->isInitialized()) 
                {
                    main.pushString(" = ");
                    generateExpr(decl->getExpressionAssigned());
                }
                main.pushstr_newL(";");
            }
        }
    }

    void dsl_cpp_generator::generateDoWhileStmt(dowhileStmt* doWhile)
    {
        main.pushstr_newL("do");
        generateStatement(doWhile->getBody());
        main.pushString("while(");
        generateExpr(doWhile->getCondition());
        main.pushString(");");
    }


    void dsl_cpp_generator::generateWhileStmt(whileStmt* whilestmt)
    {
        Expression* conditionExpr = whilestmt->getCondition();
        main.pushString("while (");
        generateExpr(conditionExpr);
        main.pushString(" )");
        generateStatement(whilestmt->getBody());
    }
}
