/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at qt-sales@nokia.com.
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
    textEditor= new QTextEdit(tr("Choose items from menu or navigate around with tab+selection to see what softkeys can do"), this);
    QAction* fill = new QAction(tr("Fill"), this);
    fill->setSoftKeyRole(QAction::OkSoftKey);
    QAction* clear = new QAction(tr("Clear"), this);
    clear->setSoftKeyRole(QAction::CancelSoftKey);

    QList<QAction*> textEditorSoftKeys;
    textEditorSoftKeys.append(fill);
    textEditorSoftKeys.append(clear);
    textEditor->setSoftKeys(textEditorSoftKeys);

    infoLabel = new QLabel(tr(""), this);
    infoLabel->setContextMenuPolicy(Qt::NoContextMenu);

    pushButton = new QPushButton(tr("Open File Dialog"), this);
    pushButton->setContextMenuPolicy(Qt::NoContextMenu);

    QComboBox* comboBox = new QComboBox(this);
    comboBox->setContextMenuPolicy(Qt::NoContextMenu);
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("MainWindow", "Selection1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("MainWindow", "Selection2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("MainWindow", "Selection3", 0, QApplication::UnicodeUTF8)
    );

    layout = new QVBoxLayout;
    layout->addWidget(textEditor);
    layout->addWidget(infoLabel);
    layout->addWidget(pushButton);
    layout->addWidget(comboBox);
    central->setLayout(layout);

    fileMenu = menuBar()->addMenu(tr("&File"));
    addSoftKeysAct = new QAction(tr("&Add Softkeys"), this);
    fileMenu->addAction(addSoftKeysAct);
    exit = new QAction(tr("&Exit"), this);
    fileMenu->addAction(exit);

    connect(fill, SIGNAL(triggered()), this, SLOT(fillTextEditor()));
    connect(clear, SIGNAL(triggered()), this, SLOT(clearTextEditor()));
    connect(pushButton, SIGNAL(clicked()), this, SLOT(openDialog()));
    connect(addSoftKeysAct, SIGNAL(triggered()), this, SLOT(addSoftKeys()));
    connect(exit, SIGNAL(triggered()), this, SLOT(exitApplication()));
    pushButton->setFocus();
}

MainWindow::~MainWindow()
{
}

void MainWindow::fillTextEditor()
{
    textEditor->setText(tr("The fill softkey was pressed and here is some text to fill the editor"));
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
    ok = new QAction(tr("Ok"), this);
    ok->setSoftKeyRole(QAction::OkSoftKey);
    connect(ok, SIGNAL(triggered()), this, SLOT(okPressed()));

    cancel = new QAction(tr("Cancel"), this);
    cancel->setSoftKeyRole(QAction::OkSoftKey);
    connect(cancel, SIGNAL(triggered()), this, SLOT(cancelPressed()));

    QList<QAction*> softkeys;
    softkeys.append(ok);
    softkeys.append(cancel);
    pushButton->setSoftKeys(softkeys);
}

void MainWindow::exitApplication()
{
    qApp->exit();
}

void MainWindow::okPressed()
{
    infoLabel->setText(tr("OK pressed"));
    pushButton->setSoftKey(0);
}

void MainWindow::cancelPressed()
{
    infoLabel->setText(tr("Cancel pressed"));
    pushButton->setSoftKey(0);
}



