#include "qtestxunitstreamer.h"
#include "qtestelement.h"

#include "QtTest/private/qtestlog_p.h"
#include "QtTest/private/qtestresult_p.h"
#include "QtTest/private/qxmltestlogger_p.h"

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

void QTestXunitStreamer::formatAttributes(const QTestElementAttribute *attribute, char *formatted) const
{
    if(!attribute || !formatted )
        return;

    QTest::AttributeIndex attrindex = attribute->index();

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

            outputElementAttributes(element->attributes());

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
