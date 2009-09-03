/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/********************************************************************************
** Form generated from reading UI file 'embeddeddialog.ui'
**
** Created: Tue Aug 18 19:03:31 2009
**      by: Qt User Interface Compiler version 4.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef EMBEDDEDDIALOG_H
#define EMBEDDEDDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFontComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>

QT_BEGIN_NAMESPACE

class Ui_embeddedDialog
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    QComboBox *layoutDirection;
    QLabel *label_2;
    QFontComboBox *fontComboBox;
    QLabel *label_3;
    QComboBox *style;
    QLabel *label_4;
    QSlider *spacing;

    void setupUi(QDialog *embeddedDialog)
    {
        if (embeddedDialog->objectName().isEmpty())
            embeddedDialog->setObjectName(QString::fromUtf8("embeddedDialog"));
        embeddedDialog->resize(407, 134);
        formLayout = new QFormLayout(embeddedDialog);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label = new QLabel(embeddedDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        layoutDirection = new QComboBox(embeddedDialog);
        layoutDirection->setObjectName(QString::fromUtf8("layoutDirection"));

        formLayout->setWidget(0, QFormLayout::FieldRole, layoutDirection);

        label_2 = new QLabel(embeddedDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_2);

        fontComboBox = new QFontComboBox(embeddedDialog);
        fontComboBox->setObjectName(QString::fromUtf8("fontComboBox"));

        formLayout->setWidget(1, QFormLayout::FieldRole, fontComboBox);

        label_3 = new QLabel(embeddedDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_3);

        style = new QComboBox(embeddedDialog);
        style->setObjectName(QString::fromUtf8("style"));

        formLayout->setWidget(2, QFormLayout::FieldRole, style);

        label_4 = new QLabel(embeddedDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_4);

        spacing = new QSlider(embeddedDialog);
        spacing->setObjectName(QString::fromUtf8("spacing"));
        spacing->setOrientation(Qt::Horizontal);

        formLayout->setWidget(3, QFormLayout::FieldRole, spacing);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(layoutDirection);
        label_2->setBuddy(fontComboBox);
        label_3->setBuddy(style);
        label_4->setBuddy(spacing);
#endif // QT_NO_SHORTCUT

        retranslateUi(embeddedDialog);

        QMetaObject::connectSlotsByName(embeddedDialog);
    } // setupUi

    void retranslateUi(QDialog *embeddedDialog)
    {
        embeddedDialog->setWindowTitle(QApplication::translate("embeddedDialog", "Embedded Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("embeddedDialog", "Layout Direction:", 0, QApplication::UnicodeUTF8));
        layoutDirection->clear();
        layoutDirection->insertItems(0, QStringList()
         << QApplication::translate("embeddedDialog", "Left to Right", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("embeddedDialog", "Right to Left", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("embeddedDialog", "Select Font:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("embeddedDialog", "Style:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("embeddedDialog", "Layout spacing:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(embeddedDialog);
    } // retranslateUi

};

namespace Ui {
    class embeddedDialog: public Ui_embeddedDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // EMBEDDEDDIALOG_H
