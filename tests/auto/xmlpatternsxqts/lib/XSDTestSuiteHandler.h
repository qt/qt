/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Patternist project on Trolltech Labs.
**
** $TROLLTECH_GPL_LICENSE$
**
***************************************************************************
*/

#ifndef PatternistSDK_XSDTestSuiteHandler_H
#define PatternistSDK_XSDTestSuiteHandler_H

#include <QUrl>
#include <QXmlDefaultHandler>

#include "ExternalSourceLoader.h"
#include "TestSuite.h"
#include "XQTSTestCase.h"

QT_BEGIN_HEADER

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
     * @author Tobias Koenig <tobias.koenig@trolltech.com>
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

QT_END_HEADER

#endif
// vim: et:ts=4:sw=4:sts=4
