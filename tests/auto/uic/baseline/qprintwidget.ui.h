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
** Form generated from reading ui file 'qprintwidget.ui'
**
** Created: Mon Sep 1 09:31:03 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef QPRINTWIDGET_H
#define QPRINTWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QPrintWidget
{
public:
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *printerGroup;
    QGridLayout *gridLayout;
    QLabel *label;
    QComboBox *printers;
    QPushButton *properties;
    QLabel *label_2;
    QLabel *location;
    QCheckBox *preview;
    QLabel *label_3;
    QLabel *type;
    QLabel *lOutput;
    QHBoxLayout *horizontalLayout;
    QLineEdit *filename;
    QToolButton *fileBrowser;

    void setupUi(QWidget *QPrintWidget)
    {
        if (QPrintWidget->objectName().isEmpty())
            QPrintWidget->setObjectName(QString::fromUtf8("QPrintWidget"));
        QPrintWidget->resize(443, 175);
        horizontalLayout_2 = new QHBoxLayout(QPrintWidget);
        horizontalLayout_2->setMargin(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        printerGroup = new QGroupBox(QPrintWidget);
        printerGroup->setObjectName(QString::fromUtf8("printerGroup"));
        gridLayout = new QGridLayout(printerGroup);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(printerGroup);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        printers = new QComboBox(printerGroup);
        printers->setObjectName(QString::fromUtf8("printers"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(printers->sizePolicy().hasHeightForWidth());
        printers->setSizePolicy(sizePolicy);

        gridLayout->addWidget(printers, 0, 1, 1, 1);

        properties = new QPushButton(printerGroup);
        properties->setObjectName(QString::fromUtf8("properties"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(1);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(properties->sizePolicy().hasHeightForWidth());
        properties->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(properties, 0, 2, 1, 1);

        label_2 = new QLabel(printerGroup);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        location = new QLabel(printerGroup);
        location->setObjectName(QString::fromUtf8("location"));

        gridLayout->addWidget(location, 1, 1, 1, 1);

        preview = new QCheckBox(printerGroup);
        preview->setObjectName(QString::fromUtf8("preview"));

        gridLayout->addWidget(preview, 1, 2, 1, 1);

        label_3 = new QLabel(printerGroup);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        type = new QLabel(printerGroup);
        type->setObjectName(QString::fromUtf8("type"));

        gridLayout->addWidget(type, 2, 1, 1, 1);

        lOutput = new QLabel(printerGroup);
        lOutput->setObjectName(QString::fromUtf8("lOutput"));

        gridLayout->addWidget(lOutput, 3, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        filename = new QLineEdit(printerGroup);
        filename->setObjectName(QString::fromUtf8("filename"));

        horizontalLayout->addWidget(filename);

        fileBrowser = new QToolButton(printerGroup);
        fileBrowser->setObjectName(QString::fromUtf8("fileBrowser"));

        horizontalLayout->addWidget(fileBrowser);


        gridLayout->addLayout(horizontalLayout, 3, 1, 1, 2);


        horizontalLayout_2->addWidget(printerGroup);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(printers);
        lOutput->setBuddy(filename);
#endif // QT_NO_SHORTCUT

        retranslateUi(QPrintWidget);

        QMetaObject::connectSlotsByName(QPrintWidget);
    } // setupUi

    void retranslateUi(QWidget *QPrintWidget)
    {
        QPrintWidget->setWindowTitle(QApplication::translate("QPrintWidget", "Form", 0, QApplication::UnicodeUTF8));
        printerGroup->setTitle(QApplication::translate("QPrintWidget", "Printer", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("QPrintWidget", "&Name:", 0, QApplication::UnicodeUTF8));
        properties->setText(QApplication::translate("QPrintWidget", "P&roperties", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("QPrintWidget", "Location:", 0, QApplication::UnicodeUTF8));
        preview->setText(QApplication::translate("QPrintWidget", "Preview", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("QPrintWidget", "Type:", 0, QApplication::UnicodeUTF8));
        lOutput->setText(QApplication::translate("QPrintWidget", "Output &file:", 0, QApplication::UnicodeUTF8));
        fileBrowser->setText(QApplication::translate("QPrintWidget", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(QPrintWidget);
    } // retranslateUi

};

namespace Ui {
    class QPrintWidget: public Ui_QPrintWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // QPRINTWIDGET_H
