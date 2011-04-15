// Commit: acc903853d5ac54d646d324b7386c998bc07d464
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qsgrectangle_p.h"
#include "qsgrectangle_p_p.h"

#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>

#include <QtGui/qpixmapcache.h>
#include <QtCore/qstringbuilder.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

// XXX todo - should we change rectangle to draw entirely within its width/height?

QSGPen::QSGPen(QObject *parent)
: QObject(parent), _width(1), _color("#000000"), _valid(false)
{
}

QColor QSGPen::color() const 
{ 
    return _color; 
}

void QSGPen::setColor(const QColor &c)
{
    _color = c;
    _valid = (_color.alpha() && _width >= 1) ? true : false;
    emit penChanged();
}

int QSGPen::width() const 
{ 
    return _width; 
}

void QSGPen::setWidth(int w)
{
    if (_width == w && _valid)
        return;

    _width = w;
    _valid = (_color.alpha() && _width >= 1) ? true : false;
    emit penChanged();
}

bool QSGPen::isValid() const
{ 
    return _valid; 
}

QSGGradientStop::QSGGradientStop(QObject *parent) 
: QObject(parent) 
{
}

qreal QSGGradientStop::position() const 
{ 
    return m_position; 
}

void QSGGradientStop::setPosition(qreal position) 
{ 
    m_position = position; updateGradient(); 
}

QColor QSGGradientStop::color() const 
{ 
    return m_color; 
}

void QSGGradientStop::setColor(const QColor &color) 
{ 
    m_color = color; updateGradient(); 
}

void QSGGradientStop::updateGradient()
{
    if (QSGGradient *grad = qobject_cast<QSGGradient*>(parent()))
        grad->doUpdate();
}

QSGGradient::QSGGradient(QObject *parent) 
: QObject(parent), m_gradient(0) 
{
}

QSGGradient::~QSGGradient() 
{ 
    delete m_gradient; 
}

QDeclarativeListProperty<QSGGradientStop> QSGGradient::stops() 
{ 
    return QDeclarativeListProperty<QSGGradientStop>(this, m_stops); 
}

const QGradient *QSGGradient::gradient() const
{
    if (!m_gradient && !m_stops.isEmpty()) {
        m_gradient = new QLinearGradient(0,0,0,1.0);
        for (int i = 0; i < m_stops.count(); ++i) {
            const QSGGradientStop *stop = m_stops.at(i);
            m_gradient->setCoordinateMode(QGradient::ObjectBoundingMode);
            m_gradient->setColorAt(stop->position(), stop->color());
        }
    }

    return m_gradient;
}

void QSGGradient::doUpdate()
{
    delete m_gradient;
    m_gradient = 0;
    emit updated();
}

int QSGRectanglePrivate::doUpdateSlotIdx = -1;

QSGRectangle::QSGRectangle(QSGItem *parent)
: QSGItem(*(new QSGRectanglePrivate), parent)
{
    setFlag(ItemHasContents);
}

void QSGRectangle::doUpdate()
{
    Q_D(QSGRectangle);
    const int pw = d->pen && d->pen->isValid() ? d->pen->width() : 0;
    d->setPaintMargin((pw+1)/2);
    update();
}

QSGPen *QSGRectangle::border()
{
    Q_D(QSGRectangle);
    return d->getPen();
}

QSGGradient *QSGRectangle::gradient() const
{
    Q_D(const QSGRectangle);
    return d->gradient;
}

void QSGRectangle::setGradient(QSGGradient *gradient)
{
    Q_D(QSGRectangle);
    if (d->gradient == gradient)
        return;
    static int updatedSignalIdx = -1;
    if (updatedSignalIdx < 0)
        updatedSignalIdx = QSGGradient::staticMetaObject.indexOfSignal("updated()");
    if (d->doUpdateSlotIdx < 0)
        d->doUpdateSlotIdx = QSGRectangle::staticMetaObject.indexOfSlot("doUpdate()");
    if (d->gradient)
        QMetaObject::disconnect(d->gradient, updatedSignalIdx, this, d->doUpdateSlotIdx);
    d->gradient = gradient;
    if (d->gradient)
        QMetaObject::connect(d->gradient, updatedSignalIdx, this, d->doUpdateSlotIdx);
    update();
}

qreal QSGRectangle::radius() const
{
    Q_D(const QSGRectangle);
    return d->radius;
}

void QSGRectangle::setRadius(qreal radius)
{
    Q_D(QSGRectangle);
    if (d->radius == radius)
        return;

    d->radius = radius;
    update();
    emit radiusChanged();
}

QColor QSGRectangle::color() const
{
    Q_D(const QSGRectangle);
    return d->color;
}

void QSGRectangle::setColor(const QColor &c)
{
    Q_D(QSGRectangle);
    if (d->color == c)
        return;

    d->color = c;
    update();
    emit colorChanged();
}

QSGNode *QSGRectangle::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(QSGRectangle);

    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGRectangleNode *rectangle = static_cast<QSGRectangleNode *>(oldNode);
    if (!rectangle) rectangle = d->sceneGraphContext()->createRectangleNode();

    rectangle->setRect(QRectF(0, 0, width(), height()));
    rectangle->setColor(d->color);

    if (d->pen && d->pen->isValid()) {
        rectangle->setPenColor(d->pen->color());
        rectangle->setPenWidth(d->pen->width());
    } else {
        rectangle->setPenColor(QColor());
        rectangle->setPenWidth(0);
    }

    rectangle->setRadius(d->radius);

    QGradientStops stops;
    if (d->gradient) {
        QList<QSGGradientStop *> qxstops = d->gradient->m_stops;
        for (int i = 0; i < qxstops.size(); ++i)
            stops.append(QGradientStop(qxstops.at(i)->position(), qxstops.at(i)->color()));
    }
    rectangle->setGradientStops(stops);

    rectangle->update();

    return rectangle;
}

QRectF QSGRectangle::boundingRect() const
{
    Q_D(const QSGRectangle);
    return QRectF(-d->paintmargin, -d->paintmargin, width()+d->paintmargin*2, height()+d->paintmargin*2);
}

QT_END_NAMESPACE
