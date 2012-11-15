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
** Form generated from reading ui file 'qttoolbardialog.ui'
**
** Created: Mon Jun 16 17:42:37 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef QTTOOLBARDIALOG_H
#define QTTOOLBARDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QtToolBarDialog
{
public:
    QGridLayout *gridLayout;
    QTreeWidget *actionTree;
    QLabel *label;
    QHBoxLayout *hboxLayout;
    QLabel *label_2;
    QToolButton *newButton;
    QToolButton *removeButton;
    QToolButton *renameButton;
    QVBoxLayout *vboxLayout;
    QToolButton *upButton;
    QToolButton *leftButton;
    QToolButton *rightButton;
    QToolButton *downButton;
    QSpacerItem *spacerItem;
    QListWidget *currentToolBarList;
    QLabel *label_3;
    QListWidget *toolBarList;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *QtToolBarDialog)
    {
        if (QtToolBarDialog->objectName().isEmpty())
            QtToolBarDialog->setObjectName(QString::fromUtf8("QtToolBarDialog"));
        QtToolBarDialog->resize(583, 508);
        gridLayout = new QGridLayout(QtToolBarDialog);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
        gridLayout->setMargin(8);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        actionTree = new QTreeWidget(QtToolBarDialog);
        actionTree->setObjectName(QString::fromUtf8("actionTree"));

        gridLayout->addWidget(actionTree, 1, 0, 3, 1);

        label = new QLabel(QtToolBarDialog);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setMargin(0);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        label_2 = new QLabel(QtToolBarDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        hboxLayout->addWidget(label_2);

        newButton = new QToolButton(QtToolBarDialog);
        newButton->setObjectName(QString::fromUtf8("newButton"));

        hboxLayout->addWidget(newButton);

        removeButton = new QToolButton(QtToolBarDialog);
        removeButton->setObjectName(QString::fromUtf8("removeButton"));

        hboxLayout->addWidget(removeButton);

        renameButton = new QToolButton(QtToolBarDialog);
        renameButton->setObjectName(QString::fromUtf8("renameButton"));

        hboxLayout->addWidget(renameButton);


        gridLayout->addLayout(hboxLayout, 0, 1, 1, 2);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        upButton = new QToolButton(QtToolBarDialog);
        upButton->setObjectName(QString::fromUtf8("upButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(upButton->sizePolicy().hasHeightForWidth());
        upButton->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(upButton);

        leftButton = new QToolButton(QtToolBarDialog);
        leftButton->setObjectName(QString::fromUtf8("leftButton"));
        sizePolicy.setHeightForWidth(leftButton->sizePolicy().hasHeightForWidth());
        leftButton->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(leftButton);

        rightButton = new QToolButton(QtToolBarDialog);
        rightButton->setObjectName(QString::fromUtf8("rightButton"));
        sizePolicy.setHeightForWidth(rightButton->sizePolicy().hasHeightForWidth());
        rightButton->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(rightButton);

        downButton = new QToolButton(QtToolBarDialog);
        downButton->setObjectName(QString::fromUtf8("downButton"));
        sizePolicy.setHeightForWidth(downButton->sizePolicy().hasHeightForWidth());
        downButton->setSizePolicy(sizePolicy);

        vboxLayout->addWidget(downButton);

        spacerItem = new QSpacerItem(29, 16, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);


        gridLayout->addLayout(vboxLayout, 3, 1, 1, 1);

        currentToolBarList = new QListWidget(QtToolBarDialog);
        currentToolBarList->setObjectName(QString::fromUtf8("currentToolBarList"));

        gridLayout->addWidget(currentToolBarList, 3, 2, 1, 1);

        label_3 = new QLabel(QtToolBarDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 1, 1, 2);

        toolBarList = new QListWidget(QtToolBarDialog);
        toolBarList->setObjectName(QString::fromUtf8("toolBarList"));

        gridLayout->addWidget(toolBarList, 1, 1, 1, 2);

        buttonBox = new QDialogButtonBox(QtToolBarDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Ok|QDialogButtonBox::RestoreDefaults);

        gridLayout->addWidget(buttonBox, 5, 0, 1, 3);

        QWidget::setTabOrder(newButton, removeButton);
        QWidget::setTabOrder(removeButton, renameButton);
        QWidget::setTabOrder(renameButton, toolBarList);
        QWidget::setTabOrder(toolBarList, upButton);
        QWidget::setTabOrder(upButton, leftButton);
        QWidget::setTabOrder(leftButton, rightButton);
        QWidget::setTabOrder(rightButton, downButton);
        QWidget::setTabOrder(downButton, currentToolBarList);

        retranslateUi(QtToolBarDialog);

        QMetaObject::connectSlotsByName(QtToolBarDialog);
    } // setupUi

    void retranslateUi(QDialog *QtToolBarDialog)
    {
        QtToolBarDialog->setWindowTitle(QApplication::translate("QtToolBarDialog", "Customize Toolbars", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = actionTree->headerItem();
        ___qtreewidgetitem->setText(0, QApplication::translate("QtToolBarDialog", "1", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("QtToolBarDialog", "Actions", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("QtToolBarDialog", "Toolbars", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        newButton->setToolTip(QApplication::translate("QtToolBarDialog", "Add new toolbar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        newButton->setText(QApplication::translate("QtToolBarDialog", "New", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        removeButton->setToolTip(QApplication::translate("QtToolBarDialog", "Remove selected toolbar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        removeButton->setText(QApplication::translate("QtToolBarDialog", "Remove", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        renameButton->setToolTip(QApplication::translate("QtToolBarDialog", "Rename toolbar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        renameButton->setText(QApplication::translate("QtToolBarDialog", "Rename", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        upButton->setToolTip(QApplication::translate("QtToolBarDialog", "Move action up", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        upButton->setText(QApplication::translate("QtToolBarDialog", "Up", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        leftButton->setToolTip(QApplication::translate("QtToolBarDialog", "Remove action from toolbar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        leftButton->setText(QApplication::translate("QtToolBarDialog", "<-", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        rightButton->setToolTip(QApplication::translate("QtToolBarDialog", "Add action to toolbar", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        rightButton->setText(QApplication::translate("QtToolBarDialog", "->", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        downButton->setToolTip(QApplication::translate("QtToolBarDialog", "Move action down", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        downButton->setText(QApplication::translate("QtToolBarDialog", "Down", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("QtToolBarDialog", "Current Toolbar Actions", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QtToolBarDialog);
    } // retranslateUi

};

namespace Ui {
    class QtToolBarDialog: public Ui_QtToolBarDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // QTTOOLBARDIALOG_H
