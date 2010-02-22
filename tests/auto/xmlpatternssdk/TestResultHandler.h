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

#ifndef PatternistSDK_TestResultHandler_H
#define PatternistSDK_TestResultHandler_H

#include <QHash>
#include <QString>
#include <QtXml/QXmlDefaultHandler>

#include "TestResult.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    /**
     * @short Reads XML in the @c XQTSResult.xsd format, and provides access to
     * the reported results.
     *
     * @author Frans Englich <frans.englich@nokia.com>
     * @ingroup PatternistSDK
     */
    class Q_PATTERNISTSDK_EXPORT TestResultHandler : public QXmlDefaultHandler
    {
    public:
        /**
         * A hash where the key is the class's name, that is <tt>test-case/@@name</tt>,
         * and the value the test's result status.
         */
        typedef QHash<QString, TestResult::Status> Hash;

        /**
         * A hash mapping test-case names to their' comments.
         */
        typedef QHash<QString, QString> CommentHash;

        /**
         * Creates a TestResultHandler that will read @p file when run() is called.
         */
        TestResultHandler();

        /**
         * Performs finalization.
         */
        virtual bool endDocument();

        /**
         * Reads the <tt>test-case</tt> element and its attributes, everything else is ignored.
         */
        virtual bool startElement(const QString &namespaceURI,
                                  const QString &localName,
                                  const QString &qName,
                                  const QXmlAttributes &atts);
        /**
         * @note Do not reimplement this function.
         * @returns the result obtained from reading the XML file.
         */
        Hash result() const;

        CommentHash comments() const;

    private:
        Q_DISABLE_COPY(TestResultHandler)
        Hash        m_result;
        CommentHash m_comments;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
