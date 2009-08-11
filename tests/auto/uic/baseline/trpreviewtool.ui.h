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
** http://qt.nokia.com/products/qt/gplexception/ and in the file
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
** Form generated from reading ui file 'trpreviewtool.ui'
**
** Created: Thu Jul 10 09:47:35 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef TRPREVIEWTOOL_H
#define TRPREVIEWTOOL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDockWidget>
#include <QtGui/QHeaderView>
#include <QtGui/QListView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrPreviewToolClass
{
public:
    QAction *actionOpenForm;
    QAction *actionLoadTranslation;
    QAction *actionReloadTranslations;
    QAction *actionClose;
    QAction *actionAbout;
    QAction *actionAbout_Qt;
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuView;
    QMenu *menuViewViews;
    QMenu *menuHelp;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *dwForms;
    QWidget *dockWidgetContents;
    QVBoxLayout *vboxLayout;
    QListView *viewForms;

    void setupUi(QMainWindow *TrPreviewToolClass)
    {
        if (TrPreviewToolClass->objectName().isEmpty())
            TrPreviewToolClass->setObjectName(QString::fromUtf8("TrPreviewToolClass"));
        TrPreviewToolClass->resize(593, 466);
        actionOpenForm = new QAction(TrPreviewToolClass);
        actionOpenForm->setObjectName(QString::fromUtf8("actionOpenForm"));
        const QIcon icon = QIcon(QString::fromUtf8(":/images/open_form.png"));
        actionOpenForm->setIcon(icon);
        actionLoadTranslation = new QAction(TrPreviewToolClass);
        actionLoadTranslation->setObjectName(QString::fromUtf8("actionLoadTranslation"));
        const QIcon icon1 = QIcon(QString::fromUtf8(":/images/load_translation.png"));
        actionLoadTranslation->setIcon(icon1);
        actionReloadTranslations = new QAction(TrPreviewToolClass);
        actionReloadTranslations->setObjectName(QString::fromUtf8("actionReloadTranslations"));
        const QIcon icon2 = QIcon(QString::fromUtf8(":/images/reload_translations.png"));
        actionReloadTranslations->setIcon(icon2);
        actionClose = new QAction(TrPreviewToolClass);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionAbout = new QAction(TrPreviewToolClass);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionAbout_Qt = new QAction(TrPreviewToolClass);
        actionAbout_Qt->setObjectName(QString::fromUtf8("actionAbout_Qt"));
        centralWidget = new QWidget(TrPreviewToolClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        TrPreviewToolClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(TrPreviewToolClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 593, 21));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuViewViews = new QMenu(menuView);
        menuViewViews->setObjectName(QString::fromUtf8("menuViewViews"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        TrPreviewToolClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TrPreviewToolClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        mainToolBar->setOrientation(Qt::Horizontal);
        TrPreviewToolClass->addToolBar(static_cast<Qt::ToolBarArea>(4), mainToolBar);
        statusBar = new QStatusBar(TrPreviewToolClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        TrPreviewToolClass->setStatusBar(statusBar);
        dwForms = new QDockWidget(TrPreviewToolClass);
        dwForms->setObjectName(QString::fromUtf8("dwForms"));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        vboxLayout = new QVBoxLayout(dockWidgetContents);
        vboxLayout->setSpacing(0);
        vboxLayout->setMargin(0);
        vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
        viewForms = new QListView(dockWidgetContents);
        viewForms->setObjectName(QString::fromUtf8("viewForms"));
        viewForms->setEditTriggers(QAbstractItemView::NoEditTriggers);
        viewForms->setAlternatingRowColors(true);
        viewForms->setUniformItemSizes(true);

        vboxLayout->addWidget(viewForms);

        dwForms->setWidget(dockWidgetContents);
        TrPreviewToolClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), dwForms);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuView->addAction(menuViewViews->menuAction());
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAbout_Qt);
        menuFile->addAction(actionOpenForm);
        menuFile->addAction(actionLoadTranslation);
        menuFile->addAction(actionReloadTranslations);
        menuFile->addSeparator();
        menuFile->addAction(actionClose);
        mainToolBar->addAction(actionOpenForm);
        mainToolBar->addAction(actionLoadTranslation);
        mainToolBar->addAction(actionReloadTranslations);

        retranslateUi(TrPreviewToolClass);

        QMetaObject::connectSlotsByName(TrPreviewToolClass);
    } // setupUi

    void retranslateUi(QMainWindow *TrPreviewToolClass)
    {
        TrPreviewToolClass->setWindowTitle(QApplication::translate("TrPreviewToolClass", "Qt Translation Preview Tool", 0, QApplication::UnicodeUTF8));
        actionOpenForm->setText(QApplication::translate("TrPreviewToolClass", "&Open Form...", 0, QApplication::UnicodeUTF8));
        actionLoadTranslation->setText(QApplication::translate("TrPreviewToolClass", "&Load Translation...", 0, QApplication::UnicodeUTF8));
        actionReloadTranslations->setText(QApplication::translate("TrPreviewToolClass", "&Reload Translations", 0, QApplication::UnicodeUTF8));
        actionReloadTranslations->setShortcut(QApplication::translate("TrPreviewToolClass", "F5", 0, QApplication::UnicodeUTF8));
        actionClose->setText(QApplication::translate("TrPreviewToolClass", "&Close", 0, QApplication::UnicodeUTF8));
        actionAbout->setText(QApplication::translate("TrPreviewToolClass", "About", 0, QApplication::UnicodeUTF8));
        actionAbout_Qt->setText(QApplication::translate("TrPreviewToolClass", "About Qt", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("TrPreviewToolClass", "&View", 0, QApplication::UnicodeUTF8));
        menuViewViews->setTitle(QApplication::translate("TrPreviewToolClass", "&Views", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("TrPreviewToolClass", "&Help", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("TrPreviewToolClass", "&File", 0, QApplication::UnicodeUTF8));
        dwForms->setWindowTitle(QApplication::translate("TrPreviewToolClass", "Forms", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TrPreviewToolClass: public Ui_TrPreviewToolClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // TRPREVIEWTOOL_H
