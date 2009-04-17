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
#include "scene.h"
#include "qgraphicspiemenu.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Scene scene;

    QGraphicsView view(&scene);
    view.show();

    return app.exec();
}
