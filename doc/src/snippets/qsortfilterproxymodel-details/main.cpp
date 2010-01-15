/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtGui>
#include <QApplication>
#include <QSortFilterProxyModel>

class MyItemModel : public QStandardItemModel
{
public:
    MyItemModel(QWidget *parent = 0);
};

MyItemModel::MyItemModel(QWidget *parent)
    : QStandardItemModel(parent)
{};

class Widget : public QWidget
{
public:
    Widget(QWidget *parent = 0);
};

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
//! [0] //! [1]
        QTreeView *treeView = new QTreeView;
//! [0]
        MyItemModel *model = new MyItemModel(this);

        treeView->setModel(model);
//! [1]

//! [2]
        MyItemModel *sourceModel = new MyItemModel(this);
        QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);

        proxyModel->setSourceModel(sourceModel);
        treeView->setModel(proxyModel);
//! [2]

//! [3]
        treeView->setSortingEnabled(true);
//! [3]

//! [4]
        proxyModel->sort(2, Qt::AscendingOrder);
//! [4] //! [5]
        proxyModel->setFilterRegExp(QRegExp(".png", Qt::CaseInsensitive,
                                            QRegExp::FixedString));
        proxyModel->setFilterKeyColumn(1);
//! [5]
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Widget widget;
    widget.show();
    return app.exec();
}
