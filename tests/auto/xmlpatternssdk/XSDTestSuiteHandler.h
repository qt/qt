/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the autotests of the Qt Toolkit.
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

#ifndef PatternistSDK_XSDTestSuiteHandler_H
#define PatternistSDK_XSDTestSuiteHandler_H

#include <QUrl>
#include <QXmlDefaultHandler>

#include "ExternalSourceLoader.h"
#include "TestSuite.h"
#include "XQTSTestCase.h"

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

namespace QPatternistSDK
{
    class TestBaseLine;
    class TestGroup;
    class XSDTSTestCase;

    /**
     * @short Creates a TestSuite from the XSD Test Suite.
     *
     * The created TestSuite can be retrieved via testSuite().
     *
     * @note XSDTestSuiteHandler assumes the XML is valid by having been validated
     * against the W3C XML Schema. It has no safety checks for that the XML format
     * is correct but is hard coded for it. Thus, the behavior is undefined if
     * the XML is invalid.
     *
     * @ingroup PatternistSDK
     * @author Tobias Koenig <tobias.koenig@nokia.com>
     */
    class Q_PATTERNISTSDK_EXPORT XSDTestSuiteHandler : public QXmlDefaultHandler
    {
    public:
        /**
         * @param catalogFile the URI for the catalog file being parsed. This
         * URI is used for creating absolute URIs for files mentioned in
         * the catalog with relative URIs.
         * @param useExclusionList whether excludeTestGroups.txt should be used to ignore
         * test groups when loading
         */
        XSDTestSuiteHandler(const QUrl &catalogFile);
        virtual bool characters(const QString &ch);

        virtual bool endElement(const QString &namespaceURI,
                                const QString &localName,
                                const QString &qName);
        virtual bool startElement(const QString &namespaceURI,
                                  const QString &localName,
                                  const QString &qName,
                                  const QXmlAttributes &atts);

        virtual TestSuite *testSuite() const;

    private:
        TestSuite* m_ts;
        const QUrl m_catalogFile;

        TestGroup* m_topLevelGroup;
        TestGroup* m_currentTestSet;
        TestGroup* m_currentTestGroup;
        XSDTSTestCase* m_currentTestCase;
        bool m_inSchemaTest;
        bool m_inInstanceTest;
        bool m_inTestGroup;
        bool m_inDescription;
        bool m_schemaBlacklisted;
        QString m_documentation;
        QString m_currentSchemaLink;
        int m_counter;
        QSet<QString> m_blackList;
    };
}

QT_END_NAMESPACE
QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
