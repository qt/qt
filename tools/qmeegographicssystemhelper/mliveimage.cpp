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
#include "mliveimage_p.h"
#include "mlivepixmap.h"
#include "mlivepixmap_p.h"

/* MLiveImagePrivate */

MLiveImagePrivate::MLiveImagePrivate()
{
}

MLiveImagePrivate::~MLiveImagePrivate()
{
    if (attachedPixmaps.length() > 0)
        qWarning("Destroying MLiveImage but it still has MLivePixmaps attached!");
}

void MLiveImagePrivate::attachPixmap(MLivePixmap* pixmap)
{
    attachedPixmaps << pixmap;
}

void MLiveImagePrivate::detachPixmap(MLivePixmap* pixmap)
{
    attachedPixmaps.removeAll(pixmap);
}

/* MLiveImage */

MLiveImage* MLiveImage::liveImageWithSize(int w, int h, Format format, int buffers)
{
    if (format != Format_ARGB32_Premultiplied) {
        qWarning("Only _ARGB32_Premultiplied format is supported for live images now!");
        return 0;
    }
    
    if (buffers != 1) {
        qWarning("Only single-buffer streams are supported at the moment");
        return 0;
    }
    
    MLiveImage *liveImage = new MLiveImage(w, h);
    return liveImage;
}

MLiveImage::MLiveImage(int w, int h) : QImage(w, h, QImage::Format_ARGB32_Premultiplied), d_ptr(new MLiveImagePrivate())
{
    Q_D(MLiveImage);
    d->q_ptr = this;
}

MLiveImage::~MLiveImage()
{
}

void MLiveImage::lock(int buffer)
{
    if (buffer != 0)
        qWarning("Only locking 0 buffer is supported at the moment!");
}

void MLiveImage::release(int buffer)
{
    Q_D(MLiveImage);
    
    if (buffer != 0) {
        qWarning("Only locking 0 buffer is supported at the moment!");
        return;
    }

    // We need to copy the update image to all the client MLivePixmap's
    foreach (MLivePixmap* livePixmap, d->attachedPixmaps) 
        livePixmap->d_ptr->copyBackFrom((const void *) bits());
}