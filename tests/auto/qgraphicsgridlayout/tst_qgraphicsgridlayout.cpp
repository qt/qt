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
#include <qgraphicsgridlayout.h>
#include <qgraphicswidget.h>
#include <qgraphicsscene.h>
#include <qgraphicsview.h>

class tst_QGraphicsGridLayout : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

private slots:
    void qgraphicsgridlayout_data();
    void qgraphicsgridlayout();
    void addItem_data();
    void addItem();
    void alignment();
    void alignment2();
    void alignment2_data();
    void columnAlignment();
    void columnCount();
    void columnMaximumWidth();
    void columnMinimumWidth();
    void columnPreferredWidth();
    void setColumnFixedWidth();
    void columnSpacing();
    void columnStretchFactor();
    void count();
    void contentsMargins();
    void horizontalSpacing_data();
    void horizontalSpacing();
    void itemAt();
    void removeAt();
    void rowAlignment();
    void rowCount();
    void rowMaximumHeight();
    void rowMinimumHeight();
    void rowPreferredHeight();
    void rowSpacing();
    void rowStretchFactor();
    void setColumnSpacing_data();
    void setColumnSpacing();
    void setGeometry_data();
    void setGeometry();
    void setRowFixedHeight();
    void setRowSpacing_data();
    void setRowSpacing();
    void setSpacing_data();
    void setSpacing();
    void sizeHint_data();
    void sizeHint();
    void verticalSpacing_data();
    void verticalSpacing();
    void layoutDirection();
    void removeLayout();
    void defaultStretchFactors_data();
    void defaultStretchFactors();
    void geometries_data();
    void geometries();
    void avoidRecursionInInsertItem();
    void styleInfoLeak();
    void task236367_maxSizeHint();
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

    QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const
    {
        if (m_sizeHints[which].isValid()) {
            return m_sizeHints[which];
        }
        return QGraphicsWidget::sizeHint(which, constraint);
    }

    void setSizeHint(Qt::SizeHint which, const QSizeF &size) {
        m_sizeHints[which] = size;
        updateGeometry();
    }

    QSizeF m_sizeHints[Qt::NSizeHints];
};

struct ItemDesc
{
    ItemDesc(int row, int col)
    : m_pos(qMakePair(row, col)),
      m_rowSpan(1),
      m_colSpan(1),
      m_sizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred)),
      m_align(0)
    {
    }

    ItemDesc &rowSpan(int span) {
        m_rowSpan = span;
        return (*this);
    }

    ItemDesc &colSpan(int span) {
        m_colSpan = span;
        return (*this);
    }

    ItemDesc &sizePolicy(const QSizePolicy &sp) {
        m_sizePolicy = sp;
        return (*this);
    }

    ItemDesc &sizePolicy(QSizePolicy::Policy horAndVer) {
        m_sizePolicy = QSizePolicy(horAndVer, horAndVer);
        return (*this);
    }

    ItemDesc &sizePolicyH(QSizePolicy::Policy hor) {
        m_sizePolicy.setHorizontalPolicy(hor);
        return (*this);
    }

    ItemDesc &sizePolicyV(QSizePolicy::Policy ver) {
        m_sizePolicy.setVerticalPolicy(ver);
        return (*this);
    }

    ItemDesc &sizePolicy(QSizePolicy::Policy hor, QSizePolicy::Policy ver) {
        m_sizePolicy = QSizePolicy(hor, ver);
        return (*this);
    }

    ItemDesc &sizeHint(Qt::SizeHint which, const QSizeF &sh) {
        m_sizeHints[which] = sh;
        return (*this);
    }

    ItemDesc &preferredSizeHint(const QSizeF &sh) {
        m_sizeHints[Qt::PreferredSize] = sh;
        return (*this);
    }

    ItemDesc &minSize(const QSizeF &sz) {
        m_sizes[Qt::MinimumSize] = sz;
        return (*this);
    }
    ItemDesc &preferredSize(const QSizeF &sz) {
        m_sizes[Qt::PreferredSize] = sz;
        return (*this);
    }
    ItemDesc &maxSize(const QSizeF &sz) {
        m_sizes[Qt::MaximumSize] = sz;
        return (*this);
    }

    ItemDesc &alignment(Qt::Alignment alignment) {
        m_align = alignment;
        return (*this);
    }

    void apply(QGraphicsGridLayout *layout, QGraphicsWidget *item) {
        item->setSizePolicy(m_sizePolicy);
        for (int i = 0; i < Qt::NSizeHints; ++i) {
            if (!m_sizes[i].isValid())
                continue;
            switch ((Qt::SizeHint)i) {
            case Qt::MinimumSize:
                item->setMinimumSize(m_sizes[i]);
                break;
            case Qt::PreferredSize:
                item->setPreferredSize(m_sizes[i]);
                break;
            case Qt::MaximumSize:
                item->setMaximumSize(m_sizes[i]);
                break;
            default:
                qWarning("not implemented");
                break;
            }
        }
        layout->addItem(item, m_pos.first, m_pos.second, m_rowSpan, m_colSpan);
        layout->setAlignment(item, m_align);
    }

    void apply(QGraphicsGridLayout *layout, RectWidget *item) {
        for (int i = 0; i < Qt::NSizeHints; ++i)
            item->setSizeHint((Qt::SizeHint)i, m_sizeHints[i]);
        apply(layout, static_cast<QGraphicsWidget*>(item));
    }

//private:
    QPair<int,int> m_pos; // row,col
    int m_rowSpan;
    int m_colSpan;
    QSizePolicy m_sizePolicy;
    QSizeF m_sizeHints[Qt::NSizeHints];
    QSizeF m_sizes[Qt::NSizeHints];
    Qt::Alignment m_align;
};

typedef QList<ItemDesc> ItemList;
Q_DECLARE_METATYPE(ItemList);

typedef QList<QSizeF> SizeList;
Q_DECLARE_METATYPE(SizeList);


// This will be called before the first test function is executed.
// It is only called once.
void tst_QGraphicsGridLayout::initTestCase()
{
}

// This will be called after the last test function is executed.
// It is only called once.
void tst_QGraphicsGridLayout::cleanupTestCase()
{
}

// This will be called before each test function is executed.
void tst_QGraphicsGridLayout::init()
{
#ifdef Q_OS_WINCE //disable magic for WindowsCE
    qApp->setAutoMaximizeThreshold(-1);
#endif
}

// This will be called after every test function.
void tst_QGraphicsGridLayout::cleanup()
{
}

void tst_QGraphicsGridLayout::qgraphicsgridlayout_data()
{
}

void tst_QGraphicsGridLayout::qgraphicsgridlayout()
{
    QGraphicsGridLayout layout;
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::addItem: invalid row span/column span: 0");
    layout.addItem(0, 0, 0, 0, 0);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::addItem: cannot add null item");
    layout.addItem(0, 0, 0);
    layout.alignment(0);
    layout.columnAlignment(0);
    layout.columnCount();
    layout.columnMaximumWidth(0);
    layout.columnMinimumWidth(0);
    layout.columnPreferredWidth(0);
    layout.columnSpacing(0);
    layout.columnStretchFactor(0);
    layout.count();
    layout.horizontalSpacing();
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::itemAt: invalid row, column 0, 0");
    layout.itemAt(0, 0);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::itemAt: invalid index 0");
    layout.itemAt(0);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::removeAt: invalid index 0");
    layout.removeAt(0);
    layout.rowAlignment(0);
    layout.rowCount();
    layout.rowMaximumHeight(0);
    layout.rowMinimumHeight(0);
    layout.rowPreferredHeight(0);
    layout.rowSpacing(0);
    layout.rowStretchFactor(0);
    layout.setAlignment(0, Qt::AlignRight);
    layout.setColumnAlignment(0, Qt::AlignRight);
    layout.setColumnFixedWidth(0, 0);
    layout.setColumnMaximumWidth(0, 0);
    layout.setColumnMinimumWidth(0, 0);
    layout.setColumnPreferredWidth(0, 0);
    layout.setColumnSpacing(0, 0);
    layout.setColumnStretchFactor(0, 0);
    layout.setGeometry(QRectF());
    layout.setHorizontalSpacing(0);
    layout.setRowAlignment(0, 0);
    layout.setRowFixedHeight(0, 0);
    layout.setRowMaximumHeight(0, 0);
    layout.setRowMinimumHeight(0, 0);
    layout.setRowPreferredHeight(0, 0);
    layout.setRowSpacing(0, 0);
    layout.setRowStretchFactor(0, 0);
    layout.setSpacing(0);
    layout.setVerticalSpacing(0);
    layout.sizeHint(Qt::MinimumSize);
    layout.verticalSpacing();
}

static void populateLayout(QGraphicsGridLayout *gridLayout, int width, int height)
{
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QGraphicsWidget *item = new RectWidget();
            item->setMinimumSize(10, 10);
            item->setPreferredSize(25, 25);
            item->setMaximumSize(50, 50);
            gridLayout->addItem(item, y, x);
        }
    }
}


/** populates \a gridLayout with a 3x2 layout:
 * +----+----+----+
 * |+---|---+|xxxx|
 * ||span=2 ||hole|
 * |+---|---+|xxxx|
 * +----+----+----+
 * |xxxx|+---|---+|
 * |hole||span=2 ||
 * |xxxx|+---|---+|
 * +----+----+----+
 */
static void populateLayoutWithSpansAndHoles(QGraphicsGridLayout *gridLayout)
{
    QGraphicsWidget *item = new RectWidget();
    item->setMinimumSize(10, 10);
    item->setPreferredSize(25, 25);
    item->setMaximumSize(50, 50);
    gridLayout->addItem(item, 0, 0, 1, 2);

    item = new RectWidget();
    item->setMinimumSize(10, 10);
    item->setPreferredSize(25, 25);
    item->setMaximumSize(50, 50);
    gridLayout->addItem(item, 1, 1, 1, 2);
}

Q_DECLARE_METATYPE(Qt::Alignment)
void tst_QGraphicsGridLayout::addItem_data()
{
    QTest::addColumn<int>("row");
    QTest::addColumn<int>("column");
    QTest::addColumn<int>("rowSpan");
    QTest::addColumn<int>("columnSpan");
    QTest::addColumn<Qt::Alignment>("alignment");

    for (int a = -1; a < 3; ++a) {
    for (int b = -1; b < 2; ++b) {
    for (int c = -1; c < 2; ++c) {
    for (int d = -1; d < 2; ++d) {
    for (int e = 0; e < 9; ++e) {
        int row = a;
        int column = b;
        int rowSpan = c;
        int columnSpan = d;
        QString name = QString::fromAscii("(%1,%2,%3,%4").arg(a).arg(b).arg(c).arg(d);
        Qt::Alignment alignment = Qt::AlignLeft;
        QTest::newRow(name.toLatin1()) << row << column << rowSpan << columnSpan << alignment;
    }}}}}
}

// public void addItem(QGraphicsLayoutItem* item, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment = 0)
void tst_QGraphicsGridLayout::addItem()
{
    QFETCH(int, row);
    QFETCH(int, column);
    QFETCH(int, rowSpan);
    QFETCH(int, columnSpan);
    QFETCH(Qt::Alignment, alignment);

    QGraphicsGridLayout *layout = new QGraphicsGridLayout;

    QGraphicsWidget *wid = new QGraphicsWidget;
    if (row < 0 || column < 0) {
        QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::addItem: invalid row/column: -1");
    } else if (rowSpan < 1 || columnSpan < 1) {
        char buf[1024];
        ::qsnprintf(buf, sizeof(buf), "QGraphicsGridLayout::addItem: invalid row span/column span: %d",
            rowSpan < 1 ? rowSpan : columnSpan);
        QTest::ignoreMessage(QtWarningMsg, buf);
    }
    layout->addItem(wid, row, column, rowSpan, columnSpan, alignment);

    delete layout;
}

// public Qt::Alignment alignment(QGraphicsLayoutItem* item) const
void tst_QGraphicsGridLayout::alignment()
{
#ifdef Q_WS_MAC
    QSKIP("Resizing a QGraphicsWidget to effectiveSizeHint(Qt::MaximumSize) is currently not supported on mac", SkipAll);
#endif
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();
    // no alignment (the default)
    QCOMPARE(layout->itemAt(0, 0)->geometry().left(), 0.0);
    QCOMPARE(layout->itemAt(0, 0)->geometry().right(), layout->itemAt(0, 1)->geometry().left());
    QCOMPARE(layout->itemAt(0, 1)->geometry().left(), 25.0);
    QCOMPARE(layout->itemAt(0, 1)->geometry().right(), layout->itemAt(0, 2)->geometry().left());
    QCOMPARE(layout->itemAt(0, 2)->geometry().left(), 50.0);
    QCOMPARE(layout->itemAt(0, 2)->geometry().right(), 75.0);

    QCOMPARE(layout->itemAt(1, 0)->geometry().left(), 0.0);
    QCOMPARE(layout->itemAt(1, 0)->geometry().right(), layout->itemAt(1, 1)->geometry().left());
    QCOMPARE(layout->itemAt(1, 1)->geometry().left(), 25.0);
    QCOMPARE(layout->itemAt(1, 1)->geometry().right(), layout->itemAt(1, 2)->geometry().left());
    QCOMPARE(layout->itemAt(1, 2)->geometry().left(), 50.0);
    QCOMPARE(layout->itemAt(1, 2)->geometry().right(), 75.0);

    QCOMPARE(layout->itemAt(0, 0)->geometry().top(), 0.0);
    QCOMPARE(layout->itemAt(0, 0)->geometry().bottom(), layout->itemAt(1, 0)->geometry().top());
    QCOMPARE(layout->itemAt(1, 0)->geometry().top(), 25.0);
    QCOMPARE(layout->itemAt(1, 0)->geometry().bottom(), 50.0);

    // align first column left, second hcenter, third right
    layout->setColumnMinimumWidth(0, 100);
    layout->setAlignment(layout->itemAt(0,0), Qt::AlignLeft);
    layout->setAlignment(layout->itemAt(1,0), Qt::AlignLeft);
    layout->setColumnMinimumWidth(1, 100);
    layout->setAlignment(layout->itemAt(0,1), Qt::AlignHCenter);
    layout->setAlignment(layout->itemAt(1,1), Qt::AlignHCenter);
    layout->setColumnMinimumWidth(2, 100);
    layout->setAlignment(layout->itemAt(0,2), Qt::AlignRight);
    layout->setAlignment(layout->itemAt(1,2), Qt::AlignRight);

    widget->resize(widget->effectiveSizeHint(Qt::MaximumSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry(), QRectF(0,    0,  50,  25));
    QCOMPARE(layout->itemAt(1,0)->geometry(), QRectF(0,   25,  50,  25));
    QCOMPARE(layout->itemAt(0,1)->geometry(), QRectF(125,  0,  50,  25));
    QCOMPARE(layout->itemAt(1,1)->geometry(), QRectF(125, 25,  50,  25));
    QCOMPARE(layout->itemAt(0,2)->geometry(), QRectF(250,  0,  50,  25));
    QCOMPARE(layout->itemAt(1,2)->geometry(), QRectF(250, 25,  50,  25));

    delete widget;
}

// public void setColumnAlignment(int column, Qt::Alignment alignment)
// public Qt::Alignment columnAlignment(int column) const
void tst_QGraphicsGridLayout::columnAlignment()
{
#ifdef Q_WS_MAC
    QSKIP("Resizing a QGraphicsWidget to effectiveSizeHint(Qt::MaximumSize) is currently not supported on mac", SkipAll);
#endif
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    widget->setContentsMargins(0, 0, 0, 0);

    layout->setColumnMinimumWidth(0, 100);
    layout->setColumnMinimumWidth(1, 100);
    layout->setColumnMinimumWidth(2, 100);

    view.resize(450,150);
    widget->resize(widget->effectiveSizeHint(Qt::MaximumSize));
    view.show();
    widget->show();
    QApplication::sendPostedEvents(0, 0);
    // Check default
    QCOMPARE(layout->columnAlignment(0), 0);
    QCOMPARE(layout->columnAlignment(1), 0);
    QCOMPARE(layout->columnAlignment(2), 0);

    layout->setColumnAlignment(0, Qt::AlignLeft);
    layout->setColumnAlignment(1, Qt::AlignHCenter);
    layout->setColumnAlignment(2, Qt::AlignRight);

    // see if item alignment takes preference over columnAlignment
    layout->setAlignment(layout->itemAt(1,0), Qt::AlignHCenter);
    layout->setAlignment(layout->itemAt(1,1), Qt::AlignRight);
    layout->setAlignment(layout->itemAt(1,2), Qt::AlignLeft);

    QApplication::sendPostedEvents(0, 0);  // process LayoutRequest
    /*
      +----------+------------+---------+
      | Left     |  HCenter   |  Right  |
      +----------+------------+---------+
      | HCenter  |   Right    |   Left  |
      +---------------------------------+
    */
    QCOMPARE(layout->itemAt(0,0)->geometry(), QRectF(0,   0,   50,  25));
    QCOMPARE(layout->itemAt(1,0)->geometry(), QRectF(25,  26,  50,  25));   // item is king
    QCOMPARE(layout->itemAt(0,1)->geometry(), QRectF(126,  0,  50,  25));
    QCOMPARE(layout->itemAt(1,1)->geometry(), QRectF(151, 26,  50,  25));   // item is king
    QCOMPARE(layout->itemAt(0,2)->geometry(), QRectF(252,  0,  50,  25));
    QCOMPARE(layout->itemAt(1,2)->geometry(), QRectF(202, 26,  50,  25));   // item is king

    delete widget;
}

// public int columnCount() const
void tst_QGraphicsGridLayout::columnCount()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    widget->setContentsMargins(0, 0, 0, 0);

    view.show();
    widget->show();
    QApplication::processEvents();

    QCOMPARE(layout->columnCount(), 0);
    layout->addItem(new RectWidget(widget), 0, 0);
    QCOMPARE(layout->columnCount(), 1);
    layout->addItem(new RectWidget(widget), 1, 1);
    QCOMPARE(layout->columnCount(), 2);
    layout->addItem(new RectWidget(widget), 0, 2);
    QCOMPARE(layout->columnCount(), 3);
    layout->addItem(new RectWidget(widget), 1, 0);
    QCOMPARE(layout->columnCount(), 3);
    layout->addItem(new RectWidget(widget), 0, 1);
    QCOMPARE(layout->columnCount(), 3);
    layout->addItem(new RectWidget(widget), 1, 2);
    QCOMPARE(layout->columnCount(), 3);

    // ### Talk with Jasmin. Not sure if removeAt() should adjust columnCount().
    widget->setLayout(0);
    layout = new QGraphicsGridLayout();
    populateLayout(layout, 3, 2);
    QCOMPARE(layout->columnCount(), 3);
    layout->removeAt(5);
    layout->removeAt(3);
    QCOMPARE(layout->columnCount(), 3);
    layout->removeAt(1);
    QCOMPARE(layout->columnCount(), 3);
    layout->removeAt(0);
    QCOMPARE(layout->columnCount(), 3);
    layout->removeAt(0);
    QCOMPARE(layout->columnCount(), 2);

    delete widget;
}

// public qreal columnMaximumWidth(int column) const
void tst_QGraphicsGridLayout::columnMaximumWidth()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // should at least be a very large number
    QVERIFY(layout->columnMaximumWidth(0) >= 10000);
    QCOMPARE(layout->columnMaximumWidth(0), layout->columnMaximumWidth(1));
    QCOMPARE(layout->columnMaximumWidth(1), layout->columnMaximumWidth(2));
    layout->setColumnMaximumWidth(0, 20);
    layout->setColumnMaximumWidth(2, 60);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().width(), 20.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().width(), 20.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,2)->geometry().width(), 25.0);

    delete widget;
}

// public qreal columnMinimumWidth(int column) const
void tst_QGraphicsGridLayout::columnMinimumWidth()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // should at least be a very large number
    QCOMPARE(layout->columnMinimumWidth(0), 0.0);
    QCOMPARE(layout->columnMinimumWidth(0), layout->columnMinimumWidth(1));
    QCOMPARE(layout->columnMinimumWidth(1), layout->columnMinimumWidth(2));
    layout->setColumnMinimumWidth(0, 20);
    layout->setColumnMinimumWidth(2, 40);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().width(), 40.0);
    QCOMPARE(layout->itemAt(1,2)->geometry().width(), 40.0);

    delete widget;
}

// public qreal columnPreferredWidth(int column) const
void tst_QGraphicsGridLayout::columnPreferredWidth()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // default preferred width ??
    QCOMPARE(layout->columnPreferredWidth(0), 0.0);
    QCOMPARE(layout->columnPreferredWidth(0), layout->columnPreferredWidth(1));
    QCOMPARE(layout->columnPreferredWidth(1), layout->columnPreferredWidth(2));
    layout->setColumnPreferredWidth(0, 20);
    layout->setColumnPreferredWidth(2, 40);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().width(), 40.0);
    QCOMPARE(layout->itemAt(1,2)->geometry().width(), 40.0);

    delete widget;
}

// public void setColumnFixedWidth(int row, qreal height)
void tst_QGraphicsGridLayout::setColumnFixedWidth()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->setColumnFixedWidth(0, 20);
    layout->setColumnFixedWidth(2, 40);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().width(), 20.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().width(), 20.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().width(), 25.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().width(), 40.0);
    QCOMPARE(layout->itemAt(1,2)->geometry().width(), 40.0);

    delete widget;
}

// public qreal columnSpacing(int column) const
void tst_QGraphicsGridLayout::columnSpacing()
{
    {
        QGraphicsScene scene;
        QGraphicsView view(&scene);
        QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
        QGraphicsGridLayout *layout = new QGraphicsGridLayout();
        scene.addItem(widget);
        widget->setLayout(layout);
        populateLayout(layout, 3, 2);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        QCOMPARE(layout->columnSpacing(0), 0.0);

        layout->setColumnSpacing(0, 20);
        view.show();
        widget->show();
        widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
        QApplication::processEvents();

        QCOMPARE(layout->itemAt(0,0)->geometry().left(),   0.0);
        QCOMPARE(layout->itemAt(0,0)->geometry().right(), 25.0);
        QCOMPARE(layout->itemAt(0,1)->geometry().left(),  45.0);
        QCOMPARE(layout->itemAt(0,1)->geometry().right(), 70.0);
        QCOMPARE(layout->itemAt(0,2)->geometry().left(),  70.0);
        QCOMPARE(layout->itemAt(0,2)->geometry().right(), 95.0);

        delete widget;
    }

    {
        // don't include items and spacings that was previously part of the layout
        // (horizontal)
        QGraphicsGridLayout *layout = new QGraphicsGridLayout;
        populateLayout(layout, 3, 1);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->setColumnSpacing(0, 10);
        layout->setColumnSpacing(1, 10);
        layout->setColumnSpacing(2, 10);
        layout->setColumnSpacing(3, 10);
        QCOMPARE(layout->preferredSize(), QSizeF(95, 25));
        layout->removeAt(2);
        QCOMPARE(layout->preferredSize(), QSizeF(60, 25));
        layout->removeAt(1);
        QCOMPARE(layout->preferredSize(), QSizeF(25, 25));
        delete layout;
    }
    {
        // don't include items and spacings that was previously part of the layout
        // (vertical)
        QGraphicsGridLayout *layout = new QGraphicsGridLayout;
        populateLayout(layout, 2, 2);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->setColumnSpacing(0, 10);
        layout->setColumnSpacing(1, 10);
        layout->setRowSpacing(0, 10);
        layout->setRowSpacing(1, 10);
        QCOMPARE(layout->preferredSize(), QSizeF(60, 60));
        layout->removeAt(3);
        QCOMPARE(layout->preferredSize(), QSizeF(60, 60));
        layout->removeAt(2);
        QCOMPARE(layout->preferredSize(), QSizeF(60, 25));
        layout->removeAt(1);
        QCOMPARE(layout->preferredSize(), QSizeF(25, 25));
        delete layout;
    }

}

// public int columnStretchFactor(int column) const
void tst_QGraphicsGridLayout::columnStretchFactor()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->setColumnStretchFactor(0, 1);
    layout->setColumnStretchFactor(1, 2);
    layout->setColumnStretchFactor(2, 3);
    view.show();
    widget->show();
    widget->resize(130, 50);
    QApplication::processEvents();

    QVERIFY(layout->itemAt(0,0)->geometry().width() <  layout->itemAt(0,1)->geometry().width());
    QVERIFY(layout->itemAt(0,1)->geometry().width() <  layout->itemAt(0,2)->geometry().width());
    QVERIFY(layout->itemAt(1,0)->geometry().width() <  layout->itemAt(1,1)->geometry().width());
    QVERIFY(layout->itemAt(1,1)->geometry().width() <  layout->itemAt(1,2)->geometry().width());

    delete widget;
}


// public int count() const
void tst_QGraphicsGridLayout::count()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    QCOMPARE(layout->count(), 6);
    layout->removeAt(5);
    layout->removeAt(3);
    QCOMPARE(layout->count(), 4);
    layout->removeAt(1);
    QCOMPARE(layout->count(), 3);
    layout->removeAt(0);
    QCOMPARE(layout->count(), 2);
    layout->removeAt(0);
    QCOMPARE(layout->count(), 1);
    layout->removeAt(0);
    QCOMPARE(layout->count(), 0);

    delete widget;
}

void tst_QGraphicsGridLayout::horizontalSpacing_data()
{
    QTest::addColumn<qreal>("horizontalSpacing");
    QTest::newRow("zero") << qreal(0.0);
    QTest::newRow("10") << qreal(10.0);
}

// public qreal horizontalSpacing() const
void tst_QGraphicsGridLayout::horizontalSpacing()
{
    QFETCH(qreal, horizontalSpacing);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    qreal w = layout->sizeHint(Qt::PreferredSize, QSizeF()).width();
    qreal oldSpacing = layout->horizontalSpacing();
    if (oldSpacing != -1) {
        layout->setHorizontalSpacing(horizontalSpacing);
        QApplication::processEvents();
        qreal new_w = layout->sizeHint(Qt::PreferredSize, QSizeF()).width();
        QCOMPARE(new_w, w - (3-1)*(oldSpacing - horizontalSpacing));
    } else {
        QSKIP("The current style uses non-uniform layout spacing", SkipAll);
    }
    delete widget;
}

void tst_QGraphicsGridLayout::contentsMargins()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    QGraphicsGridLayout *sublayout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->addItem(sublayout,0, 1);

    qreal left, top, right, bottom;
    // sublayouts have 0 margin
    sublayout->getContentsMargins(&left, &top, &right, &bottom);
    QCOMPARE(left, 0.0);
    QCOMPARE(top, 0.0);
    QCOMPARE(right, 0.0);
    QCOMPARE(bottom, 0.0);

    // top level layouts have style dependent margins.
    // we'll just check if its different from 0. (applies to all our styles)
    layout->getContentsMargins(&left, &top, &right, &bottom);
    QVERIFY(left >= 0.0);
    QVERIFY(top >= 0.0);
    QVERIFY(right >= 0.0);
    QVERIFY(bottom >= 0.0);

    delete widget;
}

// public QGraphicsLayoutItem* itemAt(int index) const
void tst_QGraphicsGridLayout::itemAt()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayoutWithSpansAndHoles(layout);

    //itemAt(int row, int column)
    QVERIFY( layout->itemAt(0,0));
    QVERIFY( layout->itemAt(0,1));
    QCOMPARE(layout->itemAt(0,2), static_cast<QGraphicsLayoutItem*>(0));
    QCOMPARE(layout->itemAt(1,0), static_cast<QGraphicsLayoutItem*>(0));
    QVERIFY( layout->itemAt(1,1));
    QVERIFY( layout->itemAt(1,2));


    //itemAt(int index)
    for (int i = -2; i < layout->count() + 2; ++i) {
        if (i >= 0 && i < layout->count()) {
            QVERIFY(layout->itemAt(i));
        } else {
            QTest::ignoreMessage(QtWarningMsg, QString::fromAscii("QGraphicsGridLayout::itemAt: invalid index %1").arg(i).toLatin1().constData());
            QCOMPARE(layout->itemAt(i), static_cast<QGraphicsLayoutItem*>(0));
        }
    }
    delete widget;
}

// public void removeAt(int index)
void tst_QGraphicsGridLayout::removeAt()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    QCOMPARE(layout->count(), 6);
    layout->removeAt(5);
    layout->removeAt(3);
    QCOMPARE(layout->count(), 4);
    layout->removeAt(1);
    QCOMPARE(layout->count(), 3);
    layout->removeAt(0);
    QCOMPARE(layout->count(), 2);
    layout->removeAt(0);
    QCOMPARE(layout->count(), 1);
    QGraphicsLayoutItem *item0 = layout->itemAt(0);
    QCOMPARE(item0->parentLayoutItem(), static_cast<QGraphicsLayoutItem *>(layout));
    layout->removeAt(0);
    QCOMPARE(item0->parentLayoutItem(), static_cast<QGraphicsLayoutItem *>(0));
    QCOMPARE(layout->count(), 0);
    QTest::ignoreMessage(QtWarningMsg, QString::fromAscii("QGraphicsGridLayout::removeAt: invalid index 0").toLatin1().constData());
    layout->removeAt(0);
    QCOMPARE(layout->count(), 0);
    delete widget;
}

// public Qt::Alignment rowAlignment(int row) const
void tst_QGraphicsGridLayout::rowAlignment()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(1);
    widget->setContentsMargins(0, 0, 0, 0);

    view.resize(330,450);
    widget->resize(300, 400);
    view.show();
    widget->show();
    QApplication::sendPostedEvents(0, 0);
    // Check default
    QCOMPARE(layout->rowAlignment(0), 0);
    QCOMPARE(layout->rowAlignment(1), 0);
    QCOMPARE(layout->rowAlignment(2), 0);

    // make the grids larger than the items, so that alignment kicks in
    layout->setRowMinimumHeight(0, 100.0);
    layout->setRowMinimumHeight(1, 100.0);
    layout->setRowMinimumHeight(2, 100.0);
    // expand columns also, so we can test combination of horiz and vertical alignment
    layout->setColumnMinimumWidth(0, 100.0);
    layout->setColumnMinimumWidth(1, 100.0);

    layout->setRowAlignment(0, Qt::AlignBottom);
    layout->setRowAlignment(1, Qt::AlignVCenter);
    layout->setRowAlignment(2, Qt::AlignTop);

    // see if item alignment takes preference over rowAlignment
    layout->setAlignment(layout->itemAt(0,0), Qt::AlignRight);
    layout->setAlignment(layout->itemAt(1,0), Qt::AlignTop);
    layout->setAlignment(layout->itemAt(2,0), Qt::AlignHCenter);

    QApplication::sendPostedEvents(0, 0);   // process LayoutRequest

    QCOMPARE(layout->alignment(layout->itemAt(0,0)), Qt::AlignRight);  //Qt::AlignRight | Qt::AlignBottom
    QCOMPARE(layout->itemAt(0,0)->geometry(), QRectF(50,  50,  50,  50));
    QCOMPARE(layout->rowAlignment(0), Qt::AlignBottom);
    QCOMPARE(layout->itemAt(0,1)->geometry(), QRectF(101, 50,  50,  50));
    QCOMPARE(layout->alignment(layout->itemAt(1,0)), Qt::AlignTop);
    QCOMPARE(layout->itemAt(1,0)->geometry(), QRectF(0,  101,  50,  50));
    QCOMPARE(layout->rowAlignment(1), Qt::AlignVCenter);
    QCOMPARE(layout->itemAt(1,1)->geometry(), QRectF(101, 126, 50,  50));
    QCOMPARE(layout->alignment(layout->itemAt(2,0)), Qt::AlignHCenter);
    QCOMPARE(layout->itemAt(2,0)->geometry(), QRectF(25, 202,  50,  50));
    QCOMPARE(layout->rowAlignment(2), Qt::AlignTop);
    QCOMPARE(layout->itemAt(2,1)->geometry(), QRectF(101,202,  50,  50));

    delete widget;
}

// public int rowCount() const
// public int columnCount() const
void tst_QGraphicsGridLayout::rowCount()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    widget->setContentsMargins(0, 0, 0, 0);
    QCOMPARE(layout->rowCount(), 3);
    QCOMPARE(layout->columnCount(), 2);

    // with spans and holes...
    widget->setLayout(0);
    layout = new QGraphicsGridLayout();
    populateLayoutWithSpansAndHoles(layout);
    QCOMPARE(layout->rowCount(), 2);
    QCOMPARE(layout->columnCount(), 3);

    delete widget;
}

// public qreal rowMaximumHeight(int row) const
void tst_QGraphicsGridLayout::rowMaximumHeight()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // should at least be a very large number
    QVERIFY(layout->rowMaximumHeight(0) >= 10000);
    QCOMPARE(layout->rowMaximumHeight(0), layout->rowMaximumHeight(1));
    QCOMPARE(layout->rowMaximumHeight(1), layout->rowMaximumHeight(2));
    layout->setRowMaximumHeight(0, 20);
    layout->setRowMaximumHeight(2, 60);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().height(), 20.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().height(), 20.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(2,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(2,1)->geometry().height(), 25.0);

    delete widget;
}

// public qreal rowMinimumHeight(int row) const
void tst_QGraphicsGridLayout::rowMinimumHeight()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // should at least be a very large number
    QCOMPARE(layout->rowMinimumHeight(0), 0.0);
    QCOMPARE(layout->rowMinimumHeight(0), layout->rowMinimumHeight(1));
    QCOMPARE(layout->rowMinimumHeight(1), layout->rowMinimumHeight(2));
    layout->setRowMinimumHeight(0, 20);
    layout->setRowMinimumHeight(2, 40);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(2,0)->geometry().height(), 40.0);
    QCOMPARE(layout->itemAt(2,1)->geometry().height(), 40.0);

    delete widget;
}

// public qreal rowPreferredHeight(int row) const
void tst_QGraphicsGridLayout::rowPreferredHeight()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // default preferred height ??
    QCOMPARE(layout->rowPreferredHeight(0), 0.0);
    QCOMPARE(layout->rowPreferredHeight(0), layout->rowPreferredHeight(1));
    QCOMPARE(layout->rowPreferredHeight(1), layout->rowPreferredHeight(2));
    layout->setRowPreferredHeight(0, 20);
    layout->setRowPreferredHeight(2, 40);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    // ### Jasmin: Should rowPreferredHeight have precedence over sizeHint(Qt::PreferredSize) ?
    QCOMPARE(layout->itemAt(0,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(2,0)->geometry().height(), 40.0);
    QCOMPARE(layout->itemAt(2,1)->geometry().height(), 40.0);

    delete widget;
}

// public void setRowFixedHeight(int row, qreal height)
void tst_QGraphicsGridLayout::setRowFixedHeight()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->setRowFixedHeight(0, 20.);
    layout->setRowFixedHeight(2, 40.);

    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().height(), 20.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().height(), 20.0);
    QCOMPARE(layout->itemAt(1,0)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(1,1)->geometry().height(), 25.0);
    QCOMPARE(layout->itemAt(2,0)->geometry().height(), 40.0);
    QCOMPARE(layout->itemAt(2,1)->geometry().height(), 40.0);

    delete widget;
}

// public qreal rowSpacing(int row) const
void tst_QGraphicsGridLayout::rowSpacing()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    QCOMPARE(layout->columnSpacing(0), 0.0);

    layout->setColumnSpacing(0, 20);
    view.show();
    widget->show();
    widget->resize(widget->effectiveSizeHint(Qt::PreferredSize));
    QApplication::processEvents();

    QCOMPARE(layout->itemAt(0,0)->geometry().left(),   0.0);
    QCOMPARE(layout->itemAt(0,0)->geometry().right(), 25.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().left(),  45.0);
    QCOMPARE(layout->itemAt(0,1)->geometry().right(), 70.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().left(),  70.0);
    QCOMPARE(layout->itemAt(0,2)->geometry().right(), 95.0);

    delete widget;

}

// public int rowStretchFactor(int row) const
void tst_QGraphicsGridLayout::rowStretchFactor()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 2, 3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->setRowStretchFactor(0, 1);
    layout->setRowStretchFactor(1, 2);
    layout->setRowStretchFactor(2, 3);
    view.show();
    widget->show();
    widget->resize(50, 130);
    QApplication::processEvents();

    QVERIFY(layout->itemAt(0,0)->geometry().height() <  layout->itemAt(1,0)->geometry().height());
    QVERIFY(layout->itemAt(1,0)->geometry().height() <  layout->itemAt(2,0)->geometry().height());
    QVERIFY(layout->itemAt(0,1)->geometry().height() <  layout->itemAt(1,1)->geometry().height());
    QVERIFY(layout->itemAt(1,1)->geometry().height() <  layout->itemAt(2,1)->geometry().height());

    delete widget;
}

void tst_QGraphicsGridLayout::setColumnSpacing_data()
{
    QTest::addColumn<int>("column");
    QTest::addColumn<qreal>("spacing");
    QTest::newRow("null") << 0 << qreal(0.0);
    QTest::newRow("10") << 0 << qreal(10.0);

}

// public void setColumnSpacing(int column, qreal spacing)
void tst_QGraphicsGridLayout::setColumnSpacing()
{
    QFETCH(int, column);
    QFETCH(qreal, spacing);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    qreal oldSpacing = layout->columnSpacing(column);
    QCOMPARE(oldSpacing, 0.0);
    qreal w = layout->sizeHint(Qt::PreferredSize, QSizeF()).width();
    layout->setColumnSpacing(column, spacing);
    QApplication::processEvents();
    QCOMPARE(layout->sizeHint(Qt::PreferredSize, QSizeF()).width(), w + spacing);
}

void tst_QGraphicsGridLayout::setGeometry_data()
{    
    QTest::addColumn<QRectF>("rect");
    QTest::newRow("null") << QRectF();
    QTest::newRow("normal") << QRectF(0,0, 50, 50);
}

// public void setGeometry(QRectF const& rect)
void tst_QGraphicsGridLayout::setGeometry()
{
    QFETCH(QRectF, rect);

    QGraphicsWidget *window = new QGraphicsWidget;
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    window->setLayout(layout);
    QGraphicsGridLayout *layout2 = new QGraphicsGridLayout();
    layout2->setMaximumSize(100, 100);
    layout->addItem(layout2, 0, 0);
    layout2->setGeometry(rect);
    QCOMPARE(layout2->geometry(), rect);
}

void tst_QGraphicsGridLayout::setRowSpacing_data()
{
    QTest::addColumn<int>("row");
    QTest::addColumn<qreal>("spacing");
    QTest::newRow("null") << 0 << qreal(0.0);
    QTest::newRow("10") << 0 << qreal(10.0);

}

// public void setRowSpacing(int row, qreal spacing)
void tst_QGraphicsGridLayout::setRowSpacing()
{
    QFETCH(int, row);
    QFETCH(qreal, spacing);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    qreal oldSpacing = layout->rowSpacing(row);
    QCOMPARE(oldSpacing, 0.0);
    qreal h = layout->sizeHint(Qt::PreferredSize, QSizeF()).height();
    layout->setRowSpacing(row, spacing);
    QApplication::processEvents();
    QCOMPARE(layout->sizeHint(Qt::PreferredSize, QSizeF()).height(), h + spacing);
}

void tst_QGraphicsGridLayout::setSpacing_data()
{
    QTest::addColumn<qreal>("spacing");
    QTest::newRow("zero") << qreal(0.0);
    QTest::newRow("17") << qreal(17.0);
}

// public void setSpacing(qreal spacing)
void tst_QGraphicsGridLayout::setSpacing()
{
    QFETCH(qreal, spacing);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    QSizeF sh = layout->sizeHint(Qt::PreferredSize, QSizeF());
    qreal oldVSpacing = layout->verticalSpacing();
    qreal oldHSpacing = layout->horizontalSpacing();
    if (oldVSpacing != -1) {
        layout->setSpacing(spacing);
        QApplication::processEvents();
        QSizeF newSH = layout->sizeHint(Qt::PreferredSize, QSizeF());
        QCOMPARE(newSH.height(), sh.height() - (2-1)*(oldVSpacing - spacing));
        QCOMPARE(newSH.width(), sh.width() - (3-1)*(oldHSpacing - spacing));
    } else {
        QSKIP("The current style uses non-uniform layout spacing", SkipAll);
    }
    delete widget;
}

void tst_QGraphicsGridLayout::sizeHint_data()
{
    QTest::addColumn<ItemList>("itemDescriptions");
    QTest::addColumn<QSizeF>("expectedMinimumSizeHint");
    QTest::addColumn<QSizeF>("expectedPreferredSizeHint");
    QTest::addColumn<QSizeF>("expectedMaximumSizeHint");

    QTest::newRow("rowSpan_larger_than_rows") << (ItemList()
                                    << ItemDesc(0,0)
                                        .minSize(QSizeF(50,300))
                                        .maxSize(QSizeF(50,300))
                                        .rowSpan(2)
                                    << ItemDesc(0,1)
                                        .minSize(QSizeF(50,0))
                                        .preferredSize(QSizeF(50,50))
                                        .maxSize(QSize(50, 1000))
                                    << ItemDesc(1,1)
                                        .minSize(QSizeF(50,0))
                                        .preferredSize(QSizeF(50,50))
                                        .maxSize(QSize(50, 1000))
                                )
                            << QSizeF(100, 300)
                            << QSizeF(100, 300)
                            << QSizeF(100, 2000);

    QTest::newRow("rowSpan_smaller_than_rows") << (ItemList()
                                    << ItemDesc(0,0)
                                        .minSize(QSizeF(50, 0))
                                        .preferredSize(QSizeF(50, 50))
                                        .maxSize(QSizeF(50, 300))
                                        .rowSpan(2)
                                    << ItemDesc(0,1)
                                        .minSize(QSizeF(50, 50))
                                        .preferredSize(QSizeF(50, 50))
                                        .maxSize(QSize(50, 50))
                                    << ItemDesc(1,1)
                                        .minSize(QSizeF(50, 50))
                                        .preferredSize(QSizeF(50, 50))
                                        .maxSize(QSize(50, 50))
                                )
                            << QSizeF(100, 100)
                            << QSizeF(100, 100)
                            << QSizeF(100, 100);

}

// public QSizeF sizeHint(Qt::SizeHint which, QSizeF const& constraint = QSizeF()) const
void tst_QGraphicsGridLayout::sizeHint()
{
    QFETCH(ItemList, itemDescriptions);
    QFETCH(QSizeF, expectedMinimumSizeHint);
    QFETCH(QSizeF, expectedPreferredSizeHint);
    QFETCH(QSizeF, expectedMaximumSizeHint);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0.0);
    widget->setContentsMargins(0, 0, 0, 0);

    int i;
    for (i = 0; i < itemDescriptions.count(); ++i) {
        ItemDesc desc = itemDescriptions.at(i);
        RectWidget *item = new RectWidget(widget);
        desc.apply(layout, item);
    }

    QApplication::sendPostedEvents(0, 0);

    widget->show();
    view.show();
    view.resize(400,300);
    QCOMPARE(layout->sizeHint(Qt::MinimumSize), expectedMinimumSizeHint);
    QCOMPARE(layout->sizeHint(Qt::PreferredSize), expectedPreferredSizeHint);
    QCOMPARE(layout->sizeHint(Qt::MaximumSize), expectedMaximumSizeHint);

}

void tst_QGraphicsGridLayout::verticalSpacing_data()
{
    QTest::addColumn<qreal>("verticalSpacing");
    QTest::newRow("zero") << qreal(0.0);
    QTest::newRow("17") << qreal(10.0);
}

// public qreal verticalSpacing() const
void tst_QGraphicsGridLayout::verticalSpacing()
{
    QFETCH(qreal, verticalSpacing);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout();
    scene.addItem(widget);
    widget->setLayout(layout);
    populateLayout(layout, 3, 2);
    layout->setContentsMargins(0, 0, 0, 0);
    qreal h = layout->sizeHint(Qt::PreferredSize, QSizeF()).height();
    qreal oldSpacing = layout->verticalSpacing();
    if (oldSpacing != -1) {
        layout->setVerticalSpacing(verticalSpacing);
        QApplication::processEvents();
        qreal new_h = layout->sizeHint(Qt::PreferredSize, QSizeF()).height();
        QCOMPARE(new_h, h - (2-1)*(oldSpacing - verticalSpacing));
    } else {
        QSKIP("The current style uses non-uniform layout spacing", SkipAll);
    }
    delete widget;
}

void tst_QGraphicsGridLayout::layoutDirection()
{
    QGraphicsScene scene;
    QGraphicsView view(&scene);

    QGraphicsWidget *window = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    layout->setContentsMargins(1, 2, 3, 4);
    layout->setSpacing(6);
    RectWidget *w1 = new RectWidget;
    w1->setMinimumSize(30, 20);
    layout->addItem(w1, 0, 0);
    RectWidget *w2 = new RectWidget;
    w2->setMinimumSize(20, 20);
    w2->setMaximumSize(20, 20);
    layout->addItem(w2, 0, 1);
    RectWidget *w3 = new RectWidget;
    w3->setMinimumSize(20, 20);
    w3->setMaximumSize(20, 20);
    layout->addItem(w3, 1, 0);
    RectWidget *w4 = new RectWidget;
    w4->setMinimumSize(30, 20);
    layout->addItem(w4, 1, 1);

    layout->setAlignment(w2, Qt::AlignRight);
    layout->setAlignment(w3, Qt::AlignLeft);

    scene.addItem(window);
    window->setLayout(layout);
    view.show();
    window->resize(70, 52);
    QApplication::processEvents();
    QCOMPARE(w1->geometry().left(), 1.0);
    QCOMPARE(w1->geometry().right(), 31.0);
    QCOMPARE(w2->geometry().left(), 47.0);
    QCOMPARE(w2->geometry().right(), 67.0);
    QCOMPARE(w3->geometry().left(), 1.0);
    QCOMPARE(w3->geometry().right(), 21.0);
    QCOMPARE(w4->geometry().left(), 37.0);
    QCOMPARE(w4->geometry().right(), 67.0);

    window->setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QCOMPARE(w1->geometry().left(),  39.0);
    QCOMPARE(w1->geometry().right(), 69.0);
    QCOMPARE(w2->geometry().left(),   3.0);
    QCOMPARE(w2->geometry().right(), 23.0);
    QCOMPARE(w3->geometry().left(),  49.0);
    QCOMPARE(w3->geometry().right(), 69.0);
    QCOMPARE(w4->geometry().left(),   3.0);
    QCOMPARE(w4->geometry().right(), 33.0);

    delete window;
}

void tst_QGraphicsGridLayout::removeLayout()
{
    QGraphicsScene scene;
    RectWidget *textEdit = new RectWidget;
    RectWidget *pushButton = new RectWidget;
    scene.addItem(textEdit);
    scene.addItem(pushButton);

    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    layout->addItem(textEdit, 0, 0);
    layout->addItem(pushButton, 1, 0);

    QGraphicsWidget *form = new QGraphicsWidget;
    form->setLayout(layout);
    scene.addItem(form);

    QGraphicsView view(&scene);
    view.show();
    QTest::qWait(20);

    QRectF r1 = textEdit->geometry();
    QRectF r2 = pushButton->geometry();
    form->setLayout(0);
    //documentation of QGraphicsWidget::setLayout:
    //If layout is 0, the widget is left without a layout. Existing subwidgets' geometries will remain unaffected.
    QCOMPARE(textEdit->geometry(), r1);
    QCOMPARE(pushButton->geometry(), r2);
}

void tst_QGraphicsGridLayout::defaultStretchFactors_data()
{
    QTest::addColumn<ItemList>("itemDescriptions");
    QTest::addColumn<QSizeF>("newSize");
    QTest::addColumn<SizeList>("expectedSizes");

    QTest::newRow("usepreferredsize") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem01") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem01_resize120x40") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(20,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(30,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(20,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(30,10))
                                )
                            << QSizeF(120, 40)
                            << (SizeList()
                                << QSizeF(20,20) << QSizeF(40,20) << QSizeF(60,20)
                                << QSizeF(20,20) << QSizeF(40,20) << QSizeF(60,20)
                            );

    QTest::newRow("ignoreitem11_resize120x40") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(20,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(30,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                    << ItemDesc(1,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(20,20))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(30,20))
                                )
                            << QSizeF(120, 60)
                            << (SizeList()
                                << QSizeF(20,20) << QSizeF(40,20) << QSizeF(60,20)
                                << QSizeF(20,40) << QSizeF(40,40) << QSizeF(60,40)
                            );

    QTest::newRow("ignoreitem01_span01_resize70x60") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(20,10))
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .rowSpan(2)
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(30,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(30,20))
                                )
                            << QSizeF(70, 60)
                            << (SizeList()
                                << QSizeF(20,20) << QSizeF(10,60) << QSizeF(40,20)
                                << QSizeF(20,40) << QSizeF(40,40)
                            );

    QTest::newRow("ignoreitem10_resize40x120") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,20))
                                    << ItemDesc(2,0)
                                        .preferredSizeHint(QSizeF(10,30))
                                    << ItemDesc(2,1)
                                        .preferredSizeHint(QSizeF(10,30))
                                )
                            << QSizeF(40, 120)
                            << (SizeList()
                                << QSizeF(20,20) << QSizeF(20,20)
                                << QSizeF(20,40) << QSizeF(20,40)
                                << QSizeF(20,60) << QSizeF(20,60)
                            );

    QTest::newRow("ignoreitem01_span02") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .rowSpan(2)
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(0,20) << QSizeF(10,10)
                                << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem02_span02") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .rowSpan(2)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(0,20)
                                << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem02_span00_span02") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .rowSpan(2)
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .rowSpan(2)
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,20) << QSizeF(10,10) << QSizeF(0,20)
                                << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem00_colspan00") << (ItemList()
                                    << ItemDesc(0,0)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .colSpan(2)
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(20,10) << QSizeF(10,10) << QSizeF(10,10)
                                << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignoreitem01_colspan01") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .colSpan(2)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(20,10) << QSizeF(10,10)
                                << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignorecolumn1_resize70x60") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(20,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(30,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                    << ItemDesc(1,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(20,20))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(30,20))
                                )
                            << QSizeF(70, 60)
                            << (SizeList()
                                << QSizeF(20,20) << QSizeF(10,20) << QSizeF(40,20)
                                << QSizeF(20,40) << QSizeF(10,40) << QSizeF(40,40)
                            );

    QTest::newRow("ignorerow0") << (ItemList()
                                    << ItemDesc(0,0)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,0) << QSizeF(10,0) << QSizeF(10,0)
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                            );

    QTest::newRow("ignorerow1") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,2)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,0)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(1,2)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                )
                            << QSizeF()
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(10,10) << QSizeF(10,10)
                                << QSizeF(10,0) << QSizeF(10,0) << QSizeF(10,0)
                            );

    QTest::newRow("ignorerow0_resize60x50") << (ItemList()
                                    << ItemDesc(0,0)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(10,10))
                                    << ItemDesc(0,1)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(20,10))
                                    << ItemDesc(0,2)
                                        .sizePolicy(QSizePolicy::Ignored)
                                        .preferredSizeHint(QSizeF(30,10))
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,30))
                                    << ItemDesc(1,1)
                                        .preferredSizeHint(QSizeF(20,30))
                                    << ItemDesc(1,2)
                                        .preferredSizeHint(QSizeF(30,30))
                                )
                            << QSizeF(60, 50)
                            << (SizeList()
                                << QSizeF(10,10) << QSizeF(20,10) << QSizeF(30,10)
                                << QSizeF(10,40) << QSizeF(20,40) << QSizeF(30,40)
                            );

}

void tst_QGraphicsGridLayout::defaultStretchFactors()
{
    QFETCH(ItemList, itemDescriptions);
    QFETCH(QSizeF, newSize);
    QFETCH(SizeList, expectedSizes);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0.0);
    widget->setContentsMargins(0, 0, 0, 0);

    int i;
    for (i = 0; i < itemDescriptions.count(); ++i) {
        ItemDesc desc = itemDescriptions.at(i);
        RectWidget *item = new RectWidget(widget);
        desc.apply(layout, item);
    }

    QApplication::sendPostedEvents(0, 0);

    widget->show();
    view.show();
    view.resize(400,300);
    if (newSize.isValid())
        widget->resize(newSize);

    QApplication::sendPostedEvents(0, 0);
    for (i = 0; i < expectedSizes.count(); ++i) {
        QSizeF itemSize = layout->itemAt(i)->geometry().size();
        QCOMPARE(itemSize, expectedSizes.at(i));
    }

    delete widget;
}

typedef QList<QRectF> RectList;
Q_DECLARE_METATYPE(RectList);

void tst_QGraphicsGridLayout::alignment2_data()
{
    QTest::addColumn<ItemList>("itemDescriptions");
    QTest::addColumn<QSizeF>("newSize");
    QTest::addColumn<RectList>("expectedGeometries");

    QTest::newRow("hor_sizepolicy_fixed") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 10,20) << QRectF(10, 0, 10,10)
                            );

    QTest::newRow("hor_sizepolicy_fixed_alignvcenter") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignVCenter)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 10,20) << QRectF(10, 5, 10,10)
                            );

    QTest::newRow("hor_sizepolicy_fixed_aligntop") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignTop)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 10,20) << QRectF(10, 0, 10,10)
                            );

    QTest::newRow("hor_sizepolicy_fixed_alignbottom") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(10,20))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                    << ItemDesc(0,1)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyV(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignBottom)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 10,20) << QRectF(10, 10, 10,10)
                            );

    QTest::newRow("ver_sizepolicy_fixed") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(20,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 20,10) << QRectF(0, 10, 10,10)
                            );

    QTest::newRow("ver_sizepolicy_fixed_alignhcenter") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(20,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignHCenter)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 20,10) << QRectF(5, 10, 10,10)
                            );

    QTest::newRow("ver_sizepolicy_fixed_alignleft") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(20,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignLeft)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 20,10) << QRectF(0, 10, 10,10)
                            );

    QTest::newRow("ver_sizepolicy_fixed_alignright") << (ItemList()
                                    << ItemDesc(0,0)
                                        .preferredSizeHint(QSizeF(20,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                    << ItemDesc(1,0)
                                        .preferredSizeHint(QSizeF(10,10))
                                        .sizePolicyH(QSizePolicy::Fixed)
                                        .alignment(Qt::AlignRight)
                                )
                            << QSizeF()
                            << (RectList()
                                << QRectF(0, 0, 20,10) << QRectF(10, 10, 10,10)
                            );
}

void tst_QGraphicsGridLayout::alignment2()
{
    QFETCH(ItemList, itemDescriptions);
    QFETCH(QSizeF, newSize);
    QFETCH(RectList, expectedGeometries);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0.0);
    widget->setContentsMargins(0, 0, 0, 0);

    int i;
    for (i = 0; i < itemDescriptions.count(); ++i) {
        ItemDesc desc = itemDescriptions.at(i);
        RectWidget *item = new RectWidget(widget);
        desc.apply(layout, item);
    }

    QApplication::sendPostedEvents(0, 0);

    widget->show();
    view.resize(400,300);
    view.show();
    if (newSize.isValid())
        widget->resize(newSize);

    QApplication::sendPostedEvents(0, 0);
    for (i = 0; i < expectedGeometries.count(); ++i) {
        QRectF itemRect = layout->itemAt(i)->geometry();
        QCOMPARE(itemRect, expectedGeometries.at(i));
    }

    delete widget;
}

void tst_QGraphicsGridLayout::geometries_data()
{

    QTest::addColumn<ItemList>("itemDescriptions");
    QTest::addColumn<QSizeF>("newSize");
    QTest::addColumn<RectList>("expectedGeometries");

    QTest::newRow("combine_max_sizes") << (ItemList()
                                    << ItemDesc(0,0)
                                        .maxSize(QSizeF(50,10))
                                    << ItemDesc(1,0)
                                        .maxSize(QSizeF(10,10))
                                )
                            << QSizeF(50, 20)
                            << (RectList()
                                << QRectF(0, 0, 50,10) << QRectF(0, 10, 10,10)
                            );

    QTest::newRow("combine_min_sizes") << (ItemList()
                                    << ItemDesc(0,0)
                                        .minSize(QSizeF(50,10))
                                    << ItemDesc(1,0)
                                        .minSize(QSizeF(10,10))
                                )
                            << QSizeF(60, 20)
                            << (RectList()
                                << QRectF(0, 0, 60,10) << QRectF(0, 10, 60,10)
                            );

}

void tst_QGraphicsGridLayout::geometries()
{
    QFETCH(ItemList, itemDescriptions);
    QFETCH(QSizeF, newSize);
    QFETCH(RectList, expectedGeometries);

    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsWidget *widget = new QGraphicsWidget(0, Qt::Window);
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    scene.addItem(widget);
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0.0);
    widget->setContentsMargins(0, 0, 0, 0);

    int i;
    for (i = 0; i < itemDescriptions.count(); ++i) {
        ItemDesc desc = itemDescriptions.at(i);
        RectWidget *item = new RectWidget(widget);
        desc.apply(layout, item);
    }

    QApplication::processEvents();

    widget->show();
    view.resize(400,300);
    view.show();
    if (newSize.isValid())
        widget->resize(newSize);

    QApplication::processEvents();
    for (i = 0; i < expectedGeometries.count(); ++i) {
        QRectF itemRect = layout->itemAt(i)->geometry();
        QCOMPARE(itemRect, expectedGeometries.at(i));
    }

    delete widget;
}

void tst_QGraphicsGridLayout::avoidRecursionInInsertItem()
{
    QGraphicsWidget window(0, Qt::Window);
	QGraphicsGridLayout *layout = new QGraphicsGridLayout(&window);
    QCOMPARE(layout->count(), 0);
    QTest::ignoreMessage(QtWarningMsg, "QGraphicsGridLayout::addItem: cannot insert itself");
    layout->addItem(layout, 0, 0);
    QCOMPARE(layout->count(), 0);
}

void tst_QGraphicsGridLayout::styleInfoLeak()
{
    QGraphicsGridLayout grid;
    grid.horizontalSpacing();
}

void tst_QGraphicsGridLayout::task236367_maxSizeHint()
{
    QGraphicsWidget *widget = new QGraphicsWidget;
    QGraphicsGridLayout *layout = new QGraphicsGridLayout;
    widget->setLayout(layout);
    layout->setContentsMargins(0, 0, 0, 0);
    int w = 203;
    int h = 204;
    widget->resize(w, h);
    QCOMPARE(widget->size(), QSizeF(w, h));
}

QTEST_MAIN(tst_QGraphicsGridLayout)
#include "tst_qgraphicsgridlayout.moc"

