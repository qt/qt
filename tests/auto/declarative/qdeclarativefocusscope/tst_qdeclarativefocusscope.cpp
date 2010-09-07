/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
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

QTEST_MAIN(tst_qdeclarativefocusscope)

#include "tst_qdeclarativefocusscope.moc"
