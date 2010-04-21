/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef BIRTHDAYPARTY_H
#define BIRTHDAYPARTY_H

#include <QObject>
#include <QDate>
#include <qdeclarative.h>
#include "person.h"

class BirthdayPartyAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDate rsvp READ rsvp WRITE setRsvp)
public:
    BirthdayPartyAttached(QObject *object);

    QDate rsvp() const;
    void setRsvp(const QDate &);

private:
    QDate m_rsvp;
};

class BirthdayParty : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Person *host READ host WRITE setHost)
    Q_PROPERTY(QDeclarativeListProperty<Person> guests READ guests)
    Q_CLASSINFO("DefaultProperty", "guests")
public:
    BirthdayParty(QObject *parent = 0);

    Person *host() const;
    void setHost(Person *);

    QDeclarativeListProperty<Person> guests();
    int guestCount() const;
    Person *guest(int) const;

    static BirthdayPartyAttached *qmlAttachedProperties(QObject *);

    void startParty();
// ![0]
signals:
    void partyStarted(const QTime &time);
// ![0]

private:
    Person *m_host;
    QList<Person *> m_guests;
};
QML_DECLARE_TYPEINFO(BirthdayParty, QML_HAS_ATTACHED_PROPERTIES)

#endif // BIRTHDAYPARTY_H
