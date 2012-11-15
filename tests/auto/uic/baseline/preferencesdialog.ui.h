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
** Form generated from reading ui file 'preferencesdialog.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <previewconfigurationwidget_p.h>
#include "fontpanel.h"
#include "gridpanel_p.h"

QT_BEGIN_NAMESPACE

class Ui_PreferencesDialog
{
public:
    QVBoxLayout *vboxLayout;
    QHBoxLayout *hboxLayout;
    QVBoxLayout *vboxLayout1;
    QGroupBox *m_uiModeGroupBox;
    QVBoxLayout *vboxLayout2;
    QComboBox *m_uiModeCombo;
    FontPanel *m_fontPanel;
    qdesigner_internal::PreviewConfigurationWidget *m_previewConfigurationWidget;
    QVBoxLayout *vboxLayout3;
    QGroupBox *m_templatePathGroupBox;
    QGridLayout *gridLayout;
    QListWidget *m_templatePathListWidget;
    QToolButton *m_addTemplatePathButton;
    QToolButton *m_removeTemplatePathButton;
    QSpacerItem *spacerItem;
    qdesigner_internal::GridPanel *m_gridPanel;
    QFrame *line;
    QDialogButtonBox *m_dialogButtonBox;

    void setupUi(QDialog *PreferencesDialog)
    {
        if (PreferencesDialog->objectName().isEmpty())
            PreferencesDialog->setObjectName(QString::fromUtf8("PreferencesDialog"));
        PreferencesDialog->resize(455, 359);
        PreferencesDialog->setModal(true);
        vboxLayout = new QVBoxLayout(PreferencesDialog);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        vboxLayout1 = new QVBoxLayout();
        vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
        m_uiModeGroupBox = new QGroupBox(PreferencesDialog);
        m_uiModeGroupBox->setObjectName(QString::fromUtf8("m_uiModeGroupBox"));
        vboxLayout2 = new QVBoxLayout(m_uiModeGroupBox);
        vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
        m_uiModeCombo = new QComboBox(m_uiModeGroupBox);
        m_uiModeCombo->setObjectName(QString::fromUtf8("m_uiModeCombo"));

        vboxLayout2->addWidget(m_uiModeCombo);


        vboxLayout1->addWidget(m_uiModeGroupBox);

        m_fontPanel = new FontPanel(PreferencesDialog);
        m_fontPanel->setObjectName(QString::fromUtf8("m_fontPanel"));

        vboxLayout1->addWidget(m_fontPanel);

        m_previewConfigurationWidget = new qdesigner_internal::PreviewConfigurationWidget(PreferencesDialog);
        m_previewConfigurationWidget->setObjectName(QString::fromUtf8("m_previewConfigurationWidget"));

        vboxLayout1->addWidget(m_previewConfigurationWidget);


        hboxLayout->addLayout(vboxLayout1);

        vboxLayout3 = new QVBoxLayout();
        vboxLayout3->setObjectName(QString::fromUtf8("vboxLayout3"));
        m_templatePathGroupBox = new QGroupBox(PreferencesDialog);
        m_templatePathGroupBox->setObjectName(QString::fromUtf8("m_templatePathGroupBox"));
        gridLayout = new QGridLayout(m_templatePathGroupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_templatePathListWidget = new QListWidget(m_templatePathGroupBox);
        m_templatePathListWidget->setObjectName(QString::fromUtf8("m_templatePathListWidget"));

        gridLayout->addWidget(m_templatePathListWidget, 0, 0, 1, 3);

        m_addTemplatePathButton = new QToolButton(m_templatePathGroupBox);
        m_addTemplatePathButton->setObjectName(QString::fromUtf8("m_addTemplatePathButton"));

        gridLayout->addWidget(m_addTemplatePathButton, 1, 0, 1, 1);

        m_removeTemplatePathButton = new QToolButton(m_templatePathGroupBox);
        m_removeTemplatePathButton->setObjectName(QString::fromUtf8("m_removeTemplatePathButton"));

        gridLayout->addWidget(m_removeTemplatePathButton, 1, 1, 1, 1);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 2, 1, 1);


        vboxLayout3->addWidget(m_templatePathGroupBox);

        m_gridPanel = new qdesigner_internal::GridPanel(PreferencesDialog);
        m_gridPanel->setObjectName(QString::fromUtf8("m_gridPanel"));

        vboxLayout3->addWidget(m_gridPanel);


        hboxLayout->addLayout(vboxLayout3);


        vboxLayout->addLayout(hboxLayout);

        line = new QFrame(PreferencesDialog);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        vboxLayout->addWidget(line);

        m_dialogButtonBox = new QDialogButtonBox(PreferencesDialog);
        m_dialogButtonBox->setObjectName(QString::fromUtf8("m_dialogButtonBox"));
        m_dialogButtonBox->setOrientation(Qt::Horizontal);
        m_dialogButtonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        vboxLayout->addWidget(m_dialogButtonBox);


        retranslateUi(PreferencesDialog);
        QObject::connect(m_dialogButtonBox, SIGNAL(accepted()), PreferencesDialog, SLOT(accept()));
        QObject::connect(m_dialogButtonBox, SIGNAL(rejected()), PreferencesDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(PreferencesDialog);
    } // setupUi

    void retranslateUi(QDialog *PreferencesDialog)
    {
        PreferencesDialog->setWindowTitle(QApplication::translate("PreferencesDialog", "Preferences", 0, QApplication::UnicodeUTF8));
        m_uiModeGroupBox->setTitle(QApplication::translate("PreferencesDialog", "User Interface Mode", 0, QApplication::UnicodeUTF8));
        m_templatePathGroupBox->setTitle(QApplication::translate("PreferencesDialog", "Additional Template Paths", 0, QApplication::UnicodeUTF8));
        m_addTemplatePathButton->setText(QApplication::translate("PreferencesDialog", "...", 0, QApplication::UnicodeUTF8));
        m_removeTemplatePathButton->setText(QApplication::translate("PreferencesDialog", "...", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(PreferencesDialog);
    } // retranslateUi

};

namespace Ui {
    class PreferencesDialog: public Ui_PreferencesDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PREFERENCESDIALOG_H
