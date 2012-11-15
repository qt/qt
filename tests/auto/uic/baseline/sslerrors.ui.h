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
** Form generated from reading ui file 'sslerrors.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef SSLERRORS_H
#define SSLERRORS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SslErrors
{
public:
    QVBoxLayout *vboxLayout;
    QLabel *label;
    QListWidget *sslErrorList;
    QHBoxLayout *hboxLayout;
    QPushButton *certificateChainButton;
    QSpacerItem *spacerItem;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *SslErrors)
    {
        if (SslErrors->objectName().isEmpty())
            SslErrors->setObjectName(QString::fromUtf8("SslErrors"));
        SslErrors->resize(371, 216);
        vboxLayout = new QVBoxLayout(SslErrors);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        label = new QLabel(SslErrors);
        label->setObjectName(QString::fromUtf8("label"));
        label->setWordWrap(true);

        vboxLayout->addWidget(label);

        sslErrorList = new QListWidget(SslErrors);
        sslErrorList->setObjectName(QString::fromUtf8("sslErrorList"));

        vboxLayout->addWidget(sslErrorList);

        hboxLayout = new QHBoxLayout();
        hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
        certificateChainButton = new QPushButton(SslErrors);
        certificateChainButton->setObjectName(QString::fromUtf8("certificateChainButton"));
        certificateChainButton->setAutoDefault(false);

        hboxLayout->addWidget(certificateChainButton);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        pushButton = new QPushButton(SslErrors);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        hboxLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(SslErrors);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));

        hboxLayout->addWidget(pushButton_2);


        vboxLayout->addLayout(hboxLayout);


        retranslateUi(SslErrors);
        QObject::connect(pushButton, SIGNAL(clicked()), SslErrors, SLOT(accept()));
        QObject::connect(pushButton_2, SIGNAL(clicked()), SslErrors, SLOT(reject()));

        QMetaObject::connectSlotsByName(SslErrors);
    } // setupUi

    void retranslateUi(QDialog *SslErrors)
    {
        SslErrors->setWindowTitle(QApplication::translate("SslErrors", "Unable To Validate The Connection", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SslErrors", "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#ff0000;\">Warning</span><span style=\" color:#ff0000;\">:</span><span style=\" color:#000000;\"> One or more errors with this connection prevent validating the authenticity of the host you are connecting to. Please review the following list of errors, and click </span><span style=\" color:#000000;\">Ignore</span><span style=\" color:#000000;\"> to continue, or </span><span style=\" color:#000000;\">Cancel</span><span style=\" color:#000000;\"> to abort the connection.</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        certificateChainButton->setText(QApplication::translate("SslErrors", "View Certificate Chain", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("SslErrors", "Ignore", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("SslErrors", "Cancel", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(SslErrors);
    } // retranslateUi

};

namespace Ui {
    class SslErrors: public Ui_SslErrors {};
} // namespace Ui

QT_END_NAMESPACE

#endif // SSLERRORS_H
