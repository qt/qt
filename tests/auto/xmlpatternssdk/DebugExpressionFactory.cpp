/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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

#include <QStringList>
#include <QVariant>
#include <QtDebug>
#include <QXmlNamePool>

#include "qfunctionfactorycollection_p.h"

#include "ASTItem.h"
#include "ExpressionInfo.h"
#include "ExpressionNamer.h"
#include "Global.h"

#include "DebugExpressionFactory.h"

using namespace QPatternistSDK;
using namespace QPatternist;

static const QPatternist::ExpressionVisitor::Ptr namer(new ExpressionNamer());

QStringList DebugExpressionFactory::availableFunctionSignatures()
{
    const QPatternist::FunctionFactory::Ptr factory(QPatternist::FunctionFactoryCollection::xpath20Factory(Global::namePool()));
    const QPatternist::FunctionSignature::Hash signs(factory->functionSignatures());
    const QPatternist::FunctionSignature::Hash::const_iterator end(signs.constEnd());
    QPatternist::FunctionSignature::Hash::const_iterator it(signs.constBegin());
    QStringList retval;

    while(it != end)
    {
        retval << it.value()->displayName(Global::namePool());
        ++it;
    }

    return retval;
}

ASTItem *DebugExpressionFactory::buildASTTree(const QPatternist::Expression::Ptr &expr,
                                              ASTItem *parent,
                                              const QPatternist::SequenceType::Ptr &reqType)
{
    Q_ASSERT(expr);
    const QPatternist::ExpressionVisitorResult::Ptr exprInfo(expr->accept(namer));
    Q_ASSERT(exprInfo);
    const ExpressionInfo *const constExprInfo = static_cast<const ExpressionInfo *>(exprInfo.data());
    const QString name(constExprInfo->first);
    const QString details(constExprInfo->second);
    const QString rType(reqType ? reqType->displayName(Global::namePool()) : QLatin1String("Not specified"));

    /* ---------- Handle its staticType() -------- */
    const QPatternist::SequenceType::Ptr type(expr->staticType());
    QString seqType;

    if(type)
        seqType = type->displayName(Global::namePool());
    else
        seqType = QLatin1String("no type, null pointer returned");
    /* ------------------------------------------- */

    ASTItem *const node = new ASTItem(parent, name, details, seqType, rType);

    /* ------------ Handle child nodes ----------- */
    const QPatternist::Expression::List children(expr->operands());
    QPatternist::Expression::List::const_iterator it(children.constBegin());
    const QPatternist::Expression::List::const_iterator end(children.constEnd());

    const QPatternist::SequenceType::List reqTypes(expr->expectedOperandTypes());
    const QPatternist::SequenceType::List::const_iterator typeEnd(reqTypes.constEnd());
    QPatternist::SequenceType::List::const_iterator typeIt(reqTypes.constBegin());
    QPatternist::SequenceType::Ptr t;

    for(; it != end; ++it)
    {
        if(typeIt != typeEnd)
        {
            t = *typeIt;
            ++typeIt;
        }

        node->appendChild(buildASTTree(*it, node, t));
    }
    /* ------------------------------------------- */

    return node;
}

QPatternist::Expression::Ptr
DebugExpressionFactory::createExpression(QIODevice *const expr,
                                         const QPatternist::StaticContext::Ptr &context,
                                         const QXmlQuery::QueryLanguage lang,
                                         const QPatternist::SequenceType::Ptr &requiredType,
                                         const QUrl &baseURI,
                                         const QXmlName &initialTemplateName)
{
    /* Create the root node. */
    m_ast = new ASTItem(0, QString());

    return ExpressionFactory::createExpression(expr, context, lang, requiredType, baseURI, initialTemplateName);
}

void DebugExpressionFactory::processTreePass(const QPatternist::Expression::Ptr &expr,
                                             const CompilationStage stage)
{
    ASTItem *newChild = 0;

    switch(stage)
    {
        case QueryBodyInitial:
        {
            newChild = new ASTItem(m_ast, QLatin1String("Initial Build"));
            break;
        }
        case QueryBodyTypeCheck:
        {
            newChild = new ASTItem(m_ast, QLatin1String("Type Check"));
            break;
        }
        case QueryBodyCompression:
        {
            newChild = new ASTItem(m_ast, QLatin1String("Compression"));
            break;
        }
        case UserFunctionTypeCheck:
        {
            newChild = new ASTItem(m_ast, QLatin1String("User Function Type Check"));
            break;
        }
        case UserFunctionCompression:
        {
            newChild = new ASTItem(m_ast, QLatin1String("User Function Compression"));
            break;
        }
        case GlobalVariableTypeCheck:
        {
            newChild = new ASTItem(m_ast, QLatin1String("Global Variable Type Check"));
            break;
        }
    }

    Q_ASSERT(newChild);
    m_ast->appendChild(newChild);
    newChild->appendChild(buildASTTree(expr, newChild, QPatternist::SequenceType::Ptr()));
}

void DebugExpressionFactory::processTemplateRule(const Expression::Ptr &body,
                                                 const TemplatePattern::Ptr &pattern,
                                                 const QXmlName &mode,
                                                 const TemplateCompilationStage stage)
{
    const char * title;

    switch(stage)
    {
        case TemplateInitial:
        {
            title = "Initial Build";
            break;
        }
        case TemplateTypeCheck:
        {
            title = "Type Check";
            break;
        }
        case TemplateCompress:
        {
            title = "Compression";
            break;
        }
    }

    const QString modeName(Global::namePool()->displayName(mode));
    Q_ASSERT(title);
    ASTItem *const newChild = new ASTItem(m_ast, QLatin1String("T-Rule ")
                                                 + QLatin1String(title)
                                                 + QLatin1String(" mode: ")
                                                 + modeName
                                                 + QLatin1String(" priority: ")
                                                 + QString::number(pattern->priority()));
    m_ast->appendChild(newChild);

    newChild->appendChild(buildASTTree(pattern->matchPattern(), newChild, QPatternist::SequenceType::Ptr()));
    newChild->appendChild(buildASTTree(body, newChild, QPatternist::SequenceType::Ptr()));
}

void DebugExpressionFactory::processNamedTemplate(const QXmlName &name,
                                                  const Expression::Ptr &body,
                                                  const TemplateCompilationStage stage)
{
    const char * title;

    switch(stage)
    {
        case TemplateInitial:
        {
            title = "Named Template Initial Build";
            break;
        }
        case TemplateTypeCheck:
        {
            title = "Named Template Type Check";
            break;
        }
        case TemplateCompress:
        {
            title = "Named Template Compression";
            break;
        }
    }

    Q_ASSERT(title);
    ASTItem *const newChild = new ASTItem(m_ast, QLatin1String(title)
                                                 + QLatin1String(": ")
                                                 + Global::namePool()->displayName(name));

    m_ast->appendChild(newChild);
    newChild->appendChild(buildASTTree(body, newChild, QPatternist::SequenceType::Ptr()));
}

ASTItem *DebugExpressionFactory::astTree() const
{
    return m_ast;
}

// vim: et:ts=4:sw=4:sts=4
