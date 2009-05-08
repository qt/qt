/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <QtCore/qglobal.h>

#include "QtTest/private/qxmltestlogger_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qbenchmark_p.h"
#include "QtTest/qtestcase.h"

QT_BEGIN_NAMESPACE

namespace QTest {

    static const char* xmlMessageType2String(QAbstractTestLogger::MessageTypes type)
    {
        switch (type) {
        case QAbstractTestLogger::Warn:
            return "warn";
        case QAbstractTestLogger::QSystem:
            return "system";
        case QAbstractTestLogger::QDebug:
            return "qdebug";
        case QAbstractTestLogger::QWarning:
            return "qwarn";
        case QAbstractTestLogger::QFatal:
            return "qfatal";
        case QAbstractTestLogger::Skip:
            return "skip";
        case QAbstractTestLogger::Info:
            return "info";
        }
        return "??????";
    }

    static const char* xmlIncidentType2String(QAbstractTestLogger::IncidentTypes type)
    {
        switch (type) {
        case QAbstractTestLogger::Pass:
            return "pass";
        case QAbstractTestLogger::XFail:
            return "xfail";
        case QAbstractTestLogger::Fail:
            return "fail";
        case QAbstractTestLogger::XPass:
            return "xpass";
        }
        return "??????";
    }

}


QXmlTestLogger::QXmlTestLogger(XmlMode mode )
    :xmlmode(mode)
{

}

QXmlTestLogger::~QXmlTestLogger()
{
}

void QXmlTestLogger::startLogging()
{
    QAbstractTestLogger::startLogging();
    char buf[1024];

    if (xmlmode == QXmlTestLogger::Complete) {
        char quotedTc[900];
        xmlQuote(quotedTc, QTestResult::currentTestObjectName(), sizeof(quotedTc));
        QTest::qt_snprintf(buf, sizeof(buf),
                "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n"
                "<TestCase name=\"%s\">\n", quotedTc);
        outputString(buf);
    }

    QTest::qt_snprintf(buf, sizeof(buf),
            "<Environment>\n"
            "    <QtVersion>%s</QtVersion>\n"
            "    <QTestVersion>"QTEST_VERSION_STR"</QTestVersion>\n"
            "</Environment>\n", qVersion());
    outputString(buf);
}

void QXmlTestLogger::stopLogging()
{
    if (xmlmode == QXmlTestLogger::Complete) {
        outputString("</TestCase>\n");
    }

    QAbstractTestLogger::stopLogging();
}

void QXmlTestLogger::enterTestFunction(const char *function)
{
    char buf[1024];
    char quotedFunction[950];
    xmlQuote(quotedFunction, function, sizeof(quotedFunction));
    QTest::qt_snprintf(buf, sizeof(buf), "<TestFunction name=\"%s\">\n", quotedFunction);
    outputString(buf);
}

void QXmlTestLogger::leaveTestFunction()
{
    outputString("</TestFunction>\n");
}

namespace QTest
{

inline static bool isEmpty(const char *str)
{
    return !str || !str[0];
}

static const char *incidentFormatString(bool noDescription, bool noTag)
{
    if (noDescription) {
        if (noTag)
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\" />\n";
        else
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <DataTag><![CDATA[%s%s%s%s]]></DataTag>\n"
                "</Incident>\n";
    } else {
        if (noTag)
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <Description><![CDATA[%s%s%s%s]]></Description>\n"
                "</Incident>\n";
        else
            return "<Incident type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <DataTag><![CDATA[%s%s%s]]></DataTag>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Incident>\n";
    }
}

static const char *benchmarkResultFormatString()
{
    return "<BenchmarkResult metric=\"%s\" tag=\"%s\" value=\"%s\" iterations=\"%d\" />\n";
}

static const char *messageFormatString(bool noDescription, bool noTag)
{
    if (noDescription) {
        if (noTag)
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\" />\n";
        else
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <DataTag><![CDATA[%s%s%s%s]]></DataTag>\n"
                "</Message>\n";
    } else {
        if (noTag)
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <Description><![CDATA[%s%s%s%s]]></Description>\n"
                "</Message>\n";
        else
            return "<Message type=\"%s\" file=\"%s\" line=\"%d\">\n"
                "    <DataTag><![CDATA[%s%s%s]]></DataTag>\n"
                "    <Description><![CDATA[%s]]></Description>\n"
                "</Message>\n";
    }
}

} // namespace

void QXmlTestLogger::addIncident(IncidentTypes type, const char *description,
                                const char *file, int line)
{
    // buffer must be large enough to hold all quoted/cdata buffers plus the format string itself
    char buf[5000];
    const char *tag = QTestResult::currentDataTag();
    const char *gtag = QTestResult::currentGlobalDataTag();
    const char *filler = (tag && gtag) ? ":" : "";
    const bool notag = QTest::isEmpty(tag) && QTest::isEmpty(gtag);

    char quotedFile[1024];
    char cdataGtag[1024];
    char cdataTag[1024];
    char cdataDescription[1024];

    xmlQuote(quotedFile, file, sizeof(quotedFile));
    xmlCdata(cdataGtag, gtag, sizeof(cdataGtag));
    xmlCdata(cdataTag, tag, sizeof(cdataTag));
    xmlCdata(cdataDescription, description, sizeof(cdataDescription));

    QTest::qt_snprintf(buf, sizeof(buf),
            QTest::incidentFormatString(QTest::isEmpty(description), notag),
            QTest::xmlIncidentType2String(type),
            quotedFile, line,
            cdataGtag,
            filler,
            cdataTag,
            cdataDescription);

    outputString(buf);
}

void QXmlTestLogger::addBenchmarkResult(const QBenchmarkResult &result)
{
    char buf[1536];
    char quotedMetric[64];
    char quotedTag[1024];

    xmlQuote(quotedMetric,
        QBenchmarkGlobalData::current->measurer->metricText().toAscii().constData(),
        sizeof(quotedMetric));
    xmlQuote(quotedTag, result.context.tag.toAscii().constData(), sizeof(quotedTag));

    QTest::qt_snprintf(
        buf, sizeof(buf),
        QTest::benchmarkResultFormatString(),
        quotedMetric,
        quotedTag,
        QByteArray::number(result.value).constData(),  //no 64-bit qt_snprintf support
        result.iterations);
    outputString(buf);
}

void QXmlTestLogger::addMessage(MessageTypes type, const char *message,
                                const char *file, int line)
{
    char buf[5000];
    const char *tag = QTestResult::currentDataTag();
    const char *gtag = QTestResult::currentGlobalDataTag();
    const char *filler = (tag && gtag) ? ":" : "";
    const bool notag = QTest::isEmpty(tag) && QTest::isEmpty(gtag);

    char quotedFile[1024];
    char cdataGtag[1024];
    char cdataTag[1024];
    char cdataDescription[1024];

    xmlQuote(quotedFile, file, sizeof(quotedFile));
    xmlCdata(cdataGtag, gtag, sizeof(cdataGtag));
    xmlCdata(cdataTag, tag, sizeof(cdataTag));
    xmlCdata(cdataDescription, message, sizeof(cdataDescription));

    QTest::qt_snprintf(buf, sizeof(buf),
            QTest::messageFormatString(QTest::isEmpty(message), notag),
            QTest::xmlMessageType2String(type),
            quotedFile, line,
            cdataGtag,
            filler,
            cdataTag,
            cdataDescription);

    outputString(buf);
}

/*
    Copy up to n characters from the src string into dest, escaping any special
    XML characters as necessary so that dest is suitable for use in an XML
    quoted attribute string.
*/
void QXmlTestLogger::xmlQuote(char* dest, char const* src, size_t n)
{
    if (n == 0) return;

    *dest = 0;
    if (!src) return;

    char* end = dest + n;

    while (dest < end) {
        switch (*src) {

#define MAP_ENTITY(chr, ent) \
            case chr:                           \
                if (dest + sizeof(ent) < end) { \
                    strcpy(dest, ent);          \
                    dest += sizeof(ent) - 1;    \
                }                               \
                else {                          \
                    *dest = 0;                  \
                    return;                     \
                }                               \
                ++src;                          \
                break;

            MAP_ENTITY('>', "&gt;");
            MAP_ENTITY('<', "&lt;");
            MAP_ENTITY('\'', "&apos;");
            MAP_ENTITY('"', "&quot;");
            MAP_ENTITY('&', "&amp;");

            // not strictly necessary, but allows handling of comments without
            // having to explicitly look for `--'
            MAP_ENTITY('-', "&#x002D;");

#undef MAP_ENTITY

            case 0:
                *dest = 0;
                return;

            default:
                *dest = *src;
                ++dest;
                ++src;
                break;
        }
    }

    // If we get here, dest was completely filled (dest == end)
    *(dest-1) = 0;
}

/*
    Copy up to n characters from the src string into dest, escaping any
    special strings such that dest is suitable for use in an XML CDATA section.
*/
void QXmlTestLogger::xmlCdata(char* dest, char const* src, size_t n)
{
    if (!n) return;

    if (!src || n == 1) {
        *dest = 0;
        return;
    }

    char const CDATA_END[] = "]]>";
    char const CDATA_END_ESCAPED[] = "]]]><![CDATA[]>";

    char* end = dest + n;
    while (dest < end) {
        if (!*src) {
            *dest = 0;
            return;
        }

        if (!strncmp(src, CDATA_END, sizeof(CDATA_END)-1)) {
            if (dest + sizeof(CDATA_END_ESCAPED) < end) {
                strcpy(dest, CDATA_END_ESCAPED);
                src += sizeof(CDATA_END)-1;
                dest += sizeof(CDATA_END_ESCAPED) - 1;
            }
            else {
                *dest = 0;
                return;
            }
            continue;
        }

        *dest = *src;
        ++src;
        ++dest;
    }

    // If we get here, dest was completely filled (dest == end)
    *(dest-1) = 0;
}

QT_END_NAMESPACE
