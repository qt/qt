/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "quikitwindowsurface.h"
#include "quikitwindow.h"

#include <QtDebug>

@implementation QImageView

- (void)dealloc
{
    CGImageRelease(m_cgImage);
    [super dealloc];
}

- (void)setImage:(QImage *)image
{
    CGImageRelease(m_cgImage);

    const uchar *imageData = image->bits();
    int bitDepth = image->depth();
    int colorBufferSize = 8;
    int bytesPrLine = image->bytesPerLine();
    int width = image->width();
    int height = image->height();

    CGColorSpaceRef cgColourSpaceRef = CGColorSpaceCreateDeviceRGB();

    CGDataProviderRef cgDataProviderRef = CGDataProviderCreateWithData(
                NULL,
                imageData,
                image->byteCount(),
                NULL);

    m_cgImage = CGImageCreate(width,
                              height,
                              colorBufferSize,
                              bitDepth,
                              bytesPrLine,
                              cgColourSpaceRef,
                              kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little,
                              cgDataProviderRef,
                              NULL,
                              false,
                              kCGRenderingIntentDefault);

    CGDataProviderRelease(cgDataProviderRef);
    CGColorSpaceRelease(cgColourSpaceRef);
}

- (void)setWidget:(QWidget *)widget
{
    m_widget = widget;
}

- (void)drawRect:(CGRect)rect
{

    if (!m_cgImage)
        return;

    CGContextRef cgContext = UIGraphicsGetCurrentContext();
    CGContextSaveGState( cgContext );

    int dy = rect.origin.y + CGRectGetMaxY(rect);
    CGContextTranslateCTM(cgContext, 0, dy);
    CGContextScaleCTM(cgContext, 1, -1);
    CGImageRef subImage = CGImageCreateWithImageInRect(m_cgImage, rect);
    CGContextDrawImage(cgContext,rect,subImage);
    CGImageRelease(subImage);

    CGContextRestoreGState(cgContext);
}

//- (QList<struct QWindowSystemInterface::TouchPoint>)touchPointsForTouches:(NSSet *)touches
//{
//    NSEnumerator *enumerator = [touches objectEnumerator];
//    id touch;
//    int count = 0;
//    QList<struct QWindowSystemInterface::TouchPoint> result;
//    while ((touch = [enumerator nextObject])) {
//        CGPoint locationInView = [touch locationInView:self];
//        CGRect bounds = [self bounds];
//        struct QWindowSystemInterface::TouchPoint p;
//        p.id = count;
//        p.isPrimary = true;
//        p.normalPosition = QPointF(locationInView.x / bounds.size.width,
//                                   locationInView.y / bounds.size.height);
//        p.area = QRectF(locationInView.x, locationInView.y, 1, 1);
//        p.pressure = 1.;
//        switch ([touch phase]) {
//        case UITouchPhaseBegan:
//            p.state = Qt::TouchPointPressed;
//            break;
//        case UITouchPhaseMoved:
//            p.state = Qt::TouchPointMoved;
//            break;
//        case UITouchPhaseStationary:
//            p.state = Qt::TouchPointStationary;
//            break;
//        case UITouchPhaseEnded:
//        case UITouchPhaseCancelled:
//            p.state = Qt::TouchPointReleased;
//            break;
//        }
//        result << p;
//        qDebug() << p.id << ":" << p.normalPosition << p.area << p.state;
//        ++count;
//    }
//    qDebug() << result.size();
//    return result;
//}

- (void)sendMouseEventForTouches:(NSSet *)touches withEvent:(UIEvent *)event fakeButtons:(Qt::MouseButtons)buttons
{
    UITouch *touch = [touches anyObject];
    CGPoint locationInView = [touch locationInView:self];
    QPoint p(locationInView.x, locationInView.y);
    // TODO handle global touch point? for status bar?
    QWindowSystemInterface::handleMouseEvent(m_widget, (ulong)(event.timestamp*1000),
        p, p, buttons);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    //QWindowSystemInterface::handleTouchEvent(m_widget, (ulong)(event.timestamp*1000),
    //                                         QEvent::TouchBegin, QTouchEvent::TouchScreen,
    //                                         [self touchPointsForTouches:touches]);
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::LeftButton];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    //QWindowSystemInterface::handleTouchEvent(m_widget, (ulong)(event.timestamp*1000),
    //                                         QEvent::TouchUpdate, QTouchEvent::TouchScreen,
    //                                         [self touchPointsForTouches:touches]);
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::LeftButton];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    //QWindowSystemInterface::handleTouchEvent(m_widget, (ulong)(event.timestamp*1000),
    //                                         QEvent::TouchEnd, QTouchEvent::TouchScreen,
    //                                         [self touchPointsForTouches:touches]);
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::NoButton];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    //QWindowSystemInterface::handleTouchEvent(m_widget, (ulong)(event.timestamp*1000),
    //                                         QEvent::TouchEnd, QTouchEvent::TouchScreen,
    //                                         [self touchPointsForTouches:touches]);
    [self sendMouseEventForTouches:touches withEvent:event fakeButtons:Qt::NoButton];
}

@end

QT_BEGIN_NAMESPACE

QUIKitWindowSurface::QUIKitWindowSurface(QWidget *window)
    : QWindowSurface(window)
{
    QUIKitWindow *platformWindow = static_cast<QUIKitWindow *>(window->platformWindow());
    Q_ASSERT(platformWindow);
    const QRect geo = window->geometry();
    mView = [[QImageView alloc] initWithFrame:CGRectMake(geo.x(),geo.y(),geo.width(),geo.height())];
    [mView setMultipleTouchEnabled:YES];
    [mView setWidget:window];
    mImage = new QImage(window->size(),QImage::Format_ARGB32_Premultiplied);
    [mView setImage:mImage];
    if (platformWindow->nativeWindow()) {
        [platformWindow->nativeWindow() addSubview:mView];
        [mView setNeedsDisplay];
    }
}

QPaintDevice *QUIKitWindowSurface::paintDevice()
{
    return mImage;
}

void QUIKitWindowSurface::flush(QWidget *widget, const QRegion &region, const QPoint &offset)
{
    Q_UNUSED(widget);
    Q_UNUSED(offset);

    QRect geo = region.boundingRect();
    [mView setNeedsDisplayInRect:CGRectMake(geo.x(),geo.y(),geo.width(),geo.height())];
}

void QUIKitWindowSurface::resize (const QSize &size)
{
    QWindowSurface::resize(size);

    delete mImage;
    mImage = new QImage(size,QImage::Format_ARGB32_Premultiplied);
    [mView setImage:mImage];
    const QRect geo = geometry();
    [mView setFrame:CGRectMake(geo.x(), geo.y(), size.width(), size.height())];
    [mView setNeedsDisplay];
}

QT_END_NAMESPACE
