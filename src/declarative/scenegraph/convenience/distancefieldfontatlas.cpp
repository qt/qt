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

#include "distancefieldfontatlas_p.h"

#include <qapplication.h>
#include <qfileinfo.h>
#include <qmath.h>

#if defined(Q_WS_X11) || defined(Q_WS_QWS) || (defined(Q_WS_S60) && !defined(QT_NO_FREETYPE))
#  define USE_FREETYPE_ENGINE
#  include <private/qfontengine_ft_p.h>
#elif defined(Q_WS_S60)
#  define USE_S60_ENGINE
#  include <private/qfontengine_s60_p.h>
#elif defined(Q_WS_WIN)
#  define USE_WIN_ENGINE
#  include <private/qfontengine_win_p.h>
#elif defined(Q_WS_MAC)
#  if defined(QT_MAC_USE_COCOA)
#    define USE_CORETEXT_ENGINE
#    include <private/qfontengine_coretext_p.h>
#  else
#    define USE_MAC_ENGINE
#    include <private/qfontengine_mac_p.h>
#  endif
#elif defined(Q_WS_QPA)
#  define USE_QPA_ENGINE
#  include <private/qfontengine_qpa_p.h>
#endif

void qt_disableFontHinting(QFont &font)
{
    QFontEngine *fontEngine = QFontPrivate::get(font)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }

#if defined(USE_FREETYPE_ENGINE)
    if (fontEngine->type() == QFontEngine::Freetype) {
        QFontEngineFT *ftEngine = static_cast<QFontEngineFT *>(fontEngine);
        ftEngine->setDefaultHintStyle(QFontEngineFT::HintNone);
    }
#elif defined(USE_S60_ENGINE)
#elif defined(USE_WIN_ENGINE)
#elif defined(USE_CORETEXT_ENGINE)
#elif defined(USE_MAC_ENGINE)
#elif defined(USE_QPA_ENGINE)
#endif
}

#define DISTANCEFIELD_CHARRANGE 0x8A
#define DISTANCEFIELD_TEXTURESIZE 2048
#define DISTANCEFIELD_BASEFONTSIZE 54
#define DISTANCEFIELD_TILESIZE 64
#define DISTANCEFIELD_SCALE 16
#define DISTANCEFIELD_RADIUS 80

static float mindist(const QImage &in, int w, int h, int x, int y, int r, float maxdist)
{
    int i, j, startx, starty, stopx, stopy, hit;
    float d, dx, dy;
    float mind = maxdist;
    int p;
    bool outside = (0 == qAlpha(in.pixel(x, y)));

    startx = qMax(0, x - r);
    starty = qMax(0, y - r);
    stopx = qMin(w, x + r);
    stopy = qMin(h, y + r);

    for (i = starty; i < stopy; i++) {
        for (j = startx; j < stopx; j++) {
            p = qAlpha(in.pixel(j, i));
            dx = j - x;
            dy = i - y;
            d = dx * dx + dy * dy;
            hit = (p != 0) == outside;
            if (hit && (d < mind)) {
                mind = d;
            }
        }
    }

    if (outside)
        return qSqrt(mind);
    else
        return -qSqrt(mind);
}

static QImage renderDistanceField(const QImage &in)
{
    int outWidth = in.width() / DISTANCEFIELD_SCALE;
    int outHeight = in.height() / DISTANCEFIELD_SCALE;
    QImage df(outWidth, outHeight, QImage::Format_ARGB32_Premultiplied);
    int x, y, ix, iy;
    float d;
    float sx = in.width() / float(outWidth);
    float sy = in.height() / float(outHeight);
    int r = DISTANCEFIELD_RADIUS;
    float maxsq = 2 * r * r;
    float max = qSqrt(maxsq);

    for(y = 0; y < outHeight; y++){
        for(x = 0; x < outWidth; x++){
            ix = (x * sx) + (sx / 2);
            iy = (y * sy) + (sy / 2);
            d = mindist(in, in.width(), in.height(), ix, iy, r, maxsq);
            float a = 127.5f + 127.5f * (-d / max);
            df.setPixel(x, y, qRgba(0, 0, 0, qRound(a)));
        }
    }

    return df;
}

QHash<QFontEngine::FaceId, QString> DistanceFieldFontAtlas::m_distfield_images;
QHash<QFontEngine::FaceId, QSGTextureRef> DistanceFieldFontAtlas::m_textures;

DistanceFieldFontAtlas::DistanceFieldFontAtlas(const QFont &font)
    : m_font(font)
{
    m_fontEngine = QFontPrivate::get(font)->engineForScript(QUnicodeTables::Common);\
    QFontEngine *realEngine = m_fontEngine;
    if (realEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(realEngine);
        realEngine = fem->engine(0);
    }

    m_faceId = realEngine->faceId();
    qt_disableFontHinting(m_font);
}

bool DistanceFieldFontAtlas::distanceFieldAvailable() const
{
    QHash<QFontEngine::FaceId, QString>::iterator available = m_distfield_images.find(m_faceId);
    if (available == m_distfield_images.end()) {
        QFileInfo fontInfo(m_faceId.filename);
        QString distFieldFile = QString("%1/%2.png").arg(fontInfo.absolutePath()).arg(fontInfo.baseName());
        QFileInfo distFieldInfo(distFieldFile);
        if (!distFieldInfo.exists()) {
            distFieldFile.clear();
            qWarning("Warning: distance-field rendering of font '%s' is not available", m_faceId.filename.constData());
        }
        available = m_distfield_images.insert(m_faceId, distFieldFile);
    }
    return !available.value().isEmpty();
}

QSGTextureRef DistanceFieldFontAtlas::texture()
{
    if (!distanceFieldAvailable())
        return QSGTextureRef();

    QHash<QFontEngine::FaceId, QSGTextureRef>::iterator tex = m_textures.find(m_faceId);
    if (tex == m_textures.end()) {
        QImage distFieldImage = distanceFieldAtlas();
        tex = m_textures.insert(m_faceId, QSGContext::current->textureManager()->upload(distFieldImage));
    }

    return tex.value();
}

QSize DistanceFieldFontAtlas::atlasSize() const
{
    const int texWidth = DISTANCEFIELD_TEXTURESIZE;
    const int texHeight = ((DISTANCEFIELD_CHARRANGE * DISTANCEFIELD_TILESIZE) / texWidth + 1) * DISTANCEFIELD_TILESIZE;
    return QSize(texWidth, texHeight);
}

DistanceFieldFontAtlas::Metrics DistanceFieldFontAtlas::glyphMetrics(glyph_t glyph) const
{
    Metrics m;

    //XXX yoann temporary (and slow) solution to get metrics in float
    QPainterPath path;
    QFixedPoint p;
    m_fontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

    m.width = DISTANCEFIELD_TILESIZE * scaleRatioFromRefSize();
    m.height = m.width;
    m.baselineX = path.boundingRect().x();
    m.baselineY = -path.boundingRect().y();
    m.margin = DISTANCEFIELD_RADIUS / qreal(DISTANCEFIELD_SCALE) * scaleRatioFromRefSize();

    return m;
}

DistanceFieldFontAtlas::TexCoord DistanceFieldFontAtlas::glyphTexCoord(glyph_t glyph) const
{
    TexCoord c;

    const QSize texSize = atlasSize();

    c.x = ((glyph * DISTANCEFIELD_TILESIZE) % texSize.width()) / qreal(texSize.width());
    c.y = ((glyph * DISTANCEFIELD_TILESIZE) / texSize.width()) * DISTANCEFIELD_TILESIZE / qreal(texSize.height());
    c.width = DISTANCEFIELD_TILESIZE / qreal(texSize.width());
    c.height = DISTANCEFIELD_TILESIZE / qreal(texSize.height());

    return c;
}

QImage DistanceFieldFontAtlas::renderDistanceFieldGlyph(glyph_t glyph) const
{
    QFont renderFont = m_font;
    renderFont.setPixelSize(DISTANCEFIELD_BASEFONTSIZE * DISTANCEFIELD_SCALE);
    qt_disableFontHinting(renderFont);

    QFontEngine *fontEngine = QFontPrivate::get(renderFont)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }

    QImage glyphImage = fontEngine->alphaMapForGlyph(glyph);
    QImage adjustedGlyph(glyphImage.width() + DISTANCEFIELD_RADIUS * 2, glyphImage.height() + DISTANCEFIELD_RADIUS * 2, QImage::Format_ARGB32_Premultiplied);
    adjustedGlyph.fill(Qt::transparent);
    QPainter p(&adjustedGlyph);
    p.drawImage(DISTANCEFIELD_RADIUS, DISTANCEFIELD_RADIUS, glyphImage);
    p.end();

    QImage im = renderDistanceField(adjustedGlyph);
    return im;
}

qreal DistanceFieldFontAtlas::scaleRatioFromRefSize() const
{
    return m_fontEngine->fontDef.pixelSize / DISTANCEFIELD_BASEFONTSIZE;
}

QImage DistanceFieldFontAtlas::distanceFieldAtlas() const
{
    if (!distanceFieldAvailable())
        return QImage();

    return QImage(m_distfield_images.value(m_faceId));
}

bool DistanceFieldFontAtlas::useDistanceFieldForFont(const QFont &font)
{
#if !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
    static QStringList args = qApp->arguments();
    if (args.contains("--distancefield-text")) {
        DistanceFieldFontAtlas a(font);
        return a.distanceFieldAvailable();
    }
#else
    static bool warningPrinted = false;
    if (!warningPrinted) {
        warningPrinted = true;
        qWarning("Warning: DistanceFieldGlyphNode is not implemented yet on Windows and Mac.");
    }
#endif
    return false;
}
