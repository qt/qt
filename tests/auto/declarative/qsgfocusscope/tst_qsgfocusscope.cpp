/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QSignalSpy>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qsgview.h>
#include <private/qsgrectangle_p.h>
#include <private/qsgtextedit_p.h>
#include <private/qsgtext_p.h>
#include <QtDeclarative/private/qsgfocusscope_p.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qsgfocusscope : public QObject
{
    Q_OBJECT
public:
    tst_qsgfocusscope() {}

    template<typename T>
    T *findItem(QSGItem *parent, const QString &id);

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
T *tst_qsgfocusscope::findItem(QSGItem *parent, const QString &objectName)
{
    const QMetaObject &mo = T::staticMetaObject;
    QList<QSGItem *> children = parent->childItems();
    for (int i = 0; i < children.count(); ++i) {
        QSGItem *item = children.at(i);
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

void tst_qsgfocusscope::basic()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test.qml"));

    QSGFocusScope *item0 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item0"));
    QSGRectangle *item1 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item1"));
    QSGRectangle *item2 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item2"));
    QSGRectangle *item3 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item3"));
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

void tst_qsgfocusscope::nested()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test2.qml"));

    QSGFocusScope *item1 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item1"));
    QSGFocusScope *item2 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item2"));
    QSGFocusScope *item3 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item3"));
    QSGFocusScope *item4 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item4"));
    QSGFocusScope *item5 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item5"));
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

    QVERIFY(item1->hasActiveFocus() == true);
    QVERIFY(item2->hasActiveFocus() == true);
    QVERIFY(item3->hasActiveFocus() == true);
    QVERIFY(item4->hasActiveFocus() == true);
    QVERIFY(item5->hasActiveFocus() == true);
    delete view;
}

void tst_qsgfocusscope::noFocus()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test4.qml"));

    QSGRectangle *item0 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item0"));
    QSGRectangle *item1 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item1"));
    QSGRectangle *item2 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item2"));
    QSGRectangle *item3 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item3"));
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

void tst_qsgfocusscope::textEdit()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/test5.qml"));

    QSGFocusScope *item0 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item0"));
    QSGTextEdit *item1 = findItem<QSGTextEdit>(view->rootObject(), QLatin1String("item1"));
    QSGRectangle *item2 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item2"));
    QSGTextEdit *item3 = findItem<QSGTextEdit>(view->rootObject(), QLatin1String("item3"));
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

void tst_qsgfocusscope::forceFocus()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/forcefocus.qml"));

    QSGFocusScope *item0 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item0"));
    QSGRectangle *item1 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item1"));
    QSGRectangle *item2 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item2"));
    QSGFocusScope *item3 = findItem<QSGFocusScope>(view->rootObject(), QLatin1String("item3"));
    QSGRectangle *item4 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item4"));
    QSGRectangle *item5 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item5"));
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

void tst_qsgfocusscope::noParentFocus()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/chain.qml"));
    QVERIFY(view->rootObject());

    QVERIFY(view->rootObject()->property("focus1") == false);
    QVERIFY(view->rootObject()->property("focus2") == false);
    QVERIFY(view->rootObject()->property("focus3") == true);
    QVERIFY(view->rootObject()->property("focus4") == true);
    QVERIFY(view->rootObject()->property("focus5") == true);

    delete view;
}

void tst_qsgfocusscope::signalEmission()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/signalEmission.qml"));

    QSGRectangle *item1 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item1"));
    QSGRectangle *item2 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item2"));
    QSGRectangle *item3 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item3"));
    QSGRectangle *item4 = findItem<QSGRectangle>(view->rootObject(), QLatin1String("item4"));
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

void tst_qsgfocusscope::qtBug13380()
{
    QSGView *view = new QSGView;
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
    QVERIFY(view->rootObject()->property("noFocus").toBool());

    view->rootObject()->setProperty("showRect", true);
    QVERIFY(view->rootObject()->property("noFocus").toBool());

    delete view;
}

void tst_qsgfocusscope::forceActiveFocus()
{
    QSGView *view = new QSGView;
    view->setSource(QUrl::fromLocalFile(SRCDIR "/data/forceActiveFocus.qml"));

    QSGItem *rootObject = view->rootObject();
    QVERIFY(rootObject);

    QSGItem *scope = findItem<QSGItem>(rootObject, QLatin1String("scope"));
    QSGItem *itemA1 = findItem<QSGItem>(rootObject, QLatin1String("item-a1"));
    QSGItem *scopeA = findItem<QSGItem>(rootObject, QLatin1String("scope-a"));
    QSGItem *itemA2 = findItem<QSGItem>(rootObject, QLatin1String("item-a2"));
    QSGItem *itemB1 = findItem<QSGItem>(rootObject, QLatin1String("item-b1"));
    QSGItem *scopeB = findItem<QSGItem>(rootObject, QLatin1String("scope-b"));
    QSGItem *itemB2 = findItem<QSGItem>(rootObject, QLatin1String("item-b2"));

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
    QVERIFY(!rootObject->hasActiveFocus());
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    itemA2->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 1);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    itemA1->forceActiveFocus();
    QVERIFY(itemA1->hasActiveFocus());
    QVERIFY(!scopeA->hasActiveFocus());
    QVERIFY(!itemA2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 2);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    // Then jump back and forth between branch 'a' and 'b'
    itemB1->forceActiveFocus();
    QVERIFY(itemB1->hasActiveFocus());
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    scopeA->forceActiveFocus();
    QVERIFY(!itemA1->hasActiveFocus());
    QVERIFY(!itemB1->hasActiveFocus());
    QVERIFY(scopeA->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 3);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    scopeB->forceActiveFocus();
    QVERIFY(!scopeA->hasActiveFocus());
    QVERIFY(!itemB1->hasActiveFocus());
    QVERIFY(scopeB->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 4);
    QCOMPARE(scopeBSpy.count(), 1);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    itemA2->forceActiveFocus();
    QVERIFY(!scopeB->hasActiveFocus());
    QVERIFY(itemA2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 5);
    QCOMPARE(scopeBSpy.count(), 2);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    itemB2->forceActiveFocus();
    QVERIFY(!itemA2->hasActiveFocus());
    QVERIFY(itemB2->hasActiveFocus());
    QCOMPARE(scopeASpy.count(), 6);
    QCOMPARE(scopeBSpy.count(), 3);
    QCOMPARE(rootSpy.count(), 0);
    QCOMPARE(scopeSpy.count(), 1);

    delete view;
}

QTEST_MAIN(tst_qsgfocusscope)

#include "tst_qsgfocusscope.moc"
