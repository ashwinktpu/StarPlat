
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "lrparser.y"

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
    #include "includeHeader.hpp"
	#include "../analyser/attachProp/attachPropAnalyser.h"
	#include "../analyser/dataRace/dataRaceAnalyser.h"
	#include "../analyser/deviceVars/deviceVarsAnalyser.h"
	#include<getopt.h>
	//#include "../symbolutil/SymbolTableBuilder.cpp"
     
	void yyerror(char *);
	int yylex(void);
    extern FILE* yyin;

	char mytext[100];
	char var[100];
	int num = 0;
	vector<map<int,vector<Identifier*>>> graphId(4);
	extern char *yytext;
	//extern SymbolTable* symbTab;
	FrontEndContext frontEndContext;
	char* backendTarget ;
    vector<Identifier*> tempIds; //stores graph vars in current function's param list.
    //symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());


/* Line 189 of yacc.c  */
#line 103 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_INT = 258,
     T_FLOAT = 259,
     T_BOOL = 260,
     T_DOUBLE = 261,
     T_LONG = 262,
     T_FORALL = 263,
     T_FOR = 264,
     T_P_INF = 265,
     T_INF = 266,
     T_N_INF = 267,
     T_FUNC = 268,
     T_IF = 269,
     T_ELSE = 270,
     T_WHILE = 271,
     T_RETURN = 272,
     T_DO = 273,
     T_IN = 274,
     T_FIXEDPOINT = 275,
     T_UNTIL = 276,
     T_FILTER = 277,
     T_ADD_ASSIGN = 278,
     T_SUB_ASSIGN = 279,
     T_MUL_ASSIGN = 280,
     T_DIV_ASSIGN = 281,
     T_MOD_ASSIGN = 282,
     T_AND_ASSIGN = 283,
     T_XOR_ASSIGN = 284,
     T_OR_ASSIGN = 285,
     T_RIGHT_OP = 286,
     T_LEFT_OP = 287,
     T_INC_OP = 288,
     T_DEC_OP = 289,
     T_PTR_OP = 290,
     T_AND_OP = 291,
     T_OR_OP = 292,
     T_LE_OP = 293,
     T_GE_OP = 294,
     T_EQ_OP = 295,
     T_NE_OP = 296,
     T_AND = 297,
     T_OR = 298,
     T_SUM = 299,
     T_AVG = 300,
     T_COUNT = 301,
     T_PRODUCT = 302,
     T_MAX = 303,
     T_MIN = 304,
     T_GRAPH = 305,
     T_DIR_GRAPH = 306,
     T_NODE = 307,
     T_EDGE = 308,
     T_UPDATES = 309,
     T_NP = 310,
     T_EP = 311,
     T_LIST = 312,
     T_SET_NODES = 313,
     T_SET_EDGES = 314,
     T_FROM = 315,
     T_BFS = 316,
     T_REVERSE = 317,
     T_INCREMENTAL = 318,
     T_DECREMENTAL = 319,
     T_STATIC = 320,
     ID = 321,
     INT_NUM = 322,
     FLOAT_NUM = 323,
     BOOL_VAL = 324
   };
#endif
/* Tokens.  */
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
#define T_RIGHT_OP 286
#define T_LEFT_OP 287
#define T_INC_OP 288
#define T_DEC_OP 289
#define T_PTR_OP 290
#define T_AND_OP 291
#define T_OR_OP 292
#define T_LE_OP 293
#define T_GE_OP 294
#define T_EQ_OP 295
#define T_NE_OP 296
#define T_AND 297
#define T_OR 298
#define T_SUM 299
#define T_AVG 300
#define T_COUNT 301
#define T_PRODUCT 302
#define T_MAX 303
#define T_MIN 304
#define T_GRAPH 305
#define T_DIR_GRAPH 306
#define T_NODE 307
#define T_EDGE 308
#define T_UPDATES 309
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
#define ID 321
#define INT_NUM 322
#define FLOAT_NUM 323
#define BOOL_VAL 324




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 33 "lrparser.y"

    int  info;
    long ival;
	bool bval;
    double fval;
    char* text;
	ASTNode* node;
	paramList* pList;
	argList* aList;
	ASTNodeList* nodeList;
    tempNode* temporary;
     


/* Line 214 of yacc.c  */
#line 292 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 304 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   557

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  88
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  132
/* YYNRULES -- Number of states.  */
#define YYNSTATES  288

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   324

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    86,     2,     2,     2,    78,     2,     2,
      79,    80,    76,    74,    81,    75,    87,    77,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    71,    82,
      72,    85,    73,    70,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    83,     2,    84,     2,     2,     2,     2,
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
      65,    66,    67,    68,    69
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,    10,    16,    22,    27,    32,
      34,    38,    41,    44,    50,    52,    53,    56,    59,    62,
      65,    67,    70,    72,    74,    77,    80,    84,    86,    88,
      91,    94,    99,   102,   107,   109,   111,   113,   115,   117,
     119,   121,   123,   125,   127,   129,   134,   139,   144,   146,
     148,   150,   155,   160,   165,   170,   174,   176,   178,   182,
     186,   190,   194,   198,   202,   206,   210,   214,   218,   222,
     226,   229,   233,   235,   237,   239,   242,   245,   250,   252,
     254,   256,   258,   260,   262,   264,   266,   275,   281,   289,
     300,   308,   316,   327,   328,   334,   336,   342,   350,   354,
     364,   368,   370,   372,   374,   376,   378,   382,   384,   388,
     392,   394,   396,   401,   403,   405,   407,   409,   411,   413,
     415,   417,   418,   422,   426,   428,   430,   444,   457,   460,
     466,   470,   476
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      89,     0,    -1,    -1,    89,    90,    -1,    91,    94,    -1,
      13,   131,    79,    92,    80,    -1,    65,   131,    79,    92,
      80,    -1,    63,    79,    92,    80,    -1,    64,    79,    92,
      80,    -1,    93,    -1,    93,    81,    92,    -1,   102,   131,
      -1,   106,   131,    -1,   106,   131,    79,   131,    80,    -1,
      97,    -1,    -1,    95,    96,    -1,   101,    82,    -1,   108,
      82,    -1,   112,    82,    -1,   114,    -1,   119,    82,    -1,
     127,    -1,    97,    -1,   111,    82,    -1,   100,    82,    -1,
      98,    95,    99,    -1,    83,    -1,    84,    -1,    17,   110,
      -1,   102,   131,    -1,   102,   131,    85,   109,    -1,   106,
     131,    -1,   106,   131,    85,   109,    -1,   103,    -1,   104,
      -1,   105,    -1,     3,    -1,     4,    -1,     5,    -1,     6,
      -1,     7,    -1,    50,    -1,    51,    -1,    57,    -1,    58,
      72,   131,    73,    -1,    59,    72,   131,    73,    -1,    54,
      72,   131,    73,    -1,    52,    -1,    53,    -1,   107,    -1,
      55,    72,   103,    73,    -1,    56,    72,   103,    73,    -1,
      55,    72,   105,    73,    -1,    56,    72,   105,    73,    -1,
     125,    85,   109,    -1,   110,    -1,   112,    -1,   110,    74,
     110,    -1,   110,    75,   110,    -1,   110,    76,   110,    -1,
     110,    77,   110,    -1,   110,    36,   110,    -1,   110,    37,
     110,    -1,   110,    38,   110,    -1,   110,    39,   110,    -1,
     110,    72,   110,    -1,   110,    73,   110,    -1,   110,    40,
     110,    -1,   110,    41,   110,    -1,    86,   110,    -1,    79,
     110,    80,    -1,   113,    -1,   125,    -1,   111,    -1,   110,
      33,    -1,   110,    34,    -1,   125,    79,   126,    80,    -1,
      67,    -1,    68,    -1,    69,    -1,    11,    -1,    10,    -1,
      12,    -1,   118,    -1,   115,    -1,    20,    21,    79,   131,
      71,   110,    80,    97,    -1,    16,    79,   117,    80,    97,
      -1,    18,    97,    16,    79,   117,    80,    82,    -1,     8,
      79,   131,    19,   131,    87,   112,   116,    80,    97,    -1,
       8,    79,   131,    19,   125,    80,    97,    -1,     9,    79,
     131,    19,   125,    80,    97,    -1,     9,    79,   131,    19,
     131,    87,   112,   116,    80,    97,    -1,    -1,    87,    22,
      79,   117,    80,    -1,   110,    -1,    14,    79,   117,    80,
      96,    -1,    14,    79,   117,    80,    96,    15,    96,    -1,
     125,    85,   123,    -1,    72,   121,    73,    85,    72,   123,
      81,   122,    73,    -1,   125,   120,   110,    -1,    23,    -1,
      25,    -1,    30,    -1,    28,    -1,    24,    -1,   125,    81,
     121,    -1,   125,    -1,   113,    81,   122,    -1,   125,    81,
     122,    -1,   113,    -1,   125,    -1,   124,    79,   126,    80,
      -1,    44,    -1,    46,    -1,    47,    -1,    48,    -1,    49,
      -1,   131,    -1,   129,    -1,   130,    -1,    -1,   108,    81,
     126,    -1,   110,    81,   126,    -1,   110,    -1,   108,    -1,
      61,    79,   131,    19,   131,    87,   112,    60,   131,    80,
     116,    97,   128,    -1,    61,    79,   131,    19,   131,    87,
     112,    60,   131,    80,   116,    97,    -1,    62,    97,    -1,
      62,    79,   117,    80,    97,    -1,   131,    87,   131,    -1,
     131,    87,   131,    87,   131,    -1,    66,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,    95,    95,    96,    98,   105,   111,   117,   123,   130,
     131,   134,   145,   149,   158,   161,   162,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   175,   177,   179,   181,
     184,   192,   195,   198,   202,   203,   204,   207,   208,   209,
     210,   211,   213,   214,   216,   217,   219,   221,   224,   225,
     226,   228,   229,   230,   231,   233,   235,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   254,   255,   256,   258,   259,   261,   270,   271,
     272,   273,   274,   275,   277,   278,   280,   281,   282,   283,
     285,   286,   287,   289,   290,   292,   294,   295,   298,   299,
     301,   304,   305,   306,   307,   308,   310,   312,   314,   315,
     317,   318,   327,   329,   330,   331,   332,   333,   335,   336,
     337,   339,   343,   358,   365,   370,   378,   379,   384,   385,
     388,   393,   397
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_INT", "T_FLOAT", "T_BOOL", "T_DOUBLE",
  "T_LONG", "T_FORALL", "T_FOR", "T_P_INF", "T_INF", "T_N_INF", "T_FUNC",
  "T_IF", "T_ELSE", "T_WHILE", "T_RETURN", "T_DO", "T_IN", "T_FIXEDPOINT",
  "T_UNTIL", "T_FILTER", "T_ADD_ASSIGN", "T_SUB_ASSIGN", "T_MUL_ASSIGN",
  "T_DIV_ASSIGN", "T_MOD_ASSIGN", "T_AND_ASSIGN", "T_XOR_ASSIGN",
  "T_OR_ASSIGN", "T_RIGHT_OP", "T_LEFT_OP", "T_INC_OP", "T_DEC_OP",
  "T_PTR_OP", "T_AND_OP", "T_OR_OP", "T_LE_OP", "T_GE_OP", "T_EQ_OP",
  "T_NE_OP", "T_AND", "T_OR", "T_SUM", "T_AVG", "T_COUNT", "T_PRODUCT",
  "T_MAX", "T_MIN", "T_GRAPH", "T_DIR_GRAPH", "T_NODE", "T_EDGE",
  "T_UPDATES", "T_NP", "T_EP", "T_LIST", "T_SET_NODES", "T_SET_EDGES",
  "T_FROM", "T_BFS", "T_REVERSE", "T_INCREMENTAL", "T_DECREMENTAL",
  "T_STATIC", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "'?'", "':'",
  "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'", "','",
  "';'", "'{'", "'}'", "'='", "'!'", "'.'", "$accept", "program",
  "function_def", "function_data", "paramList", "param", "function_body",
  "statements", "statement", "blockstatements", "block_begin", "block_end",
  "return_stmt", "declaration", "type1", "primitive", "graph",
  "collections", "type2", "property", "assignment", "rhs", "expression",
  "unary_expr", "proc_call", "val", "control_flow", "iteration_cf",
  "filterExpr", "boolean_expr", "selection_cf", "reduction", "reduce_op",
  "leftList", "rightList", "reductionCall", "reduction_calls", "leftSide",
  "arg_list", "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
      63,    58,    60,    62,    43,    45,    42,    47,    37,    40,
      41,    44,    59,   123,   125,    61,    33,    46
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    88,    89,    89,    90,    91,    91,    91,    91,    92,
      92,    93,    93,    93,    94,    95,    95,    96,    96,    96,
      96,    96,    96,    96,    96,    96,    97,    98,    99,   100,
     101,   101,   101,   101,   102,   102,   102,   103,   103,   103,
     103,   103,   104,   104,   105,   105,   105,   105,   106,   106,
     106,   107,   107,   107,   107,   108,   109,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   111,   111,   112,   113,   113,
     113,   113,   113,   113,   114,   114,   115,   115,   115,   115,
     115,   115,   115,   116,   116,   117,   118,   118,   119,   119,
     119,   120,   120,   120,   120,   120,   121,   121,   122,   122,
     122,   122,   123,   124,   124,   124,   124,   124,   125,   125,
     125,   126,   126,   126,   126,   126,   127,   127,   128,   128,
     129,   130,   131
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     1,
       3,     2,     2,     5,     1,     0,     2,     2,     2,     2,
       1,     2,     1,     1,     2,     2,     3,     1,     1,     2,
       2,     4,     2,     4,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     4,     4,     4,     1,     1,
       1,     4,     4,     4,     4,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     3,     1,     1,     1,     2,     2,     4,     1,     1,
       1,     1,     1,     1,     1,     1,     8,     5,     7,    10,
       7,     7,    10,     0,     5,     1,     5,     7,     3,     9,
       3,     1,     1,     1,     1,     1,     3,     1,     3,     3,
       1,     1,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     0,     3,     3,     1,     1,    13,    12,     2,     5,
       3,     5,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     3,     0,   132,
       0,     0,     0,     0,    27,     4,    14,    15,     0,    37,
      38,    39,    40,    41,    42,    43,    48,    49,     0,     0,
       0,    44,     0,     0,     0,     9,     0,    34,    35,    36,
       0,    50,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     7,     0,    11,    12,     8,     0,     0,     0,    82,
      81,    83,     0,     0,     0,     0,     0,     0,    78,    79,
      80,     0,     0,    28,     0,    16,    23,    26,     0,     0,
       0,     0,     0,     0,    74,    57,    72,    20,    85,    84,
       0,    73,    22,   119,   120,   118,     5,     0,     0,     0,
       0,     0,     0,     0,    10,     0,     6,     0,     0,     0,
       0,    29,    74,    57,    73,     0,     0,     0,     0,   107,
       0,    70,    25,    17,    30,    32,    18,    75,    76,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,    19,    21,   101,   105,   102,   104,   103,   121,
       0,     0,     0,    47,    51,    53,    52,    54,    45,    46,
       0,     0,     0,    95,     0,     0,     0,     0,     0,     0,
       0,    71,     0,     0,    62,    63,    64,    65,    68,    69,
      66,    67,    58,    59,    60,    61,   125,   124,    73,     0,
     113,   114,   115,   116,   117,    55,    56,    98,     0,   100,
     130,    13,     0,     0,     0,     0,     0,     0,     0,     0,
     106,    31,    33,   121,   121,     0,    77,   121,     0,     0,
     118,     0,   118,    96,    87,     0,     0,     0,     0,   122,
     123,     0,   131,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   112,    90,    93,     0,   118,    91,    93,    97,
      88,     0,     0,     0,     0,     0,     0,     0,    86,     0,
     110,     0,   111,     0,     0,   130,     0,     0,     0,    99,
       0,     0,    89,    92,    93,   108,   109,     0,     0,    94,
     127,     0,   126,     0,   128,     0,     0,   129
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     7,     8,    34,    35,    15,    44,    75,    76,
      17,    77,    78,    79,    36,    37,    38,    39,    40,    41,
     186,   195,   163,   112,   113,    86,    87,    88,   255,   164,
      89,    90,   151,   118,   261,   197,   198,   114,   189,    92,
     282,    93,    94,    95
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -236
static const yytype_int16 yypact[] =
{
    -236,     5,  -236,   -55,   -56,   -51,   -55,  -236,   -47,  -236,
     -20,    68,    68,    15,  -236,  -236,  -236,  -236,    68,  -236,
    -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,    23,    32,
      42,  -236,    45,    58,    21,    54,   -55,  -236,  -236,  -236,
     -55,  -236,    22,    68,   336,    56,   -55,    84,    84,   -55,
     -55,  -236,    68,  -236,    65,  -236,    57,    72,    77,  -236,
    -236,  -236,    78,    79,    30,   -47,   138,    88,  -236,  -236,
    -236,   -55,    30,  -236,    30,  -236,  -236,  -236,    82,    86,
     -55,   -55,    92,   228,    94,    96,  -236,  -236,  -236,  -236,
     102,     7,  -236,  -236,  -236,    83,  -236,   120,   131,   132,
     142,   144,   145,   146,  -236,   -55,  -236,   -55,   -55,    30,
      30,   228,  -236,  -236,   141,   205,   143,   -55,   151,   148,
     373,   228,  -236,  -236,   150,   152,  -236,  -236,  -236,    30,
      30,    30,    30,    30,    30,    30,    30,    30,    30,    30,
      30,  -236,  -236,  -236,  -236,  -236,  -236,  -236,  -236,    30,
      14,    30,   -55,  -236,  -236,  -236,  -236,  -236,  -236,  -236,
     156,   211,   219,   228,   160,   161,   163,   -55,   225,   162,
     -55,  -236,    30,    30,   114,   471,   106,   106,   480,   480,
     106,   106,   -26,   -26,    12,    12,   164,   243,   -52,   166,
    -236,  -236,  -236,  -236,  -236,  -236,   228,  -236,   170,   228,
     165,  -236,   -55,   -55,   420,   -47,    30,   179,   -55,   181,
    -236,  -236,  -236,    30,    30,    30,  -236,    30,   -55,   171,
     168,   180,   172,   248,  -236,   191,    30,   185,    85,  -236,
    -236,   194,  -236,   -47,   -55,   -47,   -55,   420,   193,   421,
     -55,   204,  -236,  -236,   199,   141,   -66,  -236,   199,  -236,
    -236,   -47,   227,    44,   284,   231,   -55,   232,  -236,   -55,
     226,   240,   233,   242,   -47,   -65,   -47,   245,    44,  -236,
      44,    30,  -236,  -236,   199,  -236,  -236,   246,   -47,  -236,
     260,   -63,  -236,    30,  -236,   247,   -47,  -236
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -236,  -236,  -236,  -236,   196,  -236,  -236,  -236,  -194,    -8,
    -236,  -236,  -236,  -236,   -41,    29,  -236,    37,   -38,  -236,
     -35,  -120,   159,   -25,   -27,  -204,  -236,  -236,  -235,   -98,
    -236,  -236,  -236,   153,  -203,   100,  -236,   -42,   -68,  -236,
    -236,  -236,  -236,    -2
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -132
static const yytype_int16 yytable[] =
{
      16,    10,    91,    80,    13,     2,    81,   127,   128,    82,
     223,     9,   165,   257,  -130,  -131,   283,    85,     3,    84,
      14,   256,   218,    11,    59,    60,    61,   149,    12,   119,
     144,   145,   146,   215,    53,   147,    14,   148,    54,   278,
      59,    60,    61,   249,    97,   127,   128,   102,   103,   260,
     139,   140,   211,   212,    59,    60,    61,   115,   190,    18,
     191,   192,   193,   194,   260,   275,   260,   276,     4,     5,
       6,    19,    20,    21,    22,    23,    98,   100,   124,   125,
       9,    68,    69,    70,    99,   101,   149,    19,    20,    21,
      22,    23,   150,    72,    43,    46,     9,    68,    69,    70,
      74,    51,    55,   160,    47,   161,   162,   188,   225,    72,
       9,    68,    69,    70,    48,   168,    74,    49,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,   119,   190,
      50,   191,   192,   193,   194,    52,    96,   106,    28,   127,
     128,    31,    32,    33,   105,   229,   230,   127,   128,   231,
     200,   107,   131,   132,   133,   134,   108,   109,   110,   116,
     219,   221,    91,    80,   122,   207,    81,   117,   123,    82,
     152,   188,   188,   277,   126,   188,   141,    85,   142,    84,
     137,   138,   139,   140,   143,   285,   135,   136,   137,   138,
     139,   140,   245,   153,   245,    91,    80,   224,   245,    81,
     220,   222,    82,    83,   154,   155,   227,   244,    42,   248,
      85,   262,    84,   252,    45,   156,   232,   157,   158,   159,
     149,   166,   167,   111,   169,   243,   262,   247,   262,   170,
     202,   120,   246,   121,   246,   172,   201,   173,   203,    56,
     204,   205,   206,   258,   208,   213,   216,   209,   104,   217,
     226,   233,   218,   228,   265,   234,   272,   267,   273,   236,
     235,   127,   128,   237,   129,   130,   131,   132,   133,   134,
     280,   238,   240,   284,   242,   250,   127,   128,   287,   129,
     130,   131,   132,   133,   134,   253,   254,   259,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     135,   136,   137,   138,   139,   140,   263,   268,   187,   196,
     199,   264,   266,   269,   270,   135,   136,   137,   138,   139,
     140,   271,   281,   210,   214,   274,   279,   286,   241,     0,
       0,   196,   196,     0,     0,     0,     0,     0,     0,    19,
      20,    21,    22,    23,    57,    58,    59,    60,    61,     0,
      62,     0,    63,    64,    65,     0,    66,     0,     0,     0,
       0,     0,     0,    83,     0,     0,     0,     0,     0,     0,
       0,     0,   187,   187,   196,     0,   187,     0,     0,     0,
       0,     0,     0,     0,     0,   239,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    83,    67,     0,     0,
       0,     0,     9,    68,    69,    70,   127,   128,    71,   129,
     130,   131,   132,   133,   134,    72,     0,     0,     0,    14,
      73,     0,    74,    19,    20,    21,    22,    23,    57,    58,
      59,    60,    61,     0,    62,     0,    63,    64,    65,     0,
      66,     0,     0,     0,     0,   135,   136,   137,   138,   139,
     140,     0,     0,   171,   127,   128,     0,   129,   130,   131,
     132,   133,   134,     0,     0,     0,     0,     0,     0,     0,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
       0,    67,     0,     0,     0,     0,     9,    68,    69,    70,
       0,     0,    71,   135,   136,   137,   138,   139,   140,    72,
       0,   251,     0,    14,   127,   128,    74,   129,     0,   131,
     132,   133,   134,   127,   128,     0,     0,     0,   131,   132,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   135,   136,   137,   138,   139,   140,     0,
       0,     0,   135,   136,   137,   138,   139,   140
};

static const yytype_int16 yycheck[] =
{
       8,     3,    44,    44,     6,     0,    44,    33,    34,    44,
     204,    66,   110,   248,    80,    80,    79,    44,    13,    44,
      83,    87,    87,    79,    10,    11,    12,    79,    79,    71,
      23,    24,    25,    85,    36,    28,    83,    30,    40,   274,
      10,    11,    12,   237,    46,    33,    34,    49,    50,   253,
      76,    77,   172,   173,    10,    11,    12,    65,    44,    79,
      46,    47,    48,    49,   268,   268,   270,   270,    63,    64,
      65,     3,     4,     5,     6,     7,    47,    48,    80,    81,
      66,    67,    68,    69,    47,    48,    79,     3,     4,     5,
       6,     7,    85,    79,    79,    72,    66,    67,    68,    69,
      86,    80,    80,   105,    72,   107,   108,   149,   206,    79,
      66,    67,    68,    69,    72,   117,    86,    72,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,   170,    44,
      72,    46,    47,    48,    49,    81,    80,    80,    54,    33,
      34,    57,    58,    59,    79,   213,   214,    33,    34,   217,
     152,    79,    38,    39,    40,    41,    79,    79,    79,    21,
     202,   203,   204,   204,    82,   167,   204,    79,    82,   204,
      87,   213,   214,   271,    82,   217,    82,   204,    82,   204,
      74,    75,    76,    77,    82,   283,    72,    73,    74,    75,
      76,    77,   234,    73,   236,   237,   237,   205,   240,   237,
     202,   203,   237,    44,    73,    73,   208,   234,    12,   236,
     237,   253,   237,   240,    18,    73,   218,    73,    73,    73,
      79,    16,    79,    64,    73,   233,   268,   235,   270,    81,
      19,    72,   234,    74,   236,    85,    80,    85,    19,    43,
      80,    80,    79,   251,    19,    81,    80,    85,    52,    79,
      71,    80,    87,    72,   256,    87,   264,   259,   266,    87,
      80,    33,    34,    15,    36,    37,    38,    39,    40,    41,
     278,    80,    87,   281,    80,    82,    33,    34,   286,    36,
      37,    38,    39,    40,    41,    81,    87,    60,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
      72,    73,    74,    75,    76,    77,    22,    81,   149,   150,
     151,    80,    80,    73,    81,    72,    73,    74,    75,    76,
      77,    79,    62,   170,    81,    80,    80,    80,   228,    -1,
      -1,   172,   173,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      14,    -1,    16,    17,    18,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,   204,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   213,   214,   215,    -1,   217,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   226,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,   237,    61,    -1,    -1,
      -1,    -1,    66,    67,    68,    69,    33,    34,    72,    36,
      37,    38,    39,    40,    41,    79,    -1,    -1,    -1,    83,
      84,    -1,    86,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    -1,    14,    -1,    16,    17,    18,    -1,
      20,    -1,    -1,    -1,    -1,    72,    73,    74,    75,    76,
      77,    -1,    -1,    80,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    -1,    -1,    -1,    -1,    66,    67,    68,    69,
      -1,    -1,    72,    72,    73,    74,    75,    76,    77,    79,
      -1,    80,    -1,    83,    33,    34,    86,    36,    -1,    38,
      39,    40,    41,    33,    34,    -1,    -1,    -1,    38,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    73,    74,    75,    76,    77,    -1,
      -1,    -1,    72,    73,    74,    75,    76,    77
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    89,     0,    13,    63,    64,    65,    90,    91,    66,
     131,    79,    79,   131,    83,    94,    97,    98,    79,     3,
       4,     5,     6,     7,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    92,    93,   102,   103,   104,   105,
     106,   107,    92,    79,    95,    92,    72,    72,    72,    72,
      72,    80,    81,   131,   131,    80,    92,     8,     9,    10,
      11,    12,    14,    16,    17,    18,    20,    61,    67,    68,
      69,    72,    79,    84,    86,    96,    97,    99,   100,   101,
     102,   106,   108,   110,   111,   112,   113,   114,   115,   118,
     119,   125,   127,   129,   130,   131,    80,   131,   103,   105,
     103,   105,   131,   131,    92,    79,    80,    79,    79,    79,
      79,   110,   111,   112,   125,    97,    21,    79,   121,   125,
     110,   110,    82,    82,   131,   131,    82,    33,    34,    36,
      37,    38,    39,    40,    41,    72,    73,    74,    75,    76,
      77,    82,    82,    82,    23,    24,    25,    28,    30,    79,
      85,   120,    87,    73,    73,    73,    73,    73,    73,    73,
     131,   131,   131,   110,   117,   117,    16,    79,   131,    73,
      81,    80,    85,    85,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   108,   110,   125,   126,
      44,    46,    47,    48,    49,   109,   110,   123,   124,   110,
     131,    80,    19,    19,    80,    80,    79,   131,    19,    85,
     121,   109,   109,    81,    81,    85,    80,    79,    87,   125,
     131,   125,   131,    96,    97,   117,    71,   131,    72,   126,
     126,   126,   131,    80,    87,    80,    87,    15,    80,   110,
      87,   123,    80,    97,   112,   125,   131,    97,   112,    96,
      82,    80,   112,    81,    87,   116,    87,   116,    97,    60,
     113,   122,   125,    22,    80,   131,    80,   131,    81,    73,
      81,    79,    97,    97,    80,   122,   122,   117,   116,    80,
      97,    62,   128,    79,    97,   117,    80,    97
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:

/* Line 1455 of yacc.c  */
#line 96 "lrparser.y"
    {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 98 "lrparser.y"
    { 
	                                          Function* func=(Function*)(yyvsp[(1) - (2)].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[(2) - (2)].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 105 "lrparser.y"
    { 
										   (yyval.node)=Util::createFuncNode((yyvsp[(2) - (5)].node),(yyvsp[(4) - (5)].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 111 "lrparser.y"
    { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[(2) - (5)].node),(yyvsp[(4) - (5)].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 117 "lrparser.y"
    { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[(3) - (4)].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 123 "lrparser.y"
    { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[(3) - (4)].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 130 "lrparser.y"
    {(yyval.pList)=Util::createPList((yyvsp[(1) - (1)].node));}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 131 "lrparser.y"
    {(yyval.pList)=Util::addToPList((yyvsp[(3) - (3)].pList),(yyvsp[(1) - (3)].node)); 
			                           }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 134 "lrparser.y"
    {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[(1) - (2)].node);
	                     Identifier* id=(Identifier*)(yyvsp[(2) - (2)].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)); }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 145 "lrparser.y"
    { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 149 "lrparser.y"
    { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[(1) - (5)].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[(4) - (5)].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[(2) - (5)].node));
									 }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 158 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 161 "lrparser.y"
    {}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 162 "lrparser.y"
    { Util::addToBlock((yyvsp[(2) - (2)].node)); }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 164 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 165 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 166 "lrparser.y"
    {(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[(1) - (2)].node));}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 167 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 168 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (2)].node);}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 169 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 170 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 171 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[(1) - (2)].node));}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 172 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (2)].node) ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 175 "lrparser.y"
    { (yyval.node)=Util::finishBlock();}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 177 "lrparser.y"
    { Util::createNewBlock(); }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 181 "lrparser.y"
    {(yyval.node) = Util::createReturnStatementNode((yyvsp[(2) - (2)].node));}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 184 "lrparser.y"
    {
	                     Type* type=(Type*)(yyvsp[(1) - (2)].node);
	                     Identifier* id=(Identifier*)(yyvsp[(2) - (2)].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node));}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 192 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[(1) - (4)].node),(yyvsp[(2) - (4)].node),(yyvsp[(4) - (4)].node));}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 195 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[(1) - (2)].node),(yyvsp[(2) - (2)].node)); }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 198 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[(1) - (4)].node),(yyvsp[(2) - (4)].node),(yyvsp[(4) - (4)].node));}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 202 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 203 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 204 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 207 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 208 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 209 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 210 "lrparser.y"
    { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 211 "lrparser.y"
    {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 213 "lrparser.y"
    { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 214 "lrparser.y"
    {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 216 "lrparser.y"
    { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 217 "lrparser.y"
    {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[(3) - (4)].node));}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 219 "lrparser.y"
    {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[(3) - (4)].node));}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 221 "lrparser.y"
    { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[(3) - (4)].node));}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 224 "lrparser.y"
    {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 225 "lrparser.y"
    {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 226 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 228 "lrparser.y"
    { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[(3) - (4)].node)); }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 229 "lrparser.y"
    { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[(3) - (4)].node)); }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 230 "lrparser.y"
    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[(3) - (4)].node)); }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 231 "lrparser.y"
    {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[(3) - (4)].node));}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 233 "lrparser.y"
    { (yyval.node)=Util::createAssignmentNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 235 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 237 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 238 "lrparser.y"
    { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_ADD);}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 239 "lrparser.y"
    { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_SUB);}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 240 "lrparser.y"
    {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_MUL);}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 241 "lrparser.y"
    {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_DIV);}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 242 "lrparser.y"
    {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_AND);}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 243 "lrparser.y"
    {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_OR);}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 244 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_LE);}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 245 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_GE);}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 246 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_LT);}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 247 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_GT);}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 248 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_EQ);}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 249 "lrparser.y"
    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node),OPERATOR_NE);}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 250 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(2) - (2)].node),OPERATOR_NOT);}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 251 "lrparser.y"
    { Expression* expr=(Expression*)(yyvsp[(2) - (3)].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 254 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 255 "lrparser.y"
    { (yyval.node)=Util::createNodeForId((yyvsp[(1) - (1)].node));}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 256 "lrparser.y"
    {(yyval.node)=(yyvsp[(1) - (1)].node);}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 258 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(1) - (2)].node),OPERATOR_INC);}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 259 "lrparser.y"
    {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[(1) - (2)].node),OPERATOR_DEC);}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 261 "lrparser.y"
    { 
                                       
                                       (yyval.node)=Util::createNodeForProcCall((yyvsp[(1) - (4)].node),(yyvsp[(3) - (4)].aList)->AList); 

									    }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 270 "lrparser.y"
    { (yyval.node)=Util::createNodeForIval((yyvsp[(1) - (1)].ival)); }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 271 "lrparser.y"
    {(yyval.node)=Util::createNodeForFval((yyvsp[(1) - (1)].fval));}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 272 "lrparser.y"
    { (yyval.node)=Util::createNodeForBval((yyvsp[(1) - (1)].bval));}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 273 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(true);}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 274 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(true);}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 275 "lrparser.y"
    {(yyval.node)=Util::createNodeForINF(false);}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 277 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 278 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 280 "lrparser.y"
    { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[(4) - (8)].node),(yyvsp[(6) - (8)].node),(yyvsp[(8) - (8)].node));}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 281 "lrparser.y"
    {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node)); }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 282 "lrparser.y"
    {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[(5) - (7)].node),(yyvsp[(2) - (7)].node));  }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 283 "lrparser.y"
    { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[(3) - (10)].node),(yyvsp[(5) - (10)].node),(yyvsp[(7) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].node),true);}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 285 "lrparser.y"
    { (yyval.node)=Util::createNodeForForStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node),true);}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 286 "lrparser.y"
    { (yyval.node)=Util::createNodeForForStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node),false);}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 287 "lrparser.y"
    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[(3) - (10)].node),(yyvsp[(5) - (10)].node),(yyvsp[(7) - (10)].node),(yyvsp[(8) - (10)].node),(yyvsp[(10) - (10)].node),false);}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 289 "lrparser.y"
    { (yyval.node)=NULL;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 290 "lrparser.y"
    { (yyval.node)=(yyvsp[(4) - (5)].node);}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 292 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node) ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 294 "lrparser.y"
    { (yyval.node)=Util::createNodeForIfStmt((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node),NULL); }
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 295 "lrparser.y"
    {(yyval.node)=Util::createNodeForIfStmt((yyvsp[(3) - (7)].node),(yyvsp[(5) - (7)].node),(yyvsp[(7) - (7)].node)); }
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 298 "lrparser.y"
    { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node)) ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 299 "lrparser.y"
    { reductionCall* reduc=(reductionCall*)(yyvsp[(6) - (9)].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[(2) - (9)].nodeList)->ASTNList,reduc,(yyvsp[(8) - (9)].nodeList)->ASTNList);}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 301 "lrparser.y"
    {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[(1) - (3)].node),(yyvsp[(2) - (3)].ival),(yyvsp[(3) - (3)].node));}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 304 "lrparser.y"
    {(yyval.ival)=OPERATOR_ADDASSIGN;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 305 "lrparser.y"
    {(yyval.ival)=OPERATOR_MULASSIGN;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 306 "lrparser.y"
    {(yyval.ival)=OPERATOR_ORASSIGN;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 307 "lrparser.y"
    {(yyval.ival)=OPERATOR_ANDASSIGN;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 308 "lrparser.y"
    {(yyval.ival)=OPERATOR_SUBASSIGN;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 310 "lrparser.y"
    { (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),(yyvsp[(1) - (3)].node));
                                         }
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 312 "lrparser.y"
    { (yyval.nodeList)=Util::createNList((yyvsp[(1) - (1)].node));}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 314 "lrparser.y"
    { (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),(yyvsp[(1) - (3)].node));}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 315 "lrparser.y"
    { ASTNode* node = Util::createNodeForId((yyvsp[(1) - (3)].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[(3) - (3)].nodeList),node);}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 317 "lrparser.y"
    { (yyval.nodeList)=Util::createNList((yyvsp[(1) - (1)].node));}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 318 "lrparser.y"
    { ASTNode* node = Util::createNodeForId((yyvsp[(1) - (1)].node));
			            (yyval.nodeList)=Util::createNList(node);}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 327 "lrparser.y"
    {(yyval.node)=Util::createNodeforReductionCall((yyvsp[(1) - (4)].ival),(yyvsp[(3) - (4)].aList)->AList);}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 329 "lrparser.y"
    { (yyval.ival)=REDUCE_SUM;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 330 "lrparser.y"
    {(yyval.ival)=REDUCE_COUNT;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 331 "lrparser.y"
    {(yyval.ival)=REDUCE_PRODUCT;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 332 "lrparser.y"
    {(yyval.ival)=REDUCE_MAX;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 333 "lrparser.y"
    {(yyval.ival)=REDUCE_MIN;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 335 "lrparser.y"
    { (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 336 "lrparser.y"
    {  (yyval.node)=(yyvsp[(1) - (1)].node); }
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 337 "lrparser.y"
    {(yyval.node) = (yyvsp[(1) - (1)].node); }
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 339 "lrparser.y"
    {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 343 "lrparser.y"
    {argument* a1=new argument();
		                          assignment* assign=(assignment*)(yyvsp[(1) - (3)].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
		                 //a1->assignExpr=(assignment*)$1;
						 // a1->assign=true;
						  (yyval.aList)=Util::addToAList((yyvsp[(3) - (3)].aList),a1);
						  for(argument* arg:(yyval.aList)->AList)
						  {
							  printf("VALUE OF ARG %d",arg->getAssignExpr());
						  }
						  
                          }
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 358 "lrparser.y"
    {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[(1) - (3)].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[(3) - (3)].aList),a1);
						                }
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 365 "lrparser.y"
    {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[(1) - (1)].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 370 "lrparser.y"
    { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[(1) - (1)].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 378 "lrparser.y"
    {(yyval.node)=Util::createIterateInBFSNode((yyvsp[(3) - (13)].node),(yyvsp[(5) - (13)].node),(yyvsp[(7) - (13)].node),(yyvsp[(9) - (13)].node),(yyvsp[(11) - (13)].node),(yyvsp[(12) - (13)].node),(yyvsp[(13) - (13)].node)) ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 379 "lrparser.y"
    {//$$=Util::createIterateInBFSNode($3,$6,$8,$9,$10) ;
			}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 384 "lrparser.y"
    {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[(2) - (2)].node));}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 385 "lrparser.y"
    {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[(3) - (5)].node),(yyvsp[(5) - (5)].node));}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 388 "lrparser.y"
    { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[(1) - (3)].node),(yyvsp[(3) - (3)].node));
				    }
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 393 "lrparser.y"
    {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[(1) - (5)].node),(yyvsp[(3) - (5)].node));
				    }
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 397 "lrparser.y"
    { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[(1) - (1)].text));  

            
            }
    break;



/* Line 1455 of yacc.c  */
#line 2831 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 405 "lrparser.y"



void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(int argc,char **argv) 
{
  
    dsl_cpp_generator cpp_backend;
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
		if(!(strcmp(backendTarget,"omp")==0)||(strcmp(backendTarget,"mpi")==0)||(strcmp(backendTarget,"cuda")==0))
		   {
			  fprintf(stderr, "Specified backend target is not implemented in the current version!\n");
			   exit(-1);
		   }
	}

   if(!(staticGen || dynamicGen))
      {
		fprintf(stderr, "Type of graph(static/dynamic) not specified!\n");
		exit(-1);
	  }
	  
     


   yyin= fopen(fileName,"r");
   int error=yyparse();
   printf("error val %d\n",error);


	if(error!=1)
	{
     //TODO: redirect to different backend generator after comparing with the 'b' option
    stBuilder.buildST(frontEndContext.getFuncList());

	if(staticGen)
	  {
		  if(optimize)
		  {
			  attachPropAnalyser apAnalyser;
			  apAnalyser.analyse();

			  dataRaceAnalyser drAnalyser;
			  drAnalyser.analyse();
			  
			  if(strcmp(backendTarget,"cuda")==0)
			  {
			  	deviceVarsAnalyser dvAnalyser;
				//cpp_backend.setOptimized();
			  	dvAnalyser.analyse();
			  }
		  }
	  cpp_backend.setFileName(fileName);
	  cpp_backend.generate();
	  }
	else
	 {
		 /*
		 printf("static graphsize %d\n",graphId[2][0].size());
		 dsl_dyn_cpp_generator cpp_dyn_gen;
		 cpp_dyn_gen.setFileName(fileName);
		 cpp_dyn_gen.generate();
		*/
	 }
	
	}

	printf("finished successfully\n");
   
   /* to generate code, ./finalcode -s/-d -f "filename" -b "backendname"*/
	return 0;   
	 
}

