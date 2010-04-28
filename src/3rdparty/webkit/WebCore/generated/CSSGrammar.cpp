
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
#line 1 "css/CSSGrammar.y"


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
#include "CSSPrimitiveValue.h"
#include "CSSPropertyNames.h"
#include "CSSRuleList.h"
#include "CSSSelector.h"
#include "CSSStyleSheet.h"
#include "Document.h"
#include "HTMLNames.h"
#include "MediaList.h"
#include "WebKitCSSKeyframeRule.h"
#include "WebKitCSSKeyframesRule.h"
#include <wtf/FastMalloc.h>
#include <stdlib.h>
#include <string.h>

using namespace WebCore;
using namespace HTMLNames;

#define YYMALLOC fastMalloc
#define YYFREE fastFree

#define YYENABLE_NLS 0
#define YYLTYPE_IS_TRIVIAL 1
#define YYMAXDEPTH 10000
#define YYDEBUG 0

// FIXME: Replace with %parse-param { CSSParser* parser } once we can depend on bison 2.x
#define YYPARSE_PARAM parser
#define YYLEX_PARAM parser



/* Line 189 of yacc.c  */
#line 141 "/WebCore/generated/CSSGrammar.tab.c"

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
     REMS = 294,
     QEMS = 295,
     EMS = 296,
     EXS = 297,
     PXS = 298,
     CMS = 299,
     MMS = 300,
     INS = 301,
     PTS = 302,
     PCS = 303,
     DEGS = 304,
     RADS = 305,
     GRADS = 306,
     TURNS = 307,
     MSECS = 308,
     SECS = 309,
     HERZ = 310,
     KHERZ = 311,
     DIMEN = 312,
     PERCENTAGE = 313,
     FLOATTOKEN = 314,
     INTEGER = 315,
     URI = 316,
     FUNCTION = 317,
     NOTFUNCTION = 318,
     UNICODERANGE = 319,
     VARCALL = 320
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 62 "css/CSSGrammar.y"

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
#line 269 "/WebCore/generated/CSSGrammar.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */

/* Line 264 of yacc.c  */
#line 86 "css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 264 of yacc.c  */
#line 296 "/WebCore/generated/CSSGrammar.tab.c"

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
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1452

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  84
/* YYNRULES -- Number of rules.  */
#define YYNRULES  265
/* YYNRULES -- Number of states.  */
#define YYNSTATES  512

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   320

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    84,     2,    85,     2,     2,
      74,    75,    20,    77,    76,    80,    18,    83,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    17,    73,
       2,    82,    79,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    19,     2,    81,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,    21,    72,    78,     2,     2,     2,
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
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     8,    11,    14,    17,    20,    23,    26,
      29,    36,    43,    49,    55,    61,    67,    73,    74,    77,
      78,    81,    84,    85,    87,    89,    91,    97,   101,   105,
     106,   110,   112,   114,   116,   118,   120,   122,   124,   126,
     128,   130,   132,   133,   137,   139,   141,   143,   145,   147,
     149,   151,   153,   155,   157,   159,   166,   173,   177,   181,
     189,   197,   198,   202,   204,   207,   209,   213,   215,   218,
     222,   227,   231,   237,   242,   247,   254,   259,   267,   270,
     276,   280,   285,   288,   295,   302,   306,   310,   311,   314,
     316,   318,   321,   322,   327,   335,   337,   343,   344,   348,
     349,   351,   353,   355,   360,   361,   363,   365,   370,   373,
     381,   388,   391,   400,   402,   404,   405,   409,   416,   418,
     424,   426,   428,   432,   436,   444,   448,   452,   455,   458,
     461,   463,   464,   466,   468,   474,   476,   481,   484,   487,
     489,   491,   494,   498,   501,   503,   506,   509,   511,   514,
     516,   519,   523,   526,   528,   530,   532,   535,   538,   540,
     542,   544,   546,   548,   551,   554,   559,   568,   574,   584,
     586,   588,   590,   592,   594,   596,   598,   600,   603,   607,
     614,   622,   629,   636,   638,   641,   643,   647,   649,   652,
     655,   659,   663,   668,   672,   678,   683,   688,   695,   701,
     704,   707,   714,   721,   724,   728,   733,   736,   739,   742,
     743,   745,   749,   752,   756,   759,   762,   765,   766,   768,
     771,   774,   777,   780,   784,   787,   790,   792,   795,   797,
     800,   803,   806,   809,   812,   815,   818,   821,   824,   827,
     830,   833,   836,   839,   842,   845,   848,   851,   854,   857,
     860,   863,   866,   868,   874,   878,   881,   884,   886,   889,
     893,   897,   900,   906,   910,   912
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    95,    97,    96,   100,    -1,    88,    95,
      -1,    90,    95,    -1,    92,    95,    -1,    93,    95,    -1,
      94,    95,    -1,    91,    95,    -1,    89,    95,    -1,    28,
      71,    95,   101,    95,    72,    -1,    30,    71,    95,   130,
      95,    72,    -1,    29,    71,    95,   153,    72,    -1,    38,
      71,    95,   109,    72,    -1,    32,    71,    95,   158,    72,
      -1,    33,     5,    95,   122,    72,    -1,    34,    71,    95,
     139,    72,    -1,    -1,    95,     5,    -1,    -1,    96,     6,
      -1,    96,     5,    -1,    -1,    99,    -1,    72,    -1,     0,
      -1,    26,    95,    12,    95,    73,    -1,    26,     1,   168,
      -1,    26,     1,    73,    -1,    -1,   100,   102,    96,    -1,
     138,    -1,   125,    -1,   133,    -1,   134,    -1,   127,    -1,
     113,    -1,   106,    -1,   107,    -1,   101,    -1,   167,    -1,
     166,    -1,    -1,   103,   105,    96,    -1,   138,    -1,   133,
      -1,   134,    -1,   127,    -1,   104,    -1,   167,    -1,   166,
      -1,   113,    -1,   106,    -1,   107,    -1,   125,    -1,    22,
      95,   115,    95,   123,    73,    -1,    22,    95,   115,    95,
     123,   168,    -1,    22,     1,    73,    -1,    22,     1,   168,
      -1,    35,    95,   123,    71,    95,   109,    72,    -1,    36,
      95,   108,    71,    95,   109,    72,    -1,    -1,    37,     5,
     124,    -1,   111,    -1,   110,   111,    -1,   110,    -1,     1,
     169,     1,    -1,     1,    -1,   110,     1,    -1,   111,    73,
      95,    -1,   111,   169,    73,    95,    -1,     1,    73,    95,
      -1,     1,   169,     1,    73,    95,    -1,   110,   111,    73,
      95,    -1,   110,     1,    73,    95,    -1,   110,     1,   169,
       1,    73,    95,    -1,   112,    17,    95,   158,    -1,   112,
      95,    71,    95,   153,    72,    95,    -1,   112,     1,    -1,
     112,    17,    95,     1,   158,    -1,   112,    17,    95,    -1,
     112,    17,    95,     1,    -1,    13,    95,    -1,    27,    95,
     114,   115,    95,    73,    -1,    27,    95,   114,   115,    95,
     168,    -1,    27,     1,   168,    -1,    27,     1,    73,    -1,
      -1,    13,    95,    -1,    12,    -1,    66,    -1,    13,    95,
      -1,    -1,    17,    95,   158,    95,    -1,    74,    95,   116,
      95,   117,    75,    95,    -1,   118,    -1,   119,    95,    43,
      95,   118,    -1,    -1,    43,    95,   119,    -1,    -1,    41,
      -1,    42,    -1,   119,    -1,   121,    95,   126,   120,    -1,
      -1,   124,    -1,   122,    -1,   124,    76,    95,   122,    -1,
     124,     1,    -1,    24,    95,   124,    71,    95,   103,   165,
      -1,    24,    95,    71,    95,   103,   165,    -1,    13,    95,
      -1,    31,    95,   128,    95,    71,    95,   129,    72,    -1,
      13,    -1,    12,    -1,    -1,   129,   130,    95,    -1,   131,
      95,    71,    95,   153,    72,    -1,   132,    -1,   131,    95,
      76,    95,   132,    -1,    63,    -1,    13,    -1,    23,     1,
     168,    -1,    23,     1,    73,    -1,    25,    95,    71,    95,
     153,    72,    95,    -1,    25,     1,   168,    -1,    25,     1,
      73,    -1,    77,    95,    -1,    78,    95,    -1,    79,    95,
      -1,   137,    -1,    -1,    80,    -1,    77,    -1,   139,    71,
      95,   153,    98,    -1,   141,    -1,   139,    76,    95,   141,
      -1,   139,     1,    -1,   141,     5,    -1,   143,    -1,   140,
      -1,   140,   143,    -1,   141,   135,   143,    -1,   141,     1,
      -1,    21,    -1,    20,    21,    -1,    13,    21,    -1,   144,
      -1,   144,   145,    -1,   145,    -1,   142,   144,    -1,   142,
     144,   145,    -1,   142,   145,    -1,    13,    -1,    20,    -1,
     146,    -1,   145,   146,    -1,   145,     1,    -1,    16,    -1,
      15,    -1,   147,    -1,   149,    -1,   152,    -1,    18,    13,
      -1,    13,    95,    -1,    19,    95,   148,    81,    -1,    19,
      95,   148,   150,    95,   151,    95,    81,    -1,    19,    95,
     142,   148,    81,    -1,    19,    95,   142,   148,   150,    95,
     151,    95,    81,    -1,    82,    -1,     7,    -1,     8,    -1,
       9,    -1,    10,    -1,    11,    -1,    13,    -1,    12,    -1,
      17,    13,    -1,    17,    17,    13,    -1,    17,    67,    95,
      14,    95,    75,    -1,    17,    67,    95,   136,    65,    95,
      75,    -1,    17,    67,    95,    13,    95,    75,    -1,    17,
      68,    95,   143,    95,    75,    -1,   155,    -1,   154,   155,
      -1,   154,    -1,     1,   169,     1,    -1,     1,    -1,   154,
       1,    -1,   154,   169,    -1,   155,    73,    95,    -1,   155,
     169,    95,    -1,   155,   169,    73,    95,    -1,     1,    73,
      95,    -1,     1,   169,     1,    73,    95,    -1,   154,   155,
      73,    95,    -1,   154,     1,    73,    95,    -1,   154,     1,
     169,     1,    73,    95,    -1,   156,    17,    95,   158,   157,
      -1,   162,    95,    -1,   156,     1,    -1,   156,    17,    95,
       1,   158,   157,    -1,   156,    17,    95,   158,   157,     1,
      -1,    40,    95,    -1,   156,    17,    95,    -1,   156,    17,
      95,     1,    -1,   156,   168,    -1,    13,    95,    -1,    40,
      95,    -1,    -1,   160,    -1,   158,   159,   160,    -1,   158,
     169,    -1,   158,   169,     1,    -1,   158,     1,    -1,    83,
      95,    -1,    76,    95,    -1,    -1,   161,    -1,   137,   161,
      -1,    12,    95,    -1,    13,    95,    -1,    62,    95,    -1,
     137,    62,    95,    -1,    66,    95,    -1,    69,    95,    -1,
     164,    -1,    84,    95,    -1,   163,    -1,   162,    95,    -1,
      85,    95,    -1,    65,    95,    -1,    64,    95,    -1,    63,
      95,    -1,    48,    95,    -1,    49,    95,    -1,    50,    95,
      -1,    51,    95,    -1,    52,    95,    -1,    53,    95,    -1,
      54,    95,    -1,    55,    95,    -1,    56,    95,    -1,    57,
      95,    -1,    58,    95,    -1,    59,    95,    -1,    60,    95,
      -1,    61,    95,    -1,    46,    95,    -1,    45,    95,    -1,
      47,    95,    -1,    44,    95,    -1,    70,    -1,    67,    95,
     158,    75,    95,    -1,    67,    95,     1,    -1,    15,    95,
      -1,    16,    95,    -1,    98,    -1,     1,    98,    -1,    39,
       1,   168,    -1,    39,     1,    73,    -1,     1,   168,    -1,
      71,     1,   169,     1,    98,    -1,    71,     1,    98,    -1,
     168,    -1,   169,     1,   168,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   269,   269,   270,   271,   272,   273,   274,   275,   276,
     280,   286,   292,   298,   304,   318,   325,   335,   336,   339,
     341,   342,   345,   347,   352,   353,   357,   363,   365,   369,
     371,   379,   380,   381,   382,   383,   384,   385,   386,   390,
     393,   394,   398,   399,   410,   411,   412,   413,   417,   418,
     419,   420,   421,   422,   423,   428,   431,   434,   437,   443,
     447,   453,   457,   463,   466,   471,   474,   477,   480,   486,
     489,   492,   495,   498,   503,   506,   512,   516,   520,   524,
     528,   533,   540,   546,   550,   553,   556,   562,   563,   567,
     568,   572,   578,   581,   587,   594,   599,   606,   609,   615,
     618,   621,   627,   632,   640,   643,   647,   652,   657,   663,
     666,   672,   678,   685,   686,   690,   691,   699,   705,   710,
     719,   720,   744,   747,   753,   757,   760,   766,   767,   768,
     772,   773,   777,   778,   782,   788,   798,   807,   813,   819,
     822,   826,   842,   862,   868,   869,   870,   874,   879,   886,
     892,   902,   914,   927,   935,   943,   946,   959,   965,   973,
     985,   986,   987,   991,  1002,  1013,  1018,  1024,  1032,  1044,
    1047,  1050,  1053,  1056,  1059,  1065,  1066,  1070,  1100,  1120,
    1138,  1156,  1175,  1190,  1193,  1198,  1201,  1204,  1207,  1210,
    1216,  1219,  1222,  1225,  1228,  1231,  1236,  1239,  1245,  1259,
    1271,  1275,  1282,  1287,  1292,  1297,  1302,  1309,  1315,  1316,
    1320,  1325,  1339,  1342,  1345,  1351,  1354,  1357,  1363,  1364,
    1365,  1366,  1372,  1373,  1374,  1375,  1376,  1377,  1379,  1382,
    1385,  1391,  1392,  1393,  1394,  1395,  1396,  1397,  1398,  1399,
    1400,  1401,  1402,  1403,  1404,  1405,  1406,  1407,  1408,  1409,
    1410,  1411,  1422,  1430,  1439,  1455,  1456,  1463,  1466,  1472,
    1475,  1481,  1499,  1502,  1508,  1509
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
  "MEDIA_NOT", "MEDIA_AND", "REMS", "QEMS", "EMS", "EXS", "PXS", "CMS",
  "MMS", "INS", "PTS", "PCS", "DEGS", "RADS", "GRADS", "TURNS", "MSECS",
  "SECS", "HERZ", "KHERZ", "DIMEN", "PERCENTAGE", "FLOATTOKEN", "INTEGER",
  "URI", "FUNCTION", "NOTFUNCTION", "UNICODERANGE", "VARCALL", "'{'",
  "'}'", "';'", "'('", "')'", "','", "'+'", "'~'", "'>'", "'-'", "']'",
  "'='", "'/'", "'#'", "'%'", "$accept", "stylesheet", "webkit_rule",
  "webkit_keyframe_rule", "webkit_decls", "webkit_variables_decls",
  "webkit_value", "webkit_mediaquery", "webkit_selector", "maybe_space",
  "maybe_sgml", "maybe_charset", "closing_brace", "charset", "rule_list",
  "valid_rule", "rule", "block_rule_list", "block_valid_rule",
  "block_rule", "import", "variables_rule", "variables_media_list",
  "variables_declaration_list", "variables_decl_list",
  "variables_declaration", "variable_name", "namespace", "maybe_ns_prefix",
  "string_or_uri", "media_feature", "maybe_media_value", "media_query_exp",
  "media_query_exp_list", "maybe_and_media_query_exp_list",
  "maybe_media_restrictor", "media_query", "maybe_media_list",
  "media_list", "media", "medium", "keyframes", "keyframe_name",
  "keyframes_rule", "keyframe_rule", "key_list", "key", "page",
  "font_face", "combinator", "maybe_unary_operator", "unary_operator",
  "ruleset", "selector_list", "selector_with_trailing_whitespace",
  "selector", "namespace_selector", "simple_selector", "element_name",
  "specifier_list", "specifier", "class", "attr_name", "attrib", "match",
  "ident_or_string", "pseudo", "declaration_list", "decl_list",
  "declaration", "property", "prio", "expr", "operator", "term",
  "unary_term", "variable_reference", "function", "hexcolor", "save_block",
  "invalid_at", "invalid_rule", "invalid_block", "invalid_block_list", 0
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
     320,   123,   125,    59,    40,    41,    44,    43,   126,    62,
      45,    93,    61,    47,    35,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    86,    87,    87,    87,    87,    87,    87,    87,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    95,    96,
      96,    96,    97,    97,    98,    98,    99,    99,    99,   100,
     100,   101,   101,   101,   101,   101,   101,   101,   101,   102,
     102,   102,   103,   103,   104,   104,   104,   104,   105,   105,
     105,   105,   105,   105,   105,   106,   106,   106,   106,   107,
     107,   108,   108,   109,   109,   109,   109,   109,   109,   110,
     110,   110,   110,   110,   110,   110,   111,   111,   111,   111,
     111,   111,   112,   113,   113,   113,   113,   114,   114,   115,
     115,   116,   117,   117,   118,   119,   119,   120,   120,   121,
     121,   121,   122,   122,   123,   123,   124,   124,   124,   125,
     125,   126,   127,   128,   128,   129,   129,   130,   131,   131,
     132,   132,   133,   133,   134,   134,   134,   135,   135,   135,
     136,   136,   137,   137,   138,   139,   139,   139,   140,   141,
     141,   141,   141,   141,   142,   142,   142,   143,   143,   143,
     143,   143,   143,   144,   144,   145,   145,   145,   146,   146,
     146,   146,   146,   147,   148,   149,   149,   149,   149,   150,
     150,   150,   150,   150,   150,   151,   151,   152,   152,   152,
     152,   152,   152,   153,   153,   153,   153,   153,   153,   153,
     154,   154,   154,   154,   154,   154,   154,   154,   155,   155,
     155,   155,   155,   155,   155,   155,   155,   156,   157,   157,
     158,   158,   158,   158,   158,   159,   159,   159,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   161,   161,   161,   161,   161,   161,   161,   161,
     161,   161,   162,   163,   163,   164,   164,   165,   165,   166,
     166,   167,   168,   168,   169,   169
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     2,     2,     2,     2,     2,     2,
       6,     6,     5,     5,     5,     5,     5,     0,     2,     0,
       2,     2,     0,     1,     1,     1,     5,     3,     3,     0,
       3,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     6,     3,     3,     7,
       7,     0,     3,     1,     2,     1,     3,     1,     2,     3,
       4,     3,     5,     4,     4,     6,     4,     7,     2,     5,
       3,     4,     2,     6,     6,     3,     3,     0,     2,     1,
       1,     2,     0,     4,     7,     1,     5,     0,     3,     0,
       1,     1,     1,     4,     0,     1,     1,     4,     2,     7,
       6,     2,     8,     1,     1,     0,     3,     6,     1,     5,
       1,     1,     3,     3,     7,     3,     3,     2,     2,     2,
       1,     0,     1,     1,     5,     1,     4,     2,     2,     1,
       1,     2,     3,     2,     1,     2,     2,     1,     2,     1,
       2,     3,     2,     1,     1,     1,     2,     2,     1,     1,
       1,     1,     1,     2,     2,     4,     8,     5,     9,     1,
       1,     1,     1,     1,     1,     1,     1,     2,     3,     6,
       7,     6,     6,     1,     2,     1,     3,     1,     2,     2,
       3,     3,     4,     3,     5,     4,     4,     6,     5,     2,
       2,     6,     6,     2,     3,     4,     2,     2,     2,     0,
       1,     3,     2,     3,     2,     2,     2,     0,     1,     2,
       2,     2,     2,     3,     2,     2,     1,     2,     1,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     1,     5,     3,     2,     2,     1,     2,     3,
       3,     2,     5,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      17,     0,     0,     0,     0,     0,     0,     0,     0,    17,
      17,    17,    17,    17,    17,    17,    22,    17,    17,    17,
      17,    17,    17,    17,     1,     3,     9,     4,     8,     5,
       6,     7,    18,     0,    19,    23,     0,     0,     0,     0,
      99,     0,     0,     0,     0,    29,   153,   159,   158,     0,
       0,    17,   154,   144,     0,     0,    17,     0,     0,    17,
      17,    17,    17,    37,    38,    36,    32,    35,    33,    34,
      31,     0,   140,     0,     0,   139,   147,     0,   155,   160,
     161,   162,   187,    17,    17,   252,     0,     0,   183,     0,
      17,   121,   120,    17,    17,   118,    17,    17,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17,   133,   132,    17,    17,     0,
       0,   210,   218,    17,   228,   226,   100,   101,    17,    95,
     102,    17,     0,     0,    67,    17,     0,     0,    63,     0,
       0,    28,    27,    17,    21,    20,     0,   146,   177,     0,
      17,    17,   163,     0,   145,     0,     0,     0,    99,     0,
       0,     0,    87,     0,    99,    61,     0,   137,    17,    17,
     141,   143,   138,    17,    17,    17,     0,   153,   154,   150,
       0,     0,   157,   156,    17,   264,     0,   207,   203,    12,
     188,   184,     0,    17,     0,   200,    17,   206,   199,     0,
       0,   220,   221,   255,   256,   251,   249,   248,   250,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   222,   233,   232,   231,   224,     0,   225,
     227,   230,    17,   219,   214,    14,    17,    17,     0,     0,
     229,     0,     0,     0,    15,    16,    17,     0,    82,    13,
      68,    64,    17,     0,    78,    17,     0,     0,     0,     0,
       0,    39,    19,    41,    40,   178,   131,     0,    17,     0,
       0,     0,    57,    58,    89,    90,    17,   123,   122,    17,
     106,     0,   126,   125,    17,    86,    85,    17,     0,   114,
     113,    17,     0,     0,     0,     0,    10,     0,     0,   127,
     128,   129,   142,     0,   193,   186,    17,     0,    17,     0,
     190,    17,   191,     0,    11,    17,    17,   254,     0,   223,
     216,   215,   211,   213,    17,    17,    17,    17,    97,    71,
      66,    17,     0,    17,    69,    17,     0,    17,    25,    24,
     263,     0,    26,   261,     0,    30,    17,    17,     0,   130,
      17,   164,    17,     0,   170,   171,   172,   173,   174,   165,
     169,    17,   104,    42,   108,    17,    17,     0,    88,    17,
       0,    17,    99,    17,     0,     0,    17,   265,   196,     0,
     195,   192,   205,     0,     0,     0,    17,    91,    92,     0,
     111,    17,   103,    17,    74,     0,    73,    70,    81,     0,
       0,     0,   260,   259,     0,     0,    17,     0,   167,    17,
       0,     0,     0,    42,    99,     0,     0,    17,     0,     0,
       0,   134,   194,    17,     0,    17,     0,     0,   119,   253,
      17,     0,    96,     0,    72,    17,     0,     0,   262,   181,
     179,     0,   182,     0,   176,   175,    17,    55,    56,     0,
     257,    48,    19,    52,    53,    51,    54,    47,    45,    46,
      44,   110,    50,    49,     0,   107,    17,    83,    84,   115,
       0,     0,   197,   201,   208,   202,   117,     0,    17,    98,
      75,    17,   180,    17,     0,   258,    43,   109,   124,     0,
      59,    60,     0,    94,    77,     0,   166,   112,    17,    93,
     168,   116
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     8,     9,    10,    11,    12,    13,    14,    15,   252,
      45,    34,   460,    35,   156,    62,   272,   422,   461,   462,
      63,    64,   305,   146,   147,   148,   149,    65,   298,   286,
     335,   441,   139,   140,   402,   141,   290,   302,   303,    66,
     338,    67,   301,   499,    93,    94,    95,    68,    69,   186,
     358,   129,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,   281,    80,   371,   456,    81,    86,    87,    88,
      89,   436,   130,   248,   131,   132,   133,   134,   135,   471,
     472,   473,   195,   249
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -394
static const yytype_int16 yypact[] =
{
     466,   -18,    14,   125,   131,    42,   141,   164,    68,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,    37,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,    67,    67,    67,    67,    67,
      67,    67,  -394,   453,  -394,  -394,   964,   304,   317,  1248,
     246,   524,    36,   -25,    38,   481,   231,  -394,  -394,   341,
     257,  -394,   270,  -394,   354,   327,  -394,   253,   416,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,    75,   598,   196,   674,  -394,   680,   154,  -394,  -394,
    -394,  -394,    -4,  -394,  -394,  -394,   261,   413,   118,   198,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,   782,
     960,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
       2,  -394,   281,    78,   339,  -394,   320,   173,   351,   219,
     398,  -394,  -394,  -394,  -394,  -394,   739,  -394,  -394,   410,
    -394,  -394,  -394,   447,  -394,   366,    23,   406,   298,   407,
     182,   419,   297,   429,   326,    29,   171,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  -394,  -394,   598,  -394,  -394,   680,
     372,   456,  -394,  -394,  -394,  -394,   435,    67,    67,  -394,
     434,   386,    91,  -394,   279,  -394,  -394,  -394,    67,   213,
     129,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,    67,    67,
      67,    67,    67,    67,    67,    67,    67,    67,  1185,    67,
      67,    67,  -394,  -394,  -394,  -394,  -394,  -394,  1307,   509,
      67,   333,    27,   365,  -394,  -394,  -394,   437,    67,  -394,
     444,   393,  -394,     4,  -394,  -394,   235,   177,    22,   418,
     500,  -394,  -394,  -394,  -394,  -394,    60,   524,   231,   270,
     493,   157,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,    86,  -394,  -394,  -394,  -394,  -394,  -394,   352,  -394,
    -394,  -394,   440,   146,   507,   445,  -394,   304,   524,    67,
      67,    67,  -394,   607,    67,   465,  -394,   518,  -394,   418,
      67,  -394,    67,   661,  -394,  -394,  -394,  -394,  1034,    67,
      67,    67,  -394,   418,  -394,  -394,  -394,  -394,   477,    67,
     475,  -394,   522,  -394,    67,  -394,  1111,  -394,  -394,  -394,
    -394,   529,  -394,  -394,   479,   481,  -394,  -394,   461,  -394,
    -394,    67,  -394,   200,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,   389,    67,  -394,  -394,  -394,   304,    67,  -394,
     271,  -394,    -3,  -394,    64,   236,  -394,  -394,    67,   525,
      67,    67,  1307,   587,   304,   317,  -394,    67,    35,   190,
      67,  -394,  -394,  -394,    67,   533,    67,    67,  1307,   735,
     304,   177,  -394,  -394,     3,    81,  -394,   185,  -394,  -394,
     457,   534,  1380,    67,   246,   505,   322,  -394,    36,   179,
      36,  -394,    67,  -394,   587,  -394,   227,   511,  -394,    67,
    -394,   472,  -394,   190,    67,  -394,   809,   519,  -394,  -394,
    -394,   208,  -394,   457,  -394,  -394,  -394,  -394,  -394,   177,
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,
    -394,  -394,  -394,  -394,  1380,  -394,  -394,  -394,  -394,    67,
     523,   537,    67,  -394,    67,  -394,  -394,  1248,  -394,     2,
      67,  -394,  -394,  -394,    -1,  -394,   481,  -394,    67,   392,
    -394,  -394,   883,    67,    67,     1,  -394,  -394,  -394,    67,
    -394,    67
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,  -394,     0,
    -269,  -394,  -236,  -394,  -394,   441,  -394,   167,  -394,  -394,
    -393,  -386,  -394,   -24,  -394,   454,  -394,  -240,  -394,   312,
    -394,  -394,   221,   178,  -394,  -394,   -39,   256,  -166,  -151,
    -394,  -145,  -394,  -394,   130,  -394,   260,  -126,   -90,  -394,
    -394,   399,   -59,   589,  -394,   357,   506,   -56,   594,   -50,
     -47,  -394,   400,  -394,   318,   247,  -394,  -252,  -394,   601,
    -394,   267,  -194,  -394,   455,   573,   -12,  -394,  -394,   230,
     579,   581,    92,    44
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -218
static const yytype_int16 yytable[] =
{
      16,   142,   291,   355,    32,   319,    32,   -17,    32,    25,
      26,    27,    28,    29,    30,    31,   180,    36,    37,    38,
      39,    40,    41,    42,   190,    90,   191,    32,    32,   463,
     193,   350,    32,    44,    32,   284,   464,   144,   136,   137,
      32,    32,    32,    32,   328,   -17,   150,    21,   151,   145,
     153,   163,   440,    17,   166,   384,   168,   170,   172,   173,
     174,   175,   176,    33,   348,    32,   304,   150,    24,   194,
     336,   138,    32,   356,   357,    90,   177,   345,   449,   177,
     506,   463,   510,   197,   198,    18,    32,   374,   464,   285,
     208,  -189,   319,   209,   210,   352,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   425,   196,   240,   241,   393,
     312,   202,   204,   250,    32,   152,   349,   125,   251,   313,
     126,   253,   437,   193,   193,   258,   178,   374,   431,   266,
     255,   179,   409,   268,   179,   192,   450,   375,   447,  -149,
     276,   277,   376,  -189,   364,   365,   366,   367,   368,    47,
      48,    49,    50,    51,   260,   448,    32,   348,   307,   308,
     374,   207,   465,   309,   310,   311,   145,    32,   257,   150,
      32,   203,   263,   496,   314,    32,    19,   181,   434,   205,
     325,   182,    20,   320,   322,   326,   323,   364,   365,   366,
     367,   368,    22,    32,   446,   206,   429,  -105,    32,  -105,
     264,   360,   376,   495,   -17,  -149,  -149,  -198,   485,  -149,
    -149,  -149,  -149,  -149,   465,    23,   265,   181,   369,   370,
      32,   182,   329,   306,   317,   -65,   330,   331,   150,   349,
     -62,    32,   157,   294,   169,   376,   339,   283,   -17,   288,
     452,   293,   344,   296,   138,   346,   193,  -135,  -135,   150,
     162,   466,  -135,   183,   184,   185,    32,   467,   361,   -17,
     319,   418,   370,   492,   -17,   324,   372,   136,   137,   373,
     -17,   164,   -17,   502,   377,    90,   468,   378,  -198,  -198,
    -198,   380,    32,    32,   342,    82,   347,  -136,  -136,    32,
     297,   351,  -136,   183,   184,   185,   388,    83,   390,   -17,
     138,   391,    32,   466,   -17,   394,   395,    32,   167,   467,
      91,    32,   469,   199,   397,   398,   399,   400,    32,   136,
     137,   404,   427,   406,    84,   407,   334,   410,   468,   -17,
     -17,   -17,   321,   254,   158,   165,   414,   415,   159,   -17,
     417,   353,   361,   470,   284,    90,   -17,   136,   137,   289,
      32,   420,   138,   192,    85,   423,   424,  -152,   337,   426,
      92,   428,    90,   430,   469,   475,   432,    47,    48,    49,
      50,    51,   259,   150,    32,   477,   439,  -104,    90,   267,
     138,   443,   -99,   444,   480,    91,   481,   387,   160,   161,
     150,   387,   256,  -185,   200,   470,   451,   171,   285,   453,
     -17,   -17,   150,   275,   262,   387,    83,   479,   -17,   -17,
     136,   137,   387,   482,    32,   484,   315,   150,   340,   282,
     487,   299,   300,  -152,  -152,   490,   413,  -152,  -152,  -152,
    -152,  -152,    32,    84,    43,    92,   494,   192,   -17,   318,
     278,  -148,    32,   138,   507,   -17,   343,   279,    53,   454,
     455,    47,    48,    49,    50,    51,   498,   150,   150,   287,
     292,   387,   -17,    85,   150,  -185,   154,   155,   503,   150,
     150,   504,   295,   505,     1,     2,     3,   387,     4,     5,
       6,   354,   509,   387,     7,   150,   362,   316,   511,  -212,
     333,   381,   382,   458,  -212,   150,   383,   341,   478,   389,
     401,  -212,  -212,   405,  -212,  -212,   416,  -148,  -148,    32,
     411,  -148,  -148,  -148,  -148,  -148,   150,    46,   386,    47,
      48,    49,    50,    51,    52,    53,   150,   488,   403,  -212,
     150,   353,   412,  -212,  -212,  -212,  -212,  -212,  -212,  -212,
    -212,  -212,  -212,  -212,  -212,  -212,  -212,  -212,  -212,  -212,
    -212,  -212,  -212,  -212,  -212,  -212,  -212,   476,  -212,  -212,
    -212,  -212,  -212,   486,  -212,  -212,  -212,  -209,   244,  -212,
     474,   491,  -212,  -212,  -212,   500,   150,   271,   433,  -217,
    -217,   261,  -217,  -217,   150,   150,   445,   457,   192,   501,
     379,    46,  -151,    47,    48,    49,    50,    51,    52,    53,
     442,   489,    47,    48,    49,    50,    51,   435,   421,   508,
     143,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,   438,  -217,  -217,   150,  -209,
    -209,  -204,   392,   246,  -217,   385,    32,  -217,   189,   280,
     247,  -217,  -217,    96,    97,   359,    98,    99,  -151,  -151,
     363,   419,  -151,  -151,  -151,  -151,  -151,   187,   201,    47,
      48,    49,    50,    51,   188,    47,    48,    49,    50,    51,
     493,   483,   243,   332,   497,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,     0,
     124,    85,  -204,  -204,  -204,   273,   244,   274,   125,    -2,
     269,   126,     0,     0,     0,   127,   128,  -217,  -217,     0,
    -217,  -217,    46,     0,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,     0,    58,     0,     0,     0,
      59,     0,     0,     0,    60,    61,     0,     0,   270,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,     0,  -217,  -217,   150,   -76,   -76,     0,
     244,   246,  -217,     0,     0,  -217,     0,     0,   247,  -217,
    -217,  -217,  -217,     0,  -217,  -217,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   242,   119,   120,   121,     0,     0,
       0,     0,     0,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,     0,  -217,  -217,
     150,   -79,   -79,     0,   244,   246,  -217,     0,   -17,  -217,
       0,     0,   247,  -217,  -217,  -217,  -217,     0,  -217,  -217,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,     0,  -217,  -217,   150,     0,     0,     0,   -17,   246,
    -217,   244,     0,  -217,     0,     0,   247,  -217,  -217,    32,
       0,     0,  -217,  -217,     0,  -217,  -217,    46,     0,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
       0,    58,     0,     0,     0,    59,     0,     0,     0,    60,
      61,     0,     0,     0,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,     0,  -217,
    -217,   150,   245,     0,     0,   244,   246,  -217,     0,     0,
    -217,     0,     0,   247,  -217,  -217,  -217,  -217,     0,  -217,
    -217,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,  -217,
    -217,  -217,     0,  -217,  -217,   150,     0,     0,     0,   396,
     246,  -217,   408,     0,  -217,     0,    32,   247,  -217,  -217,
       0,     0,     0,    96,    97,     0,    98,    99,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,     0,
     124,    85,   -80,   -80,   -80,     0,   327,     0,   125,     0,
      32,   126,     0,     0,     0,   127,   128,    96,    97,     0,
      98,    99,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,    32,   124,    85,     0,     0,     0,     0,
      96,    97,   125,    98,    99,   126,     0,     0,     0,   127,
     128,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,     0,   124,    85,    96,
      97,     0,    98,    99,     0,   125,     0,     0,   126,     0,
       0,     0,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,     0,   124,    85,     0,     0,
     348,   459,     0,     0,   125,     0,     0,   126,     0,     0,
       0,   127,   128,    46,     0,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,     0,    58,     0,     0,
       0,    59,     0,     0,     0,    60,    61,     0,     0,   270,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   349
};

static const yytype_int16 yycheck[] =
{
       0,    40,   168,   272,     5,     1,     5,     5,     5,     9,
      10,    11,    12,    13,    14,    15,    72,    17,    18,    19,
      20,    21,    22,    23,    74,    37,    76,     5,     5,   422,
      77,   267,     5,    33,     5,    12,   422,     1,    41,    42,
       5,     5,     5,     5,   238,    43,    71,     5,    73,    13,
      12,    51,    17,    71,    54,   307,    56,    57,    58,    59,
      60,    61,    62,    26,     0,     5,    37,    71,     0,    73,
      43,    74,     5,    13,    14,    87,     1,    73,    75,     1,
      81,   474,    81,    83,    84,    71,     5,     1,   474,    66,
      90,     0,     1,    93,    94,    73,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   377,    82,   127,   128,   323,
     186,    87,    88,   133,     5,    43,    72,    77,   138,   189,
      80,   141,   394,   190,   191,   145,    71,     1,   384,   149,
      72,    76,   346,   153,    76,     1,    75,    71,   410,     5,
     160,   161,    76,    72,     7,     8,     9,    10,    11,    15,
      16,    17,    18,    19,     1,   411,     5,     0,   178,   179,
       1,    89,   422,   183,   184,   185,    13,     5,   144,    71,
       5,    73,   148,   462,   194,     5,    71,     1,   392,     1,
      71,     5,    71,   203,   204,    76,   206,     7,     8,     9,
      10,    11,    71,     5,   408,    17,   382,    71,     5,    73,
       1,   277,    76,   459,     5,    71,    72,     0,     1,    75,
      76,    77,    78,    79,   474,    71,    17,     1,    81,    82,
       5,     5,   242,    72,   200,    72,   246,   247,    71,    72,
      71,     5,    21,    71,     1,    76,   256,   165,     5,   167,
      75,   169,   262,   171,    74,   265,   313,    71,    72,    71,
      13,   422,    76,    77,    78,    79,     5,   422,   278,     0,
       1,    81,    82,    75,     5,    72,   286,    41,    42,   289,
      71,    21,    13,   487,   294,   307,   422,   297,    71,    72,
      73,   301,     5,     5,   260,     1,    71,    71,    72,     5,
      13,   267,    76,    77,    78,    79,   316,    13,   318,    40,
      74,   321,     5,   474,    71,   325,   326,     5,     1,   474,
      13,     5,   422,    72,   334,   335,   336,   337,     5,    41,
      42,   341,    71,   343,    40,   345,    13,   347,   474,    70,
      71,    72,    73,    72,    13,     1,   356,   357,    17,     5,
     360,   269,   362,   422,    12,   377,    12,    41,    42,    71,
       5,   371,    74,     1,    70,   375,   376,     5,    13,   379,
      63,   381,   394,   383,   474,   424,   386,    15,    16,    17,
      18,    19,    72,    71,     5,    73,   396,    71,   410,     1,
      74,   401,    13,   403,   428,    13,   430,   315,    67,    68,
      71,   319,    73,     0,     1,   474,   416,     1,    66,   419,
      66,     5,    71,    13,    73,   333,    13,   427,    12,    13,
      41,    42,   340,   433,     5,   435,     1,    71,     1,    73,
     440,    12,    13,    71,    72,   445,   354,    75,    76,    77,
      78,    79,     5,    40,     1,    63,   456,     1,     5,    73,
      13,     5,     5,    74,    72,    12,    73,    20,    21,    12,
      13,    15,    16,    17,    18,    19,   476,    71,    71,    73,
      73,   389,    66,    70,    71,    72,     5,     6,   488,    71,
      71,   491,    73,   493,    28,    29,    30,   405,    32,    33,
      34,     1,   502,   411,    38,    71,    13,    73,   508,     0,
       1,    71,     5,   421,     5,    71,    71,    73,   426,     1,
      43,    12,    13,     1,    15,    16,    65,    71,    72,     5,
       1,    75,    76,    77,    78,    79,    71,    13,    73,    15,
      16,    17,    18,    19,    20,    21,    71,    75,    73,    40,
      71,   459,    73,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    72,    69,    70,
      71,    72,    73,    72,    75,    76,    77,     0,     1,    80,
     423,    72,    83,    84,    85,    72,    71,   156,    73,    12,
      13,   147,    15,    16,    71,    71,    73,    73,     1,    72,
     298,    13,     5,    15,    16,    17,    18,    19,    20,    21,
     399,   443,    15,    16,    17,    18,    19,    40,   372,   499,
      41,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,   395,    69,    70,    71,    72,
      73,     0,     1,    76,    77,   308,     5,    80,    74,   163,
      83,    84,    85,    12,    13,   276,    15,    16,    71,    72,
     280,   363,    75,    76,    77,    78,    79,    13,    87,    15,
      16,    17,    18,    19,    20,    15,    16,    17,    18,    19,
     453,   434,   129,   248,   474,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    71,    72,    73,   156,     1,   156,    77,     0,
       1,    80,    -1,    -1,    -1,    84,    85,    12,    13,    -1,
      15,    16,    13,    -1,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    -1,    27,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    35,    36,    -1,    -1,    39,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    -1,    69,    70,    71,    72,    73,    -1,
       1,    76,    77,    -1,    -1,    80,    -1,    -1,    83,    84,
      85,    12,    13,    -1,    15,    16,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    -1,    69,    70,
      71,    72,    73,    -1,     1,    76,    77,    -1,     5,    80,
      -1,    -1,    83,    84,    85,    12,    13,    -1,    15,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    -1,    69,    70,    71,    -1,    -1,    -1,    75,    76,
      77,     1,    -1,    80,    -1,    -1,    83,    84,    85,     5,
      -1,    -1,    12,    13,    -1,    15,    16,    13,    -1,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      -1,    27,    -1,    -1,    -1,    31,    -1,    -1,    -1,    35,
      36,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    71,    72,    -1,    -1,     1,    76,    77,    -1,    -1,
      80,    -1,    -1,    83,    84,    85,    12,    13,    -1,    15,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    -1,    69,    70,    71,    -1,    -1,    -1,    75,
      76,    77,     1,    -1,    80,    -1,     5,    83,    84,    85,
      -1,    -1,    -1,    12,    13,    -1,    15,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    71,    72,    73,    -1,     1,    -1,    77,    -1,
       5,    80,    -1,    -1,    -1,    84,    85,    12,    13,    -1,
      15,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,     5,    69,    70,    -1,    -1,    -1,    -1,
      12,    13,    77,    15,    16,    80,    -1,    -1,    -1,    84,
      85,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    -1,    69,    70,    12,
      13,    -1,    15,    16,    -1,    77,    -1,    -1,    80,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    -1,    69,    70,    -1,    -1,
       0,     1,    -1,    -1,    77,    -1,    -1,    80,    -1,    -1,
      -1,    84,    85,    13,    -1,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    -1,    27,    -1,    -1,
      -1,    31,    -1,    -1,    -1,    35,    36,    -1,    -1,    39,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    28,    29,    30,    32,    33,    34,    38,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    71,    71,    71,
      71,     5,    71,    71,     0,    95,    95,    95,    95,    95,
      95,    95,     5,    26,    97,    99,    95,    95,    95,    95,
      95,    95,    95,     1,    95,    96,    13,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    27,    31,
      35,    36,   101,   106,   107,   113,   125,   127,   133,   134,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     149,   152,     1,    13,    40,    70,   153,   154,   155,   156,
     162,    13,    63,   130,   131,   132,    12,    13,    15,    16,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    69,    77,    80,    84,    85,   137,
     158,   160,   161,   162,   163,   164,    41,    42,    74,   118,
     119,   121,   122,   139,     1,    13,   109,   110,   111,   112,
      71,    73,   168,    12,     5,     6,   100,    21,    13,    17,
      67,    68,    13,    95,    21,     1,    95,     1,    95,     1,
      95,     1,    95,    95,    95,    95,    95,     1,    71,    76,
     143,     1,     5,    77,    78,    79,   135,    13,    20,   144,
     145,   145,     1,   146,    73,   168,   169,    95,    95,    72,
       1,   155,   169,    73,   169,     1,    17,   168,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    62,   161,     1,    72,    76,    83,   159,   169,
      95,    95,    95,    95,    72,    72,    73,   169,    95,    72,
       1,   111,    73,   169,     1,    17,    95,     1,    95,     1,
      39,   101,   102,   166,   167,    13,    95,    95,    13,    20,
     142,   148,    73,   168,    12,    66,   115,    73,   168,    71,
     122,   124,    73,   168,    71,    73,   168,    13,   114,    12,
      13,   128,   123,   124,    37,   108,    72,    95,    95,    95,
      95,    95,   143,   145,    95,     1,    73,   169,    73,     1,
      95,    73,    95,    95,    72,    71,    76,     1,   158,    95,
      95,    95,   160,     1,    13,   116,    43,    13,   126,    95,
       1,    73,   169,    73,    95,    73,    95,    71,     0,    72,
      98,   169,    73,   168,     1,    96,    13,    14,   136,   137,
     143,    95,    13,   148,     7,     8,     9,    10,    11,    81,
      82,   150,    95,    95,     1,    71,    76,    95,    95,   115,
      95,    71,     5,    71,   153,   141,    73,   168,    95,     1,
      95,    95,     1,   158,    95,    95,    75,    95,    95,    95,
      95,    43,   120,    73,    95,     1,    95,    95,     1,   158,
      95,     1,    73,   168,    95,    95,    65,    95,    81,   150,
      95,   123,   103,    95,    95,   153,    95,    71,    95,   124,
      95,    98,    95,    73,   158,    40,   157,   153,   132,    95,
      17,   117,   118,    95,    95,    73,   158,   153,    98,    75,
      75,    95,    75,    95,    12,    13,   151,    73,   168,     1,
      98,   104,   105,   106,   107,   113,   125,   127,   133,   134,
     138,   165,   166,   167,   103,   122,    72,    73,   168,    95,
     109,   109,    95,   157,    95,     1,    72,    95,    75,   119,
      95,    72,    75,   151,    95,    98,    96,   165,    95,   129,
      72,    72,   158,    95,    95,    95,    81,    72,   130,    95,
      81,    95
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
        case 10:

/* Line 1455 of yacc.c  */
#line 280 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 286 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframeRule);
    ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 292 "css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 298 "css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 304 "css/CSSGrammar.y"
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

  case 15:

/* Line 1455 of yacc.c  */
#line 318 "css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 325 "css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 347 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 357 "css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyval.rule));
  ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 363 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 365 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 371 "css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 390 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_hadSyntacticallyValidCSSRule = true;
    ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 398 "css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 399 "css/CSSGrammar.y"
    {
      (yyval.ruleList) = (yyvsp[(1) - (3)].ruleList);
      if ((yyvsp[(2) - (3)].rule)) {
          if (!(yyval.ruleList))
              (yyval.ruleList) = static_cast<CSSParser*>(parser)->createRuleList();
          (yyval.ruleList)->append((yyvsp[(2) - (3)].rule));
      }
  ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 428 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 431 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 434 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 437 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 443 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), true);
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 447 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), false);
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 453 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 457 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(3) - (3)].mediaList);
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 463 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 466 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ((yyvsp[(2) - (2)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 471 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 474 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 477 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 480 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 486 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 489 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 492 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 495 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 498 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 503 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 506 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 512 "css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariable((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].valueList));
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 516 "css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariableDeclarationBlock((yyvsp[(1) - (7)].string));
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 520 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 524 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 528 "css/CSSGrammar.y"
    {
        /* @variables { varname: } Just reduce away this variable with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 533 "css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: @variables { varname: *; }, just discard the property/value pair */
        (yyval.boolean) = false;
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 540 "css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 546 "css/CSSGrammar.y"
    {
    static_cast<CSSParser*>(parser)->addNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
    (yyval.rule) = 0;
;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 550 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 553 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 556 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 562 "css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 563 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 572 "css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 578 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 581 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 587 "css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 594 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 599 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 606 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 609 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 615 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 618 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 621 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 627 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 632 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 640 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 647 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 652 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 657 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 663 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 666 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 672 "css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 678 "css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 690 "css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 691 "css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframeRule))
            (yyval.keyframesRule)->append((yyvsp[(2) - (3)].keyframeRule));
    ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 699 "css/CSSGrammar.y"
    {
        (yyval.keyframeRule) = static_cast<CSSParser*>(parser)->createKeyframeRule((yyvsp[(1) - (6)].valueList));
    ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 705 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 710 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 719 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 720 "css/CSSGrammar.y"
    {
        (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER;
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        if (equalIgnoringCase("from", str.characters, str.length))
            (yyval.value).fValue = 0;
        else if (equalIgnoringCase("to", str.characters, str.length))
            (yyval.value).fValue = 100;
        else
            YYERROR;
    ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 744 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 747 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 754 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 757 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 760 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 766 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 767 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 768 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 772 "css/CSSGrammar.y"
    { (yyval.integer) = (yyvsp[(1) - (1)].integer); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 773 "css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 777 "css/CSSGrammar.y"
    { (yyval.integer) = -1; ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 778 "css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 782 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selectorList));
    ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 788 "css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (1)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = p->reusableSelectorVector();
            deleteAllValues(*(yyval.selectorList));
            (yyval.selectorList)->shrink(0);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(1) - (1)].selector)));
            p->updateLastSelectorLine();
        }
    ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 798 "css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
            p->updateLastSelectorLine();
        } else
            (yyval.selectorList) = 0;
    ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 807 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 813 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 819 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 823 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 827 "css/CSSGrammar.y"
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

  case 142:

/* Line 1455 of yacc.c  */
#line 842 "css/CSSGrammar.y"
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

  case 143:

/* Line 1455 of yacc.c  */
#line 862 "css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 868 "css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 869 "css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 870 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 874 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace);
    ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 879 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 886 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->m_defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->m_defaultNamespace);
    ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 892 "css/CSSGrammar.y"
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

  case 151:

/* Line 1455 of yacc.c  */
#line 902 "css/CSSGrammar.y"
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

  case 152:

/* Line 1455 of yacc.c  */
#line 914 "css/CSSGrammar.y"
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

  case 153:

/* Line 1455 of yacc.c  */
#line 927 "css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 935 "css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 943 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 946 "css/CSSGrammar.y"
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

  case 157:

/* Line 1455 of yacc.c  */
#line 959 "css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 965 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->m_strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
    ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 973 "css/CSSGrammar.y"
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

  case 163:

/* Line 1455 of yacc.c  */
#line 991 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->m_strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
    ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1002 "css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1013 "css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1018 "css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].integer);
        (yyval.selector)->m_value = (yyvsp[(6) - (8)].string);
    ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1024 "css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1032 "css/CSSGrammar.y"
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

  case 169:

/* Line 1455 of yacc.c  */
#line 1044 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1047 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1050 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1053 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1056 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1059 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1070 "css/CSSGrammar.y"
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
        } else if (type == CSSSelector::PseudoBefore ||
                   type == CSSSelector::PseudoAfter) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesBeforeAfterRules(true);
        }
    ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1100 "css/CSSGrammar.y"
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
        } else if (type == CSSSelector::PseudoBefore ||
                   type == CSSSelector::PseudoAfter) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (Document* doc = p->document())
                doc->setUsesBeforeAfterRules(true);
        }
    ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1120 "css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        (yyval.selector)->m_value = (yyvsp[(2) - (6)].string);
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

  case 180:

/* Line 1455 of yacc.c  */
#line 1138 "css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument(String::number((yyvsp[(4) - (7)].integer) * (yyvsp[(5) - (7)].number)));
        (yyval.selector)->m_value = (yyvsp[(2) - (7)].string);
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

  case 181:

/* Line 1455 of yacc.c  */
#line 1156 "css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument((yyvsp[(4) - (6)].string));
        (yyvsp[(2) - (6)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (6)].string);
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
#line 1175 "css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector) || (yyvsp[(4) - (6)].selector)->simpleSelector() || (yyvsp[(4) - (6)].selector)->tagHistory() || (yyvsp[(4) - (6)].selector)->matchesPseudoElement())
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

  case 183:

/* Line 1455 of yacc.c  */
#line 1190 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1193 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1198 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1201 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1204 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1207 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1210 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1216 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1219 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1222 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1225 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1228 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1231 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1236 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1239 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1245 "css/CSSGrammar.y"
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

  case 199:

/* Line 1455 of yacc.c  */
#line 1259 "css/CSSGrammar.y"
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

  case 200:

/* Line 1455 of yacc.c  */
#line 1271 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1275 "css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.boolean) = false;
    ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1282 "css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        (yyval.boolean) = false;
    ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1287 "css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1292 "css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1297 "css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        (yyval.boolean) = false;
    ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1302 "css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1309 "css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1315 "css/CSSGrammar.y"
    { (yyval.boolean) = true; ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1316 "css/CSSGrammar.y"
    { (yyval.boolean) = false; ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1320 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1325 "css/CSSGrammar.y"
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

  case 212:

/* Line 1455 of yacc.c  */
#line 1339 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1342 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1345 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1351 "css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1354 "css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1357 "css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1363 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1364 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1365 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1366 "css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1372 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1373 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 1374 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 1375 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 1376 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 1377 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 1379 "css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 1382 "css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (2)].value);
  ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 1385 "css/CSSGrammar.y"
    { /* Handle width: %; */
      (yyval.value).id = 0; (yyval.value).unit = 0;
  ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 1391 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 1392 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 1393 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 1394 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 1395 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 1396 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 1397 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 1398 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 1399 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 1400 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 1401 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 1402 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 1403 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 1404 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 1405 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 1406 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 1407 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 1408 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 1409 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; ;}
    break;

  case 250:

/* Line 1455 of yacc.c  */
#line 1410 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 251:

/* Line 1455 of yacc.c  */
#line 1411 "css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).fValue = (yyvsp[(1) - (2)].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      CSSParser* p = static_cast<CSSParser*>(parser);
      if (Document* doc = p->document())
          doc->setUsesRemUnits(true);
  ;}
    break;

  case 252:

/* Line 1455 of yacc.c  */
#line 1422 "css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).string = (yyvsp[(1) - (1)].string);
      (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_VARIABLE_FUNCTION_SYNTAX;
  ;}
    break;

  case 253:

/* Line 1455 of yacc.c  */
#line 1430 "css/CSSGrammar.y"
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

  case 254:

/* Line 1455 of yacc.c  */
#line 1439 "css/CSSGrammar.y"
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

  case 255:

/* Line 1455 of yacc.c  */
#line 1455 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 256:

/* Line 1455 of yacc.c  */
#line 1456 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 257:

/* Line 1455 of yacc.c  */
#line 1463 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 258:

/* Line 1455 of yacc.c  */
#line 1466 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 259:

/* Line 1455 of yacc.c  */
#line 1472 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 260:

/* Line 1455 of yacc.c  */
#line 1475 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 261:

/* Line 1455 of yacc.c  */
#line 1481 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 262:

/* Line 1455 of yacc.c  */
#line 1499 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    ;}
    break;

  case 263:

/* Line 1455 of yacc.c  */
#line 1502 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 4347 "/WebCore/generated/CSSGrammar.tab.c"
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
#line 1512 "css/CSSGrammar.y"


