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
#include <private/qmlgraphicsflickable_p.h>
#include <private/qmlvaluetype_p.h>
#include <math.h>

class tst_qmlgraphicsflickable : public QObject
{
    Q_OBJECT
public:
    tst_qmlgraphicsflickable();

private slots:
    void create();
    void horizontalViewportSize();
    void verticalViewportSize();
    void properties();

private:
    QmlEngine engine;
};

tst_qmlgraphicsflickable::tst_qmlgraphicsflickable()
{
}

void tst_qmlgraphicsflickable::create()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable01.qml"));
    QmlGraphicsFlickable *obj = qobject_cast<QmlGraphicsFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);
    QCOMPARE(obj->viewportX(), 0.);
    QCOMPARE(obj->viewportY(), 0.);

    QCOMPARE(obj->horizontalVelocity(), 0.);
    QCOMPARE(obj->verticalVelocity(), 0.);
    QCOMPARE(obj->reportedVelocitySmoothing(), 100.);

    QCOMPARE(obj->isInteractive(), true);
    QCOMPARE(obj->overShoot(), true);
    QCOMPARE(obj->pressDelay(), 0);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    delete obj;
}

void tst_qmlgraphicsflickable::horizontalViewportSize()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable02.qml"));
    QmlGraphicsFlickable *obj = qobject_cast<QmlGraphicsFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->viewportWidth(), 800.);
    QCOMPARE(obj->viewportHeight(), 300.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qmlgraphicsflickable::verticalViewportSize()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable03.qml"));
    QmlGraphicsFlickable *obj = qobject_cast<QmlGraphicsFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->viewportWidth(), 200.);
    QCOMPARE(obj->viewportHeight(), 1200.);
    QCOMPARE(obj->isAtXBeginning(), true);
    QCOMPARE(obj->isAtXEnd(), false);
    QCOMPARE(obj->isAtYBeginning(), true);
    QCOMPARE(obj->isAtYEnd(), false);

    delete obj;
}

void tst_qmlgraphicsflickable::properties()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/flickable04.qml"));
    QmlGraphicsFlickable *obj = qobject_cast<QmlGraphicsFlickable*>(c.create());

    QVERIFY(obj != 0);
    QCOMPARE(obj->isInteractive(), false);
    QCOMPARE(obj->overShoot(), false);
    QCOMPARE(obj->pressDelay(), 200);
    QCOMPARE(obj->maximumFlickVelocity(), 2000.);

    delete obj;
}

QTEST_MAIN(tst_qmlgraphicsflickable)

#include "tst_qmlgraphicsflickable.moc"
