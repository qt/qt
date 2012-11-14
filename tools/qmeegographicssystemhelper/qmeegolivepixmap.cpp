/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmeegolivepixmap.h"
#include "qmeegolivepixmap_p.h"
#include "qmeegofencesync_p.h"
#include "qmeegoruntime.h"

/* QMeeGoLivePixmapPrivate */

QMeeGoLivePixmapPrivate::QMeeGoLivePixmapPrivate()
{
}

QMeeGoLivePixmapPrivate::~QMeeGoLivePixmapPrivate()
{
}

/* QMeeGoLivePixmap */

QMeeGoLivePixmap* QMeeGoLivePixmap::livePixmapWithSize(int w, int h, Format format)
{
    QImage::Format qtFormat;
    if (format == Format_RGB16)
        qtFormat = QImage::Format_RGB16;
    else if (format == Format_ARGB32_Premultiplied)
        qtFormat = QImage::Format_ARGB32_Premultiplied;
    else {
        qWarning("Unsupported live pixmap format!");
            return 0;
    }

    QPixmapData *pmd = QMeeGoRuntime::pixmapDataWithNewLiveTexture(w, h, qtFormat);
    if (! pmd) {
        qWarning("Failed to create a live texture with given size!");
        return NULL;
    }

    return new QMeeGoLivePixmap(pmd);
}

QMeeGoLivePixmap::QMeeGoLivePixmap(QPixmapData *p) : QPixmap(p), d_ptr(new QMeeGoLivePixmapPrivate())
{
    Q_D(QMeeGoLivePixmap);
    d->q_ptr = this;
}

QMeeGoLivePixmap* QMeeGoLivePixmap::fromHandle(Qt::HANDLE liveTextureHandle)
{
    QPixmapData *pmd = QMeeGoRuntime::pixmapDataFromLiveTextureHandle(liveTextureHandle);
    if (! pmd) {
        qWarning("Failed to create a live texture from given handle!");
        return NULL;
    }

    return new QMeeGoLivePixmap(pmd);
}

Qt::HANDLE QMeeGoLivePixmap::handle()
{
    return QMeeGoRuntime::getLiveTextureHandle(this);
}

QMeeGoLivePixmap::~QMeeGoLivePixmap()
{
}

QImage* QMeeGoLivePixmap::lock(QMeeGoFenceSync *fenceSync)
{
    if (fenceSync)
        return QMeeGoRuntime::lockLiveTexture(this, fenceSync->d_func()->syncObject);
    else
        return QMeeGoRuntime::lockLiveTexture(this, NULL);
}

void QMeeGoLivePixmap::release(QImage *img)
{
    QMeeGoRuntime::releaseLiveTexture(this, img);
}
