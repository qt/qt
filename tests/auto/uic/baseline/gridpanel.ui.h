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
** Form generated from reading ui file 'gridpanel.ui'
**
** Created: Mon Sep 1 09:31:03 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef GRIDPANEL_H
#define GRIDPANEL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class Ui_GridPanel
{
public:
    QVBoxLayout *vboxLayout;
    QGroupBox *m_gridGroupBox;
    QGridLayout *gridLayout;
    QCheckBox *m_visibleCheckBox;
    QLabel *label;
    QSpinBox *m_deltaXSpinBox;
    QCheckBox *m_snapXCheckBox;
    QHBoxLayout *hboxLayout;
    QPushButton *m_resetButton;
    QSpacerItem *spacerItem;
    QLabel *label_2;
    QSpinBox *m_deltaYSpinBox;
    QCheckBox *m_snapYCheckBox;

    void setupUi(QWidget *qdesigner_internal__GridPanel)
    {
        if (qdesigner_internal__GridPanel->objectName().isEmpty())
            qdesigner_internal__GridPanel->setObjectName(QString::fromUtf8("qdesigner_internal__GridPanel"));
        qdesigner_internal__GridPanel->resize(393, 110);
        vboxLayout = new QVBoxLayout(qdesigner_internal__GridPanel);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        vboxLayout->setContentsMargins(0, 0, 0, 0);
        m_gridGroupBox = new QGroupBox(qdesigner_internal__GridPanel);
        m_gridGroupBox->setObjectName(QString::fromUtf8("m_gridGroupBox"));
        gridLayout = new QGridLayout(m_gridGroupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_visibleCheckBox = new QCheckBox(m_gridGroupBox);
        m_visibleCheckBox->setObjectName(QString::fromUtf8("m_visibleCheckBox"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_visibleCheckBox->sizePolicy().hasHeightForWidth());
        m_visibleCheckBox->setSizePolicy(sizePolicy);

        gridLayout->addWidget(m_visibleCheckBox, 0, 0, 1, 1);

        label = new QLabel(m_gridGroupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 1, 1, 1);

        m_deltaXSpinBox = new QSpinBox(m_gridGroupBox);
        m_deltaXSpinBox->setObjectName(QString::fromUtf8("m_deltaXSpinBox"));
        m_deltaXSpinBox->setMinimum(2);
        m_deltaXSpinBox->setMaximum(100);

        gridLayout->addWidget(m_deltaXSpinBox, 0, 2, 1, 1);

        m_snapXCheckBox = new QCheckBox(m_gridGroupBox);
        m_snapXCheckBox->setObjectName(QString::fromUtf8("m_snapXCheckBox"));
        sizePolicy.setHeightForWidth(m_snapXCheckBox->sizePolicy().hasHeightForWidth());
        m_snapXCheckBox->setSizePolicy(sizePolicy);

        gridLayout->addWidget(m_snapXCheckBox, 0, 3, 1, 1);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        m_resetButton = new QPushButton(m_gridGroupBox);
        m_resetButton->setObjectName(QString::fromUtf8("m_resetButton"));

        hboxLayout->addWidget(m_resetButton);

        spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);


        gridLayout->addLayout(hboxLayout, 1, 0, 1, 1);

        label_2 = new QLabel(m_gridGroupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 1, 1, 1);

        m_deltaYSpinBox = new QSpinBox(m_gridGroupBox);
        m_deltaYSpinBox->setObjectName(QString::fromUtf8("m_deltaYSpinBox"));
        m_deltaYSpinBox->setMinimum(2);
        m_deltaYSpinBox->setMaximum(100);

        gridLayout->addWidget(m_deltaYSpinBox, 1, 2, 1, 1);

        m_snapYCheckBox = new QCheckBox(m_gridGroupBox);
        m_snapYCheckBox->setObjectName(QString::fromUtf8("m_snapYCheckBox"));
        sizePolicy.setHeightForWidth(m_snapYCheckBox->sizePolicy().hasHeightForWidth());
        m_snapYCheckBox->setSizePolicy(sizePolicy);

        gridLayout->addWidget(m_snapYCheckBox, 1, 3, 1, 1);


        vboxLayout->addWidget(m_gridGroupBox);

#ifndef QT_NO_SHORTCUT
        label->setBuddy(m_deltaXSpinBox);
        label_2->setBuddy(m_deltaYSpinBox);
#endif // QT_NO_SHORTCUT

        retranslateUi(qdesigner_internal__GridPanel);

        QMetaObject::connectSlotsByName(qdesigner_internal__GridPanel);
    } // setupUi

    void retranslateUi(QWidget *qdesigner_internal__GridPanel)
    {
        qdesigner_internal__GridPanel->setWindowTitle(QApplication::translate("qdesigner_internal::GridPanel", "Form", 0, QApplication::UnicodeUTF8));
        m_gridGroupBox->setTitle(QApplication::translate("qdesigner_internal::GridPanel", "Grid", 0, QApplication::UnicodeUTF8));
        m_visibleCheckBox->setText(QApplication::translate("qdesigner_internal::GridPanel", "Visible", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("qdesigner_internal::GridPanel", "Grid &X", 0, QApplication::UnicodeUTF8));
        m_snapXCheckBox->setText(QApplication::translate("qdesigner_internal::GridPanel", "Snap", 0, QApplication::UnicodeUTF8));
        m_resetButton->setText(QApplication::translate("qdesigner_internal::GridPanel", "Reset", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("qdesigner_internal::GridPanel", "Grid &Y", 0, QApplication::UnicodeUTF8));
        m_snapYCheckBox->setText(QApplication::translate("qdesigner_internal::GridPanel", "Snap", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(qdesigner_internal__GridPanel);
    } // retranslateUi

};

} // namespace qdesigner_internal

namespace qdesigner_internal {
namespace Ui {
    class GridPanel: public Ui_GridPanel {};
} // namespace Ui
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // GRIDPANEL_H
