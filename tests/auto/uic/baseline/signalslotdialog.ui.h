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
** Form generated from reading ui file 'signalslotdialog.ui'
**
** Created: Mon Jun 16 16:18:52 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef SIGNALSLOTDIALOG_H
#define SIGNALSLOTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SignalSlotDialogClass
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *slotGroupBox;
    QVBoxLayout *vboxLayout1;
    QListView *slotListView;
    QHBoxLayout *hboxLayout;
    QToolButton *addSlotButton;
    QToolButton *removeSlotButton;
    QSpacerItem *spacerItem;
    QGroupBox *signalGroupBox;
    QVBoxLayout *vboxLayout2;
    QListView *signalListView;
    QHBoxLayout *hboxLayout1;
    QToolButton *addSignalButton;
    QToolButton *removeSignalButton;
    QSpacerItem *spacerItem1;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SignalSlotDialogClass)
    {
        if (SignalSlotDialogClass->objectName().isEmpty())
            SignalSlotDialogClass->setObjectName(QString::fromUtf8("SignalSlotDialogClass"));
        SignalSlotDialogClass->resize(617, 535);
        vboxLayout = new QVBoxLayout(SignalSlotDialogClass);
        vboxLayout->setSpacing(6);
        vboxLayout->setMargin(11);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        slotGroupBox = new QGroupBox(SignalSlotDialogClass);
        slotGroupBox->setObjectName(QString::fromUtf8("slotGroupBox"));
        vboxLayout1 = new QVBoxLayout(slotGroupBox);
        vboxLayout1->setSpacing(6);
        vboxLayout1->setMargin(11);
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        slotListView = new QListView(slotGroupBox);
        slotListView->setObjectName(QString::fromUtf8("slotListView"));

        vboxLayout1->addWidget(slotListView);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setSpacing(6);
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        addSlotButton = new QToolButton(slotGroupBox);
        addSlotButton->setObjectName(QString::fromUtf8("addSlotButton"));

        hboxLayout->addWidget(addSlotButton);

        removeSlotButton = new QToolButton(slotGroupBox);
        removeSlotButton->setObjectName(QString::fromUtf8("removeSlotButton"));

        hboxLayout->addWidget(removeSlotButton);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        vboxLayout1->addLayout(hboxLayout);


        vboxLayout->addWidget(slotGroupBox);

        signalGroupBox = new QGroupBox(SignalSlotDialogClass);
        signalGroupBox->setObjectName(QString::fromUtf8("signalGroupBox"));
        vboxLayout2 = new QVBoxLayout(signalGroupBox);
        vboxLayout2->setSpacing(6);
        vboxLayout2->setMargin(11);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        signalListView = new QListView(signalGroupBox);
        signalListView->setObjectName(QString::fromUtf8("signalListView"));

        vboxLayout2->addWidget(signalListView);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setSpacing(6);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        addSignalButton = new QToolButton(signalGroupBox);
        addSignalButton->setObjectName(QString::fromUtf8("addSignalButton"));

        hboxLayout1->addWidget(addSignalButton);

        removeSignalButton = new QToolButton(signalGroupBox);
        removeSignalButton->setObjectName(QString::fromUtf8("removeSignalButton"));

        hboxLayout1->addWidget(removeSignalButton);

        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);


        vboxLayout2->addLayout(hboxLayout1);


        vboxLayout->addWidget(signalGroupBox);

        buttonBox = new QDialogButtonBox(SignalSlotDialogClass);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);


        retranslateUi(SignalSlotDialogClass);

        QMetaObject::connectSlotsByName(SignalSlotDialogClass);
    } // setupUi

    void retranslateUi(QDialog *SignalSlotDialogClass)
    {
        SignalSlotDialogClass->setWindowTitle(QApplication::translate("SignalSlotDialogClass", "Signals and slots", 0, QApplication::UnicodeUTF8));
        slotGroupBox->setTitle(QApplication::translate("SignalSlotDialogClass", "Slots", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addSlotButton->setToolTip(QApplication::translate("SignalSlotDialogClass", "Add", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addSlotButton->setText(QApplication::translate("SignalSlotDialogClass", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        removeSlotButton->setToolTip(QApplication::translate("SignalSlotDialogClass", "Delete", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        removeSlotButton->setText(QApplication::translate("SignalSlotDialogClass", "...", 0, QApplication::UnicodeUTF8));
        signalGroupBox->setTitle(QApplication::translate("SignalSlotDialogClass", "Signals", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        addSignalButton->setToolTip(QApplication::translate("SignalSlotDialogClass", "Add", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        addSignalButton->setText(QApplication::translate("SignalSlotDialogClass", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        removeSignalButton->setToolTip(QApplication::translate("SignalSlotDialogClass", "Delete", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        removeSignalButton->setText(QApplication::translate("SignalSlotDialogClass", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SignalSlotDialogClass);
    } // retranslateUi

};

namespace Ui {
    class SignalSlotDialogClass: public Ui_SignalSlotDialogClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SIGNALSLOTDIALOG_H
