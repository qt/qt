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
#include <QtTest/QtTest>
#include <private/qlistmodelinterface_p.h>
#include <qmlview.h>
#include <private/qmlgraphicslayoutitem_p.h>
#include <qmlexpression.h>
#include <QStyle>

class tst_QmlGraphicsLayouts : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsLayouts();

private slots:
    void test_qml();//GraphicsLayout set up in Qml
    void test_cpp();//GraphicsLayout set up in C++

private:
    QmlView *createView(const QString &filename);
};

tst_QmlGraphicsLayouts::tst_QmlGraphicsLayouts()
{
}

void tst_QmlGraphicsLayouts::test_qml()
{
    QmlView *canvas = createView(SRCDIR "/data/layouts.qml");

    qApp->processEvents();
    QmlGraphicsLayoutItem *left = static_cast<QmlGraphicsLayoutItem*>(canvas->rootObject()->findChild<QmlGraphicsItem*>("left"));
    QVERIFY(left != 0);

    QmlGraphicsLayoutItem *right = static_cast<QmlGraphicsLayoutItem*>(canvas->rootObject()->findChild<QmlGraphicsItem*>("right"));
    QVERIFY(right != 0);

    qreal l = QApplication::style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
    qreal r = QApplication::style()->pixelMetric(QStyle::PM_LayoutRightMargin);
    qreal t = QApplication::style()->pixelMetric(QStyle::PM_LayoutTopMargin);
    qreal b = QApplication::style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
    QVERIFY2(l == r && r == t && t == b, "Test assumes equal margins.");
    qreal gvMargin = l;

    QmlGraphicsItem *rootItem = qobject_cast<QmlGraphicsItem*>(canvas->rootObject());
    QVERIFY(rootItem != 0);

    //Preferred Size
    rootItem->setWidth(300 + 2*gvMargin);
    rootItem->setHeight(300 + 2*gvMargin);

    QCOMPARE(left->x(), gvMargin);
    QCOMPARE(left->y(), gvMargin);
    QCOMPARE(left->width(), 100.0);
    QCOMPARE(left->height(), 300.0);

    QCOMPARE(right->x(), 100.0 + gvMargin);
    QCOMPARE(right->y(), 0.0 + gvMargin);
    QCOMPARE(right->width(), 200.0);
    QCOMPARE(right->height(), 300.0);

    //Minimum Size
    rootItem->setWidth(10+2*gvMargin);
    rootItem->setHeight(10+2*gvMargin);

    QCOMPARE(left->x(), gvMargin);
    QCOMPARE(left->width(), 100.0);
    QCOMPARE(left->height(), 100.0);

    QCOMPARE(right->x(), 100.0 + gvMargin);
    QCOMPARE(right->width(), 100.0);
    QCOMPARE(right->height(), 100.0);

    //Between preferred and Maximum Size
    /*Note that if set to maximum size (or above) GraphicsLinearLayout behavior
      is to shrink them down to preferred size. So the exact maximum size can't
      be used*/
    rootItem->setWidth(670 + 2*gvMargin);
    rootItem->setHeight(300 + 2*gvMargin);

    QCOMPARE(left->x(), gvMargin);
    QCOMPARE(left->width(), 270.0);
    QCOMPARE(left->height(), 300.0);

    QCOMPARE(right->x(), 270.0 + gvMargin);
    QCOMPARE(right->width(), 400.0);
    QCOMPARE(right->height(), 300.0);

    delete canvas;
}

void tst_QmlGraphicsLayouts::test_cpp()
{
    //TODO: Waiting on QT-2407 to write this test
}

QmlView *tst_QmlGraphicsLayouts::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}


QTEST_MAIN(tst_QmlGraphicsLayouts)

#include "tst_layouts.moc"
