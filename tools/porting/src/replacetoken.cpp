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

#include "replacetoken.h"
#include "tokenreplacements.h"
#include <QByteArray>

QT_BEGIN_NAMESPACE

/*
    Add an entry to the tokenRuleLookup map for each token replacement rule.
*/
ReplaceToken::ReplaceToken(const QList<TokenReplacement*> &tokenReplacementRules)
:tokenReplacementRules(tokenReplacementRules)
{
    foreach (TokenReplacement* rep, tokenReplacementRules) {
        QByteArray key = rep->getReplaceKey();
        if(!key.isEmpty()) {
            tokenRuleLookup.insert(key, rep);
        }
    }
}

TextReplacements ReplaceToken::getTokenTextReplacements(const TokenEngine::TokenContainer &container)
{
    TextReplacements textReplacements;

    int t=0;
    const int numTokens = container.count();
    while(t < numTokens) {
        QByteArray tokenText = container.text(t);
        bool changed = false;

        if(isPreprocessorDirective(tokenText)) {
            foreach(TokenReplacement *tokenReplacementRule, tokenReplacementRules) {
                if(!changed)
                    changed = tokenReplacementRule->doReplace(container, t, textReplacements);
                if(changed)
                    break;
            }
        } else if (isInterestingToken(tokenText.trimmed())) {
            foreach (TokenReplacement* value, tokenRuleLookup.values(tokenText)) {
                changed = value->doReplace(container, t, textReplacements);
                if(changed) {
                    goto end;
                }
            }
        }
    end:
        ++t;
    }
    return textReplacements;
}

bool ReplaceToken::isInterestingToken(const QByteArray &text)
{
    return !(text.isEmpty() || text==";" || text=="(" || text==")" || text=="{" || text=="}" || text=="="
            || text=="+=" || text=="-=" || text=="if" || text=="then" || text=="else"
    );
}

bool ReplaceToken::isPreprocessorDirective(const QByteArray &token)
{
    return (token[0]=='#');
}

QT_END_NAMESPACE
