/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtDebug>

#include "Global.h"

#include "TestResultHandler.h"

using namespace QPatternistSDK;

TestResultHandler::TestResultHandler()
{
    /* Fifteen thousand. When finished, we squeeze them. */
    m_result.reserve(15000);
    m_comments.reserve(1000); /* Comments are only used for stuff that crash, more or less. */
}

bool TestResultHandler::startElement(const QString &namespaceURI,
                                     const QString &localName,
                                     const QString &,
                                     const QXmlAttributes &atts)
{
    /* We only care about 'test-case', ignore everything else. */
    if(localName != QLatin1String("test-case") ||
       namespaceURI != Global::xqtsResultNS)
        return true;

    /* The 'comments' attribute is optional. */
    Q_ASSERT_X(atts.count() == 2 || atts.count() == 3, Q_FUNC_INFO,
               "The input appears to not conform to XQTSResult.xsd");

    Q_ASSERT_X(!m_result.contains(atts.value(QLatin1String("name"))),
               Q_FUNC_INFO,
               qPrintable(QString::fromLatin1("A test result for test case %1 has "
                                              "already been read(duplicate entry it seems).").arg(atts.value(QLatin1String("name")))));

    m_result.insert(atts.value(0), TestResult::statusFromString(atts.value(QLatin1String("result"))));

    return true;
}

bool TestResultHandler::endDocument()
{
    m_result.squeeze();
    m_comments.squeeze();
    return true;
}

TestResultHandler::Hash TestResultHandler::result() const
{
    return m_result;
}

TestResultHandler::CommentHash TestResultHandler::comments() const
{
    return m_comments;
}

// vim: et:ts=4:sw=4:sts=4
