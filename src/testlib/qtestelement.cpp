#include "qtestelement.h"

QTestElement::QTestElement(int type)
    :QTestCoreElement<QTestElement>(type),
    listOfChildren(0),
    parent(0)
{
}

QTestElement::~QTestElement()
{
    delete listOfChildren;
}

bool QTestElement::addLogElement(QTestElement *element)
{
    if(!element)
        return false;

    if(element->elementType() != QTest::LET_Undefined){
        element->addToList(&listOfChildren);
        element->setParent(this);
        return true;
    }

    return false;
}

QTestElement *QTestElement::childElements() const
{
    return listOfChildren;
}

const QTestElement *QTestElement::parentElement() const
{
    return parent;
}

void QTestElement::setParent(const QTestElement *p)
{
    parent = p;
}
