#include <QCoreApplication>
#include <QmlEngine>
#include <QmlComponent>
#include <QDebug>
#include "birthdayparty.h"
#include "person.h"

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    QmlEngine engine;
    QmlComponent component(&engine, ":example.qml");
    BirthdayParty *party = qobject_cast<BirthdayParty *>(component.create());

    if (party && party->celebrant()) {
        qWarning() << party->celebrant()->name() << "is having a birthday!";

        if (qobject_cast<Boy *>(party->celebrant()))
            qWarning() << "He is inviting:";
        else
            qWarning() << "She is inviting:";

        for (int ii = 0; ii < party->guests()->count(); ++ii) {
            Person *guest = party->guests()->at(ii);

            QDate rsvpDate;
            QObject *attached = 
                qmlAttachedPropertiesObject<BirthdayParty>(guest, false);
            if (attached)
                rsvpDate = attached->property("rsvp").toDate();

            if (rsvpDate.isNull())
                qWarning() << "   " << guest->name() << "RSVP date: Hasn't RSVP'd";
            else
                qWarning() << "   " << guest->name() << "RSVP date:" << qPrintable(rsvpDate.toString());
        }

    } else {
        qWarning() << "An error occured";
    }

    return 0;
}
