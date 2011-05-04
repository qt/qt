/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef DEFAULT_RECTANGLENODE_H
#define DEFAULT_RECTANGLENODE_H

#include <private/qsgadaptationlayer_p.h>

#include "qsgflatcolormaterial.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGMaterial;
class QSGContext;

class QSGDefaultRectangleNode : public QSGRectangleNode
{
public:
    QSGDefaultRectangleNode(QSGContext *context);
    ~QSGDefaultRectangleNode();

    virtual void setRect(const QRectF &rect);
    virtual void setColor(const QColor &color);
    virtual void setPenColor(const QColor &color);
    virtual void setPenWidth(qreal width);
    virtual void setGradientStops(const QGradientStops &stops);
    virtual void setRadius(qreal radius);
    virtual void update();

private:
    enum {
        TypeFlat,
        TypeVertexGradient
    };
    QSGGeometryNode *border();

    void updateGeometry();
    void updateGradientTexture();

    QSGGeometryNode *m_border;
    QSGFlatColorMaterial m_border_material;
    QSGFlatColorMaterial m_fill_material;

    QRectF m_rect;
    QGradientStops m_gradient_stops;
    qreal m_radius;
    int m_pen_width;

    uint m_gradient_is_opaque : 1;
    uint m_dirty_geometry : 1;

    uint m_material_type : 2; // Only goes up to 3

    QSGGeometry m_default_geometry;

    QSGContext *m_context;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
