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
#include "qmeegographicssystemplugin.h"
#include "qmeegographicssystem.h"

QStringList QMeeGoGraphicsSystemPlugin::keys() const
{
    QStringList list;
    list << "meego";
    return list;
}

QGraphicsSystem *QMeeGoGraphicsSystemPlugin::create(const QString&)
{
    return new QMeeGoGraphicsSystem;
}

Q_EXPORT_PLUGIN2(meego, QMeeGoGraphicsSystemPlugin)
