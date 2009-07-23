#include "person.h"

ShoeDescription::ShoeDescription(QObject *parent)
: QObject(parent), m_size(0), m_price(0)
{
}

int ShoeDescription::size() const
{
    return m_size;
}

void ShoeDescription::setSize(int s)
{
    m_size = s;
}

QColor ShoeDescription::color() const
{
    return m_color;
}

void ShoeDescription::setColor(const QColor &c)
{
    m_color = c;
}

QString ShoeDescription::brand() const
{
    return m_brand;
}

void ShoeDescription::setBrand(const QString &b)
{
    m_brand = b;
}

qreal ShoeDescription::price() const
{
    return m_price;
}

void ShoeDescription::setPrice(qreal p)
{
    m_price = p;
}
QML_DEFINE_NOCREATE_TYPE(ShoeDescription);

Person::Person(QObject *parent)
: QObject(parent)
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

ShoeDescription *Person::shoe()
{
    return &m_shoe;
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
