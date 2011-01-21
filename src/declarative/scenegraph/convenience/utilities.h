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

#ifndef UTILITIES_H
#define UTILITIES_H

#include "areaallocator.h"
#include "adaptationlayer.h"

#include <QtCore/QSharedPointer>
#include <QtGui/QColor>

class TextMaskMaterial;
class FlatColorMaterial;
class VertexColorMaterial;
class Geometry;
class Renderer;

class QPixmap;
class QImage;
class QSize;
class QFontEngine;
class QRectF;

//class SubTexture2D
//{
//public:
//    SubTexture2D(TextureAtlasInterface *atlas, const QRect &allocatedRect);
//    SubTexture2D(const QGLTexture2DConstPtr &texture);
//    ~SubTexture2D();

//    bool hasOwnTexture() const { return m_source.right() == 1; }
//    bool isValid() const { return hasOwnTexture() || !m_allocated.isNull(); }
//    QRectF sourceRect() const { return m_source; }
//    QGLTexture2DConstPtr texture() const { return m_texture; }
//private:
//    QRect m_allocated;
//    QRectF m_source;
//    TextureAtlasInterface *m_atlas;
//    QGLTexture2DConstPtr m_texture;
//};

//typedef QSharedPointer<const SubTexture2D> SubTexture2DConstPtr;
//typedef QSharedPointer<SubTexture2D> SubTexture2DPtr;

class Q_DECLARATIVE_EXPORT Utilities
{
public:
//    static SubTexture2DPtr getTextureForImage(const QImage &image, bool clampToEdge, bool dynamic = true);
//    static SubTexture2DPtr getTextureForPixmap(const QPixmap &pixmap, bool clampToEdge, bool dynamic = true);

    static void setupRectGeometry(Geometry *geometry, const QRectF &rect, const QSize &textureSize = QSize(), const QRectF &sourceRect = QRectF());
    static QVector<QGLAttributeDescription> &getRectGeometryDescription();
    static QVector<QGLAttributeDescription> &getTexturedRectGeometryDescription();
    static QVector<QGLAttributeDescription> &getColoredRectGeometryDescription();
    static Geometry *createRectGeometry(const QRectF &rect);
    static Geometry *createTexturedRectGeometry(const QRectF &rect, const QSize &textureSize, const QRectF &sourceRect);

    static QColor mix(const QColor &c1, const QColor c2, qreal factor) {
        qreal inv = 1 - factor;
        return QColor::fromRgbF(c1.redF() * factor + c2.redF() * inv,
                                c1.greenF() * factor + c2.greenF() * inv,
                                c1.blueF() * factor + c2.blueF() * inv,
                                c1.alphaF() * factor + c2.alphaF() * inv);
    }
};


#endif // UTILITIES_H
