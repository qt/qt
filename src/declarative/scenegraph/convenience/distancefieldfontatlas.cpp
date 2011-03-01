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
#include <qlibraryinfo.h>

void qt_disableFontHinting(QFont &font)
{
    QFontEngine *fontEngine = QFontPrivate::get(font)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }
    fontEngine->setDefaultHintStyle(QFontEngine::HintNone);
}

#define QT_DISTANCEFIELD_TEXTURESIZE 2048
#define QT_DISTANCEFIELD_BASEFONTSIZE 54
#define QT_DISTANCEFIELD_TILESIZE 64
#define QT_DISTANCEFIELD_SCALE 16
#define QT_DISTANCEFIELD_RADIUS 80
#define QT_DISTANCEFIELD_MARGIN 50
#define QT_DISTANCEFIELD_MARGIN_THRESHOLD 0.31

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
    int outWidth = in.width() / QT_DISTANCEFIELD_SCALE;
    int outHeight = in.height() / QT_DISTANCEFIELD_SCALE;
    QImage df(outWidth, outHeight, QImage::Format_ARGB32_Premultiplied);
    int x, y, ix, iy;
    float d;
    float sx = in.width() / float(outWidth);
    float sy = in.height() / float(outHeight);
    int r = QT_DISTANCEFIELD_RADIUS;
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

uint qHash(const TexCoordCacheKey &key)
{
    return (qHash(key.distfield) << 13) | (key.glyph & 0x1FFF);
}

QHash<TexCoordCacheKey, DistanceFieldFontAtlas::TexCoord> DistanceFieldFontAtlas::m_texCoords;
QHash<QString, bool> DistanceFieldFontAtlas::m_distfield_availability;
QHash<QString, QSGTextureRef> DistanceFieldFontAtlas::m_textures;

DistanceFieldFontAtlas::DistanceFieldFontAtlas(const QFont &font)
{
    m_font = font;
    m_fontEngine = QFontPrivate::get(m_font)->engineForScript(QUnicodeTables::Common);
    if (m_fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(m_fontEngine);
        m_fontEngine = fem->engine(0);
    }
    qt_disableFontHinting(m_font);

    QFont referenceFont = m_font;
    referenceFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE);
    qt_disableFontHinting(referenceFont);
    m_referenceFontEngine = QFontPrivate::get(referenceFont)->engineForScript(QUnicodeTables::Common);
    if (m_referenceFontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(m_referenceFontEngine);
        m_referenceFontEngine = fem->engine(0);
    }

    QString basename = m_fontEngine->fontDef.family;
    basename.remove(QLatin1String(" "));
    QString italic = m_font.italic() ? QLatin1String("i") : QLatin1String("");
    QString bold = m_font.weight() > QFont::Normal ? QLatin1String("b") : QLatin1String("");
    m_distanceFieldFileName = basename + bold + italic + QLatin1String(".png");

    m_glyphCount = m_fontEngine->glyphCount();
    m_glyphMetricMargin = QT_DISTANCEFIELD_MARGIN / qreal(QT_DISTANCEFIELD_SCALE) * scaleRatioFromRefSize();
    m_glyphTexMargin = QT_DISTANCEFIELD_MARGIN / qreal(QT_DISTANCEFIELD_SCALE);
}

bool DistanceFieldFontAtlas::distanceFieldAvailable() const
{
    QHash<QString, bool>::iterator available = m_distfield_availability.find(m_distanceFieldFileName);
    if (available == m_distfield_availability.end()) {
        QString distFieldFile = QString(QLatin1String("%1/%2")).arg(distanceFieldDir()).arg(m_distanceFieldFileName);
        QFileInfo distFieldInfo(distFieldFile);
        bool exists = distFieldInfo.exists();
        if (!exists)
            qWarning("Warning: distance-field rendering '%s' does not exist", distFieldFile.toLatin1().constData());
        available = m_distfield_availability.insert(m_distanceFieldFileName, exists);
    }
    return available.value();
}

QSGTextureRef DistanceFieldFontAtlas::texture()
{
    if (!distanceFieldAvailable())
        return QSGTextureRef();

    QHash<QString, QSGTextureRef>::iterator tex = m_textures.find(m_distanceFieldFileName);
    if (tex == m_textures.end()) {
        QImage distFieldImage = distanceFieldAtlas();
        tex = m_textures.insert(m_distanceFieldFileName, uploadDistanceField(distFieldImage));
    }

    return tex.value();
}

QSize DistanceFieldFontAtlas::atlasSize() const
{
    if (!m_size.isValid()) {
        const int texWidth = QT_DISTANCEFIELD_TEXTURESIZE;
        const int texHeight = ((glyphCount() * QT_DISTANCEFIELD_TILESIZE) / texWidth + 1) * QT_DISTANCEFIELD_TILESIZE;
        m_size = QSize(texWidth, texHeight);
    }
    return m_size;
}

DistanceFieldFontAtlas::Metrics DistanceFieldFontAtlas::glyphMetrics(glyph_t glyph)
{
    QHash<glyph_t, Metrics>::iterator metric = m_metrics.find(glyph);
    if (metric == m_metrics.end()) {
        //XXX yoann temporary (and slow) solution to get metrics in float
        QPainterPath path;
        QFixedPoint p;
        m_fontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

        Metrics m;
        m.width = path.boundingRect().width();
        m.height = path.boundingRect().height();
        m.baselineX = path.boundingRect().x();
        m.baselineY = -path.boundingRect().y();

        metric = m_metrics.insert(glyph, m);
    }

    if (scaleRatioFromRefSize() <= QT_DISTANCEFIELD_MARGIN_THRESHOLD) {
        Metrics m = metric.value();
        m.width += m_glyphMetricMargin * 2;
        m.height += m_glyphMetricMargin * 2;
        m.baselineX -= m_glyphMetricMargin;
        m.baselineY += m_glyphMetricMargin;
        return m;
    }

    return metric.value();
}

DistanceFieldFontAtlas::TexCoord DistanceFieldFontAtlas::glyphTexCoord(glyph_t glyph)
{
    if (glyph >= glyphCount())
        qWarning("Warning: distance-field glyph is not available with index %d", glyph);

    TexCoordCacheKey key(distanceFieldFileName(), glyph);
    QHash<TexCoordCacheKey, TexCoord>::iterator texCoord = m_texCoords.find(key);
    if (texCoord == m_texCoords.end()) {
        const QSize texSize = atlasSize();

        //XXX yoann temporary (and slow) solution to get metrics in float
        QPainterPath path;
        QFixedPoint p;
        m_referenceFontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

        TexCoord c;
        c.xMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE) / texSize.width();
        c.yMargin = QT_DISTANCEFIELD_RADIUS / qreal(QT_DISTANCEFIELD_SCALE) / texSize.height();
        c.x = ((glyph * QT_DISTANCEFIELD_TILESIZE) % texSize.width()) / qreal(texSize.width());
        c.y = ((glyph * QT_DISTANCEFIELD_TILESIZE) / texSize.width()) * QT_DISTANCEFIELD_TILESIZE / qreal(texSize.height());
        c.width = path.boundingRect().width() / qreal(texSize.width());
        c.height = path.boundingRect().height() / qreal(texSize.height());

        texCoord = m_texCoords.insert(key, c);
    }

    if (scaleRatioFromRefSize() <= QT_DISTANCEFIELD_MARGIN_THRESHOLD) {
        TexCoord c = texCoord.value();
        c.xMargin -= m_glyphTexMargin / atlasSize().width();
        c.yMargin -= m_glyphTexMargin / atlasSize().height();
        c.width += (m_glyphTexMargin * 2) / qreal(atlasSize().width());
        c.height += (m_glyphTexMargin * 2) / qreal(atlasSize().height());
        return c;
    }

    return texCoord.value();
}

QImage DistanceFieldFontAtlas::renderDistanceFieldGlyph(glyph_t glyph) const
{
    QFont renderFont = m_font;
    renderFont.setPixelSize(QT_DISTANCEFIELD_BASEFONTSIZE * QT_DISTANCEFIELD_SCALE);
    qt_disableFontHinting(renderFont);

    QFontEngine *fontEngine = QFontPrivate::get(renderFont)->engineForScript(QUnicodeTables::Common);
    if (fontEngine->type() == QFontEngine::Multi) {
        QFontEngineMulti *fem = static_cast<QFontEngineMulti *>(fontEngine);
        fontEngine = fem->engine(0);
    }

    QPainterPath path;
    QFixedPoint pt;
    fontEngine->addGlyphsToPath(&glyph, &pt, 1, &path, 0);
    int glyphWidth = path.boundingRect().width();
    int glyphHeight = path.boundingRect().height();
    if (glyphWidth < 1)
        glyphWidth = 1;
    if (glyphHeight < 1)
        glyphHeight = 1;

    QImage glyphImage(glyphWidth + QT_DISTANCEFIELD_RADIUS * 2, glyphHeight + QT_DISTANCEFIELD_RADIUS * 2, QImage::Format_ARGB32_Premultiplied);
    glyphImage.fill(Qt::transparent);
    QPainter p(&glyphImage);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.translate(-path.boundingRect().x() + QT_DISTANCEFIELD_RADIUS, -path.boundingRect().y() + QT_DISTANCEFIELD_RADIUS);
    p.drawPath(path);
    p.end();

    QImage im = renderDistanceField(glyphImage);
    return im;
}

qreal DistanceFieldFontAtlas::scaleRatioFromRefSize() const
{
    return m_fontEngine->fontDef.pixelSize / QT_DISTANCEFIELD_BASEFONTSIZE;
}

QImage DistanceFieldFontAtlas::distanceFieldAtlas() const
{
    if (!distanceFieldAvailable())
        return QImage();

    return QImage(QString(QLatin1String("%1/%2")).arg(distanceFieldDir()).arg(m_distanceFieldFileName));
}

bool DistanceFieldFontAtlas::useDistanceFieldForFont(const QFont &font)
{
    static QStringList args = qApp->arguments();
    if (args.contains(QLatin1String("--distancefield-text"))) {
        DistanceFieldFontAtlas a(font);
        return a.distanceFieldAvailable();
    }
    return false;
}

static void convert_to_Format_Alpha(QImage *image)
{
    const int width = image->width();
    const int height = image->height();
    uchar *data = image->bits();

    for (int i = 0; i < height; ++i) {
        uchar *o = data + i * width;
        for (int x = 0; x < width; ++x)
            o[x] = (uchar)qAlpha(image->pixel(x, i));
    }
}

QSGTextureRef DistanceFieldFontAtlas::uploadDistanceField(const QImage &image)
{
    Q_ASSERT(!image.isNull());

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    QImage i = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    convert_to_Format_Alpha(&i);

    // We only need to store the alpha component
#ifdef QT_OPENGL_ES
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, i.width(), i.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, i.constBits());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA8, i.width(), i.height(), 0, GL_ALPHA, GL_UNSIGNED_BYTE, i.constBits());
#endif

    GLuint error = glGetError();
    if (error != GL_NO_ERROR) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &id);
        return QSGTextureRef();
    }

    QSGTexture *texture = new QSGTexture;
    texture->setTextureId(id);
    texture->setTextureSize(image.size());
    texture->setAlphaChannel(image.hasAlphaChannel());
    texture->setStatus(QSGTexture::Ready);

    QSGTextureRef ref(texture);
    return ref;
}

QString DistanceFieldFontAtlas::distanceFieldDir() const
{
    static QString distfieldpath = QString::fromLocal8Bit(qgetenv("QT_QML_DISTFIELDDIR"));
    if (distfieldpath.isEmpty()) {
#ifndef QT_NO_SETTINGS
        distfieldpath = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
        distfieldpath += QLatin1String("/fonts/distancefields");
#endif
    }

    return distfieldpath;
}

QString DistanceFieldFontAtlas::distanceFieldFileName() const
{
    return m_distanceFieldFileName;
}

int DistanceFieldFontAtlas::glyphCount() const
{
    return m_glyphCount;
}
