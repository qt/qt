/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse cssyyparse
#define yylex   cssyylex
#define yyerror cssyyerror
#define yylval  cssyylval
#define yychar  cssyychar
#define yydebug cssyydebug
#define yynerrs cssyynerrs


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
/* Tokens.  */
#define TOKEN_EOF 0
#define LOWEST_PREC 258
#define UNIMPORTANT_TOK 259
#define WHITESPACE 260
#define SGML_CD 261
#define INCLUDES 262
#define DASHMATCH 263
#define BEGINSWITH 264
#define ENDSWITH 265
#define CONTAINS 266
#define STRING 267
#define IDENT 268
#define NTH 269
#define HEX 270
#define IDSEL 271
#define IMPORT_SYM 272
#define PAGE_SYM 273
#define MEDIA_SYM 274
#define FONT_FACE_SYM 275
#define CHARSET_SYM 276
#define NAMESPACE_SYM 277
#define WEBKIT_RULE_SYM 278
#define WEBKIT_DECLS_SYM 279
#define WEBKIT_KEYFRAME_RULE_SYM 280
#define WEBKIT_KEYFRAMES_SYM 281
#define WEBKIT_VALUE_SYM 282
#define WEBKIT_MEDIAQUERY_SYM 283
#define WEBKIT_SELECTOR_SYM 284
#define WEBKIT_VARIABLES_SYM 285
#define WEBKIT_DEFINE_SYM 286
#define VARIABLES_FOR 287
#define WEBKIT_VARIABLES_DECLS_SYM 288
#define ATKEYWORD 289
#define IMPORTANT_SYM 290
#define MEDIA_ONLY 291
#define MEDIA_NOT 292
#define MEDIA_AND 293
#define REMS 294
#define QEMS 295
#define EMS 296
#define EXS 297
#define PXS 298
#define CMS 299
#define MMS 300
#define INS 301
#define PTS 302
#define PCS 303
#define DEGS 304
#define RADS 305
#define GRADS 306
#define TURNS 307
#define MSECS 308
#define SECS 309
#define HERZ 310
#define KHERZ 311
#define DIMEN 312
#define PERCENTAGE 313
#define FLOATTOKEN 314
#define INTEGER 315
#define URI 316
#define FUNCTION 317
#define NOTFUNCTION 318
#define UNICODERANGE 319
#define VARCALL 320




/* Copy the first part of user declarations.  */
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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 62 "css/CSSGrammar.y"
{
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
}
/* Line 187 of yacc.c.  */
#line 319 "generated/CSSGrammar.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */
#line 86 "css/CSSGrammar.y"


static inline int cssyyerror(const char*)
{
    return 1;
}

static int cssyylex(YYSTYPE* yylval, void* parser)
{
    return static_cast<CSSParser*>(parser)->lex(yylval);
}



/* Line 216 of yacc.c.  */
#line 345 "generated/CSSGrammar.tab.c"

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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
  yytype_int16 yyss;
  YYSTYPE yyvs;
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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  24
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1435

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  83
/* YYNRULES -- Number of rules.  */
#define YYNRULES  263
/* YYNRULES -- Number of states.  */
#define YYNSTATES  510

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
     461,   463,   465,   471,   473,   478,   481,   484,   486,   488,
     491,   495,   498,   500,   503,   506,   508,   511,   513,   516,
     520,   523,   525,   527,   529,   532,   535,   537,   539,   541,
     543,   545,   548,   551,   556,   565,   571,   581,   583,   585,
     587,   589,   591,   593,   595,   597,   600,   604,   611,   618,
     625,   632,   634,   637,   639,   643,   645,   648,   651,   655,
     659,   664,   668,   674,   679,   684,   691,   697,   700,   703,
     710,   717,   720,   724,   729,   732,   735,   738,   739,   741,
     745,   748,   752,   755,   758,   761,   762,   764,   767,   770,
     773,   776,   780,   783,   786,   788,   791,   793,   796,   799,
     802,   805,   808,   811,   814,   817,   820,   823,   826,   829,
     832,   835,   838,   841,   844,   847,   850,   853,   856,   859,
     862,   864,   870,   874,   877,   880,   882,   885,   889,   893,
     896,   902,   906,   908
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      87,     0,    -1,    95,    97,    96,   100,    -1,    88,    95,
      -1,    90,    95,    -1,    92,    95,    -1,    93,    95,    -1,
      94,    95,    -1,    91,    95,    -1,    89,    95,    -1,    28,
      71,    95,   101,    95,    72,    -1,    30,    71,    95,   130,
      95,    72,    -1,    29,    71,    95,   152,    72,    -1,    38,
      71,    95,   109,    72,    -1,    32,    71,    95,   157,    72,
      -1,    33,     5,    95,   122,    72,    -1,    34,    71,    95,
     138,    72,    -1,    -1,    95,     5,    -1,    -1,    96,     6,
      -1,    96,     5,    -1,    -1,    99,    -1,    72,    -1,     0,
      -1,    26,    95,    12,    95,    73,    -1,    26,     1,   167,
      -1,    26,     1,    73,    -1,    -1,   100,   102,    96,    -1,
     137,    -1,   125,    -1,   133,    -1,   134,    -1,   127,    -1,
     113,    -1,   106,    -1,   107,    -1,   101,    -1,   166,    -1,
     165,    -1,    -1,   103,   105,    96,    -1,   137,    -1,   133,
      -1,   134,    -1,   127,    -1,   104,    -1,   166,    -1,   165,
      -1,   113,    -1,   106,    -1,   107,    -1,   125,    -1,    22,
      95,   115,    95,   123,    73,    -1,    22,    95,   115,    95,
     123,   167,    -1,    22,     1,    73,    -1,    22,     1,   167,
      -1,    35,    95,   123,    71,    95,   109,    72,    -1,    36,
      95,   108,    71,    95,   109,    72,    -1,    -1,    37,     5,
     124,    -1,   111,    -1,   110,   111,    -1,   110,    -1,     1,
     168,     1,    -1,     1,    -1,   110,     1,    -1,   111,    73,
      95,    -1,   111,   168,    73,    95,    -1,     1,    73,    95,
      -1,     1,   168,     1,    73,    95,    -1,   110,   111,    73,
      95,    -1,   110,     1,    73,    95,    -1,   110,     1,   168,
       1,    73,    95,    -1,   112,    17,    95,   157,    -1,   112,
      95,    71,    95,   152,    72,    95,    -1,   112,     1,    -1,
     112,    17,    95,     1,   157,    -1,   112,    17,    95,    -1,
     112,    17,    95,     1,    -1,    13,    95,    -1,    27,    95,
     114,   115,    95,    73,    -1,    27,    95,   114,   115,    95,
     167,    -1,    27,     1,   167,    -1,    27,     1,    73,    -1,
      -1,    13,    95,    -1,    12,    -1,    66,    -1,    13,    95,
      -1,    -1,    17,    95,   157,    95,    -1,    74,    95,   116,
      95,   117,    75,    95,    -1,   118,    -1,   119,    95,    43,
      95,   118,    -1,    -1,    43,    95,   119,    -1,    -1,    41,
      -1,    42,    -1,   119,    -1,   121,    95,   126,   120,    -1,
      -1,   124,    -1,   122,    -1,   124,    76,    95,   122,    -1,
     124,     1,    -1,    24,    95,   124,    71,    95,   103,   164,
      -1,    24,    95,    71,    95,   103,   164,    -1,    13,    95,
      -1,    31,    95,   128,    95,    71,    95,   129,    72,    -1,
      13,    -1,    12,    -1,    -1,   129,   130,    95,    -1,   131,
      95,    71,    95,   152,    72,    -1,   132,    -1,   131,    95,
      76,    95,   132,    -1,    63,    -1,    13,    -1,    23,     1,
     167,    -1,    23,     1,    73,    -1,    25,    95,    71,    95,
     152,    72,    95,    -1,    25,     1,   167,    -1,    25,     1,
      73,    -1,    77,    95,    -1,    78,    95,    -1,    79,    95,
      -1,    80,    -1,    77,    -1,   138,    71,    95,   152,    98,
      -1,   140,    -1,   138,    76,    95,   140,    -1,   138,     1,
      -1,   140,     5,    -1,   142,    -1,   139,    -1,   139,   142,
      -1,   140,   135,   142,    -1,   140,     1,    -1,    21,    -1,
      20,    21,    -1,    13,    21,    -1,   143,    -1,   143,   144,
      -1,   144,    -1,   141,   143,    -1,   141,   143,   144,    -1,
     141,   144,    -1,    13,    -1,    20,    -1,   145,    -1,   144,
     145,    -1,   144,     1,    -1,    16,    -1,    15,    -1,   146,
      -1,   148,    -1,   151,    -1,    18,    13,    -1,    13,    95,
      -1,    19,    95,   147,    81,    -1,    19,    95,   147,   149,
      95,   150,    95,    81,    -1,    19,    95,   141,   147,    81,
      -1,    19,    95,   141,   147,   149,    95,   150,    95,    81,
      -1,    82,    -1,     7,    -1,     8,    -1,     9,    -1,    10,
      -1,    11,    -1,    13,    -1,    12,    -1,    17,    13,    -1,
      17,    17,    13,    -1,    17,    67,    95,    14,    95,    75,
      -1,    17,    67,    95,    65,    95,    75,    -1,    17,    67,
      95,    13,    95,    75,    -1,    17,    68,    95,   142,    95,
      75,    -1,   154,    -1,   153,   154,    -1,   153,    -1,     1,
     168,     1,    -1,     1,    -1,   153,     1,    -1,   153,   168,
      -1,   154,    73,    95,    -1,   154,   168,    95,    -1,   154,
     168,    73,    95,    -1,     1,    73,    95,    -1,     1,   168,
       1,    73,    95,    -1,   153,   154,    73,    95,    -1,   153,
       1,    73,    95,    -1,   153,     1,   168,     1,    73,    95,
      -1,   155,    17,    95,   157,   156,    -1,   161,    95,    -1,
     155,     1,    -1,   155,    17,    95,     1,   157,   156,    -1,
     155,    17,    95,   157,   156,     1,    -1,    40,    95,    -1,
     155,    17,    95,    -1,   155,    17,    95,     1,    -1,   155,
     167,    -1,    13,    95,    -1,    40,    95,    -1,    -1,   159,
      -1,   157,   158,   159,    -1,   157,   168,    -1,   157,   168,
       1,    -1,   157,     1,    -1,    83,    95,    -1,    76,    95,
      -1,    -1,   160,    -1,   136,   160,    -1,    12,    95,    -1,
      13,    95,    -1,    62,    95,    -1,   136,    62,    95,    -1,
      66,    95,    -1,    69,    95,    -1,   163,    -1,    84,    95,
      -1,   162,    -1,   161,    95,    -1,    85,    95,    -1,    65,
      95,    -1,    64,    95,    -1,    63,    95,    -1,    48,    95,
      -1,    49,    95,    -1,    50,    95,    -1,    51,    95,    -1,
      52,    95,    -1,    53,    95,    -1,    54,    95,    -1,    55,
      95,    -1,    56,    95,    -1,    57,    95,    -1,    58,    95,
      -1,    59,    95,    -1,    60,    95,    -1,    61,    95,    -1,
      46,    95,    -1,    45,    95,    -1,    47,    95,    -1,    44,
      95,    -1,    70,    -1,    67,    95,   157,    75,    95,    -1,
      67,    95,     1,    -1,    15,    95,    -1,    16,    95,    -1,
      98,    -1,     1,    98,    -1,    39,     1,   167,    -1,    39,
       1,    73,    -1,     1,   167,    -1,    71,     1,   168,     1,
      98,    -1,    71,     1,    98,    -1,   167,    -1,   168,     1,
     167,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   268,   268,   269,   270,   271,   272,   273,   274,   275,
     279,   285,   291,   297,   303,   317,   324,   334,   335,   338,
     340,   341,   344,   346,   351,   352,   356,   362,   364,   368,
     370,   378,   379,   380,   381,   382,   383,   384,   385,   389,
     392,   393,   397,   398,   409,   410,   411,   412,   416,   417,
     418,   419,   420,   421,   422,   427,   430,   433,   436,   442,
     446,   452,   456,   462,   465,   470,   473,   476,   479,   485,
     488,   491,   494,   497,   502,   505,   511,   515,   519,   523,
     527,   532,   539,   545,   549,   552,   555,   561,   562,   566,
     567,   571,   577,   580,   586,   593,   598,   605,   608,   614,
     617,   620,   626,   631,   639,   642,   646,   651,   656,   662,
     665,   671,   677,   684,   685,   689,   690,   698,   704,   709,
     718,   719,   743,   746,   752,   756,   759,   765,   766,   767,
     771,   772,   776,   782,   791,   799,   805,   811,   814,   818,
     834,   854,   860,   861,   862,   866,   871,   878,   884,   894,
     906,   919,   927,   935,   938,   951,   957,   965,   977,   978,
     979,   983,   994,  1005,  1010,  1016,  1024,  1036,  1039,  1042,
    1045,  1048,  1051,  1057,  1058,  1062,  1092,  1112,  1130,  1148,
    1167,  1182,  1185,  1190,  1193,  1196,  1199,  1202,  1208,  1211,
    1214,  1217,  1220,  1223,  1228,  1231,  1237,  1251,  1263,  1267,
    1274,  1279,  1284,  1289,  1294,  1301,  1307,  1308,  1312,  1317,
    1331,  1334,  1337,  1343,  1346,  1349,  1355,  1356,  1357,  1358,
    1364,  1365,  1366,  1367,  1368,  1369,  1371,  1374,  1377,  1383,
    1384,  1385,  1386,  1387,  1388,  1389,  1390,  1391,  1392,  1393,
    1394,  1395,  1396,  1397,  1398,  1399,  1400,  1401,  1402,  1403,
    1414,  1422,  1431,  1447,  1448,  1455,  1458,  1464,  1467,  1473,
    1491,  1494,  1500,  1501
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
  "font_face", "combinator", "unary_operator", "ruleset", "selector_list",
  "selector_with_trailing_whitespace", "selector", "namespace_selector",
  "simple_selector", "element_name", "specifier_list", "specifier",
  "class", "attr_name", "attrib", "match", "ident_or_string", "pseudo",
  "declaration_list", "decl_list", "declaration", "property", "prio",
  "expr", "operator", "term", "unary_term", "variable_reference",
  "function", "hexcolor", "save_block", "invalid_at", "invalid_rule",
  "invalid_block", "invalid_block_list", 0
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
     136,   136,   137,   138,   138,   138,   139,   140,   140,   140,
     140,   140,   141,   141,   141,   142,   142,   142,   142,   142,
     142,   143,   143,   144,   144,   144,   145,   145,   145,   145,
     145,   146,   147,   148,   148,   148,   148,   149,   149,   149,
     149,   149,   149,   150,   150,   151,   151,   151,   151,   151,
     151,   152,   152,   152,   152,   152,   152,   152,   153,   153,
     153,   153,   153,   153,   153,   153,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   155,   156,   156,   157,   157,
     157,   157,   157,   158,   158,   158,   159,   159,   159,   159,
     159,   159,   159,   159,   159,   159,   159,   159,   159,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     160,   160,   160,   160,   160,   160,   160,   160,   160,   160,
     161,   162,   162,   163,   163,   164,   164,   165,   165,   166,
     167,   167,   168,   168
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
       1,     1,     5,     1,     4,     2,     2,     1,     1,     2,
       3,     2,     1,     2,     2,     1,     2,     1,     2,     3,
       2,     1,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     2,     2,     4,     8,     5,     9,     1,     1,     1,
       1,     1,     1,     1,     1,     2,     3,     6,     6,     6,
       6,     1,     2,     1,     3,     1,     2,     2,     3,     3,
       4,     3,     5,     4,     4,     6,     5,     2,     2,     6,
       6,     2,     3,     4,     2,     2,     2,     0,     1,     3,
       2,     3,     2,     2,     2,     0,     1,     2,     2,     2,
       2,     3,     2,     2,     1,     2,     1,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       1,     5,     3,     2,     2,     1,     2,     3,     3,     2,
       5,     3,     1,     3
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
      99,     0,     0,     0,     0,    29,   151,   157,   156,     0,
       0,    17,   152,   142,     0,     0,    17,     0,     0,    17,
      17,    17,    17,    37,    38,    36,    32,    35,    33,    34,
      31,     0,   138,     0,     0,   137,   145,     0,   153,   158,
     159,   160,   185,    17,    17,   250,     0,     0,   181,     0,
      17,   121,   120,    17,    17,   118,    17,    17,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17,    17,    17,    17,    17,    17,
      17,    17,    17,    17,    17,   131,   130,    17,    17,     0,
       0,   208,   216,    17,   226,   224,   100,   101,    17,    95,
     102,    17,     0,     0,    67,    17,     0,     0,    63,     0,
       0,    28,    27,    17,    21,    20,     0,   144,   175,     0,
      17,    17,   161,     0,   143,     0,     0,     0,    99,     0,
       0,     0,    87,     0,    99,    61,     0,   135,    17,    17,
     139,   141,   136,    17,    17,    17,     0,   151,   152,   148,
       0,     0,   155,   154,    17,   262,     0,   205,   201,    12,
     186,   182,     0,    17,     0,   198,    17,   204,   197,     0,
       0,   218,   219,   253,   254,   249,   247,   246,   248,   232,
     233,   234,   235,   236,   237,   238,   239,   240,   241,   242,
     243,   244,   245,   220,   231,   230,   229,   222,     0,   223,
     225,   228,    17,   217,   212,    14,    17,    17,     0,     0,
     227,     0,     0,     0,    15,    16,    17,     0,    82,    13,
      68,    64,    17,     0,    78,    17,     0,     0,     0,     0,
       0,    39,    19,    41,    40,   176,     0,     0,    17,     0,
       0,     0,    57,    58,    89,    90,    17,   123,   122,    17,
     106,     0,   126,   125,    17,    86,    85,    17,     0,   114,
     113,    17,     0,     0,     0,     0,    10,     0,     0,   127,
     128,   129,   140,     0,   191,   184,    17,     0,    17,     0,
     188,    17,   189,     0,    11,    17,    17,   252,     0,   221,
     214,   213,   209,   211,    17,    17,    17,    17,    97,    71,
      66,    17,     0,    17,    69,    17,     0,    17,    25,    24,
     261,     0,    26,   259,     0,    30,    17,    17,    17,    17,
     162,    17,     0,   168,   169,   170,   171,   172,   163,   167,
      17,   104,    42,   108,    17,    17,     0,    88,    17,     0,
      17,    99,    17,     0,     0,    17,   263,   194,     0,   193,
     190,   203,     0,     0,     0,    17,    91,    92,     0,   111,
      17,   103,    17,    74,     0,    73,    70,    81,     0,     0,
       0,   258,   257,     0,     0,     0,     0,   165,    17,     0,
       0,     0,    42,    99,     0,     0,    17,     0,     0,     0,
     132,   192,    17,     0,    17,     0,     0,   119,   251,    17,
       0,    96,     0,    72,    17,     0,     0,   260,   179,   177,
     178,   180,     0,   174,   173,    17,    55,    56,     0,   255,
      48,    19,    52,    53,    51,    54,    47,    45,    46,    44,
     110,    50,    49,     0,   107,    17,    83,    84,   115,     0,
       0,   195,   199,   206,   200,   117,     0,    17,    98,    75,
      17,    17,     0,   256,    43,   109,   124,     0,    59,    60,
       0,    94,    77,     0,   164,   112,    17,    93,   166,   116
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     8,     9,    10,    11,    12,    13,    14,    15,   252,
      45,    34,   459,    35,   156,    62,   272,   421,   460,   461,
      63,    64,   305,   146,   147,   148,   149,    65,   298,   286,
     335,   440,   139,   140,   401,   141,   290,   302,   303,    66,
     338,    67,   301,   497,    93,    94,    95,    68,    69,   186,
     129,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,   281,    80,   370,   455,    81,    86,    87,    88,    89,
     435,   130,   248,   131,   132,   133,   134,   135,   470,   471,
     472,   195,   249
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -270
static const yytype_int16 yypact[] =
{
     497,   -23,   100,   124,   129,   126,   142,   148,   216,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,    27,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,   267,   267,   267,   267,   267,
     267,   267,  -270,   469,  -270,  -270,   947,   309,    23,  1231,
     217,   574,    37,   -24,    76,   327,   224,  -270,  -270,   379,
     287,  -270,   283,  -270,   428,   306,  -270,    84,   386,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,    66,   659,   337,   589,  -270,   646,   149,  -270,  -270,
    -270,  -270,   109,  -270,  -270,  -270,   245,   350,   275,   231,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,   765,
     943,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
      25,  -270,   272,    86,   295,  -270,   322,   237,   300,   291,
     366,  -270,  -270,  -270,  -270,  -270,   722,  -270,  -270,   398,
    -270,  -270,  -270,   285,  -270,   385,   371,   400,   298,   430,
     249,   446,    39,   472,   383,    36,   194,  -270,  -270,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,   659,  -270,  -270,   646,
     158,   252,  -270,  -270,  -270,  -270,   411,   267,   267,  -270,
     610,   355,     7,  -270,   598,  -270,  -270,  -270,   267,   280,
     141,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,   267,   267,
     267,   267,   267,   267,   267,   267,   267,   267,  1168,   267,
     267,   267,  -270,  -270,  -270,  -270,  -270,  -270,  1290,   492,
     267,    58,    26,    61,  -270,  -270,  -270,   434,   267,  -270,
     611,   378,  -270,    63,  -270,  -270,   282,     5,    19,   390,
     467,  -270,  -270,  -270,  -270,  -270,   250,   574,   224,   283,
     459,   179,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,   134,  -270,  -270,  -270,  -270,  -270,  -270,   145,  -270,
    -270,  -270,   405,   125,   475,   415,  -270,   309,   574,   267,
     267,   267,  -270,   388,   267,   614,  -270,   488,  -270,   390,
     267,  -270,   267,   644,  -270,  -270,  -270,  -270,  1017,   267,
     267,   267,  -270,   390,  -270,  -270,  -270,  -270,   455,   267,
     647,  -270,   498,  -270,   267,  -270,  1094,  -270,  -270,  -270,
    -270,   501,  -270,  -270,   654,   327,  -270,  -270,  -270,  -270,
     267,  -270,   273,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,   437,   267,  -270,  -270,  -270,   309,   267,  -270,   294,
    -270,     4,  -270,    72,   444,  -270,  -270,   267,   677,   267,
     267,  1290,   570,   309,    23,  -270,   267,    38,    60,   267,
    -270,  -270,  -270,   267,   681,   267,   267,  1290,   718,   309,
       5,  -270,  -270,   127,   176,   218,   244,  -270,  -270,   568,
     726,  1363,   267,   217,   438,   203,  -270,    37,   168,    37,
    -270,   267,  -270,   570,  -270,   240,   441,  -270,   267,  -270,
     439,  -270,    60,   267,  -270,   792,   452,  -270,  -270,  -270,
    -270,  -270,   568,  -270,  -270,  -270,  -270,  -270,     5,  -270,
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,
    -270,  -270,  -270,  1363,  -270,  -270,  -270,  -270,   267,   462,
     502,   267,  -270,   267,  -270,  -270,  1231,  -270,    25,   267,
    -270,  -270,    -1,  -270,   327,  -270,   267,   347,  -270,  -270,
     866,   267,   267,     1,  -270,  -270,  -270,   267,  -270,   267
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,  -270,     0,
    -269,  -270,  -240,  -270,  -270,   353,  -270,   138,  -270,  -270,
    -230,   -35,  -270,   404,  -270,   431,  -270,   -20,  -270,   286,
    -270,  -270,   190,   154,  -270,  -270,   -39,   226,  -166,    -4,
    -270,     9,  -270,  -270,   103,  -270,   207,    10,    15,  -270,
    -270,    22,   571,  -270,   305,   485,   -46,   577,   -37,   -61,
    -270,   372,  -270,   296,   214,  -270,  -237,  -270,   580,  -270,
     253,  -198,  -270,   425,   583,   -12,  -270,  -270,   259,   595,
     599,   108,   -53
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -216
static const yytype_int16 yytable[] =
{
      16,   142,   291,   355,    32,   348,    32,  -187,   319,    25,
      26,    27,    28,    29,    30,    31,   193,    36,    37,    38,
      39,    40,    41,    42,    32,    90,   180,   350,    32,   196,
     -17,    32,    32,    44,   202,   204,    91,   190,   144,   191,
     328,    32,    32,    32,    32,   136,   137,   150,    17,   151,
     145,   163,   297,    33,   166,   439,   168,   170,   172,   173,
     174,   175,   176,    32,   319,    32,    32,   177,   -17,   336,
     383,   334,   348,   304,   337,    90,   150,   349,   138,  -187,
     504,    32,   508,   197,   198,   169,    92,   177,   153,   -17,
     208,   257,   352,   209,   210,   263,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   392,   373,   240,   241,   193,
     193,    21,    32,   250,   138,   373,   345,   178,   251,   424,
     312,   253,   179,   430,   349,   258,    32,   317,   408,   266,
     192,   152,   313,   268,  -147,   -17,   436,   284,   255,   192,
     276,   277,   179,  -150,    47,    48,    49,    50,    51,   373,
     447,    18,   446,    47,    48,    49,    50,    51,   307,   308,
     150,    32,   194,   309,   310,   311,   363,   364,   365,   366,
     367,   462,   494,   433,   314,    19,  -105,   207,  -105,    32,
      20,   375,   448,   320,   322,   374,   323,   342,    32,   445,
     375,   285,   325,    22,   351,   428,    24,   326,   493,    23,
    -147,  -147,    32,    32,  -147,  -147,  -147,  -147,  -147,  -150,
    -150,   359,   205,  -150,  -150,  -150,  -150,  -150,   260,   -62,
    -196,   484,   329,   462,   375,   157,   330,   331,   206,    32,
     145,   449,   193,   192,    32,    32,   339,  -146,   136,   137,
     368,   369,   344,   356,   357,   346,   306,    47,    48,    49,
      50,    51,    32,   283,   150,   288,   476,   293,   360,   296,
     363,   364,   365,   366,   367,    32,   371,    32,   500,   372,
      32,   138,   264,   450,   376,    90,   -17,   377,   278,    32,
     162,   379,   150,    32,   164,   279,    53,   167,   265,   -65,
      82,  -196,  -196,  -196,    32,   358,   387,   199,   389,   451,
     294,   390,    83,  -146,  -146,   393,   394,  -146,  -146,  -146,
    -146,  -146,   154,   155,   396,   397,   398,   399,   181,   136,
     137,   403,   182,   405,   254,   406,   150,   409,   203,    84,
    -183,   200,   324,   347,   417,   369,   413,   414,   415,   416,
      91,   360,   -17,    83,    90,   426,   150,   267,   256,   289,
     419,   150,   138,   262,   422,   423,    32,   353,   425,    85,
     427,    90,   429,   284,   474,   431,   463,   171,    32,   192,
      84,   -17,   158,  -149,   259,   438,   159,    90,   -17,   -17,
     442,   464,   443,    47,    48,    49,    50,    51,  -133,  -133,
      92,   275,   315,  -133,   183,   184,   185,   465,   452,   505,
      85,   150,  -183,   386,   136,   137,   478,   386,   318,   165,
     466,   467,   481,   -17,   483,   340,   468,   285,   463,   486,
     -17,   386,    32,   469,   489,   181,   160,   161,   386,   182,
     -99,   343,   -17,   464,  -104,   492,   150,   138,   282,  -149,
    -149,   150,   412,  -149,  -149,  -149,  -149,  -149,   354,   465,
      43,   150,   361,   287,   -17,   496,   380,    32,   136,   137,
     381,   -17,   466,   467,   299,   300,   382,   501,   468,   388,
     502,   503,  -210,   333,   -17,   469,   386,  -210,   400,   404,
     507,   150,   410,   292,  -210,  -210,   509,  -210,  -210,   271,
     475,   138,   386,   485,   487,  -134,  -134,   150,   386,   295,
    -134,   183,   184,   185,   490,     1,     2,     3,   457,     4,
       5,     6,  -210,   477,   498,     7,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
    -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,  -210,
     473,  -210,  -210,  -210,  -210,  -210,   353,  -210,  -210,  -210,
    -207,   244,  -210,    32,   499,  -210,  -210,  -210,   261,    32,
     453,   454,  -215,  -215,   378,  -215,  -215,    46,   441,    47,
      48,    49,    50,    51,    52,    53,   488,   420,   -17,   319,
     506,   437,   187,   -17,    47,    48,    49,    50,    51,   188,
     434,   -17,   143,   384,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,   -17,  -215,
    -215,   150,  -207,  -207,  -202,   391,   246,  -215,   280,    32,
    -215,   189,   362,   247,  -215,  -215,    96,    97,   418,    98,
      99,    47,    48,    49,    50,    51,   491,   201,   -17,   -17,
     -17,   321,    46,   332,    47,    48,    49,    50,    51,    52,
      53,   150,   150,   316,   341,   150,   482,   385,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   243,   124,    85,  -202,  -202,  -202,   150,   244,
     402,   125,    -2,   269,   126,   150,     0,   411,   127,   128,
    -215,  -215,   495,  -215,  -215,    46,     0,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,   150,    58,
     432,   273,   150,    59,   444,   274,     0,    60,    61,     0,
       0,   270,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,     0,  -215,  -215,   150,
     -76,   -76,     0,   244,   246,  -215,     0,   150,  -215,   456,
       0,   247,  -215,  -215,  -215,  -215,     0,  -215,  -215,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   242,   119,   120,
     121,   479,     0,   480,     0,     0,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
       0,  -215,  -215,   150,   -79,   -79,     0,   244,   246,  -215,
       0,   -17,  -215,     0,     0,   247,  -215,  -215,  -215,  -215,
       0,  -215,  -215,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,     0,  -215,  -215,   150,     0,     0,
       0,   -17,   246,  -215,   244,     0,  -215,     0,     0,   247,
    -215,  -215,    32,     0,     0,  -215,  -215,     0,  -215,  -215,
      46,     0,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,     0,    58,     0,     0,     0,    59,     0,
       0,     0,    60,    61,     0,     0,     0,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,     0,  -215,  -215,   150,   245,     0,     0,   244,   246,
    -215,     0,     0,  -215,     0,     0,   247,  -215,  -215,  -215,
    -215,     0,  -215,  -215,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,  -215,     0,  -215,  -215,   150,     0,
       0,     0,   395,   246,  -215,   407,     0,  -215,     0,    32,
     247,  -215,  -215,     0,     0,     0,    96,    97,     0,    98,
      99,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,     0,   124,    85,   -80,   -80,   -80,     0,   327,
       0,   125,     0,    32,   126,     0,     0,     0,   127,   128,
      96,    97,     0,    98,    99,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,    32,   124,    85,     0,
       0,     0,     0,    96,    97,   125,    98,    99,   126,     0,
       0,     0,   127,   128,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,     0,
     124,    85,    96,    97,     0,    98,    99,     0,   125,     0,
       0,   126,     0,     0,     0,   127,   128,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,     0,   124,
      85,     0,     0,   348,   458,     0,     0,   125,     0,     0,
     126,     0,     0,     0,   127,   128,    46,     0,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,     0,
      58,     0,     0,     0,    59,     0,     0,     0,    60,    61,
       0,     0,   270,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   349
};

static const yytype_int16 yycheck[] =
{
       0,    40,   168,   272,     5,     0,     5,     0,     1,     9,
      10,    11,    12,    13,    14,    15,    77,    17,    18,    19,
      20,    21,    22,    23,     5,    37,    72,   267,     5,    82,
       5,     5,     5,    33,    87,    88,    13,    74,     1,    76,
     238,     5,     5,     5,     5,    41,    42,    71,    71,    73,
      13,    51,    13,    26,    54,    17,    56,    57,    58,    59,
      60,    61,    62,     5,     1,     5,     5,     1,    43,    43,
     307,    13,     0,    37,    13,    87,    71,    72,    74,    72,
      81,     5,    81,    83,    84,     1,    63,     1,    12,     5,
      90,   144,    73,    93,    94,   148,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   323,     1,   127,   128,   190,
     191,     5,     5,   133,    74,     1,    73,    71,   138,   376,
     186,   141,    76,   383,    72,   145,     5,   200,   346,   149,
       1,    43,   189,   153,     5,    71,   393,    12,    72,     1,
     160,   161,    76,     5,    15,    16,    17,    18,    19,     1,
     410,    71,   409,    15,    16,    17,    18,    19,   178,   179,
      71,     5,    73,   183,   184,   185,     7,     8,     9,    10,
      11,   421,   461,   391,   194,    71,    71,    89,    73,     5,
      71,    76,    75,   203,   204,    71,   206,   260,     5,   407,
      76,    66,    71,    71,   267,   381,     0,    76,   458,    71,
      71,    72,     5,     5,    75,    76,    77,    78,    79,    71,
      72,   277,     1,    75,    76,    77,    78,    79,     1,    71,
       0,     1,   242,   473,    76,    21,   246,   247,    17,     5,
      13,    75,   313,     1,     5,     5,   256,     5,    41,    42,
      81,    82,   262,    13,    14,   265,    72,    15,    16,    17,
      18,    19,     5,   165,    71,   167,    73,   169,   278,   171,
       7,     8,     9,    10,    11,     5,   286,     5,   486,   289,
       5,    74,     1,    75,   294,   307,     5,   297,    13,     5,
      13,   301,    71,     5,    21,    20,    21,     1,    17,    72,
       1,    71,    72,    73,     5,    65,   316,    72,   318,    75,
      71,   321,    13,    71,    72,   325,   326,    75,    76,    77,
      78,    79,     5,     6,   334,   335,   336,   337,     1,    41,
      42,   341,     5,   343,    72,   345,    71,   347,    73,    40,
       0,     1,    72,    71,    81,    82,   356,   357,   358,   359,
      13,   361,    71,    13,   376,    71,    71,     1,    73,    71,
     370,    71,    74,    73,   374,   375,     5,   269,   378,    70,
     380,   393,   382,    12,   423,   385,   421,     1,     5,     1,
      40,     5,    13,     5,    72,   395,    17,   409,    12,    13,
     400,   421,   402,    15,    16,    17,    18,    19,    71,    72,
      63,    13,     1,    76,    77,    78,    79,   421,   418,    72,
      70,    71,    72,   315,    41,    42,   426,   319,    73,     1,
     421,   421,   432,     5,   434,     1,   421,    66,   473,   439,
      12,   333,     5,   421,   444,     1,    67,    68,   340,     5,
      13,    73,    66,   473,    71,   455,    71,    74,    73,    71,
      72,    71,   354,    75,    76,    77,    78,    79,     1,   473,
       1,    71,    13,    73,     5,   475,    71,     5,    41,    42,
       5,    12,   473,   473,    12,    13,    71,   487,   473,     1,
     490,   491,     0,     1,    66,   473,   388,     5,    43,     1,
     500,    71,     1,    73,    12,    13,   506,    15,    16,   156,
      72,    74,   404,    72,    75,    71,    72,    71,   410,    73,
      76,    77,    78,    79,    72,    28,    29,    30,   420,    32,
      33,    34,    40,   425,    72,    38,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
     422,    69,    70,    71,    72,    73,   458,    75,    76,    77,
       0,     1,    80,     5,    72,    83,    84,    85,   147,     5,
      12,    13,    12,    13,   298,    15,    16,    13,   398,    15,
      16,    17,    18,    19,    20,    21,   442,   371,     0,     1,
     497,   394,    13,     5,    15,    16,    17,    18,    19,    20,
      40,    13,    41,   308,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    40,    69,
      70,    71,    72,    73,     0,     1,    76,    77,   163,     5,
      80,    74,   280,    83,    84,    85,    12,    13,   362,    15,
      16,    15,    16,    17,    18,    19,   452,    87,    70,    71,
      72,    73,    13,   248,    15,    16,    17,    18,    19,    20,
      21,    71,    71,    73,    73,    71,   433,    73,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,   129,    69,    70,    71,    72,    73,    71,     1,
      73,    77,     0,     1,    80,    71,    -1,    73,    84,    85,
      12,    13,   473,    15,    16,    13,    -1,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    71,    27,
      73,   156,    71,    31,    73,   156,    -1,    35,    36,    -1,
      -1,    39,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    -1,    69,    70,    71,
      72,    73,    -1,     1,    76,    77,    -1,    71,    80,    73,
      -1,    83,    84,    85,    12,    13,    -1,    15,    16,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,   427,    -1,   429,    -1,    -1,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      -1,    69,    70,    71,    72,    73,    -1,     1,    76,    77,
      -1,     5,    80,    -1,    -1,    83,    84,    85,    12,    13,
      -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    -1,    69,    70,    71,    -1,    -1,
      -1,    75,    76,    77,     1,    -1,    80,    -1,    -1,    83,
      84,    85,     5,    -1,    -1,    12,    13,    -1,    15,    16,
      13,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    -1,    27,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    35,    36,    -1,    -1,    -1,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    -1,    69,    70,    71,    72,    -1,    -1,     1,    76,
      77,    -1,    -1,    80,    -1,    -1,    83,    84,    85,    12,
      13,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    -1,    69,    70,    71,    -1,
      -1,    -1,    75,    76,    77,     1,    -1,    80,    -1,     5,
      83,    84,    85,    -1,    -1,    -1,    12,    13,    -1,    15,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    -1,    69,    70,    71,    72,    73,    -1,     1,
      -1,    77,    -1,     5,    80,    -1,    -1,    -1,    84,    85,
      12,    13,    -1,    15,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,     5,    69,    70,    -1,
      -1,    -1,    -1,    12,    13,    77,    15,    16,    80,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    -1,
      69,    70,    12,    13,    -1,    15,    16,    -1,    77,    -1,
      -1,    80,    -1,    -1,    -1,    84,    85,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    -1,    69,
      70,    -1,    -1,     0,     1,    -1,    -1,    77,    -1,    -1,
      80,    -1,    -1,    -1,    84,    85,    13,    -1,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      27,    -1,    -1,    -1,    31,    -1,    -1,    -1,    35,    36,
      -1,    -1,    39,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    72
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
     137,   138,   139,   140,   141,   142,   143,   144,   145,   146,
     148,   151,     1,    13,    40,    70,   152,   153,   154,   155,
     161,    13,    63,   130,   131,   132,    12,    13,    15,    16,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    69,    77,    80,    84,    85,   136,
     157,   159,   160,   161,   162,   163,    41,    42,    74,   118,
     119,   121,   122,   138,     1,    13,   109,   110,   111,   112,
      71,    73,   167,    12,     5,     6,   100,    21,    13,    17,
      67,    68,    13,    95,    21,     1,    95,     1,    95,     1,
      95,     1,    95,    95,    95,    95,    95,     1,    71,    76,
     142,     1,     5,    77,    78,    79,   135,    13,    20,   143,
     144,   144,     1,   145,    73,   167,   168,    95,    95,    72,
       1,   154,   168,    73,   168,     1,    17,   167,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    95,    95,    95,    95,    95,    95,    95,    95,
      95,    95,    62,   160,     1,    72,    76,    83,   158,   168,
      95,    95,    95,    95,    72,    72,    73,   168,    95,    72,
       1,   111,    73,   168,     1,    17,    95,     1,    95,     1,
      39,   101,   102,   165,   166,    13,    95,    95,    13,    20,
     141,   147,    73,   167,    12,    66,   115,    73,   167,    71,
     122,   124,    73,   167,    71,    73,   167,    13,   114,    12,
      13,   128,   123,   124,    37,   108,    72,    95,    95,    95,
      95,    95,   142,   144,    95,     1,    73,   168,    73,     1,
      95,    73,    95,    95,    72,    71,    76,     1,   157,    95,
      95,    95,   159,     1,    13,   116,    43,    13,   126,    95,
       1,    73,   168,    73,    95,    73,    95,    71,     0,    72,
      98,   168,    73,   167,     1,    96,    13,    14,    65,   142,
      95,    13,   147,     7,     8,     9,    10,    11,    81,    82,
     149,    95,    95,     1,    71,    76,    95,    95,   115,    95,
      71,     5,    71,   152,   140,    73,   167,    95,     1,    95,
      95,     1,   157,    95,    95,    75,    95,    95,    95,    95,
      43,   120,    73,    95,     1,    95,    95,     1,   157,    95,
       1,    73,   167,    95,    95,    95,    95,    81,   149,    95,
     123,   103,    95,    95,   152,    95,    71,    95,   124,    95,
      98,    95,    73,   157,    40,   156,   152,   132,    95,    17,
     117,   118,    95,    95,    73,   157,   152,    98,    75,    75,
      75,    75,    95,    12,    13,   150,    73,   167,     1,    98,
     104,   105,   106,   107,   113,   125,   127,   133,   134,   137,
     164,   165,   166,   103,   122,    72,    73,   167,    95,   109,
     109,    95,   156,    95,     1,    72,    95,    75,   119,    95,
      72,   150,    95,    98,    96,   164,    95,   129,    72,    72,
     157,    95,    95,    95,    81,    72,   130,    95,    81,    95
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
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
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






/*----------.
| yyparse.  |
`----------*/

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
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
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
#line 279 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_rule = (yyvsp[(4) - (6)].rule);
    ;}
    break;

  case 11:
#line 285 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_keyframe = (yyvsp[(4) - (6)].keyframeRule);
    ;}
    break;

  case 12:
#line 291 "css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 13:
#line 297 "css/CSSGrammar.y"
    {
        /* can be empty */
    ;}
    break;

  case 14:
#line 303 "css/CSSGrammar.y"
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
#line 317 "css/CSSGrammar.y"
    {
         CSSParser* p = static_cast<CSSParser*>(parser);
         p->m_mediaQuery = p->sinkFloatingMediaQuery((yyvsp[(4) - (5)].mediaQuery));
     ;}
    break;

  case 16:
#line 324 "css/CSSGrammar.y"
    {
        if ((yyvsp[(4) - (5)].selectorList)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            if (p->m_selectorListForParseSelector)
                p->m_selectorListForParseSelector->adoptSelectorVector(*(yyvsp[(4) - (5)].selectorList));
        }
    ;}
    break;

  case 23:
#line 346 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 26:
#line 356 "css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     (yyval.rule) = static_cast<CSSParser*>(parser)->createCharsetRule((yyvsp[(3) - (5)].string));
     if ((yyval.rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyval.rule));
  ;}
    break;

  case 27:
#line 362 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 28:
#line 364 "css/CSSGrammar.y"
    {
  ;}
    break;

  case 30:
#line 370 "css/CSSGrammar.y"
    {
     CSSParser* p = static_cast<CSSParser*>(parser);
     if ((yyvsp[(2) - (3)].rule) && p->m_styleSheet)
         p->m_styleSheet->append((yyvsp[(2) - (3)].rule));
 ;}
    break;

  case 39:
#line 389 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->m_hadSyntacticallyValidCSSRule = true;
    ;}
    break;

  case 42:
#line 397 "css/CSSGrammar.y"
    { (yyval.ruleList) = 0; ;}
    break;

  case 43:
#line 398 "css/CSSGrammar.y"
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
#line 427 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createImportRule((yyvsp[(3) - (6)].string), (yyvsp[(5) - (6)].mediaList));
    ;}
    break;

  case 56:
#line 430 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 57:
#line 433 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 58:
#line 436 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 59:
#line 442 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), true);
    ;}
    break;

  case 60:
#line 446 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createVariablesRule((yyvsp[(3) - (7)].mediaList), false);
    ;}
    break;

  case 61:
#line 452 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
    ;}
    break;

  case 62:
#line 456 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(3) - (3)].mediaList);
    ;}
    break;

  case 63:
#line 462 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 64:
#line 465 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ((yyvsp[(2) - (2)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 65:
#line 470 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 66:
#line 473 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 67:
#line 476 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 68:
#line 479 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 69:
#line 485 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 70:
#line 488 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 71:
#line 491 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 72:
#line 494 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 73:
#line 497 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 74:
#line 502 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 75:
#line 505 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 76:
#line 511 "css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariable((yyvsp[(1) - (4)].string), (yyvsp[(4) - (4)].valueList));
    ;}
    break;

  case 77:
#line 515 "css/CSSGrammar.y"
    {
        (yyval.boolean) = static_cast<CSSParser*>(parser)->addVariableDeclarationBlock((yyvsp[(1) - (7)].string));
    ;}
    break;

  case 78:
#line 519 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 79:
#line 523 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 80:
#line 527 "css/CSSGrammar.y"
    {
        /* @variables { varname: } Just reduce away this variable with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 81:
#line 532 "css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: @variables { varname: *; }, just discard the property/value pair */
        (yyval.boolean) = false;
    ;}
    break;

  case 82:
#line 539 "css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 83:
#line 545 "css/CSSGrammar.y"
    {
    static_cast<CSSParser*>(parser)->addNamespace((yyvsp[(3) - (6)].string), (yyvsp[(4) - (6)].string));
    (yyval.rule) = 0;
;}
    break;

  case 84:
#line 549 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 85:
#line 552 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 86:
#line 555 "css/CSSGrammar.y"
    {
    (yyval.rule) = 0;
;}
    break;

  case 87:
#line 561 "css/CSSGrammar.y"
    { (yyval.string).characters = 0; ;}
    break;

  case 88:
#line 562 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 91:
#line 571 "css/CSSGrammar.y"
    {
        (yyval.string) = (yyvsp[(1) - (2)].string);
    ;}
    break;

  case 92:
#line 577 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 93:
#line 580 "css/CSSGrammar.y"
    {
        (yyval.valueList) = (yyvsp[(3) - (4)].valueList);
    ;}
    break;

  case 94:
#line 586 "css/CSSGrammar.y"
    {
        (yyvsp[(3) - (7)].string).lower();
        (yyval.mediaQueryExp) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExp((yyvsp[(3) - (7)].string), (yyvsp[(5) - (7)].valueList));
    ;}
    break;

  case 95:
#line 593 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQueryExpList) = p->createFloatingMediaQueryExpList();
        (yyval.mediaQueryExpList)->append(p->sinkFloatingMediaQueryExp((yyvsp[(1) - (1)].mediaQueryExp)));
    ;}
    break;

  case 96:
#line 598 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(1) - (5)].mediaQueryExpList);
        (yyval.mediaQueryExpList)->append(static_cast<CSSParser*>(parser)->sinkFloatingMediaQueryExp((yyvsp[(5) - (5)].mediaQueryExp)));
    ;}
    break;

  case 97:
#line 605 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = static_cast<CSSParser*>(parser)->createFloatingMediaQueryExpList();
    ;}
    break;

  case 98:
#line 608 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryExpList) = (yyvsp[(3) - (3)].mediaQueryExpList);
    ;}
    break;

  case 99:
#line 614 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::None;
    ;}
    break;

  case 100:
#line 617 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Only;
    ;}
    break;

  case 101:
#line 620 "css/CSSGrammar.y"
    {
        (yyval.mediaQueryRestrictor) = MediaQuery::Not;
    ;}
    break;

  case 102:
#line 626 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaQuery) = p->createFloatingMediaQuery(p->sinkFloatingMediaQueryExpList((yyvsp[(1) - (1)].mediaQueryExpList)));
    ;}
    break;

  case 103:
#line 631 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyvsp[(3) - (4)].string).lower();
        (yyval.mediaQuery) = p->createFloatingMediaQuery((yyvsp[(1) - (4)].mediaQueryRestrictor), (yyvsp[(3) - (4)].string), p->sinkFloatingMediaQueryExpList((yyvsp[(4) - (4)].mediaQueryExpList)));
    ;}
    break;

  case 104:
#line 639 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = static_cast<CSSParser*>(parser)->createMediaList();
     ;}
    break;

  case 106:
#line 646 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.mediaList) = p->createMediaList();
        (yyval.mediaList)->appendMediaQuery(p->sinkFloatingMediaQuery((yyvsp[(1) - (1)].mediaQuery)));
    ;}
    break;

  case 107:
#line 651 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = (yyvsp[(1) - (4)].mediaList);
        if ((yyval.mediaList))
            (yyval.mediaList)->appendMediaQuery(static_cast<CSSParser*>(parser)->sinkFloatingMediaQuery((yyvsp[(4) - (4)].mediaQuery)));
    ;}
    break;

  case 108:
#line 656 "css/CSSGrammar.y"
    {
        (yyval.mediaList) = 0;
    ;}
    break;

  case 109:
#line 662 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule((yyvsp[(3) - (7)].mediaList), (yyvsp[(6) - (7)].ruleList));
    ;}
    break;

  case 110:
#line 665 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createMediaRule(0, (yyvsp[(5) - (6)].ruleList));
    ;}
    break;

  case 111:
#line 671 "css/CSSGrammar.y"
    {
      (yyval.string) = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 112:
#line 677 "css/CSSGrammar.y"
    {
        (yyval.rule) = (yyvsp[(7) - (8)].keyframesRule);
        (yyvsp[(7) - (8)].keyframesRule)->setNameInternal((yyvsp[(3) - (8)].string));
    ;}
    break;

  case 115:
#line 689 "css/CSSGrammar.y"
    { (yyval.keyframesRule) = static_cast<CSSParser*>(parser)->createKeyframesRule(); ;}
    break;

  case 116:
#line 690 "css/CSSGrammar.y"
    {
        (yyval.keyframesRule) = (yyvsp[(1) - (3)].keyframesRule);
        if ((yyvsp[(2) - (3)].keyframeRule))
            (yyval.keyframesRule)->append((yyvsp[(2) - (3)].keyframeRule));
    ;}
    break;

  case 117:
#line 698 "css/CSSGrammar.y"
    {
        (yyval.keyframeRule) = static_cast<CSSParser*>(parser)->createKeyframeRule((yyvsp[(1) - (6)].valueList));
    ;}
    break;

  case 118:
#line 704 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 119:
#line 709 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = (yyvsp[(1) - (5)].valueList);
        if ((yyval.valueList))
            (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(5) - (5)].value)));
    ;}
    break;

  case 120:
#line 718 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (1)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 121:
#line 719 "css/CSSGrammar.y"
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
#line 743 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 123:
#line 746 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 124:
#line 753 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createFontFaceRule();
    ;}
    break;

  case 125:
#line 756 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 126:
#line 759 "css/CSSGrammar.y"
    {
      (yyval.rule) = 0;
    ;}
    break;

  case 127:
#line 765 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::DirectAdjacent; ;}
    break;

  case 128:
#line 766 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::IndirectAdjacent; ;}
    break;

  case 129:
#line 767 "css/CSSGrammar.y"
    { (yyval.relation) = CSSSelector::Child; ;}
    break;

  case 130:
#line 771 "css/CSSGrammar.y"
    { (yyval.integer) = -1; ;}
    break;

  case 131:
#line 772 "css/CSSGrammar.y"
    { (yyval.integer) = 1; ;}
    break;

  case 132:
#line 776 "css/CSSGrammar.y"
    {
        (yyval.rule) = static_cast<CSSParser*>(parser)->createStyleRule((yyvsp[(1) - (5)].selectorList));
    ;}
    break;

  case 133:
#line 782 "css/CSSGrammar.y"
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

  case 134:
#line 791 "css/CSSGrammar.y"
    {
        if ((yyvsp[(1) - (4)].selectorList) && (yyvsp[(4) - (4)].selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selectorList) = (yyvsp[(1) - (4)].selectorList);
            (yyval.selectorList)->append(p->sinkFloatingSelector((yyvsp[(4) - (4)].selector)));
        } else
            (yyval.selectorList) = 0;
    ;}
    break;

  case 135:
#line 799 "css/CSSGrammar.y"
    {
        (yyval.selectorList) = 0;
    ;}
    break;

  case 136:
#line 805 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (2)].selector);
    ;}
    break;

  case 137:
#line 811 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 138:
#line 815 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 139:
#line 819 "css/CSSGrammar.y"
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

  case 140:
#line 834 "css/CSSGrammar.y"
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

  case 141:
#line 854 "css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 142:
#line 860 "css/CSSGrammar.y"
    { (yyval.string).characters = 0; (yyval.string).length = 0; ;}
    break;

  case 143:
#line 861 "css/CSSGrammar.y"
    { static UChar star = '*'; (yyval.string).characters = &star; (yyval.string).length = 1; ;}
    break;

  case 144:
#line 862 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 145:
#line 866 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (1)].string), p->m_defaultNamespace);
    ;}
    break;

  case 146:
#line 871 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(2) - (2)].selector);
        if ((yyval.selector)) {
            CSSParser* p = static_cast<CSSParser*>(parser);
            (yyval.selector)->m_tag = QualifiedName(nullAtom, (yyvsp[(1) - (2)].string), p->m_defaultNamespace);
        }
    ;}
    break;

  case 147:
#line 878 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
        CSSParser* p = static_cast<CSSParser*>(parser);
        if ((yyval.selector) && p->m_defaultNamespace != starAtom)
            (yyval.selector)->m_tag = QualifiedName(nullAtom, starAtom, p->m_defaultNamespace);
    ;}
    break;

  case 148:
#line 884 "css/CSSGrammar.y"
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

  case 149:
#line 894 "css/CSSGrammar.y"
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

  case 150:
#line 906 "css/CSSGrammar.y"
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

  case 151:
#line 919 "css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (1)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 152:
#line 927 "css/CSSGrammar.y"
    {
        static UChar star = '*';
        (yyval.string).characters = &star;
        (yyval.string).length = 1;
    ;}
    break;

  case 153:
#line 935 "css/CSSGrammar.y"
    {
        (yyval.selector) = (yyvsp[(1) - (1)].selector);
    ;}
    break;

  case 154:
#line 938 "css/CSSGrammar.y"
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

  case 155:
#line 951 "css/CSSGrammar.y"
    {
        (yyval.selector) = 0;
    ;}
    break;

  case 156:
#line 957 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Id;
        if (!p->m_strict)
            (yyvsp[(1) - (1)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(1) - (1)].string);
    ;}
    break;

  case 157:
#line 965 "css/CSSGrammar.y"
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

  case 161:
#line 983 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::Class;
        if (!p->m_strict)
            (yyvsp[(2) - (2)].string).lower();
        (yyval.selector)->m_value = (yyvsp[(2) - (2)].string);
    ;}
    break;

  case 162:
#line 994 "css/CSSGrammar.y"
    {
        CSSParserString& str = (yyvsp[(1) - (2)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        Document* doc = p->document();
        if (doc && doc->isHTMLDocument())
            str.lower();
        (yyval.string) = str;
    ;}
    break;

  case 163:
#line 1005 "css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (4)].string), nullAtom));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 164:
#line 1010 "css/CSSGrammar.y"
    {
        (yyval.selector) = static_cast<CSSParser*>(parser)->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(nullAtom, (yyvsp[(3) - (8)].string), nullAtom));
        (yyval.selector)->m_match = (CSSSelector::Match)(yyvsp[(4) - (8)].integer);
        (yyval.selector)->m_value = (yyvsp[(6) - (8)].string);
    ;}
    break;

  case 165:
#line 1016 "css/CSSGrammar.y"
    {
        AtomicString namespacePrefix = (yyvsp[(3) - (5)].string);
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->setAttribute(QualifiedName(namespacePrefix, (yyvsp[(4) - (5)].string),
                                   p->m_styleSheet->determineNamespace(namespacePrefix)));
        (yyval.selector)->m_match = CSSSelector::Set;
    ;}
    break;

  case 166:
#line 1024 "css/CSSGrammar.y"
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

  case 167:
#line 1036 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Exact;
    ;}
    break;

  case 168:
#line 1039 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::List;
    ;}
    break;

  case 169:
#line 1042 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Hyphen;
    ;}
    break;

  case 170:
#line 1045 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Begin;
    ;}
    break;

  case 171:
#line 1048 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::End;
    ;}
    break;

  case 172:
#line 1051 "css/CSSGrammar.y"
    {
        (yyval.integer) = CSSSelector::Contain;
    ;}
    break;

  case 175:
#line 1062 "css/CSSGrammar.y"
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

  case 176:
#line 1092 "css/CSSGrammar.y"
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

  case 177:
#line 1112 "css/CSSGrammar.y"
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

  case 178:
#line 1130 "css/CSSGrammar.y"
    {
        CSSParser *p = static_cast<CSSParser*>(parser);
        (yyval.selector) = p->createFloatingSelector();
        (yyval.selector)->m_match = CSSSelector::PseudoClass;
        (yyval.selector)->setArgument(String::number((yyvsp[(4) - (6)].number)));
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

  case 179:
#line 1148 "css/CSSGrammar.y"
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

  case 180:
#line 1167 "css/CSSGrammar.y"
    {
        if (!(yyvsp[(4) - (6)].selector) || (yyvsp[(4) - (6)].selector)->simpleSelector() || (yyvsp[(4) - (6)].selector)->tagHistory())
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

  case 181:
#line 1182 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 182:
#line 1185 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
        if ( (yyvsp[(2) - (2)].boolean) )
            (yyval.boolean) = (yyvsp[(2) - (2)].boolean);
    ;}
    break;

  case 183:
#line 1190 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (1)].boolean);
    ;}
    break;

  case 184:
#line 1193 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 185:
#line 1196 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 186:
#line 1199 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 187:
#line 1202 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (2)].boolean);
    ;}
    break;

  case 188:
#line 1208 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (3)].boolean);
    ;}
    break;

  case 189:
#line 1211 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 190:
#line 1214 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 191:
#line 1217 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 192:
#line 1220 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 193:
#line 1223 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
        if ((yyvsp[(2) - (4)].boolean))
            (yyval.boolean) = (yyvsp[(2) - (4)].boolean);
    ;}
    break;

  case 194:
#line 1228 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (4)].boolean);
    ;}
    break;

  case 195:
#line 1231 "css/CSSGrammar.y"
    {
        (yyval.boolean) = (yyvsp[(1) - (6)].boolean);
    ;}
    break;

  case 196:
#line 1237 "css/CSSGrammar.y"
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

  case 197:
#line 1251 "css/CSSGrammar.y"
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

  case 198:
#line 1263 "css/CSSGrammar.y"
    {
        (yyval.boolean) = false;
    ;}
    break;

  case 199:
#line 1267 "css/CSSGrammar.y"
    {
        /* The default movable type template has letter-spacing: .none;  Handle this by looking for
        error tokens at the start of an expr, recover the expr and then treat as an error, cleaning
        up and deleting the shifted expr.  */
        (yyval.boolean) = false;
    ;}
    break;

  case 200:
#line 1274 "css/CSSGrammar.y"
    {
        /* When we encounter something like p {color: red !important fail;} we should drop the declaration */
        (yyval.boolean) = false;
    ;}
    break;

  case 201:
#line 1279 "css/CSSGrammar.y"
    {
        /* Handle this case: div { text-align: center; !important } Just reduce away the stray !important. */
        (yyval.boolean) = false;
    ;}
    break;

  case 202:
#line 1284 "css/CSSGrammar.y"
    {
        /* div { font-family: } Just reduce away this property with no value. */
        (yyval.boolean) = false;
    ;}
    break;

  case 203:
#line 1289 "css/CSSGrammar.y"
    {
        /* if we come across rules with invalid values like this case: p { weight: *; }, just discard the rule */
        (yyval.boolean) = false;
    ;}
    break;

  case 204:
#line 1294 "css/CSSGrammar.y"
    {
        /* if we come across: div { color{;color:maroon} }, ignore everything within curly brackets */
        (yyval.boolean) = false;
    ;}
    break;

  case 205:
#line 1301 "css/CSSGrammar.y"
    {
        (yyval.integer) = cssPropertyID((yyvsp[(1) - (2)].string));
    ;}
    break;

  case 206:
#line 1307 "css/CSSGrammar.y"
    { (yyval.boolean) = true; ;}
    break;

  case 207:
#line 1308 "css/CSSGrammar.y"
    { (yyval.boolean) = false; ;}
    break;

  case 208:
#line 1312 "css/CSSGrammar.y"
    {
        CSSParser* p = static_cast<CSSParser*>(parser);
        (yyval.valueList) = p->createFloatingValueList();
        (yyval.valueList)->addValue(p->sinkFloatingValue((yyvsp[(1) - (1)].value)));
    ;}
    break;

  case 209:
#line 1317 "css/CSSGrammar.y"
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

  case 210:
#line 1331 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 211:
#line 1334 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 212:
#line 1337 "css/CSSGrammar.y"
    {
        (yyval.valueList) = 0;
    ;}
    break;

  case 213:
#line 1343 "css/CSSGrammar.y"
    {
        (yyval.character) = '/';
    ;}
    break;

  case 214:
#line 1346 "css/CSSGrammar.y"
    {
        (yyval.character) = ',';
    ;}
    break;

  case 215:
#line 1349 "css/CSSGrammar.y"
    {
        (yyval.character) = 0;
  ;}
    break;

  case 216:
#line 1355 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(1) - (1)].value); ;}
    break;

  case 217:
#line 1356 "css/CSSGrammar.y"
    { (yyval.value) = (yyvsp[(2) - (2)].value); (yyval.value).fValue *= (yyvsp[(1) - (2)].integer); ;}
    break;

  case 218:
#line 1357 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_STRING; ;}
    break;

  case 219:
#line 1358 "css/CSSGrammar.y"
    {
      (yyval.value).id = cssValueKeywordID((yyvsp[(1) - (2)].string));
      (yyval.value).unit = CSSPrimitiveValue::CSS_IDENT;
      (yyval.value).string = (yyvsp[(1) - (2)].string);
  ;}
    break;

  case 220:
#line 1364 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 221:
#line 1365 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(2) - (3)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_DIMENSION; ;}
    break;

  case 222:
#line 1366 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_URI; ;}
    break;

  case 223:
#line 1367 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (2)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_UNICODE_RANGE; ;}
    break;

  case 224:
#line 1368 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = (yyvsp[(1) - (1)].string); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 225:
#line 1369 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).string = CSSParserString(); (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_HEXCOLOR; ;}
    break;

  case 226:
#line 1371 "css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (1)].value);
  ;}
    break;

  case 227:
#line 1374 "css/CSSGrammar.y"
    {
      (yyval.value) = (yyvsp[(1) - (2)].value);
  ;}
    break;

  case 228:
#line 1377 "css/CSSGrammar.y"
    { /* Handle width: %; */
      (yyval.value).id = 0; (yyval.value).unit = 0;
  ;}
    break;

  case 229:
#line 1383 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = true; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 230:
#line 1384 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).isInt = false; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_NUMBER; ;}
    break;

  case 231:
#line 1385 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PERCENTAGE; ;}
    break;

  case 232:
#line 1386 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PX; ;}
    break;

  case 233:
#line 1387 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_CM; ;}
    break;

  case 234:
#line 1388 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MM; ;}
    break;

  case 235:
#line 1389 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_IN; ;}
    break;

  case 236:
#line 1390 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PT; ;}
    break;

  case 237:
#line 1391 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_PC; ;}
    break;

  case 238:
#line 1392 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_DEG; ;}
    break;

  case 239:
#line 1393 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_RAD; ;}
    break;

  case 240:
#line 1394 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_GRAD; ;}
    break;

  case 241:
#line 1395 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_TURN; ;}
    break;

  case 242:
#line 1396 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_MS; ;}
    break;

  case 243:
#line 1397 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_S; ;}
    break;

  case 244:
#line 1398 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_HZ; ;}
    break;

  case 245:
#line 1399 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_KHZ; ;}
    break;

  case 246:
#line 1400 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EMS; ;}
    break;

  case 247:
#line 1401 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSParserValue::Q_EMS; ;}
    break;

  case 248:
#line 1402 "css/CSSGrammar.y"
    { (yyval.value).id = 0; (yyval.value).fValue = (yyvsp[(1) - (2)].number); (yyval.value).unit = CSSPrimitiveValue::CSS_EXS; ;}
    break;

  case 249:
#line 1403 "css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).fValue = (yyvsp[(1) - (2)].number);
      (yyval.value).unit = CSSPrimitiveValue::CSS_REMS;
      CSSParser* p = static_cast<CSSParser*>(parser);
      if (Document* doc = p->document())
          doc->setUsesRemUnits(true);
  ;}
    break;

  case 250:
#line 1414 "css/CSSGrammar.y"
    {
      (yyval.value).id = 0;
      (yyval.value).string = (yyvsp[(1) - (1)].string);
      (yyval.value).unit = CSSPrimitiveValue::CSS_PARSER_VARIABLE_FUNCTION_SYNTAX;
  ;}
    break;

  case 251:
#line 1422 "css/CSSGrammar.y"
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
#line 1431 "css/CSSGrammar.y"
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
#line 1447 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 254:
#line 1448 "css/CSSGrammar.y"
    { (yyval.string) = (yyvsp[(1) - (2)].string); ;}
    break;

  case 255:
#line 1455 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 256:
#line 1458 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 257:
#line 1464 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 258:
#line 1467 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 259:
#line 1473 "css/CSSGrammar.y"
    {
        (yyval.rule) = 0;
    ;}
    break;

  case 260:
#line 1491 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    ;}
    break;

  case 261:
#line 1494 "css/CSSGrammar.y"
    {
        static_cast<CSSParser*>(parser)->invalidBlockHit();
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3952 "generated/CSSGrammar.tab.c"
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
      /* If just tried and failed to reuse look-ahead token after an
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

  /* Else will try to reuse look-ahead token after shifting the error
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

  if (yyn == YYFINAL)
    YYACCEPT;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
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


#line 1504 "css/CSSGrammar.y"


