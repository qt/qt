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

#ifndef MGRAPHICSSYSTEMPLUGIN_H
#define MGRAPHICSSYSTEMPLUGIN_H

#include <private/qgraphicssystemplugin_p.h>

class QMeeGoGraphicsSystemPlugin : public QGraphicsSystemPlugin
{
public:
    virtual QStringList keys() const;
    virtual QGraphicsSystem *create(const QString&);
};

#endif
