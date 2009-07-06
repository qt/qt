#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <QColor>
#include <qml.h>

class ShoeDescription : public QObject {
Q_OBJECT
Q_PROPERTY(int size READ size WRITE setSize)
Q_PROPERTY(QColor color READ color WRITE setColor)
Q_PROPERTY(QString brand READ brand WRITE setBrand)
Q_PROPERTY(qreal price READ price WRITE setPrice)
public:
    ShoeDescription(QObject *parent = 0);

    int size() const;
    void setSize(int);

    QColor color() const;
    void setColor(const QColor &);

    QString brand() const;
    void setBrand(const QString &);

    qreal price() const;
    void setPrice(qreal);
private:
    int m_size;
    QColor m_color;
    QString m_brand;
    qreal m_price;
};
QML_DECLARE_TYPE(ShoeDescription);

class Person : public QObject {
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName)
// ![1]
Q_PROPERTY(ShoeDescription *shoe READ shoe);
// ![1]
public:
    Person(QObject *parent = 0);

    QString name() const;
    void setName(const QString &);

    ShoeDescription *shoe();
private:
    QString m_name;
    ShoeDescription m_shoe;
};
QML_DECLARE_TYPE(Person);

class Boy : public Person {
Q_OBJECT
public:
    Boy(QObject * parent = 0);
};
QML_DECLARE_TYPE(Boy);

class Girl : public Person {
Q_OBJECT
public:
    Girl(QObject * parent = 0);
};
QML_DECLARE_TYPE(Girl);

#endif // PERSON_H
