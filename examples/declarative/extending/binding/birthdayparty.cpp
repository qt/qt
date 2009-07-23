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
    if (d != m_rsvp) {
        m_rsvp = d;
        emit rsvpChanged();
    }
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
    if (c == m_celebrant) return;
    m_celebrant = c;
    emit celebrantChanged();
}

QmlList<Person *> *BirthdayParty::guests() 
{
    return &m_guests;
}

void BirthdayParty::startParty()
{
    QTime time = QTime::currentTime();
    emit partyStarted(time);
}

QString BirthdayParty::speaker() const
{
    return QString();
}

void BirthdayParty::setSpeaker(const QString &speak)
{
    qWarning() << qPrintable(speak);
}

BirthdayPartyAttached *BirthdayParty::qmlAttachedProperties(QObject *object)
{
    return new BirthdayPartyAttached(object);
}

QML_DEFINE_TYPE(People, 1, 0, 0, BirthdayParty, BirthdayParty);
