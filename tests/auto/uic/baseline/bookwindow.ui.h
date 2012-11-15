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
** Form generated from reading ui file 'bookwindow.ui'
**
** Created: Thu Jul 10 09:47:34 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef BOOKWINDOW_H
#define BOOKWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QSpinBox>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BookWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout1;
    QTableView *bookTable;
    QGroupBox *groupBox_2;
    QFormLayout *formLayout;
    QLabel *label_5;
    QLineEdit *titleEdit;
    QLabel *label_2_2_2_2;
    QComboBox *authorEdit;
    QLabel *label_3;
    QComboBox *genreEdit;
    QLabel *label_4;
    QSpinBox *yearEdit;
    QLabel *label;
    QSpinBox *ratingEdit;

    void setupUi(QMainWindow *BookWindow)
    {
        if (BookWindow->objectName().isEmpty())
            BookWindow->setObjectName(QString::fromUtf8("BookWindow"));
        BookWindow->resize(601, 420);
        centralWidget = new QWidget(BookWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        vboxLayout = new QVBoxLayout(centralWidget);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        vboxLayout1 = new QVBoxLayout(groupBox);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setMargin(9);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        bookTable = new QTableView(groupBox);
        bookTable->setObjectName(QString::fromUtf8("bookTable"));
        bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        vboxLayout1->addWidget(bookTable);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        formLayout = new QFormLayout(groupBox_2);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_5);

        titleEdit = new QLineEdit(groupBox_2);
        titleEdit->setObjectName(QString::fromUtf8("titleEdit"));
        titleEdit->setEnabled(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, titleEdit);

        label_2_2_2_2 = new QLabel(groupBox_2);
        label_2_2_2_2->setObjectName(QString::fromUtf8("label_2_2_2_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2_2_2_2);

        authorEdit = new QComboBox(groupBox_2);
        authorEdit->setObjectName(QString::fromUtf8("authorEdit"));
        authorEdit->setEnabled(true);

        formLayout->setWidget(1, QFormLayout::FieldRole, authorEdit);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        genreEdit = new QComboBox(groupBox_2);
        genreEdit->setObjectName(QString::fromUtf8("genreEdit"));
        genreEdit->setEnabled(true);

        formLayout->setWidget(2, QFormLayout::FieldRole, genreEdit);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        yearEdit = new QSpinBox(groupBox_2);
        yearEdit->setObjectName(QString::fromUtf8("yearEdit"));
        yearEdit->setEnabled(true);
        yearEdit->setMaximum(2100);
        yearEdit->setMinimum(-1000);

        formLayout->setWidget(3, QFormLayout::FieldRole, yearEdit);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(4, QFormLayout::LabelRole, label);

        ratingEdit = new QSpinBox(groupBox_2);
        ratingEdit->setObjectName(QString::fromUtf8("ratingEdit"));
        ratingEdit->setMaximum(5);

        formLayout->setWidget(4, QFormLayout::FieldRole, ratingEdit);


        vboxLayout1->addWidget(groupBox_2);


        vboxLayout->addWidget(groupBox);

        BookWindow->setCentralWidget(centralWidget);
        QWidget::setTabOrder(bookTable, titleEdit);
        QWidget::setTabOrder(titleEdit, authorEdit);
        QWidget::setTabOrder(authorEdit, genreEdit);
        QWidget::setTabOrder(genreEdit, yearEdit);

        retranslateUi(BookWindow);

        QMetaObject::connectSlotsByName(BookWindow);
    } // setupUi

    void retranslateUi(QMainWindow *BookWindow)
    {
        BookWindow->setWindowTitle(QApplication::translate("BookWindow", "Books", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("BookWindow", "Books", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("BookWindow", "Details", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("BookWindow", "<b>Title:</b>", 0, QApplication::UnicodeUTF8));
        label_2_2_2_2->setText(QApplication::translate("BookWindow", "<b>Author: </b>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("BookWindow", "<b>Genre:</b>", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("BookWindow", "<b>Year:</b>", 0, QApplication::UnicodeUTF8));
        yearEdit->setPrefix(QString());
        label->setText(QApplication::translate("BookWindow", "<b>Rating:</b>", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(BookWindow);
    } // retranslateUi

};

namespace Ui {
    class BookWindow: public Ui_BookWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BOOKWINDOW_H
