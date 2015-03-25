/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/qapplication.h>
#include <QtGui/qmainwindow.h>
#include <QtGui/qtabwidget.h>
#include <QtDBus/qdbusconnection.h>
#include "qdbusviewer.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QMainWindow mw;
#ifndef Q_WS_MAC
    app.setWindowIcon(QIcon(QLatin1String(":/trolltech/qdbusviewer/images/qdbusviewer.png")));
#else
    mw.setWindowTitle(qApp->translate("QtDBusViewer", "Qt D-Bus Viewer"));
#endif


    QTabWidget *mainWidget = new QTabWidget;
    mw.setCentralWidget(mainWidget);
    QDBusViewer *sessionBusViewer = new QDBusViewer(QDBusConnection::sessionBus());
    QDBusViewer *systemBusViewer = new QDBusViewer(QDBusConnection::systemBus());
    mainWidget->addTab(sessionBusViewer, QObject::tr("Session Bus"));
    mainWidget->addTab(systemBusViewer, QObject::tr("System Bus"));

    QMenu *fileMenu = mw.menuBar()->addMenu(QObject::tr("&File"));
    QAction *quitAction = fileMenu->addAction(QObject::tr("&Quit"), &mw, SLOT(close()));
    Q_UNUSED(quitAction);

    QMenu *helpMenu = mw.menuBar()->addMenu(QObject::tr("&Help"));
    QAction *aboutAction = helpMenu->addAction(QObject::tr("&About"));
    aboutAction->setMenuRole(QAction::AboutRole);
    QObject::connect(aboutAction, SIGNAL(triggered()), sessionBusViewer, SLOT(about()));

    QAction *aboutQtAction = helpMenu->addAction(QObject::tr("About &Qt"));
    aboutQtAction->setMenuRole(QAction::AboutQtRole);
    QObject::connect(aboutQtAction, SIGNAL(triggered()), &app, SLOT(aboutQt()));

    mw.show();

    return app.exec();
}

