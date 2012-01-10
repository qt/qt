/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include <private/qdeclarativebind_p.h>
#include <private/qdeclarativerectangle_p.h>
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativebinding : public QObject

{
    Q_OBJECT
public:
    tst_qdeclarativebinding();

private slots:
    void binding();
    void whenAfterValue();
    void deletedObject();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativebinding::tst_qdeclarativebinding()
{
}

void tst_qdeclarativebinding::binding()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-binding.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QVERIFY(rect != 0);

    QDeclarativeBind *binding3 = qobject_cast<QDeclarativeBind*>(rect->findChild<QDeclarativeBind*>("binding3"));
    QVERIFY(binding3 != 0);

    QCOMPARE(rect->color(), QColor("yellow"));
    QCOMPARE(rect->property("text").toString(), QString("Hello"));
    QCOMPARE(binding3->when(), false);

    rect->setProperty("changeColor", true);
    QCOMPARE(rect->color(), QColor("red"));

    QCOMPARE(binding3->when(), true);

    QDeclarativeBind *binding = qobject_cast<QDeclarativeBind*>(rect->findChild<QDeclarativeBind*>("binding1"));
    QVERIFY(binding != 0);
    QCOMPARE(binding->object(), qobject_cast<QObject*>(rect));
    QCOMPARE(binding->property(), QLatin1String("text"));
    QCOMPARE(binding->value().toString(), QLatin1String("Hello"));

    delete rect;
}

void tst_qdeclarativebinding::whenAfterValue()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/test-binding2.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());

    QVERIFY(rect != 0);
    QCOMPARE(rect->color(), QColor("yellow"));
    QCOMPARE(rect->property("text").toString(), QString("Hello"));

    rect->setProperty("changeColor", true);
    QCOMPARE(rect->color(), QColor("red"));

    delete rect;
}

//QTBUG-20692
void tst_qdeclarativebinding::deletedObject()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/deletedObject.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QVERIFY(rect != 0);

    QApplication::sendPostedEvents(0, QEvent::DeferredDelete);

    //don't crash
    rect->setProperty("activateBinding", true);

    delete rect;
}

QTEST_MAIN(tst_qdeclarativebinding)

#include "tst_qdeclarativebinding.moc"
