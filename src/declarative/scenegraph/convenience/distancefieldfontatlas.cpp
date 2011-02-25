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

#define DISTANCEFIELD_CHARRANGE 0xFF
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

QHash<QString, bool> DistanceFieldFontAtlas::m_distfield_availability;
QHash<QString, QSGTextureRef> DistanceFieldFontAtlas::m_textures;

DistanceFieldFontAtlas::DistanceFieldFontAtlas(const QFont &font)
{
    m_font = font;
    m_fontEngine = QFontPrivate::get(m_font)->engineForScript(QUnicodeTables::Common);
    qt_disableFontHinting(m_font);

    QFont referenceFont = m_font;
    referenceFont.setPixelSize(DISTANCEFIELD_BASEFONTSIZE);
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

    m.width = path.boundingRect().width();
    m.height = path.boundingRect().height();
    m.baselineX = path.boundingRect().x();
    m.baselineY = -path.boundingRect().y();

    return m;
}

DistanceFieldFontAtlas::TexCoord DistanceFieldFontAtlas::glyphTexCoord(glyph_t glyph) const
{
    TexCoord c;

    const QSize texSize = atlasSize();

    //XXX yoann temporary (and slow) solution to get metrics in float
    QPainterPath path;
    QFixedPoint p;
    m_referenceFontEngine->addGlyphsToPath(&glyph, &p, 1, &path, 0);

    c.xMargin = DISTANCEFIELD_RADIUS / qreal(DISTANCEFIELD_SCALE) / texSize.width();
    c.yMargin = DISTANCEFIELD_RADIUS / qreal(DISTANCEFIELD_SCALE) / texSize.height();
    c.x = ((glyph * DISTANCEFIELD_TILESIZE) % texSize.width()) / qreal(texSize.width());
    c.y = ((glyph * DISTANCEFIELD_TILESIZE) / texSize.width()) * DISTANCEFIELD_TILESIZE / qreal(texSize.height());
    c.width = path.boundingRect().width() / qreal(texSize.width());
    c.height = path.boundingRect().height() / qreal(texSize.height());

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

    return QImage(QString(QLatin1String("%1/%2")).arg(distanceFieldDir()).arg(m_distanceFieldFileName));
}

bool DistanceFieldFontAtlas::useDistanceFieldForFont(const QFont &font)
{
    static QStringList args = qApp->arguments();
    if (args.contains("--distancefield-text")) {
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
    QString distfieldpath = QString::fromLocal8Bit(qgetenv("QT_QML_DISTFIELDDIR"));
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
