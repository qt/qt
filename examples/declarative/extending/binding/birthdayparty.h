#ifndef BIRTHDAYPARTY_H
#define BIRTHDAYPARTY_H

#include <QObject>
#include <QDate>
#include <QDebug>
#include <qml.h>
#include "person.h"

class BirthdayPartyAttached : public QObject
{
Q_OBJECT
Q_PROPERTY(QDate rsvp READ rsvp WRITE setRsvp NOTIFY rsvpChanged);
public:
    BirthdayPartyAttached(QObject *object);

    QDate rsvp() const;
    void setRsvp(const QDate &);

signals:
    void rsvpChanged();

private:
    QDate m_rsvp;
};
QML_DECLARE_TYPE(BirthdayPartyAttached);

class BirthdayParty : public QObject
{
Q_OBJECT
// ![0]
Q_PROPERTY(Person *celebrant READ celebrant WRITE setCelebrant NOTIFY celebrantChanged)
// ![0]
Q_PROPERTY(QmlList<Person *> *guests READ guests)
Q_PROPERTY(QString speaker READ speaker WRITE setSpeaker)
Q_CLASSINFO("DefaultProperty", "guests")
public:
    BirthdayParty(QObject *parent = 0);

    Person *celebrant() const;
    void setCelebrant(Person *);

    QmlList<Person *> *guests();

    QString speaker() const;
    void setSpeaker(const QString &);

    static BirthdayPartyAttached *qmlAttachedProperties(QObject *);

    void startParty();
signals:
    void partyStarted(const QTime &time);
    void celebrantChanged();

private:
    Person *m_celebrant;
    QmlConcreteList<Person *> m_guests;
};
QML_DECLARE_TYPE(BirthdayParty);

#endif // BIRTHDAYPARTY_H
