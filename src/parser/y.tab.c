/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

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
	char* backendTarget ;
    vector<Identifier*> tempIds; //stores graph vars in current function's param list.
    //symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());

#line 99 "y.tab.c"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

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

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    T_DYNAMIC = 321,
    T_BATCH = 322,
    T_ONADD = 323,
    T_ONDELETE = 324,
    ID = 325,
    INT_NUM = 326,
    FLOAT_NUM = 327,
    BOOL_VAL = 328,
    CHAR_VAL = 329
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
#line 33 "lrparser.y"

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
     

#line 314 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */



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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


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
#define YYLAST   591

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  93
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  141
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  331

#define YYUNDEFTOK  2
#define YYMAXUTOK   329


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
       0,    98,    98,    99,   101,   108,   114,   120,   126,   132,
     138,   139,   142,   153,   157,   166,   169,   170,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     186,   188,   190,   192,   194,   196,   198,   201,   209,   212,
     215,   219,   220,   221,   224,   225,   226,   227,   228,   230,
     231,   233,   234,   236,   238,   241,   242,   243,   245,   246,
     247,   248,   250,   252,   254,   255,   256,   257,   258,   259,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   271,
     272,   273,   275,   276,   278,   283,   287,   296,   297,   298,
     299,   300,   301,   304,   305,   307,   308,   309,   310,   312,
     313,   314,   316,   317,   319,   321,   322,   325,   326,   328,
     331,   332,   333,   334,   335,   337,   339,   341,   342,   344,
     345,   354,   356,   357,   358,   359,   360,   362,   363,   364,
     366,   370,   387,   394,   399,   407,   408,   413,   414,   417,
     422,   426
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
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
  "T_STATIC", "T_DYNAMIC", "T_BATCH", "T_ONADD", "T_ONDELETE", "ID",
  "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "CHAR_VAL", "'?'", "':'", "'<'",
  "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'", "','", "';'",
  "'.'", "'{'", "'}'", "'='", "'!'", "$accept", "program", "function_def",
  "function_data", "paramList", "param", "function_body", "statements",
  "statement", "blockstatements", "batch_blockstmt", "on_add_blockstmt",
  "on_delete_blockstmt", "block_begin", "block_end", "return_stmt",
  "declaration", "type1", "primitive", "graph", "collections", "type2",
  "property", "assignment", "rhs", "expression", "unary_expr", "proc_call",
  "val", "control_flow", "iteration_cf", "filterExpr", "boolean_expr",
  "selection_cf", "reduction", "reduce_op", "leftList", "rightList",
  "reductionCall", "reduction_calls", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", "tid", "id", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,    63,    58,    60,    62,    43,
      45,    42,    47,    37,    40,    41,    44,    59,    46,   123,
     125,    61,    33
};
# endif

#define YYPACT_NINF (-280)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-141)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -280,    98,  -280,   -60,   -45,   -30,   -60,   -60,  -280,   -65,
    -280,   -25,   297,   297,   -23,   -15,  -280,  -280,  -280,  -280,
     297,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,  -280,
      -6,     7,    13,  -280,    22,    49,    -7,    41,   -60,  -280,
    -280,  -280,   -60,  -280,    43,   297,   297,   326,    55,   -60,
     192,   192,   -60,   -60,  -280,   297,  -280,    61,  -280,    63,
      82,    88,    99,  -280,  -280,  -280,   101,   102,     4,   -65,
     147,   104,   105,   106,   110,   118,   119,  -280,  -280,  -280,
     -60,     4,  -280,     4,  -280,  -280,  -280,  -280,  -280,  -280,
     117,   121,   -60,   -60,   122,   491,   123,   125,  -280,  -280,
    -280,  -280,   126,     3,  -280,  -280,  -280,   128,  -280,   136,
     139,   140,   142,   143,   145,   148,  -280,   -60,  -280,  -280,
     -60,   -60,     4,     4,   491,  -280,  -280,   150,   219,   159,
     -60,     4,     4,   -60,   -60,   -60,   166,   167,   417,   491,
    -280,  -280,   161,   163,  -280,  -280,  -280,     4,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     4,  -280,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,     4,     9,     4,
     -60,  -280,  -280,  -280,  -280,  -280,  -280,  -280,   173,   240,
     242,   491,   177,   184,   188,   -60,   256,   191,   327,   -49,
     193,   194,   205,   265,   266,   196,   -60,  -280,     4,     4,
      97,   500,   -16,   -16,   509,   509,   -16,   -16,    84,    84,
      69,    69,   206,  -280,  -280,  -280,  -280,  -280,  -280,   491,
    -280,   209,   491,   202,  -280,   -60,   -60,   416,   -65,     4,
     221,   -60,     4,     4,     4,  -280,  -280,     4,   -60,   -60,
     229,  -280,  -280,  -280,  -280,     4,   -60,   222,   220,   224,
     223,   295,  -280,   228,     4,   230,  -280,  -280,   473,   231,
     233,    95,   232,  -280,   -65,   -41,   -65,   -41,   416,   235,
     482,   -41,   -65,   -41,   -41,   237,  -280,  -280,   239,   150,
       1,  -280,   239,  -280,  -280,   -65,   279,  -280,   260,   272,
      34,   294,   273,   -60,   274,  -280,   -60,   249,   286,   255,
     291,   284,   287,   -65,    12,   -65,   288,   -65,   -65,    34,
    -280,    34,     4,  -280,  -280,   239,  -280,  -280,  -280,  -280,
     289,   -65,  -280,   310,   -59,  -280,     4,  -280,   290,   -65,
    -280
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,     0,     3,     0,
     141,     0,     0,     0,     0,     0,    34,     4,    15,    16,
       0,    44,    45,    46,    47,    48,    49,    50,    55,    56,
       0,     0,     0,    51,     0,     0,     0,    10,     0,    41,
      42,    43,     0,    57,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     7,     0,    12,    13,     8,     0,
       0,     0,     0,    91,    90,    92,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    87,    88,    89,
       0,     0,    35,     0,    17,    24,    27,    28,    29,    30,
       0,     0,     0,     0,     0,     0,    81,    64,    79,    21,
      94,    93,     0,    80,    23,   128,   129,   127,     5,     0,
       0,     0,     0,     0,     0,     0,    11,     0,     6,     9,
       0,     0,     0,     0,    36,    81,    64,    80,     0,     0,
       0,   130,   130,     0,     0,     0,     0,   116,     0,    77,
      26,    18,    37,    39,    19,    82,    83,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    25,
      20,    22,   110,   114,   111,   113,   112,   130,     0,     0,
       0,    54,    58,    60,    59,    61,    52,    53,     0,     0,
       0,   104,     0,     0,     0,     0,     0,   134,   133,    80,
       0,     0,     0,     0,     0,     0,     0,    78,     0,     0,
      69,    70,    71,    72,    75,    76,    73,    74,    65,    66,
      67,    68,     0,   122,   123,   124,   125,   126,    62,    63,
     107,     0,   109,   139,    14,     0,     0,     0,     0,     0,
       0,     0,   130,   130,     0,    85,    86,     0,     0,     0,
       0,   115,    38,    40,    84,   130,     0,     0,   127,     0,
     127,   105,    96,     0,     0,     0,   131,   132,     0,     0,
       0,     0,     0,   140,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   121,    99,   102,     0,
     127,   100,   102,   106,    97,     0,     0,    31,     0,     0,
       0,     0,     0,     0,     0,    95,     0,     0,     0,   119,
       0,   120,     0,     0,   139,     0,     0,     0,     0,     0,
     108,     0,     0,    98,   101,   102,    32,    33,   117,   118,
       0,     0,   103,   136,     0,   135,     0,   137,     0,     0,
     138
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -280,  -280,  -280,  -280,   160,  -280,  -280,  -280,  -218,    -9,
    -280,  -280,  -280,  -280,  -280,  -280,  -280,   -46,    62,  -280,
      64,   -40,  -280,   -36,   -76,     2,   -35,     0,  -277,  -280,
    -280,  -279,  -119,  -280,  -280,  -280,   190,  -271,   127,  -280,
      15,  -124,  -280,  -280,  -280,  -280,    -1
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     8,     9,    36,    37,    17,    47,    84,    85,
      86,    87,    88,    19,    89,    90,    91,    38,    39,    40,
      41,    42,    43,   187,   218,   188,   125,   126,    98,    99,
     100,   292,   182,   101,   102,   169,   136,   300,   220,   221,
     127,   190,   104,   325,   105,   106,   107
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,    92,    11,   294,   183,    14,    15,    93,   191,   251,
      10,    94,    96,   299,    63,    64,    65,   145,   146,    63,
      64,    65,    72,    73,    16,   326,   162,   163,   164,    10,
      16,   165,   299,   166,   299,   167,   321,    56,   318,    12,
     319,    57,   234,   212,    63,    64,    65,    97,   109,    95,
     283,   114,   115,   213,    13,   214,   215,   216,   217,    20,
     128,    45,   103,   155,   156,   157,   158,    72,    73,    46,
     124,    49,    72,    73,    10,    77,    78,    79,    54,    10,
      77,    78,    79,   138,    50,   139,  -139,   167,    81,   293,
      51,   142,   143,    81,   168,   137,    83,  -140,     2,    52,
     246,    83,   145,   146,    10,    77,    78,    79,   256,   257,
     253,     3,   110,   112,   111,   113,   178,   145,   146,   179,
     180,   262,   242,   243,   181,   181,    53,    55,    58,   186,
     145,   146,   192,   193,   194,   149,   150,   151,   152,   213,
     108,   214,   215,   216,   217,   117,   189,   189,   118,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,     4,     5,     6,     7,   157,   158,   119,   129,   223,
     219,   222,   120,    44,   153,   154,   155,   156,   157,   158,
      48,    92,   189,   121,   230,   122,   123,    93,   130,   131,
     132,    94,    96,   320,   133,    21,    22,    23,    24,    25,
     219,   219,   134,   135,   140,    59,    60,   328,   141,   144,
     159,   137,   160,   161,   171,   116,   170,   172,   173,   252,
     174,   175,    92,   176,   248,   250,   177,    97,    93,    95,
     255,   181,    94,    96,   167,   184,   219,   259,   260,   258,
     247,   249,   103,   185,   195,   263,    30,   189,   189,    33,
      34,    35,   198,   196,   199,   277,   270,   281,   224,   225,
     189,   226,   227,   287,   280,   278,   280,   282,    97,   228,
      95,   286,   229,   288,   289,   231,   295,   232,   235,   236,
     279,   237,   279,   103,   238,   239,   279,   240,   279,   279,
     246,   244,   304,   245,   313,   306,   314,   254,   316,   317,
      21,    22,    23,    24,    25,   301,   261,   264,   265,   266,
     268,   267,   323,   269,   181,   327,   302,   276,   271,   273,
     330,   274,   284,   290,   301,   307,   301,   291,   181,    21,
      22,    23,    24,    25,    61,    62,    63,    64,    65,   296,
      66,   309,    67,    68,    69,   297,    70,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,   298,   303,   305,
     145,   146,   308,   147,   148,   149,   150,   151,   152,   310,
     311,   312,   324,   315,   322,   329,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,   241,    71,   275,    72,
      73,     0,     0,    74,    75,    76,    10,    77,    78,    79,
       0,     0,     0,    80,   153,   154,   155,   156,   157,   158,
      81,     0,     0,   233,     0,    16,    82,     0,    83,    21,
      22,    23,    24,    25,    61,    62,    63,    64,    65,     0,
      66,     0,    67,    68,    69,     0,    70,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     145,   146,     0,   147,   148,   149,   150,   151,   152,     0,
       0,     0,     0,     0,     0,     0,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,     0,    71,     0,    72,
      73,     0,     0,    74,    75,    76,    10,    77,    78,    79,
       0,     0,     0,    80,   153,   154,   155,   156,   157,   158,
      81,     0,   197,     0,     0,    16,   145,   146,    83,   147,
     148,   149,   150,   151,   152,   145,   146,     0,   147,   148,
     149,   150,   151,   152,   145,   146,     0,   147,   148,   149,
     150,   151,   152,   145,   146,     0,   147,     0,   149,   150,
     151,   152,   145,   146,     0,     0,     0,   149,   150,     0,
     153,   154,   155,   156,   157,   158,     0,     0,   272,   153,
     154,   155,   156,   157,   158,     0,     0,   285,   153,   154,
     155,   156,   157,   158,     0,     0,     0,   153,   154,   155,
     156,   157,   158,     0,     0,     0,   153,   154,   155,   156,
     157,   158
};

static const yytype_int16 yycheck[] =
{
       9,    47,     3,   282,   123,     6,     7,    47,   132,   227,
      70,    47,    47,   290,    10,    11,    12,    33,    34,    10,
      11,    12,    63,    64,    89,    84,    23,    24,    25,    70,
      89,    28,   309,    30,   311,    84,   315,    38,   309,    84,
     311,    42,    91,   167,    10,    11,    12,    47,    49,    47,
     268,    52,    53,    44,    84,    46,    47,    48,    49,    84,
      69,    84,    47,    79,    80,    81,    82,    63,    64,    84,
      68,    77,    63,    64,    70,    71,    72,    73,    85,    70,
      71,    72,    73,    81,    77,    83,    85,    84,    84,    88,
      77,    92,    93,    84,    91,    80,    92,    85,     0,    77,
      88,    92,    33,    34,    70,    71,    72,    73,   232,   233,
     229,    13,    50,    51,    50,    51,   117,    33,    34,   120,
     121,   245,   198,   199,   122,   123,    77,    86,    85,   130,
      33,    34,   133,   134,   135,    38,    39,    40,    41,    44,
      85,    46,    47,    48,    49,    84,   131,   132,    85,   147,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,    63,    64,    65,    66,    81,    82,    85,    21,   170,
     168,   169,    84,    13,    77,    78,    79,    80,    81,    82,
      20,   227,   167,    84,   185,    84,    84,   227,    84,    84,
      84,   227,   227,   312,    84,     3,     4,     5,     6,     7,
     198,   199,    84,    84,    87,    45,    46,   326,    87,    87,
      87,   196,    87,    87,    78,    55,    88,    78,    78,   228,
      78,    78,   268,    78,   225,   226,    78,   227,   268,   227,
     231,   229,   268,   268,    84,    16,   234,   238,   239,   237,
     225,   226,   227,    84,    78,   246,    54,   232,   233,    57,
      58,    59,    91,    86,    91,   264,   254,   266,    85,    19,
     245,    19,    85,   272,   265,   265,   267,   267,   268,    85,
     268,   271,    84,   273,   274,    19,   285,    86,    85,    85,
     265,    76,   267,   268,    19,    19,   271,    91,   273,   274,
      88,    85,   293,    84,   303,   296,   305,    76,   307,   308,
       3,     4,     5,     6,     7,   290,    77,    85,    88,    85,
      15,    88,   321,    85,   312,   324,    22,    85,    88,    88,
     329,    88,    87,    86,   309,    76,   311,    88,   326,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    60,
      14,    86,    16,    17,    18,    85,    20,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    85,    85,    85,
      33,    34,    76,    36,    37,    38,    39,    40,    41,    78,
      86,    84,    62,    85,    85,    85,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,   196,    61,   261,    63,
      64,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    -1,    77,    77,    78,    79,    80,    81,    82,
      84,    -1,    -1,    86,    -1,    89,    90,    -1,    92,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      14,    -1,    16,    17,    18,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    34,    -1,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    -1,    63,
      64,    -1,    -1,    67,    68,    69,    70,    71,    72,    73,
      -1,    -1,    -1,    77,    77,    78,    79,    80,    81,    82,
      84,    -1,    85,    -1,    -1,    89,    33,    34,    92,    36,
      37,    38,    39,    40,    41,    33,    34,    -1,    36,    37,
      38,    39,    40,    41,    33,    34,    -1,    36,    37,    38,
      39,    40,    41,    33,    34,    -1,    36,    -1,    38,    39,
      40,    41,    33,    34,    -1,    -1,    -1,    38,    39,    -1,
      77,    78,    79,    80,    81,    82,    -1,    -1,    85,    77,
      78,    79,    80,    81,    82,    -1,    -1,    85,    77,    78,
      79,    80,    81,    82,    -1,    -1,    -1,    77,    78,    79,
      80,    81,    82,    -1,    -1,    -1,    77,    78,    79,    80,
      81,    82
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    94,     0,    13,    63,    64,    65,    66,    95,    96,
      70,   139,    84,    84,   139,   139,    89,    99,   102,   106,
      84,     3,     4,     5,     6,     7,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    97,    98,   110,   111,
     112,   113,   114,   115,    97,    84,    84,   100,    97,    77,
      77,    77,    77,    77,    85,    86,   139,   139,    85,    97,
      97,     8,     9,    10,    11,    12,    14,    16,    17,    18,
      20,    61,    63,    64,    67,    68,    69,    71,    72,    73,
      77,    84,    90,    92,   101,   102,   103,   104,   105,   107,
     108,   109,   110,   114,   116,   118,   119,   120,   121,   122,
     123,   126,   127,   133,   135,   137,   138,   139,    85,   139,
     111,   113,   111,   113,   139,   139,    97,    84,    85,    85,
      84,    84,    84,    84,   118,   119,   120,   133,   102,    21,
      84,    84,    84,    84,    84,    84,   129,   133,   118,   118,
      87,    87,   139,   139,    87,    33,    34,    36,    37,    38,
      39,    40,    41,    77,    78,    79,    80,    81,    82,    87,
      87,    87,    23,    24,    25,    28,    30,    84,    91,   128,
      88,    78,    78,    78,    78,    78,    78,    78,   139,   139,
     139,   118,   125,   125,    16,    84,   139,   116,   118,   133,
     134,   134,   139,   139,   139,    78,    86,    85,    91,    91,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   134,    44,    46,    47,    48,    49,   117,   118,
     131,   132,   118,   139,    85,    19,    19,    85,    85,    84,
     139,    19,    86,    86,    91,    85,    85,    76,    19,    19,
      91,   129,   117,   117,    85,    84,    88,   133,   139,   133,
     139,   101,   102,   125,    76,   139,   134,   134,   118,   139,
     139,    77,   134,   139,    85,    88,    85,    88,    15,    85,
     118,    88,    85,    88,    88,   131,    85,   102,   120,   133,
     139,   102,   120,   101,    87,    85,   120,   102,   120,   120,
      86,    88,   124,    88,   124,   102,    60,    85,    85,   121,
     130,   133,    22,    85,   139,    85,   139,    76,    76,    86,
      78,    86,    84,   102,   102,    85,   102,   102,   130,   130,
     125,   124,    85,   102,    62,   136,    84,   102,   125,    85,
     102
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    93,    94,    94,    95,    96,    96,    96,    96,    96,
      97,    97,    98,    98,    98,    99,   100,   100,   101,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   109,   109,
     109,   110,   110,   110,   111,   111,   111,   111,   111,   112,
     112,   113,   113,   113,   113,   114,   114,   114,   115,   115,
     115,   115,   116,   117,   118,   118,   118,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   118,   119,   119,   120,   120,   120,   121,   121,   121,
     121,   121,   121,   122,   122,   123,   123,   123,   123,   123,
     123,   123,   124,   124,   125,   126,   126,   127,   127,   127,
     128,   128,   128,   128,   128,   129,   129,   130,   130,   130,
     130,   131,   132,   132,   132,   132,   132,   133,   133,   133,
     134,   134,   134,   134,   134,   135,   135,   136,   136,   137,
     138,   139
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     5,     5,     4,     4,     5,
       1,     3,     2,     2,     5,     1,     0,     2,     2,     2,
       2,     1,     2,     1,     1,     2,     2,     1,     1,     1,
       3,     7,    10,    10,     1,     1,     2,     2,     4,     2,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     4,     1,     1,     1,     4,     4,
       4,     4,     3,     1,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     3,     1,
       1,     1,     2,     2,     4,     4,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     8,     5,     7,    10,     7,
       7,    10,     0,     5,     1,     5,     7,     3,     9,     3,
       1,     1,     1,     1,     1,     3,     1,     3,     3,     1,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     3,     3,     1,     1,    13,    12,     2,     5,     3,
       5,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
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

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
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
  case 3:
#line 99 "lrparser.y"
                               {/* printf("LIST SIZE %d",frontEndContext.getFuncList().size())  ;*/ }
#line 1781 "y.tab.c"
    break;

  case 4:
#line 101 "lrparser.y"
                                            { 
	                                          Function* func=(Function*)(yyvsp[-1].node);
                                              blockStatement* block=(blockStatement*)(yyvsp[0].node);
                                              func->setBlockStatement(block);
											   Util::addFuncToList(func);
											}
#line 1792 "y.tab.c"
    break;

  case 5:
#line 108 "lrparser.y"
                                           { 
										   (yyval.node)=Util::createFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                           Util::setCurrentFuncType(GEN_FUNC);
										   Util::resetTemp(tempIds);
										   tempIds.clear();
	                                      }
#line 1803 "y.tab.c"
    break;

  case 6:
#line 114 "lrparser.y"
                                                           { 
										   (yyval.node)=Util::createStaticFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(STATIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1814 "y.tab.c"
    break;

  case 7:
#line 120 "lrparser.y"
                                                     { 
										   (yyval.node)=Util::createIncrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(INCREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1825 "y.tab.c"
    break;

  case 8:
#line 126 "lrparser.y"
                                                             { 
										   (yyval.node)=Util::createDecrementalNode((yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DECREMENTAL_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
	                                      }
#line 1836 "y.tab.c"
    break;

  case 9:
#line 132 "lrparser.y"
                                                        { (yyval.node)=Util::createDynamicFuncNode((yyvsp[-3].node),(yyvsp[-1].pList)->PList);
                                            Util::setCurrentFuncType(DYNAMIC_FUNC);
											Util::resetTemp(tempIds);
											tempIds.clear();
											}
#line 1846 "y.tab.c"
    break;

  case 10:
#line 138 "lrparser.y"
                 {(yyval.pList)=Util::createPList((yyvsp[0].node));}
#line 1852 "y.tab.c"
    break;

  case 11:
#line 139 "lrparser.y"
                                     {(yyval.pList)=Util::addToPList((yyvsp[0].pList),(yyvsp[-2].node)); 
			                           }
#line 1859 "y.tab.c"
    break;

  case 12:
#line 142 "lrparser.y"
                 {  //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
                        Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    {
							 tempIds.push_back(id);
						   
							}
					printf("\n");
                    (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 1875 "y.tab.c"
    break;

  case 13:
#line 153 "lrparser.y"
                          { // Identifier* id=(Identifier*)Util::createIdentifierNode($2);
			  
					
                             (yyval.node)=Util::createParamNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 1884 "y.tab.c"
    break;

  case 14:
#line 157 "lrparser.y"
                                                 { // Identifier* id1=(Identifier*)Util::createIdentifierNode($4);
			                            //Identifier* id=(Identifier*)Util::createIdentifierNode($2);
				                        Type* tempType=(Type*)(yyvsp[-4].node);
			                            if(tempType->isNodeEdgeType())
										  tempType->addSourceGraph((yyvsp[-1].node));
				                         (yyval.node)=Util::createParamNode(tempType,(yyvsp[-3].node));
									 }
#line 1896 "y.tab.c"
    break;

  case 15:
#line 166 "lrparser.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 1902 "y.tab.c"
    break;

  case 16:
#line 169 "lrparser.y"
              {}
#line 1908 "y.tab.c"
    break;

  case 17:
#line 170 "lrparser.y"
                               { Util::addToBlock((yyvsp[0].node)); }
#line 1914 "y.tab.c"
    break;

  case 18:
#line 172 "lrparser.y"
                          {(yyval.node)=(yyvsp[-1].node);}
#line 1920 "y.tab.c"
    break;

  case 19:
#line 173 "lrparser.y"
                       {(yyval.node)=(yyvsp[-1].node);}
#line 1926 "y.tab.c"
    break;

  case 20:
#line 174 "lrparser.y"
                       {(yyval.node)=Util::createNodeForProcCallStmt((yyvsp[-1].node));}
#line 1932 "y.tab.c"
    break;

  case 21:
#line 175 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 1938 "y.tab.c"
    break;

  case 22:
#line 176 "lrparser.y"
                      {(yyval.node)=(yyvsp[-1].node);}
#line 1944 "y.tab.c"
    break;

  case 23:
#line 177 "lrparser.y"
                          {(yyval.node)=(yyvsp[0].node); }
#line 1950 "y.tab.c"
    break;

  case 24:
#line 178 "lrparser.y"
                          {(yyval.node)=(yyvsp[0].node);}
#line 1956 "y.tab.c"
    break;

  case 25:
#line 179 "lrparser.y"
                         {(yyval.node)=Util::createNodeForUnaryStatements((yyvsp[-1].node));}
#line 1962 "y.tab.c"
    break;

  case 26:
#line 180 "lrparser.y"
                          {(yyval.node) = (yyvsp[-1].node) ;}
#line 1968 "y.tab.c"
    break;

  case 27:
#line 181 "lrparser.y"
                           {(yyval.node) = (yyvsp[0].node);}
#line 1974 "y.tab.c"
    break;

  case 28:
#line 182 "lrparser.y"
                           {(yyval.node) = (yyvsp[0].node);}
#line 1980 "y.tab.c"
    break;

  case 29:
#line 183 "lrparser.y"
                              {(yyval.node) = (yyvsp[0].node);}
#line 1986 "y.tab.c"
    break;

  case 30:
#line 186 "lrparser.y"
                                                   { (yyval.node)=Util::finishBlock();}
#line 1992 "y.tab.c"
    break;

  case 31:
#line 188 "lrparser.y"
                                                                    {(yyval.node) = Util::createBatchBlockStmt((yyvsp[-4].node), (yyvsp[-2].node), (yyvsp[0].node));}
#line 1998 "y.tab.c"
    break;

  case 32:
#line 190 "lrparser.y"
                                                                                {(yyval.node) = Util::createOnAddBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2004 "y.tab.c"
    break;

  case 33:
#line 192 "lrparser.y"
                                                                                      {(yyval.node) = Util::createOnDeleteBlock((yyvsp[-7].node), (yyvsp[-5].node), (yyvsp[-3].node), (yyvsp[0].node));}
#line 2010 "y.tab.c"
    break;

  case 34:
#line 194 "lrparser.y"
                { Util::createNewBlock(); }
#line 2016 "y.tab.c"
    break;

  case 36:
#line 198 "lrparser.y"
                                  {(yyval.node) = Util::createReturnStatementNode((yyvsp[0].node));}
#line 2022 "y.tab.c"
    break;

  case 37:
#line 201 "lrparser.y"
                         {
	                     Type* type=(Type*)(yyvsp[-1].node);
	                     Identifier* id=(Identifier*)(yyvsp[0].node);
						 
						 if(type->isGraphType())
						    Util::storeGraphId(id);

                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node));}
#line 2035 "y.tab.c"
    break;

  case 38:
#line 209 "lrparser.y"
                            {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                    
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2043 "y.tab.c"
    break;

  case 39:
#line 212 "lrparser.y"
                    {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	            
                         (yyval.node)=Util::createNormalDeclNode((yyvsp[-1].node),(yyvsp[0].node)); }
#line 2051 "y.tab.c"
    break;

  case 40:
#line 215 "lrparser.y"
                           {//Identifier* id=(Identifier*)Util::createIdentifierNode($2);
	                   
	                    (yyval.node)=Util::createAssignedDeclNode((yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2059 "y.tab.c"
    break;

  case 41:
#line 219 "lrparser.y"
                 {(yyval.node)=(yyvsp[0].node); }
#line 2065 "y.tab.c"
    break;

  case 42:
#line 220 "lrparser.y"
                {(yyval.node)=(yyvsp[0].node);}
#line 2071 "y.tab.c"
    break;

  case 43:
#line 221 "lrparser.y"
                      { (yyval.node)=(yyvsp[0].node);}
#line 2077 "y.tab.c"
    break;

  case 44:
#line 224 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_INT);}
#line 2083 "y.tab.c"
    break;

  case 45:
#line 225 "lrparser.y"
                  { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_FLOAT);}
#line 2089 "y.tab.c"
    break;

  case 46:
#line 226 "lrparser.y"
                 { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_BOOL);}
#line 2095 "y.tab.c"
    break;

  case 47:
#line 227 "lrparser.y"
                   { (yyval.node)=Util::createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 2101 "y.tab.c"
    break;

  case 48:
#line 228 "lrparser.y"
             {(yyval.node)=(yyval.node)=Util::createPrimitiveTypeNode(TYPE_LONG);}
#line 2107 "y.tab.c"
    break;

  case 49:
#line 230 "lrparser.y"
                { (yyval.node)=Util::createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 2113 "y.tab.c"
    break;

  case 50:
#line 231 "lrparser.y"
                     {(yyval.node)=Util::createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 2119 "y.tab.c"
    break;

  case 51:
#line 233 "lrparser.y"
                     { (yyval.node)=Util::createCollectionTypeNode(TYPE_LIST,NULL);}
#line 2125 "y.tab.c"
    break;

  case 52:
#line 234 "lrparser.y"
                                        {//Identifier* id=(Identifier*)Util::createIdentifierNode($3);
			                     (yyval.node)=Util::createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].node));}
#line 2132 "y.tab.c"
    break;

  case 53:
#line 236 "lrparser.y"
                                         {// Identifier* id=(Identifier*)Util::createIdentifierNode($3);
					                    (yyval.node)=Util::createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].node));}
#line 2139 "y.tab.c"
    break;

  case 54:
#line 238 "lrparser.y"
                                                         { (yyval.node)=Util::createCollectionTypeNode(TYPE_UPDATES,(yyvsp[-1].node));}
#line 2145 "y.tab.c"
    break;

  case 55:
#line 241 "lrparser.y"
               {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_NODE) ;}
#line 2151 "y.tab.c"
    break;

  case 56:
#line 242 "lrparser.y"
                {(yyval.node)=Util::createNodeEdgeTypeNode(TYPE_EDGE);}
#line 2157 "y.tab.c"
    break;

  case 57:
#line 243 "lrparser.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 2163 "y.tab.c"
    break;

  case 58:
#line 245 "lrparser.y"
                                  { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2169 "y.tab.c"
    break;

  case 59:
#line 246 "lrparser.y"
                                       { (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node)); }
#line 2175 "y.tab.c"
    break;

  case 60:
#line 247 "lrparser.y"
                                                    {  (yyval.node)=Util::createPropertyTypeNode(TYPE_PROPNODE,(yyvsp[-1].node)); }
#line 2181 "y.tab.c"
    break;

  case 61:
#line 248 "lrparser.y"
                                                     {(yyval.node)=Util::createPropertyTypeNode(TYPE_PROPEDGE,(yyvsp[-1].node));}
#line 2187 "y.tab.c"
    break;

  case 62:
#line 250 "lrparser.y"
                                { (yyval.node)=Util::createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2193 "y.tab.c"
    break;

  case 63:
#line 252 "lrparser.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 2199 "y.tab.c"
    break;

  case 64:
#line 254 "lrparser.y"
                       { (yyval.node)=(yyvsp[0].node);}
#line 2205 "y.tab.c"
    break;

  case 65:
#line 255 "lrparser.y"
                                         { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 2211 "y.tab.c"
    break;

  case 66:
#line 256 "lrparser.y"
                                             { (yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 2217 "y.tab.c"
    break;

  case 67:
#line 257 "lrparser.y"
                                             {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 2223 "y.tab.c"
    break;

  case 68:
#line 258 "lrparser.y"
                                           {(yyval.node)=Util::createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 2229 "y.tab.c"
    break;

  case 69:
#line 259 "lrparser.y"
                                              {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 2235 "y.tab.c"
    break;

  case 70:
#line 260 "lrparser.y"
                                                  {(yyval.node)=Util::createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 2241 "y.tab.c"
    break;

  case 71:
#line 261 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 2247 "y.tab.c"
    break;

  case 72:
#line 262 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 2253 "y.tab.c"
    break;

  case 73:
#line 263 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 2259 "y.tab.c"
    break;

  case 74:
#line 264 "lrparser.y"
                                                    {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 2265 "y.tab.c"
    break;

  case 75:
#line 265 "lrparser.y"
                                                        {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2271 "y.tab.c"
    break;

  case 76:
#line 266 "lrparser.y"
                                            {(yyval.node)=Util::createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2277 "y.tab.c"
    break;

  case 77:
#line 267 "lrparser.y"
                                         {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[0].node),OPERATOR_NOT);}
#line 2283 "y.tab.c"
    break;

  case 78:
#line 268 "lrparser.y"
                                          { Expression* expr=(Expression*)(yyvsp[-1].node);
				                     expr->setEnclosedBrackets();
			                        (yyval.node)=expr;}
#line 2291 "y.tab.c"
    break;

  case 79:
#line 271 "lrparser.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2297 "y.tab.c"
    break;

  case 80:
#line 272 "lrparser.y"
                                    { (yyval.node)=Util::createNodeForId((yyvsp[0].node));}
#line 2303 "y.tab.c"
    break;

  case 81:
#line 273 "lrparser.y"
                                      {(yyval.node)=(yyvsp[0].node);}
#line 2309 "y.tab.c"
    break;

  case 82:
#line 275 "lrparser.y"
                                   {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_INC);}
#line 2315 "y.tab.c"
    break;

  case 83:
#line 276 "lrparser.y"
                                                {(yyval.node)=Util::createNodeForUnaryExpr((yyvsp[-1].node),OPERATOR_DEC);}
#line 2321 "y.tab.c"
    break;

  case 84:
#line 278 "lrparser.y"
                                      { 
                                       
                                       (yyval.node) = Util::createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)->AList); 

									    }
#line 2331 "y.tab.c"
    break;

  case 85:
#line 283 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Incremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList); 

				                               }
#line 2340 "y.tab.c"
    break;

  case 86:
#line 287 "lrparser.y"
                                                         { ASTNode* id = Util::createIdentifierNode("Decremental");
			                                   (yyval.node) = Util::createNodeForProcCall(id, (yyvsp[-1].aList)->AList); 

				                               }
#line 2349 "y.tab.c"
    break;

  case 87:
#line 296 "lrparser.y"
              { (yyval.node) = Util::createNodeForIval((yyvsp[0].ival)); }
#line 2355 "y.tab.c"
    break;

  case 88:
#line 297 "lrparser.y"
                    {(yyval.node) = Util::createNodeForFval((yyvsp[0].fval));}
#line 2361 "y.tab.c"
    break;

  case 89:
#line 298 "lrparser.y"
                   { (yyval.node) = Util::createNodeForBval((yyvsp[0].bval));}
#line 2367 "y.tab.c"
    break;

  case 90:
#line 299 "lrparser.y"
                {(yyval.node)=Util::createNodeForINF(true);}
#line 2373 "y.tab.c"
    break;

  case 91:
#line 300 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(true);}
#line 2379 "y.tab.c"
    break;

  case 92:
#line 301 "lrparser.y"
                  {(yyval.node)=Util::createNodeForINF(false);}
#line 2385 "y.tab.c"
    break;

  case 93:
#line 304 "lrparser.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2391 "y.tab.c"
    break;

  case 94:
#line 305 "lrparser.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2397 "y.tab.c"
    break;

  case 95:
#line 307 "lrparser.y"
                                                                              { (yyval.node)=Util::createNodeForFixedPointStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node));}
#line 2403 "y.tab.c"
    break;

  case 96:
#line 308 "lrparser.y"
                                                                 {(yyval.node)=Util::createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2409 "y.tab.c"
    break;

  case 97:
#line 309 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForDoWhileStmt((yyvsp[-2].node),(yyvsp[-5].node));  }
#line 2415 "y.tab.c"
    break;

  case 98:
#line 310 "lrparser.y"
                                                                                       { 
																				(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2422 "y.tab.c"
    break;

  case 99:
#line 312 "lrparser.y"
                                                                        { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2428 "y.tab.c"
    break;

  case 100:
#line 313 "lrparser.y"
                                                                 { (yyval.node)=Util::createNodeForForStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2434 "y.tab.c"
    break;

  case 101:
#line 314 "lrparser.y"
                                                                                    {(yyval.node)=Util::createNodeForForAllStmt((yyvsp[-7].node),(yyvsp[-5].node),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2440 "y.tab.c"
    break;

  case 102:
#line 316 "lrparser.y"
                      { (yyval.node)=NULL;}
#line 2446 "y.tab.c"
    break;

  case 103:
#line 317 "lrparser.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2452 "y.tab.c"
    break;

  case 104:
#line 319 "lrparser.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2458 "y.tab.c"
    break;

  case 105:
#line 321 "lrparser.y"
                                                   { (yyval.node)=Util::createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2464 "y.tab.c"
    break;

  case 106:
#line 322 "lrparser.y"
                                                                           {(yyval.node)=Util::createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2470 "y.tab.c"
    break;

  case 107:
#line 325 "lrparser.y"
                                       { (yyval.node)=Util::createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) ;}
#line 2476 "y.tab.c"
    break;

  case 108:
#line 326 "lrparser.y"
                                                                              { reductionCall* reduc=(reductionCall*)(yyvsp[-3].node);
		                                                               (yyval.node)=Util::createNodeForReductionStmtList((yyvsp[-7].nodeList)->ASTNList,reduc,(yyvsp[-1].nodeList)->ASTNList);}
#line 2483 "y.tab.c"
    break;

  case 109:
#line 328 "lrparser.y"
                                                   {(yyval.node)=Util::createNodeForReductionOpStmt((yyvsp[-2].node),(yyvsp[-1].ival),(yyvsp[0].node));}
#line 2489 "y.tab.c"
    break;

  case 110:
#line 331 "lrparser.y"
                         {(yyval.ival)=OPERATOR_ADDASSIGN;}
#line 2495 "y.tab.c"
    break;

  case 111:
#line 332 "lrparser.y"
                         {(yyval.ival)=OPERATOR_MULASSIGN;}
#line 2501 "y.tab.c"
    break;

  case 112:
#line 333 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ORASSIGN;}
#line 2507 "y.tab.c"
    break;

  case 113:
#line 334 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_ANDASSIGN;}
#line 2513 "y.tab.c"
    break;

  case 114:
#line 335 "lrparser.y"
                                 {(yyval.ival)=OPERATOR_SUBASSIGN;}
#line 2519 "y.tab.c"
    break;

  case 115:
#line 337 "lrparser.y"
                                  { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));
                                         }
#line 2526 "y.tab.c"
    break;

  case 116:
#line 339 "lrparser.y"
                            { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2532 "y.tab.c"
    break;

  case 117:
#line 341 "lrparser.y"
                              { (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),(yyvsp[-2].node));}
#line 2538 "y.tab.c"
    break;

  case 118:
#line 342 "lrparser.y"
                                   { ASTNode* node = Util::createNodeForId((yyvsp[-2].node));
			                         (yyval.nodeList)=Util::addToNList((yyvsp[0].nodeList),node);}
#line 2545 "y.tab.c"
    break;

  case 119:
#line 344 "lrparser.y"
                   { (yyval.nodeList)=Util::createNList((yyvsp[0].node));}
#line 2551 "y.tab.c"
    break;

  case 120:
#line 345 "lrparser.y"
                              { ASTNode* node = Util::createNodeForId((yyvsp[0].node));
			            (yyval.nodeList)=Util::createNList(node);}
#line 2558 "y.tab.c"
    break;

  case 121:
#line 354 "lrparser.y"
                                                 {(yyval.node)=Util::createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList)->AList);}
#line 2564 "y.tab.c"
    break;

  case 122:
#line 356 "lrparser.y"
                        { (yyval.ival)=REDUCE_SUM;}
#line 2570 "y.tab.c"
    break;

  case 123:
#line 357 "lrparser.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2576 "y.tab.c"
    break;

  case 124:
#line 358 "lrparser.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2582 "y.tab.c"
    break;

  case 125:
#line 359 "lrparser.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2588 "y.tab.c"
    break;

  case 126:
#line 360 "lrparser.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2594 "y.tab.c"
    break;

  case 127:
#line 362 "lrparser.y"
              { (yyval.node)=(yyvsp[0].node); }
#line 2600 "y.tab.c"
    break;

  case 128:
#line 363 "lrparser.y"
               {  (yyval.node)=(yyvsp[0].node); }
#line 2606 "y.tab.c"
    break;

  case 129:
#line 364 "lrparser.y"
               {(yyval.node) = (yyvsp[0].node); }
#line 2612 "y.tab.c"
    break;

  case 130:
#line 366 "lrparser.y"
              {
                 argList* aList=new argList();
				 (yyval.aList)=aList;  }
#line 2620 "y.tab.c"
    break;

  case 131:
#line 370 "lrparser.y"
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
#line 2640 "y.tab.c"
    break;

  case 132:
#line 387 "lrparser.y"
                                             {argument* a1=new argument();
		                                Expression* expr=(Expression*)(yyvsp[-2].node);
										a1->setExpression(expr);
										a1->setExpressionFlag();
						               // a1->expressionflag=true;
										 (yyval.aList)=Util::addToAList((yyvsp[0].aList),a1);
						                }
#line 2652 "y.tab.c"
    break;

  case 133:
#line 394 "lrparser.y"
                            {argument* a1=new argument();
		                 Expression* expr=(Expression*)(yyvsp[0].node);
						 a1->setExpression(expr);
						a1->setExpressionFlag();
						  (yyval.aList)=Util::createAList(a1); }
#line 2662 "y.tab.c"
    break;

  case 134:
#line 399 "lrparser.y"
                            { argument* a1=new argument();
		                   assignment* assign=(assignment*)(yyvsp[0].node);
		                     a1->setAssign(assign);
							 a1->setAssignFlag();
						   (yyval.aList)=Util::createAList(a1);
						   }
#line 2673 "y.tab.c"
    break;

  case 135:
#line 407 "lrparser.y"
                                                                                                                 {(yyval.node)=Util::createIterateInBFSNode((yyvsp[-10].node),(yyvsp[-8].node),(yyvsp[-6].node),(yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2679 "y.tab.c"
    break;

  case 136:
#line 408 "lrparser.y"
                                                                                                      {//$$=Util::createIterateInBFSNode($3,$6,$8,$9,$10) ;
			}
#line 2686 "y.tab.c"
    break;

  case 137:
#line 413 "lrparser.y"
                                                 {(yyval.node)=Util::createIterateInReverseBFSNode(NULL,(yyvsp[0].node));}
#line 2692 "y.tab.c"
    break;

  case 138:
#line 414 "lrparser.y"
                                                                       {(yyval.node)=Util::createIterateInReverseBFSNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 2698 "y.tab.c"
    break;

  case 139:
#line 417 "lrparser.y"
                { //Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-2].node),(yyvsp[0].node));
				    }
#line 2707 "y.tab.c"
    break;

  case 140:
#line 422 "lrparser.y"
                       {// Identifier* id1=(Identifier*)Util::createIdentifierNode($1);
                  // Identifier* id2=(Identifier*)Util::createIdentifierNode($1);
				   (yyval.node)=Util::createPropIdNode((yyvsp[-4].node),(yyvsp[-2].node));
				    }
#line 2716 "y.tab.c"
    break;

  case 141:
#line 426 "lrparser.y"
          { 
	         (yyval.node)=Util::createIdentifierNode((yyvsp[0].text));  

            
            }
#line 2726 "y.tab.c"
    break;


#line 2730 "y.tab.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
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
                  yystos[+*yyssp], yyvsp);
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
  return yyresult;
}
#line 434 "lrparser.y"



void yyerror(const char *s) {
    fprintf(stderr, "%s\n", s);
}


int main(int argc,char **argv) 
{
  
  if(argc<4){
    std::cout<< "Usage: " << argv[0] << " [-s|-d] -f <dsl.sp> -b [cuda|omp|mpi|acc] " << '\n';
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
		if(!((strcmp(backendTarget,"omp")==0)||(strcmp(backendTarget,"mpi")==0)||(strcmp(backendTarget,"cuda")==0) || (strcmp(backendTarget,"acc")==0)))
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
	  //cpp_backend.setOptimized();
	  dvAnalyser.analyse(frontEndContext.getFuncList());
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
