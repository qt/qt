/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the documentation of the Qt Toolkit.
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
#include <QtGui/qgraphicsanchorlayout.h>
#include <QtGui/qgraphicswidget.h>
#include <QtGui/qgraphicsview.h>

class tst_QGraphicsAnchorLayout : public QObject
{
    Q_OBJECT
public:
    tst_QGraphicsAnchorLayout() {}
    ~tst_QGraphicsAnchorLayout() {}

private slots:
    void s60_hard_complex_data();
    void s60_hard_complex();
};


class RectWidget : public QGraphicsWidget
{
public:
    RectWidget(QGraphicsItem *parent = 0) : QGraphicsWidget(parent){}

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->drawRoundRect(rect());
        painter->drawLine(rect().topLeft(), rect().bottomRight());
        painter->drawLine(rect().bottomLeft(), rect().topRight());
    }
};

static QGraphicsWidget *createItem(const QSizeF &minimum = QSizeF(100.0, 100.0),
                                   const QSizeF &preferred = QSize(150.0, 100.0),
                                   const QSizeF &maximum = QSizeF(200.0, 100.0),
                                   const QString &name = QString())
{
    QGraphicsWidget *w = new RectWidget;
    w->setMinimumSize(minimum);
    w->setPreferredSize(preferred);
    w->setMaximumSize(maximum);
    w->setData(0, name);
    return w;
}

static void setAnchor(QGraphicsAnchorLayout *l,
                      QGraphicsLayoutItem *firstItem,
                      Qt::AnchorPoint firstEdge,
                      QGraphicsLayoutItem *secondItem,
                      Qt::AnchorPoint secondEdge,
                      qreal spacing)
{
    l->addAnchor(firstItem, firstEdge, secondItem, secondEdge);
    l->setAnchorSpacing(firstItem, firstEdge, secondItem, secondEdge, spacing);
}

void tst_QGraphicsAnchorLayout::s60_hard_complex_data()
{
    QTest::addColumn<int>("whichSizeHint");
    QTest::newRow("minimumSizeHint")
        << int(Qt::MinimumSize);
    QTest::newRow("preferredSizeHint")
        << int(Qt::PreferredSize);
    QTest::newRow("maximumSizeHint")
        << int(Qt::MaximumSize);
    // Add it as a reference to see how much overhead the body of effectiveSizeHint takes.
    QTest::newRow("noSizeHint")
        << -1;
}

void tst_QGraphicsAnchorLayout::s60_hard_complex()
{
    QFETCH(int, whichSizeHint);

    // Test for "hard" complex case, taken from wiki
    // https://cwiki.nokia.com/S60QTUI/AnchorLayoutComplexCases
    QSizeF min(0, 10);
    QSizeF pref(50, 10);
    QSizeF max(100, 10);

    QGraphicsWidget *a = createItem(min, pref, max, "a");
    QGraphicsWidget *b = createItem(min, pref, max, "b");
    QGraphicsWidget *c = createItem(min, pref, max, "c");
    QGraphicsWidget *d = createItem(min, pref, max, "d");
    QGraphicsWidget *e = createItem(min, pref, max, "e");
    QGraphicsWidget *f = createItem(min, pref, max, "f");
    QGraphicsWidget *g = createItem(min, pref, max, "g");

    QGraphicsAnchorLayout *l = new QGraphicsAnchorLayout;
    l->setContentsMargins(0, 0, 0, 0);

    //<!-- Trunk -->
    setAnchor(l, l, Qt::AnchorLeft, a, Qt::AnchorLeft, 10);
    setAnchor(l, a, Qt::AnchorRight, b, Qt::AnchorLeft, 10);
    setAnchor(l, b, Qt::AnchorRight, c, Qt::AnchorLeft, 10);
    setAnchor(l, c, Qt::AnchorRight, d, Qt::AnchorLeft, 10);
    setAnchor(l, d, Qt::AnchorRight, l, Qt::AnchorRight, 10);

    //<!-- Above trunk -->
    setAnchor(l, b, Qt::AnchorLeft, e, Qt::AnchorLeft, 10);
    setAnchor(l, e, Qt::AnchorRight, d, Qt::AnchorLeft, 10);

    //<!-- Below trunk -->
    setAnchor(l, a, Qt::AnchorHorizontalCenter, g, Qt::AnchorLeft, 10);
    setAnchor(l, g, Qt::AnchorRight, f, Qt::AnchorHorizontalCenter, 10);
    setAnchor(l, c, Qt::AnchorLeft, f, Qt::AnchorLeft, 10);
    setAnchor(l, f, Qt::AnchorRight, d, Qt::AnchorRight, 10);

    //<!-- vertical is simpler -->
    setAnchor(l, l, Qt::AnchorTop, e, Qt::AnchorTop, 0);
    setAnchor(l, e, Qt::AnchorBottom, a, Qt::AnchorTop, 0);
    setAnchor(l, e, Qt::AnchorBottom, b, Qt::AnchorTop, 0);
    setAnchor(l, e, Qt::AnchorBottom, c, Qt::AnchorTop, 0);
    setAnchor(l, e, Qt::AnchorBottom, d, Qt::AnchorTop, 0);
    setAnchor(l, a, Qt::AnchorBottom, f, Qt::AnchorTop, 0);
    setAnchor(l, a, Qt::AnchorBottom, b, Qt::AnchorBottom, 0);
    setAnchor(l, a, Qt::AnchorBottom, c, Qt::AnchorBottom, 0);
    setAnchor(l, a, Qt::AnchorBottom, d, Qt::AnchorBottom, 0);
    setAnchor(l, f, Qt::AnchorBottom, g, Qt::AnchorTop, 0);
    setAnchor(l, g, Qt::AnchorBottom, l, Qt::AnchorBottom, 0);

    // It won't query the size hint if it already has a size set.
    // If only one of the sizes is unset it will query sizeHint only of for that hint type.
    l->setMinimumSize(60,40);
    l->setPreferredSize(220,40);
    l->setMaximumSize(240,40);

    switch (whichSizeHint) {
    case Qt::MinimumSize:
        l->setMinimumSize(-1, -1);
        break;
    case Qt::PreferredSize:
        l->setPreferredSize(-1, -1);
        break;
    case Qt::MaximumSize:
        l->setMaximumSize(-1, -1);
        break;
    default:
        break;
    }

    QSizeF sizeHint;
    // warm up instruction cache
    l->invalidate();
    sizeHint = l->effectiveSizeHint((Qt::SizeHint)whichSizeHint);
    // ...then measure...
    QBENCHMARK {
        l->invalidate();
        sizeHint = l->effectiveSizeHint((Qt::SizeHint)whichSizeHint);
    }
}

QTEST_MAIN(tst_QGraphicsAnchorLayout)

#include "tst_qgraphicsanchorlayout.moc"
