#ifndef QTESTCOREELEMENT_H
#define QTESTCOREELEMENT_H

#include "qtestcorelist.h"
#include "qtestelementattribute.h"

QT_BEGIN_NAMESPACE

template <class ElementType>
class QTestCoreElement: public QTestCoreList<ElementType>
{
    public:
        QTestCoreElement( int type = -1 );
        virtual ~QTestCoreElement();

        void addAttribute(const QTest::AttributeIndex index, const char *value);
        QTestElementAttribute *attributes() const;
        const char *attributeValue(QTest::AttributeIndex index) const;
        const char *attributeName(QTest::AttributeIndex index) const;
        const QTestElementAttribute *attribute(QTest::AttributeIndex index) const;

        const char *elementName() const;
        QTest::LogElementType elementType() const;

    private:
        QTestElementAttribute *listOfAttributes;
        QTest::LogElementType type;
};

template<class ElementType>
QTestCoreElement<ElementType>::QTestCoreElement(int t)
:listOfAttributes(0), type((QTest::LogElementType)t)
{
}

template<class ElementType>
QTestCoreElement<ElementType>::~QTestCoreElement()
{
    delete listOfAttributes;
}

template <class ElementType>
void QTestCoreElement<ElementType>::addAttribute(const QTest::AttributeIndex attributeIndex, const char *value)
{
    if(attributeIndex == -1)
        return;

    if (attribute(attributeIndex))
        return;

    QTestElementAttribute *attribute = new QTestElementAttribute;
    attribute->setPair(attributeIndex, value);
    attribute->addToList(&listOfAttributes);
}

template <class ElementType>
QTestElementAttribute *QTestCoreElement<ElementType>::attributes() const
{
    return listOfAttributes;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::attributeValue(QTest::AttributeIndex index) const
{
    const QTestElementAttribute *attrb = attribute(index);
    if(attrb)
        return attrb->value();

    return 0;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::attributeName(QTest::AttributeIndex index) const
{
    const QTestElementAttribute *attrb = attribute(index);
    if(attrb)
        return attrb->name();

    return 0;
}

template <class ElementType>
const char *QTestCoreElement<ElementType>::elementName() const
{
    const char *xmlElementNames[] =
    {
        "property",
        "properties",
        "failure",
        "error",
        "testcase",
        "testsuite",
        "benchmark"
    };

    if(type != QTest::LET_Undefined)
        return xmlElementNames[type];

    return 0;
}

template <class ElementType>
QTest::LogElementType QTestCoreElement<ElementType>::elementType() const
{
    return type;
}

template <class ElementType>
const QTestElementAttribute *QTestCoreElement<ElementType>::attribute(QTest::AttributeIndex index) const
{
    QTestElementAttribute *iterator = listOfAttributes;
    while(iterator){
        if(iterator->index() == index)
            return iterator;

        iterator = iterator->nextElement();
    }

    return 0;
}

QT_END_NAMESPACE

#endif
