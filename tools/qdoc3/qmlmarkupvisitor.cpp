/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QDebug>
#include <QStringList>
#include <QtGlobal>
#include "private/qdeclarativejsast_p.h"
#include "private/qdeclarativejsastfwd_p.h"
#include "private/qdeclarativejsengine_p.h"

#include "qmlmarkupvisitor.h"

QT_BEGIN_NAMESPACE

QmlMarkupVisitor::QmlMarkupVisitor(const QString &source, QDeclarativeJS::Engine *engine)
{
    this->source = source;
    this->engine = engine;
    indent = 0;
    cursor = 0;
    commentIndex = 0;
    debug += QString("Comments %1\n").arg(engine->comments().length());
}

QmlMarkupVisitor::~QmlMarkupVisitor()
{
}

// The protect() function is a copy of the one from CppCodeMarker.

static const QString samp  = QLatin1String("&amp;");
static const QString slt   = QLatin1String("&lt;");
static const QString sgt   = QLatin1String("&gt;");
static const QString squot = QLatin1String("&quot;");

QString QmlMarkupVisitor::protect(const QString& str)
{
    int n = str.length();
    QString marked;
    marked.reserve(n * 2 + 30);
    const QChar *data = str.constData();
    for (int i = 0; i != n; ++i) {
        switch (data[i].unicode()) {
            case '&': marked += samp;  break;
            case '<': marked += slt;   break;
            case '>': marked += sgt;   break;
            case '"': marked += squot; break;
            default : marked += data[i];
        }
    }
    return marked;
}

QString QmlMarkupVisitor::markedUpCode()
{
    if (cursor < source.length())
        addExtra(cursor, source.length());

    //qDebug() << debug;
    qDebug() << output;
    return output;
}

void QmlMarkupVisitor::addExtra(quint32 start, quint32 finish)
{
    if (commentIndex >= engine->comments().length()) {
        QString extra = source.mid(start, finish - start);
        if (extra.trimmed().isEmpty())
            output += extra;
        else
            output += protect(extra); // text that should probably have been caught by the parser

        cursor = finish;
        return;
    }

    while (commentIndex < engine->comments().length()) {
        if (engine->comments()[commentIndex].offset - 2 >= start)
            break;
        commentIndex++;
    }

    quint32 i = start;
    while (i < finish && commentIndex < engine->comments().length()) {
        quint32 j = engine->comments()[commentIndex].offset - 2;
        if (i <= j && j < finish) {
            if (i < j)
                output += protect(source.mid(i, j - i));

            quint32 l = engine->comments()[commentIndex].length;
            if (source.mid(j, 2) == QLatin1String("/*"))
                l += 4;
            else
                l += 2;
            output += QLatin1String("<@comment>");
            output += protect(source.mid(j, l));
            output += QLatin1String("</@comment>");
            commentIndex++;
            i = j + l;
        } else
            break;
    }

    QString extra = source.mid(i, finish - i);
    if (extra.trimmed().isEmpty())
        output += extra;
    else
        output += protect(extra); // text that should probably have been caught by the parser

    cursor = finish;
}

void QmlMarkupVisitor::addMarkedUpToken(
    QDeclarativeJS::AST::SourceLocation &location, const QString &tagName)
{
    if (!location.isValid())
        return;

    if (cursor < location.offset)
        addExtra(cursor, location.offset);
    else if (cursor > location.offset)
        debug += QString("%1 %2\n").arg(cursor).arg(location.offset);

    output += QString(QLatin1String("<@%1>%2</@%3>")).arg(tagName, protect(sourceText(location)), tagName);
    cursor += location.length;
}

QString QmlMarkupVisitor::sourceText(QDeclarativeJS::AST::SourceLocation &location)
{
    return source.mid(location.offset, location.length);
}

void QmlMarkupVisitor::addVerbatim(QDeclarativeJS::AST::SourceLocation first,
                                   QDeclarativeJS::AST::SourceLocation last)
{
    if (!first.isValid())
        return;

    quint32 start = first.begin();
    quint32 finish;
    if (last.isValid())
        finish = last.end();
    else
        finish = first.end();

    if (cursor < start)
        addExtra(cursor, start);
    else if (cursor > start)
        debug += QString("%1 %2 %3 x\n").arg(sourceText(first)).arg(cursor).arg(start);

    QString text = source.mid(start, finish - start);
    write(text);
    indent -= 1;
    output += protect(text);
    cursor = finish;
}

void QmlMarkupVisitor::write(const QString &text)
{
    debug += QString().fill(QChar(' '), indent) + text + QLatin1String("\n");
    indent += 1;
}

void QmlMarkupVisitor::endWrite(const QString &)
{
    indent -= 1;
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiProgram *)
{
    write("<UiProgram>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiProgram *)
{
    endWrite("<UiProgram>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiImport *uiimport)
{
    write("<UiImport>");
    addVerbatim(uiimport->importToken);
    if (!uiimport->importUri)
        addMarkedUpToken(uiimport->fileNameToken, QLatin1String("headerfile"));
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiImport *uiimport)
{
    endWrite("<UiImport>");
    addVerbatim(uiimport->versionToken);
    addVerbatim(uiimport->asToken);
    addMarkedUpToken(uiimport->importIdToken, QLatin1String("headerfile"));
    addVerbatim(uiimport->semicolonToken);
    //endWrite("<UiImport>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiPublicMember *member)
{
    write("<UiPublicMember>");
    if (member->type == QDeclarativeJS::AST::UiPublicMember::Property) {
        addVerbatim(member->defaultToken);
        addVerbatim(member->readonlyToken);
        addVerbatim(member->propertyToken);
        addVerbatim(member->typeModifierToken);
        addMarkedUpToken(member->typeToken, QLatin1String("type"));
        addMarkedUpToken(member->identifierToken, QLatin1String("name"));
        addVerbatim(member->colonToken);
        QDeclarativeJS::AST::Node::accept(member->binding, this);
    } else {
        addVerbatim(member->propertyToken);
        addVerbatim(member->typeModifierToken);
        addMarkedUpToken(member->typeToken, QLatin1String("type"));
        //addVerbatim(member->identifierToken);
        QDeclarativeJS::AST::Node::accept(member->parameters, this);
    }
    addVerbatim(member->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiPublicMember *member)
{
    endWrite("<UiPublicMember>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiSourceElement *)
{
    write("<UiSourceElement>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiSourceElement *)
{
    endWrite("<UiSourceElement>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiParameterList *)
{
    write("<UiParameterList>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiParameterList *)
{
    endWrite("<UiParameterList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiObjectInitializer *initializer)
{
    write("<UiObjectInitializer>");
    addVerbatim(initializer->lbraceToken, initializer->lbraceToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiObjectInitializer *initializer)
{
    endWrite("<UiObjectInitializer>");
    addVerbatim(initializer->rbraceToken, initializer->rbraceToken);
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiObjectBinding *binding)
{
    write("<UiObjectBinding>");
    QDeclarativeJS::AST::Node::accept(binding->qualifiedId, this);
    addVerbatim(binding->colonToken);
    QDeclarativeJS::AST::Node::accept(binding->qualifiedTypeNameId, this);
    QDeclarativeJS::AST::Node::accept(binding->initializer, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiObjectBinding *)
{
    endWrite("<UiObjectBinding>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiScriptBinding *binding)
{
    write("<UiScriptBinding>");
    QDeclarativeJS::AST::Node::accept(binding->qualifiedId, this);
    addVerbatim(binding->colonToken);
    QDeclarativeJS::AST::Node::accept(binding->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiScriptBinding *)
{
    endWrite("<UiScriptBinding>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiArrayBinding *binding)
{
    write("<UiArrayBinding>");
    QDeclarativeJS::AST::Node::accept(binding->qualifiedId, this);
    addVerbatim(binding->colonToken);
    addVerbatim(binding->lbracketToken);
    QDeclarativeJS::AST::Node::accept(binding->members, this);
    addVerbatim(binding->rbracketToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiArrayBinding *formal)
{
    endWrite("<UiArrayBinding>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiArrayMemberList *list)
{
    write("<UiArrayMemberList>");
    for (QDeclarativeJS::AST::UiArrayMemberList *it = list; it; it = it->next) {
        QDeclarativeJS::AST::Node::accept(it->member, this);
        //addVerbatim(it->commaToken);
    }
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiArrayMemberList *)
{
    endWrite("<UiArrayMemberList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiQualifiedId *id)
{
    write("<UiQualifiedId>");
    addMarkedUpToken(id->identifierToken, QLatin1String("name"));
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiQualifiedId *)
{
    endWrite("<UiQualifiedId>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiSignature *signature)
{
    write("<UiSignature>");
    addVerbatim(signature->lparenToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiSignature *signature)
{
    addVerbatim(signature->rparenToken);
    endWrite("<UiSignature>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiFormal *formal)
{
    write("<UiFormal>");
    addMarkedUpToken(formal->identifierToken, QLatin1String("name"));
    addVerbatim(formal->asToken);
    addVerbatim(formal->aliasToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiFormal *)
{
    endWrite("<UiFormal>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ThisExpression *expression)
{
    write("<ThisExpression>");
    addVerbatim(expression->thisToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ThisExpression *)
{
    endWrite("<ThisExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::IdentifierExpression *identifier)
{
    write("<IdentifierExpression>");
    addMarkedUpToken(identifier->identifierToken, QLatin1String("name"));
    return false;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::IdentifierExpression *)
{
    endWrite("<IdentifierExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NullExpression *null)
{
    write("<NullExpression>");
    addMarkedUpToken(null->nullToken, QLatin1String("number"));
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NullExpression *)
{
    endWrite("<NullExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::TrueLiteral *literal)
{
    write("<TrueLiteral>");
    addMarkedUpToken(literal->trueToken, QLatin1String("number"));
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::TrueLiteral *)
{
    endWrite("<TrueLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FalseLiteral *literal)
{
    write("<FalseLiteral>");
    addMarkedUpToken(literal->falseToken, QLatin1String("number"));
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FalseLiteral *)
{
    endWrite("<FalseLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NumericLiteral *literal)
{
    //write("<NumericLiteral>");
    addMarkedUpToken(literal->literalToken, QLatin1String("number"));
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NumericLiteral *)
{
    //endWrite("<NumericLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::StringLiteral *literal)
{
    //write("<StringLiteral>");
    addMarkedUpToken(literal->literalToken, QLatin1String("string"));
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::StringLiteral *)
{
    //endWrite("<StringLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::RegExpLiteral *literal)
{
    write("<RegExpLiteral>");
    addVerbatim(literal->literalToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::RegExpLiteral *)
{
    endWrite("<RegExpLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ArrayLiteral *literal)
{
    write("<ArrayLiteral>");
    addVerbatim(literal->lbracketToken);
    QDeclarativeJS::AST::Node::accept(literal->elements, this);
    addVerbatim(literal->rbracketToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ArrayLiteral *)
{
    endWrite("<ArrayLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ObjectLiteral *literal)
{
    write("<ObjectLiteral>");
    addVerbatim(literal->lbraceToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ObjectLiteral *literal)
{
    addVerbatim(literal->rbraceToken);
    endWrite("<ObjectLiteral>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ElementList *list)
{
    write("<ElementList>");
    for (QDeclarativeJS::AST::ElementList *it = list; it; it = it->next) {
        QDeclarativeJS::AST::Node::accept(it->expression, this);
        //addVerbatim(it->commaToken);
    }
    QDeclarativeJS::AST::Node::accept(list->elision, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ElementList *)
{
    endWrite("<ElementList>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Elision *elision)
{
    write("<Elision>");
    addVerbatim(elision->commaToken, elision->commaToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Elision *)
{
    endWrite("<Elision>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::IdentifierPropertyName *)
{
    write("<IdentifierPropertyName>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::IdentifierPropertyName *)
{
    endWrite("<IdentifierPropertyName>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::StringLiteralPropertyName *)
{
    write("<StringLiteralPropertyName>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::StringLiteralPropertyName *)
{
    endWrite("<StringLiteralPropertyName>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NumericLiteralPropertyName *)
{
    write("<NumericLiteralPropertyName>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NumericLiteralPropertyName *)
{
    endWrite("<NumericLiteralPropertyName>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::PropertyNameAndValueList *list)
{
    write("<PropertyNameAndValueList>");
    QDeclarativeJS::AST::Node::accept(list->name, this);
    addVerbatim(list->colonToken, list->colonToken);
    QDeclarativeJS::AST::Node::accept(list->value, this);
    addVerbatim(list->commaToken, list->commaToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::PropertyNameAndValueList *)
{
    endWrite("<PropertyNameAndValueList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NestedExpression *expression)
{
    write("<NestedExpression>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NestedExpression *)
{
    endWrite("<NestedExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ArrayMemberExpression *expression)
{
    write("<ArrayMemberExpression>");
    QDeclarativeJS::AST::Node::accept(expression->base, this);
    addVerbatim(expression->lbracketToken);
    QDeclarativeJS::AST::Node::accept(expression->expression, this);
    addVerbatim(expression->rbracketToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ArrayMemberExpression *)
{
    endWrite("<ArrayMemberExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FieldMemberExpression *expression)
{
    write("<FieldMemberExpression>");
    QDeclarativeJS::AST::Node::accept(expression->base, this);
    addVerbatim(expression->dotToken);
    addMarkedUpToken(expression->identifierToken, QLatin1String("name"));
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FieldMemberExpression *expression)
{
    endWrite("<FieldMemberExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NewMemberExpression *expression)
{
    write("<NewMemberExpression>");
    addVerbatim(expression->newToken);
    QDeclarativeJS::AST::Node::accept(expression->base, this);
    addVerbatim(expression->lparenToken);
    QDeclarativeJS::AST::Node::accept(expression->arguments, this);
    addVerbatim(expression->rparenToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NewMemberExpression *expression)
{
    endWrite("<NewMemberExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NewExpression *expression)
{
    write("<NewExpression>");
    addVerbatim(expression->newToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NewExpression *)
{
    endWrite("<NewExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::CallExpression *expression)
{
    write("<CallExpression>");
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::CallExpression *)
{
    endWrite("<CallExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ArgumentList *list)
{
    write("<ArgumentList>");
    addVerbatim(list->commaToken, list->commaToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ArgumentList *)
{
    endWrite("<ArgumentList>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::PostIncrementExpression *expression)
{
    write("<PostIncrementExpression>");
    addVerbatim(expression->incrementToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::PostIncrementExpression *)
{
    endWrite("<PostIncrementExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::PostDecrementExpression *expression)
{
    write("<PostDecrementExpression>");
    addVerbatim(expression->decrementToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::PostDecrementExpression *)
{
    endWrite("<PostDecrementExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::DeleteExpression *expression)
{
    write("<DeleteExpression>");
    addVerbatim(expression->deleteToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::DeleteExpression *)
{
    endWrite("<DeleteExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::VoidExpression *expression)
{
    write("<VoidExpression>");
    addVerbatim(expression->voidToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::VoidExpression *)
{
    endWrite("<VoidExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::TypeOfExpression *expression)
{
    write("<TypeOfExpression>");
    addVerbatim(expression->typeofToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::TypeOfExpression *)
{
    endWrite("<TypeOfExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::PreIncrementExpression *expression)
{
    write("<PreIncrementExpression>");
    addVerbatim(expression->incrementToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::PreIncrementExpression *)
{
    endWrite("<PreIncrementExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::PreDecrementExpression *expression)
{
    write("<PreDecrementExpression>");
    addVerbatim(expression->decrementToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::PreDecrementExpression *)
{
    endWrite("<PreDecrementExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UnaryPlusExpression *expression)
{
    write("<UnaryPlusExpression>");
    addVerbatim(expression->plusToken);
    return true;
}
 
void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UnaryPlusExpression *)
{
    endWrite("<UnaryPlusExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UnaryMinusExpression *expression)
{
    write("<UnaryMinusExpression>");
    addVerbatim(expression->minusToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UnaryMinusExpression *)
{
    endWrite("<UnaryMinusExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::TildeExpression *expression)
{
    write("<TildeExpression>");
    addVerbatim(expression->tildeToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::TildeExpression *)
{
    endWrite("<TildeExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::NotExpression *expression)
{
    write("<NotExpression>");
    addVerbatim(expression->notToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::NotExpression *)
{
    endWrite("<NotExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::BinaryExpression *expression)
{
    write("<BinaryExpression>");
    QDeclarativeJS::AST::Node::accept(expression->left, this);
    addMarkedUpToken(expression->operatorToken, QLatin1String("op"));
    QDeclarativeJS::AST::Node::accept(expression->right, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::BinaryExpression *)
{
    endWrite("<BinaryExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ConditionalExpression *expression)
{
    write("<ConditionalExpression>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ConditionalExpression *)
{
    endWrite("<ConditionalExpression>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Expression *expression)
{
    write("<Expression>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Expression *)
{
    endWrite("<Expression>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Block *block)
{
    write("<Block>");
    addVerbatim(block->lbraceToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Block *block)
{
    addVerbatim(block->rbraceToken);
    endWrite("<Block>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::StatementList *)
{
    write("<StatementList>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::StatementList *)
{
    endWrite("<StatementList");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::VariableStatement *statement)
{
    write("<VariableStatement>");
    addVerbatim(statement->declarationKindToken);
    QDeclarativeJS::AST::Node::accept(statement->declarations, this);
    addVerbatim(statement->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::VariableStatement *)
{
    endWrite("<VariableStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::VariableDeclarationList *list)
{
    write("<VariableDeclarationList>");
    for (QDeclarativeJS::AST::VariableDeclarationList *it = list; it; it = it->next) {
        QDeclarativeJS::AST::Node::accept(it->declaration, this);
        addVerbatim(it->commaToken);
    }
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::VariableDeclarationList *)
{
    endWrite("<VariableDeclarationList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::VariableDeclaration *declaration)
{
    write("<VariableDeclaration>");
    addMarkedUpToken(declaration->identifierToken, QLatin1String("name"));
    QDeclarativeJS::AST::Node::accept(declaration->expression, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::VariableDeclaration *)
{
    endWrite("<VariableDeclaration>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::EmptyStatement *statement)
{
    write("<EmptyStatement>");
    addVerbatim(statement->semicolonToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::EmptyStatement *)
{
    endWrite("<EmptyStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ExpressionStatement *)
{
    write("<ExpressionStatement>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ExpressionStatement *statement)
{
    addVerbatim(statement->semicolonToken);
    endWrite("<ExpressionStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::IfStatement *statement)
{
    write("<IfStatement>");
    addMarkedUpToken(statement->ifToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->ok, this);
    //addMarkedUpToken(statement->elseToken, QLatin1String("keyword"));
    //addVerbatim(statement->elseToken); ### this token referred to the wrong source text for some reason
    QDeclarativeJS::AST::Node::accept(statement->ko, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::IfStatement *)
{
    endWrite("<IfStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::DoWhileStatement *statement)
{
    write("<DoWhileStatement>");
    addMarkedUpToken(statement->doToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    addMarkedUpToken(statement->whileToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    addVerbatim(statement->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::DoWhileStatement *)
{
    endWrite("<DoWhileStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::WhileStatement *statement)
{
    write("<WhileStatement>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::WhileStatement *)
{
    endWrite("<WhileStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ForStatement *statement)
{
    write("<ForStatement>");
    addMarkedUpToken(statement->forToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    QDeclarativeJS::AST::Node::accept(statement->initialiser, this);
    addVerbatim(statement->firstSemicolonToken);
    QDeclarativeJS::AST::Node::accept(statement->condition, this);
    addVerbatim(statement->secondSemicolonToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ForStatement *)
{
    endWrite("<ForStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::LocalForStatement *statement)
{
    write("<LocalForStatement>");
    addMarkedUpToken(statement->forToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    addMarkedUpToken(statement->varToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(statement->declarations, this);
    addVerbatim(statement->firstSemicolonToken);
    QDeclarativeJS::AST::Node::accept(statement->condition, this);
    addVerbatim(statement->secondSemicolonToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::LocalForStatement *)
{
    endWrite("<LocalForStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ForEachStatement *statement)
{
    write("<ForEachStatement>");
    addMarkedUpToken(statement->forToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    QDeclarativeJS::AST::Node::accept(statement->initialiser, this);
    addVerbatim(statement->inToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ForEachStatement *)
{
    endWrite("<ForEachStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::LocalForEachStatement *statement)
{
    write("<LocalForEachStatement>");
    addMarkedUpToken(statement->forToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    addMarkedUpToken(statement->varToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(statement->declaration, this);
    addVerbatim(statement->inToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::LocalForEachStatement *)
{
    endWrite("<LocalForEachStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ContinueStatement *statement)
{
    write("<ContinueStatement>");
    addMarkedUpToken(statement->continueToken, QLatin1String("keyword"));
    addMarkedUpToken(statement->identifierToken, QLatin1String("name"));
    addVerbatim(statement->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ContinueStatement *)
{
    endWrite("<ContinueStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::BreakStatement *statement)
{
    write("<BreakStatement>");
    addMarkedUpToken(statement->breakToken, QLatin1String("keyword"));
    addMarkedUpToken(statement->identifierToken, QLatin1String("name"));
    addVerbatim(statement->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::BreakStatement *)
{
    endWrite("<BreakStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ReturnStatement *statement)
{
    write("<ReturnStatement>");
    addMarkedUpToken(statement->returnToken, QLatin1String("keyword"));
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ReturnStatement *statement)
{
    addVerbatim(statement->semicolonToken);
    endWrite("<ReturnStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::WithStatement *statement)
{
    write("<WithStatement>");
    addMarkedUpToken(statement->withToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    addVerbatim(statement->rparenToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::WithStatement *)
{
    endWrite("<WithStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::CaseBlock *block)
{
    write("<CaseBlock>");
    addVerbatim(block->lbraceToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::CaseBlock *block)
{
    endWrite("<CaseBlock>");
    addVerbatim(block->rbraceToken, block->rbraceToken);
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::SwitchStatement *statement)
{
    write("<SwitchStatement>");
    addMarkedUpToken(statement->switchToken, QLatin1String("keyword"));
    addVerbatim(statement->lparenToken);
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->rparenToken);
    QDeclarativeJS::AST::Node::accept(statement->block, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::SwitchStatement *statement)
{
    endWrite("<SwitchStatement>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::CaseClauses *)
{
    write("<CaseClauses>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::CaseClauses *)
{
    endWrite("<CaseClauses>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::CaseClause *clause)
{
    write("<CaseClause>");
    addMarkedUpToken(clause->caseToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(clause->expression, this);
    addVerbatim(clause->colonToken);
    QDeclarativeJS::AST::Node::accept(clause->statements, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::CaseClause *clause)
{
    endWrite("<CaseClause>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::DefaultClause *clause)
{
    write("<DefaultClause>");
    addMarkedUpToken(clause->defaultToken, QLatin1String("keyword"));
    addVerbatim(clause->colonToken, clause->colonToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::DefaultClause *)
{
    endWrite("<DefaultClause>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::LabelledStatement *statement)
{
    write("<LabelledStatement>");
    addMarkedUpToken(statement->identifierToken, QLatin1String("name"));
    addVerbatim(statement->colonToken);
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::LabelledStatement *)
{
    endWrite("<LabelledStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::ThrowStatement *statement)
{
    write("<ThrowStatement>");
    addMarkedUpToken(statement->throwToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(statement->expression, this);
    addVerbatim(statement->semicolonToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::ThrowStatement *)
{
    endWrite("<ThrowStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Catch *c)
{
    write("<Catch>");
    addMarkedUpToken(c->catchToken, QLatin1String("keyword"));
    addVerbatim(c->lparenToken);
    addMarkedUpToken(c->identifierToken, QLatin1String("name"));
    addVerbatim(c->rparenToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Catch *)
{
    endWrite("<Catch>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Finally *f)
{
    write("<Finally>");
    addMarkedUpToken(f->finallyToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(f->statement, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Finally *)
{
    endWrite("<Finally>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::TryStatement *statement)
{
    write("<TryStatement>");
    addMarkedUpToken(statement->tryToken, QLatin1String("keyword"));
    QDeclarativeJS::AST::Node::accept(statement->statement, this);
    QDeclarativeJS::AST::Node::accept(statement->catchExpression, this);
    QDeclarativeJS::AST::Node::accept(statement->finallyExpression, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::TryStatement *)
{
    endWrite("<TryStatement>");
}


bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FunctionExpression *expression)
{
    write("<FunctionExpression>");
    addMarkedUpToken(expression->functionToken, QLatin1String("keyword"));
    addMarkedUpToken(expression->identifierToken, QLatin1String("name"));
    addVerbatim(expression->lparenToken);
    QDeclarativeJS::AST::Node::accept(expression->formals, this);
    addVerbatim(expression->rparenToken);
    addVerbatim(expression->lbraceToken);
    QDeclarativeJS::AST::Node::accept(expression->body, this);
    addVerbatim(expression->rbraceToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FunctionExpression *expression)
{
    endWrite("<FunctionExpression>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FunctionDeclaration *declaration)
{
    write("<FunctionDeclaration>");
    addMarkedUpToken(declaration->functionToken, QLatin1String("keyword"));
    addMarkedUpToken(declaration->identifierToken, QLatin1String("name"));
    addVerbatim(declaration->lparenToken);
    QDeclarativeJS::AST::Node::accept(declaration->formals, this);
    addVerbatim(declaration->rparenToken);
    addVerbatim(declaration->lbraceToken);
    QDeclarativeJS::AST::Node::accept(declaration->body, this);
    addVerbatim(declaration->rbraceToken);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FunctionDeclaration *)
{
    endWrite("<FunctionDeclaration>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FormalParameterList *list)
{
    write("<FormalParameterList>");
    addVerbatim(list->commaToken);
    addMarkedUpToken(list->identifierToken, QLatin1String("name"));
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FormalParameterList *)
{
    endWrite("<FormalParameterList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FunctionBody *body)
{
    write("<FunctionBody>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FunctionBody *body)
{
    endWrite("<FunctionBody>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::DebuggerStatement *statement)
{
    write("<DebuggerStatement>");
    addVerbatim(statement->debuggerToken);
    addVerbatim(statement->semicolonToken);
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::DebuggerStatement *)
{
    endWrite("<DebuggerStatement>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::FunctionSourceElement *)
{
    write("<FunctionSourceElement>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::FunctionSourceElement *)
{
    endWrite("<FunctionSourceElement>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::StatementSourceElement *)
{
    write("<StatementSourceElement>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::StatementSourceElement *)
{
    endWrite("<StatementSourceElement>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiObjectDefinition *definition)
{
    write("<UiObjectDefinition>");
    QDeclarativeJS::AST::Node::accept(definition->qualifiedTypeNameId, this);
    QDeclarativeJS::AST::Node::accept(definition->initializer, this);
    return false;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiObjectDefinition *)
{
    endWrite("<UiObjectDefinition>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiImportList *)
{
    write("<UiImportList>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiImportList *)
{
    endWrite("<UiImportList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiObjectMemberList *)
{
    write("<UiObjectMemberList>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiObjectMemberList *)
{
    endWrite("<UiObjectMemberList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::UiFormalList *)
{
    write("<UiFormalList>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::UiFormalList *)
{
    endWrite("<UiFormalList>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::Program *)
{
    write("<Program>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::Program *)
{
    endWrite("<Program>");
}

bool QmlMarkupVisitor::visit(QDeclarativeJS::AST::SourceElements *)
{
    write("<SourceElements>");
    return true;
}

void QmlMarkupVisitor::endVisit(QDeclarativeJS::AST::SourceElements *)
{
    endWrite("<SourceElements>");
}

QT_END_NAMESPACE
