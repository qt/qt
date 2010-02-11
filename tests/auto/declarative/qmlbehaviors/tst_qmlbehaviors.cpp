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
#include <QtDeclarative/qmlview.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlbehavior_p.h>
#include <private/qmlanimation_p.h>

class tst_behaviors : public QObject
{
    Q_OBJECT
public:
    tst_behaviors() {}

private slots:
    void simpleBehavior();
    void scriptTriggered();
    void cppTriggered();
    void loop();
    void colorBehavior();
    void parentBehavior();
    void replaceBinding();
    //void transitionOverrides();
    void group();
    void emptyBehavior();
    void nonSelectingBehavior();
    void reassignedAnimation();
    void disabled();
};

void tst_behaviors::simpleBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/simple.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);
    QVERIFY(qobject_cast<QmlBehavior*>(rect->findChild<QmlBehavior*>("MyBehavior"))->animation());

    rect->setState("moved");
    QTest::qWait(100);
    qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::scriptTriggered()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/scripttrigger.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    rect->setColor(QColor("red"));
    QTest::qWait(100);
    qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::cppTriggered()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/cpptrigger.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    QmlGraphicsRectangle *innerRect = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"));
    QVERIFY(innerRect);

    innerRect->setProperty("x", 200);
    QTest::qWait(100);
    qreal x = innerRect->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
}

void tst_behaviors::loop()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/loop.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    //don't crash
    rect->setState("moved");
}

void tst_behaviors::colorBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/color.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    rect->setState("red");
    QTest::qWait(100);
    QColor color = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->color();
    QVERIFY(color != QColor("red") && color != QColor("green"));  //i.e. the behavior has been triggered
}

void tst_behaviors::parentBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/parent.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    rect->setState("reparented");
    QTest::qWait(100);
    QmlGraphicsItem *newParent = rect->findChild<QmlGraphicsItem*>("NewParent");
    QmlGraphicsItem *parent = rect->findChild<QmlGraphicsRectangle*>("MyRect")->parentItem();
    QVERIFY(parent != newParent);
    QTest::qWait(300);
    parent = rect->findChild<QmlGraphicsRectangle*>("MyRect")->parentItem();
    QVERIFY(parent == newParent);
}

void tst_behaviors::replaceBinding()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/binding.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    rect->setState("moved");
    QTest::qWait(100);
    QmlGraphicsRectangle *innerRect = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"));
    QVERIFY(innerRect);
    qreal x = innerRect->x();
    QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("basex", 10);
    QCOMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("movedx", 210);
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)210);

    rect->setState("");
    QTest::qWait(100);
    x = innerRect->x();
    QVERIFY(x > 10 && x < 210);  //i.e. the behavior has been triggered
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("movedx", 200);
    QCOMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("basex", 20);
    QTest::qWait(300);
    QCOMPARE(innerRect->x(), (qreal)20);
}

void tst_behaviors::group()
{
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/groupProperty.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        QTest::qWait(100);
        qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
        QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/groupProperty2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        QTest::qWait(100);
        qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
        QVERIFY(x > 0 && x < 200);  //i.e. the behavior has been triggered
    }
}

void tst_behaviors::emptyBehavior()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/empty.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);

    rect->setState("moved");
    qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
    QCOMPARE(x, qreal(200));    //should change immediately
}

void tst_behaviors::nonSelectingBehavior()
{
    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/nonSelecting.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
        QCOMPARE(x, qreal(200));    //should change immediately
    }

    {
        QmlEngine engine;
        QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/nonSelecting2.qml"));
        QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
        QVERIFY(rect);

        rect->setState("moved");
        qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
        QCOMPARE(x, qreal(200));    //should change immediately
    }
}

void tst_behaviors::reassignedAnimation()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/reassignedAnimation.qml"));
    QTest::ignoreMessage(QtWarningMsg, QString("QML Behavior (" + QUrl::fromLocalFile(SRCDIR "/data/reassignedAnimation.qml").toString() + ":9:12) Can't change the animation assigned to a Behavior.").toUtf8().constData());
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);
    QCOMPARE(qobject_cast<QmlNumberAnimation*>(
                 qobject_cast<QmlBehavior*>(
                     rect->findChild<QmlBehavior*>("MyBehavior"))->animation())->duration(), 200);
}

void tst_behaviors::disabled()
{
    QmlEngine engine;
    QmlComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/disabled.qml"));
    QmlGraphicsRectangle *rect = qobject_cast<QmlGraphicsRectangle*>(c.create());
    QVERIFY(rect);
    QCOMPARE(rect->findChild<QmlBehavior*>("MyBehavior")->enabled(), false);

    rect->setState("moved");
    qreal x = qobject_cast<QmlGraphicsRectangle*>(rect->findChild<QmlGraphicsRectangle*>("MyRect"))->x();
    QCOMPARE(x, qreal(200));    //should change immediately

}

QTEST_MAIN(tst_behaviors)

#include "tst_qmlbehaviors.moc"
