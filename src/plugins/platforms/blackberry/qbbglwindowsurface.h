/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QBBGLWINDOWSURFACE_H
#define QBBGLWINDOWSURFACE_H

#include <QtGui/private/qwindowsurface_p.h>
#include <QtOpenGL/private/qglpaintdevice_p.h>

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QGLContext;
class QBBGLContext;
class QBBWindow;

class QBBGLPaintDevice : public QGLPaintDevice
{
public:
    QBBGLPaintDevice(QBBGLContext* platformGlContext);
    virtual ~QBBGLPaintDevice();

    virtual QPaintEngine *paintEngine() const;
    virtual QSize size() const;
    virtual QGLContext *context() const { return mGlContext; }

private:
    QBBGLContext* mPlatformGlContext;
    QGLContext *mGlContext;
};

class QBBGLWindowSurface : public QWindowSurface
{
public:
    QBBGLWindowSurface(QWidget *window);
    virtual ~QBBGLWindowSurface();

    virtual QPaintDevice *paintDevice() { return mPaintDevice; }
    virtual void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    virtual void resize(const QSize &size);
    virtual void beginPaint(const QRegion &region);
    virtual void endPaint(const QRegion &region);

    virtual QWindowSurface::WindowSurfaceFeatures features() const;

private:
    QBBGLContext* mPlatformGlContext;
    QBBGLPaintDevice* mPaintDevice;
};

QT_END_NAMESPACE

#endif // QBBGLWINDOWSURFACE_H
