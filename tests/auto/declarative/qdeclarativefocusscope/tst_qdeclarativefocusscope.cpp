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
#include <QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeview.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativetextedit_p.h>
#include <private/qdeclarativetext_p.h>
#include <QtDeclarative/private/qdeclarativefocusscope_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativefocusscope : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativefocusscope() {}

    template<typename T>
    T *findItem(QGraphicsObject *parent, const QString &id);

private slots:
    void basic();
    void nested();
    void noFocus();
    void textEdit();
    void forceFocus();
    void noParentFocus();
    void signalEmission();
    void qtBug13380();
    void forceActiveFocus();
};

/*
   Find an item with the specified id.
*/
template<typename T>
T *tst_qdeclarativefocusscope::findItem(QGraphicsObject *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    QList<QGraphicsItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(children.at(i)->toGraphicsObject());
        if (item) {
            if (mo.cast(item) && (objectName.isEmpty() || item->objectName() == objectName)) {
                return static_cast<T*>(item);
            }
            item = findItem<T>(item, objectName);
            if (item)
                return static_cast<T*>(item);
        }
    }
    return 0;
}

void tst_qdeclarativefocusscope::basic()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test.qml"));

    QDeclarativeFocusScope *item0 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item0"));
    QDeclarativeRectangle *item1 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeRectangle *item2 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeRectangle *item3 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item3"));
    QVERIFY(item0 != 0);
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Right);
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == true);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Down);
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == true);

    delete view;
}

void tst_qdeclarativefocusscope::nested()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test2.qml"));

    QDeclarativeFocusScope *item1 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeFocusScope *item2 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeFocusScope *item3 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item3"));
    QDeclarativeFocusScope *item4 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item4"));
    QDeclarativeFocusScope *item5 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item5"));
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);
    QVERIFY(item4 != 0);
    QVERIFY(item5 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());

    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == true);
    QVERIFY(item3->hasActiveFocus() == true);
    QVERIFY(item4->hasActiveFocus() == true);
    QVERIFY(item5->hasActiveFocus() == true);
    delete view;
}

void tst_qdeclarativefocusscope::noFocus()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test4.qml"));

    QDeclarativeRectangle *item0 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item0"));
    QDeclarativeRectangle *item1 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeRectangle *item2 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeRectangle *item3 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item3"));
    QVERIFY(item0 != 0);
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Right);
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Down);
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    delete view;
}

void tst_qdeclarativefocusscope::textEdit()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test5.qml"));

    QDeclarativeFocusScope *item0 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item0"));
    QDeclarativeTextEdit *item1 = findItem<QDeclarativeTextEdit>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeRectangle *item2 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeTextEdit *item3 = findItem<QDeclarativeTextEdit>(view->rootObject(), QLatin1String("item3"));
    QVERIFY(item0 != 0);
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Right);
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Right);
    QTest::keyClick(view, Qt::Key_Right);
    QTest::keyClick(view, Qt::Key_Right);
    QTest::keyClick(view, Qt::Key_Right);
    QTest::keyClick(view, Qt::Key_Right);
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == true);
    QVERIFY(item3->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_Down);
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == true);

    delete view;
}

void tst_qdeclarativefocusscope::forceFocus()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/forcefocus.qml"));

    QDeclarativeFocusScope *item0 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item0"));
    QDeclarativeRectangle *item1 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeRectangle *item2 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeFocusScope *item3 = findItem<QDeclarativeFocusScope>(view->rootObject(), QLatin1String("item3"));
    QDeclarativeRectangle *item4 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item4"));
    QDeclarativeRectangle *item5 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item5"));
    QVERIFY(item0 != 0);
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);
    QVERIFY(item4 != 0);
    QVERIFY(item5 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);
    QVERIFY(item4->hasActiveFocus() == false);
    QVERIFY(item5->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_4);
    QVERIFY(item0->hasActiveFocus() == true);
    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == false);
    QVERIFY(item4->hasActiveFocus() == false);
    QVERIFY(item5->hasActiveFocus() == false);

    QTest::keyClick(view, Qt::Key_5);
    QVERIFY(item0->hasActiveFocus() == false);
    QVERIFY(item1->hasActiveFocus() == false);
    QVERIFY(item2->hasActiveFocus() == false);
    QVERIFY(item3->hasActiveFocus() == true);
    QVERIFY(item4->hasActiveFocus() == false);
    QVERIFY(item5->hasActiveFocus() == true);

    delete view;
}

void tst_qdeclarativefocusscope::noParentFocus()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/chain.qml"));
    QVERIFY(view->rootObject());

    QVERIFY(view->rootObject()->property("focus1") == false);
    QVERIFY(view->rootObject()->property("focus2") == false);
    QVERIFY(view->rootObject()->property("focus3") == true);
    QVERIFY(view->rootObject()->property("focus4") == true);
    QVERIFY(view->rootObject()->property("focus5") == true);

    delete view;
}

void tst_qdeclarativefocusscope::signalEmission()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/signalEmission.qml"));

    QDeclarativeRectangle *item1 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item1"));
    QDeclarativeRectangle *item2 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item2"));
    QDeclarativeRectangle *item3 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item3"));
    QDeclarativeRectangle *item4 = findItem<QDeclarativeRectangle>(view->rootObject(), QLatin1String("item4"));
    QVERIFY(item1 != 0);
    QVERIFY(item2 != 0);
    QVERIFY(item3 != 0);
    QVERIFY(item4 != 0);

    view->show();
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVariant blue(QColor("blue"));
    QVariant red(QColor("red"));

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    item1->setFocus(true);
    QCOMPARE(item1->property("color"), red);
    QCOMPARE(item2->property("color"), blue);
    QCOMPARE(item3->property("color"), blue);
    QCOMPARE(item4->property("color"), blue);

    item2->setFocus(true);
    QCOMPARE(item1->property("color"), blue);
    QCOMPARE(item2->property("color"), red);
    QCOMPARE(item3->property("color"), blue);
    QCOMPARE(item4->property("color"), blue);

    item3->setFocus(true);
    QCOMPARE(item1->property("color"), blue);
    QCOMPARE(item2->property("color"), red);
    QCOMPARE(item3->property("color"), red);
    QCOMPARE(item4->property("color"), blue);

    item4->setFocus(true);
    QCOMPARE(item1->property("color"), blue);
    QCOMPARE(item2->property("color"), red);
    QCOMPARE(item3->property("color"), blue);
    QCOMPARE(item4->property("color"), red);

    item4->setFocus(false);
    QCOMPARE(item1->property("color"), blue);
    QCOMPARE(item2->property("color"), red);
    QCOMPARE(item3->property("color"), blue);
    QCOMPARE(item4->property("color"), blue);

    delete view;
}

void tst_qdeclarativefocusscope::qtBug13380()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/qtBug13380.qml"));

    view->show();
    QVERIFY(view->rootObject());
    qApp->setActiveWindow(view);
    qApp->processEvents();

#ifdef Q_WS_X11
    // to be safe and avoid failing setFocus with window managers
    qt_x11_wait_for_window_manager(view);
#endif

    QVERIFY(view->hasFocus());
    QVERIFY(view->scene()->hasFocus());
    QVERIFY(view->rootObject()->property("noFocus").toBool());

    view->rootObject()->setProperty("showRect", true);
    QVERIFY(view->rootObject()->property("noFocus").toBool());

    delete view;
}

void tst_qdeclarativefocusscope::forceActiveFocus()
{
    QDeclarativeView *view = new QDeclarativeView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/forceActiveFocus.qml"));

    QGraphicsObject *rootObject = view->rootObject();
    QVERIFY(rootObject);

    QDeclarativeItem *scope = findItem<QDeclarativeItem>(rootObject, QLatin1String("scope"));
    QDeclarativeItem *itemA1 = findItem<QDeclarativeItem>(rootObject, QLatin1String("item-a1"));
    QDeclarativeItem *scopeA = findItem<QDeclarativeItem>(rootObject, QLatin1String("scope-a"));
    QDeclarativeItem *itemA2 = findItem<QDeclarativeItem>(rootObject, QLatin1String("item-a2"));
    QDeclarativeItem *itemB1 = findItem<QDeclarativeItem>(rootObject, QLatin1String("item-b1"));
    QDeclarativeItem *scopeB = findItem<QDeclarativeItem>(rootObject, QLatin1String("scope-b"));
    QDeclarativeItem *itemB2 = findItem<QDeclarativeItem>(rootObject, QLatin1String("item-b2"));

    QVERIFY(scope);
    QVERIFY(itemA1);
    QVERIFY(scopeA);
    QVERIFY(itemA2);
    QVERIFY(itemB1);
    QVERIFY(scopeB);
    QVERIFY(itemB2);

    QSignalSpy rootSpy(rootObject, SIGNAL(activeFocusChanged(bool)));
    QSignalSpy scopeSpy(scope, SIGNAL(activeFocusChanged(bool)));
    QSignalSpy scopeASpy(scopeA, SIGNAL(activeFocusChanged(bool)));
    QSignalSpy scopeBSpy(scopeB, SIGNAL(activeFocusChanged(bool)));

    // First, walk the focus from item-a1 down to item-a2 and back again
    itemA1->forceActiveFocus();
    QVERIFY(itemA1->hasActiveFocus());
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    itemA2->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    itemA1->forceActiveFocus();
    QVERIFY(itemA1->hasActiveFocus());
    QVERIFY(!scopeA->hasActiveFocus());
    QVERIFY(!itemA2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 2);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    // Then jump back and forth between branch 'a' and 'b'
    itemB1->forceActiveFocus();
    QVERIFY(itemB1->hasActiveFocus());
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(!itemB1->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 3);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    scopeB->forceActiveFocus();
    QVERIFY(!scopeA->hasActiveFocus());
    QVERIFY(!itemB1->hasActiveFocus());
    QVERIFY(scopeB->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 4);
    QCOMPARE(scopeBSpy.count(), 1);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    itemA2->forceActiveFocus();
    QVERIFY(!scopeB->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 5);
    QCOMPARE(scopeBSpy.count(), 2);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    itemB2->forceActiveFocus();
    QVERIFY(!itemA2->hasActiveFocus());
    QVERIFY(itemB2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 6);
    QCOMPARE(scopeBSpy.count(), 3);
    QCOMPARE(rootSpy.count(), 1);
    QCOMPARE(scopeSpy.count(), 1);

    delete view;
}

QTEST_MAIN(tst_qdeclarativefocusscope)

#include "tst_qdeclarativefocusscope.moc"
