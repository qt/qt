/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneindex.h>
#include <private/qgraphicsscene_bsp_p.h>


//TESTED_CLASS=
//TESTED_FILES=

class tst_QGraphicsSceneIndex : public QObject
{
    Q_OBJECT
public slots:
    void initTestCase();

private slots:
    void sceneRect_data();
    void sceneRect();
    void customIndex_data();
    void customIndex();
    void scatteredItems_data();
    void scatteredItems();

private:
    void common_data();
    QGraphicsSceneIndex *createIndex(const QString &name);
};

void tst_QGraphicsSceneIndex::initTestCase()
{
}

void tst_QGraphicsSceneIndex::common_data()
{
    QTest::addColumn<QString>("indexMethod");

    QTest::newRow("BSP") << QString("bsp");
}

QGraphicsSceneIndex *tst_QGraphicsSceneIndex::createIndex(const QString &indexMethod)
{
    QGraphicsSceneIndex *index = 0;

    if (indexMethod == "bsp")
        index = new QGraphicsSceneBspTree;

    return index;
}

void tst_QGraphicsSceneIndex::sceneRect_data()
{
    common_data();
}

void tst_QGraphicsSceneIndex::sceneRect()
{
    QGraphicsSceneIndex *index = new QGraphicsSceneBspTree;
    index->setRect(QRectF(0, 0, 2000, 2000));
    QCOMPARE(index->rect(), QRectF(0, 0, 2000, 2000));
}

void tst_QGraphicsSceneIndex::customIndex_data()
{
    common_data();
}

void tst_QGraphicsSceneIndex::customIndex()
{
    QFETCH(QString, indexMethod);
    QGraphicsSceneIndex *index = createIndex(indexMethod);

    QGraphicsScene scene;
    scene.setSceneIndex(index);

    scene.addRect(0, 0, 30, 40);
    QCOMPARE(scene.items(QRectF(0, 0, 10, 10)).count(), 1);
}

void tst_QGraphicsSceneIndex::scatteredItems_data()
{
    common_data();
}

void tst_QGraphicsSceneIndex::scatteredItems()
{
    QFETCH(QString, indexMethod);
    QGraphicsSceneIndex *index = createIndex(indexMethod);

    QGraphicsScene scene;
    scene.setSceneIndex(index);

    for (int i = 0; i < 10; ++i)
        scene.addRect(i*50, i*50, 40, 35);

    QCOMPARE(scene.items(QRectF(0, 0, 10, 10)).count(), 1);
    QCOMPARE(scene.items(QRectF(0, 0, 1000, 1000)).count(), 10);
    QCOMPARE(scene.items(QRectF(-100, -1000, 0, 0)).count(), 0);
}

QTEST_MAIN(tst_QGraphicsSceneIndex)
#include "tst_qgraphicssceneindex.moc"
