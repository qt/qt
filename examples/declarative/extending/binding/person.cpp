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
    if (m_size == s)
        return;

    m_size = s;
    emit shoeChanged();
}

QColor ShoeDescription::color() const
{
    return m_color;
}

void ShoeDescription::setColor(const QColor &c)
{
    if (m_color == c)
        return;

    m_color = c;
    emit shoeChanged();
}

QString ShoeDescription::brand() const
{
    return m_brand;
}

void ShoeDescription::setBrand(const QString &b)
{
    if (m_brand == b)
        return;

    m_brand = b;
    emit shoeChanged();
}

qreal ShoeDescription::price() const
{
    return m_price;
}

void ShoeDescription::setPrice(qreal p)
{
    if (m_price == p)
        return;

    m_price = p;
    emit shoeChanged();
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
    if (m_name == n)
        return;

    m_name = n;
    emit nameChanged();
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

QML_DEFINE_TYPE(Boy, Boy);

Girl::Girl(QObject * parent)
: Person(parent)
{
}

QML_DEFINE_TYPE(Girl, Girl);
