#include "birthdayparty.h"

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

QML_DEFINE_TYPE(People, 1, 0, 0, BirthdayParty, BirthdayParty);
