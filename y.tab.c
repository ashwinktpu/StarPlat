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
	void yyerror(char *);
	int yylex(void);
	char mytext[100];
	char var[100];
	int num = 0;
	extern char *yytext;

#line 83 "y.tab.c"

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
    T_IF = 268,
    T_ELSE = 269,
    T_WHILE = 270,
    T_RETURN = 271,
    T_DO = 272,
    T_IN = 273,
    T_FIXEDPOINT = 274,
    T_UNTIL = 275,
    T_FUNC = 276,
    T_ADD_ASSIGN = 277,
    T_SUB_ASSIGN = 278,
    T_MUL_ASSIGN = 279,
    T_DIV_ASSIGN = 280,
    T_MOD_ASSIGN = 281,
    T_AND_ASSIGN = 282,
    T_XOR_ASSIGN = 283,
    T_OR_ASSIGN = 284,
    T_RIGHT_OP = 285,
    T_LEFT_OP = 286,
    T_INC_OP = 287,
    T_DEC_OP = 288,
    T_PTR_OP = 289,
    T_AND_OP = 290,
    T_OR_OP = 291,
    T_LE_OP = 292,
    T_GE_OP = 293,
    T_EQ_OP = 294,
    T_NE_OP = 295,
    T_AND = 296,
    T_OR = 297,
    T_SUM = 298,
    T_AVG = 299,
    T_COUNT = 300,
    T_PRODUCT = 301,
    T_MAX = 302,
    T_MIN = 303,
    T_GRAPH = 304,
    T_DIR_GRAPH = 305,
    T_NODE = 306,
    T_EDGE = 307,
    T_NP = 308,
    T_EP = 309,
    T_SET_NODES = 310,
    T_SET_EDGES = 311,
    T_ELEMENTS = 312,
    T_LIST = 313,
    T_FROM = 314,
    T_BFS = 315,
    T_REVERSE = 316,
    ID = 317,
    INT_NUM = 318,
    FLOAT_NUM = 319,
    BOOL_VAL = 320
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
#define T_IF 268
#define T_ELSE 269
#define T_WHILE 270
#define T_RETURN 271
#define T_DO 272
#define T_IN 273
#define T_FIXEDPOINT 274
#define T_UNTIL 275
#define T_FUNC 276
#define T_ADD_ASSIGN 277
#define T_SUB_ASSIGN 278
#define T_MUL_ASSIGN 279
#define T_DIV_ASSIGN 280
#define T_MOD_ASSIGN 281
#define T_AND_ASSIGN 282
#define T_XOR_ASSIGN 283
#define T_OR_ASSIGN 284
#define T_RIGHT_OP 285
#define T_LEFT_OP 286
#define T_INC_OP 287
#define T_DEC_OP 288
#define T_PTR_OP 289
#define T_AND_OP 290
#define T_OR_OP 291
#define T_LE_OP 292
#define T_GE_OP 293
#define T_EQ_OP 294
#define T_NE_OP 295
#define T_AND 296
#define T_OR 297
#define T_SUM 298
#define T_AVG 299
#define T_COUNT 300
#define T_PRODUCT 301
#define T_MAX 302
#define T_MIN 303
#define T_GRAPH 304
#define T_DIR_GRAPH 305
#define T_NODE 306
#define T_EDGE 307
#define T_NP 308
#define T_EP 309
#define T_SET_NODES 310
#define T_SET_EDGES 311
#define T_ELEMENTS 312
#define T_LIST 313
#define T_FROM 314
#define T_BFS 315
#define T_REVERSE 316
#define ID 317
#define INT_NUM 318
#define FLOAT_NUM 319
#define BOOL_VAL 320

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 16 "dsl_yacc_modified.y"

    int  info;
    long ival;
    double fval;
    char* text;
    //bool bval;
    //ast_node* ptr;
    //expr_list* e_list;  // list of expressions
    //lhs_list* l_list;   // list of lhs
    //struct parse_temp_t pair;

#line 274 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);





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
#define YYLAST   544

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  224

#define YYUNDEFTOK  2
#define YYMAXUTOK   320


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
       2,     2,     2,     2,     2,     2,     2,    74,     2,     2,
      75,    76,    72,    70,    82,    71,    81,    73,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    67,    77,
      68,    80,    69,    66,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,     2,    79,     2,     2,     2,     2,
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
      65
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,    58,    58,    60,    62,    69,    72,    73,    75,    76,
      77,    78,    79,    80,    81,    84,    85,    87,    88,    89,
      90,    92,    93,    94,    96,    97,    98,    99,   100,   102,
     103,   105,   106,   107,   109,   110,   111,   113,   114,   115,
     116,   118,   120,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,   132,   133,   134,   135,   137,   138,   139,
     142,   143,   144,   145,   146,   147,   149,   150,   152,   153,
     154,   155,   156,   157,   158,   160,   162,   164,   165,   168,
     169,   171,   172,   174,   175,   177,   179,   180,   181,   182,
     183,   185,   186,   188,   189,   191,   192,   193,   194,   195,
     196,   199,   200,   204,   205,   207
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_INT", "T_FLOAT", "T_BOOL", "T_DOUBLE",
  "T_LONG", "T_FORALL", "T_FOR", "T_P_INF", "T_INF", "T_N_INF", "T_IF",
  "T_ELSE", "T_WHILE", "T_RETURN", "T_DO", "T_IN", "T_FIXEDPOINT",
  "T_UNTIL", "T_FUNC", "T_ADD_ASSIGN", "T_SUB_ASSIGN", "T_MUL_ASSIGN",
  "T_DIV_ASSIGN", "T_MOD_ASSIGN", "T_AND_ASSIGN", "T_XOR_ASSIGN",
  "T_OR_ASSIGN", "T_RIGHT_OP", "T_LEFT_OP", "T_INC_OP", "T_DEC_OP",
  "T_PTR_OP", "T_AND_OP", "T_OR_OP", "T_LE_OP", "T_GE_OP", "T_EQ_OP",
  "T_NE_OP", "T_AND", "T_OR", "T_SUM", "T_AVG", "T_COUNT", "T_PRODUCT",
  "T_MAX", "T_MIN", "T_GRAPH", "T_DIR_GRAPH", "T_NODE", "T_EDGE", "T_NP",
  "T_EP", "T_SET_NODES", "T_SET_EDGES", "T_ELEMENTS", "T_LIST", "T_FROM",
  "T_BFS", "T_REVERSE", "ID", "INT_NUM", "FLOAT_NUM", "BOOL_VAL", "'?'",
  "':'", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'", "'%'", "'('", "')'",
  "';'", "'{'", "'}'", "'='", "'.'", "','", "$accept", "program",
  "function_def", "function_data", "function_body", "statements",
  "statement", "blockstatements", "declaration", "type1", "primitive",
  "graph", "collections", "type2", "property", "assignment", "rhs",
  "expression", "proc_call", "val", "control_flow", "iteration_cf",
  "filterExpr", "boolean_expr", "selection_cf", "reduction", "leftList",
  "rightList", "reductionCall", "reduction_op", "leftSide", "arg_list",
  "bfs_abstraction", "reverse_abstraction", "oid", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,    63,    58,    60,    62,
      43,    45,    42,    47,    37,    40,    41,    59,   123,   125,
      61,    46,    44
};
# endif

#define YYPACT_NINF (-193)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     -20,   -18,    51,  -193,   344,   -23,  -193,  -193,  -193,  -193,
    -193,  -193,   -10,    -9,    -8,    -1,   344,    58,  -193,  -193,
    -193,  -193,    12,    13,    21,    28,  -193,     4,   -61,    16,
      44,   428,  -193,  -193,  -193,    31,    47,  -193,  -193,  -193,
      48,  -193,    38,    40,  -193,  -193,  -193,    42,   -50,  -193,
    -193,    33,   365,    67,    70,     7,     7,   105,    63,   121,
     121,    79,    81,    83,   365,    44,   344,    92,    86,    74,
    -193,    78,   428,  -193,    80,    82,  -193,  -193,  -193,   365,
     452,    96,  -193,  -193,  -193,  -193,  -193,  -193,     7,    85,
      93,   238,  -193,  -193,   -67,    87,   143,   146,   421,    98,
     103,   111,   114,   365,    99,   122,   124,   130,   132,   133,
     127,   128,  -193,   103,  -193,  -193,   123,    44,  -193,  -193,
       7,     7,   131,  -193,  -193,  -193,  -193,  -193,  -193,   421,
    -193,   134,   135,   431,   365,   365,     7,     7,   144,     7,
       7,     7,     7,     7,     7,     7,   365,     7,   365,  -193,
      44,    44,   428,   344,     7,   138,  -193,  -193,  -193,  -193,
    -193,  -193,   157,   344,   154,  -193,  -193,  -193,   142,   365,
     365,  -193,  -193,  -193,   113,   471,  -193,   -34,    64,    64,
     -62,   -62,  -193,  -193,  -193,  -193,   148,   149,   -43,   212,
    -193,   151,   344,   166,  -193,    11,    44,   156,   158,   277,
     344,   344,   428,  -193,  -193,   163,   161,   159,  -193,  -193,
    -193,  -193,   344,  -193,  -193,  -193,   277,  -193,    30,  -193,
    -193,   344,  -193,  -193
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     0,     0,     1,    24,    25,    26,
      27,    28,     0,     0,     0,     0,     0,     0,    29,    30,
      34,    35,     0,     0,     0,     0,    31,     0,     0,    91,
       0,     0,     3,    16,     5,     0,     0,    21,    22,    23,
       0,    36,     0,     0,    11,    67,    66,     0,     0,    13,
      14,    92,    93,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    93,     0,     0,     0,     0,    82,
      92,     0,     7,     8,    17,    19,     9,    10,    12,    93,
       0,     0,    64,    63,    65,    60,    61,    62,     0,    98,
     100,    99,    43,    55,    56,     0,     0,     0,    76,     0,
      56,     0,     0,    93,     0,     0,     0,     0,     0,     0,
       0,     0,    75,     0,   104,   105,     0,     0,    15,     6,
       0,     0,     0,    86,    87,    88,    89,    90,    41,    42,
      79,     0,     0,     0,    93,    93,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    93,     0,    93,     4,
       0,     0,     0,     0,     0,     0,    37,    39,    38,    40,
      32,    33,     0,     0,     0,    81,    18,    20,    57,    93,
      93,    54,    97,    95,    48,    49,    50,    51,    52,    53,
      44,    45,    46,    47,    96,    94,     0,     0,     0,    77,
      69,     0,     0,     0,   103,     0,     0,     0,     0,     0,
       0,     0,     0,    70,    68,     0,     0,    84,    58,    85,
      59,    71,     0,    74,    73,    78,     0,    80,     0,    72,
     101,     0,    83,   102
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -193,  -193,  -193,  -193,  -193,   -69,   -29,    37,   -48,  -193,
     -31,  -193,   -13,  -193,  -193,   -30,   -57,   125,    19,    15,
    -193,  -193,  -192,   -47,  -193,  -193,   126,  -193,    49,  -193,
      -4,   -58,  -193,  -193,   -25
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,     4,    32,    71,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,   128,    91,    92,    93,
      44,    45,    66,    99,    46,    47,    68,   206,   130,   131,
     100,    95,    49,    50,    51
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      48,     1,    72,   119,    89,    70,   111,   212,    79,   101,
     144,   145,    48,   147,    64,   148,    89,    82,    83,    84,
      65,   122,    90,    43,   221,    79,    69,    48,   104,   106,
      80,    89,    79,   201,    90,    43,   142,   143,   144,   145,
      82,    83,    84,    72,     5,   155,   105,   107,    94,    90,
      43,     6,    52,    57,   123,    89,   124,   125,   126,   127,
      94,   113,    48,   166,   167,    53,    54,    55,    48,    29,
      85,    86,    87,    90,    56,    94,   172,   173,    58,    63,
      59,    60,    88,   189,   112,    43,    89,    89,   184,    61,
     185,    43,    70,    85,    86,    87,    62,    67,    89,    94,
      89,   138,   139,   114,    90,    90,    29,   191,    73,    74,
      75,   197,   198,    69,    81,    76,    90,    77,    90,    78,
     102,    89,    89,    72,     7,     8,     9,    10,    11,    96,
      94,    94,    97,   215,   142,   143,   144,   145,   103,    90,
      90,   108,    94,   109,    94,   110,   113,   188,    48,    48,
     138,   139,   140,   141,   115,   116,   117,   118,   132,    48,
     120,   150,   121,   149,   151,    94,    94,   134,   156,   186,
     187,    43,    43,    72,   152,   135,    24,    25,    79,    26,
      98,    98,    43,   142,   143,   144,   145,   153,    48,   154,
     190,   157,   113,   158,   162,    48,    48,    48,    48,   159,
     194,   160,   161,   164,   163,   129,   176,   168,    48,   169,
     170,    43,    48,   133,   192,   208,   193,    48,    43,    43,
      43,    43,   195,   196,   199,   200,   202,   203,   205,   204,
     217,    43,   209,   222,   210,    43,   211,   213,   214,   216,
      43,   218,     0,   165,   207,   129,   129,     0,     0,   219,
       0,     0,     0,   220,     0,     0,     0,     0,   223,     0,
       0,   174,   175,     0,   177,   178,   179,   180,   181,   182,
     183,     0,   129,   136,   137,   138,   139,   140,   141,    98,
       7,     8,     9,    10,    11,    12,    13,     0,     0,     0,
      14,     0,    15,     0,    16,     0,    17,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   142,   143,
     144,   145,     0,     0,     0,     0,     0,     0,     0,     0,
     146,     0,     0,     0,     0,     0,    18,    19,    20,    21,
      22,    23,    24,    25,     0,    26,     0,    27,    28,    29,
       0,     0,     0,     0,     0,    30,     0,     7,     8,     9,
      10,    11,    12,    13,     0,    31,     0,    14,    65,    15,
       0,    16,     0,    17,     0,     0,     0,     0,     7,     8,
       9,    10,    11,     0,     0,    82,    83,    84,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    18,    19,    20,    21,    22,    23,    24,
      25,     0,    26,     0,    27,    28,    29,     0,     0,     0,
       0,     0,    30,     0,    18,    19,    20,    21,    22,    23,
      24,    25,    31,    26,     0,     0,     0,    29,    85,    86,
      87,     7,     8,     9,    10,    11,    12,    13,     0,     0,
      88,    14,     0,    15,     0,    16,     0,    17,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   141,    82,    83,    84,     0,   136,   137,   138,   139,
     140,   141,     0,     0,     0,     0,     0,    18,    19,    20,
      21,    22,    23,    24,    25,     0,    26,     0,    27,    28,
      29,   142,   143,   144,   145,   123,    30,   124,   125,   126,
     127,   142,   143,   144,   145,     0,   136,   171,   138,   139,
     140,   141,     0,     0,    29,    85,    86,    87,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    88,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   142,   143,   144,   145
};

static const yytype_int16 yycheck[] =
{
       4,    21,    31,    72,    52,    30,    64,   199,    75,    56,
      72,    73,    16,    80,    75,    82,    64,    10,    11,    12,
      81,    79,    52,     4,   216,    75,    30,    31,    59,    60,
      80,    79,    75,    76,    64,    16,    70,    71,    72,    73,
      10,    11,    12,    72,    62,   103,    59,    60,    52,    79,
      31,     0,    75,    16,    43,   103,    45,    46,    47,    48,
      64,    65,    66,   120,   121,    75,    75,    75,    72,    62,
      63,    64,    65,   103,    75,    79,   134,   135,    20,    75,
      68,    68,    75,   152,    65,    66,   134,   135,   146,    68,
     148,    72,   117,    63,    64,    65,    68,    81,   146,   103,
     148,    37,    38,    66,   134,   135,    62,   154,    77,    62,
      62,   169,   170,   117,    81,    77,   146,    77,   148,    77,
      15,   169,   170,   152,     3,     4,     5,     6,     7,    62,
     134,   135,    62,   202,    70,    71,    72,    73,    75,   169,
     170,    62,   146,    62,   148,    62,   150,   151,   152,   153,
      37,    38,    39,    40,    62,    69,    82,    79,    62,   163,
      80,    18,    80,    76,    18,   169,   170,    82,    69,   150,
     151,   152,   153,   202,    76,    82,    55,    56,    75,    58,
      55,    56,   163,    70,    71,    72,    73,    76,   192,    75,
     153,    69,   196,    69,    67,   199,   200,   201,   202,    69,
     163,    69,    69,    80,    76,    80,    62,    76,   212,    75,
      75,   192,   216,    88,    76,   196,    59,   221,   199,   200,
     201,   202,    68,    81,    76,    76,    14,    76,    62,   192,
      69,   212,    76,   218,    76,   216,   199,   200,   201,    76,
     221,    82,    -1,   117,   195,   120,   121,    -1,    -1,   212,
      -1,    -1,    -1,   216,    -1,    -1,    -1,    -1,   221,    -1,
      -1,   136,   137,    -1,   139,   140,   141,   142,   143,   144,
     145,    -1,   147,    35,    36,    37,    38,    39,    40,   154,
       3,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      13,    -1,    15,    -1,    17,    -1,    19,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    70,    71,
      72,    73,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,    52,
      53,    54,    55,    56,    -1,    58,    -1,    60,    61,    62,
      -1,    -1,    -1,    -1,    -1,    68,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    78,    -1,    13,    81,    15,
      -1,    17,    -1,    19,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,    -1,    -1,    10,    11,    12,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    49,    50,    51,    52,    53,    54,    55,
      56,    -1,    58,    -1,    60,    61,    62,    -1,    -1,    -1,
      -1,    -1,    68,    -1,    49,    50,    51,    52,    53,    54,
      55,    56,    78,    58,    -1,    -1,    -1,    62,    63,    64,
      65,     3,     4,     5,     6,     7,     8,     9,    -1,    -1,
      75,    13,    -1,    15,    -1,    17,    -1,    19,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
      39,    40,    10,    11,    12,    -1,    35,    36,    37,    38,
      39,    40,    -1,    -1,    -1,    -1,    -1,    49,    50,    51,
      52,    53,    54,    55,    56,    -1,    58,    -1,    60,    61,
      62,    70,    71,    72,    73,    43,    68,    45,    46,    47,
      48,    70,    71,    72,    73,    -1,    35,    76,    37,    38,
      39,    40,    -1,    -1,    62,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    75,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    70,    71,    72,    73
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    21,    84,    85,    86,    62,     0,     3,     4,     5,
       6,     7,     8,     9,    13,    15,    17,    19,    49,    50,
      51,    52,    53,    54,    55,    56,    58,    60,    61,    62,
      68,    78,    87,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   101,   103,   104,   107,   108,   113,   115,
     116,   117,    75,    75,    75,    75,    75,    90,    20,    68,
      68,    68,    68,    75,    75,    81,   105,    81,   109,   113,
     117,    88,    89,    77,    62,    62,    77,    77,    77,    75,
      80,    81,    10,    11,    12,    63,    64,    65,    75,    91,
      98,   100,   101,   102,   113,   114,    62,    62,   100,   106,
     113,   106,    15,    75,    93,    95,    93,    95,    62,    62,
      62,   114,   101,   113,    90,    62,    69,    82,    79,    88,
      80,    80,   114,    43,    45,    46,    47,    48,    99,   100,
     111,   112,    62,   100,    82,    82,    35,    36,    37,    38,
      39,    40,    70,    71,    72,    73,    82,    80,    82,    76,
      18,    18,    76,    76,    75,   114,    69,    69,    69,    69,
      69,    69,    67,    76,    80,   109,    99,    99,    76,    75,
      75,    76,   114,   114,   100,   100,    62,   100,   100,   100,
     100,   100,   100,   100,   114,   114,   101,   101,   113,    88,
      90,   106,    76,    59,    90,    68,    81,   114,   114,    76,
      76,    76,    14,    76,    90,    62,   110,   111,   101,    76,
      76,    90,   105,    90,    90,    88,    76,    69,    82,    90,
      90,   105,   102,    90
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    83,    84,    85,    86,    87,    88,    88,    89,    89,
      89,    89,    89,    89,    89,    90,    90,    91,    91,    91,
      91,    92,    92,    92,    93,    93,    93,    93,    93,    94,
      94,    95,    95,    95,    96,    96,    96,    97,    97,    97,
      97,    98,    99,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,   100,   101,   101,   101,
     102,   102,   102,   102,   102,   102,   103,   103,   104,   104,
     104,   104,   104,   104,   104,   105,   106,   107,   107,   108,
     108,   109,   109,   110,   110,   111,   112,   112,   112,   112,
     112,   113,   113,   114,   114,   114,   114,   114,   114,   114,
     114,   115,   115,   116,   116,   117
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     5,     1,     2,     1,     2,     2,
       2,     1,     2,     1,     1,     3,     1,     2,     4,     2,
       4,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     4,     4,     1,     1,     1,     4,     4,     4,
       4,     3,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     1,     1,     4,     6,     6,
       1,     1,     1,     1,     1,     1,     1,     1,     6,     5,
       6,     7,     8,     7,     7,     2,     1,     5,     7,     3,
       7,     3,     1,     3,     1,     4,     1,     1,     1,     1,
       1,     1,     1,     0,     3,     3,     3,     3,     1,     1,
       1,     8,     9,     5,     3,     3
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
#line 58 "dsl_yacc_modified.y"
                      {printf("program.\n");}
#line 1679 "y.tab.c"
    break;

  case 3:
#line 60 "dsl_yacc_modified.y"
                                            { }
#line 1685 "y.tab.c"
    break;

  case 4:
#line 62 "dsl_yacc_modified.y"
                                          { }
#line 1691 "y.tab.c"
    break;

  case 5:
#line 69 "dsl_yacc_modified.y"
                                {printf("program.\n");}
#line 1697 "y.tab.c"
    break;

  case 6:
#line 72 "dsl_yacc_modified.y"
                                  {printf("Statement\n");}
#line 1703 "y.tab.c"
    break;

  case 7:
#line 73 "dsl_yacc_modified.y"
                    {printf("statement\n");}
#line 1709 "y.tab.c"
    break;

  case 8:
#line 75 "dsl_yacc_modified.y"
                          {printf("declaration\n");}
#line 1715 "y.tab.c"
    break;

  case 9:
#line 76 "dsl_yacc_modified.y"
                       {printf("assignment\n");}
#line 1721 "y.tab.c"
    break;

  case 10:
#line 77 "dsl_yacc_modified.y"
                       {printf("proc call \n");}
#line 1727 "y.tab.c"
    break;

  case 11:
#line 78 "dsl_yacc_modified.y"
                      {printf("control flow\n");}
#line 1733 "y.tab.c"
    break;

  case 12:
#line 79 "dsl_yacc_modified.y"
                      {printf ("Reduction\n");}
#line 1739 "y.tab.c"
    break;

  case 13:
#line 80 "dsl_yacc_modified.y"
                         { }
#line 1745 "y.tab.c"
    break;

  case 14:
#line 81 "dsl_yacc_modified.y"
                              {}
#line 1751 "y.tab.c"
    break;

  case 15:
#line 84 "dsl_yacc_modified.y"
                                    { printf("block of statements\n");}
#line 1757 "y.tab.c"
    break;

  case 16:
#line 85 "dsl_yacc_modified.y"
                            {}
#line 1763 "y.tab.c"
    break;

  case 17:
#line 87 "dsl_yacc_modified.y"
                        {}
#line 1769 "y.tab.c"
    break;

  case 18:
#line 88 "dsl_yacc_modified.y"
                           {}
#line 1775 "y.tab.c"
    break;

  case 19:
#line 89 "dsl_yacc_modified.y"
                    {}
#line 1781 "y.tab.c"
    break;

  case 20:
#line 90 "dsl_yacc_modified.y"
                           {}
#line 1787 "y.tab.c"
    break;

  case 21:
#line 92 "dsl_yacc_modified.y"
                 {}
#line 1793 "y.tab.c"
    break;

  case 22:
#line 93 "dsl_yacc_modified.y"
                {}
#line 1799 "y.tab.c"
    break;

  case 23:
#line 94 "dsl_yacc_modified.y"
                      { }
#line 1805 "y.tab.c"
    break;

  case 24:
#line 96 "dsl_yacc_modified.y"
                 {}
#line 1811 "y.tab.c"
    break;

  case 25:
#line 97 "dsl_yacc_modified.y"
                  {}
#line 1817 "y.tab.c"
    break;

  case 26:
#line 98 "dsl_yacc_modified.y"
                 {}
#line 1823 "y.tab.c"
    break;

  case 27:
#line 99 "dsl_yacc_modified.y"
                   {}
#line 1829 "y.tab.c"
    break;

  case 28:
#line 100 "dsl_yacc_modified.y"
                 {}
#line 1835 "y.tab.c"
    break;

  case 29:
#line 102 "dsl_yacc_modified.y"
                {}
#line 1841 "y.tab.c"
    break;

  case 30:
#line 103 "dsl_yacc_modified.y"
                     {}
#line 1847 "y.tab.c"
    break;

  case 31:
#line 105 "dsl_yacc_modified.y"
                     {}
#line 1853 "y.tab.c"
    break;

  case 32:
#line 106 "dsl_yacc_modified.y"
                                        { }
#line 1859 "y.tab.c"
    break;

  case 33:
#line 107 "dsl_yacc_modified.y"
                                         { }
#line 1865 "y.tab.c"
    break;

  case 34:
#line 109 "dsl_yacc_modified.y"
               { }
#line 1871 "y.tab.c"
    break;

  case 35:
#line 110 "dsl_yacc_modified.y"
                { }
#line 1877 "y.tab.c"
    break;

  case 36:
#line 111 "dsl_yacc_modified.y"
                      { }
#line 1883 "y.tab.c"
    break;

  case 37:
#line 113 "dsl_yacc_modified.y"
                                  { printf("node property\n");}
#line 1889 "y.tab.c"
    break;

  case 38:
#line 114 "dsl_yacc_modified.y"
                                       { }
#line 1895 "y.tab.c"
    break;

  case 39:
#line 115 "dsl_yacc_modified.y"
                                                    { }
#line 1901 "y.tab.c"
    break;

  case 40:
#line 116 "dsl_yacc_modified.y"
                                                     { }
#line 1907 "y.tab.c"
    break;

  case 41:
#line 118 "dsl_yacc_modified.y"
                                {}
#line 1913 "y.tab.c"
    break;

  case 42:
#line 120 "dsl_yacc_modified.y"
                 { }
#line 1919 "y.tab.c"
    break;

  case 43:
#line 122 "dsl_yacc_modified.y"
                        { }
#line 1925 "y.tab.c"
    break;

  case 44:
#line 123 "dsl_yacc_modified.y"
                                         {}
#line 1931 "y.tab.c"
    break;

  case 45:
#line 124 "dsl_yacc_modified.y"
                                             {}
#line 1937 "y.tab.c"
    break;

  case 46:
#line 125 "dsl_yacc_modified.y"
                                             {}
#line 1943 "y.tab.c"
    break;

  case 47:
#line 126 "dsl_yacc_modified.y"
                                           {}
#line 1949 "y.tab.c"
    break;

  case 48:
#line 127 "dsl_yacc_modified.y"
                                                          {}
#line 1955 "y.tab.c"
    break;

  case 49:
#line 128 "dsl_yacc_modified.y"
                                                  {}
#line 1961 "y.tab.c"
    break;

  case 50:
#line 129 "dsl_yacc_modified.y"
                                         {}
#line 1967 "y.tab.c"
    break;

  case 51:
#line 130 "dsl_yacc_modified.y"
                                                {}
#line 1973 "y.tab.c"
    break;

  case 52:
#line 131 "dsl_yacc_modified.y"
                                                        {}
#line 1979 "y.tab.c"
    break;

  case 53:
#line 132 "dsl_yacc_modified.y"
                                            {}
#line 1985 "y.tab.c"
    break;

  case 54:
#line 133 "dsl_yacc_modified.y"
                                     {}
#line 1991 "y.tab.c"
    break;

  case 55:
#line 134 "dsl_yacc_modified.y"
                       {}
#line 1997 "y.tab.c"
    break;

  case 56:
#line 135 "dsl_yacc_modified.y"
                                    { }
#line 2003 "y.tab.c"
    break;

  case 57:
#line 137 "dsl_yacc_modified.y"
                                      { }
#line 2009 "y.tab.c"
    break;

  case 58:
#line 138 "dsl_yacc_modified.y"
                                                    {}
#line 2015 "y.tab.c"
    break;

  case 59:
#line 139 "dsl_yacc_modified.y"
                                              {}
#line 2021 "y.tab.c"
    break;

  case 60:
#line 142 "dsl_yacc_modified.y"
              {}
#line 2027 "y.tab.c"
    break;

  case 61:
#line 143 "dsl_yacc_modified.y"
                    {}
#line 2033 "y.tab.c"
    break;

  case 62:
#line 144 "dsl_yacc_modified.y"
                   {}
#line 2039 "y.tab.c"
    break;

  case 63:
#line 145 "dsl_yacc_modified.y"
                {}
#line 2045 "y.tab.c"
    break;

  case 64:
#line 146 "dsl_yacc_modified.y"
                  {}
#line 2051 "y.tab.c"
    break;

  case 65:
#line 147 "dsl_yacc_modified.y"
                  {}
#line 2057 "y.tab.c"
    break;

  case 66:
#line 149 "dsl_yacc_modified.y"
                            { }
#line 2063 "y.tab.c"
    break;

  case 67:
#line 150 "dsl_yacc_modified.y"
                             { }
#line 2069 "y.tab.c"
    break;

  case 68:
#line 152 "dsl_yacc_modified.y"
                                                                     {}
#line 2075 "y.tab.c"
    break;

  case 69:
#line 153 "dsl_yacc_modified.y"
                                                                 { }
#line 2081 "y.tab.c"
    break;

  case 70:
#line 154 "dsl_yacc_modified.y"
                                                                       { }
#line 2087 "y.tab.c"
    break;

  case 71:
#line 155 "dsl_yacc_modified.y"
                                                                        {}
#line 2093 "y.tab.c"
    break;

  case 72:
#line 156 "dsl_yacc_modified.y"
                                                                                {}
#line 2099 "y.tab.c"
    break;

  case 73:
#line 157 "dsl_yacc_modified.y"
                                                                 {}
#line 2105 "y.tab.c"
    break;

  case 74:
#line 158 "dsl_yacc_modified.y"
                                                                  {}
#line 2111 "y.tab.c"
    break;

  case 75:
#line 160 "dsl_yacc_modified.y"
                            { }
#line 2117 "y.tab.c"
    break;

  case 76:
#line 162 "dsl_yacc_modified.y"
                          { }
#line 2123 "y.tab.c"
    break;

  case 77:
#line 164 "dsl_yacc_modified.y"
                                                    { }
#line 2129 "y.tab.c"
    break;

  case 78:
#line 165 "dsl_yacc_modified.y"
                                                                        { }
#line 2135 "y.tab.c"
    break;

  case 79:
#line 168 "dsl_yacc_modified.y"
                                       { }
#line 2141 "y.tab.c"
    break;

  case 80:
#line 169 "dsl_yacc_modified.y"
                                                            {}
#line 2147 "y.tab.c"
    break;

  case 81:
#line 171 "dsl_yacc_modified.y"
                                 { }
#line 2153 "y.tab.c"
    break;

  case 82:
#line 172 "dsl_yacc_modified.y"
                            { }
#line 2159 "y.tab.c"
    break;

  case 83:
#line 174 "dsl_yacc_modified.y"
                                  { }
#line 2165 "y.tab.c"
    break;

  case 85:
#line 177 "dsl_yacc_modified.y"
                                              {}
#line 2171 "y.tab.c"
    break;

  case 86:
#line 179 "dsl_yacc_modified.y"
                     {}
#line 2177 "y.tab.c"
    break;

  case 87:
#line 180 "dsl_yacc_modified.y"
                           {}
#line 2183 "y.tab.c"
    break;

  case 88:
#line 181 "dsl_yacc_modified.y"
                             {}
#line 2189 "y.tab.c"
    break;

  case 89:
#line 182 "dsl_yacc_modified.y"
                         {}
#line 2195 "y.tab.c"
    break;

  case 90:
#line 183 "dsl_yacc_modified.y"
                         {}
#line 2201 "y.tab.c"
    break;

  case 91:
#line 185 "dsl_yacc_modified.y"
              { }
#line 2207 "y.tab.c"
    break;

  case 92:
#line 186 "dsl_yacc_modified.y"
               { }
#line 2213 "y.tab.c"
    break;

  case 94:
#line 189 "dsl_yacc_modified.y"
                                   {}
#line 2219 "y.tab.c"
    break;

  case 95:
#line 191 "dsl_yacc_modified.y"
                                     {}
#line 2225 "y.tab.c"
    break;

  case 96:
#line 192 "dsl_yacc_modified.y"
                                         {}
#line 2231 "y.tab.c"
    break;

  case 97:
#line 193 "dsl_yacc_modified.y"
                                           {}
#line 2237 "y.tab.c"
    break;

  case 98:
#line 194 "dsl_yacc_modified.y"
                             {}
#line 2243 "y.tab.c"
    break;

  case 99:
#line 195 "dsl_yacc_modified.y"
                            {}
#line 2249 "y.tab.c"
    break;

  case 100:
#line 196 "dsl_yacc_modified.y"
                            {}
#line 2255 "y.tab.c"
    break;

  case 101:
#line 199 "dsl_yacc_modified.y"
                                                                  {}
#line 2261 "y.tab.c"
    break;

  case 102:
#line 200 "dsl_yacc_modified.y"
                                                                             { }
#line 2267 "y.tab.c"
    break;

  case 103:
#line 204 "dsl_yacc_modified.y"
                                                                 {}
#line 2273 "y.tab.c"
    break;

  case 104:
#line 205 "dsl_yacc_modified.y"
                                                           {}
#line 2279 "y.tab.c"
    break;

  case 105:
#line 207 "dsl_yacc_modified.y"
                {printf("oid\n");}
#line 2285 "y.tab.c"
    break;


#line 2289 "y.tab.c"

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
#line 209 "dsl_yacc_modified.y"



void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}

