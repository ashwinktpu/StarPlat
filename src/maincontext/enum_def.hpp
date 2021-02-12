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
 TYPE_PROPEDGE

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
  return ((typeId==TYPE_LIST)||(typeId==TYPE_SETE)||(typeId==TYPE_SETN));
}
inline bool check_isGraphType(int typeId)
{
  return ((typeId==TYPE_GRAPH)||(typeId==TYPE_DIRGRAPH));
}
inline bool check_isPrimitiveType(int typeId)
  {
    return ((typeId==TYPE_BOOL)||(typeId==TYPE_DOUBLE)||(typeId==TYPE_FLOAT)||(typeId==TYPE_LONG)||(typeId==TYPE_INT));
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
  return typeId==TYPE_NODE;
}
inline bool check_isEdgeType(int typeId)
{
  return typeId==TYPE_EDGE;
}

enum REDUCE
{
    REDUCE_SUM,
    REDUCE_COUNT,
    REDUCE_PRODUCT,
    REDUCE_MAX,
    REDUCE_MIN
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
 OPERATOR_NE


};

enum NODETYPE
{
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
};


enum EXPR
{
   EXPR_RELATIONAL,
   EXPR_LOGICAL,
   EXPR_ARITHMETIC,
   EXPR_BOOLCONSTANT,
   EXPR_INTCONSTANT,
   EXPR_LONGCONSTANT,
   EXPR_DOUBLECONSTANT,
   EXPR_FLOATCONSTANT,
   EXPR_ID,
   EXPR_PROPID,
   EXPR_INFINITY
};
#endif