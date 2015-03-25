/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the qt3to4 porting application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "translationunit.h"

QT_BEGIN_NAMESPACE

using namespace TokenEngine;
using namespace CodeModel;
using namespace TokenStreamAdapter;

TranslationUnit::TranslationUnit()
{
    TokenSectionSequence empty;
    d = new TranslationUnitData(empty);
}

TranslationUnit::TranslationUnit(const TokenEngine::TokenSectionSequence &tokens)
{ d = new TranslationUnitData(tokens); }

TokenSectionSequence TranslationUnit::tokens() const
{ return d->tokens; }

void TranslationUnit::setCodeModel(NamespaceScope *globalScope)
{ d->globalScope = globalScope; }

NamespaceScope *TranslationUnit::codeModel()
{ return d->globalScope; }

TypedPool<CodeModel::Item> *TranslationUnit::codeModelMemoryPool()
{ return &d->codeModelMemoryPool; }



/*
    Performs C++ parsing and semantic analysis on a translation unit.
    Returns a TranslationUnit, which contains all the data.
*/
TranslationUnit TranslationUnitAnalyzer::analyze
        (const TokenSectionSequence &translationUnitTokens, int targetMaxASTNodes)
{
    TranslationUnit translationUnit(translationUnitTokens);
    CodeModel::NamespaceScope *codeModel =
        CodeModel::Create<CodeModel::NamespaceScope>(translationUnit.codeModelMemoryPool());
    translationUnit.setCodeModel(codeModel);

    // run lexical analysis
    QVector<QT_PREPEND_NAMESPACE(Type)> typeList = lexer.lex(translationUnitTokens);
    TokenStreamAdapter::TokenStream tokenStream(translationUnitTokens, typeList);

    Semantic semantic(codeModel, &tokenStream, translationUnit.codeModelMemoryPool());

    // parse and run semantic on the translation unit
    bool done = false;
    while(!done) {
        pool p;
        TranslationUnitAST *node = parser.parse(&tokenStream, &p, targetMaxASTNodes, done);
        semantic.parseTranslationUnit(node);
    }

    return translationUnit;
}

QT_END_NAMESPACE
