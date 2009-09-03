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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created: Tue Aug 18 19:03:31 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QGridLayout *gridLayout;
    QPushButton *loadFromFileButton;
    QLabel *label;
    QPushButton *loadFromSharedMemoryButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QString::fromUtf8("Dialog"));
        Dialog->resize(451, 322);
        gridLayout = new QGridLayout(Dialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        loadFromFileButton = new QPushButton(Dialog);
        loadFromFileButton->setObjectName(QString::fromUtf8("loadFromFileButton"));

        gridLayout->addWidget(loadFromFileButton, 0, 0, 1, 1);

        label = new QLabel(Dialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        loadFromSharedMemoryButton = new QPushButton(Dialog);
        loadFromSharedMemoryButton->setObjectName(QString::fromUtf8("loadFromSharedMemoryButton"));

        gridLayout->addWidget(loadFromSharedMemoryButton, 2, 0, 1, 1);


        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", 0, QApplication::UnicodeUTF8));
        loadFromFileButton->setText(QApplication::translate("Dialog", "Load Image From File...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Dialog", "Launch two of these dialogs.  In the first, press the top button and load an image from a file.  In the second, press the bottom button and display the loaded image from shared memory.", 0, QApplication::UnicodeUTF8));
        loadFromSharedMemoryButton->setText(QApplication::translate("Dialog", "Display Image From Shared Memory", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Dialog);
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DIALOG_H
