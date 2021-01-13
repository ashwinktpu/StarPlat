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
#line 3 "dsl_yacc_modified.y"

	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <SymbolTable.h>
	#include<MainContext.h>
	#include<ASTNode.h>
	#include <list>

	void yyerror(char *);
	int yylex(void);
	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;
	extern SymbolTable* symbTab;
	
	symbTab=new SymbolTable();
	//symbolTableList.push_back(new SymbolTable());

#line 92 "y.tab.c"

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
    T_NP = 309,
    T_EP = 310,
    T_LIST = 311,
    T_SET_NODES = 312,
    T_SET_EDGES = 313,
    T_ELEMENTS = 314,
    T_FROM = 315,
    T_BFS = 316,
    T_REVERSE = 317,
    ID = 318,
    INT_NUM = 319,
    FLOAT_NUM = 320,
    BOOL_VAL = 321
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
#define T_NP 309
#define T_EP 310
#define T_LIST 311
#define T_SET_NODES 312
#define T_SET_EDGES 313
#define T_ELEMENTS 314
#define T_FROM 315
#define T_BFS 316
#define T_REVERSE 317
#define ID 318
#define INT_NUM 319
#define FLOAT_NUM 320
#define BOOL_VAL 321

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 27 "dsl_yacc_modified.y"

    int  info;
    long ival;
    double fval;
    char* text;
	ASTNode* node;
	argList* aList;
	ASTNodeList* nodeList;
    
    struct tempNode temporary;

#line 288 "y.tab.c"

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
typedef yytype_uint8 yy_state_t;

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   294

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  39
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  231

#define YYUNDEFTOK  2
#define YYMAXUTOK   321


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
       2,     2,     2,     2,     2,     2,     2,    75,     2,     2,
      76,    77,    73,    71,    78,    72,    83,    74,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    68,    79,
      69,    82,    70,    67,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    80,     2,    81,     2,     2,     2,     2,
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
      65,    66
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    86,    86,    88,    90,    93,    94,    96,    98,   100,
     109,   112,   113,   115,   116,   117,   118,   119,   120,   121,
     124,   126,   128,   131,   133,   135,   137,   140,   141,   142,
     145,   146,   147,   148,   149,   151,   152,   154,   155,   156,
     158,   159,   160,   162,   163,   164,   165,   167,   169,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   188,   192,   193,   194,   195,
     196,   197,   199,   200,   202,   203,   204,   205,   206,   207,
     209,   210,   212,   214,   215,   218,   219,   221,   222,   224,
     226,   228,   230,   231,   232,   233,   234,   236,   237,   239,
     246,   251,   255,   261,   265,   268
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
  "T_MAX", "T_MIN", "T_GRAPH", "T_DIR_GRAPH", "T_NODE", "T_EDGE", "T_NP",
  "T_EP", "T_LIST", "T_SET_NODES", "T_SET_EDGES", "T_ELEMENTS", "T_FROM",
  "T_BFS", "T_REVERSE", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "'?'",
  "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'",
  "','", "';'", "'{'", "'}'", "'='", "'.'", "$accept", "program",
  "function_def", "function_data", "paramList", "param", "function_body",
  "statements", "statement", "blockstatements", "block_begin", "block_end",
  "declaration", "type1", "primitive", "graph", "collections", "type2",
  "property", "assignment", "rhs", "expression", "proc_call", "val",
  "control_flow", "iteration_cf", "filterExpr", "boolean_expr",
  "selection_cf", "reduction", "leftList", "rightList", "reductionCall",
  "reduction_op", "leftSide", "arg_list", "bfs_abstraction",
  "reverse_abstraction", "oid", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,    63,    58,    60,
      62,    43,    45,    42,    47,    37,    40,    41,    44,    59,
     123,   125,    61,    46
};
# endif

#define YYPACT_NINF (-198)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-106)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      12,   -61,    30,  -198,   -46,   -38,  -198,  -198,  -198,  -198,
    -198,   156,    40,  -198,  -198,  -198,  -198,  -198,  -198,  -198,
    -198,  -198,   -28,   -14,  -198,   -10,     8,   -49,  -198,     0,
    -198,  -198,  -198,    18,  -198,   -19,    13,    26,    28,   -46,
      84,    31,    25,    50,  -198,  -198,  -198,  -198,    46,    55,
      56,    47,    48,  -198,  -198,  -198,    49,   -64,  -198,  -198,
     162,   162,    66,    76,  -198,   156,  -198,    67,    79,    82,
      72,    72,   130,    71,    86,    87,   -65,  -198,  -198,    88,
      89,  -198,  -198,  -198,    72,    68,    83,    94,   102,   104,
     105,   109,  -198,    91,   139,   161,  -198,  -198,  -198,  -198,
    -198,  -198,    72,   194,  -198,  -198,   110,   118,   119,   121,
      72,   127,  -198,   116,    50,    72,    72,  -198,   194,   -62,
     -45,  -198,  -198,  -198,  -198,  -198,  -198,   194,  -198,   123,
    -198,  -198,  -198,  -198,  -198,  -198,   125,   137,   140,   152,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,   -46,   -46,    72,   138,   167,   147,  -198,  -198,
    -198,    72,  -198,    72,    72,  -198,   145,   154,   165,  -198,
     206,   200,   -21,   -21,   212,   212,   -21,   -21,   -58,   -58,
    -198,  -198,   228,  -198,   171,   -46,   186,   -25,  -198,   194,
     -41,    50,   189,   -46,   -46,  -198,  -198,   176,   184,   177,
    -198,   173,   118,   -66,   173,  -198,  -198,   173,  -198,   112,
     235,   181,   182,   -46,  -198,   185,   -46,   -46,   198,    72,
    -198,  -198,   211,  -198,   213,    72,  -198,   214,   173,   -46,
    -198
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     0,     0,     1,    21,     3,    10,
      11,     0,     0,    30,    31,    32,    33,    34,    35,    36,
      40,    41,     0,     0,    37,     0,     0,     0,     5,     0,
      27,    28,    29,     0,    42,     0,     0,     0,     0,     0,
       0,     0,    97,     0,    22,    12,    19,    20,     0,     0,
       0,     0,     0,    16,    73,    72,     0,     0,    18,    98,
       0,     0,     0,     0,     4,     0,     7,     8,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    88,    13,    23,
      25,    14,    15,    17,     0,     0,     0,     0,     0,     0,
       0,     0,     6,     0,     0,     0,    70,    69,    71,    66,
      67,    68,     0,    82,    49,    63,     0,    64,     0,     0,
       0,     0,   105,     0,     0,     0,     0,   102,   101,    64,
       0,    92,    93,    94,    95,    96,    47,    48,    85,     0,
      43,    45,    44,    46,    38,    39,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    87,    24,
      26,     0,    65,     0,     0,     9,     0,    97,     0,    62,
      54,    55,    56,    57,    60,    61,    58,    59,    50,    51,
      52,    53,    83,    75,     0,     0,     0,     0,    99,   100,
       0,     0,     0,     0,     0,    76,    74,     0,     0,    90,
      91,    80,     0,    97,    80,    78,    84,    80,    86,     0,
       0,     0,     0,     0,    89,     0,     0,     0,     0,     0,
      77,    79,     0,   103,     0,     0,    81,     0,    80,     0,
     104
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -198,  -198,  -198,  -198,  -198,   197,  -198,  -198,  -198,   -12,
    -198,  -198,  -198,   276,     1,  -198,    39,   277,  -198,   -11,
      -5,   -76,    -6,    85,  -198,  -198,  -197,   -68,  -198,  -198,
    -198,  -198,   106,  -198,    -8,   128,  -198,  -198,  -198
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,    27,    28,     8,    12,    45,     9,
      10,    47,    48,    29,    30,    31,    32,    33,    34,   117,
     126,   103,   104,   105,    53,    54,   211,   106,    55,    56,
      76,   198,   128,   129,   107,   120,    58,   223,    59
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      46,    51,     5,   108,    57,   113,    52,   212,   118,   127,
     213,  -105,    84,   114,    84,   150,   151,    75,    85,   121,
     161,   122,   123,   124,   125,     1,   139,    72,    64,    65,
       6,   229,   162,   163,     7,    77,   200,   163,    11,   127,
     127,    60,   155,    13,    14,    15,    16,    17,    35,    36,
     148,   149,   150,   151,    37,    61,    38,    68,    39,    62,
      40,    86,    88,    66,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   119,    63,    96,    97,
      98,    67,    96,    97,    98,   127,   184,   189,   118,    69,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    87,
      89,    41,    70,    42,    71,    73,   158,    74,    75,    43,
     159,   160,   121,    42,   122,   123,   124,   125,    79,    80,
       7,    44,    96,    97,    98,    78,    81,    82,    83,    90,
     168,    42,    99,   100,   101,    42,    99,   100,   101,    91,
     182,   183,    94,    93,   102,    95,   109,   110,   102,   111,
     112,   224,   188,   130,   136,   119,   119,   227,   137,    13,
      14,    15,    16,    17,   131,    13,    14,    15,    16,    17,
     115,   116,   132,   196,   133,   134,    99,   100,   101,   135,
     138,   205,   206,   202,   202,   201,   204,   152,   140,   141,
     142,   143,   144,   145,    84,   156,   153,   154,   157,   164,
     166,   218,   165,   167,   220,   221,    18,    19,    20,    21,
      22,    23,    24,    25,    26,   185,   187,   230,    24,    25,
      26,   146,   147,   148,   149,   150,   151,   186,   191,   169,
     140,   141,   142,   143,   144,   145,   140,   192,   142,   143,
     144,   145,   193,   194,   142,   143,   144,   145,   195,   197,
     142,   143,   203,   207,   208,   209,   210,   215,   216,   217,
     222,   219,    92,   146,   147,   148,   149,   150,   151,   146,
     147,   148,   149,   150,   151,   146,   147,   148,   149,   150,
     151,   146,   147,   148,   149,   150,   151,   225,    49,    50,
     226,   228,   190,   199,   214
};

static const yytype_uint8 yycheck[] =
{
      12,    12,    63,    71,    12,    70,    12,   204,    84,    85,
     207,    77,    76,    78,    76,    73,    74,    83,    82,    44,
      82,    46,    47,    48,    49,    13,   102,    39,    77,    78,
       0,   228,    77,    78,    80,    43,    77,    78,    76,   115,
     116,    69,   110,     3,     4,     5,     6,     7,     8,     9,
      71,    72,    73,    74,    14,    69,    16,    76,    18,    69,
      20,    60,    61,    63,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,    84,    69,    10,    11,
      12,    63,    10,    11,    12,   161,   154,   163,   164,    76,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    60,
      61,    61,    76,    63,    76,    21,   114,    76,    83,    69,
     115,   116,    44,    63,    46,    47,    48,    49,    63,    63,
      80,    81,    10,    11,    12,    79,    79,    79,    79,    63,
     138,    63,    64,    65,    66,    63,    64,    65,    66,    63,
     152,   153,    63,    76,    76,    63,    16,    76,    76,    63,
      63,   219,   163,    70,    63,   163,   164,   225,    19,     3,
       4,     5,     6,     7,    70,     3,     4,     5,     6,     7,
      82,    82,    70,   185,    70,    70,    64,    65,    66,    70,
      19,   193,   194,   191,   192,   191,   192,    77,    36,    37,
      38,    39,    40,    41,    76,    68,    77,    76,    82,    76,
      63,   213,    77,    63,   216,   217,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    77,    69,   229,    56,    57,
      58,    69,    70,    71,    72,    73,    74,    60,    83,    77,
      36,    37,    38,    39,    40,    41,    36,    83,    38,    39,
      40,    41,    77,    15,    38,    39,    40,    41,    77,    63,
      38,    39,    63,    77,    70,    78,    83,    22,    77,    77,
      62,    76,    65,    69,    70,    71,    72,    73,    74,    69,
      70,    71,    72,    73,    74,    69,    70,    71,    72,    73,
      74,    69,    70,    71,    72,    73,    74,    76,    12,    12,
      77,    77,   164,   187,   209
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    13,    85,    86,    87,    63,     0,    80,    90,    93,
      94,    76,    91,     3,     4,     5,     6,     7,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    88,    89,    97,
      98,    99,   100,   101,   102,     8,     9,    14,    16,    18,
      20,    61,    63,    69,    81,    92,    93,    95,    96,    97,
     101,   103,   106,   108,   109,   112,   113,   118,   120,   122,
      69,    69,    69,    69,    77,    78,    63,    63,    76,    76,
      76,    76,    93,    21,    76,    83,   114,   118,    79,    63,
      63,    79,    79,    79,    76,    82,    98,   100,    98,   100,
      63,    63,    89,    76,    63,    63,    10,    11,    12,    64,
      65,    66,    76,   105,   106,   107,   111,   118,   111,    16,
      76,    63,    63,    70,    78,    82,    82,   103,   105,   118,
     119,    44,    46,    47,    48,    49,   104,   105,   116,   117,
      70,    70,    70,    70,    70,    70,    63,    19,    19,   105,
      36,    37,    38,    39,    40,    41,    69,    70,    71,    72,
      73,    74,    77,    77,    76,   111,    68,    82,   118,   104,
     104,    82,    77,    78,    76,    77,    63,    63,   118,    77,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,    93,    93,   111,    77,    60,    69,   103,   105,
     119,    83,    83,    77,    15,    77,    93,    63,   115,   116,
      77,   106,   118,    63,   106,    93,    93,    77,    70,    78,
      83,   110,   110,   110,   107,    22,    77,    77,    93,    76,
      93,    93,    62,   121,   111,    76,    77,   111,    77,   110,
      93
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    84,    85,    86,    87,    88,    88,    89,    89,    89,
      90,    91,    91,    92,    92,    92,    92,    92,    92,    92,
      93,    94,    95,    96,    96,    96,    96,    97,    97,    97,
      98,    98,    98,    98,    98,    99,    99,   100,   100,   100,
     101,   101,   101,   102,   102,   102,   102,   103,   104,   105,
     105,   105,   105,   105,   105,   105,   105,   105,   105,   105,
     105,   105,   105,   105,   105,   106,   107,   107,   107,   107,
     107,   107,   108,   108,   109,   109,   109,   109,   109,   109,
     110,   110,   111,   112,   112,   113,   113,   114,   114,   115,
     115,   116,   117,   117,   117,   117,   117,   118,   118,   119,
     119,   119,   119,   120,   121,   122
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     5,     1,     3,     2,     2,     5,
       1,     0,     2,     2,     2,     2,     1,     2,     1,     1,
       3,     1,     1,     2,     4,     2,     4,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     4,     4,
       1,     1,     1,     4,     4,     4,     4,     3,     1,     1,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     1,     1,     4,     1,     1,     1,     1,
       1,     1,     1,     1,     6,     5,     6,    10,     7,    10,
       0,     5,     1,     5,     7,     3,     7,     3,     1,     3,
       1,     4,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     1,     1,    10,     6,     3
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
  case 2:
#line 86 "dsl_yacc_modified.y"
                      {printf("program.\n");}
#line 1647 "y.tab.c"
    break;

  case 3:
#line 88 "dsl_yacc_modified.y"
                                            { }
#line 1653 "y.tab.c"
    break;

  case 4:
#line 90 "dsl_yacc_modified.y"
                                           {Identifier* funcId=createIdentifierNode((yyvsp[-3].text));
                                           (yyval.node)=createFunctionNode(funcId,(yyvsp[-1].pList)); }
#line 1660 "y.tab.c"
    break;

  case 5:
#line 93 "dsl_yacc_modified.y"
                 {(yyval.pList)=addToPList((yyval.pList),(yyvsp[0].node));}
#line 1666 "y.tab.c"
    break;

  case 6:
#line 94 "dsl_yacc_modified.y"
                                     {(yyval.pList)=addToPList((yyval.pList),(yyvsp[0].node)); }
#line 1672 "y.tab.c"
    break;

  case 7:
#line 96 "dsl_yacc_modified.y"
                 { Identifier* id=createIdentifierNode((yyvsp[0].text));
                           (yyval.node)=createParamNode((yyvsp[-1].node),id); }
#line 1679 "y.tab.c"
    break;

  case 8:
#line 98 "dsl_yacc_modified.y"
                          {  Identifier* id=createIdentifierNode((yyvsp[0].text));
                             (yyval.node)=createParamNode((yyvsp[-1].node),id);}
#line 1686 "y.tab.c"
    break;

  case 9:
#line 100 "dsl_yacc_modified.y"
                                                 {  Identifier* id1=createIdentifierNode((yyvsp[-1].text));
			                            Identifier* id=createIdentifierNode((yyvsp[-3].text));
				                        Type* tempType=(Type*)(yyvsp[-4].node);
			                            if(tempType->isNodeEdgeType())
										   (Type*)(yyvsp[-4].node)->addSourceGraph(id1);
				                         (yyval.node)=createParamNode((yyvsp[-4].node),id);
									 }
#line 1698 "y.tab.c"
    break;

  case 10:
#line 109 "dsl_yacc_modified.y"
                                {(yyval.node)=(yyvsp[0].node);}
#line 1704 "y.tab.c"
    break;

  case 11:
#line 112 "dsl_yacc_modified.y"
              {}
#line 1710 "y.tab.c"
    break;

  case 12:
#line 113 "dsl_yacc_modified.y"
                               { addToBlock((yyvsp[0].node)); }
#line 1716 "y.tab.c"
    break;

  case 13:
#line 115 "dsl_yacc_modified.y"
                          {(yyval.node)=(yyvsp[-1].node)}
#line 1722 "y.tab.c"
    break;

  case 14:
#line 116 "dsl_yacc_modified.y"
                       {(yyval.node)=(yyvsp[-1].node);}
#line 1728 "y.tab.c"
    break;

  case 15:
#line 117 "dsl_yacc_modified.y"
                       {(yyval.node)=createNodeForProcCallStmt((yyvsp[-1].node));}
#line 1734 "y.tab.c"
    break;

  case 16:
#line 118 "dsl_yacc_modified.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 1740 "y.tab.c"
    break;

  case 17:
#line 119 "dsl_yacc_modified.y"
                      {(yyval.node)=(yyvsp[-1].node);}
#line 1746 "y.tab.c"
    break;

  case 18:
#line 120 "dsl_yacc_modified.y"
                          {(yyval.node)=(yyvsp[0].node); }
#line 1752 "y.tab.c"
    break;

  case 19:
#line 121 "dsl_yacc_modified.y"
                          {(yyval.node)=(yyvsp[0].node);}
#line 1758 "y.tab.c"
    break;

  case 20:
#line 124 "dsl_yacc_modified.y"
                                                   { (yyval.node)=finishBlock();}
#line 1764 "y.tab.c"
    break;

  case 21:
#line 126 "dsl_yacc_modified.y"
                { createNewBlock(); }
#line 1770 "y.tab.c"
    break;

  case 23:
#line 131 "dsl_yacc_modified.y"
                        {Identifier* id=createIdentifierNode((yyvsp[0].text));
                         (yyval.node)=createNormalDeclNode((yyvsp[-1].node),id);}
#line 1777 "y.tab.c"
    break;

  case 24:
#line 133 "dsl_yacc_modified.y"
                           {Identifier* id=createIdentifierNode((yyvsp[-2].text));
	                    (yyval.node)=createAssignedDeclNode((yyvsp[-3].node),id,(yyvsp[0].node));}
#line 1784 "y.tab.c"
    break;

  case 25:
#line 135 "dsl_yacc_modified.y"
                    {Identifier* id=createIdentifierNode((yyvsp[0].text));
                         (yyval.node)=createNormalDeclNode((yyvsp[-1].node),id); }
#line 1791 "y.tab.c"
    break;

  case 26:
#line 137 "dsl_yacc_modified.y"
                           {Identifier* id=createIdentifierNode((yyvsp[-2].text));
	                    (yyval.node)=createAssignedDeclNode((yyvsp[-3].node),id,(yyvsp[0].node));}
#line 1798 "y.tab.c"
    break;

  case 27:
#line 140 "dsl_yacc_modified.y"
                 {(yyval.node)=(yyvsp[0].node)}
#line 1804 "y.tab.c"
    break;

  case 28:
#line 141 "dsl_yacc_modified.y"
                {(yyval.node)=(yyvsp[0].node)}
#line 1810 "y.tab.c"
    break;

  case 29:
#line 142 "dsl_yacc_modified.y"
                      { (yyval.node)=(yyvsp[0].node)}
#line 1816 "y.tab.c"
    break;

  case 30:
#line 145 "dsl_yacc_modified.y"
                 { (yyval.node)=createPrimitiveTypeNode(TYPE_INT);}
#line 1822 "y.tab.c"
    break;

  case 31:
#line 146 "dsl_yacc_modified.y"
                  { (yyval.node)=createPrimitiveTypeNode(TYPE_FLOAT);}
#line 1828 "y.tab.c"
    break;

  case 32:
#line 147 "dsl_yacc_modified.y"
                 { (yyval.node)=createPrimitiveTypeNode(TYPE_BOOL);}
#line 1834 "y.tab.c"
    break;

  case 33:
#line 148 "dsl_yacc_modified.y"
                   { (yyval.node)=createPrimitiveTypeNode(TYPE_DOUBLE); }
#line 1840 "y.tab.c"
    break;

  case 34:
#line 149 "dsl_yacc_modified.y"
             {(yyval.node)=(yyval.node)=createPrimitiveTypeNode(TYPE_LONG);}
#line 1846 "y.tab.c"
    break;

  case 35:
#line 151 "dsl_yacc_modified.y"
                { (yyval.node)=createGraphTypeNode(TYPE_GRAPH,NULL);}
#line 1852 "y.tab.c"
    break;

  case 36:
#line 152 "dsl_yacc_modified.y"
                     {(yyval.node)=createGraphTypeNode(TYPE_DIRGRAPH,NULL);}
#line 1858 "y.tab.c"
    break;

  case 37:
#line 154 "dsl_yacc_modified.y"
                     { (yyval.node)=createCollectionTypeNode(TYPE_LIST,NULL);}
#line 1864 "y.tab.c"
    break;

  case 38:
#line 155 "dsl_yacc_modified.y"
                                        { (yyval.node)=createCollectionTypeNode(TYPE_SETN,(yyvsp[-1].text));}
#line 1870 "y.tab.c"
    break;

  case 39:
#line 156 "dsl_yacc_modified.y"
                                         { (yyval.node)=createCollectionTypeNode(TYPE_SETE,(yyvsp[-1].text));}
#line 1876 "y.tab.c"
    break;

  case 40:
#line 158 "dsl_yacc_modified.y"
               {(yyval.node)=createNodeEdgeTypeNode(TYPE_NODE) }
#line 1882 "y.tab.c"
    break;

  case 41:
#line 159 "dsl_yacc_modified.y"
                {(yyval.node)=createNodeEdgeTypeNode(TYPE_EDGE)}
#line 1888 "y.tab.c"
    break;

  case 42:
#line 160 "dsl_yacc_modified.y"
                      {(yyval.node)=(yyvsp[0].node);}
#line 1894 "y.tab.c"
    break;

  case 43:
#line 162 "dsl_yacc_modified.y"
                                  { (yyval.node)=createPropertyTypeNode(PROP_NODE,(yyvsp[-1].node)); }
#line 1900 "y.tab.c"
    break;

  case 44:
#line 163 "dsl_yacc_modified.y"
                                       { (yyval.node)=createPropertyTypeNode(PROP_EDGE,(yyvsp[-1].node)); }
#line 1906 "y.tab.c"
    break;

  case 45:
#line 164 "dsl_yacc_modified.y"
                                                    {  (yyval.node)=createPropertyTypeNode(PROP_NODE,(yyvsp[-1].node)); }
#line 1912 "y.tab.c"
    break;

  case 46:
#line 165 "dsl_yacc_modified.y"
                                                     {(yyval.node)=createPropertyTypeNode(PROP_EDGE,(yyvsp[-1].node));}
#line 1918 "y.tab.c"
    break;

  case 47:
#line 167 "dsl_yacc_modified.y"
                                { (yyval.node)=createAssignmentNode((yyvsp[-2].node),(yyvsp[0].node));}
#line 1924 "y.tab.c"
    break;

  case 48:
#line 169 "dsl_yacc_modified.y"
                 { (yyval.node)=(yyvsp[0].node);}
#line 1930 "y.tab.c"
    break;

  case 49:
#line 171 "dsl_yacc_modified.y"
                       { (yyval.node)=(yyvsp[0].node)}
#line 1936 "y.tab.c"
    break;

  case 50:
#line 172 "dsl_yacc_modified.y"
                                         { (yyval.node)=createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_ADD);}
#line 1942 "y.tab.c"
    break;

  case 51:
#line 173 "dsl_yacc_modified.y"
                                             { (yyval.node)=createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_SUB);}
#line 1948 "y.tab.c"
    break;

  case 52:
#line 174 "dsl_yacc_modified.y"
                                             {(yyval.node)=createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_MUL);}
#line 1954 "y.tab.c"
    break;

  case 53:
#line 175 "dsl_yacc_modified.y"
                                           {(yyval.node)=createNodeForArithmeticExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_DIV);}
#line 1960 "y.tab.c"
    break;

  case 54:
#line 176 "dsl_yacc_modified.y"
                                              {(yyval.node)=createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_AND);}
#line 1966 "y.tab.c"
    break;

  case 55:
#line 177 "dsl_yacc_modified.y"
                                                  {(yyval.node)=createNodeForLogicalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_OR);}
#line 1972 "y.tab.c"
    break;

  case 56:
#line 178 "dsl_yacc_modified.y"
                                                 {(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LE);}
#line 1978 "y.tab.c"
    break;

  case 57:
#line 179 "dsl_yacc_modified.y"
                                                {(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GE);}
#line 1984 "y.tab.c"
    break;

  case 58:
#line 180 "dsl_yacc_modified.y"
                                                    {(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_LT);}
#line 1990 "y.tab.c"
    break;

  case 59:
#line 181 "dsl_yacc_modified.y"
                                                    {(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_GT);}
#line 1996 "y.tab.c"
    break;

  case 60:
#line 182 "dsl_yacc_modified.y"
                                                        {(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_EQ);}
#line 2002 "y.tab.c"
    break;

  case 61:
#line 183 "dsl_yacc_modified.y"
                                            {(yyval.node)=(yyval.node)=createNodeForRelationalExpr((yyvsp[-2].node),(yyvsp[0].node),OPERATOR_NE);}
#line 2008 "y.tab.c"
    break;

  case 62:
#line 184 "dsl_yacc_modified.y"
                                     {(yyval.node)=(yyvsp[-1].node);}
#line 2014 "y.tab.c"
    break;

  case 63:
#line 185 "dsl_yacc_modified.y"
                       {(yyval.node)=(yyvsp[0].node);}
#line 2020 "y.tab.c"
    break;

  case 64:
#line 186 "dsl_yacc_modified.y"
                                    { (yyval.node)=(yyvsp[0].node) ;}
#line 2026 "y.tab.c"
    break;

  case 65:
#line 188 "dsl_yacc_modified.y"
                                      { (yyval.node)=createNodeForProcCall((yyvsp[-3].node),(yyvsp[-1].aList)); }
#line 2032 "y.tab.c"
    break;

  case 66:
#line 192 "dsl_yacc_modified.y"
              { (yyval.node)=createNodeForIval((yyvsp[0].ival)); }
#line 2038 "y.tab.c"
    break;

  case 67:
#line 193 "dsl_yacc_modified.y"
                    {(yyval.node)=createNodeForFval((yyvsp[0].fval));}
#line 2044 "y.tab.c"
    break;

  case 68:
#line 194 "dsl_yacc_modified.y"
                   {(yyval.node)=createNodeForBval((yyvsp[0].bval));}
#line 2050 "y.tab.c"
    break;

  case 69:
#line 195 "dsl_yacc_modified.y"
                {(yyval.node)=createNodeForINF(true);}
#line 2056 "y.tab.c"
    break;

  case 70:
#line 196 "dsl_yacc_modified.y"
                  {(yyval.node)=createNodeForINF(true);}
#line 2062 "y.tab.c"
    break;

  case 71:
#line 197 "dsl_yacc_modified.y"
                  {(yyval.node)=createNodeForINF(false);}
#line 2068 "y.tab.c"
    break;

  case 72:
#line 199 "dsl_yacc_modified.y"
                            { (yyval.node)=(yyvsp[0].node); }
#line 2074 "y.tab.c"
    break;

  case 73:
#line 200 "dsl_yacc_modified.y"
                             { (yyval.node)=(yyvsp[0].node); }
#line 2080 "y.tab.c"
    break;

  case 74:
#line 202 "dsl_yacc_modified.y"
                                                                         { (yyval.node)=createNodeForFixedPointStmt((yyvsp[-2].node),(yyvsp[0].node));}
#line 2086 "y.tab.c"
    break;

  case 75:
#line 203 "dsl_yacc_modified.y"
                                                                 {(yyval.node)=createNodeForWhileStmt((yyvsp[-2].node),(yyvsp[0].node)); }
#line 2092 "y.tab.c"
    break;

  case 76:
#line 204 "dsl_yacc_modified.y"
                                                                       {(yyval.node)=createNodeForDoWhileStmt((yyvsp[-1].node),(yyvsp[-4].node));  }
#line 2098 "y.tab.c"
    break;

  case 77:
#line 205 "dsl_yacc_modified.y"
                                                                                       {(yyval.node)=createNodeForForAllStmt((yyvsp[-7].text),(yyvsp[-5].text),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),true);}
#line 2104 "y.tab.c"
    break;

  case 78:
#line 206 "dsl_yacc_modified.y"
                                                                 {(yyval.node)=createNodeForForStmt((yyvsp[-4].text),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2110 "y.tab.c"
    break;

  case 79:
#line 207 "dsl_yacc_modified.y"
                                                                                    {(yyval.node)=createNodeForForAllStmt((yyvsp[-7].text),(yyvsp[-5].text),(yyvsp[-3].node),(yyvsp[-2].node),(yyvsp[0].node),false);}
#line 2116 "y.tab.c"
    break;

  case 80:
#line 209 "dsl_yacc_modified.y"
                      { (yyval.node)=NULL;}
#line 2122 "y.tab.c"
    break;

  case 81:
#line 210 "dsl_yacc_modified.y"
                                              { (yyval.node)=(yyvsp[-1].node);}
#line 2128 "y.tab.c"
    break;

  case 82:
#line 212 "dsl_yacc_modified.y"
                          { (yyval.node)=(yyvsp[0].node) ;}
#line 2134 "y.tab.c"
    break;

  case 83:
#line 214 "dsl_yacc_modified.y"
                                                         { (yyval.node)=createNodeForIfStmt((yyvsp[-2].node),(yyvsp[0].node),NULL); }
#line 2140 "y.tab.c"
    break;

  case 84:
#line 215 "dsl_yacc_modified.y"
                                                                                  {(yyval.node)=createNodeForIfStmt((yyvsp[-4].node),(yyvsp[-2].node),(yyvsp[0].node)); }
#line 2146 "y.tab.c"
    break;

  case 85:
#line 218 "dsl_yacc_modified.y"
                                       { (yyval.node)=createNodeForReductionStmt((yyvsp[-2].node),(yyvsp[0].node)) }
#line 2152 "y.tab.c"
    break;

  case 86:
#line 219 "dsl_yacc_modified.y"
                                                            {(yyval.node)=createNodeForReductionStmtList((yyvsp[-5].nodeList),(yyvsp[-1].temporary).reducCall,(yyvsp[-1].temporary).exprVal);}
#line 2158 "y.tab.c"
    break;

  case 87:
#line 221 "dsl_yacc_modified.y"
                                  { (yyval.nodeList)=addToNList((yyval.nodeList),(yyvsp[0].node));}
#line 2164 "y.tab.c"
    break;

  case 88:
#line 222 "dsl_yacc_modified.y"
                            { (yyval.nodeList)=addToNList((yyval.nodeList),(yyvsp[0].node));}
#line 2170 "y.tab.c"
    break;

  case 89:
#line 224 "dsl_yacc_modified.y"
                                  { (yyval.temporary).reducCall=(yyvsp[-2].node);
                                    (yyval.temporary).exprVal=(yyvsp[0].node); }
#line 2177 "y.tab.c"
    break;

  case 90:
#line 226 "dsl_yacc_modified.y"
                          {(yyval.temporary).reducCall=(yyvsp[0].node);}
#line 2183 "y.tab.c"
    break;

  case 91:
#line 228 "dsl_yacc_modified.y"
                                              {(yyval.node)=createNodeforReductionCall((yyvsp[-3].ival),(yyvsp[-1].aList));}
#line 2189 "y.tab.c"
    break;

  case 92:
#line 230 "dsl_yacc_modified.y"
                     { (yyval.ival)=REDUCE_SUM;}
#line 2195 "y.tab.c"
    break;

  case 93:
#line 231 "dsl_yacc_modified.y"
                           {(yyval.ival)=REDUCE_COUNT;}
#line 2201 "y.tab.c"
    break;

  case 94:
#line 232 "dsl_yacc_modified.y"
                             {(yyval.ival)=REDUCE_PRODUCT;}
#line 2207 "y.tab.c"
    break;

  case 95:
#line 233 "dsl_yacc_modified.y"
                         {(yyval.ival)=REDUCE_MAX;}
#line 2213 "y.tab.c"
    break;

  case 96:
#line 234 "dsl_yacc_modified.y"
                         {(yyval.ival)=REDUCE_MIN;}
#line 2219 "y.tab.c"
    break;

  case 97:
#line 236 "dsl_yacc_modified.y"
              { (yyval.node)=createIdentifierNode((yyvsp[0].text));}
#line 2225 "y.tab.c"
    break;

  case 98:
#line 237 "dsl_yacc_modified.y"
               { (yyval.node)=(yyvsp[0].node); }
#line 2231 "y.tab.c"
    break;

  case 99:
#line 239 "dsl_yacc_modified.y"
                                     {argument a1;
		                 a1.expression=(yyvsp[-2].aList);
						 a1.expressionflag=true;
						 (yyval.aList)=addToAList((yyval.aList),a1);
                                 }
#line 2241 "y.tab.c"
    break;

  case 100:
#line 246 "dsl_yacc_modified.y"
                                           {argument a1;
		                                a1.assignExpr=(yyvsp[-2].aList);
						                a1.assign=true;
										 (yyval.aList)=addToAList((yyval.aList),a1);
						                }
#line 2251 "y.tab.c"
    break;

  case 101:
#line 251 "dsl_yacc_modified.y"
                            {argument a1;
		                 a1.expression=(yyvsp[0].node);
						 a1.expressionflag=true;
						  (yyval.aList)=addToAList((yyval.aList),a1); }
#line 2260 "y.tab.c"
    break;

  case 102:
#line 255 "dsl_yacc_modified.y"
                            { argument a1;
		                  a1.assignExpr=(yyvsp[0].node);
						  a1.assign=true;
						   (yyval.aList)=addToAList((yyval.aList),a1);}
#line 2269 "y.tab.c"
    break;

  case 103:
#line 261 "dsl_yacc_modified.y"
                                                                                               {(yyval.node)=createIterateInBFSNode((yyvsp[-7].text),(yyvsp[-4].text),(yyvsp[-2].node),(yyvsp[-1].node),(yyvsp[0].node)) ;}
#line 2275 "y.tab.c"
    break;

  case 104:
#line 265 "dsl_yacc_modified.y"
                                                                                {(yyval.node)=createIterateInReverseBFSNode((yyvsp[-3].node),(yyvsp[-1].node),(yyvsp[0].node))}
#line 2281 "y.tab.c"
    break;

  case 105:
#line 268 "dsl_yacc_modified.y"
                { Identifier* id1=createIdentifierNode((yyvsp[-2].text));
                   Identifier* id12=createIdentifierNode((yyvsp[-2].text));
				   (yyval.node)=createPropIdNode(id1,id2);
				    }
#line 2290 "y.tab.c"
    break;


#line 2294 "y.tab.c"

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
#line 273 "dsl_yacc_modified.y"



void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}

