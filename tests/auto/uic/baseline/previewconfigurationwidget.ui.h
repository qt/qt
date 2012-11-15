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
** Form generated from reading ui file 'previewconfigurationwidget.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef PREVIEWCONFIGURATIONWIDGET_H
#define PREVIEWCONFIGURATIONWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFormLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <textpropertyeditor_p.h>

QT_BEGIN_NAMESPACE

class Ui_PreviewConfigurationWidget
{
public:
    QFormLayout *formLayout;
    QLabel *m_styleLabel;
    QComboBox *m_styleCombo;
    QLabel *m_appStyleSheetLabel;
    QHBoxLayout *hboxLayout;
    qdesigner_internal::TextPropertyEditor *m_appStyleSheetLineEdit;
    QToolButton *m_appStyleSheetChangeButton;
    QToolButton *m_appStyleSheetClearButton;
    QLabel *m_skinLabel;
    QHBoxLayout *hboxLayout1;
    QComboBox *m_skinCombo;
    QToolButton *m_skinRemoveButton;

    void setupUi(QGroupBox *PreviewConfigurationWidget)
    {
        if (PreviewConfigurationWidget->objectName().isEmpty())
            PreviewConfigurationWidget->setObjectName(QString::fromUtf8("PreviewConfigurationWidget"));
        PreviewConfigurationWidget->setCheckable(true);
        formLayout = new QFormLayout(PreviewConfigurationWidget);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        m_styleLabel = new QLabel(PreviewConfigurationWidget);
        m_styleLabel->setObjectName(QString::fromUtf8("m_styleLabel"));

        formLayout->setWidget(0, QFormLayout::LabelRole, m_styleLabel);

        m_styleCombo = new QComboBox(PreviewConfigurationWidget);
        m_styleCombo->setObjectName(QString::fromUtf8("m_styleCombo"));

        formLayout->setWidget(0, QFormLayout::FieldRole, m_styleCombo);

        m_appStyleSheetLabel = new QLabel(PreviewConfigurationWidget);
        m_appStyleSheetLabel->setObjectName(QString::fromUtf8("m_appStyleSheetLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, m_appStyleSheetLabel);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        m_appStyleSheetLineEdit = new qdesigner_internal::TextPropertyEditor(PreviewConfigurationWidget);
        m_appStyleSheetLineEdit->setObjectName(QString::fromUtf8("m_appStyleSheetLineEdit"));
        m_appStyleSheetLineEdit->setMinimumSize(QSize(149, 0));

        hboxLayout->addWidget(m_appStyleSheetLineEdit);

        m_appStyleSheetChangeButton = new QToolButton(PreviewConfigurationWidget);
        m_appStyleSheetChangeButton->setObjectName(QString::fromUtf8("m_appStyleSheetChangeButton"));

        hboxLayout->addWidget(m_appStyleSheetChangeButton);

        m_appStyleSheetClearButton = new QToolButton(PreviewConfigurationWidget);
        m_appStyleSheetClearButton->setObjectName(QString::fromUtf8("m_appStyleSheetClearButton"));

        hboxLayout->addWidget(m_appStyleSheetClearButton);


        formLayout->setLayout(1, QFormLayout::FieldRole, hboxLayout);

        m_skinLabel = new QLabel(PreviewConfigurationWidget);
        m_skinLabel->setObjectName(QString::fromUtf8("m_skinLabel"));

        formLayout->setWidget(2, QFormLayout::LabelRole, m_skinLabel);

        hboxLayout1 = new QHBoxLayout();
        hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
        m_skinCombo = new QComboBox(PreviewConfigurationWidget);
        m_skinCombo->setObjectName(QString::fromUtf8("m_skinCombo"));

        hboxLayout1->addWidget(m_skinCombo);

        m_skinRemoveButton = new QToolButton(PreviewConfigurationWidget);
        m_skinRemoveButton->setObjectName(QString::fromUtf8("m_skinRemoveButton"));

        hboxLayout1->addWidget(m_skinRemoveButton);


        formLayout->setLayout(2, QFormLayout::FieldRole, hboxLayout1);


        retranslateUi(PreviewConfigurationWidget);

        QMetaObject::connectSlotsByName(PreviewConfigurationWidget);
    } // setupUi

    void retranslateUi(QGroupBox *PreviewConfigurationWidget)
    {
        PreviewConfigurationWidget->setWindowTitle(QApplication::translate("PreviewConfigurationWidget", "Form", 0, QApplication::UnicodeUTF8));
        PreviewConfigurationWidget->setTitle(QApplication::translate("PreviewConfigurationWidget", "Print/Preview Configuration", 0, QApplication::UnicodeUTF8));
        m_styleLabel->setText(QApplication::translate("PreviewConfigurationWidget", "Style", 0, QApplication::UnicodeUTF8));
        m_appStyleSheetLabel->setText(QApplication::translate("PreviewConfigurationWidget", "Style sheet", 0, QApplication::UnicodeUTF8));
        m_appStyleSheetChangeButton->setText(QApplication::translate("PreviewConfigurationWidget", "...", 0, QApplication::UnicodeUTF8));
        m_appStyleSheetClearButton->setText(QApplication::translate("PreviewConfigurationWidget", "...", 0, QApplication::UnicodeUTF8));
        m_skinLabel->setText(QApplication::translate("PreviewConfigurationWidget", "Device skin", 0, QApplication::UnicodeUTF8));
        m_skinRemoveButton->setText(QApplication::translate("PreviewConfigurationWidget", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(PreviewConfigurationWidget);
    } // retranslateUi

};

namespace Ui {
    class PreviewConfigurationWidget: public Ui_PreviewConfigurationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PREVIEWCONFIGURATIONWIDGET_H
