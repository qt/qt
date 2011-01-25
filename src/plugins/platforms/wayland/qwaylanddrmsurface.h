/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDDRMSURFACE_H
#define QWAYLANDDRMSURFACE_H

#include "qwaylanddisplay.h"
#include "qwaylandbuffer.h"

#include <QtGui/private/qwindowsurface_p.h>
#include <QtCore/QVarLengthArray>
#include <QtOpenGL/private/qglpaintdevice_p.h>

#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define QT_RESET_GLERROR()                                \
{                                                         \
    while (glGetError() != GL_NO_ERROR) {}                \
}
#define QT_CHECK_GLERROR()                                \
{                                                         \
    GLenum err = glGetError();                            \
    if (err != GL_NO_ERROR) {                             \
        qDebug("[%s line %d] GL Error: 0x%x",             \
               __FILE__, __LINE__, (int)err);             \
    }                                                     \
}

class QWaylandDrmBuffer : public QWaylandBuffer {
public:
    QWaylandDrmBuffer(QWaylandDisplay *display,
                       const QSize &size, QImage::Format format);
    ~QWaylandDrmBuffer();

    void bindToCurrentFbo();
    QSize size() const { return mSize; }

private:
    EGLImageKHR mImage;
    QWaylandDisplay *mDisplay;
    QSize mSize;
    GLuint mTexture;

};

class QWaylandPaintDevice : public QGLPaintDevice
{
public:
    QWaylandPaintDevice(QWaylandDisplay *display, QWindowSurface *windowSurface, QPlatformGLContext *context);
    ~QWaylandPaintDevice();

    QSize size() const;
    QGLContext *context() const;
    QPaintEngine *paintEngine() const;

    void beginPaint();

    bool isFlipped()const;

    void resize(const QSize &size);

    QWaylandDrmBuffer *currentDrmBuffer() const;
    QWaylandDrmBuffer *currentDrmBufferAndSwap();

private:
    QWaylandDisplay *mDisplay;
    QPlatformGLContext  *mPlatformGLContext;
    QWindowSurface *mWindowSurface;
    QVarLengthArray<QWaylandDrmBuffer *> mBufferList;
    int mCurrentPaintBuffer;
    GLuint mDepthStencil;
    QSize mSize;

};

class QWaylandDrmWindowSurface : public QWindowSurface
{
public:
    QWaylandDrmWindowSurface(QWidget *window);
    ~QWaylandDrmWindowSurface();

    void beginPaint(const QRegion &);

    QPaintDevice *paintDevice();
    void flush(QWidget *widget, const QRegion &region, const QPoint &offset);
    void resize(const QSize &size);

private:

    QWaylandDisplay *mDisplay;
    QWaylandPaintDevice *mPaintDevice;
};

#endif // QWAYLANDDRMSURFACE_H
