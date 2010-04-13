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
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativebehavior_p.h>
#include <private/qdeclarativeanimation_p.h>
#include "../../../shared/util.h"

class tst_qdeclarativebehaviors : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativebehaviors() {}

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
    void explicitSelection();
    void nonSelectingBehavior();
    void reassignedAnimation();
    void disabled();
    void dontStart();
    void startup();
};

void tst_qdeclarativebehaviors::simpleBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/simple.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);
    QTRY_VERIFY(qobject_cast<QDeclarativeBehavior*>(rect->findChild<QDeclarativeBehavior*>("MyBehavior"))->animation());

    rect->setState("moved");
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::scriptTriggered()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/scripttrigger.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setColor(QColor("red"));
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::cppTriggered()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/cpptrigger.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QDeclarativeRectangle *innerRect = qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"));
    QTRY_VERIFY(innerRect);

    innerRect->setProperty("x", 200);
    QTRY_VERIFY(innerRect->x() > 0);
    QTRY_VERIFY(innerRect->x() < 200);  //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::loop()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/loop.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    //don't crash
    rect->setState("moved");

    delete rect;
}

void tst_qdeclarativebehaviors::colorBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/color.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("red");
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->color() != QColor("red"));
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->color() != QColor("green"));
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::parentBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/parent.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("reparented");
    QTRY_VERIFY(rect->findChild<QDeclarativeRectangle*>("MyRect")->parentItem() != rect->findChild<QDeclarativeItem*>("NewParent"));
    QTRY_VERIFY(rect->findChild<QDeclarativeRectangle*>("MyRect")->parentItem() == rect->findChild<QDeclarativeItem*>("NewParent"));

    delete rect;
}

void tst_qdeclarativebehaviors::replaceBinding()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/binding.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("moved");
    QDeclarativeRectangle *innerRect = qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"));
    QTRY_VERIFY(innerRect);
    QTRY_VERIFY(innerRect->x() > 0);
    QTRY_VERIFY(innerRect->x() < 200);
    //i.e. the behavior has been triggered
    QTRY_COMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("basex", 10);
    QTRY_COMPARE(innerRect->x(), (qreal)200);
    rect->setProperty("movedx", 210);
    QTRY_COMPARE(innerRect->x(), (qreal)210);

    rect->setState("");
    QTRY_VERIFY(innerRect->x() > 10);
    QTRY_VERIFY(innerRect->x() < 210);  //i.e. the behavior has been triggered
    QTRY_COMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("movedx", 200);
    QTRY_COMPARE(innerRect->x(), (qreal)10);
    rect->setProperty("basex", 20);
    QTRY_COMPARE(innerRect->x(), (qreal)20);

    delete rect;
}

void tst_qdeclarativebehaviors::group()
{
    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/groupProperty.qml"));
        QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
        QTRY_VERIFY(rect);

        rect->setState("moved");
        //QTest::qWait(200);
        QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() > 0);
        QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() < 200);
        //i.e. the behavior has been triggered

        delete rect;
    }

    {
        QDeclarativeEngine engine;
        QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/groupProperty2.qml"));
        QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
        QTRY_VERIFY(rect);

        rect->setState("moved");
        QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() > 0);
        QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() < 200);
        //i.e. the behavior has been triggered

        delete rect;
    }
}

void tst_qdeclarativebehaviors::emptyBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/empty.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("moved");
    qreal x = qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x();
    QTRY_COMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::explicitSelection()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/explicit.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("moved");
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() > 0);
    QTRY_VERIFY(qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x() < 200);
    //i.e. the behavior has been triggered

    delete rect;
}

void tst_qdeclarativebehaviors::nonSelectingBehavior()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/nonSelecting2.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    rect->setState("moved");
    qreal x = qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x();
    QTRY_COMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::reassignedAnimation()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/reassignedAnimation.qml"));
    QTest::ignoreMessage(QtWarningMsg, QString("QML Behavior (" + QUrl::fromLocalFile(SRCDIR "/data/reassignedAnimation.qml").toString() + ":9:9) Cannot change the animation assigned to a Behavior.").toUtf8().constData());
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);
    QTRY_COMPARE(qobject_cast<QDeclarativeNumberAnimation*>(
                 qobject_cast<QDeclarativeBehavior*>(
                     rect->findChild<QDeclarativeBehavior*>("MyBehavior"))->animation())->duration(), 200);

    delete rect;
}

void tst_qdeclarativebehaviors::disabled()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/disabled.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);
    QTRY_COMPARE(rect->findChild<QDeclarativeBehavior*>("MyBehavior")->enabled(), false);

    rect->setState("moved");
    qreal x = qobject_cast<QDeclarativeRectangle*>(rect->findChild<QDeclarativeRectangle*>("MyRect"))->x();
    QTRY_COMPARE(x, qreal(200));    //should change immediately

    delete rect;
}

void tst_qdeclarativebehaviors::dontStart()
{
    QDeclarativeEngine engine;

    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/dontStart.qml"));

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeAbstractAnimation: setRunning() cannot be used on non-root animation nodes");
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QDeclarativeAbstractAnimation *myAnim = rect->findChild<QDeclarativeAbstractAnimation*>("MyAnim");
    QTRY_VERIFY(myAnim && myAnim->qtAnimation());
    QTRY_VERIFY(myAnim->qtAnimation()->state() == QAbstractAnimation::Stopped);

    delete rect;
}

void tst_qdeclarativebehaviors::startup()
{
    QDeclarativeEngine engine;
    QDeclarativeComponent c(&engine, QUrl::fromLocalFile(SRCDIR "/data/startup.qml"));
    QDeclarativeRectangle *rect = qobject_cast<QDeclarativeRectangle*>(c.create());
    QTRY_VERIFY(rect);

    QDeclarativeRectangle *innerRect = rect->findChild<QDeclarativeRectangle*>("innerRect");
    QTRY_VERIFY(innerRect);

    QTRY_COMPARE(innerRect->x(), qreal(100));    //should be set immediately

    delete rect;
}

QTEST_MAIN(tst_qdeclarativebehaviors)

#include "tst_qdeclarativebehaviors.moc"
