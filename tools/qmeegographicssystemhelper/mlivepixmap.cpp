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
#include "../private/qimage_p.h"
#include "../private/qpixmap_raster_p.h"
#include "mlivepixmap_p.h"
#include "mliveimage_p.h"
#include <QSharedMemory>

/* MLivePixmapPrivate */

MLivePixmapPrivate::MLivePixmapPrivate() : shm(0), shmSerial(0), owns(true), parentImage(0)
{
}

void MLivePixmapPrivate::copyBackFrom(const void *raw)
{
    Q_Q(MLivePixmap);
    
    q->detach();
    shm->lock();
    uchar *dest = ((uchar *) shm->data()) + (2 * sizeof(int));
    memcpy(dest, raw, q->width() * q->height() * 4);
    shm->unlock();
}

MLivePixmapPrivate::~MLivePixmapPrivate()
{
    Q_Q(MLivePixmap);
    
    if (parentImage)
        parentImage->d_ptr->detachPixmap(q);

    if (shm)
        shm->detach();
        
    if (owns)
        delete shm;        
}

/* MLivePixmap */

MLivePixmap::MLivePixmap(QPixmapData *p) : QPixmap(p), d_ptr(new MLivePixmapPrivate())
{
    Q_D(MLivePixmap);
    d->q_ptr = this;
}

MLivePixmap* MLivePixmap::fromLiveImage(MLiveImage *liveImage)
{
    static int counter = 100;
    QSharedMemory *shm = NULL;
    uchar* imgData = NULL;
    int *header = NULL;
    int w = liveImage->width();
    int h = liveImage->height();
    
    counter++;
    shm = new QSharedMemory(QString("MLivePixmap%1").arg(counter));
    shm->create((w * h * 4) + 2 * sizeof(int)); // +2 to store width & height
    shm->attach();
    
    imgData = ((uchar *) shm->data()) + (2 * sizeof(int));
    header = (int *) shm->data();
    
    header[0] = w;
    header[1] = h;
    
    QImage img(imgData, w, h, QImage::Format_ARGB32_Premultiplied);
    
    QPixmapData *pmd = new QRasterPixmapData(QPixmapData::PixmapType);
    pmd->fromImage(img, Qt::NoOpaqueDetection);
    
    MLivePixmap *livePixmap = new MLivePixmap(pmd);
    livePixmap->d_ptr->shm = shm;
    livePixmap->d_ptr->owns = true;
    livePixmap->d_ptr->shmSerial = counter;
    livePixmap->d_ptr->parentImage = liveImage;
    
    liveImage->d_ptr->attachPixmap(livePixmap);

    return livePixmap;    
}

MLivePixmap* MLivePixmap::fromHandle(Qt::HANDLE handle)
{
    QSharedMemory *shm = NULL;
    int *header;
    int width;
    int height;
    uchar* imgData;
    
    shm = new QSharedMemory(QString("MLivePixmap%1").arg(handle));
    shm->attach();
    
    shm->lock();
    header = (int *) shm->data();
    width = header[0];
    height = header[1];
    shm->unlock();
    
    imgData = ((uchar *) shm->data()) + (2 * sizeof(int));
    QImage img(imgData, width, height, QImage::Format_ARGB32_Premultiplied);
    
    QPixmapData *pmd = new QRasterPixmapData(QPixmapData::PixmapType);
    pmd->fromImage(img, Qt::NoOpaqueDetection);

    MLivePixmap *livePixmap = new MLivePixmap(pmd);
    livePixmap->d_ptr->shm = shm;
    livePixmap->d_ptr->owns = false;
    livePixmap->d_ptr->shmSerial = handle;

    return livePixmap;
}

MLivePixmap::~MLivePixmap()
{
}

Qt::HANDLE MLivePixmap::handle()
{
    Q_D(MLivePixmap);
    return d->shmSerial;
}
