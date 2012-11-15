/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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

/********************************************************************************
** Form generated from reading ui file 'identifierpage.ui'
**
** Created: Thu Jul 10 09:47:34 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef IDENTIFIERPAGE_H
#define IDENTIFIERPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_IdentifierPage
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QCheckBox *identifierCheckBox;
    QSpacerItem *spacerItem1;
    QSpacerItem *spacerItem2;
    QRadioButton *globalButton;
    QLineEdit *prefixLineEdit;
    QRadioButton *fileNameButton;
    QSpacerItem *spacerItem3;

    void setupUi(QWidget *IdentifierPage)
    {
        if (IdentifierPage->objectName().isEmpty())
            IdentifierPage->setObjectName(QString::fromUtf8("IdentifierPage"));
        IdentifierPage->resize(417, 242);
        gridLayout = new QGridLayout(IdentifierPage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        spacerItem = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(spacerItem, 0, 1, 1, 1);

        identifierCheckBox = new QCheckBox(IdentifierPage);
        identifierCheckBox->setObjectName(QString::fromUtf8("identifierCheckBox"));

        gridLayout->addWidget(identifierCheckBox, 1, 0, 1, 3);

        spacerItem1 = new QSpacerItem(161, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem1, 1, 3, 1, 1);

        spacerItem2 = new QSpacerItem(30, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem2, 2, 0, 1, 1);

        globalButton = new QRadioButton(IdentifierPage);
        globalButton->setObjectName(QString::fromUtf8("globalButton"));
        globalButton->setEnabled(false);
        globalButton->setChecked(true);

        gridLayout->addWidget(globalButton, 2, 1, 1, 1);

        prefixLineEdit = new QLineEdit(IdentifierPage);
        prefixLineEdit->setObjectName(QString::fromUtf8("prefixLineEdit"));
        prefixLineEdit->setEnabled(false);

        gridLayout->addWidget(prefixLineEdit, 2, 2, 1, 1);

        fileNameButton = new QRadioButton(IdentifierPage);
        fileNameButton->setObjectName(QString::fromUtf8("fileNameButton"));
        fileNameButton->setEnabled(false);

        gridLayout->addWidget(fileNameButton, 3, 1, 1, 2);

        spacerItem3 = new QSpacerItem(20, 31, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem3, 4, 1, 1, 1);


        retranslateUi(IdentifierPage);
        QObject::connect(globalButton, SIGNAL(toggled(bool)), prefixLineEdit, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(IdentifierPage);
    } // setupUi

    void retranslateUi(QWidget *IdentifierPage)
    {
        IdentifierPage->setWindowTitle(QApplication::translate("IdentifierPage", "Form", 0, QApplication::UnicodeUTF8));
        identifierCheckBox->setText(QApplication::translate("IdentifierPage", "Create identifiers", 0, QApplication::UnicodeUTF8));
        globalButton->setText(QApplication::translate("IdentifierPage", "Global prefix:", 0, QApplication::UnicodeUTF8));
        fileNameButton->setText(QApplication::translate("IdentifierPage", "Inherit prefix from file names", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(IdentifierPage);
    } // retranslateUi

};

namespace Ui {
    class IdentifierPage: public Ui_IdentifierPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // IDENTIFIERPAGE_H
