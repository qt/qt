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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmleasefollow_p.h>
#include <private/qmlvaluetype_p.h>
#include "../../../shared/util.h"

class tst_qmleasefollow : public QObject
{
    Q_OBJECT
public:
    tst_qmleasefollow();

private slots:
    void defaultValues();
    void values();
    void disabled();

private:
    QmlEngine engine;
};

tst_qmleasefollow::tst_qmleasefollow()
{
}

void tst_qmleasefollow::defaultValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow1.qml"));
    QmlEaseFollow *obj = qobject_cast<QmlEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 0.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->enabled(), true);
    QCOMPARE(obj->duration(), -1.);
    QCOMPARE(obj->maximumEasingTime(), -1.);
    QCOMPARE(obj->reversingMode(), QmlEaseFollow::Eased);

    delete obj;
}

void tst_qmleasefollow::values()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow2.qml"));
    QmlEaseFollow *obj = qobject_cast<QmlEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 10.);
    QCOMPARE(obj->velocity(), 200.);
    QCOMPARE(obj->enabled(), true);
    QCOMPARE(obj->duration(), 300.);
    QCOMPARE(obj->maximumEasingTime(), -1.);
    QCOMPARE(obj->reversingMode(), QmlEaseFollow::Immediate);

    delete obj;
}

void tst_qmleasefollow::disabled()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/easefollow3.qml"));
    QmlEaseFollow *obj = qobject_cast<QmlEaseFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 10.);
    QCOMPARE(obj->velocity(), 250.);
    QCOMPARE(obj->enabled(), false);
    QCOMPARE(obj->maximumEasingTime(), 150.);
    QCOMPARE(obj->reversingMode(), QmlEaseFollow::Sync);

    delete obj;
}

QTEST_MAIN(tst_qmleasefollow)

#include "tst_qmleasefollow.moc"
