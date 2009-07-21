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

#include "qtestxunitstreamer.h"
#include "qtestelement.h"

#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

QT_BEGIN_NAMESPACE

QTestXunitStreamer::QTestXunitStreamer()
    :QTestBasicStreamer()
{}

QTestXunitStreamer::~QTestXunitStreamer()
{}

void QTestXunitStreamer::indentForElement(const QTestElement* element, char* buf, int size)
{
    if (size == 0) return;

    buf[0] = 0;

    if (!element) return;

    char* endbuf = buf + size;
    element = element->parentElement();
    while (element && buf+2 < endbuf) {
        *(buf++) = ' ';
        *(buf++) = ' ';
        *buf = 0;
        element = element->parentElement();
    }
}

void QTestXunitStreamer::formatStart(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted )
        return;

    char indent[20];
    indentForElement(element, indent, sizeof(indent));

    // Errors are written as CDATA within system-err, comments elsewhere
    if (element->elementType() == QTest::LET_Error) {
        if (element->parentElement()->elementType() == QTest::LET_SystemError) {
            QTest::qt_snprintf(formatted, 1024, "<![CDATA[");
        }
        else {
            QTest::qt_snprintf(formatted, 1024, "%s<!--", indent);
        }
        return;
    }

    QTest::qt_snprintf(formatted, 1024, "%s<%s", indent, element->elementName());
}

void QTestXunitStreamer::formatEnd(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted )
        return;

    if(!element->childElements()){
        QTest::qt_snprintf(formatted, 10, "");
        return;
    }

    char indent[20];
    indentForElement(element, indent, sizeof(indent));

    QTest::qt_snprintf(formatted, 1024, "%s</%s>\n", indent, element->elementName());
}

void QTestXunitStreamer::formatAttributes(const QTestElement* element, const QTestElementAttribute *attribute, char *formatted) const
{
    if(!attribute || !formatted )
        return;

    QTest::AttributeIndex attrindex = attribute->index();

    // For errors within system-err, we only want to output `message'
    if (element && element->elementType() == QTest::LET_Error
        && element->parentElement()->elementType() == QTest::LET_SystemError) {

        if (attrindex != QTest::AI_Description) return;

        QXmlTestLogger::xmlCdata(formatted, attribute->value(), 1024);
        return;
    }

    char const* key = 0;
    if (attrindex == QTest::AI_Description)
        key = "message";
    else if (attrindex != QTest::AI_File && attrindex != QTest::AI_Line)
        key = attribute->name();

    if (key) {
        char quotedValue[900];
        QXmlTestLogger::xmlQuote(quotedValue, attribute->value(), sizeof(quotedValue));
        QTest::qt_snprintf(formatted, 1024, " %s=\"%s\"", key, quotedValue);
    }
    else {
        QTest::qt_snprintf(formatted, 10, "");
    }
}

void QTestXunitStreamer::formatAfterAttributes(const QTestElement *element, char *formatted) const
{
    if(!element || !formatted )
        return;

    // Errors are written as CDATA within system-err, comments elsewhere
    if (element->elementType() == QTest::LET_Error) {
        if (element->parentElement()->elementType() == QTest::LET_SystemError) {
            QTest::qt_snprintf(formatted, 1024, "]]>\n");
        }
        else {
            QTest::qt_snprintf(formatted, 1024, " -->\n");
        }
        return;
    }

    if(!element->childElements())
        QTest::qt_snprintf(formatted, 10, "/>\n");
    else
        QTest::qt_snprintf(formatted, 10, ">\n");
}

void QTestXunitStreamer::output(QTestElement *element) const
{
    char buf[1024];
    QTest::qt_snprintf(buf, sizeof(buf), "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
    outputString(buf);
    QTestBasicStreamer::output(element);
}

void QTestXunitStreamer::outputElements(QTestElement *element, bool) const
{
    char buf[1024];
    bool hasChildren;
    /*
        Elements are in reverse order of occurrence, so start from the end and work
        our way backwards.
    */
    while (element && element->nextElement()) {
        element = element->nextElement();
    }
    while (element) {
        hasChildren = element->childElements();

        if(element->elementType() != QTest::LET_Benchmark){
            formatStart(element, buf);
            outputString(buf);

            formatBeforeAttributes(element, buf);
            outputString(buf);

            outputElementAttributes(element, element->attributes());

            formatAfterAttributes(element, buf);
            outputString(buf);

            if(hasChildren)
                outputElements(element->childElements(), true);

            formatEnd(element, buf);
            outputString(buf);
        }
        element = element->previousElement();
    }
}

QT_END_NAMESPACE

