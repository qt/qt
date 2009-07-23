#include "person.h"

Person::Person(QObject *parent)
: QObject(parent), m_shoeSize(0)
{
}

QString Person::name() const
{
    return m_name;
}

void Person::setName(const QString &n)
{
    m_name = n;
}

int Person::shoeSize() const
{
    return m_shoeSize;
}

void Person::setShoeSize(int s)
{
    m_shoeSize = s;
}

QML_DEFINE_TYPE(People, 1, 0, 0, Person, Person);
