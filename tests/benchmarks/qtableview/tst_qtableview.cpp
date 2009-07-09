/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QDebug>
#include <QTableView>
#include <QImage>
#include <QPainter>

//TESTED_FILES=

class QtTestTableModel: public QAbstractTableModel
{
    Q_OBJECT


public:
    QtTestTableModel(int rows = 0, int columns = 0, QObject *parent = 0)
        : QAbstractTableModel(parent),
          row_count(rows),
          column_count(columns) {}

    int rowCount(const QModelIndex& = QModelIndex()) const { return row_count; }
    int columnCount(const QModelIndex& = QModelIndex()) const { return column_count; }
    bool isEditable(const QModelIndex &) const { return true; }

    QVariant data(const QModelIndex &idx, int role) const
    {
        if (!idx.isValid() || idx.row() >= row_count || idx.column() >= column_count) {
            qWarning() << "Invalid modelIndex [%d,%d,%p]" << idx;
            return QVariant();
        }

        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString("[%1,%2,%3]").arg(idx.row()).arg(idx.column()).arg(0);

        return QVariant();
    }

    int row_count;
    int column_count;
};




class tst_QTableView : public QObject
{
    Q_OBJECT

public:
    tst_QTableView();
    virtual ~tst_QTableView();

public slots:
    void init();
    void cleanup();

private slots:
    void spanInit();
    void spanDraw();
    void spanSelectColumn();
    void spanSelectAll();
private:
    static inline void spanInit_helper(QTableView *);
};

tst_QTableView::tst_QTableView()
{
}

tst_QTableView::~tst_QTableView()
{
}

void tst_QTableView::init()
{
}

void tst_QTableView::cleanup()
{
}

void tst_QTableView::spanInit_helper(QTableView *view)
{
    for (int i=0; i < 40; i++) {
        view->setSpan(1+i%2, 1+4*i, 1+i%3, 2);
    }
    
    for (int i=1; i < 40; i++) {
        view->setSpan(6 + i*7, 4, 4, 50);
    }
}

void tst_QTableView::spanInit()
{
    QtTestTableModel model(500, 500);
    QTableView v;
    v.setModel(&model);
    
    QBENCHMARK {
        spanInit_helper(&v);
    }
}

void tst_QTableView::spanDraw()
{
    QtTestTableModel model(500, 500);
    QTableView v;
    v.setModel(&model);
    
    spanInit_helper(&v);
    v.show();
    v.resize(500,500);
    QTest::qWait(30);

    QImage image(500, 500, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);
    QBENCHMARK {
        v.render(&painter);
    }
}

void tst_QTableView::spanSelectAll()
{
    QtTestTableModel model(500, 500);
    QTableView v;
    v.setModel(&model);
    
    spanInit_helper(&v);
    v.show();
    QTest::qWait(30);
    
    QBENCHMARK {
        v.selectAll();
    }
}

void tst_QTableView::spanSelectColumn()
{
    QtTestTableModel model(500, 500);
    QTableView v;
    v.setModel(&model);
    
    spanInit_helper(&v);
    v.show();
    QTest::qWait(30);
    
    QBENCHMARK {
        v.selectColumn(22);
    }
}

QTEST_MAIN(tst_QTableView)
#include "tst_qtableview.moc"
