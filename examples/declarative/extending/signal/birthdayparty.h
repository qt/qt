#ifndef BIRTHDAYPARTY_H
#define BIRTHDAYPARTY_H

#include <QObject>
#include <QDate>
#include <qml.h>
#include "person.h"

class BirthdayPartyAttached : public QObject
{
Q_OBJECT
Q_PROPERTY(QDate rsvp READ rsvp WRITE setRsvp);
public:
    BirthdayPartyAttached(QObject *object);

    QDate rsvp() const;
    void setRsvp(const QDate &);

private:
    QDate m_rsvp;
};
QML_DECLARE_TYPE(BirthdayPartyAttached);

class BirthdayParty : public QObject
{
Q_OBJECT
Q_PROPERTY(Person *celebrant READ celebrant WRITE setCelebrant)
Q_PROPERTY(QmlList<Person *> *guests READ guests)
Q_CLASSINFO("DefaultProperty", "guests")
public:
    BirthdayParty(QObject *parent = 0);

    Person *celebrant() const;
    void setCelebrant(Person *);

    QmlList<Person *> *guests();

    static BirthdayPartyAttached *qmlAttachedProperties(QObject *);

    void startParty();
// ![0]
signals:
    void partyStarted(const QTime &time);
// ![0]

private:
    Person *m_celebrant;
    QmlConcreteList<Person *> m_guests;
};
QML_DECLARE_TYPE(BirthdayParty);

#endif // BIRTHDAYPARTY_H
