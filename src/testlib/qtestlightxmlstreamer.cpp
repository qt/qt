/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#include "qtestlightxmlstreamer.h"
#include "qtestelement.h"
#include "qtestelementattribute.h"
#include "qtestlogger_p.h"

#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

#include <string.h>

QT_BEGIN_NAMESPACE

QTestLightXmlStreamer::QTestLightXmlStreamer()
    :QTestBasicStreamer()
{
}

QTestLightXmlStreamer::~QTestLightXmlStreamer()
{}

void QTestLightXmlStreamer::formatStart(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    switch(element->elementType()){
    case QTest::LET_TestCase: {
        QTestCharBuffer quotedTf;
        QXmlTestLogger::xmlQuote(&quotedTf, element->attributeValue(QTest::AI_Name));

        QTest::qt_asprintf(formatted, "<TestFunction name=\"%s\">\n", quotedTf.constData());
        break;
    }
    case QTest::LET_Failure: {
        QTestCharBuffer cdataDesc;
        QXmlTestLogger::xmlCdata(&cdataDesc, element->attributeValue(QTest::AI_Description));

        QTest::qt_asprintf(formatted, "    <Description><![CDATA[%s]]></Description>\n",
                           cdataDesc.constData());
         break;
    }
    case QTest::LET_Error: {
        // assuming type and attribute names don't need quoting
        QTestCharBuffer quotedFile;
        QTestCharBuffer cdataDesc;
        QXmlTestLogger::xmlQuote(&quotedFile, element->attributeValue(QTest::AI_File));
        QXmlTestLogger::xmlCdata(&cdataDesc, element->attributeValue(QTest::AI_Description));

        QTest::qt_asprintf(formatted, "<Message type=\"%s\" %s=\"%s\" %s=\"%s\">\n    <Description><![CDATA[%s]]></Description>\n</Message>\n",
                           element->attributeValue(QTest::AI_Type),
                           element->attributeName(QTest::AI_File),
                           quotedFile.constData(),
                           element->attributeName(QTest::AI_Line),
                           element->attributeValue(QTest::AI_Line),
                           cdataDesc.constData());
        break;
    }
    case QTest::LET_Benchmark: {
        // assuming value and iterations don't need quoting
        QTestCharBuffer quotedMetric;
        QTestCharBuffer quotedTag;
        QXmlTestLogger::xmlQuote(&quotedMetric, element->attributeValue(QTest::AI_Metric));
        QXmlTestLogger::xmlQuote(&quotedTag, element->attributeValue(QTest::AI_Tag));

        QTest::qt_asprintf(formatted, "<BenchmarkResult %s=\"%s\" %s=\"%s\" %s=\"%s\" %s=\"%s\" />\n",
                           element->attributeName(QTest::AI_Metric),
                           quotedMetric.constData(),
                           element->attributeName(QTest::AI_Tag),
                           quotedTag.constData(),
                           element->attributeName(QTest::AI_Value),
                           element->attributeValue(QTest::AI_Value),
                           element->attributeName(QTest::AI_Iterations),
                           element->attributeValue(QTest::AI_Iterations) );
        break;
    }
    default:
        formatted->data()[0] = '\0';
    }
}

void QTestLightXmlStreamer::formatEnd(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase) {
        if( element->attribute(QTest::AI_Result) && element->childElements())
            QTest::qt_asprintf(formatted, "</Incident>\n</TestFunction>\n");
        else
            QTest::qt_asprintf(formatted, "</TestFunction>\n");
    } else {
        formatted->data()[0] = '\0';
    }
}

void QTestLightXmlStreamer::formatBeforeAttributes(const QTestElement *element, QTestCharBuffer *formatted) const
{
    if(!element || !formatted)
        return;

    if (element->elementType() == QTest::LET_TestCase && element->attribute(QTest::AI_Result)) {
        QTestCharBuffer buf;
        QTestCharBuffer quotedFile;
        QXmlTestLogger::xmlQuote(&quotedFile, element->attributeValue(QTest::AI_File));

        QTest::qt_asprintf(&buf, "%s=\"%s\" %s=\"%s\"",
                element->attributeName(QTest::AI_File),
                quotedFile.constData(),
                element->attributeName(QTest::AI_Line),
                element->attributeValue(QTest::AI_Line));

        if( !element->childElements() )
            QTest::qt_asprintf(formatted, "<Incident type=\"%s\" %s/>\n",
                    element->attributeValue(QTest::AI_Result), buf.constData());
        else
            QTest::qt_asprintf(formatted, "<Incident type=\"%s\" %s>\n",
                    element->attributeValue(QTest::AI_Result), buf.constData());
    } else {
        formatted->data()[0] = '\0';
    }
}

void QTestLightXmlStreamer::output(QTestElement *element) const
{
    QTestCharBuffer buf;
    if (logger()->hasRandomSeed()) {
        QTest::qt_asprintf(&buf, "<Environment>\n    <QtVersion>%s</QtVersion>\n    <QTestVersion>%s</QTestVersion>\n    <RandomSeed>%d</RandomSeed>\n",
                       qVersion(), QTEST_VERSION_STR, logger()->randomSeed() );
    } else {
        QTest::qt_asprintf(&buf, "<Environment>\n    <QtVersion>%s</QtVersion>\n    <QTestVersion>%s</QTestVersion>\n",
                       qVersion(), QTEST_VERSION_STR );
    }
    outputString(buf.constData());

    QTest::qt_asprintf(&buf, "</Environment>\n");
    outputString(buf.constData());

    QTestBasicStreamer::output(element);
}

QT_END_NAMESPACE

