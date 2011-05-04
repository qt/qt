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

#include <qgl.h>
#include <qrawfont.h>
#include <private/qgl_p.h>
#include <private/qfont_p.h>
#include <private/qfontengine_p.h>

QT_BEGIN_NAMESPACE

class QGLShaderProgram;

class Q_DECLARATIVE_EXPORT QSGDistanceFieldGlyphCache : public QObject
{
    Q_OBJECT
public:
    ~QSGDistanceFieldGlyphCache();

    static QSGDistanceFieldGlyphCache *get(const QGLContext *ctx, const QRawFont &font);

    struct Metrics {
        qreal width;
        qreal height;
        qreal baselineX;
        qreal baselineY;

        bool isNull() const { return width == 0 || height == 0; }
    };
    Metrics glyphMetrics(glyph_t glyph);

    struct TexCoord {
        qreal x;
        qreal y;
        qreal width;
        qreal height;
        qreal xMargin;
        qreal yMargin;

        TexCoord() : x(0), y(0), width(0), height(0), xMargin(0), yMargin(0) { }

        bool isNull() const { return width == 0 || height == 0; }
    };
    TexCoord glyphTexCoord(glyph_t glyph);

    GLuint texture();
    QSize textureSize() const;
    int maxTextureSize() const;
    qreal fontScale() const;
    int distanceFieldRadius() const;
    QImage renderDistanceFieldGlyph(glyph_t glyph) const;

    int glyphCount() const;

    void populate(int count, const glyph_t *glyphs);
    void derefGlyphs(int count, const glyph_t *glyphs);
    void updateCache();

    bool cacheIsFull() const { return m_textureData->currY >= maxTextureSize(); }

    bool useWorkaroundBrokenFBOReadback() const;

    static bool distanceFieldEnabled();

private Q_SLOTS:
    void onContextDestroyed(const QGLContext *context);

private:
    QSGDistanceFieldGlyphCache(const QGLContext *c, const QRawFont &font);

    void createTexture(int width, int height);
    void resizeTexture(int width, int height);

    static QHash<QString, QSGDistanceFieldGlyphCache *> m_caches;

    QRawFont m_font;
    QRawFont m_referenceFont;

    QString m_distanceFieldKey;
    int m_glyphCount;
    QHash<glyph_t, Metrics> m_metrics;
    mutable int m_maxTextureSize;

    struct DistanceFieldTextureData {
        GLuint texture;
        GLuint fbo;
        QSize size;
        QHash<glyph_t, TexCoord> texCoords;
        QSet<glyph_t> pendingGlyphs;
        QHash<glyph_t, quint32> glyphRefCount;
        QSet<glyph_t> unusedGlyphs;
        int currX;
        int currY;
        QImage image;
        bool doubleGlyphResolution;

        DistanceFieldTextureData(const QGLContext *)
            : texture(0)
            , fbo(0)
            , currX(0)
            , currY(0)
            , doubleGlyphResolution(false)
        { }
    };
    DistanceFieldTextureData *textureData();
    DistanceFieldTextureData *m_textureData;
    static QHash<QString, QGLContextGroupResource<DistanceFieldTextureData> > m_textures_data;

    const QGLContext *ctx;
    QGLShaderProgram *m_blitProgram;
    GLfloat m_vertexCoordinateArray[8];
    GLfloat m_textureCoordinateArray[8];

};

QT_END_NAMESPACE

#endif // DISTANCEFIELDGLYPHCACHE_H
