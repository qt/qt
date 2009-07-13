
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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         cssyyparse
#define yylex           cssyylex
#define yyerror         cssyyerror
#define yylval          cssyylval
#define yychar          cssyychar
#define yydebug         cssyydebug
#define yynerrs         cssyynerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "../css/CSSGrammar.y"


/*
 *  Copyright (C) 2002-2003 Lars Knoll (knoll@kde.org)
 *  Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 *  Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "config.h"

#include "CSSMediaRule.h"
#include "CSSParser.h"
#include "CSSPropertyNames.h"
#include "CSSRuleList.h"
#include "CSSSelector.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

// FIXME: Replace with %parse-param { CSSParser* parser } once we can depend on bison 2.x
#define YYPARSE_PARAM parser
#define YYLEX_PARAM parser



/* Line 189 of yacc.c  */
#line 136 "WebCore/tmp/../generated/CSSGrammar.tab.c"

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
     TOKEN_EOF = 0,
     LOWEST_PREC = 258,
     UNIMPORTANT_TOK = 259,
     WHITESPACE = 260,
     SGML_CD = 261,
     INCLUDES = 262,
     DASHMATCH = 263,
     BEGINSWITH = 264,
     ENDSWITH = 265,
     CONTAINS = 266,
     STRING = 267,
     IDENT = 268,
     NTH = 269,
     HEX = 270,
     IDSEL = 271,
     IMPORT_SYM = 272,
     PAGE_SYM = 273,
     MEDIA_SYM = 274,
     FONT_FACE_SYM = 275,
     CHARSET_SYM = 276,
     NAMESPACE_SYM = 277,
     WEBKIT_RULE_SYM = 278,
     WEBKIT_DECLS_SYM = 279,
     WEBKIT_KEYFRAME_RULE_SYM = 280,
     WEBKIT_KEYFRAMES_SYM = 281,
     WEBKIT_VALUE_SYM = 282,
     WEBKIT_MEDIAQUERY_SYM = 283,
     WEBKIT_SELECTOR_SYM = 284,
     WEBKIT_VARIABLES_SYM = 285,
     WEBKIT_DEFINE_SYM = 286,
     VARIABLES_FOR = 287,
     WEBKIT_VARIABLES_DECLS_SYM = 288,
     ATKEYWORD = 289,
     IMPORTANT_SYM = 290,
     MEDIA_ONLY = 291,
     MEDIA_NOT = 292,
     MEDIA_AND = 293,
     QEMS = 294,
     EMS = 295,
     EXS = 296,
     PXS = 297,
     CMS = 298,
     MMS = 299,
     INS = 300,
     PTS = 301,
     PCS = 302,
     DEGS = 303,
     RADS = 304,
     GRADS = 305,
     TURNS = 306,
     MSECS = 307,
     SECS = 308,
     HERZ = 309,
     KHERZ = 310,
     DIMEN = 311,
     PERCENTAGE = 312,
     FLOATTOKEN = 313,
     INTEGER = 314,
     URI = 315,
     FUNCTION = 316,
     NOTFUNCTION = 317,
     UNICODERANGE = 318,
     VARCALL = 319
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 57 "../css/CSSGrammar.y"

    bool boolean;
    char character;
    int integer;
    double number;
    CSSParserString string;

    CSSRule* rule;
    CSSRuleList* ruleList;
    CSSSelector* selector;
    Vector<CSSSelector*>* selectorList;
    CSSSelector::Relation relation;
    MediaList* mediaList;
    MediaQuery* mediaQuery;
    MediaQuery::Restrictor mediaQueryRestrictor;
    MediaQueryExp* mediaQueryExp;
    CSSParserValue value;
    CSSParserValueList* valueList;
    Vector<MediaQueryExp*>* mediaQueryExpList;
    WebKitCSSKeyframeRule* keyframeRule;
    WebKitCSSKeyframesRule* keyframesRule;
    float val;



/* Line 214 of yacc.c  */
#line 263 "WebCore/tmp/../generated/CSSGrammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 81 "../css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 264 of yacc.c  */
#line 290 "WebCore/tmp/../generated/CSSGrammar.tab.c"

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
#define YYFINAL  28
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1315

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  85
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  90
/* YYNRULES -- Number of rules.  */
#define YYNRULES  267
/* YYNRULES -- Number of states.  */
#define YYNSTATES  513

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   319

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    83,     2,    84,     2,     2,
      73,    74,    20,    76,    75,    79,    18,    82,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,    72,
       2,    81,    78,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,    80,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    70,    21,    71,    77,     2,     2,     2,
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
      15,    16,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,    10,    13,    16,    19,    22,    25,    28,
      31,    33,    35,    42,    49,    55,    61,    67,    73,    79,
      80,    83,    84,    87,    90,    91,    93,    95,    97,   103,
     107,   111,   112,   116,   118,   119,   123,   124,   128,   129,
     133,   135,   137,   139,   141,   143,   145,   147,   149,   151,
     152,   156,   158,   160,   162,   164,   166,   168,   170,   172,
     174,   181,   188,   192,   196,   204,   212,   213,   217,   219,
     222,   224,   228,   230,   233,   237,   242,   246,   252,   257,
     262,   269,   274,   282,   285,   291,   295,   300,   303,   310,
     314,   318,   319,   322,   324,   326,   329,   330,   335,   343,
     345,   351,   352,   356,   357,   359,   361,   363,   368,   369,
     371,   373,   378,   381,   389,   396,   399,   408,   410,   412,
     413,   417,   424,   426,   432,   434,   436,   440,   444,   452,
     456,   460,   463,   466,   469,   471,   473,   479,   481,   486,
     489,   492,   494,   496,   499,   503,   506,   508,   511,   514,
     516,   519,   521,   524,   528,   531,   533,   535,   537,   540,
     543,   545,   547,   549,   551,   553,   556,   559,   564,   573,
     579,   589,   591,   593,   595,   597,   599,   601,   603,   605,
     608,   612,   617,   622,   627,   634,   636,   639,   641,   645,
     647,   650,   653,   657,   662,   666,   672,   677,   682,   689,
     695,   698,   701,   708,   715,   718,   722,   727,   730,   733,
     736,   737,   739,   743,   746,   749,   752,   753,   755,   758,
     761,   764,   767,   771,   774,   777,   779,   782,   784,   787,
     790,   793,   796,   799,   802,   805,   808,   811,   814,   817,
     820,   823,   826,   829,   832,   835,   838,   841,   844,   847,
     850,   852,   858,   862,   865,   868,   870,   873,   877,   881,
     884,   888,   890,   892,   895,   901,   905,   907
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      86,     0,    -1,    97,    96,   100,   101,   102,   103,    -1,
      88,    95,    -1,    90,    95,    -1,    92,    95,    -1,    93,
      95,    -1,    94,    95,    -1,    91,    95,    -1,    89,    95,
      -1,   104,    -1,   109,    -1,    28,    70,    95,    87,    95,
      71,    -1,    30,    70,    95,   133,    95,    71,    -1,    29,
      70,    95,   155,    71,    -1,    38,    70,    95,   112,    71,
      -1,    32,    70,    95,   160,    71,    -1,    33,     5,    95,
     125,    71,    -1,    34,    70,    95,   141,    71,    -1,    -1,
      95,     5,    -1,    -1,    96,     6,    -1,    96,     5,    -1,
      -1,    99,    -1,    71,    -1,     0,    -1,    26,    95,    12,
      95,    72,    -1,    26,     1,   173,    -1,    26,     1,    72,
      -1,    -1,   100,   109,    96,    -1,   169,    -1,    -1,   101,
     110,    96,    -1,    -1,   102,   116,    96,    -1,    -1,   103,
     105,    96,    -1,   140,    -1,   128,    -1,   136,    -1,   137,
      -1,   130,    -1,   104,    -1,   172,    -1,   168,    -1,   170,
      -1,    -1,   106,   108,    96,    -1,   140,    -1,   136,    -1,
     137,    -1,   130,    -1,   107,    -1,   172,    -1,   168,    -1,
     170,    -1,   171,    -1,    22,    95,   118,    95,   126,    72,
      -1,    22,    95,   118,    95,   126,   173,    -1,    22,     1,
      72,    -1,    22,     1,   173,    -1,    35,    95,   126,    70,
      95,   112,    71,    -1,    36,    95,   111,    70,    95,   112,
      71,    -1,    -1,    37,     5,   127,    -1,   114,    -1,   113,
     114,    -1,   113,    -1,     1,   174,     1,    -1,     1,    -1,
     113,     1,    -1,   114,    72,    95,    -1,   114,   174,    72,
      95,    -1,     1,    72,    95,    -1,     1,   174,     1,    72,
      95,    -1,   113,   114,    72,    95,    -1,   113,     1,    72,
      95,    -1,   113,     1,   174,     1,    72,    95,    -1,   115,
      17,    95,   160,    -1,   115,    95,    70,    95,   155,    71,
      95,    -1,   115,     1,    -1,   115,    17,    95,     1,   160,
      -1,   115,    17,    95,    -1,   115,    17,    95,     1,    -1,
      13,    95,    -1,    27,    95,   117,   118,    95,    72,    -1,
      27,     1,   173,    -1,    27,     1,    72,    -1,    -1,    13,
       5,    -1,    12,    -1,    65,    -1,    13,    95,    -1,    -1,
      17,    95,   160,    95,    -1,    73,    95,   119,    95,   120,
      74,    95,    -1,   121,    -1,   122,    95,    43,    95,   121,
      -1,    -1,    43,    95,   122,    -1,    -1,    41,    -1,    42,
      -1,   122,    -1,   124,    95,   129,   123,    -1,    -1,   127,
      -1,   125,    -1,   127,    75,    95,   125,    -1,   127,     1,
      -1,    24,    95,   127,    70,    95,   106,   167,    -1,    24,
      95,    70,    95,   106,   167,    -1,    13,    95,    -1,    31,
      95,   131,    95,    70,    95,   132,    71,    -1,    13,    -1,
      12,    -1,    -1,   132,   133,    95,    -1,   134,    95,    70,
      95,   155,    71,    -1,   135,    -1,   134,    95,    75,    95,
     135,    -1,    62,    -1,    13,    -1,    23,     1,   173,    -1,
      23,     1,    72,    -1,    25,    95,    70,    95,   155,    71,
      95,    -1,    25,     1,   173,    -1,    25,     1,    72,    -1,
      76,    95,    -1,    77,    95,    -1,    78,    95,    -1,    79,
      -1,    76,    -1,   141,    70,    95,   155,    98,    -1,   143,
      -1,   141,    75,    95,   143,    -1,   141,     1,    -1,   143,
       5,    -1,   145,    -1,   142,    -1,   142,   145,    -1,   143,
     138,   145,    -1,   143,     1,    -1,    21,    -1,    20,    21,
      -1,    13,    21,    -1,   146,    -1,   146,   147,    -1,   147,
      -1,   144,   146,    -1,   144,   146,   147,    -1,   144,   147,
      -1,    13,    -1,    20,    -1,   148,    -1,   147,   148,    -1,
     147,     1,    -1,    16,    -1,    15,    -1,   149,    -1,   151,
      -1,   154,    -1,    18,    13,    -1,    13,    95,    -1,    19,
      95,   150,    80,    -1,    19,    95,   150,   152,    95,   153,
      95,    80,    -1,    19,    95,   144,   150,    80,    -1,    19,
      95,   144,   150,   152,    95,   153,    95,    80,    -1,    81,
      -1,     7,    -1,     8,    -1,     9,    -1,    10,    -1,    11,
      -1,    13,    -1,    12,    -1,    17,    13,    -1,    17,    17,
      13,    -1,    17,    66,    14,    74,    -1,    17,    66,    64,
      74,    -1,    17,    66,    13,    74,    -1,    17,    67,    95,
     145,    95,    74,    -1,   157,    -1,   156,   157,    -1,   156,
      -1,     1,   174,     1,    -1,     1,    -1,   156,     1,    -1,
     156,   174,    -1,   157,    72,    95,    -1,   157,   174,    72,
      95,    -1,     1,    72,    95,    -1,     1,   174,     1,    72,
      95,    -1,   156,   157,    72,    95,    -1,   156,     1,    72,
      95,    -1,   156,     1,   174,     1,    72,    95,    -1,   158,
      17,    95,   160,   159,    -1,   164,    95,    -1,   158,     1,
      -1,   158,    17,    95,     1,   160,   159,    -1,   158,    17,
      95,   160,   159,     1,    -1,    40,    95,    -1,   158,    17,
      95,    -1,   158,    17,    95,     1,    -1,   158,   173,    -1,
      13,    95,    -1,    40,    95,    -1,    -1,   162,    -1,   160,
     161,   162,    -1,   160,     1,    -1,    82,    95,    -1,    75,
      95,    -1,    -1,   163,    -1,   139,   163,    -1,    12,    95,
      -1,    13,    95,    -1,    61,    95,    -1,   139,    61,    95,
      -1,    65,    95,    -1,    68,    95,    -1,   166,    -1,    83,
      95,    -1,   165,    -1,   164,    95,    -1,    84,    95,    -1,
      64,    95,    -1,    63,    95,    -1,    62,    95,    -1,    47,
      95,    -1,    48,    95,    -1,    49,    95,    -1,    50,    95,
      -1,    51,    95,    -1,    52,    95,    -1,    53,    95,    -1,
      54,    95,    -1,    55,    95,    -1,    56,    95,    -1,    57,
      95,    -1,    58,    95,    -1,    59,    95,    -1,    60,    95,
      -1,    45,    95,    -1,    44,    95,    -1,    46,    95,    -1,
      69,    -1,    66,    95,   160,    74,    95,    -1,    66,    95,
       1,    -1,    15,    95,    -1,    16,    95,    -1,    98,    -1,
       1,    98,    -1,    39,     1,   173,    -1,    39,     1,    72,
      -1,   168,    96,    -1,   169,   168,    96,    -1,   109,    -1,
     128,    -1,     1,   173,    -1,    70,     1,   174,     1,    98,
      -1,    70,     1,    98,    -1,   173,    -1,   174,     1,   173,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   265,   265,   266,   267,   268,   269,   270,   271,   272,
     276,   277,   281,   287,   293,   299,   305,   319,   326,   336,
     337,   340,   342,   343,   346,   348,   353,   354,   358,   364,
     366,   370,   372,   377,   381,   383,   390,   392,   395,   397,
     405,   406,   407,   408,   409,   413,   414,   415,   416,   420,
     421,   432,   433,   434,   435,   439,   440,   441,   442,   443,
     448,   451,   454,   457,   463,   467,   473,   477,   483,   486,
     491,   494,   497,   500,   506,   509,   512,   515,   518,   523,
     526,   532,   536,   540,   544,   548,   553,   560,   566,   571,
     572,   576,   577,   581,   582,   586,   592,   595,   601,   608,
     613,   620,   623,   629,   632,   635,   641,   646,   654,   657,
     661,   666,   671,   677,   680,   686,   692,   699,   700,   704,
     705,   713,   719,   724,   733,   734,   758,   761,   767,   771,
     774,   780,   781,   782,   786,   787,   791,   797,   806,   814,
     820,   826,   829,   833,   849,   869,   875,   876,   877,   881,
     886,   893,   899,   909,   921,   934,   942,   950,   953,   966,
     972,   980,   992,   993,   994,   998,  1009,  1020,  1025,  1031,
    1039,  1051,  1054,  1057,  1060,  1063,  1066,  1072,  1073,  1077,
    1102,  1117,  1135,  1153,  1172,  1187,  1190,  1195,  1198,  1201,
    1204,  1207,  1213,  1216,  1219,  1222,  1225,  1230,  1233,  1239,
    1253,  1265,  1269,  1276,  1281,  1286,  1291,  1296,  1303,  1309,
    1310,  1314,  1319,  1333,  1339,  1342,  1345,  1351,  1352,  1353,
    1354,  1360,  1361,  1362,  1363,  1364,  1365,  1367,  1370,  1373,
    1377,  1378,  1379,  1380,  1381,  1382,  1383,  1384,  1385,  1386,
    1387,  1388,  1389,  1390,  1391,  1392,  1393,  1394,  1395,  1396,
    1400,  1408,  1417,  1433,  1434,  1441,  1444,  1450,  1453,  1459,
    1460,  1464,  1470,  1476,  1494,  1495,  1499,  1500
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "TOKEN_EOF", "error", "$undefined", "LOWEST_PREC", "UNIMPORTANT_TOK",
  "WHITESPACE", "SGML_CD", "INCLUDES", "DASHMATCH", "BEGINSWITH",
  "ENDSWITH", "CONTAINS", "STRING", "IDENT", "NTH", "HEX", "IDSEL", "':'",
  "'.'", "'['", "'*'", "'|'", "IMPORT_SYM", "PAGE_SYM", "MEDIA_SYM",
  "FONT_FACE_SYM", "CHARSET_SYM", "NAMESPACE_SYM", "WEBKIT_RULE_SYM",
  "WEBKIT_DECLS_SYM", "WEBKIT_KEYFRAME_RULE_SYM", "WEBKIT_KEYFRAMES_SYM",
  "WEBKIT_VALUE_SYM", "WEBKIT_MEDIAQUERY_SYM", "WEBKIT_SELECTOR_SYM",
  "WEBKIT_VARIABLES_SYM", "WEBKIT_DEFINE_SYM", "VARIABLES_FOR",
  "WEBKIT_VARIABLES_DECLS_SYM", "ATKEYWORD", "IMPORTANT_SYM", "MEDIA_ONLY",
  "MEDIA_NOT", "MEDIA_AND", "QEMS", "EMS", "EXS", "PXS", "CMS", "MMS",
  "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS", "MSECS", "SECS",
  "HERZ", "KHERZ", "DIMEN", "PERCENTAGE", "FLOATTOKEN", "INTEGER", "URI",
  "FUNCTION", "NOTFUNCTION", "UNICODERANGE", "VARCALL", "'{'", "'}'",
  "';'", "'('", "')'", "','", "'+'", "'~'", "'>'", "'-'", "']'", "'='",
  "'/'", "'#'", "'%'", "$accept", "stylesheet", "valid_rule_or_import",
  "webkit_rule", "webkit_keyframe_rule", "webkit_decls",
  "webkit_variables_decls", "webkit_value", "webkit_mediaquery",
  "webkit_selector", "maybe_space", "maybe_sgml", "maybe_charset",
  "closing_brace", "charset", "import_list", "variables_list",
  "namespace_list", "rule_list", "valid_rule", "rule", "block_rule_list",
  "block_valid_rule", "block_rule", "import", "variables_rule",
  "variables_media_list", "variables_declaration_list",
  "variables_decl_list", "variables_declaration", "variable_name",
  "namespace", "maybe_ns_prefix", "string_or_uri", "media_feature",
  "maybe_media_value", "media_query_exp", "media_query_exp_list",
  "maybe_and_media_query_exp_list", "maybe_media_restrictor",
  "media_query", "maybe_media_list", "media_list", "media", "medium",
  "keyframes", "keyframe_name", "keyframes_rule", "keyframe_rule",
  "key_list", "key", "page", "font_face", "combinator", "unary_operator",
  "ruleset", "selector_list", "selector_with_trailing_whitespace",
  "selector", "namespace_selector", "simple_selector", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo", "declaration_list", "decl_list",
  "declaration", "property", "prio", "expr", "operator", "term",
  "unary_term", "variable_reference", "function", "hexcolor", "save_block",
  "invalid_at", "invalid_at_list", "invalid_import", "invalid_media",
  "invalid_rule", "invalid_block", "invalid_block_list", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,    58,    46,    91,
      42,   124,   272,   273,   274,   275,   276,   277,   278,   279,
     280,   281,   282,   283,   284,   285,   286,   287,   288,   289,
     290,   291,   292,   293,   294,   295,   296,   297,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   309,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     123,   125,    59,    40,    41,    44,    43,   126,    62,    45,
      93,    61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    85,    86,    86,    86,    86,    86,    86,    86,    86,
      87,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      95,    96,    96,    96,    97,    97,    98,    98,    99,    99,
      99,   100,   100,   100,   101,   101,   102,   102,   103,   103,
     104,   104,   104,   104,   104,   105,   105,   105,   105,   106,
     106,   107,   107,   107,   107,   108,   108,   108,   108,   108,
     109,   109,   109,   109,   110,   110,   111,   111,   112,   112,
     112,   112,   112,   112,   113,   113,   113,   113,   113,   113,
     113,   114,   114,   114,   114,   114,   114,   115,   116,   116,
     116,   117,   117,   118,   118,   119,   120,   120,   121,   122,
     122,   123,   123,   124,   124,   124,   125,   125,   126,   126,
     127,   127,   127,   128,   128,   129,   130,   131,   131,   132,
     132,   133,   134,   134,   135,   135,   136,   136,   137,   137,
     137,   138,   138,   138,   139,   139,   140,   141,   141,   141,
     142,   143,   143,   143,   143,   143,   144,   144,   144,   145,
     145,   145,   145,   145,   145,   146,   146,   147,   147,   147,
     148,   148,   148,   148,   148,   149,   150,   151,   151,   151,
     151,   152,   152,   152,   152,   152,   152,   153,   153,   154,
     154,   154,   154,   154,   154,   155,   155,   155,   155,   155,
     155,   155,   156,   156,   156,   156,   156,   156,   156,   157,
     157,   157,   157,   157,   157,   157,   157,   157,   158,   159,
     159,   160,   160,   160,   161,   161,   161,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     163,   163,   163,   163,   163,   163,   163,   163,   163,   163,
     164,   165,   165,   166,   166,   167,   167,   168,   168,   169,
     169,   170,   171,   172,   173,   173,   174,   174
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     6,     2,     2,     2,     2,     2,     2,     2,
       1,     1,     6,     6,     5,     5,     5,     5,     5,     0,
       2,     0,     2,     2,     0,     1,     1,     1,     5,     3,
       3,     0,     3,     1,     0,     3,     0,     3,     0,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       6,     6,     3,     3,     7,     7,     0,     3,     1,     2,
       1,     3,     1,     2,     3,     4,     3,     5,     4,     4,
       6,     4,     7,     2,     5,     3,     4,     2,     6,     3,
       3,     0,     2,     1,     1,     2,     0,     4,     7,     1,
       5,     0,     3,     0,     1,     1,     1,     4,     0,     1,
       1,     4,     2,     7,     6,     2,     8,     1,     1,     0,
       3,     6,     1,     5,     1,     1,     3,     3,     7,     3,
       3,     2,     2,     2,     1,     1,     5,     1,     4,     2,
       2,     1,     1,     2,     3,     2,     1,     2,     2,     1,
       2,     1,     2,     3,     2,     1,     1,     1,     2,     2,
       1,     1,     1,     1,     1,     2,     2,     4,     8,     5,
       9,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       3,     4,     4,     4,     6,     1,     2,     1,     3,     1,
       2,     2,     3,     4,     3,     5,     4,     4,     6,     5,
       2,     2,     6,     6,     2,     3,     4,     2,     2,     2,
       0,     1,     3,     2,     2,     2,     0,     1,     2,     2,
       2,     2,     3,     2,     2,     1,     2,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     5,     3,     2,     2,     1,     2,     3,     3,     2,
       3,     1,     1,     2,     5,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      19,    19,    19,    19,    19,    19,    19,    21,    25,     0,
       0,    19,    19,    19,    19,    19,    19,    19,     1,     3,
       9,     4,     8,     5,     6,     7,    31,     0,    30,    29,
      20,    19,     0,     0,     0,     0,   103,     0,     0,    23,
      22,     0,    34,    21,    33,     0,     0,   155,   161,   160,
       0,     0,    19,   156,   146,     0,     0,    19,     0,    19,
      19,    10,    11,    41,    44,    42,    43,    40,     0,   142,
       0,     0,   141,   149,     0,   157,   162,   163,   164,   189,
      19,    19,   250,     0,     0,   185,     0,    19,   125,   124,
      19,    19,   122,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,    19,    19,    19,    19,    19,    19,    19,    19,    19,
      19,   135,   134,    19,    19,     0,     0,   211,   217,    19,
     227,   225,   104,   105,    19,    99,   106,    19,     0,     0,
      72,    19,     0,     0,    68,     0,     0,    36,    21,   259,
      21,    27,    26,   265,   266,     0,    28,   148,   179,     0,
       0,    19,   165,     0,   147,     0,     0,     0,   103,     0,
       0,     0,     0,   139,    19,    19,   143,   145,   140,    19,
      19,    19,     0,   155,   156,   152,     0,     0,   159,   158,
      19,     0,   208,   204,    14,   190,   186,     0,    19,     0,
     201,    19,   207,   200,     0,     0,   219,   220,   253,   254,
     248,   247,   249,   233,   234,   235,   236,   237,   238,   239,
     240,   241,   242,   243,   244,   245,   246,   221,   232,   231,
     230,   223,     0,   224,   226,   229,    19,   218,   213,    16,
      19,    19,     0,   228,     0,     0,     0,    17,    18,    19,
       0,    87,    15,    73,    69,    19,     0,    83,    19,     0,
     258,   257,    19,    19,    38,    21,    32,   260,     0,   180,
       0,     0,     0,     0,    19,     0,     0,     0,    62,    63,
      93,    94,    19,   127,   126,    19,   110,     0,   130,   129,
      19,   118,   117,    19,    12,     0,     0,   131,   132,   133,
     144,     0,   194,   188,    19,     0,    19,     0,   192,    19,
       0,    13,    19,    19,   252,     0,   222,   215,   214,   212,
      19,    19,    19,    19,   101,    76,    71,    19,     0,    19,
      74,    19,     0,    19,   103,    66,     0,     0,    21,    35,
     264,   267,   183,   181,   182,    19,   166,    19,     0,   172,
     173,   174,   175,   176,   167,   171,    19,   108,    49,   112,
      19,    19,     0,     0,     0,     0,    19,   197,     0,   196,
     193,   206,     0,     0,     0,    19,    95,    96,     0,   115,
      19,   107,    19,    79,     0,    78,    75,    86,     0,     0,
       0,     0,     0,     0,     0,    91,     0,    45,    21,   261,
      47,    48,    46,    37,     0,   169,    19,     0,     0,     0,
      49,   103,     0,    19,   136,   195,    19,     0,    19,     0,
       0,   123,   251,    19,     0,   100,     0,    77,    19,     0,
       0,    19,   103,    19,    90,    89,     0,     0,   263,    39,
     184,     0,   178,   177,    19,    60,    61,     0,   255,    55,
      21,   262,    54,    52,    53,    51,   114,    57,    58,    59,
      56,     0,   111,    19,   119,   198,   202,   209,   203,   121,
       0,    19,   102,    80,    19,     0,     0,     0,    92,    19,
      19,     0,   256,    50,   113,   128,     0,     0,    98,    82,
       0,     0,     0,     0,   168,   116,    19,    97,    64,    65,
      88,   170,   120
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     9,    70,    10,    11,    12,    13,    14,    15,    16,
     255,    36,    17,   458,    18,    52,   157,   274,   347,    71,
     408,   419,   459,   460,   409,   275,   403,   152,   153,   154,
     155,   348,   447,   292,   331,   434,   145,   146,   391,   147,
     296,   400,   401,    73,   334,    74,   303,   496,   100,   101,
     102,    75,    76,   192,   135,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,   287,    87,   366,   454,    88,
      93,    94,    95,    96,   429,   136,   252,   137,   138,   139,
     140,   141,   466,   467,    54,   468,   469,   470,   164,   165
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -435
static const yytype_int16 yypact[] =
{
     818,    44,   -36,   -18,   112,   127,    66,   141,   162,   243,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,   239,
      43,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,   250,
     250,   250,   250,   250,   250,   250,    37,   304,  -435,  -435,
    -435,  -435,   763,   354,    31,  1114,   144,   622,    49,  -435,
    -435,   346,   344,  -435,   332,    27,    23,   358,  -435,  -435,
     401,   370,  -435,   371,  -435,   381,   406,  -435,   193,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,   171,   702,
     143,   631,  -435,   756,   159,  -435,  -435,  -435,  -435,   240,
    -435,  -435,  -435,   329,   303,   254,   199,  -435,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,   949,   903,  -435,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,    34,  -435,   342,     4,
     274,  -435,   353,    59,   291,   223,   331,   395,  -435,   438,
    -435,  -435,  -435,  -435,  -435,   437,  -435,  -435,  -435,   448,
      24,  -435,  -435,   415,  -435,   349,   295,   377,   375,   399,
     198,   421,   190,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
    -435,  -435,   702,  -435,  -435,   756,   334,   380,  -435,  -435,
    -435,   463,   250,   250,  -435,   409,   398,   180,  -435,    15,
    -435,  -435,  -435,   250,   221,   182,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,  1052,   250,   250,   250,  -435,  -435,  -435,  -435,
    -435,  -435,  1172,   250,   188,   166,   301,  -435,  -435,  -435,
     472,   250,  -435,   412,   404,  -435,    62,  -435,  -435,   220,
    -435,  -435,  -435,  -435,   458,  -435,   438,   438,    27,  -435,
     413,   417,   430,   622,   358,   371,   473,   158,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,   172,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,   354,   622,   250,   250,   250,
    -435,   555,   250,   420,  -435,   502,  -435,   459,   250,  -435,
     535,  -435,  -435,  -435,  -435,   976,   250,   250,   250,  -435,
    -435,  -435,  -435,  -435,   496,   250,   423,  -435,   541,  -435,
     250,  -435,   754,  -435,   424,    36,   552,   685,  -435,   438,
    -435,  -435,  -435,  -435,  -435,  -435,   250,  -435,   277,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,   856,   250,  -435,
    -435,  -435,   354,   226,    65,   203,  -435,   250,   428,   250,
     250,  1172,   462,   354,    31,  -435,   250,    53,   186,   250,
    -435,  -435,  -435,   250,   429,   250,   250,  1172,   608,   354,
     479,    83,   538,   485,   482,   320,   459,  -435,  -435,  -435,
    -435,  -435,  -435,   438,    78,  -435,  -435,   447,   489,  1244,
     250,   144,   487,  -435,  -435,   250,  -435,   462,  -435,   205,
     491,  -435,   250,  -435,   492,  -435,   186,   250,  -435,   681,
     497,  -435,     5,  -435,  -435,  -435,   558,   150,  -435,   438,
    -435,   447,  -435,  -435,  -435,  -435,  -435,    27,  -435,  -435,
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
    -435,  1244,  -435,  -435,   250,   250,  -435,   250,  -435,  -435,
    1114,  -435,    34,   250,  -435,    49,   178,    49,  -435,  -435,
    -435,     1,  -435,   438,  -435,   250,   306,   827,   250,   250,
     498,   504,   151,    14,  -435,  -435,  -435,   250,  -435,  -435,
    -435,  -435,   250
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,  -435,
      -1,   -21,  -435,   -51,  -435,  -435,  -435,  -435,  -435,   229,
    -435,   147,  -435,  -435,   256,  -435,  -435,  -434,  -435,   425,
    -435,  -435,  -435,   130,  -435,  -435,   214,   174,  -435,  -435,
     -45,   241,  -176,  -389,  -435,  -227,  -435,  -435,   116,  -435,
     231,  -154,  -137,  -435,  -435,  -130,   566,  -435,   310,   449,
     -61,   547,   -50,   -55,  -435,   348,  -435,   278,   194,  -435,
    -298,  -435,   581,  -435,   261,  -185,  -435,   443,   546,   -35,
    -435,  -435,   218,   -19,  -435,   352,  -435,   364,   -16,    -3
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -217
static const yytype_int16 yytable[] =
{
      20,   148,   297,    39,   163,   183,    40,   374,    97,    29,
      30,    31,    32,    33,    34,    35,   317,    53,   186,    40,
      42,    43,    44,    45,    46,    47,    48,   161,    40,   199,
     461,   196,   159,   197,    21,   160,    40,   280,   281,   -19,
      56,    40,    49,    50,    98,    19,   142,   143,    40,   -19,
     150,   500,    22,   501,    40,    41,   -19,   325,    40,    97,
     263,   173,   151,   317,   176,   161,   178,   180,   181,   182,
     433,    25,   151,   402,   422,   258,    51,   -19,   144,   185,
     212,   504,   461,    40,   369,   430,   201,   319,   282,   202,
     203,   207,   209,    99,   511,   166,   213,    37,   162,   214,
     215,   440,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,   238,   239,   240,   241,   242,   243,
     -70,   310,   244,   245,   341,   382,   162,   276,   253,   277,
     271,   199,   199,   254,   187,   311,   256,   260,   188,    40,
     261,   266,   450,  -109,   269,  -109,    40,   398,   371,   289,
     198,   294,   290,   299,  -151,   359,   360,   361,   362,   363,
     283,    40,   183,   369,    58,    59,    60,    61,    62,   369,
    -191,   317,    23,   305,   306,   142,   143,    40,   307,   308,
     309,    40,   462,    40,   179,    40,   427,    24,   -19,   312,
     210,   330,   315,    40,   187,  -199,   478,   318,   188,   332,
     320,    26,   439,  -137,  -137,   291,   211,   144,  -137,   189,
     190,   191,   355,   510,   267,    40,    40,   350,   -19,  -151,
    -151,    40,    27,  -151,  -151,  -151,  -151,  -151,   364,   365,
     268,   184,   370,    28,   462,   326,   185,   371,   -67,   327,
     328,  -191,   322,   371,   349,    40,   199,   323,   335,   144,
     338,   304,   351,   -19,   340,   463,   486,   342,   300,    37,
      97,   344,   345,  -138,  -138,  -199,  -199,  -199,  -138,   189,
     190,   191,   464,   356,   359,   360,   361,   362,   363,   465,
     343,   367,   321,   -19,   368,   497,   423,   351,    72,   372,
      40,   351,   373,  -187,   205,    55,    40,   290,   158,    37,
      37,    38,   200,   377,   333,   379,    90,   463,   380,    98,
     351,   383,   384,   424,    37,    40,   208,   413,   410,   386,
     387,   388,   389,   446,   464,   198,   393,    97,   395,  -154,
     396,   465,   399,    91,    37,   405,   259,   156,    97,    58,
      59,    60,    61,    62,   414,    89,   356,   415,   365,    40,
     291,    37,   351,   265,    97,   417,    65,    90,    99,   420,
     421,    51,    92,    37,  -187,   425,   472,   505,   351,   167,
      40,   198,   175,   172,   432,  -150,   -19,   449,   445,   436,
     448,   437,   174,   -19,    91,    58,    59,    60,    61,    62,
     204,    37,   456,   270,  -154,  -154,   492,   177,  -154,  -154,
    -154,  -154,  -154,   257,   168,   451,   142,   143,   169,    37,
      40,   288,   474,    92,   262,   475,    40,   477,   284,    40,
     272,   273,   480,   301,   302,   285,    64,   483,   278,   493,
     485,   448,   487,    49,    50,   295,   -19,    37,   144,   293,
    -150,  -150,    40,   491,  -150,  -150,  -150,  -150,  -150,   452,
     453,   279,  -210,   248,   313,   142,   143,   170,   171,    37,
     316,   298,   495,   336,  -216,  -216,   339,  -216,  -216,    37,
     498,   314,    37,   499,   337,   346,   357,   352,   502,   503,
      37,   353,   376,    37,  -108,   392,   507,   144,    37,    37,
     426,   438,   428,   378,   354,   512,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,    37,
    -216,  -216,  -210,  -210,  -210,  -205,   381,   250,  -216,   390,
      40,  -216,   394,   442,   251,  -216,  -216,   103,   104,   441,
     105,   106,    37,   404,   444,   443,   198,   -19,   473,    37,
    -153,   455,   479,   488,   -19,   -19,   481,   471,   484,   508,
      58,    59,    60,    61,    62,   509,   407,   489,   264,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   435,   130,    92,  -205,  -205,  -205,   418,   248,
     482,   131,   506,   149,   132,   431,   375,   -19,   133,   134,
    -216,  -216,   286,  -216,  -216,  -153,  -153,    40,   195,  -153,
    -153,  -153,  -153,  -153,   358,    57,   416,    58,    59,    60,
      61,    62,    63,    64,   193,   490,    58,    59,    60,    61,
      62,   194,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,   206,  -216,  -216,   -81,   -81,
     -81,   247,   248,   250,  -216,    -2,   406,  -216,   476,   494,
     251,  -216,  -216,  -216,  -216,   329,  -216,  -216,    57,   411,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,   412,     0,     0,     0,    57,    69,    58,    59,    60,
      61,    62,    63,    64,    51,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,     0,  -216,
    -216,   -84,   -84,   -84,     0,   397,   250,  -216,     0,    40,
    -216,     0,     0,   251,  -216,  -216,   103,   104,    40,   105,
     106,    58,    59,    60,    61,    62,    57,     0,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,     0,
       0,     0,     0,     0,    69,     0,     0,     0,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,     0,   130,    92,   -85,   -85,   -85,     0,   248,     0,
     131,     0,   -19,   132,     0,     0,     0,   133,   134,  -216,
    -216,     0,  -216,  -216,     1,     0,     2,     3,     4,     0,
       5,     6,     7,     0,     0,     0,     8,     0,     0,     0,
       0,    40,     0,     0,     0,     0,     0,     0,     0,  -103,
       0,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,     0,  -216,  -216,   142,   143,     0,
       0,   -19,   250,  -216,   248,     0,  -216,     0,     0,   251,
    -216,  -216,     0,     0,     0,  -216,  -216,     0,  -216,  -216,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
       0,  -216,  -216,     0,   249,     0,     0,   248,   250,  -216,
       0,     0,  -216,     0,     0,   251,  -216,  -216,  -216,  -216,
       0,  -216,  -216,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     246,   125,   126,   127,     0,     0,     0,     0,     0,     0,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,  -216,
    -216,  -216,  -216,     0,  -216,  -216,     0,     0,     0,     0,
     385,   250,  -216,   324,     0,  -216,     0,    40,   251,  -216,
    -216,     0,     0,     0,   103,   104,     0,   105,   106,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,    40,
     130,    92,     0,     0,     0,     0,   103,   104,   131,   105,
     106,   132,     0,     0,     0,   133,   134,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,     0,   130,    92,   103,   104,     0,   105,   106,     0,
     131,     0,     0,   132,     0,     0,     0,   133,   134,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,     0,
     130,    92,     0,     0,   161,   457,     0,     0,   131,     0,
       0,   132,     0,     0,     0,   133,   134,    57,     0,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
       0,     0,     0,     0,     0,    69,     0,     0,     0,     0,
       0,     0,     0,    51,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   162
};

static const yytype_int16 yycheck[] =
{
       1,    46,   178,    19,    55,     1,     5,   305,    43,    10,
      11,    12,    13,    14,    15,    16,     1,    36,    79,     5,
      21,    22,    23,    24,    25,    26,    27,     0,     5,    84,
     419,    81,    53,    83,    70,    54,     5,    13,    14,     5,
      41,     5,     5,     6,    13,     1,    41,    42,     5,     5,
       1,   485,    70,   487,     5,    12,    12,   242,     5,    94,
       1,    62,    13,     1,    65,     0,    67,    68,    69,    70,
      17,     5,    13,    37,   372,    71,    39,    43,    73,    75,
      96,    80,   471,     5,     1,   383,    89,    72,    64,    90,
      91,    94,    95,    62,    80,    72,    97,    70,    71,   100,
     101,   399,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
      71,   192,   133,   134,    72,   320,    71,   158,   139,   160,
     156,   196,   197,   144,     1,   195,   147,   150,     5,     5,
     151,   154,    74,    70,   155,    72,     5,   342,    75,   175,
       1,   177,    12,   179,     5,     7,     8,     9,    10,    11,
     171,     5,     1,     1,    15,    16,    17,    18,    19,     1,
       0,     1,    70,   184,   185,    41,    42,     5,   189,   190,
     191,     5,   419,     5,     1,     5,   381,    70,     5,   200,
       1,    13,   205,     5,     1,     0,     1,   208,     5,    43,
     211,    70,   397,    70,    71,    65,    17,    73,    75,    76,
      77,    78,   283,    72,     1,     5,     5,   278,     5,    70,
      71,     5,    70,    74,    75,    76,    77,    78,    80,    81,
      17,    70,    70,     0,   471,   246,    75,    75,    70,   250,
     251,    71,    70,    75,   275,     5,   311,    75,   259,    73,
     263,    71,   278,    70,   265,   419,   442,   268,    70,    70,
     305,   272,   273,    70,    71,    70,    71,    72,    75,    76,
      77,    78,   419,   284,     7,     8,     9,    10,    11,   419,
      70,   292,    71,    70,   295,   480,    70,   313,    42,   300,
       5,   317,   303,     0,     1,     1,     5,    12,    52,    70,
      70,    72,    72,   314,    13,   316,    13,   471,   319,    13,
     336,   322,   323,   374,    70,     5,    72,   348,   347,   330,
     331,   332,   333,    13,   471,     1,   337,   372,   339,     5,
     341,   471,   343,    40,    70,   346,    72,     1,   383,    15,
      16,    17,    18,    19,   355,     1,   357,    80,    81,     5,
      65,    70,   378,    72,   399,   366,    22,    13,    62,   370,
     371,    39,    69,    70,    71,   376,   421,    71,   394,    21,
       5,     1,     1,    13,   385,     5,     5,   408,   404,   390,
     406,   392,    21,    12,    40,    15,    16,    17,    18,    19,
      71,    70,   418,    72,    70,    71,   457,     1,    74,    75,
      76,    77,    78,    71,    13,   416,    41,    42,    17,    70,
       5,    72,   423,    69,    71,   426,     5,   428,    13,     5,
      35,    36,   433,    12,    13,    20,    21,   438,     1,   460,
     441,   457,   443,     5,     6,    70,    65,    70,    73,    72,
      70,    71,     5,   454,    74,    75,    76,    77,    78,    12,
      13,    13,     0,     1,     1,    41,    42,    66,    67,    70,
      72,    72,   473,     1,    12,    13,    72,    15,    16,    70,
     481,    72,    70,   484,    72,    27,    13,    74,   489,   490,
      70,    74,    72,    70,    70,    72,   497,    73,    70,    70,
      72,    72,    40,     1,    74,   506,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    70,
      68,    69,    70,    71,    72,     0,     1,    75,    76,    43,
       5,    79,     1,     5,    82,    83,    84,    12,    13,    70,
      15,    16,    70,     1,    72,    70,     1,     5,    71,    70,
       5,    72,    71,     5,    12,    13,    74,   420,    71,    71,
      15,    16,    17,    18,    19,    71,   347,   447,   153,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,   388,    68,    69,    70,    71,    72,   367,     1,
     436,    76,   496,    47,    79,   384,   306,    65,    83,    84,
      12,    13,   173,    15,    16,    70,    71,     5,    81,    74,
      75,    76,    77,    78,   286,    13,   358,    15,    16,    17,
      18,    19,    20,    21,    13,   451,    15,    16,    17,    18,
      19,    20,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    94,    68,    69,    70,    71,
      72,   135,     1,    75,    76,     0,     1,    79,   427,   471,
      82,    83,    84,    12,    13,   252,    15,    16,    13,   347,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,   347,    -1,    -1,    -1,    13,    31,    15,    16,    17,
      18,    19,    20,    21,    39,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    -1,    68,
      69,    70,    71,    72,    -1,     1,    75,    76,    -1,     5,
      79,    -1,    -1,    82,    83,    84,    12,    13,     5,    15,
      16,    15,    16,    17,    18,    19,    13,    -1,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    68,    69,    70,    71,    72,    -1,     1,    -1,
      76,    -1,     5,    79,    -1,    -1,    -1,    83,    84,    12,
      13,    -1,    15,    16,    26,    -1,    28,    29,    30,    -1,
      32,    33,    34,    -1,    -1,    -1,    38,    -1,    -1,    -1,
      -1,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    -1,    68,    69,    41,    42,    -1,
      -1,    74,    75,    76,     1,    -1,    79,    -1,    -1,    82,
      83,    84,    -1,    -1,    -1,    12,    13,    -1,    15,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    73,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      -1,    68,    69,    -1,    71,    -1,    -1,     1,    75,    76,
      -1,    -1,    79,    -1,    -1,    82,    83,    84,    12,    13,
      -1,    15,    16,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    -1,    68,    69,    -1,    -1,    -1,    -1,
      74,    75,    76,     1,    -1,    79,    -1,     5,    82,    83,
      84,    -1,    -1,    -1,    12,    13,    -1,    15,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,     5,
      68,    69,    -1,    -1,    -1,    -1,    12,    13,    76,    15,
      16,    79,    -1,    -1,    -1,    83,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    -1,    68,    69,    12,    13,    -1,    15,    16,    -1,
      76,    -1,    -1,    79,    -1,    -1,    -1,    83,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    -1,
      68,    69,    -1,    -1,     0,     1,    -1,    -1,    76,    -1,
      -1,    79,    -1,    -1,    -1,    83,    84,    13,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    26,    28,    29,    30,    32,    33,    34,    38,    86,
      88,    89,    90,    91,    92,    93,    94,    97,    99,     1,
      95,    70,    70,    70,    70,     5,    70,    70,     0,    95,
      95,    95,    95,    95,    95,    95,    96,    70,    72,   173,
       5,    12,    95,    95,    95,    95,    95,    95,    95,     5,
       6,    39,   100,   168,   169,     1,    95,    13,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    31,
      87,   104,   109,   128,   130,   136,   137,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   151,   154,     1,
      13,    40,    69,   155,   156,   157,   158,   164,    13,    62,
     133,   134,   135,    12,    13,    15,    16,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      68,    76,    79,    83,    84,   139,   160,   162,   163,   164,
     165,   166,    41,    42,    73,   121,   122,   124,   125,   141,
       1,    13,   112,   113,   114,   115,     1,   101,   109,    96,
     168,     0,    71,    98,   173,   174,    72,    21,    13,    17,
      66,    67,    13,    95,    21,     1,    95,     1,    95,     1,
      95,    95,    95,     1,    70,    75,   145,     1,     5,    76,
      77,    78,   138,    13,    20,   146,   147,   147,     1,   148,
      72,   174,    95,    95,    71,     1,   157,   174,    72,   174,
       1,    17,   173,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    61,   163,     1,    71,
      75,    82,   161,    95,    95,    95,    95,    71,    71,    72,
     174,    95,    71,     1,   114,    72,   174,     1,    17,    95,
      72,   173,    35,    36,   102,   110,    96,    96,     1,    13,
      13,    14,    64,    95,    13,    20,   144,   150,    72,   173,
      12,    65,   118,    72,   173,    70,   125,   127,    72,   173,
      70,    12,    13,   131,    71,    95,    95,    95,    95,    95,
     145,   147,    95,     1,    72,   174,    72,     1,    95,    72,
      95,    71,    70,    75,     1,   160,    95,    95,    95,   162,
      13,   119,    43,    13,   129,    95,     1,    72,   174,    72,
      95,    72,    95,    70,    95,    95,    27,   103,   116,    96,
      98,   173,    74,    74,    74,   145,    95,    13,   150,     7,
       8,     9,    10,    11,    80,    81,   152,    95,    95,     1,
      70,    75,    95,    95,   155,   143,    72,    95,     1,    95,
      95,     1,   160,    95,    95,    74,    95,    95,    95,    95,
      43,   123,    72,    95,     1,    95,    95,     1,   160,    95,
     126,   127,    37,   111,     1,    95,     1,   104,   105,   109,
     168,   170,   172,    96,    95,    80,   152,    95,   126,   106,
      95,    95,   155,    70,    98,    95,    72,   160,    40,   159,
     155,   135,    95,    17,   120,   121,    95,    95,    72,   160,
     155,    70,     5,    70,    72,   173,    13,   117,   173,    96,
      74,    95,    12,    13,   153,    72,   173,     1,    98,   107,
     108,   128,   130,   136,   137,   140,   167,   168,   170,   171,
     172,   106,   125,    71,    95,    95,   159,    95,     1,    71,
      95,    74,   122,    95,    71,    95,   127,    95,     5,   118,
     153,    95,    98,    96,   167,    95,   132,   160,    95,    95,
     112,   112,    95,    95,    80,    71,   133,    95,    71,    71,
      72,    80,    95
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
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
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
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

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
        case 12:

/* Line 1455 of yacc.c  */
#line 281 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 287 "../css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframeRule);
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 293 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 299 "../css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 305 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            if (!p->parseValue(p->m_id, p->m_important))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 319 "../css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 326 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 348 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 358 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyval.rule));
  ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 364 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 366 "../css/CSSGrammar.y"
    {
  ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 372 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 377 "../css/CSSGrammar.y"
    {
 ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 383 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 397 "../css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 420 "../css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 421 "../css/CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if ((yyvsp[(2) - (3)].rule)) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
          (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
      }
  ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 448 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 451 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 454 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 457 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 463 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), true);
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 467 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), false);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 473 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 477 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(3) - (3)].mediaList);
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 483 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 486 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ((yyvsp[(2) - (2)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 491 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 494 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 497 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 500 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 506 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 509 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 512 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 515 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 518 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 523 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 526 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 532 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariable((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].valueList));
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 536 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariableDeclarationBlock((yyvsp[(1) - (7)].string));
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 540 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 544 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 548 "../css/CSSGrammar.y"
    {
        /* @variables { varname: } Just reduce away this variable with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 553 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: @variables { varname: *; }, just discard the property/value pair */
        (yyval.boolean) = false;
    ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 560 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 566 "../css/CSSGrammar.y"
    {
    CSSParser* p = static_cast<CSSParser*>(parser);
    if (p->m_styleSheet)
        p->m_styleSheet->addNamespace(p, (yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 576 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 577 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 586 "../css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 592 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 595 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 601 "../css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 608 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 613 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 620 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 623 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 629 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 632 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 635 "../css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 641 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 646 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 654 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 661 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 666 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 671 "../css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 677 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 680 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 686 "../css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 692 "../css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 704 "../css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 705 "../css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframeRule))
            (yyval.keyframesRule)->append((yyvsp[(2) - (3)].keyframeRule));
    ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 713 "../css/CSSGrammar.y"
    {
        (yyval.keyframeRule) = static_cast<CSSParser*>(parser)->createKeyframeRule((yyvsp[(1) - (6)].valueList));
    ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 719 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 724 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 733 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 734 "../css/CSSGrammar.y"
    {
        (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (equalIgnoringCase(static_cast<const String&>(str), "from"))
            (yyval.value).fValue = 0;
        else if (equalIgnoringCase(static_cast<const String&>(str), "to"))
            (yyval.value).fValue = 100;
        else
            YYERROR;
    ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 758 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 761 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 768 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 771 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 774 "../css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 780 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 781 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 782 "../css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 786 "../css/CSSGrammar.y"
    { (yyval.integer) = -1; ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 787 "../css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 791 "../css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selectorList));
    ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 797 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->reusableSelectorVector();
            deleteAllValues(*(yyval.selectorList));
            (yyval.selectorList)->shrink(0);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
        }
    ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 806 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
        } else
            (yyval.selectorList) = 0;
    ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 814 "../css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 820 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 826 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 830 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 834 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if (!(yyvsp[(1) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::Descendant;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (2)].selector)));
            if (Document* doc = p->document())
                doc->setUsesDescendantRules(true);
        }
    ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 849 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if (!(yyvsp[(1) - (3)].selector))
            (yyval.selector) = 0;
        else if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyval.selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = (yyvsp[(2) - (3)].relation);
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(1) - (3)].selector)));
            if ((yyvsp[(2) - (3)].relation) == CSSSelector::Child) {
                if (Document* doc = p->document())
                    doc->setUsesDescendantRules(true);
            } else if ((yyvsp[(2) - (3)].relation) == CSSSelector::DirectAdjacent || (yyvsp[(2) - (3)].relation) == CSSSelector::IndirectAdjacent) {
                if (Document* doc = p->document())
                    doc->setUsesSiblingRules(true);
            }
        }
    ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 869 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 875 "../css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 876 "../css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 877 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 881 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace);
    ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 886 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 893 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->m_defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->m_defaultNamespace);
    ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 899 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        if (p->m_styleSheet)
            (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (2)].string),
                                      p->m_styleSheet->determineNamespace(namespacePrefix));
        else // FIXME: Shouldn't this case be an error?
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (2)].string), p->m_defaultNamespace);
    ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 909 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(3) - (3)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (3)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, (yyvsp[(2) - (3)].string),
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
            else // FIXME: Shouldn't this case be an error?
                (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(2) - (3)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 921 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            AtomicString namespacePrefix = (yyvsp[(1) - (2)].string);
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_styleSheet)
                (yyval.selector)->m_tag = QualifiedName(namespacePrefix, starAtom,
                                          p->m_styleSheet->determineNamespace(namespacePrefix));
        }
    ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 934 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 942 "../css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 950 "../css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 953 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(2) - (2)].selector))
            (yyval.selector) = 0;
        else if ((yyvsp[(1) - (2)].selector)) {
            (yyval.selector) = (yyvsp[(1) - (2)].selector);
            CSSParser* p = static_cast<CSSParser*>(parser);
            CSSSelector* end = (yyvsp[(1) - (2)].selector);
            while (end->tagHistory())
                end = end->tagHistory();
            end->m_relation = CSSSelector::SubSelector;
            end->setTagHistory(p->sinkFloatingSelector((yyvsp[(2) - (2)].selector)));
        }
    ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 966 "../css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 972 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->m_strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
    ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 980 "../css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].string).characters[0] >= '0' && (yyvsp[(1) - (1)].string).characters[0] <= '9') {
            (yyval.selector) = 0;
        } else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::Id;
            if (!p->m_strict)
                (yyvsp[(1) - (1)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
        }
    ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 998 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->m_strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
    ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1009 "../css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1020 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1025 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].integer);
        (yyval.selector)->m_value = (yyvsp[(6) - (8)].string);
    ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1031 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1039 "../css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (9)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (9)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(5) - (9)].integer);
        (yyval.selector)->m_value = (yyvsp[(7) - (9)].string);
    ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1051 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1054 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1057 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1060 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1063 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1066 "../css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1077 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoEmpty ||
                 type == CSSSelector::PseudoFirstChild ||
                 type == CSSSelector::PseudoFirstOfType ||
                 type == CSSSelector::PseudoLastChild ||
                 type == CSSSelector::PseudoLastOfType ||
                 type == CSSSelector::PseudoOnlyChild ||
                 type == CSSSelector::PseudoOnlyOfType) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            Document* doc = p->document();
            if (doc)
                doc->setUsesSiblingRules(true);
        } else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        }
    ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1102 "../css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoElement;
        (yyvsp[(3) - (3)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(3) - (3)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoFirstLine) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesFirstLineRules(true);
        }
    ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1117 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1135 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument(String::number((yyvsp[(3) - (4)].number)));
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1153 "../css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(3) - (4)].string));
        (yyvsp[(2) - (4)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (4)].string);
        CSSSelector::PseudoType type = (yyval.selector)->pseudoType();
        if (type == CSSSelector::PseudoUnknown)
            (yyval.selector) = 0;
        else if (type == CSSSelector::PseudoNthChild ||
                 type == CSSSelector::PseudoNthOfType ||
                 type == CSSSelector::PseudoNthLastChild ||
                 type == CSSSelector::PseudoNthLastOfType) {
            if (p->document())
                p->document()->setUsesSiblingRules(true);
        }
    ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1172 "../css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector))
            (yyval.selector) = 0;
        else {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector) = p->createFloatingSelector();
            (yyval.selector)->m_match = CSSSelector::PseudoClass;
            (yyval.selector)->setSimpleSelector(p->sinkFloatingSelector((yyvsp[(4) - (6)].selector)));
            (yyvsp[(2) - (6)].string).lower();
            (yyval.selector)->m_value = (yyvsp[(2) - (6)].string);
        }
    ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1187 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1190 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1195 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1198 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1201 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1204 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1207 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1213 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1216 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1219 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1222 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1225 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1230 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1233 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1239 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyvsp[(1) - (5)].integer) && (yyvsp[(4) - (5)].valueList)) {
            p->m_valueList = p->sinkFloatingValueList((yyvsp[(4) - (5)].valueList));
            int oldParsedProperties = p->m_numParsedProperties;
            (yyval.boolean) = p->parseValue((yyvsp[(1) - (5)].integer), (yyvsp[(5) - (5)].boolean));
            if (!(yyval.boolean))
                p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
            delete p->m_valueList;
            p->m_valueList = 0;
        }
    ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1253 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        p->m_valueList = new CSSParserValueList;
        p->m_valueList->addValue(p->sinkFloatingValue((yyvsp[(1) - (2)].value)));
        int oldParsedProperties = p->m_numParsedProperties;
        (yyval.boolean) = p->parseValue(CSSPropertyWebkitVariableDeclarationBlock, false);
        if (!(yyval.boolean))
            p->rollbackLastProperties(p->m_numParsedProperties - oldParsedProperties);
        delete p->m_valueList;
        p->m_valueList = 0;
    ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1265 "../css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1269 "../css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.boolean) = false;
    ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1276 "../css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        (yyval.boolean) = false;
    ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1281 "../css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1286 "../css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1291 "../css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        (yyval.boolean) = false;
    ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1296 "../css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1303 "../css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1309 "../css/CSSGrammar.y"
    { (yyval.boolean) = true; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1310 "../css/CSSGrammar.y"
    { (yyval.boolean) = false; ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1314 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1319 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (3)].valueList);
        if ((yyval.valueList)) {
            if ((yyvsp[(2) - (3)].character)) {
                CSSParserValue v;
                v.id = 0;
                v.unit = CSSParserValue::Operator;
                v.iValue = (yyvsp[(2) - (3)].character);
                (yyval.valueList)->addValue(v);
            }
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(3) - (3)].value)));
        }
    ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1333 "../css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1339 "../css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1342 "../css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1345 "../css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1351 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1352 "../css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1353 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1354 "../css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1360 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1361 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1362 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1363 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1364 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1365 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1367 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1370 "../css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (2)].value);
  ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1373 "../css/CSSGrammar.y"
    {;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1377 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1378 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1379 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1380 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1381 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1382 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1383 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1384 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1385 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1386 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1387 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1388 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1389 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1390 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1391 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1392 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1393 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1394 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1395 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1396 "../css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1400 "../css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).string = (yyvsp[(1) - (1)].string);
      (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_VARIABLE_FUNCTION_SYNTAX;
  ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1408 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (5)].string);
        f->args = p->sinkFloatingValueList((yyvsp[(3) - (5)].valueList));
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
    ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1417 "../css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        CSSParserFunction* f = p->createFloatingFunction();
        f->name = (yyvsp[(1) - (3)].string);
        f->args = 0;
        (yyval.value).id = 0;
        (yyval.value).unit = CSSParserValue::Function;
        (yyval.value).function = f;
  ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1433 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 254:

/* Line 1455 of yacc.c  */
#line 1434 "../css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 255:

/* Line 1455 of yacc.c  */
#line 1441 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1444 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1450 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1453 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1464 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1470 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1476 "../css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 4240 "WebCore/tmp/../generated/CSSGrammar.tab.c"
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
#line 1503 "../css/CSSGrammar.y"


