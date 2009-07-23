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

QML_DEFINE_NOCREATE_TYPE(Person);

Boy::Boy(QObject * parent)
: Person(parent)
{
}

QML_DEFINE_TYPE(People, 1, 0, 0, Boy, Boy);

Girl::Girl(QObject * parent)
: Person(parent)
{
}

QML_DEFINE_TYPE(People, 1, 0, 0, Girl, Girl);
