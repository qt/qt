/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QCoreApplication>
#include "moverlaywidget.h"
#include "mgraphicssystemhelper.h"
#include "mruntime.h"

MOverlayWidget::MOverlayWidget(int surfaceWidth, int surfaceHeight, QWidget *parent) : QWidget(parent, 0), 
    sw(surfaceWidth), 
    sh(surfaceHeight)
{
    if (! MGraphicsSystemHelper::isRunningMeeGo())
        qFatal("MOverlayWidget can only be used when running with 'meego' graphics system!");

    MRuntime::setSurfaceFixedSize(surfaceWidth, surfaceHeight);

    scaleW = sw / 864.0;
    scaleH = sh / 480.0;
    installEventFilter(this);
}

QPoint MOverlayWidget::convertPoint(const QPoint &p)
{
    int x = p.x() * scaleW;
    int y = p.y() * scaleH;
    return QPoint(x, y);
}

void MOverlayWidget::showEvent(QShowEvent *event)
{
    MRuntime::setSurfaceScaling(0, 0, width(), height());
}

bool MOverlayWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->spontaneous() == false)
        return false;

    switch(event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        {
            QMouseEvent *e = static_cast <QMouseEvent *>(event);
            QMouseEvent newEvent = QMouseEvent(e->type(), 
                                               convertPoint(e->pos()), 
                                               convertPoint(e->globalPos()), 
                                               e->button(), 
                                               e->buttons(), 
                                               e->modifiers());
            QCoreApplication::sendEvent(this, &newEvent);
            return true;
        }

        default:
            return false;
    }
}
