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
    setCentralWidget(central);
    infoLabel = new QLabel(tr("Open menu to start!"));

    layout = new QVBoxLayout;
    layout->addWidget(infoLabel);
    central->setLayout(layout);
    central->setFocusPolicy(Qt::TabFocus);
    
    fileMenu = menuBar()->addMenu(tr("&File"));
    openDialogAct = new QAction(tr("&Open Dialog"), this);
    addSoftKeysAct = new QAction(tr("&Add Softkeys"), this);
    clearSoftKeysAct = new QAction(tr("&Clear Softkeys"), this);
    fileMenu->addAction(openDialogAct);
    fileMenu->addAction(addSoftKeysAct);
    fileMenu->addAction(clearSoftKeysAct);
    connect(openDialogAct, SIGNAL(triggered()), this, SLOT(openDialog()));    
    connect(addSoftKeysAct, SIGNAL(triggered()), this, SLOT(addSoftKeys()));    
    connect(clearSoftKeysAct, SIGNAL(triggered()), this, SLOT(clearSoftKeys()));    
}

MainWindow::~MainWindow()
{
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
    central->setSoftKeys(softkeys);
    central->setFocus();
    
}

void MainWindow::clearSoftKeys()
{
    central->setSoftKey(0);
}

void MainWindow::okPressed()
{
    infoLabel->setText(tr("OK pressed"));
    central->setSoftKey(0);
}

void MainWindow::cancelPressed()
{
    infoLabel->setText(tr("Cancel pressed"));
    central->setSoftKey(0);
}
