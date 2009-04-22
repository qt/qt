/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include "window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Window w;
    w.resize(400, 400);
    w.show();
    return app.exec();
}
