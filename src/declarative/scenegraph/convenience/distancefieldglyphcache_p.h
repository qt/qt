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

#ifndef DISTANCEFIELDGLYPHCACHE_H
#define DISTANCEFIELDGLYPHCACHE_H

#include <private/qfont_p.h>
#include <private/qfontengine_p.h>
#include <qsgtexture.h>

QT_BEGIN_NAMESPACE

void qt_disableFontHinting(QFont &font);

class Q_DECLARATIVE_EXPORT DistanceFieldGlyphCache
{
public:
    static DistanceFieldGlyphCache *get(const QFont &font);

    struct Metrics {
        qreal width;
        qreal height;
        qreal baselineX;
        qreal baselineY;
    };
    Metrics glyphMetrics(glyph_t glyph);

    struct TexCoord {
        qreal x;
        qreal y;
        qreal width;
        qreal height;
        qreal xMargin;
        qreal yMargin;
    };
    TexCoord glyphTexCoord(glyph_t glyph);

    QSGTextureRef texture();
    QSize textureSize() const;
    qreal scaleRatioFromRefSize() const;
    QImage renderDistanceFieldGlyph(glyph_t glyph) const;

    int glyphCount() const;
    qreal glyphMargin() const;

    void populate(int count, const glyph_t *glyphs);

    QPointF pixelToTexel(const QPointF &pixel) const;

    static bool distanceFieldEnabled();

private:
    DistanceFieldGlyphCache(const QFont &font);

    QSGTextureRef createTexture();

    static QHash<QString, DistanceFieldGlyphCache *> m_caches;

    QFont m_font;
    QFontEngine *m_fontEngine;
    QFontEngine *m_referenceFontEngine;
    QString m_distanceFieldKey;
    int m_glyphCount;
    QHash<glyph_t, Metrics> m_metrics;

    struct DistanceFieldTextureData {
        QSGTextureRef texture;
        QSize size;
        QHash<glyph_t, TexCoord> texCoords;
        QSet<glyph_t> generatedGlyphs;
    };
    DistanceFieldTextureData *textureData();
    DistanceFieldTextureData *m_textureData;
    static QHash<QString, DistanceFieldTextureData *> m_textures_data;

};

QT_END_NAMESPACE

#endif // DISTANCEFIELDGLYPHCACHE_H
