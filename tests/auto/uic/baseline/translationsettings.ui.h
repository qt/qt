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
** Form generated from reading ui file 'translationsettings.ui'
**
** Created: Mon Sep 1 09:31:03 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef TRANSLATIONSETTINGS_H
#define TRANSLATIONSETTINGS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_TranslationSettings
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QComboBox *cbLanguageList;
    QLabel *label;
    QComboBox *cbCountryList;
    QLabel *lblCountry;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TranslationSettings)
    {
        if (TranslationSettings->objectName().isEmpty())
            TranslationSettings->setObjectName(QString::fromUtf8("TranslationSettings"));
        TranslationSettings->resize(346, 125);
        vboxLayout = new QVBoxLayout(TranslationSettings);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setMargin(9);
#endif
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        groupBox = new QGroupBox(TranslationSettings);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cbLanguageList = new QComboBox(groupBox);
        cbLanguageList->setObjectName(QString::fromUtf8("cbLanguageList"));

        gridLayout->addWidget(cbLanguageList, 0, 1, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        cbCountryList = new QComboBox(groupBox);
        cbCountryList->setObjectName(QString::fromUtf8("cbCountryList"));

        gridLayout->addWidget(cbCountryList, 1, 1, 1, 1);

        lblCountry = new QLabel(groupBox);
        lblCountry->setObjectName(QString::fromUtf8("lblCountry"));

        gridLayout->addWidget(lblCountry, 1, 0, 1, 1);


        vboxLayout->addWidget(groupBox);

        buttonBox = new QDialogButtonBox(TranslationSettings);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        vboxLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(cbLanguageList);
#endif // QT_NO_SHORTCUT

        retranslateUi(TranslationSettings);
        QObject::connect(buttonBox, SIGNAL(accepted()), TranslationSettings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TranslationSettings, SLOT(reject()));

        QMetaObject::connectSlotsByName(TranslationSettings);
    } // setupUi

    void retranslateUi(QDialog *TranslationSettings)
    {
        TranslationSettings->setWindowTitle(QApplication::translate("TranslationSettings", "Qt Linguist - Translation file settings", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("TranslationSettings", "Target language", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TranslationSettings", "Language", 0, QApplication::UnicodeUTF8));
        lblCountry->setText(QApplication::translate("TranslationSettings", "Country/Region", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(TranslationSettings);
    } // retranslateUi

};

namespace Ui {
    class TranslationSettings: public Ui_TranslationSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TRANSLATIONSETTINGS_H
