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
    T_NP = 310,                    /* T_NP  */
    T_EP = 311,                    /* T_EP  */
    T_LIST = 312,                  /* T_LIST  */
    T_SET_NODES = 313,             /* T_SET_NODES  */
    T_SET_EDGES = 314,             /* T_SET_EDGES  */
    T_FROM = 315,                  /* T_FROM  */
    T_BFS = 316,                   /* T_BFS  */
    T_REVERSE = 317,               /* T_REVERSE  */
    T_INCREMENTAL = 318,           /* T_INCREMENTAL  */
    T_DECREMENTAL = 319,           /* T_DECREMENTAL  */
    T_STATIC = 320,                /* T_STATIC  */
    T_DYNAMIC = 321,               /* T_DYNAMIC  */
    T_BATCH = 322,                 /* T_BATCH  */
    T_ONADD = 323,                 /* T_ONADD  */
    T_ONDELETE = 324,              /* T_ONDELETE  */
    ID = 325,                      /* ID  */
    INT_NUM = 326,                 /* INT_NUM  */
    FLOAT_NUM = 327,               /* FLOAT_NUM  */
    BOOL_VAL = 328,                /* BOOL_VAL  */
    CHAR_VAL = 329                 /* CHAR_VAL  */
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
#define T_NP 310
#define T_EP 311
#define T_LIST 312
#define T_SET_NODES 313
#define T_SET_EDGES 314
#define T_FROM 315
#define T_BFS 316
#define T_REVERSE 317
#define T_INCREMENTAL 318
#define T_DECREMENTAL 319
#define T_STATIC 320
#define T_DYNAMIC 321
#define T_BATCH 322
#define T_ONADD 323
#define T_ONDELETE 324
#define ID 325
#define INT_NUM 326
#define FLOAT_NUM 327
#define BOOL_VAL 328
#define CHAR_VAL 329

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
     

#line 321 "y.tab.c"

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
  YYSYMBOL_T_NP = 55,                      /* T_NP  */
  YYSYMBOL_T_EP = 56,                      /* T_EP  */
  YYSYMBOL_T_LIST = 57,                    /* T_LIST  */
  YYSYMBOL_T_SET_NODES = 58,               /* T_SET_NODES  */
  YYSYMBOL_T_SET_EDGES = 59,               /* T_SET_EDGES  */
  YYSYMBOL_T_FROM = 60,                    /* T_FROM  */
  YYSYMBOL_T_BFS = 61,                     /* T_BFS  */
  YYSYMBOL_T_REVERSE = 62,                 /* T_REVERSE  */
  YYSYMBOL_T_INCREMENTAL = 63,             /* T_INCREMENTAL  */
  YYSYMBOL_T_DECREMENTAL = 64,             /* T_DECREMENTAL  */
  YYSYMBOL_T_STATIC = 65,                  /* T_STATIC  */
  YYSYMBOL_T_DYNAMIC = 66,                 /* T_DYNAMIC  */
  YYSYMBOL_T_BATCH = 67,                   /* T_BATCH  */
  YYSYMBOL_T_ONADD = 68,                   /* T_ONADD  */
  YYSYMBOL_T_ONDELETE = 69,                /* T_ONDELETE  */
  YYSYMBOL_ID = 70,                        /* ID  */
  YYSYMBOL_INT_NUM = 71,                   /* INT_NUM  */
  YYSYMBOL_FLOAT_NUM = 72,                 /* FLOAT_NUM  */
  YYSYMBOL_BOOL_VAL = 73,                  /* BOOL_VAL  */
  YYSYMBOL_CHAR_VAL = 74,                  /* CHAR_VAL  */
  YYSYMBOL_75_ = 75,                       /* '?'  */
  YYSYMBOL_76_ = 76,                       /* ':'  */
  YYSYMBOL_77_ = 77,                       /* '<'  */
  YYSYMBOL_78_ = 78,                       /* '>'  */
  YYSYMBOL_79_ = 79,                       /* '+'  */
  YYSYMBOL_80_ = 80,                       /* '-'  */
  YYSYMBOL_81_ = 81,                       /* '*'  */
  YYSYMBOL_82_ = 82,                       /* '/'  */
  YYSYMBOL_83_ = 83,                       /* '%'  */
  YYSYMBOL_84_ = 84,                       /* '('  */
  YYSYMBOL_85_ = 85,                       /* ')'  */
  YYSYMBOL_86_ = 86,                       /* ','  */
  YYSYMBOL_87_ = 87,                       /* ';'  */
  YYSYMBOL_88_ = 88,                       /* '.'  */
  YYSYMBOL_89_ = 89,                       /* '{'  */
  YYSYMBOL_90_ = 90,                       /* '}'  */
  YYSYMBOL_91_ = 91,                       /* '='  */
  YYSYMBOL_92_ = 92,                       /* '!'  */
  YYSYMBOL_93_ = 93,                       /* '['  */
  YYSYMBOL_94_ = 94,                       /* ']'  */
  YYSYMBOL_YYACCEPT = 95,                  /* $accept  */
  YYSYMBOL_program = 96,                   /* program  */
  YYSYMBOL_function_def = 97,              /* function_def  */
  YYSYMBOL_function_data = 98,             /* function_data  */
  YYSYMBOL_paramList = 99,                 /* paramList  */
  YYSYMBOL_type = 100,                     /* type  */
  YYSYMBOL_param = 101,                    /* param  */
  YYSYMBOL_function_body = 102,            /* function_body  */
  YYSYMBOL_statements = 103,               /* statements  */
  YYSYMBOL_statement = 104,                /* statement  */
  YYSYMBOL_blockstatements = 105,          /* blockstatements  */
  YYSYMBOL_batch_blockstmt = 106,          /* batch_blockstmt  */
  YYSYMBOL_on_add_blockstmt = 107,         /* on_add_blockstmt  */
  YYSYMBOL_on_delete_blockstmt = 108,      /* on_delete_blockstmt  */
  YYSYMBOL_block_begin = 109,              /* block_begin  */
  YYSYMBOL_block_end = 110,                /* block_end  */
  YYSYMBOL_return_stmt = 111,              /* return_stmt  */
  YYSYMBOL_declaration = 112,              /* declaration  */
  YYSYMBOL_type1 = 113,                    /* type1  */
  YYSYMBOL_primitive = 114,                /* primitive  */
  YYSYMBOL_graph = 115,                    /* graph  */
  YYSYMBOL_collections = 116,              /* collections  */
  YYSYMBOL_container = 117,                /* container  */
  YYSYMBOL_nodemap = 118,                  /* nodemap  */
  YYSYMBOL_type2 = 119,                    /* type2  */
  YYSYMBOL_property = 120,                 /* property  */
  YYSYMBOL_assignment = 121,               /* assignment  */
  YYSYMBOL_rhs = 122,                      /* rhs  */
  YYSYMBOL_expression = 123,               /* expression  */
  YYSYMBOL_indexExpr = 124,                /* indexExpr  */
  YYSYMBOL_unary_expr = 125,               /* unary_expr  */
  YYSYMBOL_proc_call = 126,                /* proc_call  */
  YYSYMBOL_val = 127,                      /* val  */
  YYSYMBOL_control_flow = 128,             /* control_flow  */
  YYSYMBOL_iteration_cf = 129,             /* iteration_cf  */
  YYSYMBOL_filterExpr = 130,               /* filterExpr  */
  YYSYMBOL_boolean_expr = 131,             /* boolean_expr  */
  YYSYMBOL_selection_cf = 132,             /* selection_cf  */
  YYSYMBOL_reduction = 133,                /* reduction  */
  YYSYMBOL_reduce_op = 134,                /* reduce_op  */
  YYSYMBOL_leftList = 135,                 /* leftList  */
  YYSYMBOL_rightList = 136,                /* rightList  */
  YYSYMBOL_reductionCall = 137,            /* reductionCall  */
  YYSYMBOL_reduction_calls = 138,          /* reduction_calls  */
  YYSYMBOL_leftSide = 139,                 /* leftSide  */
  YYSYMBOL_arg_list = 140,                 /* arg_list  */
  YYSYMBOL_bfs_abstraction = 141,          /* bfs_abstraction  */
  YYSYMBOL_reverse_abstraction = 142,      /* reverse_abstraction  */
  YYSYMBOL_oid = 143,                      /* oid  */
  YYSYMBOL_tid = 144,                      /* tid  */
  YYSYMBOL_id = 145                        /* id  */
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
#define YYLAST   789

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  95
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  51
/* YYNRULES -- Number of rules.  */
#define YYNRULES  160
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  376

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   329


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
       2,     2,     2,    92,     2,     2,     2,    83,     2,     2,
      84,    85,    81,    79,    86,    80,    88,    82,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    76,    87,
      77,    91,    78,    75,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    93,     2,    94,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    89,     2,    90,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74
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
     239,   242,   243,   245,   246,   248,   250,   251,   252,   254,
     255,   256,   259,   261,   262,   263,   265,   266,   267,   268,
     269,   271,   274,   275,   278,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     297,   298,   299,   300,   302,   304,   305,   307,   312,   316,
     320,   331,   332,   333,   334,   335,   336,   339,   340,   342,
     343,   344,   345,   347,   348,   349,   350,   351,   353,   354,
     356,   358,   359,   362,   363,   365,   366,   369,   370,   371,
     372,   373,   375,   377,   379,   380,   382,   383,   392,   394,
     395,   396,   397,   398,   400,   401,   402,   403,   406,   410,
     427,   434,   439,   447,   448,   452,   453,   456,   460,   467,
     471
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
  "T_UPDATES", "T_CONTAINER", "T_NODEMAP", "T_NP", "T_EP", "T_LIST",
  "T_SET_NODES", "T_SET_EDGES", "T_FROM", "T_BFS", "T_REVERSE",
  "T_INCREMENTAL", "T_DECREMENTAL", "T_STATIC", "T_DYNAMIC", "T_BATCH",
  "T_ONADD", "T_ONDELETE", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL",
  "CHAR_VAL", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'('", "')'", "','", "';'", "'.'", "'{'", "'}'", "'='", "'!'",
  "'['", "']'", "$accept", "program", "function_def", "function_data",
  "paramList", "type", "param", "function_body", "statements", "statement",
  "blockstatements", "batch_blockstmt", "on_add_blockstmt",
  "on_delete_blockstmt", "block_begin", "block_end", "return_stmt",
  "declaration", "type1", "primitive", "graph", "collections", "container",
  "nodemap", "type2", "property", "assignment", "rhs", "expression",
  "indexExpr", "unary_expr", "proc_call", "val", "control_flow",
  "iteration_cf", "filterExpr", "boolean_expr", "selection_cf",
  "reduction", "reduce_op", "leftList", "rightList", "reductionCall",
  "reduction_calls", "leftSide", "arg_list", "bfs_abstraction",
  "reverse_abstraction", "oid", "tid", "id", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-319)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-160)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -319,   190,  -319,   -21,   -25,   -16,   -21,   -21,  -319,   -10,
    -319,     2,   720,   720,    13,    17,  -319,  -319,  -319,  -319,
     720,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,
     -42,    36,    25,    59,    66,  -319,    72,    77,    82,    90,
     -21,  -319,  -319,  -319,  -319,  -319,   -21,  -319,    98,   720,
     720,   368,   104,   -21,   720,   720,   730,   390,   -21,   -21,
    -319,   720,  -319,    78,  -319,   106,   109,   112,   115,  -319,
    -319,  -319,   121,   123,    12,   -10,   195,   127,   133,   137,
     141,   143,   147,  -319,  -319,  -319,    12,    12,  -319,    12,
    -319,  -319,  -319,  -319,  -319,  -319,   149,   151,   -21,   -21,
     152,   459,   -54,   153,   156,  -319,  -319,  -319,  -319,   158,
     -11,  -319,  -319,  -319,   146,  -319,   157,   168,  -319,  -319,
     163,   173,   184,   188,   198,   201,   204,   205,   206,  -319,
     -21,  -319,  -319,   -21,   -21,    12,    12,   620,   -43,  -319,
    -319,   189,   272,   202,   -21,    12,    12,   -21,   -21,   -21,
     620,   213,    32,   532,   620,  -319,  -319,   207,   208,  -319,
    -319,  -319,  -319,  -319,  -319,  -319,  -319,    12,    12,    12,
      12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
      12,    12,    12,  -319,  -319,  -319,    12,   100,    12,   -21,
    -319,   211,  -319,  -319,  -319,  -319,  -319,  -319,  -319,  -319,
    -319,   215,   273,   285,   620,   220,   221,   223,   -21,   295,
     229,   541,   -45,   232,   236,   247,   306,   307,   239,    12,
    -319,    12,    12,   638,   629,   270,   270,   322,   322,   270,
     270,    76,    76,    -6,    -6,   522,   620,   249,  -319,   620,
     246,  -319,  -319,  -319,  -319,  -319,  -319,  -319,   251,   620,
     -35,    12,  -319,    12,    12,   458,   -10,    12,   260,   -21,
      12,    12,    12,  -319,  -319,    12,   -21,   -21,   261,  -319,
    -319,  -319,  -319,    12,  -319,    12,   -21,   -21,    20,   -14,
      97,   252,    -7,   101,   258,   332,  -319,   271,    12,   267,
    -319,  -319,   551,   274,   278,    48,   276,   282,  -319,   243,
    -319,   720,   -10,   -10,    12,   -10,   -10,    12,   458,   294,
     611,    12,   -10,    12,    12,   283,   304,  -319,  -319,   298,
    -319,  -319,   -67,   -33,  -319,  -319,   -67,  -319,  -319,   -10,
     327,  -319,   320,   321,    12,  -319,   385,   323,   -21,   324,
    -319,   -21,   334,   335,   326,   336,    33,   329,   -10,   -24,
     -10,   343,   -10,   -10,    12,  -319,    12,    12,  -319,  -319,
     342,  -319,  -319,  -319,  -319,   348,   -10,  -319,   372,   -17,
    -319,    12,  -319,   361,   -10,  -319
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     160,     0,     0,     0,     0,     0,    36,     4,    17,    18,
       0,    46,    47,    48,    49,    50,    51,    52,    63,    64,
       0,     0,     0,     0,     0,    53,     0,     0,     0,    10,
       0,    43,    44,    45,    57,    58,     0,    65,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     0,    14,    15,     8,     0,     0,     0,     0,   105,
     104,   106,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   101,   102,   103,     0,     0,    37,     0,
      19,    26,    29,    30,    31,    32,     0,     0,     0,     0,
       0,     0,    93,    92,    75,    90,    23,   108,   107,     0,
      91,    25,   145,   146,   144,     5,     0,     0,    12,    13,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    11,
       0,     6,     9,     0,     0,     0,     0,    38,    93,    92,
      75,    91,     0,     0,     0,   148,   148,     0,     0,     0,
       0,     0,    91,     0,    88,    28,    20,    39,    41,    21,
     127,   131,   128,   130,   129,    95,    96,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    27,    22,    24,   148,     0,     0,     0,
      56,    61,    62,    70,    71,    66,    68,    67,    69,    54,
      55,     0,     0,     0,   120,     0,     0,     0,     0,     0,
     152,   151,    91,     0,     0,     0,     0,     0,     0,     0,
      89,     0,     0,    80,    81,    82,    83,    86,    87,    84,
      85,    76,    77,    78,    79,     0,   126,    91,    73,    74,
       0,   139,   140,   141,   142,   143,    72,   123,     0,   125,
     157,   148,    16,     0,     0,     0,     0,     0,     0,     0,
     148,   148,     0,    98,    99,     0,     0,     0,     0,   132,
      40,    42,    94,   148,    97,   148,     0,     0,     0,    93,
      91,   144,    93,    91,   144,   121,   110,     0,     0,     0,
     149,   150,     0,     0,     0,     0,     0,     0,   159,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    97,   138,   158,     0,
     117,   113,    75,   144,   116,   114,    75,   122,   111,     0,
      75,    33,    75,    75,     0,    59,     0,     0,     0,     0,
     109,     0,     0,     0,    90,     0,    91,     0,     0,   157,
       0,     0,     0,     0,     0,   124,     0,     0,   112,   115,
     118,    34,    35,   134,   135,     0,     0,   119,   154,     0,
     153,     0,   155,     0,     0,   156
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -319,  -319,  -319,  -319,   148,   -52,  -319,  -319,  -319,  -254,
      -9,  -319,  -319,  -319,  -319,  -319,  -319,  -319,   -49,   131,
    -319,   145,  -319,  -319,   -23,  -319,   -36,  -174,   -47,   -31,
     -18,     5,  -318,  -319,  -319,  -317,  -107,  -319,  -319,   340,
     234,  -177,   159,  -319,    14,   -95,  -319,  -319,  -319,  -319,
       4
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     8,     9,    38,   117,    39,    17,    51,    90,
      91,    92,    93,    94,    19,    95,    96,    97,    40,    41,
      42,    43,    44,    45,    46,    47,   210,   246,   150,   138,
     139,   140,   105,   106,   107,   337,   205,   108,   109,   180,
     151,   345,   247,   248,   141,   213,   111,   370,   112,   113,
     114
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,   285,    98,   120,   101,   118,   118,    11,   238,   339,
      14,    15,   160,   161,   162,   100,   344,   163,  -118,   164,
     102,   336,    69,    70,    71,   165,   166,   137,    99,   206,
    -147,   119,   119,   103,   181,    53,   344,   182,   344,   186,
     153,  -147,   154,   366,    62,   181,   262,   270,   271,    10,
      63,   214,  -157,   276,   327,   338,   104,   116,   277,    12,
     277,  -159,   127,   128,   276,   110,   142,   371,    13,   277,
    -147,   302,    16,   186,   181,    78,    79,  -147,   305,    16,
     187,   181,    10,    83,    84,    85,    20,   179,   204,   204,
     241,   240,   242,   243,   244,   245,    87,    49,   211,   211,
     152,    50,   157,   158,    89,   300,   301,   165,   166,    55,
      69,    70,    71,    54,   102,   102,   186,   186,   219,   356,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   201,   239,    56,   202,   203,   211,
     239,   249,   241,    57,   242,   243,   244,   245,   209,    58,
     287,   215,   216,   217,    59,   102,   278,   177,   178,   212,
     212,    48,   130,    78,    79,   290,   291,    60,    52,   179,
      10,    83,    84,    85,   239,   239,    61,   363,   296,   364,
     297,   186,   303,    64,    87,   186,   306,   123,   125,   115,
       2,   131,    89,   250,   132,   237,   133,    65,    66,   134,
     212,   124,   126,     3,   211,   135,    98,   136,   101,   129,
     204,   144,   258,   211,   211,   239,   143,   145,   292,   100,
     102,   146,   279,   282,   102,   147,   211,   148,   211,   102,
     102,   149,    99,   152,   189,   190,   155,   103,   156,   159,
     183,   310,   102,   184,   102,   185,   191,   286,   192,   319,
     365,   193,   118,     4,     5,     6,     7,   281,   284,    98,
     104,   101,   194,   289,   373,   212,   195,   280,   283,   110,
     293,   294,   100,   186,   212,   212,   196,   102,   119,   197,
     298,   299,   198,   199,   200,    99,   208,   212,   207,   212,
     103,   218,   253,   320,   321,   251,   324,   325,   221,   222,
     252,   165,   166,   331,   254,   255,   256,   257,   323,   322,
     204,   323,   326,   104,   259,   260,   330,   263,   332,   333,
     340,   264,   110,   265,   204,   266,   267,  -100,  -100,  -100,
     268,   274,  -100,   273,  -100,   275,   288,   318,   295,   358,
     304,   359,   349,   361,   362,   351,   307,   308,   346,   175,
     176,   177,   178,   165,   166,   311,   309,   368,   169,   170,
     372,   316,   313,   179,  -100,   375,   314,   317,   346,   334,
     346,    21,    22,    23,    24,    25,    67,    68,    69,    70,
      71,   328,    72,   335,    73,    74,    75,   341,    76,  -100,
    -100,  -100,  -100,    21,    22,    23,    24,    25,  -100,   173,
     174,   175,   176,   177,   178,   342,   343,   347,   348,   350,
     352,   353,   354,   357,   355,   179,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,   360,    77,
     336,    78,    79,   367,   369,    80,    81,    82,    10,    83,
      84,    85,    30,    31,    32,    86,   374,    35,    36,    37,
     188,     0,    87,   269,   315,     0,     0,    16,    88,     0,
      89,    21,    22,    23,    24,    25,    67,    68,    69,    70,
      71,     0,    72,     0,    73,    74,    75,     0,    76,     0,
       0,     0,   160,   161,   162,     0,     0,   163,     0,   164,
     165,   166,     0,   167,   168,   169,   170,   171,   172,     0,
       0,     0,     0,     0,     0,     0,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,    77,
       0,    78,    79,     0,     0,    80,    81,    82,    10,    83,
      84,    85,     0,     0,     0,    86,   173,   174,   175,   176,
     177,   178,    87,     0,     0,     0,     0,    16,     0,     0,
      89,     0,   179,   165,   166,     0,   167,   168,   169,   170,
     171,   172,     0,   165,   166,     0,   167,   168,   169,   170,
     171,   172,   165,   166,     0,   167,   168,   169,   170,   171,
     172,     0,   165,   166,     0,   167,   168,   169,   170,   171,
     172,     0,     0,     0,     0,     0,     0,     0,     0,   173,
     174,   175,   176,   177,   178,     0,     0,     0,     0,   173,
     174,   175,   176,   177,   178,   179,   272,   220,   173,   174,
     175,   176,   177,   178,     0,   179,     0,   261,   173,   174,
     175,   176,   177,   178,   179,     0,   312,     0,     0,     0,
       0,     0,   165,   166,   179,   167,   168,   169,   170,   171,
     172,   165,   166,     0,   167,   168,   169,   170,   171,   172,
     165,   166,     0,   167,     0,   169,   170,   171,   172,   165,
     166,     0,     0,     0,   169,   170,   171,   172,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   173,   174,
     175,   176,   177,   178,     0,     0,   329,   173,   174,   175,
     176,   177,   178,     0,   179,     0,   173,   174,   175,   176,
     177,   178,     0,   179,     0,   173,   174,   175,   176,   177,
     178,     0,   179,    21,    22,    23,    24,    25,     0,     0,
       0,   179,     0,    21,    22,    23,    24,    25,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
     121,   122,    30,    31,    32,     0,     0,    35,    36,    37
};

static const yytype_int16 yycheck[] =
{
       9,   255,    51,    55,    51,    54,    55,     3,   182,   326,
       6,     7,    23,    24,    25,    51,   334,    28,    85,    30,
      51,    88,    10,    11,    12,    31,    32,    74,    51,   136,
      84,    54,    55,    51,    88,    77,   354,    91,   356,    84,
      87,    84,    89,   360,    40,    88,    91,   221,   222,    70,
      46,   146,    85,    88,   308,    88,    51,    53,    93,    84,
      93,    85,    58,    59,    88,    51,    75,    84,    84,    93,
      84,    85,    89,    84,    88,    63,    64,    84,    85,    89,
      91,    88,    70,    71,    72,    73,    84,    93,   135,   136,
      42,   186,    44,    45,    46,    47,    84,    84,   145,   146,
      86,    84,    98,    99,    92,    85,    86,    31,    32,    84,
      10,    11,    12,    77,   145,   146,    84,    84,    86,    86,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   130,   182,    77,   133,   134,   186,
     187,   188,    42,    77,    44,    45,    46,    47,   144,    77,
     257,   147,   148,   149,    77,   186,   251,    81,    82,   145,
     146,    13,    84,    63,    64,   260,   261,    85,    20,    93,
      70,    71,    72,    73,   221,   222,    86,   354,   273,   356,
     275,    84,    85,    85,    84,    84,    85,    56,    57,    85,
       0,    85,    92,   189,    85,   181,    84,    49,    50,    84,
     186,    56,    57,    13,   251,    84,   255,    84,   255,    61,
     257,    84,   208,   260,   261,   262,    21,    84,   265,   255,
     251,    84,   253,   254,   255,    84,   273,    84,   275,   260,
     261,    84,   255,   219,    88,    78,    87,   255,    87,    87,
      87,   288,   273,    87,   275,    87,    78,   256,    85,   301,
     357,    78,   301,    63,    64,    65,    66,   253,   254,   308,
     255,   308,    78,   259,   371,   251,    78,   253,   254,   255,
     266,   267,   308,    84,   260,   261,    78,   308,   301,    78,
     276,   277,    78,    78,    78,   308,    84,   273,    16,   275,
     308,    78,    19,   302,   303,    84,   305,   306,    91,    91,
      85,    31,    32,   312,    19,    85,    85,    84,   304,   304,
     357,   307,   307,   308,    19,    86,   311,    85,   313,   314,
     329,    85,   308,    76,   371,    19,    19,    23,    24,    25,
      91,    85,    28,    84,    30,    84,    76,    94,    77,   348,
      88,   350,   338,   352,   353,   341,    88,    15,   334,    79,
      80,    81,    82,    31,    32,    88,    85,   366,    36,    37,
     369,    85,    88,    93,    60,   374,    88,    85,   354,    86,
     356,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    87,    14,    85,    16,    17,    18,    60,    20,    85,
      86,    87,    88,     3,     4,     5,     6,     7,    94,    77,
      78,    79,    80,    81,    82,    85,    85,    22,    85,    85,
      76,    76,    86,    84,    78,    93,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    85,    61,
      88,    63,    64,    85,    62,    67,    68,    69,    70,    71,
      72,    73,    52,    53,    54,    77,    85,    57,    58,    59,
     110,    -1,    84,   219,   295,    -1,    -1,    89,    90,    -1,
      92,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    14,    -1,    16,    17,    18,    -1,    20,    -1,
      -1,    -1,    23,    24,    25,    -1,    -1,    28,    -1,    30,
      31,    32,    -1,    34,    35,    36,    37,    38,    39,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    61,
      -1,    63,    64,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    -1,    -1,    -1,    77,    77,    78,    79,    80,
      81,    82,    84,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      92,    -1,    93,    31,    32,    -1,    34,    35,    36,    37,
      38,    39,    -1,    31,    32,    -1,    34,    35,    36,    37,
      38,    39,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    -1,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      78,    79,    80,    81,    82,    -1,    -1,    -1,    -1,    77,
      78,    79,    80,    81,    82,    93,    94,    85,    77,    78,
      79,    80,    81,    82,    -1,    93,    -1,    86,    77,    78,
      79,    80,    81,    82,    93,    -1,    85,    -1,    -1,    -1,
      -1,    -1,    31,    32,    93,    34,    35,    36,    37,    38,
      39,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      31,    32,    -1,    34,    -1,    36,    37,    38,    39,    31,
      32,    -1,    -1,    -1,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,    78,
      79,    80,    81,    82,    -1,    -1,    85,    77,    78,    79,
      80,    81,    82,    -1,    93,    -1,    77,    78,    79,    80,
      81,    82,    -1,    93,    -1,    77,    78,    79,    80,    81,
      82,    -1,    93,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    93,    -1,     3,     4,     5,     6,     7,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      50,    51,    52,    53,    54,    -1,    -1,    57,    58,    59
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    96,     0,    13,    63,    64,    65,    66,    97,    98,
      70,   145,    84,    84,   145,   145,    89,   102,   105,   109,
      84,     3,     4,     5,     6,     7,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    99,   101,
     113,   114,   115,   116,   117,   118,   119,   120,    99,    84,
      84,   103,    99,    77,    77,    84,    77,    77,    77,    77,
      85,    86,   145,   145,    85,    99,    99,     8,     9,    10,
      11,    12,    14,    16,    17,    18,    20,    61,    63,    64,
      67,    68,    69,    71,    72,    73,    77,    84,    90,    92,
     104,   105,   106,   107,   108,   110,   111,   112,   113,   119,
     121,   123,   124,   125,   126,   127,   128,   129,   132,   133,
     139,   141,   143,   144,   145,    85,   145,   100,   113,   119,
     100,    50,    51,   114,   116,   114,   116,   145,   145,    99,
      84,    85,    85,    84,    84,    84,    84,   123,   124,   125,
     126,   139,   105,    21,    84,    84,    84,    84,    84,    84,
     123,   135,   139,   123,   123,    87,    87,   145,   145,    87,
      23,    24,    25,    28,    30,    31,    32,    34,    35,    36,
      37,    38,    39,    77,    78,    79,    80,    81,    82,    93,
     134,    88,    91,    87,    87,    87,    84,    91,   134,    88,
      78,    78,    85,    78,    78,    78,    78,    78,    78,    78,
      78,   145,   145,   145,   123,   131,   131,    16,    84,   145,
     121,   123,   139,   140,   140,   145,   145,   145,    78,    86,
      85,    91,    91,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   139,   122,   123,
     140,    42,    44,    45,    46,    47,   122,   137,   138,   123,
     145,    84,    85,    19,    19,    85,    85,    84,   145,    19,
      86,    86,    91,    85,    85,    76,    19,    19,    91,   135,
     122,   122,    94,    84,    85,    84,    88,    93,   140,   124,
     139,   145,   124,   139,   145,   104,   105,   131,    76,   145,
     140,   140,   123,   145,   145,    77,   140,   140,   145,   145,
      85,    86,    85,    85,    88,    85,    85,    88,    15,    85,
     123,    88,    85,    88,    88,   137,    85,    85,    94,   100,
     105,   105,   126,   145,   105,   105,   126,   104,    87,    85,
     126,   105,   126,   126,    86,    85,    88,   130,    88,   130,
     105,    60,    85,    85,   127,   136,   139,    22,    85,   145,
      85,   145,    76,    76,    86,    78,    86,    84,   105,   105,
      85,   105,   105,   136,   136,   131,   130,    85,   105,    62,
     142,    84,   105,   131,    85,   105
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    95,    96,    96,    97,    98,    98,    98,    98,    98,
      99,    99,   100,   100,   101,   101,   101,   102,   103,   103,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     112,   112,   112,   113,   113,   113,   114,   114,   114,   114,
     114,   115,   115,   116,   116,   116,   116,   116,   116,   117,
     117,   117,   118,   119,   119,   119,   120,   120,   120,   120,
     120,   120,   121,   121,   122,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     123,   123,   123,   123,   124,   125,   125,   126,   126,   126,
     126,   127,   127,   127,   127,   127,   127,   128,   128,   129,
     129,   129,   129,   129,   129,   129,   129,   129,   130,   130,
     131,   132,   132,   133,   133,   133,   133,   134,   134,   134,
     134,   134,   135,   135,   136,   136,   136,   136,   137,   138,
     138,   138,   138,   138,   139,   139,   139,   139,   140,   140,
     140,   140,   140,   141,   141,   142,   142,   143,   143,   144,
     145
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     1,     1,     2,     2,     5,     1,     0,     2,
       2,     2,     2,     1,     2,     1,     1,     2,     2,     1,
       1,     1,     3,     7,    10,    10,     1,     1,     2,     2,
       4,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     4,     4,     4,     1,     1,     9,
       7,     4,     4,     1,     1,     1,     4,     4,     4,     4,
       4,     4,     3,     3,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     3,
       1,     1,     1,     1,     4,     2,     2,     4,     4,     4,
       6,     1,     1,     1,     1,     1,     1,     1,     1,     8,
       5,     7,    10,     7,     7,    10,     7,     7,     0,     5,
       1,     5,     7,     3,     9,     3,     3,     1,     1,     1,
       1,     1,     3,     1,     3,     3,     1,     1,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     0,     3,
       3,     1,     1,    13,    12,     2,     5,     3,     6,     5,
       1
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
#line 1787 "y.tab.c"
    break;

  case 4: /* function_def: function_data function_body  */
#line 108 "lrparser.y"
                                            { 
	                                          Function* func=(Function*)(yyvsp[-1].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[0].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
#line 1798 "y.tab.c"
    break;

  case 5: /* function_data: T_FUNC id '(' paramList ')'  */
#line 115 "lrparser.y"
                                           { 
										   (yyval.node)=Util::createFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
#line 1809 "y.tab.c"
    break;

  case 6: /* function_data: T_STATIC id '(' paramList ')'  */
#line 121 "lrparser.y"
                                                           { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1820 "y.tab.c"
    break;

  case 7: /* function_data: T_INCREMENTAL '(' paramList ')'  */
#line 127 "lrparser.y"
                                                     { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1831 "y.tab.c"
    break;

  case 8: /* function_data: T_DECREMENTAL '(' paramList ')'  */
#line 133 "lrparser.y"
                                                             { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1842 "y.tab.c"
    break;

  case 9: /* function_data: T_DYNAMIC id '(' paramList ')'  */
#line 139 "lrparser.y"
                                                        { (yyval.node)=Util::createDynamicFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
#line 1852 "y.tab.c"
    break;

  case 10: /* paramList: param  */
#line 146 "lrparser.y"
                 {(yyval.pList)=Util::createPList((yyvsp[0].node));}
#line 1858 "y.tab.c"
    break;

  case 11: /* paramList: param ',' paramList  */
#line 147 "lrparser.y"
                                     {(yyval.pList)=Util::addToPList((yyvsp[0].pList),(yyvsp[-2].node)); 
			                           }
#line 1865 "y.tab.c"
    break;

  case 12: /* type: type1  */
#line 150 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1871 "y.tab.c"
    break;

  case 13: /* type: type2  */
#line 151 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1877 "y.tab.c"
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
#line 1893 "y.tab.c"
    break;

  case 15: /* param: type2 id  */
#line 164 "lrparser.y"
                          { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 1902 "y.tab.c"
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
#line 1914 "y.tab.c"
    break;

  case 17: /* function_body: blockstatements  */
#line 177 "lrparser.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 1920 "y.tab.c"
    break;

  case 18: /* statements: %empty  */
#line 180 "lrparser.y"
              {}
#line 1926 "y.tab.c"
    break;

  case 19: /* statements: statements statement  */
#line 181 "lrparser.y"
                               {printf ("found one statement\n") ; Util::addToBlock((yyvsp[0].node)); }
#line 1932 "y.tab.c"
    break;

  case 20: /* statement: declaration ';'  */
#line 183 "lrparser.y"
                          {(yyval.node)=(yyvsp[-1].node);}
#line 1938 "y.tab.c"
    break;

  case 21: /* statement: assignment ';'  */
#line 184 "lrparser.y"
                       {printf ("found an assignment type statement" ); (yyval.node)=(yyvsp[-1].node);}
#line 1944 "y.tab.c"
    break;

  case 22: /* statement: proc_call ';'  */
#line 185 "lrparser.y"
                       {printf ("found an proc call type statement" );(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[-1].node));}
#line 1950 "y.tab.c"
    break;

  case 23: /* statement: control_flow  */
#line 186 "lrparser.y"
                      {printf ("found an control flow type statement" );(yyval.node)=(yyvsp[0].node);}
#line 1956 "y.tab.c"
    break;

  case 24: /* statement: reduction ';'  */
#line 187 "lrparser.y"
                      {printf ("found an reduction type statement" );(yyval.node)=(yyvsp[-1].node);}
#line 1962 "y.tab.c"
    break;

  case 25: /* statement: bfs_abstraction  */
#line 188 "lrparser.y"
                          {printf ("found bfs\n") ;(yyval.node)=(yyvsp[0].node); }
#line 1968 "y.tab.c"
    break;

  case 26: /* statement: blockstatements  */
#line 189 "lrparser.y"
                          {printf ("found block\n") ;(yyval.node)=(yyvsp[0].node);}
#line 1974 "y.tab.c"
    break;

  case 27: /* statement: unary_expr ';'  */
#line 190 "lrparser.y"
                         {printf ("found unary\n") ;(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[-1].node));}
#line 1980 "y.tab.c"
    break;

  case 28: /* statement: return_stmt ';'  */
#line 191 "lrparser.y"
                          {printf ("found return\n") ;(yyval.node) = (yyvsp[-1].node) ;}
#line 1986 "y.tab.c"
    break;

  case 29: /* statement: batch_blockstmt  */
#line 192 "lrparser.y"
                           {printf ("found batch\n") ;(yyval.node) = (yyvsp[0].node);}
#line 1992 "y.tab.c"
    break;

  case 30: /* statement: on_add_blockstmt  */
#line 193 "lrparser.y"
                           {printf ("found on add block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 1998 "y.tab.c"
    break;

  case 31: /* statement: on_delete_blockstmt  */
#line 194 "lrparser.y"
                              {printf ("found delete block\n") ;(yyval.node) = (yyvsp[0].node);}
#line 2004 "y.tab.c"
    break;

  case 32: /* blockstatements: block_begin statements block_end  */
#line 197 "lrparser.y"
                                                   { (yyval.node)=Util::finishBlock();}
#line 2010 "y.tab.c"
    break;

  case 33: /* batch_blockstmt: T_BATCH '(' id ':' expression ')' blockstatements  */
#line 199 "lrparser.y"
                                                                    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));}
#line 2016 "y.tab.c"
    break;

  case 34: /* on_add_blockstmt: T_ONADD '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 201 "lrparser.y"
                                                                                {(yyval.node) = Util::createOnAddBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2022 "y.tab.c"
    break;

  case 35: /* on_delete_blockstmt: T_ONDELETE '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 203 "lrparser.y"
                                                                                      {(yyval.node) = Util::createOnDeleteBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2028 "y.tab.c"
    break;

  case 36: /* block_begin: '{'  */
#line 205 "lrparser.y"
                { Util::createNewBlock(); }
#line 2034 "y.tab.c"
    break;

  case 38: /* return_stmt: T_RETURN expression  */
#line 209 "lrparser.y"
                                  {(yyval.node) = Util::createReturnStatementNode((yyvsp[0].node));}
#line 2040 "y.tab.c"
    break;

  case 39: /* declaration: type1 id  */
#line 212 "lrparser.y"
                         {
	                     Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 2053 "y.tab.c"
    break;

  case 40: /* declaration: type1 id '=' rhs  */
#line 220 "lrparser.y"
                            {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2061 "y.tab.c"
    break;

  case 41: /* declaration: type2 id  */
#line 223 "lrparser.y"
                    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 2069 "y.tab.c"
    break;

  case 42: /* declaration: type2 id '=' rhs  */
#line 226 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2077 "y.tab.c"
    break;

  case 43: /* type1: primitive  */
#line 230 "lrparser.y"
                 {(yyval.node)=(yyvsp[0].node); }
#line 2083 "y.tab.c"
    break;

  case 44: /* type1: graph  */
#line 231 "lrparser.y"
                {(yyval.node)=(yyvsp[0].node);}
#line 2089 "y.tab.c"
    break;

  case 45: /* type1: collections  */
#line 232 "lrparser.y"
                      { (yyval.node)=(yyvsp[0].node);}
#line 2095 "y.tab.c"
    break;

  case 46: /* primitive: T_INT  */
#line 235 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
#line 2101 "y.tab.c"
    break;

  case 47: /* primitive: T_FLOAT  */
#line 236 "lrparser.y"
                  { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
#line 2107 "y.tab.c"
    break;

  case 48: /* primitive: T_BOOL  */
#line 237 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
#line 2113 "y.tab.c"
    break;

  case 49: /* primitive: T_DOUBLE  */
#line 238 "lrparser.y"
                   { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 2119 "y.tab.c"
    break;

  case 50: /* primitive: T_LONG  */
#line 239 "lrparser.y"
             {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
#line 2125 "y.tab.c"
    break;

  case 51: /* graph: T_GRAPH  */
#line 242 "lrparser.y"
                { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 2131 "y.tab.c"
    break;

  case 52: /* graph: T_DIR_GRAPH  */
#line 243 "lrparser.y"
                     {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 2137 "y.tab.c"
    break;

  case 53: /* collections: T_LIST  */
#line 245 "lrparser.y"
                     { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
#line 2143 "y.tab.c"
    break;

  case 54: /* collections: T_SET_NODES '<' id '>'  */
#line 246 "lrparser.y"
                                         {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].node));}
#line 2150 "y.tab.c"
    break;

  case 55: /* collections: T_SET_EDGES '<' id '>'  */
#line 248 "lrparser.y"
                                 {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].node));}
#line 2157 "y.tab.c"
    break;

  case 56: /* collections: T_UPDATES '<' id '>'  */
#line 250 "lrparser.y"
                                         { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[-1].node));}
#line 2163 "y.tab.c"
    break;

  case 57: /* collections: container  */
#line 251 "lrparser.y"
                        {(yyval.node) = (yyvsp[0].node);}
#line 2169 "y.tab.c"
    break;

  case 58: /* collections: nodemap  */
#line 252 "lrparser.y"
                            {(yyval.node) = (yyvsp[0].node);}
#line 2175 "y.tab.c"
    break;

  case 59: /* container: T_CONTAINER '<' type '>' '(' arg_list ',' type ')'  */
#line 254 "lrparser.y"
                                                               {(yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2181 "y.tab.c"
    break;

  case 60: /* container: T_CONTAINER '<' type '>' '(' arg_list ')'  */
#line 255 "lrparser.y"
                                                      { (yyval.node) =  Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2187 "y.tab.c"
    break;

  case 61: /* container: T_CONTAINER '<' type '>'  */
#line 256 "lrparser.y"
                                     { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-1].node), argList, NULL);}
#line 2194 "y.tab.c"
    break;

  case 62: /* nodemap: T_NODEMAP '(' type ')'  */
#line 259 "lrparser.y"
                                 {(yyval.node) = Util::createNodeMapTypeNode(TYPE_NODEMAP, (yyvsp[-1].node));}
#line 2200 "y.tab.c"
    break;

  case 63: /* type2: T_NODE  */
#line 261 "lrparser.y"
               {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
#line 2206 "y.tab.c"
    break;

  case 64: /* type2: T_EDGE  */
#line 262 "lrparser.y"
                {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
#line 2212 "y.tab.c"
    break;

  case 65: /* type2: property  */
#line 263 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 2218 "y.tab.c"
    break;

  case 66: /* property: T_NP '<' primitive '>'  */
#line 265 "lrparser.y"
                                  { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2224 "y.tab.c"
    break;

  case 67: /* property: T_EP '<' primitive '>'  */
#line 266 "lrparser.y"
                                       { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node)); }
#line 2230 "y.tab.c"
    break;

  case 68: /* property: T_NP '<' collections '>'  */
#line 267 "lrparser.y"
                                                    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2236 "y.tab.c"
    break;

  case 69: /* property: T_EP '<' collections '>'  */
#line 268 "lrparser.y"
                                                     {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node));}
#line 2242 "y.tab.c"
    break;

  case 70: /* property: T_NP '<' T_NODE '>'  */
#line 269 "lrparser.y"
                                    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_NODE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2249 "y.tab.c"
    break;

  case 71: /* property: T_NP '<' T_EDGE '>'  */
#line 271 "lrparser.y"
                                                {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_EDGE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2256 "y.tab.c"
    break;

  case 72: /* assignment: leftSide '=' rhs  */
#line 274 "lrparser.y"
                                { printf("testassign\n");(yyval.node)=Util::createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2262 "y.tab.c"
    break;

  case 73: /* assignment: indexExpr '=' rhs  */
#line 275 "lrparser.y"
                                  {printf ("called assign for count\n") ; (yyval.node)=Util::createAssignmentNode((yyvsp[-2].node) , (yyvsp[0].node));}
#line 2268 "y.tab.c"
    break;

  case 74: /* rhs: expression  */
#line 278 "lrparser.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 2274 "y.tab.c"
    break;

  case 75: /* expression: proc_call  */
#line 280 "lrparser.y"
                       { (yyval.node)=(yyvsp[0].node);}
#line 2280 "y.tab.c"
    break;

  case 76: /* expression: expression '+' expression  */
#line 281 "lrparser.y"
                                         { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 2286 "y.tab.c"
    break;

  case 77: /* expression: expression '-' expression  */
#line 282 "lrparser.y"
                                             { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 2292 "y.tab.c"
    break;

  case 78: /* expression: expression '*' expression  */
#line 283 "lrparser.y"
                                             {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 2298 "y.tab.c"
    break;

  case 79: /* expression: expression '/' expression  */
#line 284 "lrparser.y"
                                           {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 2304 "y.tab.c"
    break;

  case 80: /* expression: expression T_AND_OP expression  */
#line 285 "lrparser.y"
                                              {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 2310 "y.tab.c"
    break;

  case 81: /* expression: expression T_OR_OP expression  */
#line 286 "lrparser.y"
                                                  {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 2316 "y.tab.c"
    break;

  case 82: /* expression: expression T_LE_OP expression  */
#line 287 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 2322 "y.tab.c"
    break;

  case 83: /* expression: expression T_GE_OP expression  */
#line 288 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 2328 "y.tab.c"
    break;

  case 84: /* expression: expression '<' expression  */
#line 289 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 2334 "y.tab.c"
    break;

  case 85: /* expression: expression '>' expression  */
#line 290 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 2340 "y.tab.c"
    break;

  case 86: /* expression: expression T_EQ_OP expression  */
#line 291 "lrparser.y"
                                                        {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2346 "y.tab.c"
    break;

  case 87: /* expression: expression T_NE_OP expression  */
#line 292 "lrparser.y"
                                            {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2352 "y.tab.c"
    break;

  case 88: /* expression: '!' expression  */
#line 293 "lrparser.y"
                                         {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[0].node),OPERATOR_NOT);}
#line 2358 "y.tab.c"
    break;

  case 89: /* expression: '(' expression ')'  */
#line 294 "lrparser.y"
                                          { Expression* expr=(Expression*)(yyvsp[-1].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
#line 2366 "y.tab.c"
    break;

  case 90: /* expression: val  */
#line 297 "lrparser.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2372 "y.tab.c"
    break;

  case 91: /* expression: leftSide  */
#line 298 "lrparser.y"
                                    { (yyval.node)=Util::createNodeForId((yyvsp[0].node));}
#line 2378 "y.tab.c"
    break;

  case 92: /* expression: unary_expr  */
#line 299 "lrparser.y"
                                      {(yyval.node)=(yyvsp[0].node);}
#line 2384 "y.tab.c"
    break;

  case 93: /* expression: indexExpr  */
#line 300 "lrparser.y"
                                     {(yyval.node) = (yyvsp[0].node);}
#line 2390 "y.tab.c"
    break;

  case 94: /* indexExpr: expression '[' expression ']'  */
#line 302 "lrparser.y"
                                          {printf("first done this \n");(yyval.node) = Util::createNodeForIndexExpr((yyvsp[-3].node), (yyvsp[-1].node), OPERATOR_INDEX);}
#line 2396 "y.tab.c"
    break;

  case 95: /* unary_expr: expression T_INC_OP  */
#line 304 "lrparser.y"
                                   {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_INC);}
#line 2402 "y.tab.c"
    break;

  case 96: /* unary_expr: expression T_DEC_OP  */
#line 305 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_DEC);}
#line 2408 "y.tab.c"
    break;

  case 97: /* proc_call: leftSide '(' arg_list ')'  */
#line 307 "lrparser.y"
                                      { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)->AList,NULL); 

									    }
#line 2418 "y.tab.c"
    break;

  case 98: /* proc_call: T_INCREMENTAL '(' arg_list ')'  */
#line 312 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2427 "y.tab.c"
    break;

  case 99: /* proc_call: T_DECREMENTAL '(' arg_list ')'  */
#line 316 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2436 "y.tab.c"
    break;

  case 100: /* proc_call: indexExpr '.' leftSide '(' arg_list ')'  */
#line 320 "lrparser.y"
                                                                  {
                                                   
													 Expression* expr = (Expression*)(yyvsp[-5].node);
                                                     (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node) , (yyvsp[-1].aList)->AList, expr); 

									                 }
#line 2447 "y.tab.c"
    break;

  case 101: /* val: INT_NUM  */
#line 331 "lrparser.y"
              { (yyval.node) = Util::createNodeForIval((yyvsp[0].ival)); }
#line 2453 "y.tab.c"
    break;

  case 102: /* val: FLOAT_NUM  */
#line 332 "lrparser.y"
                    {(yyval.node) = Util::createNodeForFval((yyvsp[0].fval));}
#line 2459 "y.tab.c"
    break;

  case 103: /* val: BOOL_VAL  */
#line 333 "lrparser.y"
                   { (yyval.node) = Util::createNodeForBval((yyvsp[0].bval));}
#line 2465 "y.tab.c"
    break;

  case 104: /* val: T_INF  */
#line 334 "lrparser.y"
                {(yyval.node)=Util::createNodeForINF(true);}
#line 2471 "y.tab.c"
    break;

  case 105: /* val: T_P_INF  */
#line 335 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(true);}
#line 2477 "y.tab.c"
    break;

  case 106: /* val: T_N_INF  */
#line 336 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(false);}
#line 2483 "y.tab.c"
    break;

  case 107: /* control_flow: selection_cf  */
#line 339 "lrparser.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2489 "y.tab.c"
    break;

  case 108: /* control_flow: iteration_cf  */
#line 340 "lrparser.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2495 "y.tab.c"
    break;

  case 109: /* iteration_cf: T_FIXEDPOINT T_UNTIL '(' id ':' expression ')' blockstatements  */
#line 342 "lrparser.y"
                                                                              { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2501 "y.tab.c"
    break;

  case 110: /* iteration_cf: T_WHILE '(' boolean_expr ')' blockstatements  */
#line 343 "lrparser.y"
                                                                 {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2507 "y.tab.c"
    break;

  case 111: /* iteration_cf: T_DO blockstatements T_WHILE '(' boolean_expr ')' ';'  */
#line 344 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[-2].node),(yyvsp[-5].node));  }
#line 2513 "y.tab.c"
    break;

  case 112: /* iteration_cf: T_FORALL '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 345 "lrparser.y"
                                                                                       { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2520 "y.tab.c"
    break;

  case 113: /* iteration_cf: T_FORALL '(' id T_IN leftSide ')' blockstatements  */
#line 347 "lrparser.y"
                                                                        { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2526 "y.tab.c"
    break;

  case 114: /* iteration_cf: T_FOR '(' id T_IN leftSide ')' blockstatements  */
#line 348 "lrparser.y"
                                                                 { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2532 "y.tab.c"
    break;

  case 115: /* iteration_cf: T_FOR '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 349 "lrparser.y"
                                                                                    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2538 "y.tab.c"
    break;

  case 116: /* iteration_cf: T_FOR '(' id T_IN indexExpr ')' blockstatements  */
#line 350 "lrparser.y"
                                                                  {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), false);}
#line 2544 "y.tab.c"
    break;

  case 117: /* iteration_cf: T_FORALL '(' id T_IN indexExpr ')' blockstatements  */
#line 351 "lrparser.y"
                                                                     {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), true);}
#line 2550 "y.tab.c"
    break;

  case 118: /* filterExpr: %empty  */
#line 353 "lrparser.y"
                      { (yyval.node)=NULL;}
#line 2556 "y.tab.c"
    break;

  case 119: /* filterExpr: '.' T_FILTER '(' boolean_expr ')'  */
#line 354 "lrparser.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2562 "y.tab.c"
    break;

  case 120: /* boolean_expr: expression  */
#line 356 "lrparser.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2568 "y.tab.c"
    break;

  case 121: /* selection_cf: T_IF '(' boolean_expr ')' statement  */
#line 358 "lrparser.y"
                                                   { (yyval.node)=Util::createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2574 "y.tab.c"
    break;

  case 122: /* selection_cf: T_IF '(' boolean_expr ')' statement T_ELSE statement  */
#line 359 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2580 "y.tab.c"
    break;

  case 123: /* reduction: leftSide '=' reductionCall  */
#line 362 "lrparser.y"
                                       { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) ;}
#line 2586 "y.tab.c"
    break;

  case 124: /* reduction: '<' leftList '>' '=' '<' reductionCall ',' rightList '>'  */
#line 363 "lrparser.y"
                                                                              { reductionCall* reduc=(reductionCall*)(yyvsp[-3].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[-7].nodeList)->ASTNList,reduc,(yyvsp[-1].nodeList)->ASTNList);}
#line 2593 "y.tab.c"
    break;

  case 125: /* reduction: leftSide reduce_op expression  */
#line 365 "lrparser.y"
                                                   {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2599 "y.tab.c"
    break;

  case 126: /* reduction: expression reduce_op expression  */
#line 366 "lrparser.y"
                                         {printf ("here calling creation for red op\n") ;(yyval.node)=Util::createNodeForReductionOpStmt ((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2605 "y.tab.c"
    break;

  case 127: /* reduce_op: T_ADD_ASSIGN  */
#line 369 "lrparser.y"
                         {(yyval.ival)=OPERATOR_ADDASSIGN;}
#line 2611 "y.tab.c"
    break;

  case 128: /* reduce_op: T_MUL_ASSIGN  */
#line 370 "lrparser.y"
                         {(yyval.ival)=OPERATOR_MULASSIGN;}
#line 2617 "y.tab.c"
    break;

  case 129: /* reduce_op: T_OR_ASSIGN  */
#line 371 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ORASSIGN;}
#line 2623 "y.tab.c"
    break;

  case 130: /* reduce_op: T_AND_ASSIGN  */
#line 372 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ANDASSIGN;}
#line 2629 "y.tab.c"
    break;

  case 131: /* reduce_op: T_SUB_ASSIGN  */
#line 373 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_SUBASSIGN;}
#line 2635 "y.tab.c"
    break;

  case 132: /* leftList: leftSide ',' leftList  */
#line 375 "lrparser.y"
                                  { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));
                                         }
#line 2642 "y.tab.c"
    break;

  case 133: /* leftList: leftSide  */
#line 377 "lrparser.y"
                           { (yyval.nodeList)=Util::createNList((yyvsp[0].node));;}
#line 2648 "y.tab.c"
    break;

  case 134: /* rightList: val ',' rightList  */
#line 379 "lrparser.y"
                              { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));}
#line 2654 "y.tab.c"
    break;

  case 135: /* rightList: leftSide ',' rightList  */
#line 380 "lrparser.y"
                                   { ASTNode* node = Util::createNodeForId((yyvsp[-2].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),node);}
#line 2661 "y.tab.c"
    break;

  case 136: /* rightList: val  */
#line 382 "lrparser.y"
                   { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2667 "y.tab.c"
    break;

  case 137: /* rightList: leftSide  */
#line 383 "lrparser.y"
                              { ASTNode* node = Util::createNodeForId((yyvsp[0].node));
			            (yyval.nodeList)=Util::createNList(node);}
#line 2674 "y.tab.c"
    break;

  case 138: /* reductionCall: reduction_calls '(' arg_list ')'  */
#line 392 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList)->AList);}
#line 2680 "y.tab.c"
    break;

  case 139: /* reduction_calls: T_SUM  */
#line 394 "lrparser.y"
                        { (yyval.ival)=REDUCE_SUM;}
#line 2686 "y.tab.c"
    break;

  case 140: /* reduction_calls: T_COUNT  */
#line 395 "lrparser.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2692 "y.tab.c"
    break;

  case 141: /* reduction_calls: T_PRODUCT  */
#line 396 "lrparser.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2698 "y.tab.c"
    break;

  case 142: /* reduction_calls: T_MAX  */
#line 397 "lrparser.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2704 "y.tab.c"
    break;

  case 143: /* reduction_calls: T_MIN  */
#line 398 "lrparser.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2710 "y.tab.c"
    break;

  case 144: /* leftSide: id  */
#line 400 "lrparser.y"
              { (yyval.node)=(yyvsp[0].node); }
#line 2716 "y.tab.c"
    break;

  case 145: /* leftSide: oid  */
#line 401 "lrparser.y"
               { printf("Here hello \n"); (yyval.node)=(yyvsp[0].node); }
#line 2722 "y.tab.c"
    break;

  case 146: /* leftSide: tid  */
#line 402 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node); }
#line 2728 "y.tab.c"
    break;

  case 147: /* leftSide: indexExpr  */
#line 403 "lrparser.y"
                    {(yyval.node)=(yyvsp[0].node);}
#line 2734 "y.tab.c"
    break;

  case 148: /* arg_list: %empty  */
#line 406 "lrparser.y"
              {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
#line 2742 "y.tab.c"
    break;

  case 149: /* arg_list: assignment ',' arg_list  */
#line 410 "lrparser.y"
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
#line 2762 "y.tab.c"
    break;

  case 150: /* arg_list: expression ',' arg_list  */
#line 427 "lrparser.y"
                                             {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[-2].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						                }
#line 2774 "y.tab.c"
    break;

  case 151: /* arg_list: expression  */
#line 434 "lrparser.y"
                            {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[0].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
#line 2784 "y.tab.c"
    break;

  case 152: /* arg_list: assignment  */
#line 439 "lrparser.y"
                            { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[0].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
#line 2795 "y.tab.c"
    break;

  case 153: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements reverse_abstraction  */
#line 447 "lrparser.y"
                                                                                                                 {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-10].node),(yyvsp[-8].node),(yyvsp[-6].node),(yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2801 "y.tab.c"
    break;

  case 154: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements  */
#line 448 "lrparser.y"
                                                                                                      {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-9].node),(yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node),NULL) ; }
#line 2807 "y.tab.c"
    break;

  case 155: /* reverse_abstraction: T_REVERSE blockstatements  */
#line 452 "lrparser.y"
                                                 {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[0].node));}
#line 2813 "y.tab.c"
    break;

  case 156: /* reverse_abstraction: T_REVERSE '(' boolean_expr ')' blockstatements  */
#line 453 "lrparser.y"
                                                                       {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2819 "y.tab.c"
    break;

  case 157: /* oid: id '.' id  */
#line 456 "lrparser.y"
                 { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node) = Util::createPropIdNode((yyvsp[-2].node),(yyvsp[0].node));
				    }
#line 2828 "y.tab.c"
    break;

  case 158: /* oid: id '.' id '[' id ']'  */
#line 460 "lrparser.y"
                                { ASTNode* expr1 = Util::createNodeForId((yyvsp[-3].node));
	                          ASTNode* expr2 = Util::createNodeForId((yyvsp[-1].node));
							  ASTNode* indexexpr =  Util::createNodeForIndexExpr(expr1, expr2, OPERATOR_INDEX);
	                          (yyval.node) = Util::createPropIdNode((yyvsp[-5].node) , indexexpr);}
#line 2837 "y.tab.c"
    break;

  case 159: /* tid: id '.' id '.' id  */
#line 467 "lrparser.y"
                       {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-4].node),(yyvsp[-2].node));
				    }
#line 2846 "y.tab.c"
    break;

  case 160: /* id: ID  */
#line 471 "lrparser.y"
          { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[0].text));  

            
            }
#line 2856 "y.tab.c"
    break;


#line 2860 "y.tab.c"

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

#line 479 "lrparser.y"



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
