/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "liveselectionrectangle.h"

#include "../qmlinspectorconstants.h"

#include <QtGui/QPen>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsObject>
#include <QtGui/QGraphicsScene>

#include <QtCore/QtDebug>

#include <cmath>

#ifdef Q_OS_WINCE
#    include <qfunctions_wince.h>
#endif

namespace QmlJSDebugger {

class SelectionRectShape : public QGraphicsRectItem
{
public:
    SelectionRectShape(QGraphicsItem *parent = 0) : QGraphicsRectItem(parent) {}
    int type() const { return Constants::EditorItemType; }
};

LiveSelectionRectangle::LiveSelectionRectangle(QGraphicsObject *layerItem)
    : m_controlShape(new SelectionRectShape(layerItem)),
      m_layerItem(layerItem)
{
    m_controlShape->setPen(QPen(Qt::black));
    m_controlShape->setBrush(QColor(128, 128, 128, 50));
}

LiveSelectionRectangle::~LiveSelectionRectangle()
{
    if (m_layerItem)
        m_layerItem.data()->scene()->removeItem(m_controlShape);
}

void LiveSelectionRectangle::clear()
{
    hide();
}
void LiveSelectionRectangle::show()
{
    m_controlShape->show();
}

void LiveSelectionRectangle::hide()
{
    m_controlShape->hide();
}

QRectF LiveSelectionRectangle::rect() const
{
    return m_controlShape->mapFromScene(m_controlShape->rect()).boundingRect();
}

void LiveSelectionRectangle::setRect(const QPointF &firstPoint,
                                 const QPointF &secondPoint)
{
    double firstX = std::floor(firstPoint.x()) + 0.5;
    double firstY = std::floor(firstPoint.y()) + 0.5;
    double secondX = std::floor(secondPoint.x()) + 0.5;
    double secondY = std::floor(secondPoint.y()) + 0.5;
    QPointF topLeftPoint(firstX < secondX ? firstX : secondX,
                         firstY < secondY ? firstY : secondY);
    QPointF bottomRightPoint(firstX > secondX ? firstX : secondX,
                             firstY > secondY ? firstY : secondY);

    QRectF rect(topLeftPoint, bottomRightPoint);
    m_controlShape->setRect(rect);
}

}
