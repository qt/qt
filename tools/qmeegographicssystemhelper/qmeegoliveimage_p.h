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

#include "qmeegoliveimage.h"

#ifndef QMEEGOLIVEIMAGE_P_H
#define QMEEGOLIVEIMAGE_P_H

class QMeeGoLiveImagePrivate
{
public:
    Q_DECLARE_PUBLIC(QMeeGoLiveImage);
    QMeeGoLiveImagePrivate();
    virtual ~QMeeGoLiveImagePrivate();
    void attachPixmap(QMeeGoLivePixmap* pixmap);
    void detachPixmap(QMeeGoLivePixmap* pixmap);
        
    QList <QMeeGoLivePixmap*> attachedPixmaps;
    QMeeGoLiveImage *q_ptr;
    
    friend class QMeeGoLivePixmap;
    friend class QMeeGoLivePixmapPrivate;
};

#endif
