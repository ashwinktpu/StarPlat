/*enum for various graph characteristics*/
#ifndef ENUMDEF_H
#define ENUMDEF_H

enum TYPE
{
 TYPE_LONG,
 TYPE_INT,
 TYPE_BOOL,
 TYPE_FLOAT,
 TYPE_DOUBLE,
 TYPE_GRAPH,
 TYPE_DIRGRAPH,
 TYPE_LIST,
 TYPE_SETN,
 TYPE_SETE,
 TYPE_NODE,
 TYPE_EDGE,
 TYPE_PROPNODE,
 TYPE_PROPEDGE,
 TYPE_NONE,
 TYPE_UPDATES,
 TYPE_CONTAINER,
 TYPE_NODEMAP,
 TYPE_VECTOR,
 TYPE_HASHMAP,
 TYPE_HASHSET,
 TYPE_AUTOREF,
};

inline bool check_isNodeEdgeType(int typeId)
{
  return ((typeId==TYPE_NODE)||(typeId==TYPE_EDGE));
}
inline bool check_isPropType(int typeId)
{
  return ((typeId==TYPE_PROPNODE)||(typeId==TYPE_PROPEDGE));
}
inline bool check_isCollectionType(int typeId)
{
  return ((typeId == TYPE_LIST)||(typeId==TYPE_SETE)||(typeId==TYPE_SETN)||(typeId == TYPE_UPDATES) || (typeId == TYPE_NODEMAP) || (typeId == TYPE_CONTAINER) || (typeId == TYPE_VECTOR) || (typeId == TYPE_HASHMAP) || (typeId == TYPE_HASHSET));
}
inline bool check_isGraphType(int typeId)
{
  return ((typeId==TYPE_GRAPH)||(typeId==TYPE_DIRGRAPH));
}
inline bool check_isPrimitiveType(int typeId)
  {
    return ((typeId==TYPE_BOOL)||(typeId==TYPE_DOUBLE)||(typeId==TYPE_FLOAT)||(typeId==TYPE_LONG)||(typeId==TYPE_INT)||(typeId==TYPE_AUTOREF));
  }

 inline bool check_isPropNodeType(int typeId)
 {
   return typeId==TYPE_PROPNODE;
 }

 inline bool check_isPropEdgeType(int typeId)
 {
   return typeId==TYPE_PROPEDGE;
 }
inline bool check_isListCollectionType(int typeId)
{
  return typeId==TYPE_LIST;
}
inline bool check_isSetCollectionType(int typeId)
{
  return ((typeId==TYPE_SETN)||(typeId==TYPE_SETE));
}
inline bool check_isNodeType(int typeId)
{
  return typeId == TYPE_NODE;
}
inline bool check_isEdgeType(int typeId)
{
  return typeId == TYPE_EDGE;
}
inline bool check_isContainerType(int typeId)
{

 return typeId == TYPE_CONTAINER;
   
}
inline bool check_isNodeMapType(int typeId)
{
 
return typeId == TYPE_NODEMAP;

}
inline bool check_isHashMapType(int typeId)
{

 return typeId == TYPE_HASHMAP;
   
}
inline bool check_isHashSetType(int typeId)
{

 return typeId == TYPE_HASHSET;
   
}

enum REDUCE
{
    REDUCE_SUM,
    REDUCE_COUNT,
    REDUCE_PRODUCT,
    REDUCE_MAX,
    REDUCE_MIN,
   

};

enum OPERATOR
{
 OPERATOR_ADD,
 OPERATOR_SUB,
 OPERATOR_MUL,
 OPERATOR_DIV,
 OPERATOR_MOD,
 OPERATOR_OR,
 OPERATOR_AND,
 OPERATOR_LT,
 OPERATOR_GT,
 OPERATOR_LE,
 OPERATOR_GE,
 OPERATOR_EQ,
 OPERATOR_NE,
 OPERATOR_NOT,
 OPERATOR_INC,
 OPERATOR_DEC,
 OPERATOR_ADDASSIGN,
 OPERATOR_MULASSIGN,
 OPERATOR_ORASSIGN,
 OPERATOR_ANDASSIGN,
 OPERATOR_SUBASSIGN,
 OPERATOR_INDEX,

};

enum FUNCTYPE
{
  GEN_FUNC,
  STATIC_FUNC,
  INCREMENTAL_FUNC,
  DECREMENTAL_FUNC,
  DYNAMIC_FUNC,
  
};

enum NODETYPE{
  NODE_ID,
  NODE_PROPACCESS,
  NODE_FUNC,
  NODE_TYPE,
  NODE_FORMALPARAM,
  NODE_STATEMENT,
  NODE_BLOCKSTMT,
  NODE_DECL,
  NODE_ASSIGN,
  NODE_WHILESTMT,
  NODE_DOWHILESTMT,
  NODE_FIXEDPTSTMT,
  NODE_IFSTMT,
  NODE_ITRBFS,
  NODE_ITRRBFS,
  NODE_EXPR,
  NODE_PROCCALLEXPR,
  NODE_PROCCALLSTMT,
  NODE_FORALLSTMT,
  NODE_REDUCTIONCALL,
  NODE_REDUCTIONCALLSTMT,
  NODE_UNARYSTMT,
  NODE_RETURN,
  NODE_BATCHBLOCKSTMT,
  NODE_ONADDBLOCK,
  NODE_ONDELETEBLOCK,
  NODE_TRANSFERSTMT,
};


enum EXPR
{
   EXPR_RELATIONAL,
   EXPR_LOGICAL,
   EXPR_ARITHMETIC,
   EXPR_UNARY,
   EXPR_BOOLCONSTANT,
   EXPR_INTCONSTANT,
   EXPR_LONGCONSTANT,
   EXPR_DOUBLECONSTANT,
   EXPR_FLOATCONSTANT,
   EXPR_ID,
   EXPR_PROPID,
   EXPR_INFINITY,
   EXPR_PROCCALL,
   EXPR_DEPENDENT,
   EXPR_MAPGET,
};

static const char *currentBatch = "currentBatch";
static const char *attachNodeCall = "attachNodeProperty";
static const char *attachEdgeCall = "attachEdgeProperty" ;
static const char* nbrCall = "neighbors" ;
static const char* edgeCall   = "get_edge" ;
static const char* countOutNbrCall = "count_outNbrs";
static const char* isAnEdgeCall = "is_an_edge"; 
static const char* nodesToCall = "nodes_to";
static const char* nodesCall = "nodes";

#endif

