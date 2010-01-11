/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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
#include "filereader.h"


FileReader::FileReader(QWidget *parent)
    : QWidget(parent)
{
    textEdit = new QTextEdit;

    taxFileButton = new QPushButton("Tax File");
    accountFileButton = new QPushButton("Accounts File");
    reportFileButton = new QPushButton("Report File");

//! [0]
    signalMapper = new QSignalMapper(this);
    signalMapper->setMapping(taxFileButton, QString("taxfile.txt"));
    signalMapper->setMapping(accountFileButton, QString("accountsfile.txt"));
    signalMapper->setMapping(reportFileButton, QString("reportfile.txt"));

    connect(taxFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
    connect(accountFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
    connect(reportFileButton, SIGNAL(clicked()),
        signalMapper, SLOT (map()));
//! [0]

//! [1]
    connect(signalMapper, SIGNAL(mapped(const QString &)),
        this, SLOT(readFile(const QString &)));
//! [1]

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(taxFileButton);
    buttonLayout->addWidget(accountFileButton);
    buttonLayout->addWidget(reportFileButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void FileReader::readFile(const QString &filename)
{
    QFile file(filename);
    
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
            file.errorString());
        return;
    }


    QTextStream in(&file);
    textEdit->setPlainText(in.readAll());
}

