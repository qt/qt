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
#include <QCoreApplication>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDebug>
#include "birthdayparty.h"
#include "person.h"

int main(int argc, char ** argv)
{
    QCoreApplication app(argc, argv);

    qmlRegisterType<BirthdayParty>("People", 1,0, "BirthdayParty");
    qmlRegisterType<Person>();
    qmlRegisterType<Boy>("People", 1,0, "Boy");
    qmlRegisterType<Girl>("People", 1,0, "Girl");

    QDeclarativeEngine engine;
    QDeclarativeComponent component(&engine, ":example.qml");
    BirthdayParty *party = qobject_cast<BirthdayParty *>(component.create());

    if (party && party->celebrant()) {
        qWarning() << party->celebrant()->name() << "is having a birthday!";

        if (qobject_cast<Boy *>(party->celebrant()))
            qWarning() << "He is inviting:";
        else
            qWarning() << "She is inviting:";

        for (int ii = 0; ii < party->guestCount(); ++ii)
            qWarning() << "   " << party->guest(ii)->name();
    } else {
        qWarning() << "An error occured";
    }

    return 0;
}
