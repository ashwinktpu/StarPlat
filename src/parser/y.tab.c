/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "lrparser.y"

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
    #include "includeHeader.hpp"
	#include "../analyser/attachProp/attachPropAnalyser.h"
	#include "../analyser/dataRace/dataRaceAnalyser.h"
	#include "../analyser/deviceVars/deviceVarsAnalyser.h"
	#include "../analyser/pushpull/pushpullAnalyser.h"

	#include "../analyser/blockVars/blockVarsAnalyser.h"
	#include<getopt.h>
	//#include "../symbolutil/SymbolTableBuilder.cpp"
     
	void yyerror(const char *);
	int yylex(void);
    extern FILE* yyin;

	char mytext[100];
	char var[100];
	int num = 0;
	vector<map<int,vector<Identifier*>>> graphId(5);
	extern char *yytext;
	//extern SymbolTable* symbTab;
	FrontEndContext frontEndContext;
	map<string,int> push_map;
	set<string> allGpuUsedVars;
	char* backendTarget ;
    vector<Identifier*> tempIds; //stores graph vars in current function's param list.
    //symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());

#line 105 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    T_INT = 258,                   /* T_INT  */
    T_FLOAT = 259,                 /* T_FLOAT  */
    T_BOOL = 260,                  /* T_BOOL  */
    T_DOUBLE = 261,                /* T_DOUBLE  */
    T_LONG = 262,                  /* T_LONG  */
    T_FORALL = 263,                /* T_FORALL  */
    T_FOR = 264,                   /* T_FOR  */
    T_P_INF = 265,                 /* T_P_INF  */
    T_INF = 266,                   /* T_INF  */
    T_N_INF = 267,                 /* T_N_INF  */
    T_FUNC = 268,                  /* T_FUNC  */
    T_IF = 269,                    /* T_IF  */
    T_ELSE = 270,                  /* T_ELSE  */
    T_WHILE = 271,                 /* T_WHILE  */
    T_RETURN = 272,                /* T_RETURN  */
    T_DO = 273,                    /* T_DO  */
    T_IN = 274,                    /* T_IN  */
    T_FIXEDPOINT = 275,            /* T_FIXEDPOINT  */
    T_UNTIL = 276,                 /* T_UNTIL  */
    T_FILTER = 277,                /* T_FILTER  */
    T_ADD_ASSIGN = 278,            /* T_ADD_ASSIGN  */
    T_SUB_ASSIGN = 279,            /* T_SUB_ASSIGN  */
    T_MUL_ASSIGN = 280,            /* T_MUL_ASSIGN  */
    T_DIV_ASSIGN = 281,            /* T_DIV_ASSIGN  */
    T_MOD_ASSIGN = 282,            /* T_MOD_ASSIGN  */
    T_AND_ASSIGN = 283,            /* T_AND_ASSIGN  */
    T_XOR_ASSIGN = 284,            /* T_XOR_ASSIGN  */
    T_OR_ASSIGN = 285,             /* T_OR_ASSIGN  */
    T_INC_OP = 286,                /* T_INC_OP  */
    T_DEC_OP = 287,                /* T_DEC_OP  */
    T_PTR_OP = 288,                /* T_PTR_OP  */
    T_AND_OP = 289,                /* T_AND_OP  */
    T_OR_OP = 290,                 /* T_OR_OP  */
    T_LE_OP = 291,                 /* T_LE_OP  */
    T_GE_OP = 292,                 /* T_GE_OP  */
    T_EQ_OP = 293,                 /* T_EQ_OP  */
    T_NE_OP = 294,                 /* T_NE_OP  */
    T_AND = 295,                   /* T_AND  */
    T_OR = 296,                    /* T_OR  */
    T_SUM = 297,                   /* T_SUM  */
    T_AVG = 298,                   /* T_AVG  */
    T_COUNT = 299,                 /* T_COUNT  */
    T_PRODUCT = 300,               /* T_PRODUCT  */
    T_MAX = 301,                   /* T_MAX  */
    T_MIN = 302,                   /* T_MIN  */
    T_GRAPH = 303,                 /* T_GRAPH  */
    T_DIR_GRAPH = 304,             /* T_DIR_GRAPH  */
    T_NODE = 305,                  /* T_NODE  */
    T_EDGE = 306,                  /* T_EDGE  */
    T_UPDATES = 307,               /* T_UPDATES  */
    T_CONTAINER = 308,             /* T_CONTAINER  */
    T_NODEMAP = 309,               /* T_NODEMAP  */
    T_VECTOR = 310,                /* T_VECTOR  */
    T_NP = 311,                    /* T_NP  */
    T_EP = 312,                    /* T_EP  */
    T_LIST = 313,                  /* T_LIST  */
    T_SET_NODES = 314,             /* T_SET_NODES  */
    T_SET_EDGES = 315,             /* T_SET_EDGES  */
    T_FROM = 316,                  /* T_FROM  */
    T_BFS = 317,                   /* T_BFS  */
    T_REVERSE = 318,               /* T_REVERSE  */
    T_INCREMENTAL = 319,           /* T_INCREMENTAL  */
    T_DECREMENTAL = 320,           /* T_DECREMENTAL  */
    T_STATIC = 321,                /* T_STATIC  */
    T_DYNAMIC = 322,               /* T_DYNAMIC  */
    T_BATCH = 323,                 /* T_BATCH  */
    T_ONADD = 324,                 /* T_ONADD  */
    T_ONDELETE = 325,              /* T_ONDELETE  */
    ID = 326,                      /* ID  */
    INT_NUM = 327,                 /* INT_NUM  */
    FLOAT_NUM = 328,               /* FLOAT_NUM  */
    BOOL_VAL = 329,                /* BOOL_VAL  */
    CHAR_VAL = 330                 /* CHAR_VAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define T_INT 258
#define T_FLOAT 259
#define T_BOOL 260
#define T_DOUBLE 261
#define T_LONG 262
#define T_FORALL 263
#define T_FOR 264
#define T_P_INF 265
#define T_INF 266
#define T_N_INF 267
#define T_FUNC 268
#define T_IF 269
#define T_ELSE 270
#define T_WHILE 271
#define T_RETURN 272
#define T_DO 273
#define T_IN 274
#define T_FIXEDPOINT 275
#define T_UNTIL 276
#define T_FILTER 277
#define T_ADD_ASSIGN 278
#define T_SUB_ASSIGN 279
#define T_MUL_ASSIGN 280
#define T_DIV_ASSIGN 281
#define T_MOD_ASSIGN 282
#define T_AND_ASSIGN 283
#define T_XOR_ASSIGN 284
#define T_OR_ASSIGN 285
#define T_INC_OP 286
#define T_DEC_OP 287
#define T_PTR_OP 288
#define T_AND_OP 289
#define T_OR_OP 290
#define T_LE_OP 291
#define T_GE_OP 292
#define T_EQ_OP 293
#define T_NE_OP 294
#define T_AND 295
#define T_OR 296
#define T_SUM 297
#define T_AVG 298
#define T_COUNT 299
#define T_PRODUCT 300
#define T_MAX 301
#define T_MIN 302
#define T_GRAPH 303
#define T_DIR_GRAPH 304
#define T_NODE 305
#define T_EDGE 306
#define T_UPDATES 307
#define T_CONTAINER 308
#define T_NODEMAP 309
#define T_VECTOR 310
#define T_NP 311
#define T_EP 312
#define T_LIST 313
#define T_SET_NODES 314
#define T_SET_EDGES 315
#define T_FROM 316
#define T_BFS 317
#define T_REVERSE 318
#define T_INCREMENTAL 319
#define T_DECREMENTAL 320
#define T_STATIC 321
#define T_DYNAMIC 322
#define T_BATCH 323
#define T_ONADD 324
#define T_ONDELETE 325
#define ID 326
#define INT_NUM 327
#define FLOAT_NUM 328
#define BOOL_VAL 329
#define CHAR_VAL 330

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 39 "lrparser.y"

    int  info;
    long ival;
	bool bval;
    double fval;
    char* text;
	char cval;
	ASTNode* node;
	paramList* pList;
	argList* aList;
	ASTNodeList* nodeList;
    tempNode* temporary;
     

#line 323 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_T_INT = 3,                      /* T_INT  */
  YYSYMBOL_T_FLOAT = 4,                    /* T_FLOAT  */
  YYSYMBOL_T_BOOL = 5,                     /* T_BOOL  */
  YYSYMBOL_T_DOUBLE = 6,                   /* T_DOUBLE  */
  YYSYMBOL_T_LONG = 7,                     /* T_LONG  */
  YYSYMBOL_T_FORALL = 8,                   /* T_FORALL  */
  YYSYMBOL_T_FOR = 9,                      /* T_FOR  */
  YYSYMBOL_T_P_INF = 10,                   /* T_P_INF  */
  YYSYMBOL_T_INF = 11,                     /* T_INF  */
  YYSYMBOL_T_N_INF = 12,                   /* T_N_INF  */
  YYSYMBOL_T_FUNC = 13,                    /* T_FUNC  */
  YYSYMBOL_T_IF = 14,                      /* T_IF  */
  YYSYMBOL_T_ELSE = 15,                    /* T_ELSE  */
  YYSYMBOL_T_WHILE = 16,                   /* T_WHILE  */
  YYSYMBOL_T_RETURN = 17,                  /* T_RETURN  */
  YYSYMBOL_T_DO = 18,                      /* T_DO  */
  YYSYMBOL_T_IN = 19,                      /* T_IN  */
  YYSYMBOL_T_FIXEDPOINT = 20,              /* T_FIXEDPOINT  */
  YYSYMBOL_T_UNTIL = 21,                   /* T_UNTIL  */
  YYSYMBOL_T_FILTER = 22,                  /* T_FILTER  */
  YYSYMBOL_T_ADD_ASSIGN = 23,              /* T_ADD_ASSIGN  */
  YYSYMBOL_T_SUB_ASSIGN = 24,              /* T_SUB_ASSIGN  */
  YYSYMBOL_T_MUL_ASSIGN = 25,              /* T_MUL_ASSIGN  */
  YYSYMBOL_T_DIV_ASSIGN = 26,              /* T_DIV_ASSIGN  */
  YYSYMBOL_T_MOD_ASSIGN = 27,              /* T_MOD_ASSIGN  */
  YYSYMBOL_T_AND_ASSIGN = 28,              /* T_AND_ASSIGN  */
  YYSYMBOL_T_XOR_ASSIGN = 29,              /* T_XOR_ASSIGN  */
  YYSYMBOL_T_OR_ASSIGN = 30,               /* T_OR_ASSIGN  */
  YYSYMBOL_T_INC_OP = 31,                  /* T_INC_OP  */
  YYSYMBOL_T_DEC_OP = 32,                  /* T_DEC_OP  */
  YYSYMBOL_T_PTR_OP = 33,                  /* T_PTR_OP  */
  YYSYMBOL_T_AND_OP = 34,                  /* T_AND_OP  */
  YYSYMBOL_T_OR_OP = 35,                   /* T_OR_OP  */
  YYSYMBOL_T_LE_OP = 36,                   /* T_LE_OP  */
  YYSYMBOL_T_GE_OP = 37,                   /* T_GE_OP  */
  YYSYMBOL_T_EQ_OP = 38,                   /* T_EQ_OP  */
  YYSYMBOL_T_NE_OP = 39,                   /* T_NE_OP  */
  YYSYMBOL_T_AND = 40,                     /* T_AND  */
  YYSYMBOL_T_OR = 41,                      /* T_OR  */
  YYSYMBOL_T_SUM = 42,                     /* T_SUM  */
  YYSYMBOL_T_AVG = 43,                     /* T_AVG  */
  YYSYMBOL_T_COUNT = 44,                   /* T_COUNT  */
  YYSYMBOL_T_PRODUCT = 45,                 /* T_PRODUCT  */
  YYSYMBOL_T_MAX = 46,                     /* T_MAX  */
  YYSYMBOL_T_MIN = 47,                     /* T_MIN  */
  YYSYMBOL_T_GRAPH = 48,                   /* T_GRAPH  */
  YYSYMBOL_T_DIR_GRAPH = 49,               /* T_DIR_GRAPH  */
  YYSYMBOL_T_NODE = 50,                    /* T_NODE  */
  YYSYMBOL_T_EDGE = 51,                    /* T_EDGE  */
  YYSYMBOL_T_UPDATES = 52,                 /* T_UPDATES  */
  YYSYMBOL_T_CONTAINER = 53,               /* T_CONTAINER  */
  YYSYMBOL_T_NODEMAP = 54,                 /* T_NODEMAP  */
  YYSYMBOL_T_VECTOR = 55,                  /* T_VECTOR  */
  YYSYMBOL_T_NP = 56,                      /* T_NP  */
  YYSYMBOL_T_EP = 57,                      /* T_EP  */
  YYSYMBOL_T_LIST = 58,                    /* T_LIST  */
  YYSYMBOL_T_SET_NODES = 59,               /* T_SET_NODES  */
  YYSYMBOL_T_SET_EDGES = 60,               /* T_SET_EDGES  */
  YYSYMBOL_T_FROM = 61,                    /* T_FROM  */
  YYSYMBOL_T_BFS = 62,                     /* T_BFS  */
  YYSYMBOL_T_REVERSE = 63,                 /* T_REVERSE  */
  YYSYMBOL_T_INCREMENTAL = 64,             /* T_INCREMENTAL  */
  YYSYMBOL_T_DECREMENTAL = 65,             /* T_DECREMENTAL  */
  YYSYMBOL_T_STATIC = 66,                  /* T_STATIC  */
  YYSYMBOL_T_DYNAMIC = 67,                 /* T_DYNAMIC  */
  YYSYMBOL_T_BATCH = 68,                   /* T_BATCH  */
  YYSYMBOL_T_ONADD = 69,                   /* T_ONADD  */
  YYSYMBOL_T_ONDELETE = 70,                /* T_ONDELETE  */
  YYSYMBOL_ID = 71,                        /* ID  */
  YYSYMBOL_INT_NUM = 72,                   /* INT_NUM  */
  YYSYMBOL_FLOAT_NUM = 73,                 /* FLOAT_NUM  */
  YYSYMBOL_BOOL_VAL = 74,                  /* BOOL_VAL  */
  YYSYMBOL_CHAR_VAL = 75,                  /* CHAR_VAL  */
  YYSYMBOL_76_ = 76,                       /* '?'  */
  YYSYMBOL_77_ = 77,                       /* ':'  */
  YYSYMBOL_78_ = 78,                       /* '<'  */
  YYSYMBOL_79_ = 79,                       /* '>'  */
  YYSYMBOL_80_ = 80,                       /* '+'  */
  YYSYMBOL_81_ = 81,                       /* '-'  */
  YYSYMBOL_82_ = 82,                       /* '*'  */
  YYSYMBOL_83_ = 83,                       /* '/'  */
  YYSYMBOL_84_ = 84,                       /* '%'  */
  YYSYMBOL_85_ = 85,                       /* '('  */
  YYSYMBOL_86_ = 86,                       /* ')'  */
  YYSYMBOL_87_ = 87,                       /* ','  */
  YYSYMBOL_88_ = 88,                       /* ';'  */
  YYSYMBOL_89_ = 89,                       /* '.'  */
  YYSYMBOL_90_ = 90,                       /* '{'  */
  YYSYMBOL_91_ = 91,                       /* '}'  */
  YYSYMBOL_92_ = 92,                       /* '='  */
  YYSYMBOL_93_ = 93,                       /* '!'  */
  YYSYMBOL_94_ = 94,                       /* '['  */
  YYSYMBOL_95_ = 95,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 96,                  /* $accept  */
  YYSYMBOL_program = 97,                   /* program  */
  YYSYMBOL_function_def = 98,              /* function_def  */
  YYSYMBOL_function_data = 99,             /* function_data  */
  YYSYMBOL_paramList = 100,                /* paramList  */
  YYSYMBOL_type = 101,                     /* type  */
  YYSYMBOL_param = 102,                    /* param  */
  YYSYMBOL_function_body = 103,            /* function_body  */
  YYSYMBOL_statements = 104,               /* statements  */
  YYSYMBOL_statement = 105,                /* statement  */
  YYSYMBOL_blockstatements = 106,          /* blockstatements  */
  YYSYMBOL_batch_blockstmt = 107,          /* batch_blockstmt  */
  YYSYMBOL_on_add_blockstmt = 108,         /* on_add_blockstmt  */
  YYSYMBOL_on_delete_blockstmt = 109,      /* on_delete_blockstmt  */
  YYSYMBOL_block_begin = 110,              /* block_begin  */
  YYSYMBOL_block_end = 111,                /* block_end  */
  YYSYMBOL_return_stmt = 112,              /* return_stmt  */
  YYSYMBOL_declaration = 113,              /* declaration  */
  YYSYMBOL_type1 = 114,                    /* type1  */
  YYSYMBOL_primitive = 115,                /* primitive  */
  YYSYMBOL_graph = 116,                    /* graph  */
  YYSYMBOL_collections = 117,              /* collections  */
  YYSYMBOL_container = 118,                /* container  */
  YYSYMBOL_vector = 119,                   /* vector  */
  YYSYMBOL_nodemap = 120,                  /* nodemap  */
  YYSYMBOL_type2 = 121,                    /* type2  */
  YYSYMBOL_property = 122,                 /* property  */
  YYSYMBOL_assignment = 123,               /* assignment  */
  YYSYMBOL_rhs = 124,                      /* rhs  */
  YYSYMBOL_expression = 125,               /* expression  */
  YYSYMBOL_indexExpr = 126,                /* indexExpr  */
  YYSYMBOL_unary_expr = 127,               /* unary_expr  */
  YYSYMBOL_proc_call = 128,                /* proc_call  */
  YYSYMBOL_val = 129,                      /* val  */
  YYSYMBOL_control_flow = 130,             /* control_flow  */
  YYSYMBOL_iteration_cf = 131,             /* iteration_cf  */
  YYSYMBOL_filterExpr = 132,               /* filterExpr  */
  YYSYMBOL_boolean_expr = 133,             /* boolean_expr  */
  YYSYMBOL_selection_cf = 134,             /* selection_cf  */
  YYSYMBOL_reduction = 135,                /* reduction  */
  YYSYMBOL_reduce_op = 136,                /* reduce_op  */
  YYSYMBOL_leftList = 137,                 /* leftList  */
  YYSYMBOL_rightList = 138,                /* rightList  */
  YYSYMBOL_reductionCall = 139,            /* reductionCall  */
  YYSYMBOL_reduction_calls = 140,          /* reduction_calls  */
  YYSYMBOL_leftSide = 141,                 /* leftSide  */
  YYSYMBOL_arg_list = 142,                 /* arg_list  */
  YYSYMBOL_bfs_abstraction = 143,          /* bfs_abstraction  */
  YYSYMBOL_reverse_abstraction = 144,      /* reverse_abstraction  */
  YYSYMBOL_oid = 145,                      /* oid  */
  YYSYMBOL_tid = 146,                      /* tid  */
  YYSYMBOL_id = 147                        /* id  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   870

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  164
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  387

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   330


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    93,     2,     2,     2,    84,     2,     2,
      85,    86,    82,    80,    87,    81,    89,    83,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    77,    88,
      78,    92,    79,    76,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    94,     2,    95,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    90,     2,    91,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   105,   105,   106,   108,   115,   121,   127,   133,   139,
     146,   147,   150,   151,   153,   164,   168,   177,   180,   181,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   197,   199,   201,   203,   205,   207,   209,   212,
     220,   223,   226,   230,   231,   232,   235,   236,   237,   238,
     239,   242,   243,   245,   246,   248,   250,   251,   252,   253,
     255,   256,   257,   260,   261,   262,   264,   266,   267,   268,
     270,   271,   272,   273,   274,   276,   279,   280,   283,   285,
     286,   287,   288,   289,   290,   291,   292,   293,   294,   295,
     296,   297,   298,   299,   302,   303,   304,   305,   307,   309,
     310,   312,   317,   321,   325,   336,   337,   338,   339,   340,
     341,   344,   345,   347,   348,   349,   350,   352,   353,   354,
     355,   356,   358,   359,   361,   363,   364,   367,   368,   370,
     371,   374,   375,   376,   377,   378,   380,   382,   384,   385,
     387,   388,   397,   399,   400,   401,   402,   403,   405,   406,
     407,   408,   411,   415,   432,   439,   444,   452,   453,   457,
     458,   461,   465,   472,   476
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "T_INT", "T_FLOAT",
  "T_BOOL", "T_DOUBLE", "T_LONG", "T_FORALL", "T_FOR", "T_P_INF", "T_INF",
  "T_N_INF", "T_FUNC", "T_IF", "T_ELSE", "T_WHILE", "T_RETURN", "T_DO",
  "T_IN", "T_FIXEDPOINT", "T_UNTIL", "T_FILTER", "T_ADD_ASSIGN",
  "T_SUB_ASSIGN", "T_MUL_ASSIGN", "T_DIV_ASSIGN", "T_MOD_ASSIGN",
  "T_AND_ASSIGN", "T_XOR_ASSIGN", "T_OR_ASSIGN", "T_INC_OP", "T_DEC_OP",
  "T_PTR_OP", "T_AND_OP", "T_OR_OP", "T_LE_OP", "T_GE_OP", "T_EQ_OP",
  "T_NE_OP", "T_AND", "T_OR", "T_SUM", "T_AVG", "T_COUNT", "T_PRODUCT",
  "T_MAX", "T_MIN", "T_GRAPH", "T_DIR_GRAPH", "T_NODE", "T_EDGE",
  "T_UPDATES", "T_CONTAINER", "T_NODEMAP", "T_VECTOR", "T_NP", "T_EP",
  "T_LIST", "T_SET_NODES", "T_SET_EDGES", "T_FROM", "T_BFS", "T_REVERSE",
  "T_INCREMENTAL", "T_DECREMENTAL", "T_STATIC", "T_DYNAMIC", "T_BATCH",
  "T_ONADD", "T_ONDELETE", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL",
  "CHAR_VAL", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'('", "')'", "','", "';'", "'.'", "'{'", "'}'", "'='", "'!'",
  "'['", "']'", "$accept", "program", "function_def", "function_data",
  "paramList", "type", "param", "function_body", "statements", "statement",
  "blockstatements", "batch_blockstmt", "on_add_blockstmt",
  "on_delete_blockstmt", "block_begin", "block_end", "return_stmt",
  "declaration", "type1", "primitive", "graph", "collections", "container",
  "vector", "nodemap", "type2", "property", "assignment", "rhs",
  "expression", "indexExpr", "unary_expr", "proc_call", "val",
  "control_flow", "iteration_cf", "filterExpr", "boolean_expr",
  "selection_cf", "reduction", "reduce_op", "leftList", "rightList",
  "reductionCall", "reduction_calls", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-328)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-164)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -328,    26,  -328,    24,   -22,    31,    24,    24,  -328,    36,
    -328,    46,   752,   752,    50,    59,  -328,  -328,  -328,  -328,
     752,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
      81,    91,    85,    93,   100,   101,  -328,   105,   106,    86,
      99,    24,  -328,  -328,  -328,  -328,  -328,  -328,    24,  -328,
     108,   752,   752,   442,   109,    24,   752,   752,   752,   788,
     810,    24,    24,  -328,   752,  -328,   107,  -328,   112,   113,
     115,   117,  -328,  -328,  -328,   118,   119,     3,    36,   170,
     122,   128,   129,   134,   135,   136,  -328,  -328,  -328,     3,
       3,  -328,     3,  -328,  -328,  -328,  -328,  -328,  -328,   137,
     138,    24,    24,   139,   607,   -67,   141,   149,  -328,  -328,
    -328,  -328,   150,    12,  -328,  -328,  -328,   133,  -328,   144,
     160,  -328,  -328,   154,   169,   171,   172,   176,   178,   179,
     180,   181,   183,  -328,    24,  -328,  -328,    24,    24,     3,
       3,   696,   -56,  -328,  -328,   164,   185,   187,    24,     3,
       3,    24,    24,    24,   696,   186,     2,   588,   696,  -328,
    -328,   182,   190,  -328,  -328,  -328,  -328,  -328,  -328,  -328,
    -328,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,  -328,  -328,  -328,
       3,   103,     3,    24,  -328,   193,  -328,   194,  -328,  -328,
    -328,  -328,  -328,  -328,  -328,  -328,   189,   265,   270,   696,
     204,   205,   209,    24,   276,   210,   616,   -58,   213,   215,
     219,   285,   288,   216,     3,  -328,     3,     3,   705,   492,
      29,    29,   338,   338,    29,    29,    39,    39,    -8,    -8,
     328,   696,   224,  -328,   696,   225,  -328,  -328,  -328,  -328,
    -328,  -328,  -328,   228,   696,   -77,     3,     3,  -328,     3,
       3,   543,    36,     3,   233,    24,     3,     3,     3,  -328,
    -328,     3,    24,    24,   237,  -328,  -328,  -328,  -328,     3,
    -328,     3,    24,    24,   -29,   -14,    51,    -7,   229,   104,
      16,   230,   306,  -328,   238,     3,   234,  -328,  -328,   626,
     239,   244,   116,   241,   248,  -328,   243,  -328,   752,  -328,
     752,    36,    36,     3,    36,    36,     3,   543,   251,   687,
       3,    36,     3,     3,   249,   157,  -328,  -328,   254,   255,
    -328,  -328,    14,   -38,  -328,  -328,    14,  -328,  -328,    36,
     281,  -328,   257,   259,     3,  -328,  -328,   324,   263,    24,
     275,  -328,    24,   274,   295,   286,   298,    42,   293,    36,
      57,    36,   307,    36,    36,     3,  -328,     3,     3,  -328,
    -328,   308,  -328,  -328,  -328,  -328,   309,    36,  -328,   333,
     -36,  -328,     3,  -328,   315,    36,  -328
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     164,     0,     0,     0,     0,     0,    36,     4,    17,    18,
       0,    46,    47,    48,    49,    50,    51,    52,    67,    68,
       0,     0,     0,     0,     0,     0,    53,     0,     0,     0,
      10,     0,    43,    44,    45,    57,    58,    59,     0,    69,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     7,     0,    14,    15,     8,     0,     0,
       0,     0,   109,   108,   110,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   105,   106,   107,     0,
       0,    37,     0,    19,    26,    29,    30,    31,    32,     0,
       0,     0,     0,     0,     0,    97,    96,    79,    94,    23,
     112,   111,     0,    95,    25,   149,   150,   148,     5,     0,
       0,    12,    13,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    11,     0,     6,     9,     0,     0,     0,
       0,    38,    97,    96,    79,    95,     0,     0,     0,   152,
     152,     0,     0,     0,     0,     0,    95,     0,    92,    28,
      20,    39,    41,    21,   131,   135,   132,   134,   133,    99,
     100,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    27,    22,    24,
     152,     0,     0,     0,    56,    62,    66,    65,    74,    75,
      70,    72,    71,    73,    54,    55,     0,     0,     0,   124,
       0,     0,     0,     0,     0,   156,   155,    95,     0,     0,
       0,     0,     0,     0,     0,    93,     0,     0,    84,    85,
      86,    87,    90,    91,    88,    89,    80,    81,    82,    83,
       0,   130,    95,    77,    78,     0,   143,   144,   145,   146,
     147,    76,   127,     0,   129,   161,   152,   152,    16,     0,
       0,     0,     0,     0,     0,     0,   152,   152,     0,   102,
     103,     0,     0,     0,     0,   136,    40,    42,    98,   152,
     101,   152,     0,     0,     0,     0,    97,    95,   148,    97,
      95,   148,   125,   114,     0,     0,     0,   153,   154,     0,
       0,     0,     0,     0,     0,   163,     0,    61,     0,    64,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   101,   142,   162,     0,     0,
     121,   117,    79,   148,   120,   118,    79,   126,   115,     0,
      79,    33,    79,    79,     0,    60,    63,     0,     0,     0,
       0,   113,     0,     0,     0,    94,     0,    95,     0,     0,
     161,     0,     0,     0,     0,     0,   128,     0,     0,   116,
     119,   122,    34,    35,   138,   139,     0,     0,   123,   158,
       0,   157,     0,   159,     0,     0,   160
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -328,  -328,  -328,  -328,    30,   -27,  -328,  -328,  -328,  -253,
      -9,  -328,  -328,  -328,  -328,  -328,  -328,  -328,   -37,    60,
    -328,    94,  -328,  -328,  -328,    27,  -328,   -50,  -180,   208,
     -25,   -49,     9,  -312,  -328,  -328,  -327,  -135,  -328,  -328,
     289,   188,  -201,   102,  -328,   -51,  -149,  -328,  -328,  -328,
    -328,     4
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     8,     9,    39,   120,    40,    17,    53,    93,
      94,    95,    96,    97,    19,    98,    99,   100,    41,    42,
      43,    44,    45,    46,    47,    48,    49,   215,   251,   154,
     142,   143,   144,   108,   109,   110,   348,   210,   111,   112,
     184,   155,   356,   252,   253,   145,   218,   114,   381,   115,
     116,   117
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,   219,   113,   103,   106,   211,   243,    11,   292,   350,
      14,    15,   282,    72,    73,    74,   101,   283,  -151,   121,
     121,   121,   185,   169,   170,   186,     2,   190,   105,  -151,
     123,   124,   355,   185,   268,   164,   165,   166,   156,     3,
     167,   245,   168,    50,   377,    65,   276,   277,  -161,   382,
      54,   349,    66,   355,    16,   355,   283,   307,   308,   119,
     169,   170,   107,    12,   337,   131,   132,    81,    82,   146,
     169,   170,   309,   310,    10,    86,    87,    88,   190,   312,
     102,    68,    69,   122,   122,   122,   183,   190,    90,   224,
       4,     5,     6,     7,   133,    10,    92,   190,   217,   217,
    -122,   190,   315,   347,   191,   161,   162,   284,   285,   179,
     180,   181,   182,    72,    73,    74,    13,   297,   298,   127,
     129,   181,   182,   183,   105,   105,    16,   190,   294,   367,
     303,    20,   304,   183,   242,    51,  -151,   311,   206,   217,
     185,   207,   208,  -163,    52,   246,   282,   247,   248,   249,
     250,   283,   214,   128,   130,   220,   221,   222,   246,    55,
     247,   248,   249,   250,   374,   105,   375,    81,    82,    56,
      57,    58,    63,   156,    10,    86,    87,    88,    59,    60,
    -104,  -104,  -104,    61,    62,  -104,    64,  -104,    90,  -151,
     314,   147,   134,   185,    67,   118,    92,   255,   135,   136,
     137,   212,   138,   139,   140,   217,   217,   148,   287,   290,
     113,   103,   106,   149,   150,   217,   217,   264,  -104,   151,
     152,   153,   193,   194,   101,   159,   160,   163,   217,   187,
     217,   105,   105,   376,   286,   289,   105,   188,   189,   195,
     196,   105,   105,  -104,  -104,  -104,  -104,   384,   197,   190,
     198,   199,  -104,   293,   105,   200,   105,   201,   202,   203,
     204,   104,   205,   288,   291,   223,   113,   103,   106,   296,
     107,   121,   213,   121,   226,   258,   300,   301,   256,   257,
     101,   328,   227,   329,   259,   141,   305,   306,   102,   260,
     261,   262,   105,   357,   263,   265,   271,   266,   157,   269,
     158,   270,   330,   331,   272,   334,   335,   273,   274,   279,
     295,   280,   341,   281,   357,   302,   357,   333,   313,   316,
     333,   317,   332,   320,   318,   336,   107,   325,   322,   340,
     351,   342,   343,   323,   326,   122,   344,   122,   327,   338,
     345,   346,   352,   353,   102,   354,   358,   209,   209,   359,
     369,   363,   370,   360,   372,   373,   362,   216,   216,   169,
     170,   361,   171,   172,   173,   174,   175,   176,   379,   169,
     170,   383,   364,   365,   173,   174,   386,   366,   368,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,   238,
     239,   240,   241,   371,   244,   378,   380,   347,   216,   244,
     254,   385,   192,     0,   324,     0,   177,   178,   179,   180,
     181,   182,   275,     0,     0,     0,   177,   178,   179,   180,
     181,   182,   183,   278,     0,     0,     0,     0,     0,     0,
       0,     0,   183,     0,   244,   244,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,    23,    24,    25,
      70,    71,    72,    73,    74,     0,    75,     0,    76,    77,
      78,     0,    79,     0,   216,   216,     0,     0,     0,   104,
       0,   209,     0,     0,   216,   216,   244,     0,     0,   299,
       0,     0,     0,     0,     0,     0,     0,   216,     0,   216,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,   319,    80,     0,    81,    82,     0,     0,
      83,    84,    85,    10,    86,    87,    88,     0,     0,     0,
      89,     0,     0,   169,   170,   104,   171,    90,   173,   174,
     175,   176,    16,    91,     0,    92,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    21,    22,    23,    24,
      25,    70,    71,    72,    73,    74,     0,    75,     0,    76,
      77,    78,     0,    79,     0,     0,     0,     0,     0,     0,
     177,   178,   179,   180,   181,   182,   209,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   183,     0,     0,     0,
     209,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,     0,    80,     0,    81,    82,     0,
       0,    83,    84,    85,    10,    86,    87,    88,     0,   169,
     170,    89,   171,   172,   173,   174,   175,   176,    90,     0,
     164,   165,   166,    16,     0,   167,    92,   168,   169,   170,
       0,   171,   172,   173,   174,   175,   176,   169,   170,     0,
     171,   172,   173,   174,   175,   176,     0,   169,   170,     0,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,     0,     0,   225,     0,     0,     0,     0,     0,
       0,     0,   183,     0,     0,   177,   178,   179,   180,   181,
     182,     0,     0,     0,   177,   178,   179,   180,   181,   182,
       0,   183,     0,   267,   177,   178,   179,   180,   181,   182,
     183,     0,   321,     0,     0,     0,     0,     0,   169,   170,
     183,   171,   172,   173,   174,   175,   176,   169,   170,     0,
     171,   172,   173,   174,   175,   176,   169,   170,     0,     0,
       0,   173,   174,   175,   176,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    21,    22,    23,    24,    25,
       0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,     0,     0,   339,   177,   178,   179,   180,   181,   182,
       0,   183,     0,   177,   178,   179,   180,   181,   182,     0,
     183,    21,    22,    23,    24,    25,     0,     0,     0,   183,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   125,   126,
      30,    31,    32,    33,     0,     0,    36,    37,    38,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,    32,    33,     0,     0,    36,    37,
      38
};

static const yytype_int16 yycheck[] =
{
       9,   150,    53,    53,    53,   140,   186,     3,   261,   336,
       6,     7,    89,    10,    11,    12,    53,    94,    85,    56,
      57,    58,    89,    31,    32,    92,     0,    85,    53,    85,
      57,    58,   344,    89,    92,    23,    24,    25,    89,    13,
      28,   190,    30,    13,   371,    41,   226,   227,    86,    85,
      20,    89,    48,   365,    90,   367,    94,    86,    87,    55,
      31,    32,    53,    85,   317,    61,    62,    64,    65,    78,
      31,    32,    86,    87,    71,    72,    73,    74,    85,    86,
      53,    51,    52,    56,    57,    58,    94,    85,    85,    87,
      64,    65,    66,    67,    64,    71,    93,    85,   149,   150,
      86,    85,    86,    89,    92,   101,   102,   256,   257,    80,
      81,    82,    83,    10,    11,    12,    85,   266,   267,    59,
      60,    82,    83,    94,   149,   150,    90,    85,   263,    87,
     279,    85,   281,    94,   185,    85,    85,    86,   134,   190,
      89,   137,   138,    86,    85,    42,    89,    44,    45,    46,
      47,    94,   148,    59,    60,   151,   152,   153,    42,    78,
      44,    45,    46,    47,   365,   190,   367,    64,    65,    78,
      85,    78,    86,   224,    71,    72,    73,    74,    78,    78,
      23,    24,    25,    78,    78,    28,    87,    30,    85,    85,
      86,    21,    85,    89,    86,    86,    93,   193,    86,    86,
      85,    16,    85,    85,    85,   256,   257,    85,   259,   260,
     261,   261,   261,    85,    85,   266,   267,   213,    61,    85,
      85,    85,    89,    79,   261,    88,    88,    88,   279,    88,
     281,   256,   257,   368,   259,   260,   261,    88,    88,    79,
      86,   266,   267,    86,    87,    88,    89,   382,    79,    85,
      79,    79,    95,   262,   279,    79,   281,    79,    79,    79,
      79,    53,    79,   259,   260,    79,   317,   317,   317,   265,
     261,   308,    85,   310,    92,    86,   272,   273,    85,    85,
     317,   308,    92,   310,    19,    77,   282,   283,   261,    19,
      86,    86,   317,   344,    85,    19,    77,    87,    90,    86,
      92,    86,   311,   312,    19,   314,   315,    19,    92,    85,
      77,    86,   321,    85,   365,    78,   367,   313,    89,    89,
     316,    15,   313,    89,    86,   316,   317,    86,    89,   320,
     339,   322,   323,    89,    86,   308,    87,   310,    95,    88,
      86,    86,    61,    86,   317,    86,    22,   139,   140,    86,
     359,    77,   361,   349,   363,   364,   352,   149,   150,    31,
      32,    86,    34,    35,    36,    37,    38,    39,   377,    31,
      32,   380,    77,    87,    36,    37,   385,    79,    85,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,    86,   186,    86,    63,    89,   190,   191,
     192,    86,   113,    -1,   302,    -1,    78,    79,    80,    81,
      82,    83,   224,    -1,    -1,    -1,    78,    79,    80,    81,
      82,    83,    94,    95,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,   226,   227,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    14,    -1,    16,    17,
      18,    -1,    20,    -1,   256,   257,    -1,    -1,    -1,   261,
      -1,   263,    -1,    -1,   266,   267,   268,    -1,    -1,   271,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   279,    -1,   281,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,   295,    62,    -1,    64,    65,    -1,    -1,
      68,    69,    70,    71,    72,    73,    74,    -1,    -1,    -1,
      78,    -1,    -1,    31,    32,   317,    34,    85,    36,    37,
      38,    39,    90,    91,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    -1,    14,    -1,    16,
      17,    18,    -1,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      78,    79,    80,    81,    82,    83,   368,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    94,    -1,    -1,    -1,
     382,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    -1,    62,    -1,    64,    65,    -1,
      -1,    68,    69,    70,    71,    72,    73,    74,    -1,    31,
      32,    78,    34,    35,    36,    37,    38,    39,    85,    -1,
      23,    24,    25,    90,    -1,    28,    93,    30,    31,    32,
      -1,    34,    35,    36,    37,    38,    39,    31,    32,    -1,
      34,    35,    36,    37,    38,    39,    -1,    31,    32,    -1,
      34,    35,    36,    37,    38,    39,    78,    79,    80,    81,
      82,    83,    -1,    -1,    86,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    94,    -1,    -1,    78,    79,    80,    81,    82,
      83,    -1,    -1,    -1,    78,    79,    80,    81,    82,    83,
      -1,    94,    -1,    87,    78,    79,    80,    81,    82,    83,
      94,    -1,    86,    -1,    -1,    -1,    -1,    -1,    31,    32,
      94,    34,    35,    36,    37,    38,    39,    31,    32,    -1,
      34,    35,    36,    37,    38,    39,    31,    32,    -1,    -1,
      -1,    36,    37,    38,    39,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81,    82,
      83,    -1,    -1,    86,    78,    79,    80,    81,    82,    83,
      -1,    94,    -1,    78,    79,    80,    81,    82,    83,    -1,
      94,     3,     4,     5,     6,     7,    -1,    -1,    -1,    94,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    -1,    -1,    58,    59,    60,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    52,    53,    54,    55,    -1,    -1,    58,    59,
      60
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    97,     0,    13,    64,    65,    66,    67,    98,    99,
      71,   147,    85,    85,   147,   147,    90,   103,   106,   110,
      85,     3,     4,     5,     6,     7,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,   100,
     102,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     100,    85,    85,   104,   100,    78,    78,    85,    78,    78,
      78,    78,    78,    86,    87,   147,   147,    86,   100,   100,
       8,     9,    10,    11,    12,    14,    16,    17,    18,    20,
      62,    64,    65,    68,    69,    70,    72,    73,    74,    78,
      85,    91,    93,   105,   106,   107,   108,   109,   111,   112,
     113,   114,   121,   123,   125,   126,   127,   128,   129,   130,
     131,   134,   135,   141,   143,   145,   146,   147,    86,   147,
     101,   114,   121,   101,   101,    50,    51,   115,   117,   115,
     117,   147,   147,   100,    85,    86,    86,    85,    85,    85,
      85,   125,   126,   127,   128,   141,   106,    21,    85,    85,
      85,    85,    85,    85,   125,   137,   141,   125,   125,    88,
      88,   147,   147,    88,    23,    24,    25,    28,    30,    31,
      32,    34,    35,    36,    37,    38,    39,    78,    79,    80,
      81,    82,    83,    94,   136,    89,    92,    88,    88,    88,
      85,    92,   136,    89,    79,    79,    86,    79,    79,    79,
      79,    79,    79,    79,    79,    79,   147,   147,   147,   125,
     133,   133,    16,    85,   147,   123,   125,   141,   142,   142,
     147,   147,   147,    79,    87,    86,    92,    92,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   141,   124,   125,   142,    42,    44,    45,    46,
      47,   124,   139,   140,   125,   147,    85,    85,    86,    19,
      19,    86,    86,    85,   147,    19,    87,    87,    92,    86,
      86,    77,    19,    19,    92,   137,   124,   124,    95,    85,
      86,    85,    89,    94,   142,   142,   126,   141,   147,   126,
     141,   147,   105,   106,   133,    77,   147,   142,   142,   125,
     147,   147,    78,   142,   142,   147,   147,    86,    87,    86,
      87,    86,    86,    89,    86,    86,    89,    15,    86,   125,
      89,    86,    89,    89,   139,    86,    86,    95,   101,   101,
     106,   106,   128,   147,   106,   106,   128,   105,    88,    86,
     128,   106,   128,   128,    87,    86,    86,    89,   132,    89,
     132,   106,    61,    86,    86,   129,   138,   141,    22,    86,
     147,    86,   147,    77,    77,    87,    79,    87,    85,   106,
     106,    86,   106,   106,   138,   138,   133,   132,    86,   106,
      63,   144,    85,   106,   133,    86,   106
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    96,    97,    97,    98,    99,    99,    99,    99,    99,
     100,   100,   101,   101,   102,   102,   102,   103,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     113,   113,   113,   114,   114,   114,   115,   115,   115,   115,
     115,   116,   116,   117,   117,   117,   117,   117,   117,   117,
     118,   118,   118,   119,   119,   119,   120,   121,   121,   121,
     122,   122,   122,   122,   122,   122,   123,   123,   124,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   126,   127,
     127,   128,   128,   128,   128,   129,   129,   129,   129,   129,
     129,   130,   130,   131,   131,   131,   131,   131,   131,   131,
     131,   131,   132,   132,   133,   134,   134,   135,   135,   135,
     135,   136,   136,   136,   136,   136,   137,   137,   138,   138,
     138,   138,   139,   140,   140,   140,   140,   140,   141,   141,
     141,   141,   142,   142,   142,   142,   142,   143,   143,   144,
     144,   145,   145,   146,   147
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     1,     1,     2,     2,     5,     1,     0,     2,
       2,     2,     2,     1,     2,     1,     1,     2,     2,     1,
       1,     1,     3,     7,    10,    10,     1,     1,     2,     2,
       4,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     4,     1,     1,     1,
       9,     7,     4,     9,     7,     4,     4,     1,     1,     1,
       4,     4,     4,     4,     4,     4,     3,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     3,     1,     1,     1,     1,     4,     2,
       2,     4,     4,     4,     6,     1,     1,     1,     1,     1,
       1,     1,     1,     8,     5,     7,    10,     7,     7,    10,
       7,     7,     0,     5,     1,     5,     7,     3,     9,     3,
       3,     1,     1,     1,     1,     1,     3,     1,     3,     3,
       1,     1,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     3,     1,     1,    13,    12,     2,
       5,     3,     6,     5,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3: /* program: program function_def  */
#line 106 "lrparser.y"
                               {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
#line 1813 "y.tab.c"
    break;

  case 4: /* function_def: function_data function_body  */
#line 108 "lrparser.y"
                                            { 
	                                          Function* func=(Function*)(yyvsp[-1].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[0].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
#line 1824 "y.tab.c"
    break;

  case 5: /* function_data: T_FUNC id '(' paramList ')'  */
#line 115 "lrparser.y"
                                           { 
										   (yyval.node)=Util::createFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
#line 1835 "y.tab.c"
    break;

  case 6: /* function_data: T_STATIC id '(' paramList ')'  */
#line 121 "lrparser.y"
                                                           { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1846 "y.tab.c"
    break;

  case 7: /* function_data: T_INCREMENTAL '(' paramList ')'  */
#line 127 "lrparser.y"
                                                     { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1857 "y.tab.c"
    break;

  case 8: /* function_data: T_DECREMENTAL '(' paramList ')'  */
#line 133 "lrparser.y"
                                                             { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1868 "y.tab.c"
    break;

  case 9: /* function_data: T_DYNAMIC id '(' paramList ')'  */
#line 139 "lrparser.y"
                                                        { (yyval.node)=Util::createDynamicFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
#line 1878 "y.tab.c"
    break;

  case 10: /* paramList: param  */
#line 146 "lrparser.y"
                 {(yyval.pList)=Util::createPList((yyvsp[0].node));}
#line 1884 "y.tab.c"
    break;

  case 11: /* paramList: param ',' paramList  */
#line 147 "lrparser.y"
                                     {(yyval.pList)=Util::addToPList((yyvsp[0].pList),(yyvsp[-2].node)); 
			                           }
#line 1891 "y.tab.c"
    break;

  case 12: /* type: type1  */
#line 150 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1897 "y.tab.c"
    break;

  case 13: /* type: type2  */
#line 151 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1903 "y.tab.c"
    break;

  case 14: /* param: type1 id  */
#line 153 "lrparser.y"
                 {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 1919 "y.tab.c"
    break;

  case 15: /* param: type2 id  */
#line 164 "lrparser.y"
                          { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 1928 "y.tab.c"
    break;

  case 16: /* param: type2 id '(' id ')'  */
#line 168 "lrparser.y"
                                                 { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[-4].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[-1].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[-3].node));
									 }
#line 1940 "y.tab.c"
    break;

  case 17: /* function_body: blockstatements  */
#line 177 "lrparser.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 1946 "y.tab.c"
    break;

  case 18: /* statements: %empty  */
#line 180 "lrparser.y"
              {}
#line 1952 "y.tab.c"
    break;

  case 19: /* statements: statements statement  */
#line 181 "lrparser.y"
                               {printf ("found one statement\n") ; Util::addToBlock((yyvsp[0].node)); }
#line 1958 "y.tab.c"
    break;

  case 20: /* statement: declaration ';'  */
#line 183 "lrparser.y"
                          {(yyval.node)=(yyvsp[-1].node);}
#line 1964 "y.tab.c"
    break;

  case 21: /* statement: assignment ';'  */
#line 184 "lrparser.y"
                       {printf ("found an assignment type statement" ); (yyval.node)=(yyvsp[-1].node);}
#line 1970 "y.tab.c"
    break;

  case 22: /* statement: proc_call ';'  */
#line 185 "lrparser.y"
                       {printf ("found an proc call type statement" );(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[-1].node));}
#line 1976 "y.tab.c"
    break;

  case 23: /* statement: control_flow  */
#line 186 "lrparser.y"
                      {printf ("found an control flow type statement" );(yyval.node)=(yyvsp[0].node);}
#line 1982 "y.tab.c"
    break;

  case 24: /* statement: reduction ';'  */
#line 187 "lrparser.y"
                      {printf ("found an reduction type statement" );(yyval.node)=(yyvsp[-1].node);}
#line 1988 "y.tab.c"
    break;

  case 25: /* statement: bfs_abstraction  */
#line 188 "lrparser.y"
                          {printf ("found bfs\n") ;(yyval.node)=(yyvsp[0].node); }
#line 1994 "y.tab.c"
    break;

  case 26: /* statement: blockstatements  */
#line 189 "lrparser.y"
                          {printf ("found block\n") ;(yyval.node)=(yyvsp[0].node);}
#line 2000 "y.tab.c"
    break;

  case 27: /* statement: unary_expr ';'  */
#line 190 "lrparser.y"
                         {printf ("found unary\n") ;(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[-1].node));}
#line 2006 "y.tab.c"
    break;

  case 28: /* statement: return_stmt ';'  */
#line 191 "lrparser.y"
                          {printf ("found return\n") ;(yyval.node) = (yyvsp[-1].node) ;}
#line 2012 "y.tab.c"
    break;

  case 29: /* statement: batch_blockstmt  */
#line 192 "lrparser.y"
                           {printf ("found batch\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2018 "y.tab.c"
    break;

  case 30: /* statement: on_add_blockstmt  */
#line 193 "lrparser.y"
                           {printf ("found on add block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2024 "y.tab.c"
    break;

  case 31: /* statement: on_delete_blockstmt  */
#line 194 "lrparser.y"
                              {printf ("found delete block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2030 "y.tab.c"
    break;

  case 32: /* blockstatements: block_begin statements block_end  */
#line 197 "lrparser.y"
                                                   { (yyval.node)=Util::finishBlock();}
#line 2036 "y.tab.c"
    break;

  case 33: /* batch_blockstmt: T_BATCH '(' id ':' expression ')' blockstatements  */
#line 199 "lrparser.y"
                                                                    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));}
#line 2042 "y.tab.c"
    break;

  case 34: /* on_add_blockstmt: T_ONADD '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 201 "lrparser.y"
                                                                                {(yyval.node) = Util::createOnAddBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2048 "y.tab.c"
    break;

  case 35: /* on_delete_blockstmt: T_ONDELETE '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 203 "lrparser.y"
                                                                                      {(yyval.node) = Util::createOnDeleteBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2054 "y.tab.c"
    break;

  case 36: /* block_begin: '{'  */
#line 205 "lrparser.y"
                { Util::createNewBlock(); }
#line 2060 "y.tab.c"
    break;

  case 38: /* return_stmt: T_RETURN expression  */
#line 209 "lrparser.y"
                                  {(yyval.node) = Util::createReturnStatementNode((yyvsp[0].node));}
#line 2066 "y.tab.c"
    break;

  case 39: /* declaration: type1 id  */
#line 212 "lrparser.y"
                         {
	                     Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 2079 "y.tab.c"
    break;

  case 40: /* declaration: type1 id '=' rhs  */
#line 220 "lrparser.y"
                            {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2087 "y.tab.c"
    break;

  case 41: /* declaration: type2 id  */
#line 223 "lrparser.y"
                    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 2095 "y.tab.c"
    break;

  case 42: /* declaration: type2 id '=' rhs  */
#line 226 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2103 "y.tab.c"
    break;

  case 43: /* type1: primitive  */
#line 230 "lrparser.y"
                 {(yyval.node)=(yyvsp[0].node); }
#line 2109 "y.tab.c"
    break;

  case 44: /* type1: graph  */
#line 231 "lrparser.y"
                {(yyval.node)=(yyvsp[0].node);}
#line 2115 "y.tab.c"
    break;

  case 45: /* type1: collections  */
#line 232 "lrparser.y"
                      { (yyval.node)=(yyvsp[0].node);}
#line 2121 "y.tab.c"
    break;

  case 46: /* primitive: T_INT  */
#line 235 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
#line 2127 "y.tab.c"
    break;

  case 47: /* primitive: T_FLOAT  */
#line 236 "lrparser.y"
                  { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
#line 2133 "y.tab.c"
    break;

  case 48: /* primitive: T_BOOL  */
#line 237 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
#line 2139 "y.tab.c"
    break;

  case 49: /* primitive: T_DOUBLE  */
#line 238 "lrparser.y"
                   { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 2145 "y.tab.c"
    break;

  case 50: /* primitive: T_LONG  */
#line 239 "lrparser.y"
             {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
#line 2151 "y.tab.c"
    break;

  case 51: /* graph: T_GRAPH  */
#line 242 "lrparser.y"
                { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 2157 "y.tab.c"
    break;

  case 52: /* graph: T_DIR_GRAPH  */
#line 243 "lrparser.y"
                     {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 2163 "y.tab.c"
    break;

  case 53: /* collections: T_LIST  */
#line 245 "lrparser.y"
                     { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
#line 2169 "y.tab.c"
    break;

  case 54: /* collections: T_SET_NODES '<' id '>'  */
#line 246 "lrparser.y"
                                         {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].node));}
#line 2176 "y.tab.c"
    break;

  case 55: /* collections: T_SET_EDGES '<' id '>'  */
#line 248 "lrparser.y"
                                 {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].node));}
#line 2183 "y.tab.c"
    break;

  case 56: /* collections: T_UPDATES '<' id '>'  */
#line 250 "lrparser.y"
                                         { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[-1].node));}
#line 2189 "y.tab.c"
    break;

  case 57: /* collections: container  */
#line 251 "lrparser.y"
                        {(yyval.node) = (yyvsp[0].node);}
#line 2195 "y.tab.c"
    break;

  case 58: /* collections: vector  */
#line 252 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node);}
#line 2201 "y.tab.c"
    break;

  case 59: /* collections: nodemap  */
#line 253 "lrparser.y"
                            {(yyval.node) = (yyvsp[0].node);}
#line 2207 "y.tab.c"
    break;

  case 60: /* container: T_CONTAINER '<' type '>' '(' arg_list ',' type ')'  */
#line 255 "lrparser.y"
                                                               {(yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2213 "y.tab.c"
    break;

  case 61: /* container: T_CONTAINER '<' type '>' '(' arg_list ')'  */
#line 256 "lrparser.y"
                                                      { (yyval.node) =  Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2219 "y.tab.c"
    break;

  case 62: /* container: T_CONTAINER '<' type '>'  */
#line 257 "lrparser.y"
                                     { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-1].node), argList, NULL);}
#line 2226 "y.tab.c"
    break;

  case 63: /* vector: T_VECTOR '<' type '>' '(' arg_list ',' type ')'  */
#line 260 "lrparser.y"
                                                       {(yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2232 "y.tab.c"
    break;

  case 64: /* vector: T_VECTOR '<' type '>' '(' arg_list ')'  */
#line 261 "lrparser.y"
                                                  { (yyval.node) =  Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2238 "y.tab.c"
    break;

  case 65: /* vector: T_VECTOR '<' type '>'  */
#line 262 "lrparser.y"
                                 { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_VECTOR, (yyvsp[-1].node), argList, NULL);}
#line 2245 "y.tab.c"
    break;

  case 66: /* nodemap: T_NODEMAP '(' type ')'  */
#line 264 "lrparser.y"
                                 {(yyval.node) = Util::createNodeMapTypeNode(TYPE_NODEMAP, (yyvsp[-1].node));}
#line 2251 "y.tab.c"
    break;

  case 67: /* type2: T_NODE  */
#line 266 "lrparser.y"
               {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
#line 2257 "y.tab.c"
    break;

  case 68: /* type2: T_EDGE  */
#line 267 "lrparser.y"
                {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
#line 2263 "y.tab.c"
    break;

  case 69: /* type2: property  */
#line 268 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 2269 "y.tab.c"
    break;

  case 70: /* property: T_NP '<' primitive '>'  */
#line 270 "lrparser.y"
                                  { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2275 "y.tab.c"
    break;

  case 71: /* property: T_EP '<' primitive '>'  */
#line 271 "lrparser.y"
                                       { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node)); }
#line 2281 "y.tab.c"
    break;

  case 72: /* property: T_NP '<' collections '>'  */
#line 272 "lrparser.y"
                                                    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2287 "y.tab.c"
    break;

  case 73: /* property: T_EP '<' collections '>'  */
#line 273 "lrparser.y"
                                                     {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node));}
#line 2293 "y.tab.c"
    break;

  case 74: /* property: T_NP '<' T_NODE '>'  */
#line 274 "lrparser.y"
                                    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_NODE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2300 "y.tab.c"
    break;

  case 75: /* property: T_NP '<' T_EDGE '>'  */
#line 276 "lrparser.y"
                                                {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_EDGE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2307 "y.tab.c"
    break;

  case 76: /* assignment: leftSide '=' rhs  */
#line 279 "lrparser.y"
                                { printf("testassign\n");(yyval.node)=Util::createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2313 "y.tab.c"
    break;

  case 77: /* assignment: indexExpr '=' rhs  */
#line 280 "lrparser.y"
                                  {printf ("called assign for count\n") ; (yyval.node)=Util::createAssignmentNode((yyvsp[-2].node) , (yyvsp[0].node));}
#line 2319 "y.tab.c"
    break;

  case 78: /* rhs: expression  */
#line 283 "lrparser.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 2325 "y.tab.c"
    break;

  case 79: /* expression: proc_call  */
#line 285 "lrparser.y"
                       { (yyval.node)=(yyvsp[0].node);}
#line 2331 "y.tab.c"
    break;

  case 80: /* expression: expression '+' expression  */
#line 286 "lrparser.y"
                                         { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 2337 "y.tab.c"
    break;

  case 81: /* expression: expression '-' expression  */
#line 287 "lrparser.y"
                                             { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 2343 "y.tab.c"
    break;

  case 82: /* expression: expression '*' expression  */
#line 288 "lrparser.y"
                                             {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 2349 "y.tab.c"
    break;

  case 83: /* expression: expression '/' expression  */
#line 289 "lrparser.y"
                                           {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 2355 "y.tab.c"
    break;

  case 84: /* expression: expression T_AND_OP expression  */
#line 290 "lrparser.y"
                                              {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 2361 "y.tab.c"
    break;

  case 85: /* expression: expression T_OR_OP expression  */
#line 291 "lrparser.y"
                                                  {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 2367 "y.tab.c"
    break;

  case 86: /* expression: expression T_LE_OP expression  */
#line 292 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 2373 "y.tab.c"
    break;

  case 87: /* expression: expression T_GE_OP expression  */
#line 293 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 2379 "y.tab.c"
    break;

  case 88: /* expression: expression '<' expression  */
#line 294 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 2385 "y.tab.c"
    break;

  case 89: /* expression: expression '>' expression  */
#line 295 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 2391 "y.tab.c"
    break;

  case 90: /* expression: expression T_EQ_OP expression  */
#line 296 "lrparser.y"
                                                        {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2397 "y.tab.c"
    break;

  case 91: /* expression: expression T_NE_OP expression  */
#line 297 "lrparser.y"
                                            {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2403 "y.tab.c"
    break;

  case 92: /* expression: '!' expression  */
#line 298 "lrparser.y"
                                         {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[0].node),OPERATOR_NOT);}
#line 2409 "y.tab.c"
    break;

  case 93: /* expression: '(' expression ')'  */
#line 299 "lrparser.y"
                                          { Expression* expr=(Expression*)(yyvsp[-1].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
#line 2417 "y.tab.c"
    break;

  case 94: /* expression: val  */
#line 302 "lrparser.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2423 "y.tab.c"
    break;

  case 95: /* expression: leftSide  */
#line 303 "lrparser.y"
                                    { (yyval.node)=Util::createNodeForId((yyvsp[0].node));}
#line 2429 "y.tab.c"
    break;

  case 96: /* expression: unary_expr  */
#line 304 "lrparser.y"
                                      {(yyval.node)=(yyvsp[0].node);}
#line 2435 "y.tab.c"
    break;

  case 97: /* expression: indexExpr  */
#line 305 "lrparser.y"
                                     {(yyval.node) = (yyvsp[0].node);}
#line 2441 "y.tab.c"
    break;

  case 98: /* indexExpr: expression '[' expression ']'  */
#line 307 "lrparser.y"
                                          {printf("first done this \n");(yyval.node) = Util::createNodeForIndexExpr((yyvsp[-3].node), (yyvsp[-1].node), OPERATOR_INDEX);}
#line 2447 "y.tab.c"
    break;

  case 99: /* unary_expr: expression T_INC_OP  */
#line 309 "lrparser.y"
                                   {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_INC);}
#line 2453 "y.tab.c"
    break;

  case 100: /* unary_expr: expression T_DEC_OP  */
#line 310 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_DEC);}
#line 2459 "y.tab.c"
    break;

  case 101: /* proc_call: leftSide '(' arg_list ')'  */
#line 312 "lrparser.y"
                                      { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)->AList,NULL); 

									    }
#line 2469 "y.tab.c"
    break;

  case 102: /* proc_call: T_INCREMENTAL '(' arg_list ')'  */
#line 317 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2478 "y.tab.c"
    break;

  case 103: /* proc_call: T_DECREMENTAL '(' arg_list ')'  */
#line 321 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2487 "y.tab.c"
    break;

  case 104: /* proc_call: indexExpr '.' leftSide '(' arg_list ')'  */
#line 325 "lrparser.y"
                                                                  {
                                                   
													 Expression* expr = (Expression*)(yyvsp[-5].node);
                                                     (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node) , (yyvsp[-1].aList)->AList, expr); 

									                 }
#line 2498 "y.tab.c"
    break;

  case 105: /* val: INT_NUM  */
#line 336 "lrparser.y"
              { (yyval.node) = Util::createNodeForIval((yyvsp[0].ival)); }
#line 2504 "y.tab.c"
    break;

  case 106: /* val: FLOAT_NUM  */
#line 337 "lrparser.y"
                    {(yyval.node) = Util::createNodeForFval((yyvsp[0].fval));}
#line 2510 "y.tab.c"
    break;

  case 107: /* val: BOOL_VAL  */
#line 338 "lrparser.y"
                   { (yyval.node) = Util::createNodeForBval((yyvsp[0].bval));}
#line 2516 "y.tab.c"
    break;

  case 108: /* val: T_INF  */
#line 339 "lrparser.y"
                {(yyval.node)=Util::createNodeForINF(true);}
#line 2522 "y.tab.c"
    break;

  case 109: /* val: T_P_INF  */
#line 340 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(true);}
#line 2528 "y.tab.c"
    break;

  case 110: /* val: T_N_INF  */
#line 341 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(false);}
#line 2534 "y.tab.c"
    break;

  case 111: /* control_flow: selection_cf  */
#line 344 "lrparser.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2540 "y.tab.c"
    break;

  case 112: /* control_flow: iteration_cf  */
#line 345 "lrparser.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2546 "y.tab.c"
    break;

  case 113: /* iteration_cf: T_FIXEDPOINT T_UNTIL '(' id ':' expression ')' blockstatements  */
#line 347 "lrparser.y"
                                                                              { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2552 "y.tab.c"
    break;

  case 114: /* iteration_cf: T_WHILE '(' boolean_expr ')' blockstatements  */
#line 348 "lrparser.y"
                                                                 {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2558 "y.tab.c"
    break;

  case 115: /* iteration_cf: T_DO blockstatements T_WHILE '(' boolean_expr ')' ';'  */
#line 349 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[-2].node),(yyvsp[-5].node));  }
#line 2564 "y.tab.c"
    break;

  case 116: /* iteration_cf: T_FORALL '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 350 "lrparser.y"
                                                                                       { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2571 "y.tab.c"
    break;

  case 117: /* iteration_cf: T_FORALL '(' id T_IN leftSide ')' blockstatements  */
#line 352 "lrparser.y"
                                                                        { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2577 "y.tab.c"
    break;

  case 118: /* iteration_cf: T_FOR '(' id T_IN leftSide ')' blockstatements  */
#line 353 "lrparser.y"
                                                                 { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2583 "y.tab.c"
    break;

  case 119: /* iteration_cf: T_FOR '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 354 "lrparser.y"
                                                                                    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2589 "y.tab.c"
    break;

  case 120: /* iteration_cf: T_FOR '(' id T_IN indexExpr ')' blockstatements  */
#line 355 "lrparser.y"
                                                                  {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), false);}
#line 2595 "y.tab.c"
    break;

  case 121: /* iteration_cf: T_FORALL '(' id T_IN indexExpr ')' blockstatements  */
#line 356 "lrparser.y"
                                                                     {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), true);}
#line 2601 "y.tab.c"
    break;

  case 122: /* filterExpr: %empty  */
#line 358 "lrparser.y"
                      { (yyval.node)=NULL;}
#line 2607 "y.tab.c"
    break;

  case 123: /* filterExpr: '.' T_FILTER '(' boolean_expr ')'  */
#line 359 "lrparser.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2613 "y.tab.c"
    break;

  case 124: /* boolean_expr: expression  */
#line 361 "lrparser.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2619 "y.tab.c"
    break;

  case 125: /* selection_cf: T_IF '(' boolean_expr ')' statement  */
#line 363 "lrparser.y"
                                                   { (yyval.node)=Util::createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2625 "y.tab.c"
    break;

  case 126: /* selection_cf: T_IF '(' boolean_expr ')' statement T_ELSE statement  */
#line 364 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2631 "y.tab.c"
    break;

  case 127: /* reduction: leftSide '=' reductionCall  */
#line 367 "lrparser.y"
                                       { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) ;}
#line 2637 "y.tab.c"
    break;

  case 128: /* reduction: '<' leftList '>' '=' '<' reductionCall ',' rightList '>'  */
#line 368 "lrparser.y"
                                                                              { reductionCall* reduc=(reductionCall*)(yyvsp[-3].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[-7].nodeList)->ASTNList,reduc,(yyvsp[-1].nodeList)->ASTNList);}
#line 2644 "y.tab.c"
    break;

  case 129: /* reduction: leftSide reduce_op expression  */
#line 370 "lrparser.y"
                                                   {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2650 "y.tab.c"
    break;

  case 130: /* reduction: expression reduce_op expression  */
#line 371 "lrparser.y"
                                         {printf ("here calling creation for red op\n") ;(yyval.node)=Util::createNodeForReductionOpStmt ((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2656 "y.tab.c"
    break;

  case 131: /* reduce_op: T_ADD_ASSIGN  */
#line 374 "lrparser.y"
                         {(yyval.ival)=OPERATOR_ADDASSIGN;}
#line 2662 "y.tab.c"
    break;

  case 132: /* reduce_op: T_MUL_ASSIGN  */
#line 375 "lrparser.y"
                         {(yyval.ival)=OPERATOR_MULASSIGN;}
#line 2668 "y.tab.c"
    break;

  case 133: /* reduce_op: T_OR_ASSIGN  */
#line 376 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ORASSIGN;}
#line 2674 "y.tab.c"
    break;

  case 134: /* reduce_op: T_AND_ASSIGN  */
#line 377 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ANDASSIGN;}
#line 2680 "y.tab.c"
    break;

  case 135: /* reduce_op: T_SUB_ASSIGN  */
#line 378 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_SUBASSIGN;}
#line 2686 "y.tab.c"
    break;

  case 136: /* leftList: leftSide ',' leftList  */
#line 380 "lrparser.y"
                                  { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));
                                         }
#line 2693 "y.tab.c"
    break;

  case 137: /* leftList: leftSide  */
#line 382 "lrparser.y"
                           { (yyval.nodeList)=Util::createNList((yyvsp[0].node));;}
#line 2699 "y.tab.c"
    break;

  case 138: /* rightList: val ',' rightList  */
#line 384 "lrparser.y"
                              { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));}
#line 2705 "y.tab.c"
    break;

  case 139: /* rightList: leftSide ',' rightList  */
#line 385 "lrparser.y"
                                   { ASTNode* node = Util::createNodeForId((yyvsp[-2].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),node);}
#line 2712 "y.tab.c"
    break;

  case 140: /* rightList: val  */
#line 387 "lrparser.y"
                   { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2718 "y.tab.c"
    break;

  case 141: /* rightList: leftSide  */
#line 388 "lrparser.y"
                              { ASTNode* node = Util::createNodeForId((yyvsp[0].node));
			            (yyval.nodeList)=Util::createNList(node);}
#line 2725 "y.tab.c"
    break;

  case 142: /* reductionCall: reduction_calls '(' arg_list ')'  */
#line 397 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList)->AList);}
#line 2731 "y.tab.c"
    break;

  case 143: /* reduction_calls: T_SUM  */
#line 399 "lrparser.y"
                        { (yyval.ival)=REDUCE_SUM;}
#line 2737 "y.tab.c"
    break;

  case 144: /* reduction_calls: T_COUNT  */
#line 400 "lrparser.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2743 "y.tab.c"
    break;

  case 145: /* reduction_calls: T_PRODUCT  */
#line 401 "lrparser.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2749 "y.tab.c"
    break;

  case 146: /* reduction_calls: T_MAX  */
#line 402 "lrparser.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2755 "y.tab.c"
    break;

  case 147: /* reduction_calls: T_MIN  */
#line 403 "lrparser.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2761 "y.tab.c"
    break;

  case 148: /* leftSide: id  */
#line 405 "lrparser.y"
              { (yyval.node)=(yyvsp[0].node); }
#line 2767 "y.tab.c"
    break;

  case 149: /* leftSide: oid  */
#line 406 "lrparser.y"
               { printf("Here hello \n"); (yyval.node)=(yyvsp[0].node); }
#line 2773 "y.tab.c"
    break;

  case 150: /* leftSide: tid  */
#line 407 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node); }
#line 2779 "y.tab.c"
    break;

  case 151: /* leftSide: indexExpr  */
#line 408 "lrparser.y"
                    {(yyval.node)=(yyvsp[0].node);}
#line 2785 "y.tab.c"
    break;

  case 152: /* arg_list: %empty  */
#line 411 "lrparser.y"
              {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
#line 2793 "y.tab.c"
    break;

  case 153: /* arg_list: assignment ',' arg_list  */
#line 415 "lrparser.y"
                                         {argument* a1=new argument();
		                          assignment* assign=(assignment*)(yyvsp[-2].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
		                 //a1->assignExpr=(assignment*)$1;
						 // a1->assign=true;
						  (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						  /*
						  for(argument* arg:$$->AList)
						  {
							  printf("VALUE OF ARG %d",arg->getAssignExpr()); //rm for warnings
						  }
						  */ 
						  
                          }
#line 2813 "y.tab.c"
    break;

  case 154: /* arg_list: expression ',' arg_list  */
#line 432 "lrparser.y"
                                             {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[-2].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						                }
#line 2825 "y.tab.c"
    break;

  case 155: /* arg_list: expression  */
#line 439 "lrparser.y"
                            {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[0].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
#line 2835 "y.tab.c"
    break;

  case 156: /* arg_list: assignment  */
#line 444 "lrparser.y"
                            { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[0].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
#line 2846 "y.tab.c"
    break;

  case 157: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements reverse_abstraction  */
#line 452 "lrparser.y"
                                                                                                                 {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-10].node),(yyvsp[-8].node),(yyvsp[-6].node),(yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2852 "y.tab.c"
    break;

  case 158: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements  */
#line 453 "lrparser.y"
                                                                                                      {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-9].node),(yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node),NULL) ; }
#line 2858 "y.tab.c"
    break;

  case 159: /* reverse_abstraction: T_REVERSE blockstatements  */
#line 457 "lrparser.y"
                                                 {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[0].node));}
#line 2864 "y.tab.c"
    break;

  case 160: /* reverse_abstraction: T_REVERSE '(' boolean_expr ')' blockstatements  */
#line 458 "lrparser.y"
                                                                       {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2870 "y.tab.c"
    break;

  case 161: /* oid: id '.' id  */
#line 461 "lrparser.y"
                 { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node) = Util::createPropIdNode((yyvsp[-2].node),(yyvsp[0].node));
				    }
#line 2879 "y.tab.c"
    break;

  case 162: /* oid: id '.' id '[' id ']'  */
#line 465 "lrparser.y"
                                { ASTNode* expr1 = Util::createNodeForId((yyvsp[-3].node));
	                          ASTNode* expr2 = Util::createNodeForId((yyvsp[-1].node));
							  ASTNode* indexexpr =  Util::createNodeForIndexExpr(expr1, expr2, OPERATOR_INDEX);
	                          (yyval.node) = Util::createPropIdNode((yyvsp[-5].node) , indexexpr);}
#line 2888 "y.tab.c"
    break;

  case 163: /* tid: id '.' id '.' id  */
#line 472 "lrparser.y"
                       {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-4].node),(yyvsp[-2].node));
				    }
#line 2897 "y.tab.c"
    break;

  case 164: /* id: ID  */
#line 476 "lrparser.y"
          { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[0].text));  

            
            }
#line 2907 "y.tab.c"
    break;


#line 2911 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 484 "lrparser.y"



void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(int argc,char **argv) 
{
  
  if(argc<4){
    std::cout<< "Usage: " << argv[0] << " [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc|multigpu|amd] " << '\n';
    std::cout<< "E.g. : " << argv[0] << " -s -f ../graphcode/sssp_dslV2 -b omp " << '\n';
    exit(-1);
  }
  
    //dsl_cpp_generator cpp_backend;
    SymbolTableBuilder stBuilder;
     FILE    *fd;
     
  int opt;
  char* fileName=NULL;
  backendTarget = NULL;
  bool staticGen = false;
  bool dynamicGen = false;
  bool optimize = false;

  while ((opt = getopt(argc, argv, "sdf:b:o")) != -1) 
  {
     switch (opt) 
     {
      case 'f':
        fileName = optarg;
        break;
      case 'b':
        backendTarget = optarg;
        break;
      case 's':
	    staticGen = true;
		break;
	  case 'd':
	    dynamicGen = true;
        break;	
	  case 'o':
	  	optimize = true;
		break;	
      case '?':
        fprintf(stderr,"Unknown option: %c\n", optopt);
		exit(-1);
        break;
      case ':':
        fprintf(stderr,"Missing arg for %c\n", optopt);
		exit(-1);
        break;
     }
  }
   
   printf("fileName %s\n",fileName);
   printf("Backend Target %s\n",backendTarget);

   
   if(fileName==NULL||backendTarget==NULL)
   {
	   if(fileName==NULL)
	      fprintf(stderr,"FileName option Error!\n");
	   if(backendTarget==NULL)
	      fprintf(stderr,"backendTarget option Error!\n")	;
	   exit(-1);	    
   }
   else
    {

		if(!((strcmp(backendTarget,"omp")==0)|| (strcmp(backendTarget,"amd")==0) || (strcmp(backendTarget,"mpi")==0)||(strcmp(backendTarget,"cuda")==0) || (strcmp(backendTarget,"acc")==0) || (strcmp(backendTarget,"sycl")==0)|| (strcmp(backendTarget,"multigpu")==0)))

		   {
			  fprintf(stderr, "Specified backend target is not implemented in the current version!\n");
			   exit(-1);
		   }
	}

   if(!(staticGen || dynamicGen)) {
		fprintf(stderr, "Type of graph(static/dynamic) not specified!\n");
		exit(-1);
     }
	  
     


   yyin= fopen(fileName,"r");
   
   if(!yyin) {
	printf("file doesn't exists!\n");
	exit(-1);
   }
   
   
   int error=yyparse();
   printf("error val %d\n",error);


	if(error!=1)
	{
     //TODO: redirect to different backend generator after comparing with the 'b' option
    std::cout << "at 1" << std::endl;
	stBuilder.buildST(frontEndContext.getFuncList());
	frontEndContext.setDynamicLinkFuncs(stBuilder.getDynamicLinkedFuncs());
	std::cout << "at 2" << std::endl;

	if(staticGen)
	  {
		  /*
		  if(optimize)
		  {
			  attachPropAnalyser apAnalyser;
			  apAnalyser.analyse(frontEndContext.getFuncList());

			  dataRaceAnalyser drAnalyser;
			  drAnalyser.analyse(frontEndContext.getFuncList());
			  
			  if(strcmp(backendTarget,"cuda")==0)
			  {
			  	deviceVarsAnalyser dvAnalyser;
				//cpp_backend.setOptimized();
			  	dvAnalyser.analyse(frontEndContext.getFuncList());
			  }
		  }
		  */
	  //cpp_backend.setFileName(fileName);
	  //cpp_backend.generate();
     if (strcmp(backendTarget, "cuda") == 0) {
        spcuda::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
	//~ cpp_backend.setOptimized();
	
	if (optimize) {
	  attachPropAnalyser apAnalyser;
	  apAnalyser.analyse(frontEndContext.getFuncList());

	  dataRaceAnalyser drAnalyser;
	  drAnalyser.analyse(frontEndContext.getFuncList());

	  deviceVarsAnalyser dvAnalyser;
	  dvAnalyser.analyse(frontEndContext.getFuncList());
	  cpp_backend.setOptimized();
	}
		  
        cpp_backend.generate();
      } 
      else if (strcmp(backendTarget, "omp") == 0) {
        spomp::dsl_cpp_generator cpp_backend;
	std::cout<< "size:" << frontEndContext.getFuncList().size() << '\n';
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
      } 
	  else if (strcmp(backendTarget, "mpi") == 0) {
        spmpi::dsl_cpp_generator cpp_backend;
		std::cout<< "size:" << frontEndContext.getFuncList().size() << '\n';
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
      } 
      else if (strcmp(backendTarget, "acc") == 0) {
        spacc::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
		if(optimize) {
			cpp_backend.setOptimized();
			blockVarsAnalyser bvAnalyser;
			bvAnalyser.analyse(frontEndContext.getFuncList());
		}
        cpp_backend.generate();
      }
	  else if(strcmp(backendTarget, "multigpu") == 0){
		spmultigpu::dsl_cpp_generator cpp_backend;
		pushpullAnalyser pp;
		pp.analyse(frontEndContext.getFuncList());
		cpp_backend.setFileName(fileName);
		cpp_backend.generate();
}
	  else if (strcmp(backendTarget, "sycl") == 0) {
		std::cout<<"GENERATING SYCL CODE"<<std::endl;
        spsycl::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
	  }
	  else if (strcmp(backendTarget, "amd") == 0) {
		std::cout<<"GENERATING OPENCL CODE"<<std::endl;
        spamd::dsl_cpp_generator cpp_backend;
        cpp_backend.setFileName(fileName);
        cpp_backend.generate();
	  }
      else
	    std::cout<< "invalid backend" << '\n';
	  }
	else 
	 {
		if(strcmp(backendTarget, "omp") == 0) {
		   spdynomp::dsl_dyn_cpp_generator cpp_dyn_gen;
		   cpp_dyn_gen.setFileName(fileName);
	       cpp_dyn_gen.generate();
		}
		if(strcmp(backendTarget, "mpi") == 0){
		   spdynmpi::dsl_dyn_cpp_generator cpp_dyn_gen;
		   std::cout<<"created dyn mpi"<<std::endl;
		   cpp_dyn_gen.setFileName(fileName);
		   std::cout<<"file name set"<<std::endl;
	       cpp_dyn_gen.generate();	
		}
	 }
	
   }

	printf("finished successfully\n");
   
   /* to generate code, ./finalcode -s/-d -f "filename" -b "backendname"*/
	return 0;   
	 
}
