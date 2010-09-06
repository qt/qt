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

#include "qmeegolivepixmap.h"

#ifndef QMEEGOLIVEPIXMAP_P_H
#define QMEEGOLIVEPIXMAP_P_H

class QMeeGoLivePixmapPrivate
{
public:
    Q_DECLARE_PUBLIC(QMeeGoLivePixmap);
    QMeeGoLivePixmapPrivate();
    void copyBackFrom(const void *raw);
    virtual ~QMeeGoLivePixmapPrivate();
    
    QSharedMemory *shm;
    int shmSerial;
    bool owns;
    QMeeGoLiveImage *parentImage;
    
    QMeeGoLivePixmap *q_ptr;
    
    friend class QMeeGoLiveImage;
    friend class QMeeGoLiveImagePrivate;
};

#endif
