/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwindowsurface_ahigl_p.h"
#include "qscreenahigl_qws.h"

#include <qwsdisplay_qws.h>
#include <QtGui/QPaintDevice>
#include <QtGui/QWidget>
#include <QtOpenGL/private/qglpaintdevice_qws_p.h>
#include <QtOpenGL/private/qgl_p.h>
#include <GLES/gl.h>

/*!
  \class QAhiGLWindowSurfacePrivate
  \internal

  \brief The QAhiGLWindowSurfacePrivate class is the private implementation
  class for class QAhiGLWindowSurface.

  This class contains only state variables.
  */
//! [0]
class QAhiGLWindowSurfacePrivate
{
public:
    QAhiGLWindowSurfacePrivate(EGLDisplay eglDisplay,
			       EGLSurface eglSurface,
                               EGLContext eglContext);

    QPaintDevice *device;

    int textureWidth;
    int textureHeight;

    GLuint texture;
    GLuint frameBufferObject;
    GLuint depthbuf;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};
//! [0]

/*!
  The construct just sets statwe variables using the ones
  provided.
 */
QAhiGLWindowSurfacePrivate::QAhiGLWindowSurfacePrivate(EGLDisplay eglDisplay,
                                                       EGLSurface eglSurface,
                                                       EGLContext eglContext)
    : texture(0), frameBufferObject(0), depthbuf(0), display(eglDisplay),
      surface(eglSurface), context(eglContext)
{
}

inline static int nextPowerOfTwo(uint v)
{
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    ++v;
    return v;
}

/*!
    \class QAhiGLWindowSurface
    \preliminary
    \internal

    \brief The QAhiGLWindowSurface class provides the drawing area
    for top-level windows using OpenGL for drawing on an ATI handheld
    device.

    In \l{Qt for Embedded Linux}, the default behavior for each client is to
    render widgets into an area of memory. The server then displays
    the contents of that memory on the screen. For ATI handheld
    devices using OpenGL, QAhiGLWindowSurface is the window surface
    class that allocates and manages the memory areas in the clients
    and the server.

    When a screen update is required, the server runs through all the
    top-level windows that intersect with the region being updated,
    ensuring that the clients have updated their window surfaces. Then
    the server uses the screen driver to copy the contents of the
    affected window surfaces into its composition and then display the
    composition on the screen.

    \tableofcontents

    \section1 Pure Virtual Functions

    There are two window surface instances for each top-level window.
    One is used by the application when drawing a window, and the
    other is used by the server application to make its copy for
    building a window composition to send to the screen.

    The key() function is implemented to uniquely identify this window
    surface class as "ahigl", and the isValid() function is
    implemented to determine whether the associated window is still
    acceptable for representation by this window surface class.  It
    must either be a window using an \l {QGLWidget} {OpenGL widget},
    or it must be a window whose frame is no bigger than 256 x 256.

    The setGeometry() function is implemented to change the geometry
    of the frame buffer whenever the geometry of the associated
    top-level window changes. The image() function is called by the
    window system when building window compositions to return an image
    of the top-level window.

    The paintDevice() function is implemented to return the appropriate
    paint device.

    \section1 Virtual Functions

    When painting onto the surface, the window system will always call
    the beginPaint() function before any painting operations are
    performed. It ensures that the correct frame buffer will be used
    by the OpenGL library for painting operations. Likewise, the
    endPaint() function is automatically called when the painting is
    done, but it isn't implemented for this example.
*/

/*!
  This is the client side constructor.
 */
//! [1]
QAhiGLWindowSurface::QAhiGLWindowSurface(QWidget *widget,
                                         EGLDisplay eglDisplay,
                                         EGLSurface eglSurface,
                                         EGLContext eglContext)
    : QWSGLWindowSurface(widget)
{
    d_ptr = new QAhiGLWindowSurfacePrivate(eglDisplay, eglSurface, eglContext);
    d_ptr->device = new QWSGLPaintDevice(widget);

    setSurfaceFlags(QWSWindowSurface::Buffered);
}
//! [1]

/*!
  This is the server side constructor.
 */
//! [2]
QAhiGLWindowSurface::QAhiGLWindowSurface(EGLDisplay eglDisplay,
                                         EGLSurface eglSurface,
                                         EGLContext eglContext)
{
    d_ptr = new QAhiGLWindowSurfacePrivate(eglDisplay, eglSurface, eglContext);
    setSurfaceFlags(QWSWindowSurface::Buffered);
}
//! [2]

/*!
  The destructor deletes the OpenGL structures held by the
  private implementation class, and then it deletes the
  private implementation class.
 */
QAhiGLWindowSurface::~QAhiGLWindowSurface()
{
    if (d_ptr->texture)
        glDeleteTextures(1, &d_ptr->texture);
    if (d_ptr->depthbuf)
        glDeleteRenderbuffersOES(1, &d_ptr->depthbuf);
    if (d_ptr->frameBufferObject)
        glDeleteFramebuffersOES(1, &d_ptr->frameBufferObject);

    delete d_ptr;
}

/*!
  This function changes the geometry of the frame buffer
  to the geometry in \a rect. It is called whenever the
  geometry of the associated top-level window changes. It
  also rebuilds the window surface's texture and binds
  the OpenGL identifier to the texture for use by the
  OpenGL library.
 */
//! [3]
void QAhiGLWindowSurface::setGeometry(const QRect &rect)
{
    QSize size = rect.size();

    const QWidget *w = window();
    if (w && !w->mask().isEmpty()) {
        const QRegion region = w->mask()
                               & rect.translated(-w->geometry().topLeft());
        size = region.boundingRect().size();
    }

    if (geometry().size() != size) {

        // Driver specific limitations:
        //   FBO maximimum size of 256x256
        //   Depth buffer required

        d_ptr->textureWidth = qMin(256, nextPowerOfTwo(size.width()));
        d_ptr->textureHeight = qMin(256, nextPowerOfTwo(size.height()));

        glGenTextures(1, &d_ptr->texture);
        glBindTexture(GL_TEXTURE_2D, d_ptr->texture);

        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        const int bufSize = d_ptr->textureWidth * d_ptr->textureHeight * 2;
        GLshort buf[bufSize];
        memset(buf, 0, sizeof(GLshort) * bufSize);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, d_ptr->textureWidth,
                     d_ptr->textureHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, buf);

        glGenRenderbuffersOES(1, &d_ptr->depthbuf);
        glBindRenderbufferOES(GL_RENDERBUFFER_EXT, d_ptr->depthbuf);
        glRenderbufferStorageOES(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT16,
                                 d_ptr->textureWidth, d_ptr->textureHeight);

        glGenFramebuffersOES(1, &d_ptr->frameBufferObject);
        glBindFramebufferOES(GL_FRAMEBUFFER_EXT, d_ptr->frameBufferObject);

        glFramebufferTexture2DOES(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
                                  GL_TEXTURE_2D, d_ptr->texture, 0);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, d_ptr->depthbuf);
        glBindFramebufferOES(GL_FRAMEBUFFER_EXT, 0);
    }

    QWSGLWindowSurface::setGeometry(rect);
}
//! [3]

/*!
  Returns the window  surface's texture as a QByteArray.
 */
QByteArray QAhiGLWindowSurface::permanentState() const
{
    QByteArray array;
    array.resize(sizeof(GLuint));

    char *ptr = array.data();
    reinterpret_cast<GLuint*>(ptr)[0] = textureId();
    return array;
}

/*!
  Sets the window surface's texture to \a data.
 */
void QAhiGLWindowSurface::setPermanentState(const QByteArray &data)
{
    const char *ptr = data.constData();
    d_ptr->texture = reinterpret_cast<const GLuint*>(ptr)[0];
}

/*!
  Returns the paint device being used for this window surface.
 */
QPaintDevice *QAhiGLWindowSurface::paintDevice()
{
    return d_ptr->device;
}

/*!
  Returns the window surface's texture.
 */
GLuint QAhiGLWindowSurface::textureId() const
{
    return d_ptr->texture;
}

/*!
  The \l {QWSServer} {window system} always calls this function
  before any painting operations begin for this window surface.
  It ensures that the correct frame buffer will be used by the
  OpenGL library for painting operations.
 */
//! [4]
void QAhiGLWindowSurface::beginPaint(const QRegion &region)
{
    QWSGLWindowSurface::beginPaint(region);

    if (d_ptr->frameBufferObject)
        glBindFramebufferOES(GL_FRAMEBUFFER_EXT, d_ptr->frameBufferObject);
}
//! [4]

/*!
  This function returns true if the window associated with
  this window surface can still rendered using this window
  surface class. Either the window's top-level widget must
  be an \l {QGLWidget} {OpenGL widget}, or the window's
  frame must be no bigger than 256 x 256.
 */
//! [5]
bool QAhiGLWindowSurface::isValid() const
{
    if (!qobject_cast<QGLWidget*>(window())) {
        const QRect r = window()->frameGeometry();
        if (r.width() > 256 || r.height() > 256)
            return false;
    }
    return true;
}
//! [5]
