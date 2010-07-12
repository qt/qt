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
#include <QtDeclarative/qdeclarativeview.h>
#include <QtDeclarative/qdeclarativeitem.h>
#include <QtGui/qmenubar.h>
#include "../../../shared/util.h"
#include "qmlruntime.h"
#include "../../../shared/util.h"

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

#if defined(Q_OS_MAC) || defined(Q_WS_MAEMO_5) || defined(Q_WS_S60)
#  define MENUBAR_HEIGHT(mw) 0
#else
#  define MENUBAR_HEIGHT(mw) (mw->menuBar()->height())
#endif

class tst_QDeclarativeViewer : public QObject

{
    Q_OBJECT
public:
    tst_QDeclarativeViewer();

private slots:
    void orientation();
    void loading();
    void fileBrowser();
    void resizing();
    void paths();
    void slowMode();

private:
    QDeclarativeEngine engine;
};

tst_QDeclarativeViewer::tst_QDeclarativeViewer()
{
}

#define TEST_INITIAL_SIZES(viewer) { \
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject()); \
    QVERIFY(rootItem); \
\
    QCOMPARE(rootItem->width(), 200.0); \
    QCOMPARE(rootItem->height(), 300.0); \
    QTRY_COMPARE(viewer->view()->size(), QSize(200, 300)); \
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300)); \
    QCOMPARE(viewer->size(), QSize(200, 300 + MENUBAR_HEIGHT(viewer))); \
    QCOMPARE(viewer->size(), viewer->sizeHint()); \
}

void tst_QDeclarativeViewer::orientation()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);
    viewer->open(SRCDIR "/data/orientation.qml");
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);
    viewer->show();

    QApplication::setActiveWindow(viewer);
    QTest::qWaitForWindowShown(viewer);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(viewer));

    TEST_INITIAL_SIZES(viewer);

    viewer->rotateOrientation();
    qApp->processEvents();

    QCOMPARE(rootItem->width(), 300.0);
    QCOMPARE(rootItem->height(), 200.0);
    QTRY_COMPARE(viewer->view()->size(), QSize(300, 200));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(300, 200));
    QCOMPARE(viewer->size(), QSize(300, 200 + MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->rotateOrientation();
    qApp->processEvents();

    QCOMPARE(rootItem->width(), 200.0);
    QCOMPARE(rootItem->height(), 300.0);
    QTRY_COMPARE(viewer->view()->size(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300));
    QCOMPARE(viewer->size(), QSize(200, 300 + MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), viewer->sizeHint());
}

void tst_QDeclarativeViewer::loading()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);
    viewer->setSizeToView(true);
    viewer->open(SRCDIR "/data/orientation.qml");
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);
    viewer->show();

    QApplication::setActiveWindow(viewer);
    QTest::qWaitForWindowShown(viewer);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(viewer));

    TEST_INITIAL_SIZES(viewer);

    viewer->resize(QSize(400, 500));
    qApp->processEvents();

    // window resized
    QTRY_COMPARE(rootItem->width(), 400.0);
    QTRY_COMPARE(rootItem->height(), 500.0 - MENUBAR_HEIGHT(viewer));
    QCOMPARE(viewer->view()->size(), QSize(400, 500 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(400, 500 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), QSize(400, 500));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->reload();
    rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);

    // reload cause the window to return back to initial size
    QTRY_COMPARE(rootItem->width(), 200.0);
    QTRY_COMPARE(rootItem->height(), 300.0);
    QCOMPARE(viewer->view()->size(), QSize(200, 300));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300));
    QCOMPARE(viewer->size(), QSize(200, 300 + MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->resize(QSize(400, 500));
    qApp->processEvents();

    // window resized again
    QTRY_COMPARE(rootItem->width(), 400.0);
    QTRY_COMPARE(rootItem->height(), 500.0 - MENUBAR_HEIGHT(viewer));
    QCOMPARE(viewer->view()->size(), QSize(400, 500 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(400, 500 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), QSize(400, 500));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->open(SRCDIR "/data/orientation.qml");
    rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);

    // open also causes the window to return back to initial size
    QTRY_COMPARE(rootItem->width(), 200.0);
    QTRY_COMPARE(rootItem->height(), 300.0);
    QCOMPARE(viewer->view()->size(), QSize(200, 300));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300));
    QCOMPARE(viewer->size(), QSize(200, 300 + MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), viewer->sizeHint());
}

void tst_QDeclarativeViewer::fileBrowser()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);
    viewer->setUseNativeFileBrowser(false);
    viewer->openFile();
    viewer->show();

    QApplication::setActiveWindow(viewer);
    QTest::qWaitForWindowShown(viewer);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(viewer));

    // Browser.qml successfully loaded
    QDeclarativeItem* browserItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QVERIFY(browserItem);

    // load something
    viewer->open(SRCDIR "/data/orientation.qml");
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);
    QVERIFY(browserItem != rootItem);

    // go back to Browser.qml
    viewer->openFile();
    browserItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QVERIFY(browserItem);
}

void tst_QDeclarativeViewer::resizing()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);
    viewer->open(SRCDIR "/data/orientation.qml");
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);
    viewer->show();

    QApplication::setActiveWindow(viewer);
    QTest::qWaitForWindowShown(viewer);
    QTRY_COMPARE(QApplication::activeWindow(), static_cast<QWidget *>(viewer));

    TEST_INITIAL_SIZES(viewer);

    viewer->setSizeToView(false);

    // size view to root object
    rootItem->setWidth(150);
    rootItem->setHeight(200);
    qApp->processEvents();

    QCOMPARE(rootItem->width(), 150.0);
    QCOMPARE(rootItem->height(), 200.0);
    QTRY_COMPARE(viewer->view()->size(), QSize(150, 200));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(150, 200));
    QCOMPARE(viewer->size(), QSize(150, 200 + MENUBAR_HEIGHT(viewer)));

    // do not size root object to view
    viewer->resize(QSize(180,250));
    QCOMPARE(rootItem->width(), 150.0);
    QCOMPARE(rootItem->height(), 200.0);

    viewer->setSizeToView(true);

    // size root object to view
    viewer->resize(QSize(250,350));
    qApp->processEvents();

    QTRY_COMPARE(rootItem->width(), 250.0);
    QTRY_COMPARE(rootItem->height(), 350.0 - MENUBAR_HEIGHT(viewer));
    QTRY_COMPARE(viewer->view()->size(), QSize(250, 350 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->view()->initialSize(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(250, 350 - MENUBAR_HEIGHT(viewer)));
    QCOMPARE(viewer->size(), QSize(250, 350));

    // do not size view to root object
    rootItem->setWidth(150);
    rootItem->setHeight(200);
    QTRY_COMPARE(viewer->size(), QSize(250, 350));
}

void tst_QDeclarativeViewer::paths()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);

    viewer->addLibraryPath("miscImportPath");
    viewer->view()->engine()->importPathList().contains("miscImportPath");

    viewer->addPluginPath("miscPluginPath");
    viewer->view()->engine()->pluginPathList().contains("miscPluginPath");
}

void tst_QDeclarativeViewer::slowMode()
{
    QDeclarativeViewer *viewer = new QDeclarativeViewer();
    QVERIFY(viewer);

    viewer->setSlowMode(true);
    viewer->setSlowMode(false);
}

QTEST_MAIN(tst_QDeclarativeViewer)

#include "tst_qdeclarativeviewer.moc"
