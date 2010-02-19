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

#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <private/qmlgraphicsmouseregion_p.h>
#include <QtDeclarative/qmlview.h>

class tst_QmlGraphicsMouseRegion: public QObject
{
    Q_OBJECT
private slots:
    void dragProperties();
private:
    QmlView *createView(const QString &filename);
};

void tst_QmlGraphicsMouseRegion::dragProperties()
{
    QmlView *canvas = createView(SRCDIR "/data/dragproperties.qml");
    canvas->execute();
    canvas->show();
    canvas->setFocus();
    QVERIFY(canvas->rootObject() != 0);

    QmlGraphicsMouseRegion *mouseRegion = canvas->rootObject()->findChild<QmlGraphicsMouseRegion*>("mouseregion");
    QmlGraphicsDrag *drag = mouseRegion->drag();
    QVERIFY(mouseRegion != 0);
    QVERIFY(drag != 0);

    // target
    QmlGraphicsItem *blackRect = canvas->rootObject()->findChild<QmlGraphicsItem*>("blackrect");
    QVERIFY(blackRect != 0);
    QVERIFY(blackRect == drag->target());
    QmlGraphicsItem *rootItem = qobject_cast<QmlGraphicsItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);
    QSignalSpy targetSpy(drag, SIGNAL(targetChanged()));
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);
    drag->setTarget(rootItem);
    QCOMPARE(targetSpy.count(),1);

    // axis
    QCOMPARE(drag->axis(), QmlGraphicsDrag::XandYAxis);
    QSignalSpy axisSpy(drag, SIGNAL(axisChanged()));
    drag->setAxis(QmlGraphicsDrag::XAxis);
    QCOMPARE(drag->axis(), QmlGraphicsDrag::XAxis);
    QCOMPARE(axisSpy.count(),1);
    drag->setAxis(QmlGraphicsDrag::XAxis);
    QCOMPARE(axisSpy.count(),1);

    // minimum and maximum properties
    QSignalSpy xminSpy(drag, SIGNAL(minimumXChanged()));
    QSignalSpy xmaxSpy(drag, SIGNAL(maximumXChanged()));
    QSignalSpy yminSpy(drag, SIGNAL(minimumYChanged()));
    QSignalSpy ymaxSpy(drag, SIGNAL(maximumYChanged()));

    QCOMPARE(drag->xmin(), 0.0);
    QCOMPARE(drag->xmax(), rootItem->width()-blackRect->width());
    QCOMPARE(drag->ymin(), 0.0);
    QCOMPARE(drag->ymax(), rootItem->height()-blackRect->height());

    drag->setXmin(10);
    drag->setXmax(10);
    drag->setYmin(10);
    drag->setYmax(10);

    QCOMPARE(drag->xmin(), 10.0);
    QCOMPARE(drag->xmax(), 10.0);
    QCOMPARE(drag->ymin(), 10.0);
    QCOMPARE(drag->ymax(), 10.0);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);

    drag->setXmin(10);
    drag->setXmax(10);
    drag->setYmin(10);
    drag->setYmax(10);

    QCOMPARE(xminSpy.count(),1);
    QCOMPARE(xmaxSpy.count(),1);
    QCOMPARE(yminSpy.count(),1);
    QCOMPARE(ymaxSpy.count(),1);
}

QmlView *tst_QmlGraphicsMouseRegion::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}

QTEST_MAIN(tst_QmlGraphicsMouseRegion)

#include "tst_qmlgraphicsmouseregion.moc"
