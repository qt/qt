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
#include "qt7movieviewoutput.h"
#include "qt7playersession.h"
#include <QtCore/qdebug.h>

#include <QuartzCore/CIFilter.h>
#include <QuartzCore/CIVector.h>


#define VIDEO_TRANSPARENT(m) -(void)m:(NSEvent *)e{[[self superview] m:e];}

@interface TransparentQTMovieView : QTMovieView
{
@private
    QRect *m_drawRect;
    qreal m_brightness, m_contrast, m_saturation, m_hue;
}

- (TransparentQTMovieView *) init;
- (void) setDrawRect:(QRect &)rect;
- (CIImage *) view:(QTMovieView *)view willDisplayImage:(CIImage *)img;
- (void) setContrast:(qreal) contrast;
@end

@implementation TransparentQTMovieView

- (TransparentQTMovieView *) init
{
    self = [super initWithFrame:NSZeroRect];
    if (self) {
        [self setControllerVisible:NO];
        [self setContrast:1.0];
        [self setDelegate:self];
    }
    return self;
}

- (void) dealloc
{
    [super dealloc];
}

- (void) setContrast:(qreal) contrast
{
    m_hue = 0.0;
    m_brightness = 0.0;
    m_contrast = contrast;
    m_saturation = 1.0;
}


- (void) setDrawRect:(QRect &)rect
{
    *m_drawRect = rect;

    NSRect nsrect;
    nsrect.origin.x = m_drawRect->x();
    nsrect.origin.y = m_drawRect->y();
    nsrect.size.width = m_drawRect->width();
    nsrect.size.height = m_drawRect->height();
    [self setFrame:nsrect];
}

- (CIImage *) view:(QTMovieView *)view willDisplayImage:(CIImage *)img
{
    // This method is called from QTMovieView just
    // before the image will be drawn.
    Q_UNUSED(view);

    if ( !qFuzzyCompare(m_brightness, 0.0) ||
         !qFuzzyCompare(m_contrast, 1.0) ||
         !qFuzzyCompare(m_saturation, 1.0)){
        CIFilter *colorFilter = [CIFilter filterWithName:@"CIColorControls"];
        [colorFilter setValue:[NSNumber numberWithFloat:m_brightness] forKey:@"inputBrightness"];
        [colorFilter setValue:[NSNumber numberWithFloat:(m_contrast < 1) ? m_contrast : 1 + ((m_contrast-1)*3)] forKey:@"inputContrast"];
        [colorFilter setValue:[NSNumber numberWithFloat:m_saturation] forKey:@"inputSaturation"];
        [colorFilter setValue:img forKey:@"inputImage"];
        img = [colorFilter valueForKey:@"outputImage"];
    }

    /*if (m_hue){
        CIFilter *colorFilter = [CIFilter filterWithName:@"CIHueAdjust"];
        [colorFilter setValue:[NSNumber numberWithFloat:(m_hue * 3.14)] forKey:@"inputAngle"];
        [colorFilter setValue:img forKey:@"inputImage"];
        img = [colorFilter valueForKey:@"outputImage"];
    }*/

    return img;
}


VIDEO_TRANSPARENT(mouseDown);
VIDEO_TRANSPARENT(mouseDragged);
VIDEO_TRANSPARENT(mouseUp);
VIDEO_TRANSPARENT(mouseMoved);
VIDEO_TRANSPARENT(mouseEntered);
VIDEO_TRANSPARENT(mouseExited);
VIDEO_TRANSPARENT(rightMouseDown);
VIDEO_TRANSPARENT(rightMouseDragged);
VIDEO_TRANSPARENT(rightMouseUp);
VIDEO_TRANSPARENT(otherMouseDown);
VIDEO_TRANSPARENT(otherMouseDragged);
VIDEO_TRANSPARENT(otherMouseUp);
VIDEO_TRANSPARENT(keyDown);
VIDEO_TRANSPARENT(keyUp);
VIDEO_TRANSPARENT(scrollWheel)

@end


QT7MovieViewOutput::QT7MovieViewOutput(QObject *parent)
   :QT7VideoWindowControl(parent),
    m_movie(0),
    m_movieView(0),
    m_layouted(false),
    m_winId(0),
    m_fullscreen(false),
    m_aspectRatioMode(Qt::KeepAspectRatio),
    m_brightness(0),
    m_contrast(0),
    m_hue(0),
    m_saturation(0)
{    
}

QT7MovieViewOutput::~QT7MovieViewOutput()
{
    [(QTMovieView*)m_movieView release];
    [(QTMovie*)m_movie release];
}

void QT7MovieViewOutput::setupVideoOutput()
{
    AutoReleasePool pool;

    //qDebug() << "QT7MovieViewOutput::setupVideoOutput" << m_movie << m_winId;
    if (m_movie == 0 || m_winId <= 0)
        return;

    NSSize size = [[(QTMovie*)m_movie attributeForKey:@"QTMovieNaturalSizeAttribute"] sizeValue];
    m_nativeSize = QSize(size.width, size.height);

    if (!m_movieView)
        m_movieView = [[TransparentQTMovieView alloc] init];

    [(QTMovieView*)m_movieView setControllerVisible:NO];
    [(QTMovieView*)m_movieView setMovie:(QTMovie*)m_movie];

    [(NSView *)m_winId addSubview:(QTMovieView*)m_movieView];
    m_layouted = true;

    setDisplayRect(m_displayRect);
}

void QT7MovieViewOutput::setMovie(void *movie)
{
    if (m_movie != movie) {
        if (m_movie) {
            if (m_movieView)
                [(QTMovieView*)m_movieView setMovie:nil];

            [(QTMovie*)m_movie release];
        }

        m_movie = movie;

        if (m_movie)
            [(QTMovie*)m_movie retain];

        setupVideoOutput();
    }
}

void QT7MovieViewOutput::updateNaturalSize(const QSize &newSize)
{
    if (m_nativeSize != newSize) {
        m_nativeSize = newSize;
        emit nativeSizeChanged();
    }
}

WId QT7MovieViewOutput::winId() const
{
    return m_winId;
}

void QT7MovieViewOutput::setWinId(WId id)
{
    if (m_winId != id) {
        if (m_movieView && m_layouted) {
            [(QTMovieView*)m_movieView removeFromSuperview];
            m_layouted = false;
        }

        m_winId = id;
        setupVideoOutput();
    }
}

QRect QT7MovieViewOutput::displayRect() const
{
    return m_displayRect;
}

void QT7MovieViewOutput::setDisplayRect(const QRect &rect)
{
    m_displayRect = rect;

    if (m_movieView) {
        AutoReleasePool pool;
        [(QTMovieView*)m_movieView setPreservesAspectRatio:(m_aspectRatioMode == Qt::KeepAspectRatio ? YES : NO)];
        [(QTMovieView*)m_movieView setFrame:NSMakeRect(m_displayRect.x(),
                                                       m_displayRect.y(),
                                                       m_displayRect.width(),
                                                       m_displayRect.height())];
     }

}

bool QT7MovieViewOutput::isFullScreen() const
{
    return m_fullscreen;
}

void QT7MovieViewOutput::setFullScreen(bool fullScreen)
{
    m_fullscreen = fullScreen;
    setDisplayRect(m_displayRect);
}

void QT7MovieViewOutput::repaint()
{
}

QSize QT7MovieViewOutput::nativeSize() const
{
    return m_nativeSize;
}

Qt::AspectRatioMode QT7MovieViewOutput::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QT7MovieViewOutput::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;
    setDisplayRect(m_displayRect);
}

int QT7MovieViewOutput::brightness() const
{
    return m_brightness;
}

void QT7MovieViewOutput::setBrightness(int brightness)
{
    m_brightness = brightness;
}

int QT7MovieViewOutput::contrast() const
{
    return m_contrast;
}

void QT7MovieViewOutput::setContrast(int contrast)
{
    m_contrast = contrast;
    [(TransparentQTMovieView*)m_movieView setContrast:(contrast/100.0+1.0)];
}

int QT7MovieViewOutput::hue() const
{
    return m_hue;
}

void QT7MovieViewOutput::setHue(int hue)
{
    m_hue = hue;
}

int QT7MovieViewOutput::saturation() const
{
    return m_saturation;
}

void QT7MovieViewOutput::setSaturation(int saturation)
{
    m_saturation = saturation;
}
