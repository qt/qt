/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qdrag.h>

#ifdef Q_WS_X11
#include <QBitmap>
#include <QPainter>
#include <QX11Info>

#include <X11/extensions/shape.h>
#endif

//TESTED_CLASS=
//TESTED_FILES=

class DragEnterCounterWidget : public QWidget
{
public:
    DragEnterCounterWidget();

    void dragEnterEvent(QDragEnterEvent * event);

    int enterEvents;
};

class DragCounterAndCreatorWidget : public DragEnterCounterWidget
{
public:
    DragCounterAndCreatorWidget();

    void mousePressEvent(QMouseEvent * event);

    int startedDrags;
};

class BiggerDragCounterWidget : public DragEnterCounterWidget
{
public:
    BiggerDragCounterWidget();
};

DragEnterCounterWidget::DragEnterCounterWidget() : QWidget(0), enterEvents(0)
{
    setAcceptDrops(true);
    setWindowFlags(Qt::FramelessWindowHint);
    show();
}

void DragEnterCounterWidget::dragEnterEvent(QDragEnterEvent * event)
{
    event->acceptProposedAction();
    ++enterEvents;
}

DragCounterAndCreatorWidget::DragCounterAndCreatorWidget() : startedDrags(0)
{
    resize(80, 80);
    move(300, 300);
}

void DragCounterAndCreatorWidget::mousePressEvent(QMouseEvent * /*event*/)
{
    ++startedDrags;
    QDrag *drag = new QDrag(this);
    drag->setMimeData(new QMimeData);
    QPixmap p("dummy.png");
    drag->setHotSpot(QPoint( p.width() / 2, p.height() / 2 ));
    drag->setPixmap(p);
    drag->exec();
}

BiggerDragCounterWidget::BiggerDragCounterWidget()
{
    resize(180, 180);
    move(250, 250);
}

class DoMouseReleaseHelper : public QTimer
{
Q_OBJECT

public:
    DoMouseReleaseHelper(QWidget *w, int timeout = 0);

private slots:
    void doMouseRelease();

private:
    QWidget *m_w;
};

DoMouseReleaseHelper::DoMouseReleaseHelper(QWidget *w, int timeout) : m_w(w)
{
    setSingleShot(true);
    start(timeout);
    connect(this, SIGNAL(timeout()), this, SLOT(doMouseRelease()));
}

void DoMouseReleaseHelper::doMouseRelease()
{
    QTest::mouseRelease(m_w, Qt::LeftButton);
}

class DoMouseMoveHelper : public QTimer
{
Q_OBJECT

public:
    DoMouseMoveHelper(QWidget *w, const QPoint &p, int timeout = 0);

private slots:
    void doMouseMove();

private:
    QWidget *m_w;
    QPoint m_p;
};

DoMouseMoveHelper::DoMouseMoveHelper(QWidget *w, const QPoint &p, int timeout) : m_w(w), m_p(p)
{
    setSingleShot(true);
    start(timeout);
    connect(this, SIGNAL(timeout()), this, SLOT(doMouseMove()));
}

void DoMouseMoveHelper::doMouseMove()
{
    QTest::mouseMove(m_w, m_p);
}

class tst_QDrag : public QObject
{
Q_OBJECT

public:
    tst_QDrag();
    virtual ~tst_QDrag();

private slots:
    void getSetCheck();
    void testDragEnterSelf();
    void testDragEnterNoShaping();
    void testDragEnterSomeShaping();
    void testDragEnterAllShaping();
};

tst_QDrag::tst_QDrag()
{
}

tst_QDrag::~tst_QDrag()
{
}

// Testing get/set functions
void tst_QDrag::getSetCheck()
{
    QDrag obj1(0);
    // QMimeData * QDrag::mimeData()
    // void QDrag::setMimeData(QMimeData *)
    QMimeData *var1 = new QMimeData;
    obj1.setMimeData(var1);
    QCOMPARE(var1, obj1.mimeData());
    obj1.setMimeData(var1);
    QCOMPARE(var1, obj1.mimeData());
    obj1.setMimeData((QMimeData *)0);
    QCOMPARE((QMimeData *)0, obj1.mimeData());
    // delete var1; // No delete, since QDrag takes ownership

    Qt::DropAction result = obj1.start();
    QCOMPARE(result, Qt::IgnoreAction);
    result = obj1.start(Qt::MoveAction | Qt::LinkAction);
    QCOMPARE(result, Qt::IgnoreAction);
}

void tst_QDrag::testDragEnterSelf()
{
#ifdef Q_WS_X11
    // Widget of 80x80 at 300, 300
    DragCounterAndCreatorWidget dw;
    QTest::qWaitForWindowShown(&dw);

    // Press mouse to create a drag in dw
    QTest::qWait(100);
    QTest::mouseMove(&dw);
    DoMouseReleaseHelper aux(&dw);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify that without a window in the middle the drag goes to dw itself
    QCOMPARE(dw.startedDrags, 1);
    QCOMPARE(dw.enterEvents, 1);
#endif
}

void tst_QDrag::testDragEnterNoShaping()
{
#ifdef Q_WS_X11
    // Widget of 80x80 at 300, 300
    DragCounterAndCreatorWidget dw;
    QTest::qWaitForWindowShown(&dw);

    // Widget of 180x180 at 250, 250
    BiggerDragCounterWidget widgetOnTop;
    QTest::qWaitForWindowShown(&widgetOnTop);

    // Press mouse to create a drag in dw
    QTest::qWait(100);
    QTest::mouseMove(&dw);
    DoMouseReleaseHelper aux(&dw);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify that with widgetOnTop in the middle the drag, the drag event does not go to dw
    // and goes to widgetOnTop
    QCOMPARE(dw.startedDrags, 1);
    QCOMPARE(dw.enterEvents, 0);
    QCOMPARE(widgetOnTop.enterEvents, 1);
#endif
}

void tst_QDrag::testDragEnterSomeShaping()
{
#if defined(Q_WS_X11) && defined(ShapeInput)
    // Widget of 80x80 at 300, 300
    DragCounterAndCreatorWidget dw;
    QTest::qWaitForWindowShown(&dw);

    // Widget of 180x180 at 250, 250
    BiggerDragCounterWidget widgetOnTop;
    QTest::qWaitForWindowShown(&widgetOnTop);

    // Punch a hole in widgetOnTop to let the mouse go through the widget
    // in the center of dw
    QBitmap inputShape(180, 180);
    inputShape.fill(Qt::color1);
    QPainter painter(&inputShape);
    painter.fillRect(80, 80, 50, 50, Qt::color0);
    painter.end();
    XShapeCombineRegion(QX11Info::display(), widgetOnTop.effectiveWinId(), ShapeInput, 0, 0, QRegion(inputShape).handle(), ShapeSet);

    // Press mouse to create a drag in dw
    QTest::qWait(100);
    QTest::mouseMove(&dw);
    DoMouseReleaseHelper aux(&dw);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify that with a input shaped widgetOnTop in the middle the drag, the drag event goes to dw
    // and does not go to widgetOnTop
    QCOMPARE(dw.startedDrags, 1);
    QCOMPARE(dw.enterEvents, 1);
    QCOMPARE(widgetOnTop.enterEvents, 0);

    // Press mouse to create a drag in dw and move it to the corner of the dw where widgetOnTop is not shaped anymore
    DoMouseMoveHelper aux2(&dw, QPoint(1, 1), 100);
    DoMouseReleaseHelper aux3(&dw, 200);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify once we get out of the shaped area the drag event also goes to widgetOnTop
    QCOMPARE(dw.startedDrags, 2);
    QCOMPARE(dw.enterEvents, 2);
    QCOMPARE(widgetOnTop.enterEvents, 1);
#endif
}

void tst_QDrag::testDragEnterAllShaping()
{
#if defined(Q_WS_X11) && defined(ShapeInput)
    // Widget of 80x80 at 300, 300
    DragCounterAndCreatorWidget dw;
    QTest::qWaitForWindowShown(&dw);

    // Widget of 180x180 at 250, 250
    BiggerDragCounterWidget widgetOnTop;
    QTest::qWaitForWindowShown(&widgetOnTop);

    // Make widgetOnTop totally a hole regarding input
    QBitmap inputShape(180, 180);
    inputShape.fill(Qt::color0);
    XShapeCombineRegion(QX11Info::display(), widgetOnTop.effectiveWinId(), ShapeInput, 0, 0, QRegion(inputShape).handle(), ShapeSet);

    // Press mouse to create a drag in dw
    QTest::qWait(100);
    QTest::mouseMove(&dw);
    DoMouseReleaseHelper aux(&dw);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify that with a input shaped widgetOnTop in the middle the drag, the drag event goes to dw
    // and does not go to widgetOnTop
    QCOMPARE(dw.startedDrags, 1);
    QCOMPARE(dw.enterEvents, 1);
    QCOMPARE(widgetOnTop.enterEvents, 0);

    // Press mouse to create a drag in dw and move it to the corner of the dw
    DoMouseMoveHelper aux2(&widgetOnTop, QPoint(1, 1));
    DoMouseReleaseHelper aux3(&dw, 200);
    QTest::mousePress(&dw, Qt::LeftButton);

    // Verify the event also went to dw
    QCOMPARE(dw.startedDrags, 2);
    QCOMPARE(dw.enterEvents, 2);
    QCOMPARE(widgetOnTop.enterEvents, 0);
#endif
}

QTEST_MAIN(tst_QDrag)
#include "tst_qdrag.moc"
