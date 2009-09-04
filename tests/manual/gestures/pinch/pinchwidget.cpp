/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include "pinchwidget.h"

#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QPanGesture>
#include <QPinchGesture>
#include <QPushButton>

PinchWidget::PinchWidget(const QImage &image, QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(100,100);
    this->image = image;
    pan = new QPanGesture(this);
    connect(pan, SIGNAL(triggered()), this, SLOT(onPanTriggered()));
    connect(pan, SIGNAL(finished()), this, SLOT(onPanFinished()));
    pinch = new QPinchGesture(this);
    connect(pinch, SIGNAL(triggered()), this, SLOT(onPinchTriggered()));
    connect(pinch, SIGNAL(finished()), this, SLOT(onPinchFinished()));
}

QSize PinchWidget::sizeHint() const
{
    return image.size()*1.5;
}

void PinchWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QTransform t = worldTransform * currentPanTransform * currentPinchTransform;
    p.setTransform(t);
    QPoint center = QPoint(width()/2, height()/2);
    QPoint size = QPoint(image.width()/2, image.height()/2);
    p.translate(center - size);
    p.drawImage(QPoint(0,0), image);
}

void PinchWidget::acceptTouchEvents()
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    if (QWidget *w = qobject_cast<QPushButton*>(sender()))
        w->setEnabled(false);
}

void PinchWidget::onPanTriggered()
{
    currentPanTransform = QTransform()
        .translate(pan->totalOffset().width(),
                   pan->totalOffset().height());
    update();
}

void PinchWidget::onPanFinished()
{
    worldTransform *= currentPanTransform;
    currentPanTransform.reset();
    update();
}

void PinchWidget::onPinchTriggered()
{
    QPoint transformCenter = worldTransform.map(QPoint(width()/2, height()/2));
    currentPinchTransform = QTransform()
        .translate(transformCenter.x(), transformCenter.y())
        .scale(pinch->totalScaleFactor(), pinch->totalScaleFactor())
        .rotate(pinch->totalRotationAngle())
        .translate(-transformCenter.x(), -transformCenter.y());
    update();
}

void PinchWidget::onPinchFinished()
{
    worldTransform *= currentPinchTransform;
    currentPinchTransform.reset();
    update();
}
