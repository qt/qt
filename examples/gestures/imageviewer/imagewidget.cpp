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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "imagewidget.h"

#include <QtGui>

ImageWidget::ImageWidget(QWidget *parent)
    : QWidget(parent),
    position(0),
    horizontalOffset(0),
    verticalOffset(0),
    rotationAngle(0),
    scaleFactor(1)

{
    setObjectName("ImageWidget");
    setMinimumSize(QSize(100,100));

    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    QGesture *panGesture = new QPanGesture(this);
    connect(panGesture, SIGNAL(started()), this, SLOT(panTriggered()));
    connect(panGesture, SIGNAL(finished()), this, SLOT(panTriggered()));
    connect(panGesture, SIGNAL(cancelled()), this, SLOT(panTriggered()));
    connect(panGesture, SIGNAL(triggered()), this, SLOT(panTriggered()));

    QGesture *pinchGesture = new QPinchGesture(this);
    connect(pinchGesture, SIGNAL(started()), this, SLOT(pinchTriggered()));
    connect(pinchGesture, SIGNAL(finished()), this, SLOT(pinchTriggered()));
    connect(pinchGesture, SIGNAL(cancelled()), this, SLOT(pinchTriggered()));
    connect(pinchGesture, SIGNAL(triggered()), this, SLOT(pinchTriggered()));

    QGesture *swipeGesture = new QSwipeGesture(this);
    connect(swipeGesture, SIGNAL(triggered()), this, SLOT(swipeTriggered()));
}

void ImageWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.fillRect(rect(), Qt::white);

    float iw = currentImage.width();
    float ih = currentImage.height();
    float wh = height();
    float ww = width();

    p.translate(ww/2, wh/2);
    p.translate(horizontalOffset, verticalOffset);
    p.rotate(rotationAngle);
    p.scale(scaleFactor, scaleFactor);
    p.translate(-iw/2, -ih/2);
    p.drawImage(0, 0, currentImage);
}

void ImageWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    rotationAngle = 0;
    scaleFactor = 1;
    verticalOffset = 0;
    horizontalOffset = 0;
    update();
}

void ImageWidget::panTriggered()
{
    QPanGesture *pg = qobject_cast<QPanGesture*>(sender());
#ifndef QT_NO_CURSOR
    switch (pg->state()) {
        case Qt::GestureStarted:
        case Qt::GestureUpdated:
            setCursor(Qt::SizeAllCursor);
            break;
        default:
            setCursor(Qt::ArrowCursor);
    }
#endif
    horizontalOffset += pg->lastOffset().width();
    verticalOffset += pg->lastOffset().height();
    update();
}

void ImageWidget::pinchTriggered()
{
    QPinchGesture *pg = qobject_cast<QPinchGesture*>(sender());
    rotationAngle += pg->rotationAngle();
    scaleFactor += pg->scaleFactor();
    update();
}

void ImageWidget::swipeTriggered()
{
    QSwipeGesture *pg = qobject_cast<QSwipeGesture*>(sender());
    qDebug() << (int) pg->horizontalDirection();
    qDebug() << pg->swipeAngle();

    if (pg->horizontalDirection() == QSwipeGesture::Left
            || pg->verticalDirection() == QSwipeGesture::Up)
        goPrevImage();
    else
        goNextImage();
    update();
}

void ImageWidget::resizeEvent(QResizeEvent*)
{
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
    update();
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
    update();
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
    update();
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

    if (index > 0)
        prevImage = loadImage(path+QLatin1String("/")+files.at(position-1));
    else
        prevImage = QImage();
    currentImage = loadImage(path+QLatin1String("/")+files.at(position));
    if (position+1 < files.size())
        nextImage = loadImage(path+QLatin1String("/")+files.at(position+1));
    else
        nextImage = QImage();
    update();
}

#include "moc_imagewidget.cpp"
