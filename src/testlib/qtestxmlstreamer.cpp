/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qtestxmlstreamer.h"
#include "qtestelement.h"
#include "qtestelementattribute.h"

#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

#include <string.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

QTestXmlStreamer::QTestXmlStreamer()
    :QTestBasicStreamer()
{
}

QTestXmlStreamer::~QTestXmlStreamer()
{}

void QTestXmlStreamer::formatStart(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted)
        return;

    switch(element->elementType()){
    case QTest::LET_TestCase: {
        char quotedTf[950];
        QXmlTestLogger::xmlQuote(quotedTf, element->attributeValue(QTest::AI_Name),
            sizeof(quotedTf));

        QTest::qt_snprintf(formatted, 1024, "<TestFunction name=\"%s\">\n", quotedTf);
        break;
    }
    case QTest::LET_Failure: {
        char cdataDesc[800];
        QXmlTestLogger::xmlCdata(cdataDesc, element->attributeValue(QTest::AI_Description),
            sizeof(cdataDesc));

        char location[100];
        char quotedFile[70];
        QXmlTestLogger::xmlQuote(quotedFile, element->attributeValue(QTest::AI_File),
            sizeof(quotedFile));

        QTest::qt_snprintf(location, sizeof(location), "%s=\"%s\" %s=\"%s\"",
                           element->attributeName(QTest::AI_File),
                           quotedFile,
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line));

        if (element->attribute(QTest::AI_Tag)) {
            char cdataTag[100];
            QXmlTestLogger::xmlCdata(cdataTag, element->attributeValue(QTest::AI_Tag),
                sizeof(cdataTag));
            QTest::qt_snprintf(formatted, 1024, "<Incident type=\"%s\" %s>\n"
                "    <DataTag><![CDATA[%s]]></DataTag>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Incident>\n", element->attributeValue(QTest::AI_Result),
                location, cdataTag, cdataDesc);
        }
        else {
            QTest::qt_snprintf(formatted, 1024, "<Incident type=\"%s\" %s>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Incident>\n", element->attributeValue(QTest::AI_Result),
                location, cdataDesc);
        }
        break;
    }
    case QTest::LET_Error: {
        // assuming type and attribute names don't need quoting
        char quotedFile[128];
        char cdataDesc[700];
        QXmlTestLogger::xmlQuote(quotedFile, element->attributeValue(QTest::AI_File),
            sizeof(quotedFile));
        QXmlTestLogger::xmlCdata(cdataDesc, element->attributeValue(QTest::AI_Description),
            sizeof(cdataDesc));

        QTest::qt_snprintf(formatted, 1024, "<Message type=\"%s\" %s=\"%s\" %s=\"%s\">\n    <Description><![CDATA[%s]]></Description>\n</Message>\n",
                           element->attributeValue(QTest::AI_Type),
                           element->attributeName(QTest::AI_File),
                           quotedFile,
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line),
                           cdataDesc);
        break;
    }
    case QTest::LET_Benchmark: {
        // assuming value and iterations don't need quoting
        char quotedMetric[256];
        char quotedTag[256];
        QXmlTestLogger::xmlQuote(quotedMetric, element->attributeValue(QTest::AI_Metric),
            sizeof(quotedMetric));
        QXmlTestLogger::xmlQuote(quotedTag, element->attributeValue(QTest::AI_Tag),
            sizeof(quotedTag));

        QTest::qt_snprintf(formatted, 1024, "<BenchmarkResult %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n",
                           element->attributeName(QTest::AI_Metric),
                           quotedMetric,
                           element->attributeName(QTest::AI_Tag),
                           quotedTag,
                           element->attributeName(QTest::AI_Value),
                           element->attributeValue(QTest::AI_Value),
                           element->attributeName(QTest::AI_Iterations),
                           element->attributeValue(QTest::AI_Iterations) );
        break;
    }
    default:
        QTest::qt_snprintf(formatted, 10, "");
    }
}

void QTestXmlStreamer::formatEnd(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase) {
        QTest::qt_snprintf(formatted, 1024, "</TestFunction>\n");
    }
    else
        QTest::qt_snprintf(formatted, 10, "");
}

void QTestXmlStreamer::formatBeforeAttributes(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase && element->attribute(QTest::AI_Result)){
        char buf[900];
        char quotedFile[700];
        QXmlTestLogger::xmlQuote(quotedFile, element->attributeValue(QTest::AI_File),
            sizeof(quotedFile));

        QTest::qt_snprintf(buf, sizeof(buf), "%s=\"%s\" %s=\"%s\"",
                           element->attributeName(QTest::AI_File),
                           quotedFile,
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line));

        if( !element->childElements() ) {
            QTest::qt_snprintf(formatted, 1024, "<Incident type=\"%s\" %s/>\n",
                               element->attributeValue(QTest::AI_Result), buf);
        }
        else {
            QTest::qt_snprintf(formatted, 10, "");
        }
    }else{
        QTest::qt_snprintf(formatted, 10, "");
    }
}

void QTestXmlStreamer::output(QTestElement *element) const
{
    char buf[1024];
    char quotedTc[800];
    QXmlTestLogger::xmlQuote(quotedTc, QTestResult::currentTestObjectName(), sizeof(quotedTc));

    QTest::qt_snprintf(buf, sizeof(buf), "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<TestCase name=\"%s\">\n",
                       quotedTc);
    outputString(buf);

    QTest::qt_snprintf(buf, sizeof(buf), "<Environment>\n    <QtVersion>%s</QtVersion>\n    <QTestVersion>%s</QTestVersion>\n",
                       qVersion(), QTEST_VERSION_STR );
    outputString(buf);

    QTest::qt_snprintf(buf, sizeof(buf), "</Environment>\n");
    outputString(buf);

    QTestBasicStreamer::output(element);

    QTest::qt_snprintf(buf, sizeof(buf), "</TestCase>\n");
    outputString(buf);
}

QT_END_NAMESPACE

