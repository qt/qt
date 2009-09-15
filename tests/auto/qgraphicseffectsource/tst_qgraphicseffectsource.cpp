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
#include <QtGui/qgraphicseffect.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicsitem.h>
#include <QtGui/qstyleoption.h>

//TESTED_CLASS=
//TESTED_FILES=

class CustomItem : public QGraphicsRectItem
{
public:
    CustomItem(qreal x, qreal y, qreal width, qreal height)
        : QGraphicsRectItem(x, y, width, height), numRepaints(0),
          m_painter(0), m_styleOption(0)
    {}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        m_painter = painter;
        m_styleOption = option;
        ++numRepaints;
        QGraphicsRectItem::paint(painter, option, widget);
    }

    void reset()
    {
        numRepaints = 0;
        m_painter = 0;
        m_styleOption = 0;
    }

    int numRepaints;
    QPainter *m_painter;
    const QStyleOption *m_styleOption;
};

class CustomEffect : public QGraphicsEffect
{
public:
    CustomEffect()
        : QGraphicsEffect(), numRepaints(0), m_margin(10), m_sourceChanged(false),
          m_sourceBoundingRectChanged(false), doNothingInDraw(false),
          storeDeviceDependentStuff(false),
          m_painter(0), m_styleOption(0), m_source(0)
    {}

    QRectF boundingRectFor(const QRectF &rect) const
    { return rect.adjusted(-m_margin, -m_margin, m_margin, m_margin); }

    void reset()
    {
        numRepaints = 0;
        m_sourceChanged = false;
        m_sourceBoundingRectChanged = false;
        m_painter = 0;
        m_styleOption = 0;
        m_source = 0;
        deviceCoordinatesPixmap = QPixmap();
        deviceRect = QRect();
        sourceDeviceBoundingRect = QRectF();
    }

    void setMargin(int margin)
    {
        m_margin = margin;
        updateBoundingRect();
    }

    int margin() const
    { return m_margin; }

    void draw(QPainter *painter, QGraphicsEffectSource *source)
    {
        ++numRepaints;
        if (storeDeviceDependentStuff) {
            deviceCoordinatesPixmap = source->pixmap(Qt::DeviceCoordinates);
            deviceRect = source->deviceRect();
            sourceDeviceBoundingRect = source->boundingRect(Qt::DeviceCoordinates);
        }
        if (doNothingInDraw)
            return;
        m_source = source;
        m_painter = painter;
        m_styleOption = source->styleOption();
        source->draw(painter);
    }

    void sourceChanged()
    { m_sourceChanged = true; }

    void sourceBoundingRectChanged()
    { m_sourceBoundingRectChanged = true; }

    int numRepaints;
    int m_margin;
    bool m_sourceChanged;
    bool m_sourceBoundingRectChanged;
    bool doNothingInDraw;
    bool storeDeviceDependentStuff;
    QPainter *m_painter;
    const QStyleOption *m_styleOption;
    QGraphicsEffectSource *m_source;
    QPixmap deviceCoordinatesPixmap;
    QRect deviceRect;
    QRectF sourceDeviceBoundingRect;
};

class tst_QGraphicsEffectSource : public QObject
{
    Q_OBJECT
public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

private slots:
    void graphicsItem();
    void styleOption();
    void isPixmap();
    void draw();
    void update();
    void boundingRect();
    void deviceRect();
    void pixmap();

private:
    QGraphicsView *view;
    QGraphicsScene *scene;
    CustomItem *item;
    CustomEffect *effect;
};

void tst_QGraphicsEffectSource::initTestCase()
{
    scene = new QGraphicsScene;
    item = new CustomItem(0, 0, 100, 100);
    effect = new CustomEffect;
    item->setGraphicsEffect(effect);
    scene->addItem(item);
    view = new QGraphicsView(scene);
    view->show();
#ifdef Q_WS_X11
    qt_x11_wait_for_window_manager(view);
#endif
    QTest::qWait(200);
}

void tst_QGraphicsEffectSource::cleanupTestCase()
{
    delete view;
}

void tst_QGraphicsEffectSource::init()
{
    QVERIFY(effect);
    QVERIFY(item);
    QVERIFY(effect->source());
    effect->reset();
    effect->storeDeviceDependentStuff = false;
    effect->doNothingInDraw = false;
    item->reset();
}

void tst_QGraphicsEffectSource::graphicsItem()
{
    QVERIFY(effect->source());
    QCOMPARE(effect->source()->graphicsItem(), item);
}

void tst_QGraphicsEffectSource::styleOption()
{
    // We don't have style options unless the source is drawing.
    QVERIFY(effect->source());
    QVERIFY(!effect->source()->styleOption());

    // Trigger an update and check that the style option in QGraphicsEffect::draw
    // was the same as the one in QGraphicsItem::paint.
    QCOMPARE(item->numRepaints, 0);
    QCOMPARE(effect->numRepaints, 0);
    item->update();
    QTest::qWait(50);
    QCOMPARE(item->numRepaints, 1);
    QCOMPARE(effect->numRepaints, 1);
    QVERIFY(effect->m_styleOption);
    QCOMPARE(effect->m_styleOption, item->m_styleOption);
}

void tst_QGraphicsEffectSource::isPixmap()
{
    // Current source is a CustomItem (which is not a pixmap item).
    QVERIFY(!effect->source()->isPixmap());

    // Make sure isPixmap() returns true for QGraphicsPixmapItem.
    QGraphicsPixmapItem *pixmapItem = new QGraphicsPixmapItem;
    CustomEffect *anotherEffect = new CustomEffect;
    pixmapItem->setGraphicsEffect(anotherEffect);
    QVERIFY(anotherEffect->source());
    QCOMPARE(anotherEffect->source()->graphicsItem(), static_cast<QGraphicsItem *>(pixmapItem));
    QVERIFY(anotherEffect->source()->isPixmap());
    delete pixmapItem;
}

void tst_QGraphicsEffectSource::draw()
{
    // The source can only draw as a result of QGraphicsEffect::draw.
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsEffectSource::draw: Can only begin as a result of QGraphicsEffect::draw");
    QPixmap dummyPixmap(10, 10);
    QPainter dummyPainter(&dummyPixmap);
    effect->source()->draw(&dummyPainter);
}

void tst_QGraphicsEffectSource::update()
{
    QCOMPARE(item->numRepaints, 0);
    QCOMPARE(effect->numRepaints, 0);

    effect->source()->update();
    QTest::qWait(50);

    QCOMPARE(item->numRepaints, 1);
    QCOMPARE(effect->numRepaints, 1);
}

void tst_QGraphicsEffectSource::boundingRect()
{
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsEffectSource::boundingRect: Not yet implemented, lacking device context");
    QCOMPARE(effect->source()->boundingRect(Qt::DeviceCoordinates), QRectF());

    QRectF itemBoundingRect = item->boundingRect();
    if (!item->children().isEmpty())
        itemBoundingRect |= item->childrenBoundingRect();

    // We can at least check that the device bounding rect was correct in QGraphicsEffect::draw.
    effect->storeDeviceDependentStuff = true;
    effect->source()->update();
    QTest::qWait(50);
    const QTransform deviceTransform = item->deviceTransform(view->viewportTransform());
    QCOMPARE(effect->sourceDeviceBoundingRect, deviceTransform.mapRect(itemBoundingRect));

    // Bounding rect in logical coordinates is of course fine.
    QCOMPARE(effect->source()->boundingRect(Qt::LogicalCoordinates), itemBoundingRect);
    // Make sure default value is Qt::LogicalCoordinates.
    QCOMPARE(effect->source()->boundingRect(), itemBoundingRect);
}

void tst_QGraphicsEffectSource::deviceRect()
{
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsEffectSource::deviceRect: Not yet implemented, lacking device context");
    QCOMPARE(effect->source()->deviceRect(), QRect());

    // We can at least check that the rect was correct in QGraphicsEffect::draw.
    effect->storeDeviceDependentStuff = true;
    effect->source()->update();
    QTest::qWait(50);
    QCOMPARE(effect->deviceRect, view->viewport()->rect());
}

void tst_QGraphicsEffectSource::pixmap()
{
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsEffectSource::pixmap: Not yet implemented, lacking device context");
    QCOMPARE(effect->source()->pixmap(Qt::DeviceCoordinates), QPixmap());

    // We can at least verify a valid pixmap from QGraphicsEffect::draw.
    effect->storeDeviceDependentStuff = true;
    effect->source()->update();
    QTest::qWait(50);
    QVERIFY(!effect->deviceCoordinatesPixmap.isNull());

    // Pixmaps in logical coordinates we can do fine.
    QPixmap pixmap1 = effect->source()->pixmap(Qt::LogicalCoordinates);
    QVERIFY(!pixmap1.isNull());

    // Make sure default value is Qt::LogicalCoordinates.
    QPixmap pixmap2 = effect->source()->pixmap();
    QCOMPARE(pixmap1, pixmap2);
}

QTEST_MAIN(tst_QGraphicsEffectSource)
#include "tst_qgraphicseffectsource.moc"

