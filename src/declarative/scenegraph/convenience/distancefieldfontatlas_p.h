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

#ifndef DISTANCEFIELDFONTATLAS_H
#define DISTANCEFIELDFONTATLAS_H

#include <private/qfont_p.h>
#include <private/qfontengine_p.h>
#include <QtDeclarative/qsgtexturemanager.h>

void qt_disableFontHinting(QFont &font);

struct TexCoordCacheKey {
    QString distfield;
    glyph_t glyph;

    TexCoordCacheKey(const QString &df, glyph_t g) : distfield(df), glyph(g) { }

    bool operator==(const TexCoordCacheKey &other) const {
        return other.distfield == distfield && other.glyph == glyph;
    }
};

uint qHash(const TexCoordCacheKey &key);

class Q_DECLARATIVE_EXPORT DistanceFieldFontAtlas
{
public:
    static DistanceFieldFontAtlas *get(const QFont &font);

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
    QSize atlasSize() const;
    qreal scaleRatioFromRefSize() const;
    QImage renderDistanceFieldGlyph(glyph_t glyph) const;

    int glyphCount() const;
    qreal glyphMargin() const;

    void populate(int count, const glyph_t *glyphs);

    QPointF pixelToTexel(const QPointF &pixel) const;

    static bool distanceFieldEnabled();

private:
    DistanceFieldFontAtlas(const QFont &font);

    QSGTextureRef createTexture();

    static QHash<QString, DistanceFieldFontAtlas *> m_atlases;

    QFont m_font;
    QFontEngine *m_fontEngine;
    QFontEngine *m_referenceFontEngine;
    QString m_distanceFieldKey;
    int m_glyphCount;
    mutable QSize m_size;

    QHash<glyph_t, Metrics> m_metrics;
    static QHash<TexCoordCacheKey, DistanceFieldFontAtlas::TexCoord> m_texCoords;
    static QSet<TexCoordCacheKey> m_generatedGlyphs;

    static QHash<QString, QSGTextureRef> m_textures;
};

#endif // DISTANCEFIELDFONTATLAS_H
