/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Nokia Corporation and/or its subsidiary(-ies)
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://qt.nokia.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
*********************************************************************
*/

/********************************************************************************
** Form generated from reading UI file 'orderdialog.ui'
**
** Created: Mon Jun 16 17:55:54 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef ORDERDIALOG_H
#define ORDERDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_OrderDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QHBoxLayout *hboxLayout;
    QListWidget *pageList;
    QVBoxLayout *vboxLayout1;
    QToolButton *upButton;
    QToolButton *downButton;
    QSpacerItem *spacerItem;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__OrderDialog)
    {
        if (qdesigner_internal__OrderDialog->objectName().isEmpty())
            qdesigner_internal__OrderDialog->setObjectName(QString::fromUtf8("qdesigner_internal__OrderDialog"));
        qdesigner_internal__OrderDialog->resize(467, 310);
        vboxLayout = new QVBoxLayout(qdesigner_internal__OrderDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(qdesigner_internal__OrderDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        hboxLayout = new QHBoxLayout(groupBox);
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        hboxLayout->setContentsMargins(9, 9, 9, 9);
        pageList = new QListWidget(groupBox);
        pageList->setObjectName(QString::fromUtf8("pageList"));
        pageList->setMinimumSize(QSize(344, 0));
        pageList->setDragDropMode(QAbstractItemView::InternalMove);
        pageList->setSelectionMode(QAbstractItemView::ContiguousSelection);
        pageList->setMovement(QListView::Snap);

        hboxLayout->addWidget(pageList);

        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setSpacing(6);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
        upButton = new QToolButton(groupBox);
        upButton->setObjectName(QString::fromUtf8("upButton"));

        vboxLayout1->addWidget(upButton);

        downButton = new QToolButton(groupBox);
        downButton->setObjectName(QString::fromUtf8("downButton"));

        vboxLayout1->addWidget(downButton);

        spacerItem = new QSpacerItem(20, 99, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout1->addItem(spacerItem);


        hboxLayout->addLayout(vboxLayout1);


        vboxLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(qdesigner_internal__OrderDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::Reset);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(qdesigner_internal__OrderDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__OrderDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__OrderDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__OrderDialog);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__OrderDialog)
    {
        qdesigner_internal__OrderDialog->setWindowTitle(QApplication::translate("qdesigner_internal::OrderDialog", "Change Page Order", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("qdesigner_internal::OrderDialog", "Page Order", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        upButton->setToolTip(QApplication::translate("qdesigner_internal::OrderDialog", "Move page up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        downButton->setToolTip(QApplication::translate("qdesigner_internal::OrderDialog", "Move page down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(qdesigner_internal__OrderDialog);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class OrderDialog: public Ui_OrderDialog {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // ORDERDIALOG_H
