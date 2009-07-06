#ifndef BIRTHDAYPARTY_H
#define BIRTHDAYPARTY_H

#include <QObject>
#include <qml.h>
#include "person.h"

class BirthdayParty : public QObject
{
Q_OBJECT
// ![0]
Q_PROPERTY(Person *celebrant READ celebrant WRITE setCelebrant)
Q_PROPERTY(QmlList<Person *> *guests READ guests)
// ![0]
public:
    BirthdayParty(QObject *parent = 0);

    Person *celebrant() const;
    void setCelebrant(Person *);

    QmlList<Person *> *guests();

private:
    Person *m_celebrant;
    QmlConcreteList<Person *> m_guests;
};
QML_DECLARE_TYPE(BirthdayParty);

#endif // BIRTHDAYPARTY_H
