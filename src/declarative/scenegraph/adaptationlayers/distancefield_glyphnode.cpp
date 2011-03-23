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

#include "distancefield_glyphnode.h"
#include "distancefield_glyphnode_p.h"
#include "distancefieldglyphcache_p.h"

#include <QtCore/qfileinfo.h>

QT_BEGIN_NAMESPACE

#define QT_DISTANCEFIELD_MARGIN_THRESHOLD 0.31

DistanceFieldGlyphNode::DistanceFieldGlyphNode()
    : m_material(0)
    , m_glyph_cache(0)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 0)
    , m_style(QSGText::Normal)
{
    m_geometry.setDrawingMode(GL_TRIANGLES);
    setGeometry(&m_geometry);
    setFlag(Node::UsePreprocess);
}

DistanceFieldGlyphNode::~DistanceFieldGlyphNode()
{
    delete m_material;
}

void DistanceFieldGlyphNode::setColor(const QColor &color)
{
    m_color = color;
    if (m_material != 0) {
        m_material->setColor(color);
        setMaterial(m_material); // Indicate the material state has changed
    }
}

void DistanceFieldGlyphNode::setGlyphs(const QPointF &position, const QGlyphs &glyphs)
{
    QFontEngine *fe = QFontPrivate::get(glyphs.font())->engineForScript(QUnicodeTables::Common);
    m_position = QPointF(position.x(), position.y() - fe->ascent().toReal());
    m_glyphs = glyphs;

    updateFont();
    updateGeometry();
    updateMaterial();

#ifdef QML_RUNTIME_TESTING
    description = QLatin1String("glyphs");
#endif
}

void DistanceFieldGlyphNode::setStyle(QSGText::TextStyle style)
{
    m_style = style;
}

void DistanceFieldGlyphNode::setStyleColor(const QColor &color)
{
    m_styleColor = color;
}

void DistanceFieldGlyphNode::preprocess()
{
    Q_ASSERT(m_glyph_cache);
    m_glyph_cache->updateCache();
}

void DistanceFieldGlyphNode::updateGeometry()
{
    Q_ASSERT(m_glyph_cache);

    QSGGeometry *g = geometry();
    QRectF boundingRect;

    const QVector<quint32> &glyphIndexes = m_glyphs.glyphIndexes();

    m_glyph_cache->populate(glyphIndexes.count(), glyphIndexes.constData());

    Q_ASSERT(g->indexType() == GL_UNSIGNED_SHORT);
    g->allocate(glyphIndexes.size() * 4, glyphIndexes.size() * 6);
    QVector4D *vp = (QVector4D *)g->vertexData();
    ushort *ip = g->indexDataAsUShort();

    QPointF margins(1.5, 1.5);
    QPointF texMargins = margins / m_glyph_cache->fontScale();

    for (int i = 0; i < glyphIndexes.size(); ++i) {
        quint32 glyphIndex = glyphIndexes.at(i);
        DistanceFieldGlyphCache::Metrics metrics = m_glyph_cache->glyphMetrics(glyphIndex);
        DistanceFieldGlyphCache::TexCoord c = m_glyph_cache->glyphTexCoord(glyphIndex);

        if (metrics.width > 0 && metrics.height > 0) {
            if (m_style != QSGText::Normal) {
                metrics.width += margins.x() * 2;
                metrics.height += margins.y() * 2;
                metrics.baselineX -= margins.x();
                metrics.baselineY += margins.y();
                c.xMargin -= texMargins.x();
                c.yMargin -= texMargins.y();
                c.width += texMargins.x() * 2;
                c.height += texMargins.y() * 2;

            } else if (m_glyph_cache->fontScale() <= QT_DISTANCEFIELD_MARGIN_THRESHOLD) {
                metrics.width += m_glyph_cache->glyphMargin() * m_glyph_cache->fontScale() * 2;
                metrics.height += m_glyph_cache->glyphMargin() * m_glyph_cache->fontScale() * 2;
                metrics.baselineX -= m_glyph_cache->glyphMargin() * m_glyph_cache->fontScale();
                metrics.baselineY += m_glyph_cache->glyphMargin() * m_glyph_cache->fontScale();
                c.xMargin -= m_glyph_cache->glyphMargin();
                c.yMargin -= m_glyph_cache->glyphMargin();
                c.width += m_glyph_cache->glyphMargin() * 2;
                c.height += m_glyph_cache->glyphMargin() * 2;
            }
        }

        QPointF glyphPosition = m_glyphs.positions().at(i) + m_position;
        qreal x = glyphPosition.x() + metrics.baselineX;
        qreal y = glyphPosition.y() - metrics.baselineY;

        boundingRect |= QRectF(x, y, metrics.width, metrics.height);

        float cx1 = x;
        float cx2 = x + metrics.width;
        float cy1 = y;
        float cy2 = y + metrics.height;

        float tx1 = c.x + c.xMargin;
        float tx2 = tx1 + c.width;
        float ty1 = c.y + c.yMargin;
        float ty2 = ty1 + c.height;

        if (m_baseLine.isNull())
            m_baseLine = glyphPosition;

        vp[4 * i + 0] = QVector4D(cx1, cy1, tx1, ty1);
        vp[4 * i + 1] = QVector4D(cx2, cy1, tx2, ty1);
        vp[4 * i + 2] = QVector4D(cx1, cy2, tx1, ty2);
        vp[4 * i + 3] = QVector4D(cx2, cy2, tx2, ty2);

        int o = i * 4;
        ip[6 * i + 0] = o + 0;
        ip[6 * i + 1] = o + 2;
        ip[6 * i + 2] = o + 3;
        ip[6 * i + 3] = o + 3;
        ip[6 * i + 4] = o + 1;
        ip[6 * i + 5] = o + 0;
    }

    setBoundingRect(boundingRect);
    markDirty(DirtyGeometry);
}

void DistanceFieldGlyphNode::updateFont()
{
    m_glyph_cache = DistanceFieldGlyphCache::get(m_glyphs.font());
}

void DistanceFieldGlyphNode::updateMaterial()
{
    delete m_material;

    if (m_style == QSGText::Normal) {
        m_material = new DistanceFieldTextMaterial;
    } else {
        DistanceFieldStyledTextMaterial *material;
        if (m_style == QSGText::Outline) {
            material = new DistanceFieldOutlineTextMaterial;
        } else {
            DistanceFieldShiftedStyleTextMaterial *sMaterial = new DistanceFieldShiftedStyleTextMaterial;
            if (m_style == QSGText::Raised)
                sMaterial->setShift(QPointF(0.0, 1.0));
            else
                sMaterial->setShift(QPointF(0.0, -1.0));
            material = sMaterial;
        }
        material->setStyleColor(m_styleColor);
        m_material = material;
    }

    m_material->setGlyphCache(m_glyph_cache);
    m_material->setColor(m_color);
    setMaterial(m_material);
}

QT_END_NAMESPACE
