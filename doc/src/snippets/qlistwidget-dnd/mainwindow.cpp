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

#include "mainwindow.h"

MainWindow::MainWindow()
{
    QMenu *fileMenu = new QMenu(tr("&File"));

    QAction *quitAction = fileMenu->addAction(tr("E&xit"));
    quitAction->setShortcut(tr("Ctrl+Q"));

    menuBar()->addMenu(fileMenu);

//  For convenient quoting:
//! [0]
QListWidget *listWidget = new QListWidget(this);
listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
listWidget->setDragEnabled(true);
listWidget->viewport()->setAcceptDrops(true);
listWidget->setDropIndicatorShown(true);
//! [0] //! [1]
listWidget->setDragDropMode(QAbstractItemView::InternalMove);
//! [1]

    this->listWidget = listWidget;

    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    setupListItems();

    setCentralWidget(listWidget);
    setWindowTitle(tr("List Widget"));
}

void MainWindow::setupListItems()
{
    QListWidgetItem *item;
    item = new QListWidgetItem(tr("Oak"), listWidget);
    item = new QListWidgetItem(tr("Fir"), listWidget);
    item = new QListWidgetItem(tr("Pine"), listWidget);
    item = new QListWidgetItem(tr("Birch"), listWidget);
    item = new QListWidgetItem(tr("Hazel"), listWidget);
    item = new QListWidgetItem(tr("Redwood"), listWidget);
    item = new QListWidgetItem(tr("Sycamore"), listWidget);
    item = new QListWidgetItem(tr("Chestnut"), listWidget);
    item = new QListWidgetItem(tr("Mahogany"), listWidget);
}
