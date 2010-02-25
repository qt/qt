/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <qtest.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <private/qdeclarativeeasefollow_p.h>
#include <private/qdeclarativevaluetype_p.h>
#include "../../../shared/util.h"

class tst_qdeclarativeeasefollow : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeeasefollow();

private slots:
    void defaultValues();
    void values();
    void disabled();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativeeasefollow::tst_qdeclarativeeasefollow()
{
}

void tst_qdeclarativeeasefollow::defaultValues()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow1.qml"));
    QDeclarativeEaseFollow *obj = qobject_cast<QDeclarativeEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 0.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->enabled(), true);
    QCOMPARE(obj->duration(), -1.);
    QCOMPARE(obj->maximumEasingTime(), -1.);
    QCOMPARE(obj->reversingMode(), QDeclarativeEaseFollow::Eased);

    delete obj;
}

void tst_qdeclarativeeasefollow::values()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow2.qml"));
    QDeclarativeEaseFollow *obj = qobject_cast<QDeclarativeEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 10.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->enabled(), true);
    QCOMPARE(obj->duration(), 300.);
    QCOMPARE(obj->maximumEasingTime(), -1.);
    QCOMPARE(obj->reversingMode(), QDeclarativeEaseFollow::Immediate);

    delete obj;
}

void tst_qdeclarativeeasefollow::disabled()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow3.qml"));
    QDeclarativeEaseFollow *obj = qobject_cast<QDeclarativeEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 10.);
    QCOMPARE(obj->velocity(), 250.);
    QCOMPARE(obj->enabled(), false);
    QCOMPARE(obj->maximumEasingTime(), 150.);
    QCOMPARE(obj->reversingMode(), QDeclarativeEaseFollow::Sync);

    delete obj;
}

QTEST_MAIN(tst_qdeclarativeeasefollow)

#include "tst_qdeclarativeeasefollow.moc"
