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
** Form generated from reading ui file 'bookmarkdialog.ui'
**
** Created: Mon Jun 16 18:01:55 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef BOOKMARKDIALOG_H
#define BOOKMARKDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include "bookmarkwidget.h"

QT_BEGIN_NAMESPACE

class Ui_BookmarkDialog
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label;
    QLabel *label_2;
    QVBoxLayout *verticalLayout;
    QLineEdit *bookmarkEdit;
    QComboBox *bookmarkFolders;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *toolButton;
    QFrame *line;
    BookmarkWidget *bookmarkWidget;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *newFolderButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *BookmarkDialog)
    {
        if (BookmarkDialog->objectName().isEmpty())
            BookmarkDialog->setObjectName(QString::fromUtf8("BookmarkDialog"));
        BookmarkDialog->resize(450, 135);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(BookmarkDialog->sizePolicy().hasHeightForWidth());
        BookmarkDialog->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(BookmarkDialog);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label = new QLabel(BookmarkDialog);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout_2->addWidget(label);

        label_2 = new QLabel(BookmarkDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        verticalLayout_2->addWidget(label_2);


        horizontalLayout->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        bookmarkEdit = new QLineEdit(BookmarkDialog);
        bookmarkEdit->setObjectName(QString::fromUtf8("bookmarkEdit"));

        verticalLayout->addWidget(bookmarkEdit);

        bookmarkFolders = new QComboBox(BookmarkDialog);
        bookmarkFolders->setObjectName(QString::fromUtf8("bookmarkFolders"));

        verticalLayout->addWidget(bookmarkFolders);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        toolButton = new QToolButton(BookmarkDialog);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setMinimumSize(QSize(25, 20));

        horizontalLayout_3->addWidget(toolButton);

        line = new QFrame(BookmarkDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_3->addWidget(line);


        verticalLayout_3->addLayout(horizontalLayout_3);

        bookmarkWidget = new BookmarkWidget(BookmarkDialog);
        bookmarkWidget->setObjectName(QString::fromUtf8("bookmarkWidget"));
        bookmarkWidget->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Ignored);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(bookmarkWidget->sizePolicy().hasHeightForWidth());
        bookmarkWidget->setSizePolicy(sizePolicy1);

        verticalLayout_3->addWidget(bookmarkWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        newFolderButton = new QPushButton(BookmarkDialog);
        newFolderButton->setObjectName(QString::fromUtf8("newFolderButton"));

        horizontalLayout_4->addWidget(newFolderButton);

        buttonBox = new QDialogButtonBox(BookmarkDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout_4->addWidget(buttonBox);


        verticalLayout_3->addLayout(horizontalLayout_4);


        retranslateUi(BookmarkDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), BookmarkDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), BookmarkDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(BookmarkDialog);
    } // setupUi

    void retranslateUi(QDialog *BookmarkDialog)
    {
        BookmarkDialog->setWindowTitle(QApplication::translate("BookmarkDialog", "Add Bookmark", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("BookmarkDialog", "Bookmark:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("BookmarkDialog", "Add in Folder:", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("BookmarkDialog", "+", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = bookmarkWidget->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("BookmarkDialog", "1", 0, QApplication::UnicodeUTF8));
        newFolderButton->setText(QApplication::translate("BookmarkDialog", "New Folder", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(BookmarkDialog);
    } // retranslateUi

};

namespace Ui {
    class BookmarkDialog: public Ui_BookmarkDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BOOKMARKDIALOG_H
