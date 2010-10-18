/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmeegoliveimage.h"
#include "qmeegoliveimage_p.h"
#include "qmeegolivepixmap.h"
#include "qmeegolivepixmap_p.h"

/* QMeeGoLiveImagePrivate */

QMeeGoLiveImagePrivate::QMeeGoLiveImagePrivate()
{
}

QMeeGoLiveImagePrivate::~QMeeGoLiveImagePrivate()
{
    if (attachedPixmaps.length() > 0)
        qWarning("Destroying QMeeGoLiveImage but it still has QMeeGoLivePixmaps attached!");
}

void QMeeGoLiveImagePrivate::attachPixmap(QMeeGoLivePixmap* pixmap)
{
    attachedPixmaps << pixmap;
}

void QMeeGoLiveImagePrivate::detachPixmap(QMeeGoLivePixmap* pixmap)
{
    attachedPixmaps.removeAll(pixmap);
}

/* QMeeGoLiveImage */

QMeeGoLiveImage* QMeeGoLiveImage::liveImageWithSize(int w, int h, Format format, int buffers)
{
    if (format != Format_ARGB32_Premultiplied) {
        qWarning("Only _ARGB32_Premultiplied format is supported for live images now!");
        return 0;
    }
    
    if (buffers != 1) {
        qWarning("Only single-buffer streams are supported at the moment");
        return 0;
    }
    
    QMeeGoLiveImage *liveImage = new QMeeGoLiveImage(w, h);
    return liveImage;
}

QMeeGoLiveImage::QMeeGoLiveImage(int w, int h) : QImage(w, h, QImage::Format_ARGB32_Premultiplied), d_ptr(new QMeeGoLiveImagePrivate())
{
    Q_D(QMeeGoLiveImage);
    d->q_ptr = this;
}

QMeeGoLiveImage::~QMeeGoLiveImage()
{
}

void QMeeGoLiveImage::lock(int buffer)
{
    if (buffer != 0)
        qWarning("Only locking 0 buffer is supported at the moment!");
}

void QMeeGoLiveImage::release(int buffer)
{
    Q_D(QMeeGoLiveImage);
    
    if (buffer != 0) {
        qWarning("Only locking 0 buffer is supported at the moment!");
        return;
    }

    // We need to copy the update image to all the client QMeeGoLivePixmap's
    foreach (QMeeGoLivePixmap* livePixmap, d->attachedPixmaps) 
        livePixmap->d_ptr->copyBackFrom((const void *) bits());
}
