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

#ifndef ADAPTATIONINTERFACES_H
#define ADAPTATIONINTERFACES_H

#include "qsgnode.h"
#include "qsgtexture.h"

#include <QtCore/qobject.h>
#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <QtCore/qsharedpointer.h>
#include <QtGui/qglyphs.h>
#include <QtCore/qurl.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QSGNode;
class QImage;
class TextureReference;

// TODO: Rename from XInterface to AbstractX.
class Q_DECLARATIVE_EXPORT QSGRectangleNode : public QSGGeometryNode
{
public:
    virtual void setRect(const QRectF &rect) = 0;
    virtual void setColor(const QColor &color) = 0;
    virtual void setPenColor(const QColor &color) = 0;
    virtual void setPenWidth(qreal width) = 0;
    virtual void setGradientStops(const QGradientStops &stops) = 0;
    virtual void setRadius(qreal radius) = 0;

    virtual void update() = 0;
};


class Q_DECLARATIVE_EXPORT QSGImageNode : public QSGGeometryNode
{
public:
    virtual void setTargetRect(const QRectF &rect) = 0;
    virtual void setSourceRect(const QRectF &rect) = 0;
    virtual void setTexture(QSGTexture *texture) = 0;

    virtual void setMipmapFiltering(QSGTexture::Filtering filtering) = 0;
    virtual void setFiltering(QSGTexture::Filtering filtering) = 0;
    virtual void setHorizontalWrapMode(QSGTexture::WrapMode wrapMode) = 0;
    virtual void setVerticalWrapMode(QSGTexture::WrapMode wrapMode) = 0;

    virtual void update() = 0;
};


class Q_DECLARATIVE_EXPORT QSGGlyphNode : public QSGGeometryNode
{
public:
    enum AntialiasingMode
    {
        GrayAntialiasing,
        SubPixelAntialiasing
    };

    virtual void setGlyphs(const QPointF &position, const QGlyphs &glyphs) = 0;
    virtual void setColor(const QColor &color) = 0;
    virtual QPointF baseLine() const = 0;

    virtual QRectF boundingRect() const { return m_bounding_rect; }
    virtual void setBoundingRect(const QRectF &bounds) { m_bounding_rect = bounds; }

    virtual void setPreferredAntialiasingMode(AntialiasingMode) = 0;

protected:
    QRectF m_bounding_rect;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif
