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
** Form generated from reading ui file 'outputpage.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef OUTPUTPAGE_H
#define OUTPUTPAGE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OutputPage
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *spacerItem;
    QLabel *label;
    QLineEdit *projectLineEdit;
    QLabel *label_2;
    QLineEdit *collectionLineEdit;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *OutputPage)
    {
        if (OutputPage->objectName().isEmpty())
            OutputPage->setObjectName(QString::fromUtf8("OutputPage"));
        OutputPage->resize(417, 242);
        gridLayout = new QGridLayout(OutputPage);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        spacerItem = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(spacerItem, 0, 1, 1, 1);

        label = new QLabel(OutputPage);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        projectLineEdit = new QLineEdit(OutputPage);
        projectLineEdit->setObjectName(QString::fromUtf8("projectLineEdit"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(projectLineEdit->sizePolicy().hasHeightForWidth());
        projectLineEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(projectLineEdit, 1, 1, 1, 1);

        label_2 = new QLabel(OutputPage);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        sizePolicy.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy);

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        collectionLineEdit = new QLineEdit(OutputPage);
        collectionLineEdit->setObjectName(QString::fromUtf8("collectionLineEdit"));
        sizePolicy1.setHeightForWidth(collectionLineEdit->sizePolicy().hasHeightForWidth());
        collectionLineEdit->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(collectionLineEdit, 2, 1, 1, 1);

        spacerItem1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(spacerItem1, 3, 1, 1, 1);


        retranslateUi(OutputPage);

        QMetaObject::connectSlotsByName(OutputPage);
    } // setupUi

    void retranslateUi(QWidget *OutputPage)
    {
        OutputPage->setWindowTitle(QApplication::translate("OutputPage", "Form", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("OutputPage", "Project file name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("OutputPage", "Collection file name:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(OutputPage);
    } // retranslateUi

};

namespace Ui {
    class OutputPage: public Ui_OutputPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // OUTPUTPAGE_H
