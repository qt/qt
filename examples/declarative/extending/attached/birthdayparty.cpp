#include "birthdayparty.h"

BirthdayPartyAttached::BirthdayPartyAttached(QObject *object)
: QObject(object)
{
}

QDate BirthdayPartyAttached::rsvp() const
{
    return m_rsvp;
}

void BirthdayPartyAttached::setRsvp(const QDate &d)
{
    m_rsvp = d;
}

QML_DEFINE_NOCREATE_TYPE(BirthdayPartyAttached);

BirthdayParty::BirthdayParty(QObject *parent)
: QObject(parent), m_celebrant(0)
{
}

Person *BirthdayParty::celebrant() const
{
    return m_celebrant;
}

void BirthdayParty::setCelebrant(Person *c)
{
    m_celebrant = c;
}

QmlList<Person *> *BirthdayParty::guests() 
{
    return &m_guests;
}

BirthdayPartyAttached *BirthdayParty::qmlAttachedProperties(QObject *object)
{
    return new BirthdayPartyAttached(object);
}

QML_DEFINE_TYPE(BirthdayParty, BirthdayParty);
