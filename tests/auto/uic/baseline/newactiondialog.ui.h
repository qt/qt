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
** Form generated from reading UI file 'newactiondialog.ui'
**
** Created: Mon Sep 1 09:31:03 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef NEWACTIONDIALOG_H
#define NEWACTIONDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "iconselector_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_NewActionDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *editActionText;
    QLabel *label_3;
    QLineEdit *editObjectName;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout;
    qdesigner_internal::IconSelector *iconSelector;
    QSpacerItem *spacerItem;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        if (qdesigner_internal__NewActionDialog->objectName().isEmpty())
            qdesigner_internal__NewActionDialog->setObjectName(QString::fromUtf8("qdesigner_internal__NewActionDialog"));
        qdesigner_internal__NewActionDialog->resize(363, 156);
        verticalLayout = new QVBoxLayout(qdesigner_internal__NewActionDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(qdesigner_internal__NewActionDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        editActionText = new QLineEdit(qdesigner_internal__NewActionDialog);
        editActionText->setObjectName(QString::fromUtf8("editActionText"));
        editActionText->setMinimumSize(QSize(255, 0));

        formLayout->setWidget(0, QFormLayout::FieldRole, editActionText);

        label_3 = new QLabel(qdesigner_internal__NewActionDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        editObjectName = new QLineEdit(qdesigner_internal__NewActionDialog);
        editObjectName->setObjectName(QString::fromUtf8("editObjectName"));

        formLayout->setWidget(1, QFormLayout::FieldRole, editObjectName);

        label_2 = new QLabel(qdesigner_internal__NewActionDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        iconSelector = new qdesigner_internal::IconSelector(qdesigner_internal__NewActionDialog);
        iconSelector->setObjectName(QString::fromUtf8("iconSelector"));

        horizontalLayout->addWidget(iconSelector);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(spacerItem);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(qdesigner_internal__NewActionDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        buttonBox = new QDialogButtonBox(qdesigner_internal__NewActionDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(editActionText);
        label_3->setBuddy(editObjectName);
        label_2->setBuddy(iconSelector);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(editActionText, editObjectName);

        retranslateUi(qdesigner_internal__NewActionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), qdesigner_internal__NewActionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), qdesigner_internal__NewActionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(qdesigner_internal__NewActionDialog);
    } // setupUi

    void retranslateUi(QDialog *qdesigner_internal__NewActionDialog)
    {
        qdesigner_internal__NewActionDialog->setWindowTitle(QApplication::translate("qdesigner_internal::NewActionDialog", "New Action...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Text:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "Object &name:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("qdesigner_internal::NewActionDialog", "&Icon:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__NewActionDialog);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class NewActionDialog: public Ui_NewActionDialog {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // NEWACTIONDIALOG_H
