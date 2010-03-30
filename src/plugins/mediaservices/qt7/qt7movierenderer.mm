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
#include "qt7movierenderer.h"
#include "qt7playersession.h"
#include "qt7ciimagevideobuffer.h"
#include "qcvdisplaylink.h"
#include <QtCore/qdebug.h>
#include <QtCore/qcoreapplication.h>

#include <QGLWidget>

#include <QtMultimedia/qabstractvideobuffer.h>
#include <QtMultimedia/qabstractvideosurface.h>
#include <QtMultimedia/qvideosurfaceformat.h>

QT_BEGIN_NAMESPACE

//#define USE_MAIN_MONITOR_COLOR_SPACE 1

class CVGLTextureVideoBuffer : public QAbstractVideoBuffer
{
public:
    CVGLTextureVideoBuffer(CVOpenGLTextureRef buffer)
        : QAbstractVideoBuffer(GLTextureHandle)
        , m_buffer(buffer)
        , m_mode(NotMapped)
    {        
        CVOpenGLTextureRetain(m_buffer);
    }

    virtual ~CVGLTextureVideoBuffer()
    {
        CVOpenGLTextureRelease(m_buffer);
    }

    QVariant handle() const
    {
        GLuint id = CVOpenGLTextureGetName(m_buffer);
        return QVariant(int(id));
    }

    MapMode mapMode() const { return m_mode; }

    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine)
    {
        if (numBytes)
            *numBytes = 0;

        if (bytesPerLine)
            *bytesPerLine = 0;

        m_mode = mode;
        return 0;
    }

    void unmap() { m_mode = NotMapped; }

private:
    CVOpenGLTextureRef m_buffer;
    MapMode m_mode;
};


class CVPixelBufferVideoBuffer : public QAbstractVideoBuffer
{
public:
    CVPixelBufferVideoBuffer(CVPixelBufferRef buffer)
        : QAbstractVideoBuffer(NoHandle)
        , m_buffer(buffer)
        , m_mode(NotMapped)
    {
        CVPixelBufferRetain(m_buffer);
    }

    virtual ~CVPixelBufferVideoBuffer()
    {
        CVPixelBufferRelease(m_buffer);
    }

    MapMode mapMode() const { return m_mode; }

    uchar *map(MapMode mode, int *numBytes, int *bytesPerLine)
    {
        if (mode != NotMapped && m_mode == NotMapped) {
            CVPixelBufferLockBaseAddress(m_buffer, 0);

            if (numBytes)
                *numBytes = CVPixelBufferGetDataSize(m_buffer);

            if (bytesPerLine)
                *bytesPerLine = CVPixelBufferGetBytesPerRow(m_buffer);

            m_mode = mode;

            return (uchar*)CVPixelBufferGetBaseAddress(m_buffer);
        } else {
            return 0;
        }
    }

    void unmap()
    {
        if (m_mode != NotMapped) {
            m_mode = NotMapped;
            CVPixelBufferUnlockBaseAddress(m_buffer, 0);
        }
    }

private:
    CVPixelBufferRef m_buffer;
    MapMode m_mode;
};



QT7MovieRenderer::QT7MovieRenderer(QObject *parent)
   :QT7VideoRendererControl(parent),
    m_movie(0),
#ifdef QUICKTIME_C_API_AVAILABLE
    m_visualContext(0),
    m_usingGLContext(false),
    m_currentGLContext(0),
#endif
    m_surface(0)
{
//    qDebug() << "QT7MovieRenderer";

    m_displayLink = new QCvDisplayLink(this);
    connect(m_displayLink, SIGNAL(tick(CVTimeStamp)), SLOT(updateVideoFrame(CVTimeStamp)));
}


bool QT7MovieRenderer::createGLVisualContext()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    AutoReleasePool pool;
    CGLContextObj cglContext = CGLGetCurrentContext();
    NSOpenGLPixelFormat *nsglPixelFormat = [NSOpenGLView defaultPixelFormat];
    CGLPixelFormatObj cglPixelFormat = static_cast<CGLPixelFormatObj>([nsglPixelFormat CGLPixelFormatObj]);

    OSStatus err = QTOpenGLTextureContextCreate(kCFAllocatorDefault, cglContext,
                                                cglPixelFormat, NULL, &m_visualContext);
    if (err != noErr)
        qWarning() << "Could not create visual context (OpenGL)";

    return (err == noErr);
#endif // QUICKTIME_C_API_AVAILABLE

    return false;
}

#ifdef QUICKTIME_C_API_AVAILABLE
static bool DictionarySetValue(CFMutableDictionaryRef dict, CFStringRef key, SInt32 value)
{
    CFNumberRef  number    = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &value);

    if (number) {
        CFDictionarySetValue( dict, key, number );
        CFRelease( number );
        return true;
    }
    return false;
}
#endif // QUICKTIME_C_API_AVAILABLE

bool QT7MovieRenderer::createPixelBufferVisualContext()
{
#ifdef QUICKTIME_C_API_AVAILABLE
    if (m_visualContext) {
        QTVisualContextRelease(m_visualContext);
        m_visualContext = 0;
    }

    m_pixelBufferContextGeometry = m_nativeSize;

    CFMutableDictionaryRef  pixelBufferOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                           &kCFTypeDictionaryKeyCallBacks,
                                                                           &kCFTypeDictionaryValueCallBacks);
    //DictionarySetValue(pixelBufferOptions, kCVPixelBufferPixelFormatTypeKey, k32ARGBPixelFormat );
    DictionarySetValue(pixelBufferOptions, kCVPixelBufferPixelFormatTypeKey, k32BGRAPixelFormat );
    DictionarySetValue(pixelBufferOptions, kCVPixelBufferWidthKey, m_nativeSize.width() );
    DictionarySetValue(pixelBufferOptions, kCVPixelBufferHeightKey, m_nativeSize.height() );
    DictionarySetValue(pixelBufferOptions, kCVPixelBufferBytesPerRowAlignmentKey, 16);
    //CFDictionarySetValue(pixelBufferOptions, kCVPixelBufferOpenGLCompatibilityKey, kCFBooleanTrue);

    CFMutableDictionaryRef  visualContextOptions = CFDictionaryCreateMutable(kCFAllocatorDefault, 0,
                                                                             &kCFTypeDictionaryKeyCallBacks,
                                                                             &kCFTypeDictionaryValueCallBacks);
    CFDictionarySetValue(visualContextOptions, kQTVisualContextPixelBufferAttributesKey, pixelBufferOptions);

    CGColorSpaceRef colorSpace = NULL;

#if USE_MAIN_MONITOR_COLOR_SPACE
    CMProfileRef sysprof = NULL;

    // Get the Systems Profile for the main display
    if (CMGetSystemProfile(&sysprof) == noErr) {
        // Create a colorspace with the systems profile
        colorSpace = CGColorSpaceCreateWithPlatformColorSpace(sysprof);        
        CMCloseProfile(sysprof);
    }
#endif

    if (!colorSpace)
        colorSpace = CGColorSpaceCreateDeviceRGB();

    CFDictionarySetValue(visualContextOptions, kQTVisualContextOutputColorSpaceKey, colorSpace);

    OSStatus err = QTPixelBufferContextCreate(kCFAllocatorDefault,
                                               visualContextOptions,
                                               &m_visualContext);
    CFRelease(pixelBufferOptions);
    CFRelease(visualContextOptions);

    if (err != noErr) {
        qWarning() << "Could not create visual context (PixelBuffer)";
        return false;
    }

    return true;
#endif // QUICKTIME_C_API_AVAILABLE

    return false;
}


QT7MovieRenderer::~QT7MovieRenderer()
{
    m_displayLink->stop();
}

void QT7MovieRenderer::setupVideoOutput()
{
    AutoReleasePool pool;

//    qDebug() << "QT7MovieRenderer::setupVideoOutput" << m_movie;

    if (m_movie == 0 || m_surface == 0) {
        m_displayLink->stop();
        return;
    }

    NSSize size = [[(QTMovie*)m_movie attributeForKey:@"QTMovieNaturalSizeAttribute"] sizeValue];
    m_nativeSize = QSize(size.width, size.height);

#ifdef QUICKTIME_C_API_AVAILABLE
    bool usedGLContext = m_usingGLContext;

    if (!m_nativeSize.isEmpty()) {

        bool glSupported = !m_surface->supportedPixelFormats(QAbstractVideoBuffer::GLTextureHandle).isEmpty();

        //Try rendering using opengl textures first:
        if (glSupported) {
            QVideoSurfaceFormat format(m_nativeSize, QVideoFrame::Format_RGB32, QAbstractVideoBuffer::GLTextureHandle);

            if (m_surface->isActive())
                m_surface->stop();

//            qDebug() << "Starting the surface with format" << format;
            if (!m_surface->start(format)) {
//                qDebug() << "failed to start video surface" << m_surface->error();
                glSupported = false;
            } else {
                m_usingGLContext = true;
            }

        }

        if (!glSupported) {
            m_usingGLContext = false;
            QVideoSurfaceFormat format(m_nativeSize, QVideoFrame::Format_RGB32);

            if (m_surface->isActive() && m_surface->surfaceFormat() != format) {
//                qDebug() << "Surface format was changed, stop the surface.";
                m_surface->stop();
            }

            if (!m_surface->isActive()) {
//                qDebug() << "Starting the surface with format" << format;
                m_surface->start(format);
//                if (!m_surface->start(format))
//                    qDebug() << "failed to start video surface" << m_surface->error();
            }
        }
    }


    if (m_visualContext) {
        //check if the visual context still can be reused
        if (usedGLContext != m_usingGLContext ||
            (m_usingGLContext && (m_currentGLContext != QGLContext::currentContext())) ||
            (!m_usingGLContext && (m_pixelBufferContextGeometry != m_nativeSize))) {
            QTVisualContextRelease(m_visualContext);
            m_pixelBufferContextGeometry = QSize();
            m_visualContext = 0;
        }
    }

    if (!m_nativeSize.isEmpty()) {
        if (!m_visualContext) {
            if (m_usingGLContext) {
//                qDebug() << "Building OpenGL visual context" << m_nativeSize;
                m_currentGLContext = QGLContext::currentContext();
                if (!createGLVisualContext()) {
                    qWarning() << "QT7MovieRenderer: failed to create visual context";
                    return;
                }
            } else {
//                qDebug() << "Building Pixel Buffer visual context" << m_nativeSize;
                if (!createPixelBufferVisualContext()) {
                    qWarning() << "QT7MovieRenderer: failed to create visual context";
                    return;
                }
            }
        }

        // targets a Movie to render into a visual context
        SetMovieVisualContext([(QTMovie*)m_movie quickTimeMovie], m_visualContext);

        m_displayLink->start();
    }
#endif

}

void QT7MovieRenderer::setMovie(void *movie)
{
//    qDebug() << "QT7MovieRenderer::setMovie" << movie;

#ifdef QUICKTIME_C_API_AVAILABLE
    QMutexLocker locker(&m_mutex);

    if (m_movie != movie) {
        if (m_movie) {
            //ensure the old movie doesn't hold the visual context, otherwise it can't be reused
            SetMovieVisualContext([(QTMovie*)m_movie quickTimeMovie], nil);
            [(QTMovie*)m_movie release];
        }

        m_movie = movie;
        [(QTMovie*)m_movie retain];

        setupVideoOutput();
    }
#endif
}

void QT7MovieRenderer::updateNaturalSize(const QSize &newSize)
{
    if (m_nativeSize != newSize) {
        m_nativeSize = newSize;
        setupVideoOutput();
    }
}

QAbstractVideoSurface *QT7MovieRenderer::surface() const
{
    return m_surface;
}

void QT7MovieRenderer::setSurface(QAbstractVideoSurface *surface)
{
//    qDebug() << "Set video surface" << surface;

    if (surface == m_surface)
        return;

    QMutexLocker locker(&m_mutex);

    if (m_surface && m_surface->isActive())
        m_surface->stop();

    m_surface = surface;
    setupVideoOutput();
}


QSize QT7MovieRenderer::nativeSize() const
{
    return m_nativeSize;
}

void QT7MovieRenderer::updateVideoFrame(const CVTimeStamp &ts)
{
#ifdef QUICKTIME_C_API_AVAILABLE

    QMutexLocker locker(&m_mutex);

    if (m_surface && m_surface->isActive() &&
        m_visualContext && QTVisualContextIsNewImageAvailable(m_visualContext, &ts)) {

        CVImageBufferRef imageBuffer = NULL;

        OSStatus status = QTVisualContextCopyImageForTime(m_visualContext, NULL, &ts, &imageBuffer);

        if (status == noErr && imageBuffer) {
            QAbstractVideoBuffer *buffer = 0;

            if (m_usingGLContext) {
                buffer = new QT7CIImageVideoBuffer([CIImage imageWithCVImageBuffer:imageBuffer]);
                CVOpenGLTextureRelease((CVOpenGLTextureRef)imageBuffer);
            } else {
                buffer = new CVPixelBufferVideoBuffer((CVPixelBufferRef)imageBuffer);
                //buffer = new QT7CIImageVideoBuffer( [CIImage imageWithCVImageBuffer:imageBuffer] );
                CVPixelBufferRelease((CVPixelBufferRef)imageBuffer);
            }

            QVideoFrame frame(buffer, m_nativeSize, QVideoFrame::Format_RGB32);
            m_surface->present(frame);
            QTVisualContextTask(m_visualContext);
        }
    }
#else
    Q_UNUSED(ts);
#endif
}

#include "moc_qt7movierenderer.cpp"

QT_END_NAMESPACE
