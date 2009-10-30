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
#include <QtDeclarative/qfxrect.h>

class tst_states : public QObject
{
    Q_OBJECT
public:
    tst_states() {}

private slots:
    void basicChanges();
    void basicExtension();
    void basicBinding();
    void signalOverride();
};

void tst_states::basicChanges()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges2.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicChanges3.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);
        //### we should be checking that this is an implicit rather than explicit 1 (which currently fails)

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

    }
}

void tst_states::basicExtension()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicExtension.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);

        rect->setState("bordered");
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(rect->border()->width(),2);

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        QCOMPARE(rect->border()->width(),1);
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/fakeExtension.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
    }
}

void tst_states::basicBinding()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding2.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("green"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding3.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("green"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("red"));
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor2", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor2", QColor("green"));
        QCOMPARE(rect->color(),QColor("red"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/basicBinding4.qml");
        QFxRect *rect = qobject_cast<QFxRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("blue"));
        rect->setProperty("sourceColor", QColor("yellow"));
        QCOMPARE(rect->color(),QColor("yellow"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));
        rect->setProperty("sourceColor", QColor("purple"));
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("blue");
        QCOMPARE(rect->color(),QColor("purple"));

        rect->setState("green");
        QCOMPARE(rect->color(),QColor("green"));

        rect->setState("");
        QCOMPARE(rect->color(),QColor("red"));
    }
}

class MyRect : public QFxRect
{
   Q_OBJECT
public:
    MyRect() {}
    void doSomething() { emit didSomething(); }
Q_SIGNALS:
    void didSomething();
};

QML_DECLARE_TYPE(MyRect)
QML_DEFINE_TYPE(Qt.test, 1, 0, 0, MyRectangle,MyRect);

void tst_states::signalOverride()
{
    QmlEngine engine;

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/signalOverride.qml");
        MyRect *rect = qobject_cast<MyRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("red"));
        rect->doSomething();
        QCOMPARE(rect->color(),QColor("blue"));

        rect->setState("green");
        rect->doSomething();
        QCOMPARE(rect->color(),QColor("green"));
    }

    {
        QmlComponent rectComponent(&engine, SRCDIR "/data/signalOverride2.qml");
        MyRect *rect = qobject_cast<MyRect*>(rectComponent.create());
        QVERIFY(rect != 0);

        QCOMPARE(rect->color(),QColor("white"));
        rect->doSomething();
        QCOMPARE(rect->color(),QColor("blue"));

        QFxRect *innerRect = qobject_cast<QFxRect*>(rect->findChild<QFxRect*>("extendedRect"));

        innerRect->setState("green");
        rect->doSomething();
        QCOMPARE(rect->color(),QColor("blue"));
        QCOMPARE(innerRect->color(),QColor("green"));
        QCOMPARE(innerRect->property("extendedColor").value<QColor>(),QColor("green"));
    }
}

QTEST_MAIN(tst_states)

#include "tst_states.moc"
