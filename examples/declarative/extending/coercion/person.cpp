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

// ![0]
QML_DEFINE_NOCREATE_TYPE(Person);
// ![0]

// ![1]
Boy::Boy(QObject * parent)
: Person(parent)
{
}

QML_DEFINE_TYPE(Boy, Boy);

Girl::Girl(QObject * parent)
: Person(parent)
{
}

QML_DEFINE_TYPE(Girl, Girl);
// ![1]
