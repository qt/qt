/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "softkeys.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    central = new QWidget(this);
    central->setContextMenuPolicy(Qt::NoContextMenu); // explicitly forbid usage of context menu so actions item is not shown menu
    setCentralWidget(central);

    // Create text editor and set softkeys to it
    textEditor= new QTextEdit(tr("Navigate in UI to see context sensitive softkeys in action"), this);
    QAction* clear = new QAction(tr("Clear"), this);
    clear->setSoftKeyRole(QAction::NegativeSoftKey);

    textEditor->addAction(clear);

    ok = new QAction(tr("Ok"), this);
    ok->setSoftKeyRole(QAction::PositiveSoftKey);
    connect(ok, SIGNAL(triggered()), this, SLOT(okPressed()));

    cancel = new QAction(tr("Cancel"), this);
    cancel->setSoftKeyRole(QAction::NegativeSoftKey);
    connect(cancel, SIGNAL(triggered()), this, SLOT(cancelPressed()));

    infoLabel = new QLabel(tr(""), this);
    infoLabel->setContextMenuPolicy(Qt::NoContextMenu);

    toggleButton = new QPushButton(tr("Custom"), this);
    toggleButton->setContextMenuPolicy(Qt::NoContextMenu);
    toggleButton->setCheckable(true);

    pushButton = new QPushButton(tr("File Dialog"), this);
    pushButton->setContextMenuPolicy(Qt::NoContextMenu);

    QComboBox* comboBox = new QComboBox(this);
    comboBox->setContextMenuPolicy(Qt::NoContextMenu);
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("MainWindow", "Selection1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("MainWindow", "Selection2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("MainWindow", "Selection3", 0, QApplication::UnicodeUTF8)
    );

    layout = new QGridLayout;
    layout->addWidget(textEditor, 0, 0, 1, 2);
    layout->addWidget(infoLabel, 1, 0, 1, 2);
    layout->addWidget(toggleButton, 2, 0);
    layout->addWidget(pushButton, 2, 1);
    layout->addWidget(comboBox, 3, 0, 1, 2);
    central->setLayout(layout);

    fileMenu = menuBar()->addMenu(tr("&File"));
    exit = new QAction(tr("&Exit"), this);
    fileMenu->addAction(exit);

    connect(clear, SIGNAL(triggered()), this, SLOT(clearTextEditor()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(openDialog()));
    connect(exit, SIGNAL(triggered()), this, SLOT(exitApplication()));
    connect(toggleButton, SIGNAL(clicked()), this, SLOT(setCustomSoftKeys()));
    pushButton->setFocus();
}

MainWindow::~MainWindow()
{
}

void MainWindow::clearTextEditor()
{
    textEditor->setText(tr(""));
}

void MainWindow::openDialog()
{
    QFileDialog::getOpenFileName(this);
}

void MainWindow::addSoftKeys()
{
    addAction(ok);
    addAction(cancel);
}

void MainWindow::setCustomSoftKeys()
{
    if (toggleButton->isChecked()) {
        infoLabel->setText(tr("Custom softkeys set"));
        addSoftKeys();
        }
    else {
        infoLabel->setText(tr("Custom softkeys removed"));
        removeAction(ok);
        removeAction(cancel);
    }
}

void MainWindow::exitApplication()
{
    qApp->exit();
}

void MainWindow::okPressed()
{
    infoLabel->setText(tr("OK pressed"));
}

void MainWindow::cancelPressed()
{
    infoLabel->setText(tr("Cancel pressed"));
}


