#ifndef CSSGRAMMAR_H
#define CSSGRAMMAR_H
/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

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
#define QEMS 294
#define EMS 295
#define EXS 296
#define PXS 297
#define CMS 298
#define MMS 299
#define INS 300
#define PTS 301
#define PCS 302
#define DEGS 303
#define RADS 304
#define GRADS 305
#define TURNS 306
#define MSECS 307
#define SECS 308
#define HERZ 309
#define KHERZ 310
#define DIMEN 311
#define PERCENTAGE 312
#define FLOATTOKEN 313
#define INTEGER 314
#define URI 315
#define FUNCTION 316
#define NOTFUNCTION 317
#define UNICODERANGE 318
#define VARCALL 319




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 58 "../css/CSSGrammar.y"
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
/* Line 1489 of yacc.c.  */
#line 203 "WebCore/tmp/../generated/CSSGrammar.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#endif
