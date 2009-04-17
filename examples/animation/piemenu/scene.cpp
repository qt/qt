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

#include "qgraphicspiemenu.h"
#include "scene.h"

#include <QtGui/qaction.h>
#include <QtGui/qgraphicssceneevent.h>

Scene::Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height, parent)
{
}

Scene::Scene(const QRectF &sceneRect, QObject *parent)
    : QGraphicsScene(sceneRect, parent)
{
}

Scene::Scene(QObject *parent)
    : QGraphicsScene(parent)
{
}

Scene::~Scene()
{
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPieMenu *menu = new QGraphicsPieMenu;
    for (int i = 0; i < 5; ++i)
        menu->addAction(new QAction(QString("Item %1").arg(i), menu));
    menu->popup(event->scenePos());
}
