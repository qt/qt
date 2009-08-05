/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
****************************************************************************/

#include <qtest.h>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>

//TESTED_FILES=

class tst_QGraphicsItem : public QObject
{
    Q_OBJECT

public:
    tst_QGraphicsItem();
    virtual ~tst_QGraphicsItem();

public slots:
    void init();
    void cleanup();

private slots:
    void setPos_data();
    void setPos();
    void setTransform_data();
    void setTransform();
    void rotate();
    void scale();
    void shear();
    void translate();
    void setRotation();
};

tst_QGraphicsItem::tst_QGraphicsItem()
{
}

tst_QGraphicsItem::~tst_QGraphicsItem()
{
}

void tst_QGraphicsItem::init()
{
}

void tst_QGraphicsItem::cleanup()
{
}

void tst_QGraphicsItem::setPos_data()
{
    QTest::addColumn<QPointF>("pos");

    QTest::newRow("0, 0") << QPointF(0, 0);
    QTest::newRow("10, 10") << QPointF(10, 10);
    QTest::newRow("-10, -10") << QPointF(-10, -10);
}

void tst_QGraphicsItem::setPos()
{
    QFETCH(QPointF, pos);

    QGraphicsScene scene;
    QGraphicsRectItem *rect = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        rect->setPos(10, 10);
        rect->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::setTransform_data()
{
    QTest::addColumn<QTransform>("transform");

    QTest::newRow("id") << QTransform();
    QTest::newRow("rotate 45z") << QTransform().rotate(45);
    QTest::newRow("scale 2x2") << QTransform().scale(2, 2);
    QTest::newRow("translate 100, 100") << QTransform().translate(100, 100);
    QTest::newRow("rotate 45x 45y 45z") << QTransform().rotate(45, Qt::XAxis)
        .rotate(45, Qt::YAxis).rotate(45, Qt::ZAxis);
}

void tst_QGraphicsItem::setTransform()
{
    QFETCH(QTransform, transform);

    QGraphicsScene scene;
    QGraphicsRectItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->setTransform(transform);
        item->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::rotate()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->rotate(45);
        item->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::scale()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->scale(2, 2);
        item->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::shear()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->shear(1.5, 1.5);
        item->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::translate()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->translate(100, 100);
        item->transform(); // prevent lazy optimizing
    }
}

void tst_QGraphicsItem::setRotation()
{
    QGraphicsScene scene;
    QGraphicsItem *item = scene.addRect(QRectF(0, 0, 100, 100));

    QBENCHMARK {
        item->setRotation(45);
        item->transform(); // prevent lazy optimizing
    }
}

QTEST_MAIN(tst_QGraphicsItem)
#include "tst_qgraphicsitem.moc"
