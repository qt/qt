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

#include "qscreenahigl_qws.h"
#include "qwindowsurface_ahigl_p.h"

#include <QWSServer>
#include <QMap>
#include <QTimer>
#include <QTimeLine>

#include <qwindowsystem_qws.h>
#include <private/qwindowsurface_qws_p.h>
#include <private/qfixed_p.h>

#include <GLES/egl.h>
#include <GLES/gl.h>
#include <math.h>

const int animationLength = 1500;
const int frameSpan = 20;

static GLuint createTexture(const QImage &img);

class QAhiGLCursor : public QScreenCursor
{
public:
    QAhiGLCursor() : texture(0) {}
    ~QAhiGLCursor();

    void set(const QImage &image, int hotx, int hoty);

    GLuint texture;
};

QAhiGLCursor::~QAhiGLCursor()
{
    if (texture)
        glDeleteTextures(1, &texture);
}

void QAhiGLCursor::set(const QImage &image, int hotx, int hoty)
{
    if (texture)
        glDeleteTextures(1, &texture);

    if (image.isNull())
        texture = 0;
    else
        texture = createTexture(image.convertToFormat(QImage::Format_ARGB32));

    QScreenCursor::set(image, hotx, hoty);
}


/*!
  \class QAhiGLScreenPrivate
  The QAhiGLScreenPrivate class contains state information for class QAhiGLScreen.

  An instance of this class points to the owning instance of
  class QAhiGLScreen. This class uses a QTimer to limit the
  update frequency.
 */
//! [0]
class QAhiGLScreenPrivate : public QObject
{
    Q_OBJECT

public:
    QAhiGLScreenPrivate(QAhiGLScreen *s);

public slots:
    void windowEvent(QWSWindow *w, QWSServer::WindowEvent e);
    void redrawScreen();

public:
    QAhiGLScreen *screen;
    QAhiGLCursor *cursor;

    EGLContext eglContext;
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;

    QTimer updateTimer;
    bool doEffects;
};
//! [0]

//! [1]
class ShowAnimation : public QTimeLine
{
public:
    ShowAnimation(QAhiGLScreenPrivate *screen);
    qreal valueForTime(int msec);
};
//! [1]

//! [2]
struct WindowInfo
{
    WindowInfo() : texture(0), animation(0) {}

    GLuint texture;
    QPointer<ShowAnimation> animation;
};

static QMap<QWSWindow*, WindowInfo*> windowMap;
//! [2]

/*!
  Constructs the animation for the transition effect used
  when the window associated with \a screen is displayed.
 */
//! [3]
ShowAnimation::ShowAnimation(QAhiGLScreenPrivate *screen)
    : QTimeLine(animationLength)
{
    setUpdateInterval(frameSpan);
    connect(this, SIGNAL(valueChanged(qreal)), screen, SLOT(redrawScreen()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    start();
}
//! [3]

//! [4]
qreal ShowAnimation::valueForTime(int msec)
{
    const qreal t = msec / qreal(duration());
    return 3*t*t - 2*t*t*t;
}
//! [4]

QAhiGLScreenPrivate::QAhiGLScreenPrivate(QAhiGLScreen *s)
    : screen(s), cursor(0), doEffects(false)
{
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(redrawScreen()));
}

/*!
  This slot handles the \a event when the \l {QWSServer}
  {window server} emits a window event for the specified
  \a window.

  The \l {QWSServer::WindowEvent} {window events} handled
  are \c Create, \c Destroy, and \c Show. The \c Create
  event creates a new instance of \l {WindowInfo} and stores
  it in a window map to mark the creation of a new window.
  The \c Destroy event causes the \l {WindoInfo} instance
  to be removed from the map and destroyed.

  The \c Show event is the most interesting. If the user
  has started the application with -display ahigl:effects,
  then the \c Show event is handled by creating a small
  \l {ShowAnimation} {animation} for use when the window
  is first shown.
 */
//! [5]
void QAhiGLScreenPrivate::windowEvent(QWSWindow *window,
                                      QWSServer::WindowEvent event)
{
    switch (event) {
    case QWSServer::Create:
        windowMap[window] = new WindowInfo;
        break;
    case QWSServer::Show:
        if (doEffects)
            windowMap[window]->animation = new ShowAnimation(this);
        break;
    case QWSServer::Destroy:
        delete windowMap[window];
        windowMap.remove(window);
        break;
    default:
        break;
    }
}
//! [5]

/*!
  This function assumes the updateTimer is still counting down and stops it
  and then calls redrawScreen() in the public screen driver class QAhiGLScreen.
 */
//! [6]
void QAhiGLScreenPrivate::redrawScreen()
{
    updateTimer.stop();
    screen->redrawScreen();
}
//! [6]

/*!
  \class QAhiGLScreen

  \brief The QAhiGLScreen class is the screen driver for the ATI handheld device interface.

  QAhiGLScreen is implemented with the d-pointer pattern. That is,
  the only data member the class contains is a pointer called d_ptr,
  which means data pointer. It points to an instance of a private
  class called QAhiGLScreenPrivate, where all the screen driver's
  context data members are defined. The d-pointer pattern is used
  so that changes can be made to the screen driver's context data
  members without destroying the binary compatibility of the public
  screen driver class.

  The pure virtual functions found in the base class QScreen are
  listed below. All must have implementations in any screen driver
  class derived from QScreen. All are impemented in this example,
  except for setMode(), which has only been given a stub
  implementation to satisfy the compiler.

  bool connect(const QString & displaySpec);
  void disconnect();
  bool initDevice();
  void setMode(int width, int height, int depth);

  The stub implementation of setMode() is not meant to indicate
  setMode() can be ignored in your own screen driver class. It was
  simply decided not to provide a fully implemented screen driver
  class for the example, which would normally be tailored to your
  device's specific requirements.

  The base class QGLScreen has only one pure virtual function,
  hasOpenGL(), which must return true if your screen driver class
  supports OpenGL.

  QWSWindowSurface * createSurface(const QString & key) const
  QWSWindowSurface * createSurface(QWidget * widget) const
  void exposeRegion(QRegion region, int windowIndex)

 */

/*!
  Constructs a new, ATI handheld device screen driver.

  The displayId identifies the QWS server to connect to.
 */
QAhiGLScreen::QAhiGLScreen(int displayId) : QGLScreen(displayId)
{
    d_ptr = new QAhiGLScreenPrivate(this);
    d_ptr->eglDisplay = EGL_NO_DISPLAY;
    d_ptr->eglSurface = EGL_NO_SURFACE;
}

/*!
  Destroys this ATI handheld device screen driver.
 */
QAhiGLScreen::~QAhiGLScreen()
{
    delete d_ptr;
}

/*!
  \reimp
 */
//! [7]
bool QAhiGLScreen::connect(const QString &displaySpec)
{
    // Hardcoded values for this device
    w = 480;
    h = 640;
    dw = w;
    dh = h;
    d = 16;

    const int dpi = 120;
    physWidth = qRound(dw * 25.4 / dpi);
    physHeight = qRound(dh * 25.4 / dpi);

    if (displaySpec.section(':', 1, 1).contains("effects"))
        d_ptr->doEffects = true;

    return true;
}
//! [7]

/*!
  \reimp
 */
//! [8]
bool QAhiGLScreen::initDevice()
{
    EGLint version, subversion;
    EGLint attrs[] = { EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                       EGL_STENCIL_SIZE, 8, EGL_DEPTH_SIZE, 16,
                       EGL_NONE };
    EGLint numConfig;
    EGLConfig eglConfig;

    d_ptr->eglDisplay = eglGetDisplay(0);
    if (d_ptr->eglDisplay == EGL_NO_DISPLAY) {
        qCritical("QAhiGLScreen::initDevice(): eglGetDisplay failed: 0x%x",
                  eglGetError());
        return false;
    }

    if (!eglInitialize(d_ptr->eglDisplay, &version, &subversion)) {
        qCritical("QAhiGLScreen::initDevice(): eglInitialize failed: 0x%x",
                  eglGetError());
        return false;
    }

    if (!eglChooseConfig(d_ptr->eglDisplay, attrs, &eglConfig, 1, &numConfig)) {
        qCritical("QAhiGLScreen::initDevice(): eglChooseConfig failed: 0x%x",
                  eglGetError());
        return false;
    }

    static DummyScreen win = { w, h };
    d_ptr->eglSurface = eglCreateWindowSurface(d_ptr->eglDisplay, eglConfig,
                                               &win, 0);
    if (d_ptr->eglSurface == EGL_NO_SURFACE) {
        qCritical("QAhiGLScreen::initDevice(): eglCreateWindowSurface failed: 0x%x",
                  eglGetError());
        return false;
    }

    d_ptr->eglContext = eglCreateContext(d_ptr->eglDisplay, eglConfig,
                                         EGL_NO_CONTEXT, 0);
    if (d_ptr->eglContext == EGL_NO_CONTEXT) {
        qCritical("QAhiGLScreen::initDevice(): eglCreateContext failed: 0x%x",
                  eglGetError());
        return false;
    }

    if (!eglMakeCurrent(d_ptr->eglDisplay, d_ptr->eglSurface, d_ptr->eglSurface, d_ptr->eglContext)) {
        qCritical("QAhiGLScreen::initDevice(): eglMakeCurrent failed: 0x%x",
                  eglGetError());
        return false;
    }

    d_ptr->connect(QWSServer::instance(),
                   SIGNAL(windowEvent(QWSWindow*, QWSServer::WindowEvent)),
                   SLOT(windowEvent(QWSWindow*, QWSServer::WindowEvent)));

    d_ptr->cursor = new QAhiGLCursor;
    qt_screencursor = d_ptr->cursor;

    return true;
}
//! [8]

/*!
  \reimp
 */
//! [9]
void QAhiGLScreen::shutdownDevice()
{
    delete d_ptr->cursor;
    d_ptr->cursor = 0;
    qt_screencursor = 0;

    eglMakeCurrent(EGL_NO_DISPLAY, EGL_NO_SURFACE,
                   EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(d_ptr->eglDisplay, d_ptr->eglContext);
    eglDestroySurface(d_ptr->eglDisplay, d_ptr->eglSurface);
    eglTerminate(d_ptr->eglDisplay);
}
//! [9]

/*!
  \reimp

  In this case, the reimplimentation does nothing. It is
  required because the function is declared as pure virtual
  in the base class QScreen.
 */
void QAhiGLScreen::disconnect()
{
}

/*
  This internal function rounds up to the next power of
  two. If v is already a power of two, that same value is
  returned.
 */
inline static uint nextPowerOfTwo(uint v)
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

/*
  This internal function creates a texture from the image img
  and returns its texture identifier.

  The term "texture" is a graphics technology term that refers
  to a pixmap constructed from an image by adding extra points
  of contrast to the otherwise plain color image. The texture
  has a, well, texture, that the original image doesn't have.
 */
static GLuint createTexture(const QImage &img)
{
    if (img.isNull())
        return 0;

    int width = img.width();
    int height = img.height();
    int textureWidth;
    int textureHeight;
    GLuint texture;

    glGenTextures(1, &texture);
    textureWidth = nextPowerOfTwo(width);
    textureHeight = nextPowerOfTwo(height);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    switch (img.format()) {
    case QImage::Format_RGB16:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
                     textureWidth,
                     textureHeight, 0,
                     GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0);

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RGB, GL_UNSIGNED_SHORT_5_6_5, img.bits());
        break;

    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     textureWidth,
                     textureHeight, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
                        GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
        break;

    default:
        break;
    }

    return texture;
}

/*
  A helper function used by QAhiGLScreen::drawQuad().
 */
static void drawQuad_helper(GLshort *coords, GLfloat *texCoords)
{
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_SHORT, 0, coords);
    glEnable(GL_TEXTURE_2D);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

/*
  A helper function used by QAhiGLScreen::drawQuadWavyFlag().
 */
static void drawQuad_helper(GLshort *coords, GLfloat *texCoords,
                            int arraySize, int numArrays)
{
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_SHORT, 0, coords);

    for (int i = 0; i < numArrays-1; ++i)
        glDrawArrays(GL_TRIANGLE_STRIP, i*arraySize, arraySize);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

/*
  A convenience function used by QAhiGLScreen::drawQuad().
 */
static void setRectCoords(GLshort *coords, QRect rect)
{
    coords[0] = GLshort(rect.left());
    coords[1] = GLshort(rect.top());

    coords[2] = GLshort(rect.right());
    coords[3] = GLshort(rect.top());

    coords[4] = GLshort(rect.right());
    coords[5] = GLshort(rect.bottom());

    coords[6] = GLshort(rect.left());
    coords[7] = GLshort(rect.bottom());
}

/*!
  A helper function used by QAhiGLScreen::drawWindow() and
  QAhiGLScreen::redrawScreen().
 */
void QAhiGLScreen::drawQuad(const QRect &textureGeometry,
                            const QRect &subGeometry,
                            const QRect &screenGeometry)
{
    qreal textureWidth = qreal(nextPowerOfTwo(textureGeometry.width()));
    qreal textureHeight = qreal(nextPowerOfTwo(textureGeometry.height()));

    GLshort coords[8];
    setRectCoords(coords, screenGeometry);

    GLfloat texcoords[8];
    texcoords[0] = (subGeometry.left() - textureGeometry.left()) / textureWidth;
    texcoords[1] = (subGeometry.top() - textureGeometry.top()) / textureHeight;

    texcoords[2] = (subGeometry.right() - textureGeometry.left()) / textureWidth;
    texcoords[3] = (subGeometry.top() - textureGeometry.top()) / textureHeight;

    texcoords[4] = (subGeometry.right() - textureGeometry.left()) / textureWidth;
    texcoords[5] = (subGeometry.bottom() - textureGeometry.top()) / textureHeight;

    texcoords[6] = (subGeometry.left() - textureGeometry.left()) / textureWidth;
    texcoords[7] = (subGeometry.bottom() - textureGeometry.top()) / textureHeight;

    drawQuad_helper(coords, texcoords);
}

/*
  My own sine function.
 */
static qreal mySin(QFixed radians)
{
    const QFixed twoPI = QFixed::fromReal(2*M_PI);

    const int tableSize = 40;
    static int *table = 0;

    if (!table) {
        table = new int[tableSize];
        for (int i = 0; i < tableSize; ++i) {
            table[i] = qRound(sin(M_PI*(i*360.0/40.0)/180.0) * 16776960.0);
        }
    }

    QFixed tableLookup = radians*tableSize/twoPI;
    return table[tableLookup.truncate()%tableSize]/16776960.0;
}

/*
  My own cosine function.
 */
static qreal myCos(QFixed radians)
{
    const int twoPI = qRound(2*M_PI);

    const int tableSize = 40;
    static int *table = 0;

    if (!table) {
        table = new int[tableSize];
        for (int i = 0; i < tableSize; ++i) {
            table[i] = int(cos(M_PI*(i*360.0/40.0)/180.0) * 16776960.0);
        }
    }

    QFixed tableLookup = radians*tableSize/twoPI;
    return table[tableLookup.truncate()%tableSize]/16776960.0;
}

// number of grid cells in wavy flag tesselation in x- and y-direction
const int subdivisions = 10;

/*
  A helper function used by drawQuadWavyFlag(). It computes
  coordinates for grid cells for a wavy flag tesselation and
  stores them in the array called coords.
 */
static void setFlagCoords(GLshort *coords,
			  QRectF screenGeometry,
                          int frameNum,
			  qreal progress)
{
    int coordIndex = 0;
    qreal waveHeight = 30.0*(1.0-progress);
    for (int j = 0; j < subdivisions-1; ++j) {
        for (int i = 0; i < subdivisions; ++i) {
            qreal c;
            c = screenGeometry.left()
                + (i * screenGeometry.width() / (subdivisions - 1))
                + waveHeight * qRound(mySin(QFixed::fromReal(M_PI * 20 * (frameNum + i) / 180.0)))
                + waveHeight * qRound(myCos(QFixed::fromReal(M_PI * 20 * (frameNum + j) / 180.0)));
            coords[coordIndex++] = qRound(c);
            c = screenGeometry.top()
                + (j * screenGeometry.height() / (subdivisions - 1))
                + waveHeight * mySin(QFixed::fromReal(M_PI * 20 * (frameNum + i) / 180.0))
                + waveHeight * myCos(QFixed::fromReal(M_PI * 20 * (frameNum + j) / 180.0));
            coords[coordIndex++] = qRound(c);
            c = screenGeometry.left() + (i * screenGeometry.width() / (subdivisions - 1))
                + waveHeight * mySin(QFixed::fromReal(M_PI * 20 * (frameNum + i) / 180.0))
                + waveHeight * myCos(QFixed::fromReal(M_PI * 20 * (frameNum + (j+1)) / 180.0));
            coords[coordIndex++] = qRound(c);

            c = screenGeometry.top()
                + ((j + 1) * screenGeometry.height() / (subdivisions - 1))
                + waveHeight * mySin(QFixed::fromReal(M_PI * 20 * (frameNum + i) / 180.0))
                + waveHeight * myCos(QFixed::fromReal(M_PI * 20 * (frameNum + (j + 1)) / 180.0));
            coords[coordIndex++] = qRound(c);
        }
    }
}

static void setFlagTexCoords(GLfloat *texcoords,
                             const QRectF &subTexGeometry,
                             const QRectF &textureGeometry,
                             int textureWidth, int textureHeight)
{
    qreal topLeftX = (subTexGeometry.left() - textureGeometry.left())/textureWidth;
    qreal topLeftY = (textureGeometry.height() - (subTexGeometry.top() - textureGeometry.top()))/textureHeight;

    qreal width = (subTexGeometry.right() - textureGeometry.left())/textureWidth - topLeftX;
    qreal height = (textureGeometry.height() - (subTexGeometry.bottom() - textureGeometry.top()))/textureHeight - topLeftY;

    int coordIndex = 0;
    qreal spacing = subdivisions - 1;
    for (int j = 0; j < subdivisions-1; ++j) {
        for (int i = 0; i < subdivisions; ++i) {
            texcoords[coordIndex++] = topLeftX + (i*width) / spacing;
            texcoords[coordIndex++] = topLeftY + (j*height) / spacing;
            texcoords[coordIndex++] = topLeftX + (i*width) / spacing;
            texcoords[coordIndex++] = topLeftY + ((j+1)*height) / spacing;
        }
    }
}

void QAhiGLScreen::drawQuadWavyFlag(const QRect &textureGeometry,
                                    const QRect &subTexGeometry,
                                    const QRect &screenGeometry,
                                    qreal progress)
{
    const int textureWidth = nextPowerOfTwo(textureGeometry.width());
    const int textureHeight = nextPowerOfTwo(textureGeometry.height());

    static int frameNum = 0;

    GLshort coords[subdivisions*subdivisions*2*2];
    setFlagCoords(coords, screenGeometry, frameNum++, progress);

    GLfloat texcoords[subdivisions*subdivisions*2*2];
    setFlagTexCoords(texcoords, subTexGeometry, textureGeometry,
                     textureWidth, textureHeight);

    drawQuad_helper(coords, texcoords, subdivisions*2, subdivisions);
}

void QAhiGLScreen::invalidateTexture(int windowIndex)
{
    if (windowIndex < 0)
        return;

    QList<QWSWindow*> windows = QWSServer::instance()->clientWindows();
    if (windowIndex > windows.size() - 1)
        return;

    QWSWindow *win = windows.at(windowIndex);
    if (!win)
        return;

    WindowInfo *info = windowMap[win];
    if (info->texture) {
        glDeleteTextures(1, &info->texture);
        info->texture = 0;
    }
}

void QAhiGLScreen::drawWindow(QWSWindow *win, qreal progress)
{
    const QRect screenRect = win->allocatedRegion().boundingRect();
    QRect drawRect = screenRect;

    glColor4f(1.0, 1.0, 1.0, progress);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    QWSWindowSurface *surface = win->windowSurface();
    if (!surface)
        return;

    if (progress >= 1.0) {
        if (surface->key() == QLatin1String("ahigl")) {
            drawRect.setCoords(drawRect.left(), drawRect.bottom(),
                               drawRect.right(), drawRect.top());
        }
        drawQuad(win->requestedRegion().boundingRect(), screenRect, drawRect);
        return;
    }

    const int dx = qRound((1 - progress) * drawRect.width() / 2);
    const int dy = qRound((1 - progress) * drawRect.height() / 2);

    drawRect.adjust(dx, dy, -dx, -dy);

    if (surface->key() != QLatin1String("ahigl")) {
        drawRect.setCoords(drawRect.left(), drawRect.bottom(),
                           drawRect.right(), drawRect.top());
    }

    drawQuadWavyFlag(win->requestedRegion().boundingRect(), screenRect,
                     drawRect, progress);
}

/*!
  The window compositions are constructed here.
 */
//! [10]
void QAhiGLScreen::redrawScreen()
{
    glBindFramebufferOES(GL_FRAMEBUFFER_EXT, 0);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, w, h, 0, -999999, 999999);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Fill background color

    QColor bgColor = QWSServer::instance()->backgroundBrush().color();
    glClearColor(bgColor.redF(), bgColor.greenF(),
                 bgColor.blueF(), bgColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT);
//! [10]

    // Draw all windows

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_ALPHA_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

//! [11]
    QList<QWSWindow*> windows = QWSServer::instance()->clientWindows();
    for (int i = windows.size() - 1; i >= 0; --i) {
        QWSWindow *win = windows.at(i);
        QWSWindowSurface *surface = win->windowSurface();
        if (!surface)
            continue;

        WindowInfo *info = windowMap[win];

        if (!info->texture) {
            if (surface->key() == QLatin1String("ahigl"))
                info->texture = static_cast<QAhiGLWindowSurface*>(surface)->textureId();
            else
                info->texture = createTexture(surface->image());
        }
        qreal progress;
        if (info->animation)
            progress = info->animation->currentValue();
        else
            progress = 1.0;

        glBindTexture(GL_TEXTURE_2D, info->texture);
        drawWindow(win, progress);
    } // for i
//! [11] //! [12]

    // Draw cursor

    const QAhiGLCursor *cursor = d_ptr->cursor;
    if (cursor->texture) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, d_ptr->cursor->texture);
        drawQuad(cursor->boundingRect(), cursor->boundingRect(),
                 cursor->boundingRect());
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    eglSwapBuffers(d_ptr->eglDisplay, d_ptr->eglSurface);
}
//! [12]

/*!
  \reimp

 */
//! [13]
void QAhiGLScreen::exposeRegion(QRegion r, int windowIndex)
{
    if ((r & region()).isEmpty())
        return;

    invalidateTexture(windowIndex);

    if (!d_ptr->updateTimer.isActive())
        d_ptr->updateTimer.start(frameSpan);
}
//! [13]

/*!
  \reimp

  This overloading of createSurface() is called on the client side to
  create a window surface for a new window. If the \a widget is a
  QGLWidget, or if the widget's width and height are both less than or
  equal to 256, it creates an instance of QAhiGLWindowSurface.
  Otherwise, it calls QScreen::createSurface() to create a non-OpenGL
  window surface. The pointer to the new window surface is returned.

  Note that this function first asks whether this application is the
  server, and it only creates an instance of QAhiGLWindowSurface if
  the answer is yes. What this means is we only let the server have
  access to the OpenGL hardware, because of an implementation
  restyriction in the OpenGL libraries we are using. Thus only clients
  that are in the server process get to create OpenGL window surfaces.
 */
//! [14]
QWSWindowSurface* QAhiGLScreen::createSurface(QWidget *widget) const
{
    if (QApplication::type() == QApplication::GuiServer) {
        if (qobject_cast<QGLWidget*>(widget)) {
            return new QAhiGLWindowSurface(widget,
					   d_ptr->eglDisplay,
                                           d_ptr->eglSurface,
                                           d_ptr->eglContext);
        }

        const QRect rect = widget->frameGeometry();
        if (rect.width() <= 256 && rect.height() <= 256) {
            return new QAhiGLWindowSurface(widget,
					   d_ptr->eglDisplay,
                                           d_ptr->eglSurface,
                                           d_ptr->eglContext);
        }
    }

    return QScreen::createSurface(widget);
}
//! [14]

/*!
  \reimp

  This overloading of createSurface() is called on the server side
  to manage a window surface corresponding to a window surface
  already created on the client side.

  If the \a key is "ahigl", create an instance of QAhiGLWindowSurface
  and return it. Otherwise, call QScreen::createSurface() and return
  the window surface it creates.

  See QScreen::createSurface().
 */
//! [15]
QWSWindowSurface* QAhiGLScreen::createSurface(const QString &key) const
{
    if (key == QLatin1String("ahigl")) {
        return new QAhiGLWindowSurface(d_ptr->eglDisplay,
				       d_ptr->eglSurface,
                                       d_ptr->eglContext);
    }

    return QScreen::createSurface(key);
}
//! [15]

/*!
  This function would normally reset the frame buffer resolution
  according to \a width, \a height, and the bit \a depth. It would
  then notify other applications that their frame buffer size had
  changed so they could redraw. The function is a no-op in this
  example, which means the example screen driver can't change its
  frame buffer resolution. There is no significance to that in the
  example. You would normally implement setMode() in an OpenGL
  screen driver. This no-op reimplementation is required here
  because setMode() in QScreen is a pure virtual function.

  See QScreen::setMode()
 */
void QAhiGLScreen::setMode(int width, int height, int depth)
{
    Q_UNUSED(width);
    Q_UNUSED(height);
    Q_UNUSED(depth);
}

/*!
  This function would normally be reimplemented to prevent the
  screen driver from updating the screen if \a on is true. It is a
  no-op in this example, which means the screen driver can always
  update the screen.

  See QScreen::blank().
 */
void QAhiGLScreen::blank(bool on)
{
    Q_UNUSED(on);
}

/*!
  This function always returns true, since the purpose of
  this screen driver class is to implement an OpenGL ES
  screen driver. In some other class designed to handle both
  OpenGL and non-OpenGL graphics, it might test the system to
  determine whether OpenGL graphics are supported and return
  true or false accordingly.
 */
bool QAhiGLScreen::hasOpenGL()
{
    return true;
}

#include "qscreenahigl_qws.moc"
