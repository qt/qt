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

#include "river.h"
#include "menu.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    if (app.arguments().size() == 1) {
        qWarning("you have to specifiy a path to look for the photos");
        return 0;
    }
    
    
    QGraphicsScene scene;
    scene.setSceneRect(QRectF(QPointF(), River::fixedSize()));

    QGraphicsView view(&scene);
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    const int fw = view.frameWidth() * 2;
    view.setFixedSize(River::fixedSize() + QSize(fw,fw));

    River river(app.arguments()[1]);
    scene.addItem(&river);
    
    Menu menu(&river);
    menu.addAction(QLatin1String("River Mode"), &river, SLOT(setRiverMode()));
    menu.addAction(QLatin1String("Grid Mode"), &river, SLOT(setGridMode()));
    menu.addAction(QLatin1String("Cover Flow"), &river, SLOT(setCoverMode()));
    menu.addAction(QLatin1String("Hide Menu"), &menu, SLOT(hide()));
    menu.addAction(QLatin1String("Exit"), &app, SLOT(quit()));
    menu.setZValue(2);
    menu.setFocus();

    river.menu = &menu;
    view.show();

    return app.exec();
}
