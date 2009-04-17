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

#ifndef SCENE_H
#define SCENE_H

#include <QtGui/qgraphicsscene.h>

class Scene : public QGraphicsScene
{
    Q_OBJECT
public:
    Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent = 0);
    Scene(const QRectF &sceneRect, QObject *parent = 0);
    Scene(QObject *parent = 0);
    ~Scene();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
};

#endif
