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

#include <QApplication>
#include "desktopwidget.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(desktopservices);

    QApplication app(argc, argv);
    DesktopWidget* myWidget = new DesktopWidget(0);
    myWidget->showMaximized(); 
    
    return app.exec();
}

// End of file
