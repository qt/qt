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
#include "qmlruntime.h"

class tst_QDeclarativeViewer : public QObject

{
    Q_OBJECT
public:
    tst_QDeclarativeViewer();

private slots:
    void orientation();

private:
    QDeclarativeEngine engine;
};

tst_QDeclarativeViewer::tst_QDeclarativeViewer()
{
}

void tst_QDeclarativeViewer::orientation()
{
    QWidget window;
    QDeclarativeViewer *viewer = new QDeclarativeViewer(&window);
    QVERIFY(viewer);
    viewer->open(SRCDIR "/data/orientation.qml");
    QVERIFY(viewer->view());
    QVERIFY(viewer->menuBar());
    QDeclarativeItem* rootItem = qobject_cast<QDeclarativeItem*>(viewer->view()->rootObject());
    QVERIFY(rootItem);
    window.show();

    QCOMPARE(rootItem->width(), 200.0);
    QCOMPARE(rootItem->height(), 300.0);
    QCOMPARE(viewer->view()->size(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300));
    QCOMPARE(viewer->size(), QSize(200, 300+viewer->menuBar()->height()));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->toggleOrientation();
    qApp->processEvents();

    QCOMPARE(rootItem->width(), 300.0);
    QCOMPARE(rootItem->height(), 200.0);
    QCOMPARE(viewer->view()->size(), QSize(300, 200));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(300, 200));
    QCOMPARE(viewer->size(), QSize(300, 200+viewer->menuBar()->height()));
    QCOMPARE(viewer->size(), viewer->sizeHint());

    viewer->toggleOrientation();
    qApp->processEvents();

    QCOMPARE(rootItem->width(), 200.0);
    QCOMPARE(rootItem->height(), 300.0);
    QCOMPARE(viewer->view()->size(), QSize(200, 300));
    QCOMPARE(viewer->view()->sceneRect().size(), QSizeF(200, 300));
    QCOMPARE(viewer->size(), QSize(200, 300+viewer->menuBar()->height()));
    QCOMPARE(viewer->size(), viewer->sizeHint());
}

QTEST_MAIN(tst_QDeclarativeViewer)

#include "tst_qdeclarativeviewer.moc"
