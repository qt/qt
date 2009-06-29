/*
*********************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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
** Form generated from reading ui file 'batchtranslation.ui'
**
** Created: Thu Jul 10 09:47:34 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef BATCHTRANSLATION_H
#define BATCHTRANSLATION_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_databaseTranslationDialog
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QVBoxLayout *vboxLayout1;
    QCheckBox *ckOnlyUntranslated;
    QCheckBox *ckMarkFinished;
    QGroupBox *groupBox_2;
    QVBoxLayout *vboxLayout2;
    QHBoxLayout *hboxLayout;
    QListView *phrasebookList;
    QVBoxLayout *vboxLayout3;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QSpacerItem *spacerItem;
    QLabel *label;
    QHBoxLayout *hboxLayout1;
    QSpacerItem *spacerItem1;
    QPushButton *runButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *databaseTranslationDialog)
    {
        if (databaseTranslationDialog->objectName().isEmpty())
            databaseTranslationDialog->setObjectName(QString::fromUtf8("databaseTranslationDialog"));
        databaseTranslationDialog->resize(425, 370);
        vboxLayout = new QVBoxLayout(databaseTranslationDialog);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(databaseTranslationDialog);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(4));
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        vboxLayout1 = new QVBoxLayout(groupBox);
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        ckOnlyUntranslated = new QCheckBox(groupBox);
        ckOnlyUntranslated->setObjectName(QString::fromUtf8("ckOnlyUntranslated"));
        ckOnlyUntranslated->setChecked(true);

        vboxLayout1->addWidget(ckOnlyUntranslated);

        ckMarkFinished = new QCheckBox(groupBox);
        ckMarkFinished->setObjectName(QString::fromUtf8("ckMarkFinished"));
        ckMarkFinished->setChecked(true);

        vboxLayout1->addWidget(ckMarkFinished);


        vboxLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(databaseTranslationDialog);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(1));
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        vboxLayout2 = new QVBoxLayout(groupBox_2);
#ifndef Q_OS_MAC
        vboxLayout2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout2->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setContentsMargins(0, 0, 0, 0);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        phrasebookList = new QListView(groupBox_2);
        phrasebookList->setObjectName(QString::fromUtf8("phrasebookList"));
        phrasebookList->setUniformItemSizes(true);

        hboxLayout->addWidget(phrasebookList);

        vboxLayout3 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout3->setSpacing(6);
#endif
        vboxLayout3->setContentsMargins(0, 0, 0, 0);
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        moveUpButton = new QPushButton(groupBox_2);
        moveUpButton->setObjectName(QString::fromUtf8("moveUpButton"));

        vboxLayout3->addWidget(moveUpButton);

        moveDownButton = new QPushButton(groupBox_2);
        moveDownButton->setObjectName(QString::fromUtf8("moveDownButton"));

        vboxLayout3->addWidget(moveDownButton);

        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout3->addItem(spacerItem);


        hboxLayout->addLayout(vboxLayout3);


        vboxLayout2->addLayout(hboxLayout);

        label = new QLabel(groupBox_2);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        vboxLayout2->addWidget(label);


        vboxLayout->addWidget(groupBox_2);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setContentsMargins(0, 0, 0, 0);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout1->addItem(spacerItem1);

        runButton = new QPushButton(databaseTranslationDialog);
        runButton->setObjectName(QString::fromUtf8("runButton"));

        hboxLayout1->addWidget(runButton);

        cancelButton = new QPushButton(databaseTranslationDialog);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));

        hboxLayout1->addWidget(cancelButton);


        vboxLayout->addLayout(hboxLayout1);


        retranslateUi(databaseTranslationDialog);

        QMetaObject::connectSlotsByName(databaseTranslationDialog);
    } // setupUi

    void retranslateUi(QDialog *databaseTranslationDialog)
    {
        databaseTranslationDialog->setWindowTitle(QApplication::translate("databaseTranslationDialog", "Qt Linguist - Batch Translation", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("databaseTranslationDialog", "Options", 0, QApplication::UnicodeUTF8));
        ckOnlyUntranslated->setText(QApplication::translate("databaseTranslationDialog", "Only translate entries with no translation", 0, QApplication::UnicodeUTF8));
        ckMarkFinished->setText(QApplication::translate("databaseTranslationDialog", "Set translated entries to finished", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("databaseTranslationDialog", "Phrase book preference", 0, QApplication::UnicodeUTF8));
        moveUpButton->setText(QApplication::translate("databaseTranslationDialog", "Move up", 0, QApplication::UnicodeUTF8));
        moveDownButton->setText(QApplication::translate("databaseTranslationDialog", "Move down", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("databaseTranslationDialog", "The batch translator will search through the selected phrasebooks in the order given above.", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("databaseTranslationDialog", "&Run", 0, QApplication::UnicodeUTF8));
        cancelButton->setText(QApplication::translate("databaseTranslationDialog", "&Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(databaseTranslationDialog);
    } // retranslateUi

};

namespace Ui {
    class databaseTranslationDialog: public Ui_databaseTranslationDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // BATCHTRANSLATION_H
