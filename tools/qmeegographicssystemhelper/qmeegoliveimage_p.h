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

#include "mliveimage.h"

#ifndef MLIVEIMAGE_P_H
#define MLIVEIMAGE_P_H

class MLiveImagePrivate
{
public:
    Q_DECLARE_PUBLIC(MLiveImage);
    MLiveImagePrivate();
    virtual ~MLiveImagePrivate();
    void attachPixmap(MLivePixmap* pixmap);
    void detachPixmap(MLivePixmap* pixmap);
        
    QList <MLivePixmap*> attachedPixmaps;
    MLiveImage *q_ptr;
    
    friend class MLivePixmap;
    friend class MLivePixmapPrivate;
};

#endif