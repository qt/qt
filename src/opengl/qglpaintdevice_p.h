/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGLPAINTDEVICE_P_H
#define QGLPAINTDEVICE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of the QtOpenGL module.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//

#include <qpaintdevice.h>
#include <qgl.h>

class QGLPaintDevice : public QPaintDevice
{
public:
    QGLPaintDevice();
    virtual ~QGLPaintDevice();

    virtual void beginPaint();
    virtual void ensureActiveTarget();
    virtual void endPaint();

//    virtual void clearOnBegin() const;
//    virtual QColor clearColor() const;
//    virtual QColor backgroundColor() const;
//    virtual bool autoFillBackground() const;
//    virtual bool hasTransparentBackground() const;

    virtual QGLContext* context() const = 0;
    QGLFormat format() const;
    virtual QSize size() const;

    // returns the QGLPaintDevice for the given QPaintDevice
    static QGLPaintDevice* getDevice(QPaintDevice*);

protected:
    // Inline?
//    void setContext(QGLContext* c);

private:
//    QGLContext* m_context;
    GLuint         m_previousFBO;
};


// Wraps a QGLWidget
class QGLWidget;
class QGLWidgetGLPaintDevice : public QGLPaintDevice
{
public:
    QGLWidgetGLPaintDevice();

    virtual QPaintEngine* paintEngine() const;

//    virtual void clearOnBegin() const;
//    virtual QColor clearColor() const;
//    virtual bool autoFillBackground() const;
//    virtual bool hasTransparentBackground() const;

    // QGLWidgets need to do swapBufers in endPaint:
    virtual void beginPaint();
    virtual void endPaint();
    virtual QSize size() const;
    virtual QGLContext* context() const;

    void setWidget(QGLWidget*);

private:
    friend class QGLWidget;
    QGLWidget *glWidget;
};



/*
Replaces:

class QGLPixelBuffer;
class QGLFramebufferObject;
class QWSGLWindowSurface;
class QGLWindowSurface;
class QGLPixmapData;
class QGLDrawable {
public:
    QGLDrawable() : widget(0), buffer(0), fbo(0)
#if defined(Q_WS_QWS) || (!defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL))
                  , wsurf(0)
#endif
#if !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
                  , pixmapData(0)
#endif
        {}
    void setDevice(QPaintDevice *pdev);
    void swapBuffers();
    void makeCurrent();
    void doneCurrent();
    QSize size() const;
    QGLFormat format() const;
    GLuint bindTexture(const QImage &image, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA);
    GLuint bindTexture(const QPixmap &pixmap, GLenum target = GL_TEXTURE_2D, GLint format = GL_RGBA);
    QColor backgroundColor() const;
    QGLContext *context() const;
    bool autoFillBackground() const;
    bool hasTransparentBackground() const;

#if !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
    QGLPixmapData *copyOnBegin() const;
#endif

private:
    bool wasBound;
    QGLWidget *widget;
    QGLPixelBuffer *buffer;
    QGLFramebufferObject *fbo;
#ifdef Q_WS_QWS
    QWSGLWindowSurface *wsurf;
#elif !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
    QGLWindowSurface *wsurf;
#endif

#if !defined(QT_OPENGL_ES_1) && !defined(QT_OPENGL_ES_1_CL)
    QGLPixmapData *pixmapData;
#endif
    int previous_fbo;
};
*/

#endif // QGLPAINTDEVICE_P_H
