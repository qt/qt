/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include "mainwindow.h"

//! [0]
int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(schema);
    QApplication app(argc, argv);
    MainWindow* const window = new MainWindow;
    window->show();
    return app.exec();
}
//! [0]
