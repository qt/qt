/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtXmlPatterns module of the Qt Toolkit.
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

#include "qtemplateinvoker_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

TemplateInvoker::TemplateInvoker(const WithParam::Hash &withParams,
                                 const QXmlName &name) : CallSite(name)
                                                       , m_withParams(withParams)
{
    const WithParam::Hash::const_iterator end(m_withParams.constEnd());

    for(WithParam::Hash::const_iterator it(m_withParams.constBegin()); it != end; ++it)
    {
        /* In the case of for instance:
         *  <xsl:with-param name="empty_seq" as="item()"/>
         *
         * we have no default expression. */
        Q_ASSERT(it.value()->sourceExpression());
        m_operands.append(it.value()->sourceExpression());
    }
}

Expression::Ptr TemplateInvoker::compress(const StaticContext::Ptr &context)
{
    /* CallSite::compress() may have changed our children, so update
     * our m_withParams. */
    const Expression::Ptr me(CallSite::compress(context));
    const WithParam::Hash::const_iterator end(m_withParams.constEnd());
    int exprIndex = -1;

    for(WithParam::Hash::const_iterator it(m_withParams.constBegin()); it != end; ++it)
    {
        if(it.value()->sourceExpression())
        {
            ++exprIndex;
            it.value()->setSourceExpression(m_operands.at(exprIndex));
        }
    }

    return me;
}

SequenceType::List TemplateInvoker::expectedOperandTypes() const
{
    SequenceType::List result;

    /* We don't return the type of the m_template->templateParameters(), we
     * return the type of the @c xsl:with-param first. @em After that, we
     * manually apply the parameter types in typeCheck(). */
    const WithParam::Hash::const_iterator end(m_withParams.constEnd());

    for(WithParam::Hash::const_iterator it(m_withParams.constBegin()); it != end; ++it)
    {
        /* We're not guaranteed to have a with-param, we may be using the
         * default value of the xsl:param. Tunnel parameters may also play
         * in. */
        result.append(it.value()->type());
    }

    return result;
}

QT_END_NAMESPACE

