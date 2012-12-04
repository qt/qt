/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** All rights reserved.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the documentation of the Qt Toolkit.
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

#include <QtGui>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Dock Widgets");

    setupDockWindow();
    setupContents();
    setupMenus();

    textBrowser = new QTextBrowser(this);

    connect(headingList, SIGNAL(itemClicked(QListWidgetItem *)),
            this, SLOT(updateText(QListWidgetItem *)));

    updateText(headingList->item(0));
    headingList->setCurrentRow(0);
    setCentralWidget(textBrowser);
}

void MainWindow::setupContents()
{
    QFile titlesFile(":/Resources/titles.txt");
    titlesFile.open(QFile::ReadOnly);
    int chapter = 0;

    do {
        QString line = titlesFile.readLine().trimmed();
        QStringList parts = line.split("\t", QString::SkipEmptyParts);
        if (parts.size() != 2)
            break;

        QString chapterTitle = parts[0];
        QString fileName = parts[1];

        QFile chapterFile(fileName);

        chapterFile.open(QFile::ReadOnly);
        QListWidgetItem *item = new QListWidgetItem(chapterTitle, headingList);
        item->setData(Qt::DisplayRole, chapterTitle);
        item->setData(Qt::UserRole, chapterFile.readAll());
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        chapterFile.close();

        chapter++;
    } while (titlesFile.isOpen());

    titlesFile.close();
}

void MainWindow::setupDockWindow()
{
//! [0]
    contentsWindow = new QDockWidget(tr("Table of Contents"), this);
    contentsWindow->setAllowedAreas(Qt::LeftDockWidgetArea
                                  | Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, contentsWindow);

    headingList = new QListWidget(contentsWindow);
    contentsWindow->setWidget(headingList);
//! [0]
}

void MainWindow::setupMenus()
{
    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);
}

void MainWindow::updateText(QListWidgetItem *item)
{
    QString text = item->data(Qt::UserRole).toString();
    textBrowser->setHtml(text);
}
