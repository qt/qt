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

#include "mlivepixmap.h"

#ifndef MLIVEPIXMAP_P_H
#define MLIVEPIXMAP_P_H

class MLivePixmapPrivate
{
public:
    Q_DECLARE_PUBLIC(MLivePixmap);
    MLivePixmapPrivate();
    void copyBackFrom(const void *raw);
    virtual ~MLivePixmapPrivate();
    
    QSharedMemory *shm;
    int shmSerial;
    bool owns;
    MLiveImage *parentImage;
    
    MLivePixmap *q_ptr;
    
    friend class MLiveImage;
    friend class MLiveImagePrivate;
};

#endif