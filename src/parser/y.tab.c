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
#line 1 "lrparser.y"

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
# define YYDEBUG 0
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
    T_HEAP = 325,                  /* T_HEAP  */
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
#define T_HEAP 325
#define ID 326
#define INT_NUM 327
#define FLOAT_NUM 328
#define BOOL_VAL 329
#define CHAR_VAL 330

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 38 "lrparser.y"

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
  YYSYMBOL_T_HEAP = 70,                    /* T_HEAP  */
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
  YYSYMBOL_heap = 118,                     /* heap  */
  YYSYMBOL_container = 119,                /* container  */
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
#define YYLAST   784

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  96
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  52
/* YYNRULES -- Number of rules.  */
#define YYNRULES  160
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  375

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
       0,   104,   104,   105,   107,   114,   120,   126,   132,   138,
     145,   146,   149,   150,   152,   163,   167,   176,   179,   180,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   196,   198,   200,   202,   204,   206,   208,   211,
     219,   222,   225,   229,   230,   231,   232,   235,   236,   237,
     238,   239,   242,   243,   245,   246,   248,   250,   251,   252,
     254,   256,   257,   258,   261,   263,   264,   265,   267,   268,
     269,   270,   271,   273,   276,   277,   279,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   291,   292,   293,
     294,   295,   298,   299,   300,   301,   303,   305,   306,   308,
     313,   317,   321,   332,   333,   334,   335,   336,   337,   340,
     341,   343,   344,   345,   346,   348,   349,   350,   351,   352,
     354,   355,   357,   359,   360,   363,   364,   366,   369,   370,
     371,   372,   373,   375,   377,   379,   380,   382,   383,   392,
     394,   395,   396,   397,   398,   400,   401,   402,   405,   409,
     426,   433,   438,   446,   447,   451,   452,   455,   459,   466,
     470
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
  "T_ONADD", "T_ONDELETE", "T_HEAP", "ID", "INT_NUM", "FLOAT_NUM",
  "BOOL_VAL", "CHAR_VAL", "'?'", "':'", "'<'", "'>'", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'('", "')'", "','", "';'", "'.'", "'{'", "'}'", "'='",
  "'!'", "'['", "']'", "$accept", "program", "function_def",
  "function_data", "paramList", "type", "param", "function_body",
  "statements", "statement", "blockstatements", "batch_blockstmt",
  "on_add_blockstmt", "on_delete_blockstmt", "block_begin", "block_end",
  "return_stmt", "declaration", "type1", "primitive", "graph",
  "collections", "heap", "container", "nodemap", "type2", "property",
  "assignment", "rhs", "expression", "indexExpr", "unary_expr",
  "proc_call", "val", "control_flow", "iteration_cf", "filterExpr",
  "boolean_expr", "selection_cf", "reduction", "reduce_op", "leftList",
  "rightList", "reductionCall", "reduction_calls", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-317)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-160)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -317,    20,  -317,    -5,   -83,     7,    -5,    -5,  -317,    12,
    -317,    37,   362,   362,    42,    44,  -317,  -317,  -317,  -317,
     362,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
      59,    64,    71,    66,    80,  -317,    85,    86,  -317,    63,
      78,    -5,  -317,  -317,  -317,  -317,  -317,  -317,    -5,  -317,
      82,   362,   362,   442,    83,    -5,   362,   362,   702,   318,
      -5,    -5,  -317,   362,  -317,    81,  -317,    84,    93,    87,
      95,  -317,  -317,  -317,    97,    98,    88,    12,   150,    99,
     101,   102,   104,   107,   109,  -317,  -317,  -317,    -5,    88,
    -317,    88,  -317,  -317,  -317,  -317,  -317,  -317,   111,   115,
      -5,    -5,   116,   619,   -60,   121,   122,  -317,  -317,  -317,
    -317,   123,    18,  -317,  -317,  -317,   108,  -317,   135,   139,
    -317,  -317,   140,   148,   153,   155,   156,   157,   169,   171,
     172,  -317,    -5,  -317,  -317,    -5,    -5,    88,    88,   619,
     174,  -317,  -317,   144,   228,   175,    -5,    88,    88,    -5,
      -5,    -5,   182,   167,   487,   619,  -317,  -317,   183,   184,
    -317,  -317,  -317,    88,    88,    88,    88,    88,    88,    88,
      88,    88,    88,    88,    88,    88,    -5,    88,  -317,  -317,
    -317,  -317,  -317,  -317,  -317,  -317,    88,    16,    88,    -5,
    -317,   192,  -317,  -317,  -317,  -317,  -317,  -317,  -317,  -317,
    -317,   198,   275,   278,   619,   212,   213,   220,    -5,   281,
     221,   583,   -69,   223,   225,   235,   294,   295,   224,    -5,
    -317,    88,    88,   690,   637,    94,    94,   159,   159,    94,
      94,    -1,    -1,     3,     3,   251,   230,  -317,   619,   231,
    -317,  -317,  -317,  -317,  -317,  -317,  -317,   233,   619,   -45,
      88,  -317,    88,    88,   538,    12,    88,   243,    -5,    88,
      88,    88,  -317,  -317,    88,    -5,    -5,   248,  -317,  -317,
    -317,  -317,    88,  -317,    88,    -5,    -5,   -36,     5,    -8,
     238,    22,    21,   239,   320,  -317,   250,    88,   258,  -317,
    -317,   601,   259,   260,    29,   253,   264,  -317,   242,  -317,
     362,    12,    12,    88,    12,    12,    88,   538,   263,   610,
      88,    12,    88,    88,   267,  -317,  -317,  -317,   269,  -317,
    -317,    46,    34,  -317,  -317,    46,  -317,  -317,    12,   297,
    -317,   272,   274,    43,  -317,   339,   287,    -5,   288,  -317,
      -5,   314,   316,   307,   317,   308,   312,    12,    52,    12,
     313,    12,    12,    43,  -317,    43,    88,  -317,  -317,   309,
    -317,  -317,  -317,  -317,   319,    12,  -317,   338,   -18,  -317,
      88,  -317,   321,    12,  -317
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     160,     0,     0,     0,     0,     0,    36,     4,    17,    18,
       0,    47,    48,    49,    50,    51,    52,    53,    65,    66,
       0,     0,     0,     0,     0,    54,     0,     0,    60,     0,
      10,     0,    43,    44,    45,    46,    58,    59,     0,    67,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     7,     0,    14,    15,     8,     0,     0,     0,
       0,   107,   106,   108,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   103,   104,   105,     0,     0,
      37,     0,    19,    26,    29,    30,    31,    32,     0,     0,
       0,     0,     0,     0,    95,    94,    77,    92,    23,   110,
     109,     0,    93,    25,   146,   147,   145,     5,     0,     0,
      12,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    11,     0,     6,     9,     0,     0,     0,     0,    38,
      95,    94,    77,    93,     0,     0,     0,   148,   148,     0,
       0,     0,     0,   134,     0,    90,    28,    20,    39,    41,
      21,    97,    98,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    27,    22,
      24,   128,   132,   129,   131,   130,   148,     0,     0,     0,
      57,    63,    64,    72,    73,    68,    70,    69,    71,    55,
      56,     0,     0,     0,   122,     0,     0,     0,     0,     0,
     152,   151,    93,     0,     0,     0,     0,     0,     0,     0,
      91,     0,     0,    82,    83,    84,    85,    88,    89,    86,
      87,    78,    79,    80,    81,     0,     0,    75,    76,     0,
     140,   141,   142,   143,   144,    74,   125,     0,   127,   157,
     148,    16,     0,     0,     0,     0,     0,     0,     0,   148,
     148,     0,   100,   101,     0,     0,     0,     0,   133,    40,
      42,    96,   148,    99,   148,     0,     0,     0,    95,    93,
     145,    95,    93,   145,   123,   112,     0,     0,     0,   149,
     150,     0,     0,     0,     0,     0,     0,   159,     0,    62,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   102,   139,   158,     0,   119,
     115,    77,   145,   118,   116,    77,   124,   113,     0,    77,
      33,    77,    77,     0,    61,     0,     0,     0,     0,   111,
       0,     0,     0,   137,     0,   138,     0,     0,   157,     0,
       0,     0,     0,     0,   126,     0,     0,   114,   117,   120,
      34,    35,   135,   136,     0,     0,   121,   154,     0,   153,
       0,   155,     0,     0,   156
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -317,  -317,  -317,  -317,   165,   -51,  -317,  -317,  -317,  -251,
      -9,  -317,  -317,  -317,  -317,  -317,  -317,  -317,   -48,    54,
    -317,    89,  -317,  -317,  -317,   -35,  -317,   -49,  -152,   215,
     -29,   -41,   -39,  -316,  -317,  -317,  -312,  -123,  -317,  -317,
    -317,   185,  -315,   112,  -317,   -52,  -129,  -317,  -317,  -317,
    -317,     4
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     1,     8,     9,    39,   119,    40,    17,    53,    92,
      93,    94,    95,    96,    19,    97,    98,    99,    41,    42,
      43,    44,    45,    46,    47,    48,    49,   210,   245,   103,
     140,   141,   142,   107,   108,   109,   336,   205,   110,   111,
     188,   152,   344,   246,   247,   143,   213,   113,   369,   114,
     115,   116
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,   112,    12,   284,   102,   100,   122,    11,   120,   120,
      14,    15,   105,   338,   106,   206,   186,   343,   101,   214,
       2,   121,   121,   261,   104,   237,    71,    72,    73,   176,
     161,   162,   177,     3,   161,   162,   153,   343,   362,   343,
     363,   181,   182,   183,   275,    64,   184,   365,   185,   276,
     299,   300,    65,    71,    72,    73,   326,   239,   240,   118,
     241,   242,   243,   244,   129,   130,    10,   370,   144,   269,
     270,   240,    16,   241,   242,   243,   244,   186,   302,    80,
      81,   173,   174,     4,     5,     6,     7,    10,    85,    86,
      87,   301,    13,   175,   176,   212,   212,   175,    71,    72,
      73,    89,    16,   186,   158,   159,   186,   305,   304,    91,
     187,   176,   125,   127,    10,    85,    86,    87,   104,   104,
    -157,   277,    20,   337,   236,   161,   162,    51,   276,    52,
     289,   290,  -120,   286,   212,   335,   201,    55,  -159,   202,
     203,   275,    56,   295,    58,   296,   276,   126,   128,    62,
     209,    80,    81,   215,   216,   217,    57,   104,    59,    10,
      85,    86,    87,    60,    61,    63,   132,   153,    66,   117,
     133,   145,   135,    89,   171,   172,   173,   174,    50,   134,
     136,    91,   137,   138,   146,    54,   147,   148,   175,   149,
     161,   162,   150,   249,   151,   165,   166,   189,   212,   156,
     279,   282,   112,   157,   160,   102,   100,   212,   212,   178,
     179,   180,   257,   105,   190,   106,    67,    68,   191,   101,
     212,   104,   212,   278,   281,   104,   192,   193,   131,   186,
     104,   104,   194,   364,   195,   196,   197,   169,   170,   171,
     172,   173,   174,   104,   207,   104,   285,   372,   198,   318,
     199,   200,   120,   175,   219,   112,   280,   283,   102,   100,
     208,   218,   288,   176,   321,   121,   105,   325,   106,   292,
     293,   329,   101,   331,   332,   221,   222,   250,   104,   297,
     298,   345,   161,   162,   251,   163,   164,   165,   166,   167,
     168,   139,   319,   320,   252,   323,   324,   253,   254,   255,
     258,   345,   330,   345,   154,   256,   155,   322,   259,   262,
     322,   263,   264,   265,   266,   272,   267,   273,   274,   339,
     287,    21,    22,    23,    24,    25,   294,   303,   306,   169,
     170,   171,   172,   173,   174,   307,   308,   317,   357,   315,
     358,   348,   360,   361,   350,   175,   271,   310,   312,   313,
     316,   327,   204,   204,   333,   334,   367,   340,   341,   371,
     342,   346,   211,   211,   374,    21,    22,    23,    24,    25,
      30,    31,    32,   347,   349,    35,    36,    37,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   351,   238,   352,   353,   355,   354,   356,   335,   359,
     368,   211,   238,   248,   268,   366,   314,   373,     0,     0,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,     0,     0,     0,   238,   238,     0,     0,
       0,     0,     0,     0,     0,    21,    22,    23,    24,    25,
      69,    70,    71,    72,    73,     0,    74,     0,    75,    76,
      77,     0,    78,     0,     0,   211,     0,     0,     0,     0,
       0,   204,     0,     0,   211,   211,   238,     0,     0,   291,
       0,     0,     0,     0,     0,     0,     0,   211,     0,   211,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,   309,    79,     0,    80,    81,     0,     0,    82,
      83,    84,    38,    10,    85,    86,    87,     0,   161,   162,
      88,   163,   164,   165,   166,   167,   168,    89,     0,     0,
       0,     0,    16,    90,     0,    91,     0,     0,     0,     0,
       0,    21,    22,    23,    24,    25,    69,    70,    71,    72,
      73,     0,    74,     0,    75,    76,    77,     0,    78,     0,
       0,     0,     0,     0,     0,   169,   170,   171,   172,   173,
     174,   204,     0,   220,     0,     0,     0,     0,     0,     0,
       0,   175,     0,     0,     0,   204,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     0,    79,
       0,    80,    81,     0,     0,    82,    83,    84,    38,    10,
      85,    86,    87,     0,   161,   162,    88,   163,   164,   165,
     166,   167,   168,    89,     0,     0,     0,     0,    16,     0,
       0,    91,   161,   162,     0,   163,   164,   165,   166,   167,
     168,   161,   162,     0,   163,   164,   165,   166,   167,   168,
     161,   162,     0,   163,   164,   165,   166,   167,   168,     0,
       0,   169,   170,   171,   172,   173,   174,     0,   161,   162,
     260,   163,     0,   165,   166,   167,   168,   175,     0,   169,
     170,   171,   172,   173,   174,     0,     0,   311,   169,   170,
     171,   172,   173,   174,     0,   175,   328,   169,   170,   171,
     172,   173,   174,     0,   175,    21,    22,    23,    24,    25,
       0,     0,     0,   175,     0,   169,   170,   171,   172,   173,
     174,   161,   162,     0,     0,     0,   165,   166,   167,   168,
       0,   175,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   123,   124,    30,    31,    32,     0,     0,    35,
      36,    37,     0,     0,     0,     0,     0,     0,   169,   170,
     171,   172,   173,   174,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175
};

static const yytype_int16 yycheck[] =
{
       9,    53,    85,   254,    53,    53,    57,     3,    56,    57,
       6,     7,    53,   325,    53,   138,    85,   333,    53,   148,
       0,    56,    57,    92,    53,   177,    10,    11,    12,    89,
      31,    32,    92,    13,    31,    32,    88,   353,   353,   355,
     355,    23,    24,    25,    89,    41,    28,   359,    30,    94,
      86,    87,    48,    10,    11,    12,   307,   186,    42,    55,
      44,    45,    46,    47,    60,    61,    71,    85,    77,   221,
     222,    42,    90,    44,    45,    46,    47,    85,    86,    63,
      64,    82,    83,    63,    64,    65,    66,    71,    72,    73,
      74,    86,    85,    94,    89,   147,   148,    94,    10,    11,
      12,    85,    90,    85,   100,   101,    85,    86,    86,    93,
      92,    89,    58,    59,    71,    72,    73,    74,   147,   148,
      86,   250,    85,    89,   176,    31,    32,    85,    94,    85,
     259,   260,    86,   256,   186,    89,   132,    78,    86,   135,
     136,    89,    78,   272,    78,   274,    94,    58,    59,    86,
     146,    63,    64,   149,   150,   151,    85,   186,    78,    71,
      72,    73,    74,    78,    78,    87,    85,   219,    86,    86,
      86,    21,    85,    85,    80,    81,    82,    83,    13,    86,
      85,    93,    85,    85,    85,    20,    85,    85,    94,    85,
      31,    32,    85,   189,    85,    36,    37,    89,   250,    88,
     252,   253,   254,    88,    88,   254,   254,   259,   260,    88,
      88,    88,   208,   254,    79,   254,    51,    52,    79,   254,
     272,   250,   274,   252,   253,   254,    86,    79,    63,    85,
     259,   260,    79,   356,    79,    79,    79,    78,    79,    80,
      81,    82,    83,   272,    16,   274,   255,   370,    79,   300,
      79,    79,   300,    94,    87,   307,   252,   253,   307,   307,
      85,    79,   258,    89,   303,   300,   307,   306,   307,   265,
     266,   310,   307,   312,   313,    92,    92,    85,   307,   275,
     276,   333,    31,    32,    86,    34,    35,    36,    37,    38,
      39,    76,   301,   302,    19,   304,   305,    19,    86,    86,
      19,   353,   311,   355,    89,    85,    91,   303,    87,    86,
     306,    86,    77,    19,    19,    85,    92,    86,    85,   328,
      77,     3,     4,     5,     6,     7,    78,    89,    89,    78,
      79,    80,    81,    82,    83,    15,    86,    95,   347,    86,
     349,   337,   351,   352,   340,    94,    95,    89,    89,    89,
      86,    88,   137,   138,    87,    86,   365,    60,    86,   368,
      86,    22,   147,   148,   373,     3,     4,     5,     6,     7,
      52,    53,    54,    86,    86,    57,    58,    59,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,    77,   177,    77,    87,    87,    79,    85,    89,    86,
      62,   186,   187,   188,   219,    86,   294,    86,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    70,    -1,    -1,    -1,   221,   222,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    14,    -1,    16,    17,
      18,    -1,    20,    -1,    -1,   250,    -1,    -1,    -1,    -1,
      -1,   256,    -1,    -1,   259,   260,   261,    -1,    -1,   264,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   272,    -1,   274,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,   287,    61,    -1,    63,    64,    -1,    -1,    67,
      68,    69,    70,    71,    72,    73,    74,    -1,    31,    32,
      78,    34,    35,    36,    37,    38,    39,    85,    -1,    -1,
      -1,    -1,    90,    91,    -1,    93,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    -1,    14,    -1,    16,    17,    18,    -1,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    78,    79,    80,    81,    82,
      83,   356,    -1,    86,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    94,    -1,    -1,    -1,   370,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    61,
      -1,    63,    64,    -1,    -1,    67,    68,    69,    70,    71,
      72,    73,    74,    -1,    31,    32,    78,    34,    35,    36,
      37,    38,    39,    85,    -1,    -1,    -1,    -1,    90,    -1,
      -1,    93,    31,    32,    -1,    34,    35,    36,    37,    38,
      39,    31,    32,    -1,    34,    35,    36,    37,    38,    39,
      31,    32,    -1,    34,    35,    36,    37,    38,    39,    -1,
      -1,    78,    79,    80,    81,    82,    83,    -1,    31,    32,
      87,    34,    -1,    36,    37,    38,    39,    94,    -1,    78,
      79,    80,    81,    82,    83,    -1,    -1,    86,    78,    79,
      80,    81,    82,    83,    -1,    94,    86,    78,    79,    80,
      81,    82,    83,    -1,    94,     3,     4,     5,     6,     7,
      -1,    -1,    -1,    94,    -1,    78,    79,    80,    81,    82,
      83,    31,    32,    -1,    -1,    -1,    36,    37,    38,    39,
      -1,    94,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    -1,    -1,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    78,    79,
      80,    81,    82,    83,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    94
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    97,     0,    13,    63,    64,    65,    66,    98,    99,
      71,   147,    85,    85,   147,   147,    90,   103,   106,   110,
      85,     3,     4,     5,     6,     7,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    70,   100,
     102,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     100,    85,    85,   104,   100,    78,    78,    85,    78,    78,
      78,    78,    86,    87,   147,   147,    86,   100,   100,     8,
       9,    10,    11,    12,    14,    16,    17,    18,    20,    61,
      63,    64,    67,    68,    69,    72,    73,    74,    78,    85,
      91,    93,   105,   106,   107,   108,   109,   111,   112,   113,
     114,   121,   123,   125,   126,   127,   128,   129,   130,   131,
     134,   135,   141,   143,   145,   146,   147,    86,   147,   101,
     114,   121,   101,    50,    51,   115,   117,   115,   117,   147,
     147,   100,    85,    86,    86,    85,    85,    85,    85,   125,
     126,   127,   128,   141,   106,    21,    85,    85,    85,    85,
      85,    85,   137,   141,   125,   125,    88,    88,   147,   147,
      88,    31,    32,    34,    35,    36,    37,    38,    39,    78,
      79,    80,    81,    82,    83,    94,    89,    92,    88,    88,
      88,    23,    24,    25,    28,    30,    85,    92,   136,    89,
      79,    79,    86,    79,    79,    79,    79,    79,    79,    79,
      79,   147,   147,   147,   125,   133,   133,    16,    85,   147,
     123,   125,   141,   142,   142,   147,   147,   147,    79,    87,
      86,    92,    92,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   141,   124,   125,   142,
      42,    44,    45,    46,    47,   124,   139,   140,   125,   147,
      85,    86,    19,    19,    86,    86,    85,   147,    19,    87,
      87,    92,    86,    86,    77,    19,    19,    92,   137,   124,
     124,    95,    85,    86,    85,    89,    94,   142,   126,   141,
     147,   126,   141,   147,   105,   106,   133,    77,   147,   142,
     142,   125,   147,   147,    78,   142,   142,   147,   147,    86,
      87,    86,    86,    89,    86,    86,    89,    15,    86,   125,
      89,    86,    89,    89,   139,    86,    86,    95,   101,   106,
     106,   128,   147,   106,   106,   128,   105,    88,    86,   128,
     106,   128,   128,    87,    86,    89,   132,    89,   132,   106,
      60,    86,    86,   129,   138,   141,    22,    86,   147,    86,
     147,    77,    77,    87,    79,    87,    85,   106,   106,    86,
     106,   106,   138,   138,   133,   132,    86,   106,    62,   144,
      85,   106,   133,    86,   106
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    96,    97,    97,    98,    99,    99,    99,    99,    99,
     100,   100,   101,   101,   102,   102,   102,   103,   104,   104,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     113,   113,   113,   114,   114,   114,   114,   115,   115,   115,
     115,   115,   116,   116,   117,   117,   117,   117,   117,   117,
     118,   119,   119,   119,   120,   121,   121,   121,   122,   122,
     122,   122,   122,   122,   123,   123,   124,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   126,   127,   127,   128,
     128,   128,   128,   129,   129,   129,   129,   129,   129,   130,
     130,   131,   131,   131,   131,   131,   131,   131,   131,   131,
     132,   132,   133,   134,   134,   135,   135,   135,   136,   136,
     136,   136,   136,   137,   137,   138,   138,   138,   138,   139,
     140,   140,   140,   140,   140,   141,   141,   141,   142,   142,
     142,   142,   142,   143,   143,   144,   144,   145,   145,   146,
     147
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     1,     1,     2,     2,     5,     1,     0,     2,
       2,     2,     2,     1,     2,     1,     1,     2,     2,     1,
       1,     1,     3,     7,    10,    10,     1,     1,     2,     2,
       4,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     4,     1,     1,
       1,     9,     7,     4,     4,     1,     1,     1,     4,     4,
       4,     4,     4,     4,     3,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     1,     1,     1,     1,     4,     2,     2,     4,
       4,     4,     6,     1,     1,     1,     1,     1,     1,     1,
       1,     8,     5,     7,    10,     7,     7,    10,     7,     7,
       0,     5,     1,     5,     7,     3,     9,     3,     1,     1,
       1,     1,     1,     3,     1,     3,     3,     1,     1,     4,
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
#line 105 "lrparser.y"
                               {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
#line 1792 "y.tab.c"
    break;

  case 4: /* function_def: function_data function_body  */
#line 107 "lrparser.y"
                                            { 
	                                          Function* func=(Function*)(yyvsp[-1].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[0].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
#line 1803 "y.tab.c"
    break;

  case 5: /* function_data: T_FUNC id '(' paramList ')'  */
#line 114 "lrparser.y"
                                           { 
										   (yyval.node)=Util::createFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
#line 1814 "y.tab.c"
    break;

  case 6: /* function_data: T_STATIC id '(' paramList ')'  */
#line 120 "lrparser.y"
                                                           { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1825 "y.tab.c"
    break;

  case 7: /* function_data: T_INCREMENTAL '(' paramList ')'  */
#line 126 "lrparser.y"
                                                     { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1836 "y.tab.c"
    break;

  case 8: /* function_data: T_DECREMENTAL '(' paramList ')'  */
#line 132 "lrparser.y"
                                                             { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1847 "y.tab.c"
    break;

  case 9: /* function_data: T_DYNAMIC id '(' paramList ')'  */
#line 138 "lrparser.y"
                                                        { (yyval.node)=Util::createDynamicFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
#line 1857 "y.tab.c"
    break;

  case 10: /* paramList: param  */
#line 145 "lrparser.y"
                 {(yyval.pList)=Util::createPList((yyvsp[0].node));}
#line 1863 "y.tab.c"
    break;

  case 11: /* paramList: param ',' paramList  */
#line 146 "lrparser.y"
                                     {(yyval.pList)=Util::addToPList((yyvsp[0].pList),(yyvsp[-2].node)); 
			                           }
#line 1870 "y.tab.c"
    break;

  case 12: /* type: type1  */
#line 149 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1876 "y.tab.c"
    break;

  case 13: /* type: type2  */
#line 150 "lrparser.y"
            {(yyval.node) = (yyvsp[0].node);}
#line 1882 "y.tab.c"
    break;

  case 14: /* param: type1 id  */
#line 152 "lrparser.y"
                 {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 1898 "y.tab.c"
    break;

  case 15: /* param: type2 id  */
#line 163 "lrparser.y"
                          { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 1907 "y.tab.c"
    break;

  case 16: /* param: type2 id '(' id ')'  */
#line 167 "lrparser.y"
                                                 { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[-4].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[-1].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[-3].node));
									 }
#line 1919 "y.tab.c"
    break;

  case 17: /* function_body: blockstatements  */
#line 176 "lrparser.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 1925 "y.tab.c"
    break;

  case 18: /* statements: %empty  */
#line 179 "lrparser.y"
              {}
#line 1931 "y.tab.c"
    break;

  case 19: /* statements: statements statement  */
#line 180 "lrparser.y"
                               { Util::addToBlock((yyvsp[0].node)); }
#line 1937 "y.tab.c"
    break;

  case 20: /* statement: declaration ';'  */
#line 182 "lrparser.y"
                          {(yyval.node)=(yyvsp[-1].node);}
#line 1943 "y.tab.c"
    break;

  case 21: /* statement: assignment ';'  */
#line 183 "lrparser.y"
                       {(yyval.node)=(yyvsp[-1].node);}
#line 1949 "y.tab.c"
    break;

  case 22: /* statement: proc_call ';'  */
#line 184 "lrparser.y"
                       {(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[-1].node));}
#line 1955 "y.tab.c"
    break;

  case 23: /* statement: control_flow  */
#line 185 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 1961 "y.tab.c"
    break;

  case 24: /* statement: reduction ';'  */
#line 186 "lrparser.y"
                      {(yyval.node)=(yyvsp[-1].node);}
#line 1967 "y.tab.c"
    break;

  case 25: /* statement: bfs_abstraction  */
#line 187 "lrparser.y"
                          {(yyval.node)=(yyvsp[0].node); }
#line 1973 "y.tab.c"
    break;

  case 26: /* statement: blockstatements  */
#line 188 "lrparser.y"
                          {(yyval.node)=(yyvsp[0].node);}
#line 1979 "y.tab.c"
    break;

  case 27: /* statement: unary_expr ';'  */
#line 189 "lrparser.y"
                         {(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[-1].node));}
#line 1985 "y.tab.c"
    break;

  case 28: /* statement: return_stmt ';'  */
#line 190 "lrparser.y"
                          {(yyval.node) = (yyvsp[-1].node) ;}
#line 1991 "y.tab.c"
    break;

  case 29: /* statement: batch_blockstmt  */
#line 191 "lrparser.y"
                           {(yyval.node) = (yyvsp[0].node);}
#line 1997 "y.tab.c"
    break;

  case 30: /* statement: on_add_blockstmt  */
#line 192 "lrparser.y"
                           {(yyval.node) = (yyvsp[0].node);}
#line 2003 "y.tab.c"
    break;

  case 31: /* statement: on_delete_blockstmt  */
#line 193 "lrparser.y"
                              {(yyval.node) = (yyvsp[0].node);}
#line 2009 "y.tab.c"
    break;

  case 32: /* blockstatements: block_begin statements block_end  */
#line 196 "lrparser.y"
                                                   { (yyval.node)=Util::finishBlock();}
#line 2015 "y.tab.c"
    break;

  case 33: /* batch_blockstmt: T_BATCH '(' id ':' expression ')' blockstatements  */
#line 198 "lrparser.y"
                                                                    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));}
#line 2021 "y.tab.c"
    break;

  case 34: /* on_add_blockstmt: T_ONADD '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 200 "lrparser.y"
                                                                                {(yyval.node) = Util::createOnAddBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2027 "y.tab.c"
    break;

  case 35: /* on_delete_blockstmt: T_ONDELETE '(' id T_IN id '.' proc_call ')' ':' blockstatements  */
#line 202 "lrparser.y"
                                                                                      {(yyval.node) = Util::createOnDeleteBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2033 "y.tab.c"
    break;

  case 36: /* block_begin: '{'  */
#line 204 "lrparser.y"
                { Util::createNewBlock(); }
#line 2039 "y.tab.c"
    break;

  case 38: /* return_stmt: T_RETURN expression  */
#line 208 "lrparser.y"
                                  {(yyval.node) = Util::createReturnStatementNode((yyvsp[0].node));}
#line 2045 "y.tab.c"
    break;

  case 39: /* declaration: type1 id  */
#line 211 "lrparser.y"
                         {
	                     Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 2058 "y.tab.c"
    break;

  case 40: /* declaration: type1 id '=' rhs  */
#line 219 "lrparser.y"
                            {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2066 "y.tab.c"
    break;

  case 41: /* declaration: type2 id  */
#line 222 "lrparser.y"
                    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 2074 "y.tab.c"
    break;

  case 42: /* declaration: type2 id '=' rhs  */
#line 225 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2082 "y.tab.c"
    break;

  case 43: /* type1: primitive  */
#line 229 "lrparser.y"
                 {(yyval.node)=(yyvsp[0].node); }
#line 2088 "y.tab.c"
    break;

  case 44: /* type1: graph  */
#line 230 "lrparser.y"
                {(yyval.node)=(yyvsp[0].node);}
#line 2094 "y.tab.c"
    break;

  case 45: /* type1: collections  */
#line 231 "lrparser.y"
                      { (yyval.node)=(yyvsp[0].node);}
#line 2100 "y.tab.c"
    break;

  case 46: /* type1: heap  */
#line 232 "lrparser.y"
               { (yyval.node)=(yyvsp[0].node);}
#line 2106 "y.tab.c"
    break;

  case 47: /* primitive: T_INT  */
#line 235 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
#line 2112 "y.tab.c"
    break;

  case 48: /* primitive: T_FLOAT  */
#line 236 "lrparser.y"
                  { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
#line 2118 "y.tab.c"
    break;

  case 49: /* primitive: T_BOOL  */
#line 237 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
#line 2124 "y.tab.c"
    break;

  case 50: /* primitive: T_DOUBLE  */
#line 238 "lrparser.y"
                   { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 2130 "y.tab.c"
    break;

  case 51: /* primitive: T_LONG  */
#line 239 "lrparser.y"
             {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
#line 2136 "y.tab.c"
    break;

  case 52: /* graph: T_GRAPH  */
#line 242 "lrparser.y"
                { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 2142 "y.tab.c"
    break;

  case 53: /* graph: T_DIR_GRAPH  */
#line 243 "lrparser.y"
                     {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 2148 "y.tab.c"
    break;

  case 54: /* collections: T_LIST  */
#line 245 "lrparser.y"
                     { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
#line 2154 "y.tab.c"
    break;

  case 55: /* collections: T_SET_NODES '<' id '>'  */
#line 246 "lrparser.y"
                                         {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].node));}
#line 2161 "y.tab.c"
    break;

  case 56: /* collections: T_SET_EDGES '<' id '>'  */
#line 248 "lrparser.y"
                                 {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].node));}
#line 2168 "y.tab.c"
    break;

  case 57: /* collections: T_UPDATES '<' id '>'  */
#line 250 "lrparser.y"
                                         { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[-1].node));}
#line 2174 "y.tab.c"
    break;

  case 58: /* collections: container  */
#line 251 "lrparser.y"
                        {(yyval.node) = (yyvsp[0].node);}
#line 2180 "y.tab.c"
    break;

  case 59: /* collections: nodemap  */
#line 252 "lrparser.y"
                            {(yyval.node) = (yyvsp[0].node);}
#line 2186 "y.tab.c"
    break;

  case 60: /* heap: T_HEAP  */
#line 254 "lrparser.y"
              { (yyval.node)=Util::createHeapTypeNode(TYPE_HEAP);}
#line 2192 "y.tab.c"
    break;

  case 61: /* container: T_CONTAINER '<' type '>' '(' arg_list ',' type ')'  */
#line 256 "lrparser.y"
                                                               {(yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-6].node), (yyvsp[-3].aList)->AList, (yyvsp[-1].node));}
#line 2198 "y.tab.c"
    break;

  case 62: /* container: T_CONTAINER '<' type '>' '(' arg_list ')'  */
#line 257 "lrparser.y"
                                                      { (yyval.node) =  Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-4].node), (yyvsp[-1].aList)->AList, NULL);}
#line 2204 "y.tab.c"
    break;

  case 63: /* container: T_CONTAINER '<' type '>'  */
#line 258 "lrparser.y"
                                     { list<argument*> argList;
			                          (yyval.node) = Util::createContainerTypeNode(TYPE_CONTAINER, (yyvsp[-1].node), argList, NULL);}
#line 2211 "y.tab.c"
    break;

  case 64: /* nodemap: T_NODEMAP '(' type ')'  */
#line 261 "lrparser.y"
                                 {(yyval.node) = Util::createNodeMapTypeNode(TYPE_NODEMAP, (yyvsp[-1].node));}
#line 2217 "y.tab.c"
    break;

  case 65: /* type2: T_NODE  */
#line 263 "lrparser.y"
               {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
#line 2223 "y.tab.c"
    break;

  case 66: /* type2: T_EDGE  */
#line 264 "lrparser.y"
                {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
#line 2229 "y.tab.c"
    break;

  case 67: /* type2: property  */
#line 265 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 2235 "y.tab.c"
    break;

  case 68: /* property: T_NP '<' primitive '>'  */
#line 267 "lrparser.y"
                                  { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2241 "y.tab.c"
    break;

  case 69: /* property: T_EP '<' primitive '>'  */
#line 268 "lrparser.y"
                                       { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node)); }
#line 2247 "y.tab.c"
    break;

  case 70: /* property: T_NP '<' collections '>'  */
#line 269 "lrparser.y"
                                                    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2253 "y.tab.c"
    break;

  case 71: /* property: T_EP '<' collections '>'  */
#line 270 "lrparser.y"
                                                     {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node));}
#line 2259 "y.tab.c"
    break;

  case 72: /* property: T_NP '<' T_NODE '>'  */
#line 271 "lrparser.y"
                                    {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_NODE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2266 "y.tab.c"
    break;

  case 73: /* property: T_NP '<' T_EDGE '>'  */
#line 273 "lrparser.y"
                                                {ASTNode* type = Util::createNodeEdgeTypeNode(TYPE_EDGE);
			                         (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE, type); }
#line 2273 "y.tab.c"
    break;

  case 74: /* assignment: leftSide '=' rhs  */
#line 276 "lrparser.y"
                                { printf("testassign\n");(yyval.node)=Util::createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2279 "y.tab.c"
    break;

  case 75: /* assignment: indexExpr '=' rhs  */
#line 277 "lrparser.y"
                                  { (yyval.node)=Util::createAssignmentNode((yyvsp[-2].node) , (yyvsp[0].node));}
#line 2285 "y.tab.c"
    break;

  case 76: /* rhs: expression  */
#line 279 "lrparser.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 2291 "y.tab.c"
    break;

  case 77: /* expression: proc_call  */
#line 281 "lrparser.y"
                       { (yyval.node)=(yyvsp[0].node);}
#line 2297 "y.tab.c"
    break;

  case 78: /* expression: expression '+' expression  */
#line 282 "lrparser.y"
                                         { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 2303 "y.tab.c"
    break;

  case 79: /* expression: expression '-' expression  */
#line 283 "lrparser.y"
                                             { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 2309 "y.tab.c"
    break;

  case 80: /* expression: expression '*' expression  */
#line 284 "lrparser.y"
                                             {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 2315 "y.tab.c"
    break;

  case 81: /* expression: expression '/' expression  */
#line 285 "lrparser.y"
                                           {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 2321 "y.tab.c"
    break;

  case 82: /* expression: expression T_AND_OP expression  */
#line 286 "lrparser.y"
                                              {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 2327 "y.tab.c"
    break;

  case 83: /* expression: expression T_OR_OP expression  */
#line 287 "lrparser.y"
                                                  {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 2333 "y.tab.c"
    break;

  case 84: /* expression: expression T_LE_OP expression  */
#line 288 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 2339 "y.tab.c"
    break;

  case 85: /* expression: expression T_GE_OP expression  */
#line 289 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 2345 "y.tab.c"
    break;

  case 86: /* expression: expression '<' expression  */
#line 290 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 2351 "y.tab.c"
    break;

  case 87: /* expression: expression '>' expression  */
#line 291 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 2357 "y.tab.c"
    break;

  case 88: /* expression: expression T_EQ_OP expression  */
#line 292 "lrparser.y"
                                                        {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2363 "y.tab.c"
    break;

  case 89: /* expression: expression T_NE_OP expression  */
#line 293 "lrparser.y"
                                            {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2369 "y.tab.c"
    break;

  case 90: /* expression: '!' expression  */
#line 294 "lrparser.y"
                                         {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[0].node),OPERATOR_NOT);}
#line 2375 "y.tab.c"
    break;

  case 91: /* expression: '(' expression ')'  */
#line 295 "lrparser.y"
                                          { Expression* expr=(Expression*)(yyvsp[-1].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
#line 2383 "y.tab.c"
    break;

  case 92: /* expression: val  */
#line 298 "lrparser.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2389 "y.tab.c"
    break;

  case 93: /* expression: leftSide  */
#line 299 "lrparser.y"
                                    { (yyval.node)=Util::createNodeForId((yyvsp[0].node));}
#line 2395 "y.tab.c"
    break;

  case 94: /* expression: unary_expr  */
#line 300 "lrparser.y"
                                      {(yyval.node)=(yyvsp[0].node);}
#line 2401 "y.tab.c"
    break;

  case 95: /* expression: indexExpr  */
#line 301 "lrparser.y"
                                     {(yyval.node) = (yyvsp[0].node);}
#line 2407 "y.tab.c"
    break;

  case 96: /* indexExpr: expression '[' expression ']'  */
#line 303 "lrparser.y"
                                          {printf("first done this \n");(yyval.node) = Util::createNodeForIndexExpr((yyvsp[-3].node), (yyvsp[-1].node), OPERATOR_INDEX);}
#line 2413 "y.tab.c"
    break;

  case 97: /* unary_expr: expression T_INC_OP  */
#line 305 "lrparser.y"
                                   {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_INC);}
#line 2419 "y.tab.c"
    break;

  case 98: /* unary_expr: expression T_DEC_OP  */
#line 306 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_DEC);}
#line 2425 "y.tab.c"
    break;

  case 99: /* proc_call: leftSide '(' arg_list ')'  */
#line 308 "lrparser.y"
                                      { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)->AList,NULL); 

									    }
#line 2435 "y.tab.c"
    break;

  case 100: /* proc_call: T_INCREMENTAL '(' arg_list ')'  */
#line 313 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2444 "y.tab.c"
    break;

  case 101: /* proc_call: T_DECREMENTAL '(' arg_list ')'  */
#line 317 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList,NULL); 

				                               }
#line 2453 "y.tab.c"
    break;

  case 102: /* proc_call: indexExpr '.' leftSide '(' arg_list ')'  */
#line 321 "lrparser.y"
                                                                  {
                                                   
													 Expression* expr = (Expression*)(yyvsp[-5].node);
                                                     (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node) , (yyvsp[-1].aList)->AList, expr); 

									                 }
#line 2464 "y.tab.c"
    break;

  case 103: /* val: INT_NUM  */
#line 332 "lrparser.y"
              { (yyval.node) = Util::createNodeForIval((yyvsp[0].ival)); }
#line 2470 "y.tab.c"
    break;

  case 104: /* val: FLOAT_NUM  */
#line 333 "lrparser.y"
                    {(yyval.node) = Util::createNodeForFval((yyvsp[0].fval));}
#line 2476 "y.tab.c"
    break;

  case 105: /* val: BOOL_VAL  */
#line 334 "lrparser.y"
                   { (yyval.node) = Util::createNodeForBval((yyvsp[0].bval));}
#line 2482 "y.tab.c"
    break;

  case 106: /* val: T_INF  */
#line 335 "lrparser.y"
                {(yyval.node)=Util::createNodeForINF(true);}
#line 2488 "y.tab.c"
    break;

  case 107: /* val: T_P_INF  */
#line 336 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(true);}
#line 2494 "y.tab.c"
    break;

  case 108: /* val: T_N_INF  */
#line 337 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(false);}
#line 2500 "y.tab.c"
    break;

  case 109: /* control_flow: selection_cf  */
#line 340 "lrparser.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2506 "y.tab.c"
    break;

  case 110: /* control_flow: iteration_cf  */
#line 341 "lrparser.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2512 "y.tab.c"
    break;

  case 111: /* iteration_cf: T_FIXEDPOINT T_UNTIL '(' id ':' expression ')' blockstatements  */
#line 343 "lrparser.y"
                                                                              { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2518 "y.tab.c"
    break;

  case 112: /* iteration_cf: T_WHILE '(' boolean_expr ')' blockstatements  */
#line 344 "lrparser.y"
                                                                 {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2524 "y.tab.c"
    break;

  case 113: /* iteration_cf: T_DO blockstatements T_WHILE '(' boolean_expr ')' ';'  */
#line 345 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[-2].node),(yyvsp[-5].node));  }
#line 2530 "y.tab.c"
    break;

  case 114: /* iteration_cf: T_FORALL '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 346 "lrparser.y"
                                                                                       { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2537 "y.tab.c"
    break;

  case 115: /* iteration_cf: T_FORALL '(' id T_IN leftSide ')' blockstatements  */
#line 348 "lrparser.y"
                                                                        { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2543 "y.tab.c"
    break;

  case 116: /* iteration_cf: T_FOR '(' id T_IN leftSide ')' blockstatements  */
#line 349 "lrparser.y"
                                                                 { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2549 "y.tab.c"
    break;

  case 117: /* iteration_cf: T_FOR '(' id T_IN id '.' proc_call filterExpr ')' blockstatements  */
#line 350 "lrparser.y"
                                                                                    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2555 "y.tab.c"
    break;

  case 118: /* iteration_cf: T_FOR '(' id T_IN indexExpr ')' blockstatements  */
#line 351 "lrparser.y"
                                                                  {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), false);}
#line 2561 "y.tab.c"
    break;

  case 119: /* iteration_cf: T_FORALL '(' id T_IN indexExpr ')' blockstatements  */
#line 352 "lrparser.y"
                                                                     {(yyval.node) = Util::createNodeForForStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node), true);}
#line 2567 "y.tab.c"
    break;

  case 120: /* filterExpr: %empty  */
#line 354 "lrparser.y"
                      { (yyval.node)=NULL;}
#line 2573 "y.tab.c"
    break;

  case 121: /* filterExpr: '.' T_FILTER '(' boolean_expr ')'  */
#line 355 "lrparser.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2579 "y.tab.c"
    break;

  case 122: /* boolean_expr: expression  */
#line 357 "lrparser.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2585 "y.tab.c"
    break;

  case 123: /* selection_cf: T_IF '(' boolean_expr ')' statement  */
#line 359 "lrparser.y"
                                                   { (yyval.node)=Util::createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2591 "y.tab.c"
    break;

  case 124: /* selection_cf: T_IF '(' boolean_expr ')' statement T_ELSE statement  */
#line 360 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2597 "y.tab.c"
    break;

  case 125: /* reduction: leftSide '=' reductionCall  */
#line 363 "lrparser.y"
                                       { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) ;}
#line 2603 "y.tab.c"
    break;

  case 126: /* reduction: '<' leftList '>' '=' '<' reductionCall ',' rightList '>'  */
#line 364 "lrparser.y"
                                                                              { reductionCall* reduc=(reductionCall*)(yyvsp[-3].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[-7].nodeList)->ASTNList,reduc,(yyvsp[-1].nodeList)->ASTNList);}
#line 2610 "y.tab.c"
    break;

  case 127: /* reduction: leftSide reduce_op expression  */
#line 366 "lrparser.y"
                                                   {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2616 "y.tab.c"
    break;

  case 128: /* reduce_op: T_ADD_ASSIGN  */
#line 369 "lrparser.y"
                         {(yyval.ival)=OPERATOR_ADDASSIGN;}
#line 2622 "y.tab.c"
    break;

  case 129: /* reduce_op: T_MUL_ASSIGN  */
#line 370 "lrparser.y"
                         {(yyval.ival)=OPERATOR_MULASSIGN;}
#line 2628 "y.tab.c"
    break;

  case 130: /* reduce_op: T_OR_ASSIGN  */
#line 371 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ORASSIGN;}
#line 2634 "y.tab.c"
    break;

  case 131: /* reduce_op: T_AND_ASSIGN  */
#line 372 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ANDASSIGN;}
#line 2640 "y.tab.c"
    break;

  case 132: /* reduce_op: T_SUB_ASSIGN  */
#line 373 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_SUBASSIGN;}
#line 2646 "y.tab.c"
    break;

  case 133: /* leftList: leftSide ',' leftList  */
#line 375 "lrparser.y"
                                  { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));
                                         }
#line 2653 "y.tab.c"
    break;

  case 134: /* leftList: leftSide  */
#line 377 "lrparser.y"
                            { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2659 "y.tab.c"
    break;

  case 135: /* rightList: val ',' rightList  */
#line 379 "lrparser.y"
                              { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));}
#line 2665 "y.tab.c"
    break;

  case 136: /* rightList: leftSide ',' rightList  */
#line 380 "lrparser.y"
                                   { ASTNode* node = Util::createNodeForId((yyvsp[-2].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),node);}
#line 2672 "y.tab.c"
    break;

  case 137: /* rightList: val  */
#line 382 "lrparser.y"
                   { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2678 "y.tab.c"
    break;

  case 138: /* rightList: leftSide  */
#line 383 "lrparser.y"
                              { ASTNode* node = Util::createNodeForId((yyvsp[0].node));
			            (yyval.nodeList)=Util::createNList(node);}
#line 2685 "y.tab.c"
    break;

  case 139: /* reductionCall: reduction_calls '(' arg_list ')'  */
#line 392 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList)->AList);}
#line 2691 "y.tab.c"
    break;

  case 140: /* reduction_calls: T_SUM  */
#line 394 "lrparser.y"
                        { (yyval.ival)=REDUCE_SUM;}
#line 2697 "y.tab.c"
    break;

  case 141: /* reduction_calls: T_COUNT  */
#line 395 "lrparser.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2703 "y.tab.c"
    break;

  case 142: /* reduction_calls: T_PRODUCT  */
#line 396 "lrparser.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2709 "y.tab.c"
    break;

  case 143: /* reduction_calls: T_MAX  */
#line 397 "lrparser.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2715 "y.tab.c"
    break;

  case 144: /* reduction_calls: T_MIN  */
#line 398 "lrparser.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2721 "y.tab.c"
    break;

  case 145: /* leftSide: id  */
#line 400 "lrparser.y"
              { (yyval.node)=(yyvsp[0].node); }
#line 2727 "y.tab.c"
    break;

  case 146: /* leftSide: oid  */
#line 401 "lrparser.y"
               { printf("Here hello \n"); (yyval.node)=(yyvsp[0].node); }
#line 2733 "y.tab.c"
    break;

  case 147: /* leftSide: tid  */
#line 402 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node); }
#line 2739 "y.tab.c"
    break;

  case 148: /* arg_list: %empty  */
#line 405 "lrparser.y"
              {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
#line 2747 "y.tab.c"
    break;

  case 149: /* arg_list: assignment ',' arg_list  */
#line 409 "lrparser.y"
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
#line 2767 "y.tab.c"
    break;

  case 150: /* arg_list: expression ',' arg_list  */
#line 426 "lrparser.y"
                                             {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[-2].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						                }
#line 2779 "y.tab.c"
    break;

  case 151: /* arg_list: expression  */
#line 433 "lrparser.y"
                            {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[0].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
#line 2789 "y.tab.c"
    break;

  case 152: /* arg_list: assignment  */
#line 438 "lrparser.y"
                            { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[0].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
#line 2800 "y.tab.c"
    break;

  case 153: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements reverse_abstraction  */
#line 446 "lrparser.y"
                                                                                                                 {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-10].node),(yyvsp[-8].node),(yyvsp[-6].node),(yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2806 "y.tab.c"
    break;

  case 154: /* bfs_abstraction: T_BFS '(' id T_IN id '.' proc_call T_FROM id ')' filterExpr blockstatements  */
#line 447 "lrparser.y"
                                                                                                      {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-9].node),(yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node),NULL) ; }
#line 2812 "y.tab.c"
    break;

  case 155: /* reverse_abstraction: T_REVERSE blockstatements  */
#line 451 "lrparser.y"
                                                 {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[0].node));}
#line 2818 "y.tab.c"
    break;

  case 156: /* reverse_abstraction: T_REVERSE '(' boolean_expr ')' blockstatements  */
#line 452 "lrparser.y"
                                                                       {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2824 "y.tab.c"
    break;

  case 157: /* oid: id '.' id  */
#line 455 "lrparser.y"
                 { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node) = Util::createPropIdNode((yyvsp[-2].node),(yyvsp[0].node));
				    }
#line 2833 "y.tab.c"
    break;

  case 158: /* oid: id '.' id '[' id ']'  */
#line 459 "lrparser.y"
                                { ASTNode* expr1 = Util::createNodeForId((yyvsp[-3].node));
	                          ASTNode* expr2 = Util::createNodeForId((yyvsp[-1].node));
							  ASTNode* indexexpr =  Util::createNodeForIndexExpr(expr1, expr2, OPERATOR_INDEX);
	                          (yyval.node) = Util::createPropIdNode((yyvsp[-5].node) , indexexpr);}
#line 2842 "y.tab.c"
    break;

  case 159: /* tid: id '.' id '.' id  */
#line 466 "lrparser.y"
                       {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-4].node),(yyvsp[-2].node));
				    }
#line 2851 "y.tab.c"
    break;

  case 160: /* id: ID  */
#line 470 "lrparser.y"
          { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[0].text));  

            
            }
#line 2861 "y.tab.c"
    break;


#line 2865 "y.tab.c"

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

#line 478 "lrparser.y"



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
