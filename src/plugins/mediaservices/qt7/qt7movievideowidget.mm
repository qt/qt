/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#import <QTKit/QTKit.h>

#include "qt7backend.h"

#include "qt7playercontrol.h"
#include "qt7movievideowidget.h"
#include "qt7playersession.h"
#include "qcvdisplaylink.h"
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>

#include <QGLWidget>

#import <QuartzCore/QuartzCore.h>

#include "math.h"

QT_BEGIN_NAMESPACE

class GLVideoWidget : public QGLWidget
{
public:

    GLVideoWidget(QWidget *parent, const QGLFormat &format)
        : QGLWidget(format, parent),
          m_texRef(0),
          m_nativeSize(640,480),
          m_aspectRatioMode(Qt::KeepAspectRatio)
    {
        setAutoFillBackground(false);        
    }

    void initializeGL()
    {
        QColor bgColor = palette().color(QPalette::Background);
        glClearColor(bgColor.redF(), bgColor.greenF(), bgColor.blueF(), bgColor.alphaF());
    }

    void resizeGL(int w, int h)
    {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glViewport(0, 0, GLsizei(w), GLsizei(h));
        gluOrtho2D(0, GLsizei(w), 0, GLsizei(h));
        updateGL();
    }

    void paintGL()
    {
        glClear(GL_COLOR_BUFFER_BIT);
        if (!m_texRef)
            return;

        glPushMatrix();
            glDisable(GL_CULL_FACE);
            GLenum target = CVOpenGLTextureGetTarget(m_texRef);
            glEnable(target);

            glBindTexture(target, CVOpenGLTextureGetName(m_texRef));
            glTexParameterf(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            GLfloat lowerLeft[2], lowerRight[2], upperRight[2], upperLeft[2];
            CVOpenGLTextureGetCleanTexCoords(m_texRef, lowerLeft, lowerRight, upperRight, upperLeft);

            glBegin(GL_QUADS);
                QRect rect = displayRect();
                glTexCoord2f(lowerLeft[0], lowerLeft[1]);
                glVertex2i(rect.topLeft().x(), rect.topLeft().y());
                glTexCoord2f(lowerRight[0], lowerRight[1]);
                glVertex2i(rect.topRight().x() + 1, rect.topRight().y());
                glTexCoord2f(upperRight[0], upperRight[1]);
                glVertex2i(rect.bottomRight().x() + 1, rect.bottomRight().y() + 1);
                glTexCoord2f(upperLeft[0], upperLeft[1]);
                glVertex2i(rect.bottomLeft().x(), rect.bottomLeft().y() + 1);
            glEnd();
        glPopMatrix();
    }

    void setCVTexture(CVOpenGLTextureRef texRef)
    {
        if (m_texRef)
            CVOpenGLTextureRelease(m_texRef);

        m_texRef = texRef;

        if (m_texRef)
            CVOpenGLTextureRetain(m_texRef);

        if (isVisible()) {
            makeCurrent();
            paintGL();
            swapBuffers();
        }
    }

    QSize sizeHint() const
    {
        return m_nativeSize;
    }

    void setNativeSize(const QSize &size)
    {
        m_nativeSize = size;        
    }

    void setAspectRatioMode(Qt::AspectRatioMode mode)
    {
        if (m_aspectRatioMode != mode) {
            m_aspectRatioMode = mode;
            update();
        }
    }

private:
    QRect displayRect() const
    {
        QRect displayRect = rect();

        if (m_aspectRatioMode == Qt::KeepAspectRatio) {
            QSize size = m_nativeSize;
            size.scale(displayRect.size(), Qt::KeepAspectRatio);

            displayRect = QRect(QPoint(0, 0), size);
            displayRect.moveCenter(rect().center());
        }
        return displayRect;
    }

    CVOpenGLTextureRef m_texRef;
    QSize m_nativeSize;
    Qt::AspectRatioMode m_aspectRatioMode;
};

QT7MovieVideoWidget::QT7MovieVideoWidget(QObject *parent)
   :QT7VideoWidgetControl(parent),
    m_movie(0),    
    m_videoWidget(0),    
    m_fullscreen(false),
    m_aspectRatioMode(Qt::KeepAspectRatio),
    m_brightness(0),
    m_contrast(0),
    m_hue(0),
    m_saturation(0)
{
//    qDebug() << "QT7MovieVideoWidget";

    QGLFormat format = QGLFormat::defaultFormat();
    format.setSwapInterval(1); // Vertical sync (avoid tearing)
    m_videoWidget = new GLVideoWidget(0, format);

    m_displayLink = new QCvDisplayLink(this);

    connect(m_displayLink, SIGNAL(tick(CVTimeStamp)), SLOT(updateVideoFrame(CVTimeStamp)));

    if (!createVisualContext()) {
        qWarning() << "QT7MovieVideoWidget: failed to create visual context";
    }
}

bool QT7MovieVideoWidget::createVisualContext()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    m_videoWidget->makeCurrent();

    AutoReleasePool pool;
    CGLContextObj cglContext = CGLGetCurrentContext();
    NSOpenGLPixelFormat *nsglPixelFormat = [NSOpenGLView defaultPixelFormat];
    CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>([nsglPixelFormat CGLPixelFormatObj]);

    CFTypeRef keys[] = { kQTVisualContextOutputColorSpaceKey };
    CGColorSpaceRef colorSpace = NULL;
    CMProfileRef sysprof = NULL;

    // Get the Systems Profile for the main display
    if (CMGetSystemProfile(&sysprof) == noErr) {
        // Create a colorspace with the systems profile
        colorSpace = CGColorSpaceCreateWithPlatformColorSpace(sysprof);        
        CMCloseProfile(sysprof);
    }

    if (!colorSpace)
        colorSpace = CGColorSpaceCreateDeviceRGB();

    CFDictionaryRef textureContextAttributes = CFDictionaryCreate(kCFAllocatorDefault,
                                                                  (const void **)keys,
                                                                  (const void **)&colorSpace, 1,
                                                                  &kCFTypeDictionaryKeyCallBacks,
                                                                  &kCFTypeDictionaryValueCallBacks);

    OSStatus err = QTOpenGLTextureContextCreate(kCFAllocatorDefault,
                                                cglContext,
                                                cglPixelFormat,
                                                textureContextAttributes,
                                                &m_visualContext);
    if (err != noErr)
        qWarning() << "Could not create visual context (OpenGL)";


    return (err == noErr);
#endif // QUICKTIME_C_API_AVAILABLE

    return false;
}

QT7MovieVideoWidget::~QT7MovieVideoWidget()
{
    m_displayLink->stop();
    [(QTMovie*)m_movie release];
    delete m_videoWidget;
}

QWidget *QT7MovieVideoWidget::videoWidget()
{
    return m_videoWidget;
}

void QT7MovieVideoWidget::setupVideoOutput()
{
    AutoReleasePool pool;

//    qDebug() << "QT7MovieVideoWidget::setupVideoOutput" << m_movie;

    if (m_movie == 0) {
        m_displayLink->stop();
        return;
    }

    NSSize size = [[(QTMovie*)m_movie attributeForKey:@"QTMovieNaturalSizeAttribute"] sizeValue];
    m_nativeSize = QSize(size.width, size.height);
    m_videoWidget->setNativeSize(m_nativeSize);

#ifdef QUICKTIME_C_API_AVAILABLE
    // targets a Movie to render into a visual context
    SetMovieVisualContext([(QTMovie*)m_movie quickTimeMovie], m_visualContext);
#endif

    m_displayLink->start();
}

void QT7MovieVideoWidget::setMovie(void *movie)
{
    if (m_movie == movie)
        return;

    if (m_movie) {
#ifdef QUICKTIME_C_API_AVAILABLE
        SetMovieVisualContext([(QTMovie*)m_movie quickTimeMovie], nil);
#endif
        [(QTMovie*)m_movie release];
    }

    m_movie = movie;
    [(QTMovie*)m_movie retain];

    setupVideoOutput();
}

void QT7MovieVideoWidget::updateNaturalSize(const QSize &newSize)
{
    if (m_nativeSize != newSize) {
        m_nativeSize = newSize;
        setupVideoOutput();
    }
}

bool QT7MovieVideoWidget::isFullScreen() const
{
    return m_fullscreen;
}

void QT7MovieVideoWidget::setFullScreen(bool fullScreen)
{
    m_fullscreen = fullScreen;    
}

QSize QT7MovieVideoWidget::nativeSize() const
{
    return m_nativeSize;
}

Qt::AspectRatioMode QT7MovieVideoWidget::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QT7MovieVideoWidget::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;
    m_videoWidget->setAspectRatioMode(mode);    
}

int QT7MovieVideoWidget::brightness() const
{
    return m_brightness;
}

void QT7MovieVideoWidget::setBrightness(int brightness)
{
    m_brightness = brightness;
    updateColors();
}

int QT7MovieVideoWidget::contrast() const
{
    return m_contrast;
}

void QT7MovieVideoWidget::setContrast(int contrast)
{
    m_contrast = contrast;
    updateColors();
}

int QT7MovieVideoWidget::hue() const
{
    return m_hue;
}

void QT7MovieVideoWidget::setHue(int hue)
{
    m_hue = hue;
    updateColors();
}

int QT7MovieVideoWidget::saturation() const
{
    return m_saturation;
}

void QT7MovieVideoWidget::setSaturation(int saturation)
{
    m_saturation = saturation;
    updateColors();
}

void QT7MovieVideoWidget::updateColors()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    if (m_movie) {
        QTMovie *movie = (QTMovie*)m_movie;

        Float32 value;
        value = m_brightness/100.0;
        SetMovieVisualBrightness([movie quickTimeMovie], value, 0);
        value = pow(2, m_contrast/50.0);
        SetMovieVisualContrast([movie quickTimeMovie], value, 0);
        value = m_hue/100.0;
        SetMovieVisualHue([movie quickTimeMovie], value, 0);
        value = 1.0+m_saturation/100.0;
        SetMovieVisualSaturation([movie quickTimeMovie], value, 0);
    }
#endif
}

void QT7MovieVideoWidget::updateVideoFrame(const CVTimeStamp &ts)
{
#ifdef QUICKTIME_C_API_AVAILABLE
    AutoReleasePool pool;    
    // check for new frame
    if (m_visualContext && QTVisualContextIsNewImageAvailable(m_visualContext, &ts)) {
        CVOpenGLTextureRef currentFrame = NULL;

        // get a "frame" (image buffer) from the Visual Context, indexed by the provided time
        OSStatus status = QTVisualContextCopyImageForTime(m_visualContext, NULL, &ts, &currentFrame);

        // the above call may produce a null frame so check for this first
        // if we have a frame, then draw it
        if (status == noErr && currentFrame) {
            //qDebug() << "render video frame";
            m_videoWidget->setCVTexture(currentFrame);
            CVOpenGLTextureRelease(currentFrame);
        }
        QTVisualContextTask(m_visualContext);
    }
#else
    Q_UNUSED(ts);
#endif
}

#include "moc_qt7movievideowidget.cpp"

QT_END_NAMESPACE
