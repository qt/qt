#include "qtestelementattribute.h"
#include <QtCore/qbytearray.h>
#include <string.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

QTestElementAttribute::QTestElementAttribute()
    :attributeValue(0),
    attributeIndex(QTest::AI_Undefined)
{
}

QTestElementAttribute::~QTestElementAttribute()
{
    delete[] attributeValue;
}

const char *QTestElementAttribute::value() const
{
    return attributeValue;
}

const char *QTestElementAttribute::name() const
{
    const char *AttributeNames[] =
    {
        "name",
        "result",
        "tests",
        "failures",
        "errors",
        "type",
        "description",
        "value",
        "qtestversion",
        "qtversion",
        "file",
        "line",
        "metric",
        "tag",
        "value",
        "iterations"
    };

    if(attributeIndex != QTest::AI_Undefined)
        return AttributeNames[attributeIndex];

    return 0;
}

QTest::AttributeIndex QTestElementAttribute::index() const
{
    return attributeIndex;
}

bool QTestElementAttribute::isNull() const
{
    return attributeIndex == QTest::AI_Undefined;
}

bool QTestElementAttribute::setPair(QTest::AttributeIndex index, const char *value)
{
    if(!value)
        return false;

    delete[] attributeValue;

    attributeIndex = index;
    attributeValue = qstrdup(value);

    return (attributeValue!=0) ? true:false;
}

QT_END_NAMESPACE

