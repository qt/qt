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
** Form generated from reading ui file 'form.ui'
**
** Created: Thu Jul 10 09:47:34 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef FORM_H
#define FORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QTimeEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "worldtimeclock.h"

QT_BEGIN_NAMESPACE

class Ui_WorldTimeForm
{
public:
    QHBoxLayout *hboxLayout;
    WorldTimeClock *worldTimeClock;
    QVBoxLayout *vboxLayout;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout1;
    QLabel *label;
    QTimeEdit *timeEdit;
    QHBoxLayout *hboxLayout2;
    QLabel *label_2;
    QSpinBox *spinBox;
    QSpacerItem *spacerItem1;

    void setupUi(QWidget *WorldTimeForm)
    {
        if (WorldTimeForm->objectName().isEmpty())
            WorldTimeForm->setObjectName(QString::fromUtf8("WorldTimeForm"));
        WorldTimeForm->resize(400, 300);
        hboxLayout = new QHBoxLayout(WorldTimeForm);
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setMargin(9);
#endif
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        worldTimeClock = new WorldTimeClock(WorldTimeForm);
        worldTimeClock->setObjectName(QString::fromUtf8("worldTimeClock"));

        hboxLayout->addWidget(worldTimeClock);

        vboxLayout = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
        vboxLayout->setMargin(1);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem);

        hboxLayout1 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout1->setSpacing(6);
#endif
        hboxLayout1->setMargin(1);
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        label = new QLabel(WorldTimeForm);
        label->setObjectName(QString::fromUtf8("label"));

        hboxLayout1->addWidget(label);

        timeEdit = new QTimeEdit(WorldTimeForm);
        timeEdit->setObjectName(QString::fromUtf8("timeEdit"));
        timeEdit->setReadOnly(true);

        hboxLayout1->addWidget(timeEdit);


        vboxLayout->addLayout(hboxLayout1);

        hboxLayout2 = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout2->setSpacing(6);
#endif
        hboxLayout2->setMargin(1);
        hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
        label_2 = new QLabel(WorldTimeForm);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        hboxLayout2->addWidget(label_2);

        spinBox = new QSpinBox(WorldTimeForm);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setMaximum(12);
        spinBox->setMinimum(-12);

        hboxLayout2->addWidget(spinBox);


        vboxLayout->addLayout(hboxLayout2);

        spacerItem1 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        vboxLayout->addItem(spacerItem1);


        hboxLayout->addLayout(vboxLayout);


        retranslateUi(WorldTimeForm);
        QObject::connect(spinBox, SIGNAL(valueChanged(int)), worldTimeClock, SLOT(setTimeZone(int)));
        QObject::connect(worldTimeClock, SIGNAL(updated(QTime)), timeEdit, SLOT(setTime(QTime)));

        QMetaObject::connectSlotsByName(WorldTimeForm);
    } // setupUi

    void retranslateUi(QWidget *WorldTimeForm)
    {
        WorldTimeForm->setWindowTitle(QApplication::translate("WorldTimeForm", "World Time Clock", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("WorldTimeForm", "Current time:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("WorldTimeForm", "Set time zone:", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(WorldTimeForm);
    } // retranslateUi

};

namespace Ui {
    class WorldTimeForm: public Ui_WorldTimeForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // FORM_H
