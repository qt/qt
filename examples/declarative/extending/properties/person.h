#ifndef PERSON_H
#define PERSON_H

#include <QObject>
#include <qml.h>

class Person : public QObject {
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName)
Q_PROPERTY(int shoeSize READ shoeSize WRITE setShoeSize)
public:
    Person(QObject *parent = 0);

    QString name() const;
    void setName(const QString &);

    int shoeSize() const;
    void setShoeSize(int);
private:
    QString m_name;
    int m_shoeSize;
};
QML_DECLARE_TYPE(Person);

#endif // PERSON_H
