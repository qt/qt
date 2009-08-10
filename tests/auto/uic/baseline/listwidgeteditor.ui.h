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
** http://www.trolltech.com/products/qt/gplexception/ and in the file
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
** Form generated from reading UI file 'listwidgeteditor.ui'
**
** Created: Mon Jun 16 17:54:30 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef LISTWIDGETEDITOR_H
#define LISTWIDGETEDITOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include "iconselector_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_ListWidgetEditor
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QListWidget *listWidget;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *newItemButton;
    QToolButton *deleteItemButton;
    QSpacerItem *spacerItem;
    QToolButton *moveItemUpButton;
    QToolButton *moveItemDownButton;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    qdesigner_internal::IconSelector *itemIconSelector;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__ListWidgetEditor)
    {
        if (qdesigner_internal__ListWidgetEditor->objectName().isEmpty())
            qdesigner_internal__ListWidgetEditor->setObjectName(QString::fromUtf8("qdesigner_internal__ListWidgetEditor"));
        qdesigner_internal__ListWidgetEditor->resize(223, 245);
        vboxLayout = new QVBoxLayout(qdesigner_internal__ListWidgetEditor);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(qdesigner_internal__ListWidgetEditor);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        listWidget = new QListWidget(groupBox);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        gridLayout->addWidget(listWidget, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        newItemButton = new QToolButton(groupBox);
        newItemButton->setObjectName(QString::fromUtf8("newItemButton"));

        horizontalLayout_2->addWidget(newItemButton);

        deleteItemButton = new QToolButton(groupBox);
        deleteItemButton->setObjectName(QString::fromUtf8("deleteItemButton"));

        horizontalLayout_2->addWidget(deleteItemButton);

        spacerItem = new QSpacerItem(16, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(spacerItem);

        moveItemUpButton = new QToolButton(groupBox);
        moveItemUpButton->setObjectName(QString::fromUtf8("moveItemUpButton"));

        horizontalLayout_2->addWidget(moveItemUpButton);

        moveItemDownButton = new QToolButton(groupBox);
        moveItemDownButton->setObjectName(QString::fromUtf8("moveItemDownButton"));

        horizontalLayout_2->addWidget(moveItemDownButton);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        itemIconSelector = new qdesigner_internal::IconSelector(groupBox);
        itemIconSelector->setObjectName(QString::fromUtf8("itemIconSelector"));

        horizontalLayout->addWidget(itemIconSelector);

        horizontalSpacer = new QSpacerItem(108, 21, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);


        vboxLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(qdesigner_internal__ListWidgetEditor);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);

        buttonBox->raise();
        groupBox->raise();
        QWidget::setTabOrder(listWidget, newItemButton);
        QWidget::setTabOrder(newItemButton, deleteItemButton);
        QWidget::setTabOrder(deleteItemButton, moveItemUpButton);
        QWidget::setTabOrder(moveItemUpButton, moveItemDownButton);

        retranslateUi(qdesigner_internal__ListWidgetEditor);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__ListWidgetEditor, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__ListWidgetEditor, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__ListWidgetEditor);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__ListWidgetEditor)
    {
        qdesigner_internal__ListWidgetEditor->setWindowTitle(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Dialog", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Items List", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Items List", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        newItemButton->setToolTip(QApplication::translate("qdesigner_internal::ListWidgetEditor", "New Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newItemButton->setText(QApplication::translate("qdesigner_internal::ListWidgetEditor", "&New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        deleteItemButton->setToolTip(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Delete Item", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        deleteItemButton->setText(QApplication::translate("qdesigner_internal::ListWidgetEditor", "&Delete", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemUpButton->setToolTip(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Move Item Up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemUpButton->setText(QApplication::translate("qdesigner_internal::ListWidgetEditor", "U", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        moveItemDownButton->setToolTip(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Move Item Down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        moveItemDownButton->setText(QApplication::translate("qdesigner_internal::ListWidgetEditor", "D", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("qdesigner_internal::ListWidgetEditor", "Icon", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__ListWidgetEditor);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class ListWidgetEditor: public Ui_ListWidgetEditor {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // LISTWIDGETEDITOR_H
