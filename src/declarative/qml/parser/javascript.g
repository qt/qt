----------------------------------------------------------------------------
--
-- Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
-- Contact: Qt Software Information (qt-info@nokia.com)
--
-- This file is part of the QtScript module of the Qt Toolkit.
--
-- $QT_BEGIN_LICENSE:LGPL$
-- No Commercial Usage
-- This file contains pre-release code and may not be distributed.
-- You may use this file in accordance with the terms and conditions
-- contained in the either Technology Preview License Agreement or the
-- Beta Release License Agreement.
--
-- GNU Lesser General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU Lesser
-- General Public License version 2.1 as published by the Free Software
-- Foundation and appearing in the file LICENSE.LGPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU Lesser General Public License version 2.1 requirements
-- will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
--
-- In addition, as a special exception, Nokia gives you certain
-- additional rights. These rights are described in the Nokia Qt LGPL
-- Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
-- package.
--
-- GNU General Public License Usage
-- Alternatively, this file may be used under the terms of the GNU
-- General Public License version 3.0 as published by the Free Software
-- Foundation and appearing in the file LICENSE.GPL included in the
-- packaging of this file.  Please review the following information to
-- ensure the GNU General Public License version 3.0 requirements will be
-- met: http://www.gnu.org/copyleft/gpl.html.
--
-- If you are unsure which license is appropriate for your use, please
-- contact the sales department at qt-sales@nokia.com.
-- $QT_END_LICENSE$
--
-- This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
-- WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
--
----------------------------------------------------------------------------

%parser         JavaScriptGrammar
%decl           javascriptparser_p.h
%impl           javascriptparser.cpp
%expect         3
%expect-rr      1

%token T_AND "&"                T_AND_AND "&&"              T_AND_EQ "&="
%token T_BREAK "break"          T_CASE "case"               T_CATCH "catch"
%token T_COLON ":"              T_COMMA ";"                 T_CONTINUE "continue"
%token T_DEFAULT "default"      T_DELETE "delete"           T_DIVIDE_ "/"
%token T_DIVIDE_EQ "/="         T_DO "do"                   T_DOT "."
%token T_ELSE "else"            T_EQ "="                    T_EQ_EQ "=="
%token T_EQ_EQ_EQ "==="         T_FINALLY "finally"         T_FOR "for"
%token T_FUNCTION "function"    T_GE ">="                   T_GT ">"
%token T_GT_GT ">>"             T_GT_GT_EQ ">>="            T_GT_GT_GT ">>>"
%token T_GT_GT_GT_EQ ">>>="     T_IDENTIFIER "identifier"   T_IF "if"
%token T_IN "in"                T_INSTANCEOF "instanceof"   T_LBRACE "{"
%token T_LBRACKET "["           T_LE "<="                   T_LPAREN "("
%token T_LT "<"                 T_LT_LT "<<"                T_LT_LT_EQ "<<="
%token T_MINUS "-"              T_MINUS_EQ "-="             T_MINUS_MINUS "--"
%token T_NEW "new"              T_NOT "!"                   T_NOT_EQ "!="
%token T_NOT_EQ_EQ "!=="        T_NUMERIC_LITERAL "numeric literal"     T_OR "|"
%token T_OR_EQ "|="             T_OR_OR "||"                T_PLUS "+"
%token T_PLUS_EQ "+="           T_PLUS_PLUS "++"            T_QUESTION "?"
%token T_RBRACE "}"             T_RBRACKET "]"              T_REMAINDER "%"
%token T_REMAINDER_EQ "%="      T_RETURN "return"           T_RPAREN ")"
%token T_SEMICOLON ";"          T_AUTOMATIC_SEMICOLON       T_STAR "*"
%token T_STAR_EQ "*="           T_STRING_LITERAL "string literal"
%token T_SWITCH "switch"        T_THIS "this"               T_THROW "throw"
%token T_TILDE "~"              T_TRY "try"                 T_TYPEOF "typeof"
%token T_VAR "var"              T_VOID "void"               T_WHILE "while"
%token T_WITH "with"            T_XOR "^"                   T_XOR_EQ "^="
%token T_NULL "null"            T_TRUE "true"               T_FALSE "false"
%token T_CONST "const"
%token T_DEBUGGER "debugger"
%token T_RESERVED_WORD "reserved word"

--- context keywords.
%token T_PUBLIC "public"

%nonassoc SHIFT_THERE
%nonassoc T_IDENTIFIER T_COLON
%nonassoc REDUCE_HERE

%start Program

/.
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QtDebug>

#include <string.h>

#include "javascriptengine_p.h"
#include "javascriptlexer_p.h"
#include "javascriptast_p.h"
#include "javascriptnodepool_p.h"

./

/:
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//
// This file is automatically generated from javascript.g.
// Changes will be lost.
//

#ifndef JAVASCRIPTPARSER_P_H
#define JAVASCRIPTPARSER_P_H

#include "javascriptgrammar_p.h"
#include "javascriptast_p.h"
#include <QtCore/QList>

QT_BEGIN_NAMESPACE

class QString;
class JavaScriptEnginePrivate;
class JavaScriptNameIdImpl;

class JavaScriptParser: protected $table
{
public:
    union Value {
      int ival;
      double dval;
      JavaScriptNameIdImpl *sval;
      JavaScript::AST::ArgumentList *ArgumentList;
      JavaScript::AST::CaseBlock *CaseBlock;
      JavaScript::AST::CaseClause *CaseClause;
      JavaScript::AST::CaseClauses *CaseClauses;
      JavaScript::AST::Catch *Catch;
      JavaScript::AST::DefaultClause *DefaultClause;
      JavaScript::AST::ElementList *ElementList;
      JavaScript::AST::Elision *Elision;
      JavaScript::AST::ExpressionNode *Expression;
      JavaScript::AST::Finally *Finally;
      JavaScript::AST::FormalParameterList *FormalParameterList;
      JavaScript::AST::FunctionBody *FunctionBody;
      JavaScript::AST::FunctionDeclaration *FunctionDeclaration;
      JavaScript::AST::Node *Node;
      JavaScript::AST::PropertyName *PropertyName;
      JavaScript::AST::PropertyNameAndValueList *PropertyNameAndValueList;
      JavaScript::AST::SourceElement *SourceElement;
      JavaScript::AST::SourceElements *SourceElements;
      JavaScript::AST::Statement *Statement;
      JavaScript::AST::StatementList *StatementList;
      JavaScript::AST::VariableDeclaration *VariableDeclaration;
      JavaScript::AST::VariableDeclarationList *VariableDeclarationList;

      JavaScript::AST::UiProgram *UiProgram;
      JavaScript::AST::UiPublicMember *UiPublicMember;
      JavaScript::AST::UiObjectDefinition *UiObjectDefinition;
      JavaScript::AST::UiObjectInitializer *UiObjectInitializer;
      JavaScript::AST::UiObjectBinding *UiObjectBinding;
      JavaScript::AST::UiScriptBinding *UiScriptBinding;
      JavaScript::AST::UiArrayBinding *UiArrayBinding;
      JavaScript::AST::UiObjectMember *UiObjectMember;
      JavaScript::AST::UiObjectMemberList *UiObjectMemberList;
      JavaScript::AST::UiQualifiedId *UiQualifiedId;
    };

    struct DiagnosticMessage {
        enum Kind { Warning, Error };

        DiagnosticMessage()
            : kind(Error), line(0), column(0) {}

        DiagnosticMessage(Kind kind, int line, int column, const QString &message)
            : kind(kind), line(line), column(column), message(message) {}

        bool isWarning() const
        { return kind == Warning; }

        bool isError() const
        { return kind == Error; }

        Kind kind;
        int line;
        int column;
        QString message;
    };

public:
    JavaScriptParser();
    ~JavaScriptParser();

    bool parse(JavaScriptEnginePrivate *driver);

    JavaScript::AST::UiProgram *ast()
    { return sym(1).UiProgram; }

    QList<DiagnosticMessage> diagnosticMessages() const
    { return diagnostic_messages; }

    inline DiagnosticMessage diagnosticMessage() const
    {
        foreach (const DiagnosticMessage &d, diagnostic_messages) {
            if (! d.kind == DiagnosticMessage::Warning)
                return d;
        }

        return DiagnosticMessage();
    }

    inline QString errorMessage() const
    { return diagnosticMessage().message; }

    inline int errorLineNumber() const
    { return diagnosticMessage().line; }

    inline int errorColumnNumber() const
    { return diagnosticMessage().column; }

protected:
    void reallocateStack();

    inline Value &sym(int index)
    { return sym_stack [tos + index - 1]; }

    inline JavaScript::AST::SourceLocation &loc(int index)
    { return location_stack [tos + index - 1]; }

protected:
    int tos;
    int stack_size;
    Value *sym_stack;
    int *state_stack;
    JavaScript::AST::SourceLocation *location_stack;

    // error recovery
    enum { TOKEN_BUFFER_SIZE = 3 };

    struct SavedToken {
       int token;
       double dval;
       JavaScript::AST::SourceLocation loc;
    };

    double yylval;
    JavaScript::AST::SourceLocation yylloc;
    JavaScript::AST::SourceLocation yyprevlloc;

    SavedToken token_buffer[TOKEN_BUFFER_SIZE];
    SavedToken *first_token;
    SavedToken *last_token;

    QList<DiagnosticMessage> diagnostic_messages;
};

:/


/.

#include "javascriptparser_p.h"

//
// This file is automatically generated from javascript.g.
// Changes will be lost.
//

using namespace JavaScript;

QT_BEGIN_NAMESPACE

void JavaScriptParser::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    sym_stack = reinterpret_cast<Value*> (qRealloc(sym_stack, stack_size * sizeof(Value)));
    state_stack = reinterpret_cast<int*> (qRealloc(state_stack, stack_size * sizeof(int)));
    location_stack = reinterpret_cast<AST::SourceLocation*> (qRealloc(location_stack, stack_size * sizeof(AST::SourceLocation)));
}

inline static bool automatic(JavaScriptEnginePrivate *driver, int token)
{
    return token == $table::T_RBRACE
        || token == 0
        || driver->lexer()->prevTerminator();
}


JavaScriptParser::JavaScriptParser():
    tos(0),
    stack_size(0),
    sym_stack(0),
    state_stack(0),
    location_stack(0),
    first_token(0),
    last_token(0)
{
}

JavaScriptParser::~JavaScriptParser()
{
    if (stack_size) {
        qFree(sym_stack);
        qFree(state_stack);
        qFree(location_stack);
    }
}

static inline AST::SourceLocation location(Lexer *lexer)
{
    AST::SourceLocation loc;
    loc.offset = lexer->tokenOffset();
    loc.length = lexer->tokenLength();
    loc.startLine = lexer->startLineNo();
    loc.startColumn = lexer->startColumnNo();
    return loc;
}

bool JavaScriptParser::parse(JavaScriptEnginePrivate *driver)
{
    Lexer *lexer = driver->lexer();
    bool hadErrors = false;
    int yytoken = -1;
    int action = 0;

    first_token = last_token = 0;

    tos = -1;

    do {
        if (++tos == stack_size)
            reallocateStack();

        state_stack[tos] = action;

    _Lcheck_token:
        if (yytoken == -1 && -TERMINAL_COUNT != action_index[action]) {
            yyprevlloc = yylloc;

            if (first_token == last_token) {
                yytoken = lexer->lex();
                yylval = lexer->dval();
                yylloc = location(lexer);
            } else {
                yytoken = first_token->token;
                yylval = first_token->dval;
                yylloc = first_token->loc;
                ++first_token;
            }
        }

        action = t_action(action, yytoken);
        if (action > 0) {
            if (action != ACCEPT_STATE) {
                yytoken = -1;
                sym(1).dval = yylval;
                loc(1) = yylloc;
            } else {
              --tos;
              return ! hadErrors;
            }
        } else if (action < 0) {
          const int r = -action - 1;
          tos -= rhs[r];

          switch (r) {
./

--------------------------------------------------------------------------------------------------------
-- Declarative UI
--------------------------------------------------------------------------------------------------------
Program: UiObjectMemberList ;
/.
case $rule_number: {
    sym(1).Node = makeAstNode<AST::UiProgram> (driver->nodePool(), sym(1).UiObjectMemberList->finish());
} break;
./

Empty: ;

UiObjectMemberList: UiObjectMember ;
/.
case $rule_number: {
    sym(1).Node = makeAstNode<AST::UiObjectMemberList> (driver->nodePool(), sym(1).UiObjectMember);
} break;
./

UiObjectMemberList: UiObjectMemberList Empty UiObjectMember ;
/.case $rule_number:./
UiObjectMemberList: UiObjectMemberList T_COMMA UiObjectMember ;
/.
case $rule_number: {
    AST::UiObjectMemberList *node = makeAstNode<AST:: UiObjectMemberList> (driver->nodePool(),
        sym(1).UiObjectMemberList, sym(3).UiObjectMember);
    sym(1).Node = node;
} break;
./

UiObjectInitializer: T_LBRACE UiObjectMemberList T_RBRACE ;
/.
case $rule_number: {
    AST::UiObjectInitializer *node = makeAstNode<AST::UiObjectInitializer> (driver->nodePool(), sym(2).UiObjectMemberList->finish());
    node->lbraceToken = loc(1);
    node->rbraceToken = loc(3);
    sym(1).Node = node;
}   break;
./

UiObjectMember: UiQualifiedId T_COLON T_IDENTIFIER UiObjectInitializer ;
/.
case $rule_number: {
    AST::UiObjectBinding *node = makeAstNode<AST::UiObjectBinding> (driver->nodePool(), sym(1).UiQualifiedId->finish(),
        sym(3).sval, sym(4).UiObjectInitializer);
    node->colonToken = loc(2);
    node->identifierToken = loc(3);
    sym(1).Node = node;
}   break;
./

UiObjectMember: T_IDENTIFIER UiObjectInitializer ;
/.
case $rule_number: {
    AST::UiObjectDefinition *node = makeAstNode<AST::UiObjectDefinition> (driver->nodePool(), sym(1).sval,
        sym(2).UiObjectInitializer);
    node->identifierToken = loc(1);
    sym(1).Node = node;
}   break;
./

UiObjectMember: UiQualifiedId T_COLON T_LBRACKET UiObjectMemberList T_RBRACKET ;
/.
case $rule_number: {
    AST::UiArrayBinding *node = makeAstNode<AST::UiArrayBinding> (driver->nodePool(), sym(1).UiQualifiedId->finish(),
        sym(4).UiObjectMemberList->finish());
    node->colonToken = loc(2);
    node->lbracketToken = loc(3);
    node->rbraceToken = loc(5);
    sym(1).Node = node;
}   break;
./

UiObjectMember: UiQualifiedId T_COLON Statement ;
/.
case $rule_number: {
    AST::UiScriptBinding *node = makeAstNode<AST::UiScriptBinding> (driver->nodePool(), sym(1).UiQualifiedId->finish(),
        sym(3).Statement);
    node->colonToken = loc(2);
    sym(1).Node = node;
}   break;
./

UiObjectMember: T_PUBLIC T_IDENTIFIER T_IDENTIFIER T_COLON Expression UiObjectInitializer ;
/.
case $rule_number: {
    AST::UiPublicMember *node = makeAstNode<AST::UiPublicMember> (driver->nodePool(), sym(2).sval, sym(3).sval,
        sym(5).Expression, sym(6).UiObjectInitializer);
    node->publicToken = loc(1);
    node->attributeTypeToken = loc(2);
    node->identifierToken = loc(3);
    node->colonToken = loc(4);
    sym(1).Node = node;
}   break;
./

UiObjectMember: T_PUBLIC T_IDENTIFIER T_IDENTIFIER ;
/.
case $rule_number: {
    AST::UiPublicMember *node = makeAstNode<AST::UiPublicMember> (driver->nodePool(), sym(2).sval, sym(3).sval);
    node->publicToken = loc(1);
    node->attributeTypeToken = loc(2);
    node->identifierToken = loc(3);
    sym(1).Node = node;
}   break;
./

UiObjectMember: T_PUBLIC T_IDENTIFIER T_IDENTIFIER T_COLON Expression ;
/.
case $rule_number: {
    AST::UiPublicMember *node = makeAstNode<AST::UiPublicMember> (driver->nodePool(), sym(2).sval, sym(3).sval,
        sym(5).Expression, static_cast<AST::UiObjectInitializer *>(0));
    node->publicToken = loc(1);
    node->attributeTypeToken = loc(2);
    node->identifierToken = loc(3);
    node->colonToken = loc(4);
    sym(1).Node = node;
}   break;
./


UiQualifiedId: T_IDENTIFIER ;
/.
case $rule_number: {
    AST::UiQualifiedId *node = makeAstNode<AST::UiQualifiedId> (driver->nodePool(), sym(1).sval);
    node->identifierToken = loc(1);
    sym(1).Node = node;
}   break;
./

UiQualifiedId: UiQualifiedId T_DOT T_IDENTIFIER ;
/.
case $rule_number: {
    AST::UiQualifiedId *node = makeAstNode<AST::UiQualifiedId> (driver->nodePool(), sym(1).UiQualifiedId, sym(3).sval);
    node->identifierToken = loc(3);
    sym(1).Node = node;
}   break;
./


--------------------------------------------------------------------------------------------------------
-- Expressions
--------------------------------------------------------------------------------------------------------
PrimaryExpression: T_THIS ;
/.
case $rule_number: {
  AST::ThisExpression *node = makeAstNode<AST::ThisExpression> (driver->nodePool());
  node->thisToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_IDENTIFIER ;
/.
case $rule_number: {
  AST::IdentifierExpression *node = makeAstNode<AST::IdentifierExpression> (driver->nodePool(), sym(1).sval);
  node->identifierToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_NULL ;
/.
case $rule_number: {
  AST::NullExpression *node = makeAstNode<AST::NullExpression> (driver->nodePool());
  node->nullToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_TRUE ;
/.
case $rule_number: {
  AST::TrueLiteral *node = makeAstNode<AST::TrueLiteral> (driver->nodePool());
  node->trueToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_FALSE ;
/.
case $rule_number: {
  AST::FalseLiteral *node = makeAstNode<AST::FalseLiteral> (driver->nodePool());
  node->falseToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
  AST::NumericLiteral *node = makeAstNode<AST::NumericLiteral> (driver->nodePool(), sym(1).dval);
  node->literalToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_STRING_LITERAL ;
/.
case $rule_number: {
  AST::StringLiteral *node = makeAstNode<AST::StringLiteral> (driver->nodePool(), sym(1).sval);
  node->literalToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_DIVIDE_ ;
/:
#define J_SCRIPT_REGEXPLITERAL_RULE1 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(Lexer::NoPrefix);
  if (!rx) {
    diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error, lexer->startLineNo(),
        lexer->startColumnNo(), lexer->errorMessage()));
      return false;
  }
  AST::RegExpLiteral *node = makeAstNode<AST::RegExpLiteral> (driver->nodePool(), lexer->pattern, lexer->flags);
  node->literalToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_DIVIDE_EQ ;
/:
#define J_SCRIPT_REGEXPLITERAL_RULE2 $rule_number
:/
/.
case $rule_number: {
  bool rx = lexer->scanRegExp(Lexer::EqualPrefix);
  if (!rx) {
    diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error, lexer->startLineNo(),
        lexer->startColumnNo(), lexer->errorMessage()));
      return false;
  }
  AST::RegExpLiteral *node = makeAstNode<AST::RegExpLiteral> (driver->nodePool(), lexer->pattern, lexer->flags);
  node->literalToken = loc(1);
  sym(1).Node = node;
} break;
./

PrimaryExpression: T_LBRACKET ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArrayLiteral> (driver->nodePool(), sym(2).Elision);
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArrayLiteral> (driver->nodePool(), sym(2).ElementList->finish ());
} break;
./

PrimaryExpression: T_LBRACKET ElementList T_COMMA ElisionOpt T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArrayLiteral> (driver->nodePool(), sym(2).ElementList->finish (), sym(4).Elision);
} break;
./

-- PrimaryExpression: T_LBRACE T_RBRACE ;
-- /.
-- case $rule_number: {
--   sym(1).Node = makeAstNode<AST::ObjectLiteral> (driver->nodePool());
-- } break;
-- ./

PrimaryExpression: T_LBRACE PropertyNameAndValueListOpt T_RBRACE ;
/.
case $rule_number: {
  if (sym(2).Node)
    sym(1).Node = makeAstNode<AST::ObjectLiteral> (driver->nodePool(), sym(2).PropertyNameAndValueList->finish ());
  else
    sym(1).Node = makeAstNode<AST::ObjectLiteral> (driver->nodePool());
} break;
./

PrimaryExpression: T_LBRACE PropertyNameAndValueList T_COMMA T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ObjectLiteral> (driver->nodePool(), sym(2).PropertyNameAndValueList->finish ());
} break;
./

PrimaryExpression: T_LPAREN Expression T_RPAREN ;
/.
case $rule_number: {
  sym(1) = sym(2);
} break;
./

ElementList: ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ElementList> (driver->nodePool(), sym(1).Elision, sym(2).Expression);
} break;
./

ElementList: ElementList T_COMMA ElisionOpt AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ElementList> (driver->nodePool(), sym(1).ElementList, sym(3).Elision, sym(4).Expression);
} break;
./

Elision: T_COMMA ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Elision> (driver->nodePool());
} break;
./

Elision: Elision T_COMMA ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Elision> (driver->nodePool(), sym(1).Elision);
} break;
./

ElisionOpt: %prec SHIFT_THERE ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ElisionOpt: Elision ;
/.
case $rule_number: {
  sym(1).Elision = sym(1).Elision->finish ();
} break;
./

PropertyNameAndValueList: PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PropertyNameAndValueList> (driver->nodePool(), sym(1).PropertyName, sym(3).Expression);
} break;
./

PropertyNameAndValueList: PropertyNameAndValueList T_COMMA PropertyName T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PropertyNameAndValueList> (driver->nodePool(), sym(1).PropertyNameAndValueList, sym(3).PropertyName, sym(5).Expression);
} break;
./

PropertyName: T_IDENTIFIER %prec REDUCE_HERE ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::IdentifierPropertyName> (driver->nodePool(), sym(1).sval);
} break;
./

PropertyName: T_STRING_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::StringLiteralPropertyName> (driver->nodePool(), sym(1).sval);
} break;
./

PropertyName: T_NUMERIC_LITERAL ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::NumericLiteralPropertyName> (driver->nodePool(), sym(1).dval);
} break;
./

PropertyName: ReservedIdentifier ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::IdentifierPropertyName> (driver->nodePool(), sym(1).sval);
} break;
./

ReservedIdentifier: T_BREAK ;
/.
case $rule_number:
./
ReservedIdentifier: T_CASE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CATCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONTINUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEFAULT ;
/.
case $rule_number:
./
ReservedIdentifier: T_DELETE ;
/.
case $rule_number:
./
ReservedIdentifier: T_DO ;
/.
case $rule_number:
./
ReservedIdentifier: T_ELSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FALSE ;
/.
case $rule_number:
./
ReservedIdentifier: T_FINALLY ;
/.
case $rule_number:
./
ReservedIdentifier: T_FOR ;
/.
case $rule_number:
./
ReservedIdentifier: T_FUNCTION ;
/.
case $rule_number:
./
ReservedIdentifier: T_IF ;
/.
case $rule_number:
./
ReservedIdentifier: T_IN ;
/.
case $rule_number:
./
ReservedIdentifier: T_INSTANCEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_NEW ;
/.
case $rule_number:
./
ReservedIdentifier: T_NULL ;
/.
case $rule_number:
./
ReservedIdentifier: T_RETURN ;
/.
case $rule_number:
./
ReservedIdentifier: T_SWITCH ;
/.
case $rule_number:
./
ReservedIdentifier: T_THIS ;
/.
case $rule_number:
./
ReservedIdentifier: T_THROW ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRUE ;
/.
case $rule_number:
./
ReservedIdentifier: T_TRY ;
/.
case $rule_number:
./
ReservedIdentifier: T_TYPEOF ;
/.
case $rule_number:
./
ReservedIdentifier: T_VAR ;
/.
case $rule_number:
./
ReservedIdentifier: T_VOID ;
/.
case $rule_number:
./
ReservedIdentifier: T_WHILE ;
/.
case $rule_number:
./
ReservedIdentifier: T_CONST ;
/.
case $rule_number:
./
ReservedIdentifier: T_DEBUGGER ;
/.
case $rule_number:
./
ReservedIdentifier: T_RESERVED_WORD ;
/.
case $rule_number:
./
ReservedIdentifier: T_WITH ;
/.
case $rule_number:
{
  sym(1).sval = driver->intern(lexer->characterBuffer(), lexer->characterCount());
} break;
./

PropertyIdentifier: T_IDENTIFIER ;
PropertyIdentifier: ReservedIdentifier ;

MemberExpression: PrimaryExpression ;
MemberExpression: FunctionExpression ;

MemberExpression: MemberExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArrayMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
} break;
./

MemberExpression: MemberExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
  AST::FieldMemberExpression *node = makeAstNode<AST::FieldMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).sval);
  node->dotToken = loc(2);
  node->identifierToken = loc(3);
  sym(1).Node = node;
} break;
./

MemberExpression: T_NEW MemberExpression T_LPAREN ArgumentListOpt T_RPAREN ;
/.
case $rule_number: {
  AST::NewMemberExpression *node = makeAstNode<AST::NewMemberExpression> (driver->nodePool(), sym(2).Expression, sym(4).ArgumentList);
  node->newToken = loc(1);
  node->lparenToken = loc(3);
  node->rparenToken = loc(5);
  sym(1).Node = node;
} break;
./

NewExpression: MemberExpression ;

NewExpression: T_NEW NewExpression ;
/.
case $rule_number: {
  AST::NewExpression *node = makeAstNode<AST::NewExpression> (driver->nodePool(), sym(2).Expression);
  node->newToken = loc(1);
  sym(1).Node = node;
} break;
./

CallExpression: MemberExpression T_LPAREN ArgumentListOpt T_RPAREN ;
/.
case $rule_number: {
  AST::CallExpression *node = makeAstNode<AST::CallExpression> (driver->nodePool(), sym(1).Expression, sym(3).ArgumentList);
  node->lparenToken = loc(2);
  node->rparenToken = loc(4);
  sym(1).Node = node;
} break;
./

CallExpression: CallExpression T_LPAREN ArgumentListOpt T_RPAREN ;
/.
case $rule_number: {
  AST::CallExpression *node = makeAstNode<AST::CallExpression> (driver->nodePool(), sym(1).Expression, sym(3).ArgumentList);
  node->lparenToken = loc(2);
  node->rparenToken = loc(4);
  sym(1).Node = node;
} break;
./

CallExpression: CallExpression T_LBRACKET Expression T_RBRACKET ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArrayMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
} break;
./

CallExpression: CallExpression T_DOT PropertyIdentifier ;
/.
case $rule_number: {
  AST::FieldMemberExpression *node = makeAstNode<AST::FieldMemberExpression> (driver->nodePool(), sym(1).Expression, sym(3).sval);
  node->dotToken = loc(2);
  node->identifierToken = loc(3);
  sym(1).Node = node;
} break;
./

ArgumentListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ArgumentListOpt: ArgumentList ;
/.
case $rule_number: {
  sym(1).Node = sym(1).ArgumentList->finish();
} break;
./

ArgumentList: AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ArgumentList> (driver->nodePool(), sym(1).Expression);
} break;
./

ArgumentList: ArgumentList T_COMMA AssignmentExpression ;
/.
case $rule_number: {
  AST::ArgumentList *node = makeAstNode<AST::ArgumentList> (driver->nodePool(), sym(1).ArgumentList, sym(3).Expression);
  node->commaToken = loc(2);
  sym(1).Node = node;
} break;
./

LeftHandSideExpression: NewExpression ;
LeftHandSideExpression: CallExpression ;
PostfixExpression: LeftHandSideExpression ;

PostfixExpression: LeftHandSideExpression T_PLUS_PLUS ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PostIncrementExpression> (driver->nodePool(), sym(1).Expression);
} break;
./

PostfixExpression: LeftHandSideExpression T_MINUS_MINUS ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PostDecrementExpression> (driver->nodePool(), sym(1).Expression);
} break;
./

UnaryExpression: PostfixExpression ;

UnaryExpression: T_DELETE UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::DeleteExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_VOID UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::VoidExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_TYPEOF UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::TypeOfExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_PLUS_PLUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PreIncrementExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_MINUS_MINUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::PreDecrementExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_PLUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::UnaryPlusExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_MINUS UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::UnaryMinusExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_TILDE UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::TildeExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

UnaryExpression: T_NOT UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::NotExpression> (driver->nodePool(), sym(2).Expression);
} break;
./

MultiplicativeExpression: UnaryExpression ;

MultiplicativeExpression: MultiplicativeExpression T_STAR UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Mul, sym(3).Expression);
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_DIVIDE_ UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Div, sym(3).Expression);
} break;
./

MultiplicativeExpression: MultiplicativeExpression T_REMAINDER UnaryExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Mod, sym(3).Expression);
} break;
./

AdditiveExpression: MultiplicativeExpression ;

AdditiveExpression: AdditiveExpression T_PLUS MultiplicativeExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Add, sym(3).Expression);
} break;
./

AdditiveExpression: AdditiveExpression T_MINUS MultiplicativeExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Sub, sym(3).Expression);
} break;
./

ShiftExpression: AdditiveExpression ;

ShiftExpression: ShiftExpression T_LT_LT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::LShift, sym(3).Expression);
} break;
./

ShiftExpression: ShiftExpression T_GT_GT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::RShift, sym(3).Expression);
} break;
./

ShiftExpression: ShiftExpression T_GT_GT_GT AdditiveExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::URShift, sym(3).Expression);
} break;
./

RelationalExpression: ShiftExpression ;

RelationalExpression: RelationalExpression T_LT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Lt, sym(3).Expression);
} break;
./

RelationalExpression: RelationalExpression T_GT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Gt, sym(3).Expression);
} break;
./

RelationalExpression: RelationalExpression T_LE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Le, sym(3).Expression);
} break;
./

RelationalExpression: RelationalExpression T_GE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Ge, sym(3).Expression);
} break;
./

RelationalExpression: RelationalExpression T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::InstanceOf, sym(3).Expression);
} break;
./

RelationalExpression: RelationalExpression T_IN ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::In, sym(3).Expression);
} break;
./

RelationalExpressionNotIn: ShiftExpression ;

RelationalExpressionNotIn: RelationalExpressionNotIn T_LT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Lt, sym(3).Expression);
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GT ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Gt, sym(3).Expression);
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_LE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Le, sym(3).Expression);
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_GE ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Ge, sym(3).Expression);
} break;
./

RelationalExpressionNotIn: RelationalExpressionNotIn T_INSTANCEOF ShiftExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::InstanceOf, sym(3).Expression);
} break;
./

EqualityExpression: RelationalExpression ;

EqualityExpression: EqualityExpression T_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Equal, sym(3).Expression);
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::NotEqual, sym(3).Expression);
} break;
./

EqualityExpression: EqualityExpression T_EQ_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictEqual, sym(3).Expression);
} break;
./

EqualityExpression: EqualityExpression T_NOT_EQ_EQ RelationalExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictNotEqual, sym(3).Expression);
} break;
./

EqualityExpressionNotIn: RelationalExpressionNotIn ;

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Equal, sym(3).Expression);
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ RelationalExpressionNotIn;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::NotEqual, sym(3).Expression);
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_EQ_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictEqual, sym(3).Expression);
} break;
./

EqualityExpressionNotIn: EqualityExpressionNotIn T_NOT_EQ_EQ RelationalExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::StrictNotEqual, sym(3).Expression);
} break;
./

BitwiseANDExpression: EqualityExpression ;

BitwiseANDExpression: BitwiseANDExpression T_AND EqualityExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitAnd, sym(3).Expression);
} break;
./

BitwiseANDExpressionNotIn: EqualityExpressionNotIn ;

BitwiseANDExpressionNotIn: BitwiseANDExpressionNotIn T_AND EqualityExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitAnd, sym(3).Expression);
} break;
./

BitwiseXORExpression: BitwiseANDExpression ;

BitwiseXORExpression: BitwiseXORExpression T_XOR BitwiseANDExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitXor, sym(3).Expression);
} break;
./

BitwiseXORExpressionNotIn: BitwiseANDExpressionNotIn ;

BitwiseXORExpressionNotIn: BitwiseXORExpressionNotIn T_XOR BitwiseANDExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitXor, sym(3).Expression);
} break;
./

BitwiseORExpression: BitwiseXORExpression ;

BitwiseORExpression: BitwiseORExpression T_OR BitwiseXORExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitOr, sym(3).Expression);
} break;
./

BitwiseORExpressionNotIn: BitwiseXORExpressionNotIn ;

BitwiseORExpressionNotIn: BitwiseORExpressionNotIn T_OR BitwiseXORExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::BitOr, sym(3).Expression);
} break;
./

LogicalANDExpression: BitwiseORExpression ;

LogicalANDExpression: LogicalANDExpression T_AND_AND BitwiseORExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::And, sym(3).Expression);
} break;
./

LogicalANDExpressionNotIn: BitwiseORExpressionNotIn ;

LogicalANDExpressionNotIn: LogicalANDExpressionNotIn T_AND_AND BitwiseORExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::And, sym(3).Expression);
} break;
./

LogicalORExpression: LogicalANDExpression ;

LogicalORExpression: LogicalORExpression T_OR_OR LogicalANDExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Or, sym(3).Expression);
} break;
./

LogicalORExpressionNotIn: LogicalANDExpressionNotIn ;

LogicalORExpressionNotIn: LogicalORExpressionNotIn T_OR_OR LogicalANDExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, QSOperator::Or, sym(3).Expression);
} break;
./

ConditionalExpression: LogicalORExpression ;

ConditionalExpression: LogicalORExpression T_QUESTION AssignmentExpression T_COLON AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ConditionalExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression, sym(5).Expression);
} break;
./

ConditionalExpressionNotIn: LogicalORExpressionNotIn ;

ConditionalExpressionNotIn: LogicalORExpressionNotIn T_QUESTION AssignmentExpressionNotIn T_COLON AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ConditionalExpression> (driver->nodePool(), sym(1).Expression, sym(3).Expression, sym(5).Expression);
} break;
./

AssignmentExpression: ConditionalExpression ;

AssignmentExpression: LeftHandSideExpression AssignmentOperator AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, sym(2).ival, sym(3).Expression);
} break;
./

AssignmentExpressionNotIn: ConditionalExpressionNotIn ;

AssignmentExpressionNotIn: LeftHandSideExpression AssignmentOperator AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BinaryExpression> (driver->nodePool(), sym(1).Expression, sym(2).ival, sym(3).Expression);
} break;
./

AssignmentOperator: T_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::Assign;
} break;
./

AssignmentOperator: T_STAR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceMul;
} break;
./

AssignmentOperator: T_DIVIDE_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceDiv;
} break;
./

AssignmentOperator: T_REMAINDER_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceMod;
} break;
./

AssignmentOperator: T_PLUS_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceAdd;
} break;
./

AssignmentOperator: T_MINUS_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceSub;
} break;
./

AssignmentOperator: T_LT_LT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceLeftShift;
} break;
./

AssignmentOperator: T_GT_GT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceRightShift;
} break;
./

AssignmentOperator: T_GT_GT_GT_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceURightShift;
} break;
./

AssignmentOperator: T_AND_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceAnd;
} break;
./

AssignmentOperator: T_XOR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceXor;
} break;
./

AssignmentOperator: T_OR_EQ ;
/.
case $rule_number: {
  sym(1).ival = QSOperator::InplaceOr;
} break;
./

Expression: AssignmentExpression ;

Expression: Expression T_COMMA AssignmentExpression ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Expression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
} break;
./

ExpressionOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ExpressionOpt: Expression ;

ExpressionNotIn: AssignmentExpressionNotIn ;

ExpressionNotIn: ExpressionNotIn T_COMMA AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Expression> (driver->nodePool(), sym(1).Expression, sym(3).Expression);
} break;
./

ExpressionNotInOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

ExpressionNotInOpt: ExpressionNotIn ;

Statement: Block ;
Statement: VariableStatement ;
Statement: EmptyStatement ;
Statement: ExpressionStatement ;
Statement: IfStatement ;
Statement: IterationStatement ;
Statement: ContinueStatement ;
Statement: BreakStatement ;
Statement: ReturnStatement ;
Statement: WithStatement ;
Statement: LabelledStatement ;
Statement: SwitchStatement ;
Statement: ThrowStatement ;
Statement: TryStatement ;
Statement: DebuggerStatement ;


Block: T_LBRACE StatementListOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Block> (driver->nodePool(), sym(2).StatementList);
} break;
./

StatementList: Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::StatementList> (driver->nodePool(), sym(1).Statement);
} break;
./

StatementList: StatementList Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::StatementList> (driver->nodePool(), sym(1).StatementList, sym(2).Statement);
} break;
./

StatementListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

StatementListOpt: StatementList ;
/.
case $rule_number: {
  sym(1).Node = sym(1).StatementList->finish ();
} break;
./

VariableStatement: VariableDeclarationKind VariableDeclarationList T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
VariableStatement: VariableDeclarationKind VariableDeclarationList T_SEMICOLON ;
/.
case $rule_number: {
  AST::VariableStatement *node = makeAstNode<AST::VariableStatement> (driver->nodePool(), sym(2).VariableDeclarationList->finish (/*readOnly=*/sym(1).ival == T_CONST));
  node->declarationKindToken = loc(1);
  node->semicolonToken = loc(3);
  sym(1).Node = node;
} break;
./

VariableDeclarationKind: T_CONST ;
/.
case $rule_number: {
  sym(1).ival = T_CONST;
} break;
./

VariableDeclarationKind: T_VAR ;
/.
case $rule_number: {
  sym(1).ival = T_VAR;
} break;
./

VariableDeclarationList: VariableDeclaration ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclaration);
} break;
./

VariableDeclarationList: VariableDeclarationList T_COMMA VariableDeclaration ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclarationList, sym(3).VariableDeclaration);
} break;
./

VariableDeclarationListNotIn: VariableDeclarationNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclaration);
} break;
./

VariableDeclarationListNotIn: VariableDeclarationListNotIn T_COMMA VariableDeclarationNotIn ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::VariableDeclarationList> (driver->nodePool(), sym(1).VariableDeclarationList, sym(3).VariableDeclaration);
} break;
./

VariableDeclaration: T_IDENTIFIER InitialiserOpt ;
/.
case $rule_number: {
  AST::VariableDeclaration *node = makeAstNode<AST::VariableDeclaration> (driver->nodePool(), sym(1).sval, sym(2).Expression);
  node->identifierToken = loc(1);
  sym(1).Node = node;
} break;
./

VariableDeclarationNotIn: T_IDENTIFIER InitialiserNotInOpt ;
/.
case $rule_number: {
  AST::VariableDeclaration *node = makeAstNode<AST::VariableDeclaration> (driver->nodePool(), sym(1).sval, sym(2).Expression);
  node->identifierToken = loc(1);
  sym(1).Node = node;
} break;
./

Initialiser: T_EQ AssignmentExpression ;
/.
case $rule_number: {
  sym(1) = sym(2);
} break;
./

InitialiserOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

InitialiserOpt: Initialiser ;

InitialiserNotIn: T_EQ AssignmentExpressionNotIn ;
/.
case $rule_number: {
  sym(1) = sym(2);
} break;
./

InitialiserNotInOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

InitialiserNotInOpt: InitialiserNotIn ;

EmptyStatement: T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::EmptyStatement> (driver->nodePool());
} break;
./

ExpressionStatement: Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ExpressionStatement: Expression T_SEMICOLON ;
/.
case $rule_number: {
  AST::ExpressionStatement *node = makeAstNode<AST::ExpressionStatement> (driver->nodePool(), sym(1).Expression);
  node->semicolonToken = loc(2);
  sym(1).Node = node;
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement T_ELSE Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::IfStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement, sym(7).Statement);
} break;
./

IfStatement: T_IF T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::IfStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
} break;
./


IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
IterationStatement: T_DO Statement T_WHILE T_LPAREN Expression T_RPAREN T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::DoWhileStatement> (driver->nodePool(), sym(2).Statement, sym(5).Expression);
} break;
./

IterationStatement: T_WHILE T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::WhileStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
} break;
./

IterationStatement: T_FOR T_LPAREN ExpressionNotInOpt T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ForStatement> (driver->nodePool(), sym(3).Expression, sym(5).Expression, sym(7).Expression, sym(9).Statement);
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationListNotIn T_SEMICOLON ExpressionOpt T_SEMICOLON ExpressionOpt T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::LocalForStatement> (driver->nodePool(), sym(4).VariableDeclarationList->finish (/*readOnly=*/false), sym(6).Expression, sym(8).Expression, sym(10).Statement);
} break;
./

IterationStatement: T_FOR T_LPAREN LeftHandSideExpression T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ForEachStatement> (driver->nodePool(), sym(3).Expression, sym(5).Expression, sym(7).Statement);
} break;
./

IterationStatement: T_FOR T_LPAREN T_VAR VariableDeclarationNotIn T_IN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::LocalForEachStatement> (driver->nodePool(), sym(4).VariableDeclaration, sym(6).Expression, sym(8).Statement);
} break;
./

ContinueStatement: T_CONTINUE T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ContinueStatement> (driver->nodePool());
} break;
./

ContinueStatement: T_CONTINUE T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ContinueStatement: T_CONTINUE T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ContinueStatement> (driver->nodePool(), sym(2).sval);
} break;
./

BreakStatement: T_BREAK T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BreakStatement> (driver->nodePool());
} break;
./

BreakStatement: T_BREAK T_IDENTIFIER T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
BreakStatement: T_BREAK T_IDENTIFIER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::BreakStatement> (driver->nodePool(), sym(2).sval);
} break;
./

ReturnStatement: T_RETURN ExpressionOpt T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ReturnStatement: T_RETURN ExpressionOpt T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ReturnStatement> (driver->nodePool(), sym(2).Expression);
} break;
./

WithStatement: T_WITH T_LPAREN Expression T_RPAREN Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::WithStatement> (driver->nodePool(), sym(3).Expression, sym(5).Statement);
} break;
./

SwitchStatement: T_SWITCH T_LPAREN Expression T_RPAREN CaseBlock ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::SwitchStatement> (driver->nodePool(), sym(3).Expression, sym(5).CaseBlock);
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::CaseBlock> (driver->nodePool(), sym(2).CaseClauses);
} break;
./

CaseBlock: T_LBRACE CaseClausesOpt DefaultClause CaseClausesOpt T_RBRACE ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::CaseBlock> (driver->nodePool(), sym(2).CaseClauses, sym(3).DefaultClause, sym(4).CaseClauses);
} break;
./

CaseClauses: CaseClause ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::CaseClauses> (driver->nodePool(), sym(1).CaseClause);
} break;
./

CaseClauses: CaseClauses CaseClause ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::CaseClauses> (driver->nodePool(), sym(1).CaseClauses, sym(2).CaseClause);
} break;
./

CaseClausesOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

CaseClausesOpt: CaseClauses ;
/.
case $rule_number: {
  sym(1).Node = sym(1).CaseClauses->finish ();
} break;
./

CaseClause: T_CASE Expression T_COLON StatementListOpt ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::CaseClause> (driver->nodePool(), sym(2).Expression, sym(4).StatementList);
} break;
./

DefaultClause: T_DEFAULT T_COLON StatementListOpt ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::DefaultClause> (driver->nodePool(), sym(3).StatementList);
} break;
./

LabelledStatement: T_IDENTIFIER T_COLON Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::LabelledStatement> (driver->nodePool(), sym(1).sval, sym(3).Statement);
} break;
./

ThrowStatement: T_THROW Expression T_AUTOMATIC_SEMICOLON ;  -- automatic semicolon
ThrowStatement: T_THROW Expression T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::ThrowStatement> (driver->nodePool(), sym(2).Expression);
} break;
./

TryStatement: T_TRY Block Catch ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Catch);
} break;
./

TryStatement: T_TRY Block Finally ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Finally);
} break;
./

TryStatement: T_TRY Block Catch Finally ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::TryStatement> (driver->nodePool(), sym(2).Statement, sym(3).Catch, sym(4).Finally);
} break;
./

Catch: T_CATCH T_LPAREN T_IDENTIFIER T_RPAREN Block ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Catch> (driver->nodePool(), sym(3).sval, sym(5).Statement);
} break;
./

Finally: T_FINALLY Block ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::Finally> (driver->nodePool(), sym(2).Statement);
} break;
./

DebuggerStatement: T_DEBUGGER T_AUTOMATIC_SEMICOLON ; -- automatic semicolon
DebuggerStatement: T_DEBUGGER T_SEMICOLON ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::DebuggerStatement> (driver->nodePool());
} break;
./

FunctionDeclaration: T_FUNCTION T_IDENTIFIER T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
  AST::FunctionDeclaration *node = makeAstNode<AST::FunctionDeclaration> (driver->nodePool(), sym(2).sval, sym(4).FormalParameterList, sym(7).FunctionBody);
  node->functionToken = loc(1);
  node->identifierToken = loc(2);
  node->lparenToken = loc(3);
  node->rparenToken = loc(5);
  node->lbraceToken = loc(6);
  node->rbraceToken = loc(8);
  sym(1).Node = node;
} break;
./

FunctionExpression: T_FUNCTION IdentifierOpt T_LPAREN FormalParameterListOpt T_RPAREN T_LBRACE FunctionBodyOpt T_RBRACE ;
/.
case $rule_number: {
  AST::FunctionExpression *node = makeAstNode<AST::FunctionExpression> (driver->nodePool(), sym(2).sval, sym(4).FormalParameterList, sym(7).FunctionBody);
  node->functionToken = loc(1);
  if (sym(2).sval)
      node->identifierToken = loc(2);
  node->lparenToken = loc(3);
  node->rparenToken = loc(5);
  node->lbraceToken = loc(6);
  node->rbraceToken = loc(8);
  sym(1).Node = node;
} break;
./

FormalParameterList: T_IDENTIFIER ;
/.
case $rule_number: {
  AST::FormalParameterList *node = makeAstNode<AST::FormalParameterList> (driver->nodePool(), sym(1).sval);
  node->identifierToken = loc(1);
  sym(1).Node = node;
} break;
./

FormalParameterList: FormalParameterList T_COMMA T_IDENTIFIER ;
/.
case $rule_number: {
  AST::FormalParameterList *node = makeAstNode<AST::FormalParameterList> (driver->nodePool(), sym(1).FormalParameterList, sym(3).sval);
  node->commaToken = loc(2);
  node->identifierToken = loc(3);
  sym(1).Node = node;
} break;
./

FormalParameterListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

FormalParameterListOpt: FormalParameterList ;
/.
case $rule_number: {
  sym(1).Node = sym(1).FormalParameterList->finish ();
} break;
./

FunctionBodyOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

FunctionBodyOpt: FunctionBody ;

FunctionBody: SourceElements ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::FunctionBody> (driver->nodePool(), sym(1).SourceElements->finish ());
} break;
./

--JavaScriptProgram: SourceElements ;
--/.
--case $rule_number: {
--  sym(1).Node = makeAstNode<AST::Program> (driver->nodePool(), sym(1).SourceElements->finish ());
--  driver->changeAbstractSyntaxTree(sym(1).Node);
--} break;
--./

SourceElements: SourceElement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::SourceElements> (driver->nodePool(), sym(1).SourceElement);
} break;
./

SourceElements: SourceElements SourceElement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::SourceElements> (driver->nodePool(), sym(1).SourceElements, sym(2).SourceElement);
} break;
./

SourceElement: Statement ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::StatementSourceElement> (driver->nodePool(), sym(1).Statement);
} break;
./

SourceElement: FunctionDeclaration ;
/.
case $rule_number: {
  sym(1).Node = makeAstNode<AST::FunctionSourceElement> (driver->nodePool(), sym(1).FunctionDeclaration);
} break;
./

IdentifierOpt: ;
/.
case $rule_number: {
  sym(1).sval = 0;
} break;
./

IdentifierOpt: T_IDENTIFIER ;

PropertyNameAndValueListOpt: ;
/.
case $rule_number: {
  sym(1).Node = 0;
} break;
./

PropertyNameAndValueListOpt: PropertyNameAndValueList ;

/.
            } // switch
            action = nt_action(state_stack[tos], lhs[r] - TERMINAL_COUNT);
        } // if
    } while (action != 0);

    if (first_token == last_token) {
        const int errorState = state_stack[tos];

        // automatic insertion of `;'
        if (t_action(errorState, T_AUTOMATIC_SEMICOLON) && automatic(driver, yytoken)) {
            SavedToken &tk = token_buffer[0];
            tk.token = yytoken;
            tk.dval = yylval;
            tk.loc = yylloc;

            const QString msg = QString::fromUtf8("Missing `;'");

            diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Warning,
                yyprevlloc.startLine, yyprevlloc.startColumn, msg));

            first_token = &token_buffer[0];
            last_token = &token_buffer[1];

            yytoken = T_SEMICOLON;
            yylval = 0;

            action = errorState;

            goto _Lcheck_token;
        }

        hadErrors = true;

        token_buffer[0].token = yytoken;
        token_buffer[0].dval = yylval;
        token_buffer[0].loc = yylloc;

        token_buffer[1].token = yytoken = lexer->lex();
        token_buffer[1].dval  = yylval  = lexer->dval();
        token_buffer[1].loc   = yylloc  = location(lexer);

        if (t_action(errorState, yytoken)) {
            const QString msg = QString::fromUtf8("Removed token: `%1'").arg(spell[token_buffer[0].token]);

            diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error,
                token_buffer[0].loc.startLine, token_buffer[0].loc.startColumn, msg));

            action = errorState;
            goto _Lcheck_token;
        }

        static int tokens[] = {
            T_PLUS,
            T_EQ,

            T_COMMA,
            T_COLON,
            T_SEMICOLON,

            T_RPAREN, T_RBRACKET, T_RBRACE,

            T_NUMERIC_LITERAL,
            T_IDENTIFIER,

            T_LPAREN, T_LBRACKET, T_LBRACE,

            EOF_SYMBOL
        };

        for (int *tk = tokens; *tk != EOF_SYMBOL; ++tk) {
            int a = t_action(errorState, *tk);
            if (a > 0 && t_action(a, yytoken)) {
                const QString msg = QString::fromUtf8("Inserted token: `%1'").arg(spell[*tk]);

                diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error,
                    token_buffer[0].loc.startLine, token_buffer[0].loc.startColumn, msg));

                yytoken = *tk;
                yylval = 0;
                yylloc = token_buffer[0].loc;

                first_token = &token_buffer[0];
                last_token = &token_buffer[2];

                action = errorState;
                goto _Lcheck_token;
            }
        }

        for (int tk = 1; tk < TERMINAL_COUNT; ++tk) {
            int a = t_action(errorState, tk);
            if (a > 0 && t_action(a, yytoken)) {
                const QString msg = QString::fromUtf8("Inserted token: `%1'").arg(spell[tk]);
                diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error,
                    token_buffer[0].loc.startLine, token_buffer[0].loc.startColumn, msg));

                yytoken = tk;
                yylval = 0;
                yylloc = token_buffer[0].loc;

                action = errorState;
                goto _Lcheck_token;
            }
        }

        const QString msg = QString::fromUtf8("Unexpected token");
        diagnostic_messages.append(DiagnosticMessage(DiagnosticMessage::Error,
            token_buffer[0].loc.startLine, token_buffer[0].loc.startColumn, msg));
    }

    return false;
}

QT_END_NAMESPACE


./
/:
QT_END_NAMESPACE



#endif // JAVASCRIPTPARSER_P_H
:/
