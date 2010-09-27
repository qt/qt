/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmeegolivepixmap.h"
#include <private/qimage_p.h>
#include <private/qpixmap_raster_p.h>
#include "qmeegolivepixmap_p.h"
#include "qmeegoliveimage_p.h"
#include <QSharedMemory>

/* QMeeGoLivePixmapPrivate */

QMeeGoLivePixmapPrivate::QMeeGoLivePixmapPrivate() : shm(0), shmSerial(0), owns(true), parentImage(0)
{
}

void QMeeGoLivePixmapPrivate::copyBackFrom(const void *raw)
{
    Q_Q(QMeeGoLivePixmap);
    
    q->detach();
    shm->lock();
    uchar *dest = ((uchar *) shm->data()) + (2 * sizeof(int));
    memcpy(dest, raw, q->width() * q->height() * 4);
    shm->unlock();
}

QMeeGoLivePixmapPrivate::~QMeeGoLivePixmapPrivate()
{
    Q_Q(QMeeGoLivePixmap);
    
    if (parentImage)
        parentImage->d_ptr->detachPixmap(q);

    if (shm)
        shm->detach();
        
    if (owns)
        delete shm;        
}

/* QMeeGoLivePixmap */

QMeeGoLivePixmap::QMeeGoLivePixmap(QPixmapData *p) : QPixmap(p), d_ptr(new QMeeGoLivePixmapPrivate())
{
    Q_D(QMeeGoLivePixmap);
    d->q_ptr = this;
}

QMeeGoLivePixmap* QMeeGoLivePixmap::fromLiveImage(QMeeGoLiveImage *liveImage)
{
    static int counter = 100;
    QSharedMemory *shm = NULL;
    uchar* imgData = NULL;
    int *header = NULL;
    int w = liveImage->width();
    int h = liveImage->height();
    
    counter++;
    shm = new QSharedMemory(QString(QLatin1String("QMeeGoLivePixmap%1")).arg(counter));
    shm->create((w * h * 4) + 2 * sizeof(int)); // +2 to store width & height
    shm->attach();
    
    imgData = ((uchar *) shm->data()) + (2 * sizeof(int));
    header = (int *) shm->data();
    
    header[0] = w;
    header[1] = h;
    
    QImage img(imgData, w, h, QImage::Format_ARGB32_Premultiplied);
    
    QPixmapData *pmd = new QRasterPixmapData(QPixmapData::PixmapType);
    pmd->fromImage(img, Qt::NoOpaqueDetection);
    
    QMeeGoLivePixmap *livePixmap = new QMeeGoLivePixmap(pmd);
    livePixmap->d_ptr->shm = shm;
    livePixmap->d_ptr->owns = true;
    livePixmap->d_ptr->shmSerial = counter;
    livePixmap->d_ptr->parentImage = liveImage;
    
    liveImage->d_ptr->attachPixmap(livePixmap);

    return livePixmap;    
}

QMeeGoLivePixmap* QMeeGoLivePixmap::fromHandle(Qt::HANDLE handle)
{
    QSharedMemory *shm = NULL;
    int *header;
    int width;
    int height;
    uchar* imgData;
    
    shm = new QSharedMemory(QString(QLatin1String("QMeeGoLivePixmap%1")).arg(handle));
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

    QMeeGoLivePixmap *livePixmap = new QMeeGoLivePixmap(pmd);
    livePixmap->d_ptr->shm = shm;
    livePixmap->d_ptr->owns = false;
    livePixmap->d_ptr->shmSerial = handle;

    return livePixmap;
}

QMeeGoLivePixmap::~QMeeGoLivePixmap()
{
}

Qt::HANDLE QMeeGoLivePixmap::handle()
{
    Q_D(QMeeGoLivePixmap);
    return d->shmSerial;
}
