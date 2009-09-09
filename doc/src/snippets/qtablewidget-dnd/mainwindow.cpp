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

#include <QtGui>
#include "math.h"

#include "mainwindow.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    QMenu *itemsMenu = new QMenu(tr("&Items"));

    QAction *sumItemsAction = itemsMenu->addAction(tr("&Sum Items"));
    QAction *averageItemsAction = itemsMenu->addAction(tr("&Average Items"));

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(itemsMenu);

    tableWidget = new QTableWidget(12, 3, this);
    tableWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tableWidget->setDragEnabled(true);
    tableWidget->setAcceptDrops(true);
    tableWidget->setDropIndicatorShown(true);

    QTableWidgetItem *valuesHeaderItem = new QTableWidgetItem(tr("Values"));
    tableWidget->setHorizontalHeaderItem(0, valuesHeaderItem);
    valuesHeaderItem->setTextAlignment(Qt::AlignVCenter);
    QTableWidgetItem *squaresHeaderItem = new QTableWidgetItem(tr("Squares"));
    squaresHeaderItem->setIcon(QIcon(QPixmap(":/Images/squared.png")));
    squaresHeaderItem->setTextAlignment(Qt::AlignVCenter);
    QTableWidgetItem *cubesHeaderItem = new QTableWidgetItem(tr("Cubes"));
    cubesHeaderItem->setIcon(QIcon(QPixmap(":/Images/cubed.png")));
    cubesHeaderItem->setTextAlignment(Qt::AlignVCenter);
    tableWidget->setHorizontalHeaderItem(1, squaresHeaderItem);
    tableWidget->setHorizontalHeaderItem(2, cubesHeaderItem);

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(sumItemsAction, SIGNAL(triggered()), this, SLOT(sumItems()));
    connect(averageItemsAction, SIGNAL(triggered()), this, SLOT(averageItems()));

    setupTableItems();

    setCentralWidget(tableWidget);
    setWindowTitle(tr("Table Widget"));
}

void MainWindow::setupTableItems()
{
    for (int row = 0; row < tableWidget->rowCount()-1; ++row) {
        for (int column = 0; column < tableWidget->columnCount(); ++column) {
    QTableWidgetItem *newItem = new QTableWidgetItem(tr("%1").arg(
        pow((float)row, (float)column+1)));
    tableWidget->setItem(row, column, newItem);
        }
    }
    for (int column = 0; column < tableWidget->columnCount(); ++column) {
        QTableWidgetItem *newItem = new QTableWidgetItem;
        newItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        tableWidget->setItem(tableWidget->rowCount()-1, column, newItem);
    }
}

void MainWindow::averageItems()
{
    QList<QTableWidgetItem *> selected = tableWidget->selectedItems();
    QTableWidgetItem *item;
    int number = 0;
    double total = 0;

    foreach (item, selected) {
        bool ok;
        double value = item->text().toDouble(&ok);
        
        if (ok && !item->text().isEmpty()) {
            total += value;
            number++;
        }
    }
    if (number > 0)
        tableWidget->currentItem()->setText(QString::number(total/number));
}

void MainWindow::sumItems()
{
    QList<QTableWidgetItem *> selected = tableWidget->selectedItems();
    QTableWidgetItem *item;
    int number = 0;
    double total = 0;

    foreach (item, selected) {
        bool ok;
        double value = item->text().toDouble(&ok);

        if (ok && !item->text().isEmpty()) {
            total += value;
            number++;
        }
    }
    if (number > 0)
        tableWidget->currentItem()->setText(QString::number(total));
}
