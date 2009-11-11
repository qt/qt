/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <private/qmlspringfollow_p.h>
#include <private/qmlvaluetype_p.h>
#include "../../../shared/util.h"

class tst_qmlspringfollow : public QObject
{
    Q_OBJECT
public:
    tst_qmlspringfollow();

private slots:
    void defaultValues();
    void values();
    void disabled();

private:
    QmlEngine engine;
};

tst_qmlspringfollow::tst_qmlspringfollow()
{
}

void tst_qmlspringfollow::defaultValues()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/springfollow1.qml"));
    QmlSpringFollow *obj = qobject_cast<QmlSpringFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 0.);
    QCOMPARE(obj->velocity(), 0.);
    QCOMPARE(obj->spring(), 0.);
    QCOMPARE(obj->damping(), 0.);
    QCOMPARE(obj->epsilon(), 0.01);
    QCOMPARE(obj->modulus(), 0.);
    QCOMPARE(obj->value(), 0.);
    QCOMPARE(obj->mass(), 1.);
    QCOMPARE(obj->enabled(), true);
    QCOMPARE(obj->inSync(), true);

    delete obj;
}

void tst_qmlspringfollow::values()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/springfollow2.qml"));
    QmlSpringFollow *obj = qobject_cast<QmlSpringFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 1.44);
    QCOMPARE(obj->velocity(), 0.9);
    QCOMPARE(obj->spring(), 1.0);
    QCOMPARE(obj->damping(), 0.5);
    QCOMPARE(obj->epsilon(), 0.25);
    QCOMPARE(obj->modulus(), 360.0);
    QCOMPARE(obj->mass(), 2.0);
    QCOMPARE(obj->enabled(), true);

    QTRY_COMPARE(obj->value(), 1.44);
    QTRY_COMPARE(obj->inSync(), true);

    delete obj;
}

void tst_qmlspringfollow::disabled()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl("file://" SRCDIR "/data/springfollow3.qml"));
    QmlSpringFollow *obj = qobject_cast<QmlSpringFollow*>(c.create());

    QVERIFY(obj != 0);

    QCOMPARE(obj->sourceValue(), 1.44);
    QCOMPARE(obj->velocity(), 0.9);
    QCOMPARE(obj->spring(), 1.0);
    QCOMPARE(obj->damping(), 0.5);
    QCOMPARE(obj->epsilon(), 0.25);
    QCOMPARE(obj->modulus(), 360.0);
    QCOMPARE(obj->mass(), 2.0);
    QCOMPARE(obj->enabled(), false);

    QCOMPARE(obj->value(), 0.0);
    QCOMPARE(obj->inSync(), false);

    delete obj;
}

QTEST_MAIN(tst_qmlspringfollow)

#include "tst_qmlspringfollow.moc"
