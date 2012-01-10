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
#include "../../../shared/util.h"
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeitem.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>

class tst_qdeclarativeapplication : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeapplication();

private slots:
    void active();
    void layoutDirection();

private:
    QDeclarativeEngine engine;
};

tst_qdeclarativeapplication::tst_qdeclarativeapplication()
{
}

void tst_qdeclarativeapplication::active()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 1.0; Item { property bool active: Qt.application.active }", QUrl::fromLocalFile(""));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(component.create());
    QVERIFY(item);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    scene.addItem(item);

    // not active
    QVERIFY(!item->property("active").toBool());
    QCOMPARE(item->property("active").toBool(), QApplication::activeWindow() != 0);

    // active
    view.show();
    QApplication::setActiveWindow(&view);
    QTest::qWaitForWindowShown(&view);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(&view));
    QCOMPARE(item->property("active").toBool(), QApplication::activeWindow() != 0);

    // not active again
    // on mac, setActiveWindow(0) on mac does not deactivate the current application
    // (you have to switch to a different app or hide the current app to trigger this)
#if !defined(Q_WS_MAC)
    QApplication::setActiveWindow(0);
    QVERIFY(!item->property("active").toBool());
    QCOMPARE(item->property("active").toBool(), QApplication::activeWindow() != 0);
#endif
}

void tst_qdeclarativeapplication::layoutDirection()
{
    QDeclarativeComponent component(&engine);
    component.setData("import QtQuick 1.0; Item { property bool layoutDirection: Qt.application.layoutDirection }", QUrl::fromLocalFile(""));
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(component.create());
    QVERIFY(item);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    scene.addItem(item);

    // not mirrored
    QCOMPARE(Qt::LayoutDirection(item->property("layoutDirection").toInt()), Qt::LeftToRight);

    // mirrored
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QCOMPARE(Qt::LayoutDirection(item->property("layoutDirection").toInt()), Qt::RightToLeft);

    // not mirrored again
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QCOMPARE(Qt::LayoutDirection(item->property("layoutDirection").toInt()), Qt::LeftToRight);
}

QTEST_MAIN(tst_qdeclarativeapplication)

#include "tst_qdeclarativeapplication.moc"
