/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "imagewidget.h"

#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    setObjectName("ImageWidget");

    setMinimumSize(QSize(100,100));

    position = 0;
    zoomed = rotated = false;

    zoomedIn = false;
    horizontalOffset = 0;
    verticalOffset = 0;

    grabGesture(Qt::DoubleTapGesture);
    grabGesture(Qt::PanGesture);
    grabGesture(Qt::TapAndHoldGesture);
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    if (currentImage.isNull()) {
        p.fillRect(geometry(), Qt::white);
        return;
    }
    int hoffset = 0;
    int voffset = 0;
    const int w = pixmap.width();
    const int h = pixmap.height();
    p.save();
    if (zoomedIn) {
        hoffset = horizontalOffset;
        voffset = verticalOffset;
        if (horizontalOffset > 0)
            p.fillRect(0, 0, horizontalOffset, height(), Qt::white);
        if (verticalOffset > 0)
            p.fillRect(0, 0, width(), verticalOffset, Qt::white);
    }
    p.drawPixmap(hoffset, voffset, pixmap);
    if (hoffset + w < width())
        p.fillRect(hoffset + w, 0, width() - w - hoffset, height(), Qt::white);
    if (voffset + h < height())
        p.fillRect(0, voffset + h, width(), height() - h - voffset, Qt::white);

    // paint touch feedback
    if (touchFeedback.tapped || touchFeedback.doubleTapped) {
        p.setPen(QPen(Qt::gray, 2));
        p.drawEllipse(touchFeedback.position, 5, 5);
        if (touchFeedback.doubleTapped) {
            p.setPen(QPen(Qt::gray, 2, Qt::DotLine));
            p.drawEllipse(touchFeedback.position, 15, 15);
        } else if (touchFeedback.tapAndHoldState != 0) {
            QPoint pts[8] = {
                touchFeedback.position + QPoint(  0, -15),
                touchFeedback.position + QPoint( 10, -10),
                touchFeedback.position + QPoint( 15,   0),
                touchFeedback.position + QPoint( 10,  10),
                touchFeedback.position + QPoint(  0,  15),
                touchFeedback.position + QPoint(-10,  10),
                touchFeedback.position + QPoint(-15,   0)
            };
            for (int i = 0; i < (touchFeedback.tapAndHoldState-20)/10; ++i)
                p.drawEllipse(pts[i], 3, 3);
        }
    } else if (touchFeedback.sliding) {
        p.setPen(QPen(Qt::red, 3));
        QPoint endPos = QPoint(touchFeedback.position.x(), touchFeedback.slidingStartPosition.y());
        p.drawLine(touchFeedback.slidingStartPosition, endPos);
        int dx = 10;
        if (touchFeedback.slidingStartPosition.x() < endPos.x())
            dx = -1*dx;
        p.drawLine(endPos, endPos + QPoint(dx, 5));
        p.drawLine(endPos, endPos + QPoint(dx, -5));
    }

    for (int i = 0; i < TouchFeedback::MaximumNumberOfTouches; ++i) {
        if (touchFeedback.touches[i].isNull())
            break;
        p.drawEllipse(touchFeedback.touches[i], 10, 10);
    }
    p.restore();
}

bool ImageWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture) {
        gestureEvent(static_cast<QGestureEvent*>(event));
        return true;
    }
    return QWidget::event(event);
}

void ImageWidget::gestureEvent(QGestureEvent *event)
{
    touchFeedback.doubleTapped = false;

    Q_ASSERT(event);
    if (event->contains(Qt::TapGesture)) {
        //
    } else if (const QGesture *g = event->gesture(Qt::DoubleTapGesture)) {
        touchFeedback.doubleTapped = true;
        horizontalOffset = g->hotSpot().x() - currentImage.width()*1.0*g->hotSpot().x()/width();
        verticalOffset = g->hotSpot().y() - currentImage.height()*1.0*g->hotSpot().y()/height();
        setZoomedIn(!zoomedIn);
        zoomed = rotated = false;
        updateImage();
    } else if (const QGesture *g = event->gesture(Qt::PanGesture)) {
        if (zoomedIn) {
            // usual panning
#ifndef QT_NO_CURSOR
            if (g->state() == Qt::GestureStarted)
                setCursor(Qt::SizeAllCursor);
            else
                setCursor(Qt::ArrowCursor);
#endif
            const int dx = g->pos().x() - g->lastPos().x();
            const int dy = g->pos().y() - g->lastPos().y();
            horizontalOffset += dx;
            verticalOffset += dy;
            update();
        } else {
            // only slide gesture should be accepted
            const QPanningGesture *pg = static_cast<const QPanningGesture*>(g);
            if (pg->direction() != pg->lastDirection()) {
                // ###: event->cancel();
            }
            if (g->state() == Qt::GestureFinished) {
                touchFeedback.sliding = false;
                zoomed = rotated = false;
                if (pg->direction() == Qt::RightDirection) {
                    qDebug() << "slide right";
                    goNextImage();
                } else if (pg->direction() == Qt::LeftDirection) {
                    qDebug() << "slide left";
                    goPrevImage();
                }
                updateImage();
            }
        }
    } else if (const QGesture *g = event->gesture(Qt::TapAndHoldGesture)) {
        if (g->state() == Qt::GestureFinished) {
            qDebug() << "tap and hold detected";
            touchFeedback.reset();
            update();

            QMenu menu;
            menu.addAction("Action 1");
            menu.addAction("Action 2");
            menu.addAction("Action 3");
            menu.exec(mapToGlobal(g->hotSpot()));
        }
    } else {
        qDebug() << "unknown gesture";
    }
    feedbackFadeOutTimer.start(500, this);
    event->accept();
}

void ImageWidget::resizeEvent(QResizeEvent*)
{
    updateImage();
}

void ImageWidget::updateImage()
{
    // should use qtconcurrent here?
    transformation = QTransform();
    if (zoomedIn) {
    } else {
        if (currentImage.isNull())
            return;
        if (zoomed) {
            transformation = transformation.scale(zoom, zoom);
        } else {
            double xscale = (double)width()/currentImage.width();
            double yscale = (double)height()/currentImage.height();
            if (xscale < yscale)
                yscale = xscale;
            else
                xscale = yscale;
            transformation = transformation.scale(xscale, yscale);
        }
        if (rotated)
            transformation = transformation.rotate(angle);
    }
    pixmap = QPixmap::fromImage(currentImage).transformed(transformation);
    update();
}

void ImageWidget::openDirectory(const QString &path)
{
    this->path = path;
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png";
    files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);

    position = 0;
    goToImage(0);
    updateImage();
}

QImage ImageWidget::loadImage(const QString &fileName)
{
    QImageReader reader(fileName);
    if (!reader.canRead()) {
        qDebug() << fileName << ": can't load image";
        return QImage();
    }
    QImage image;
    if (!reader.read(&image)) {
        qDebug() << fileName << ": corrupted image";
        return QImage();
    }
    return image;
}

void ImageWidget::setZoomedIn(bool zoomed)
{
    zoomedIn = zoomed;
}

void ImageWidget::goNextImage()
{
    if (files.isEmpty())
        return;
    if (position < files.size()-1) {
        ++position;
        prevImage = currentImage;
        currentImage = nextImage;
        if (position+1 < files.size())
            nextImage = loadImage(path+QLatin1String("/")+files.at(position+1));
        else
            nextImage = QImage();
    }
    setZoomedIn(false);
    updateImage();
}

void ImageWidget::goPrevImage()
{
    if (files.isEmpty())
        return;
    if (position > 0) {
        --position;
        nextImage = currentImage;
        currentImage = prevImage;
        if (position > 0)
            prevImage = loadImage(path+QLatin1String("/")+files.at(position-1));
        else
            prevImage = QImage();
    }
    setZoomedIn(false);
    updateImage();
}

void ImageWidget::goToImage(int index)
{
    if (files.isEmpty())
        return;
    if (index < 0 || index >= files.size()) {
        qDebug() << "goToImage: invalid index: " << index;
        return;
    }
    if (index == position+1) {
        goNextImage();
        return;
    }
    if (position > 0 && index == position-1) {
        goPrevImage();
        return;
    }
    position = index;
    pixmap = QPixmap();
    if (index > 0)
        prevImage = loadImage(path+QLatin1String("/")+files.at(position-1));
    else
        prevImage = QImage();
    currentImage = loadImage(path+QLatin1String("/")+files.at(position));
    if (position+1 < files.size())
        nextImage = loadImage(path+QLatin1String("/")+files.at(position+1));
    else
        nextImage = QImage();
    setZoomedIn(false);
    updateImage();
}

void ImageWidget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == touchFeedback.tapTimer.timerId()) {
        touchFeedback.tapTimer.stop();
    } else if (event->timerId() == feedbackFadeOutTimer.timerId()) {
        feedbackFadeOutTimer.stop();
        touchFeedback.reset();
    }
    update();
}
