/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qfontengine_s60_p.h"
#include "qtextengine_p.h"
#include "qglobal.h"
#include <private/qapplication_p.h>
#include "qimage.h"
#include <private/qt_s60_p.h>

#include <e32base.h>
#include <e32std.h>
#include <eikenv.h>
#include <gdi.h>
#if defined(Q_SYMBIAN_HAS_FONTTABLE_API) || defined(Q_SYMBIAN_HAS_GLYPHOUTLINE_API)
#include <graphics/gdi/gdiplatapi.h>
#endif // Q_SYMBIAN_HAS_FONTTABLE_API || Q_SYMBIAN_HAS_GLYPHOUTLINE_API

QT_BEGIN_NAMESPACE

#ifdef Q_SYMBIAN_HAS_FONTTABLE_API
QSymbianTypeFaceExtras::QSymbianTypeFaceExtras(CFont* cFont, COpenFont *openFont)
    : m_cFont(cFont)
    , m_symbolCMap(false)
{
    Q_UNUSED(openFont)
}

QSymbianTypeFaceExtras::~QSymbianTypeFaceExtras()
{
    S60->screenDevice()->ReleaseFont(m_cFont);
}

QByteArray QSymbianTypeFaceExtras::getSfntTable(uint tag) const
{
    RFontTable fontTable;
    if (fontTable.Open(*m_cFont, tag) != KErrNone)
        return QByteArray();
    const QByteArray byteArray(reinterpret_cast<const char *>
            (fontTable.TableContent()),fontTable.TableLength());
    fontTable.Close();
    return byteArray;
}

bool QSymbianTypeFaceExtras::getSfntTableData(uint tag, uchar *buffer, uint *length) const
{
    RFontTable fontTable;
    if (fontTable.Open(*m_cFont, tag) != KErrNone)
        return false;

    bool result = true;
    const TInt tableByteLength = fontTable.TableLength();

    if (*length > 0 && *length < tableByteLength) {
        result = false; // Caller did not allocate enough memory
    } else {
        *length = tableByteLength;
        if (buffer)
            qMemCopy(buffer, fontTable.TableContent(), tableByteLength);
    }

    fontTable.Close();
    return result;
}

#else // Q_SYMBIAN_HAS_FONTTABLE_API
QSymbianTypeFaceExtras::QSymbianTypeFaceExtras(CFont* cFont, COpenFont *openFont)
    : m_cFont(cFont)
    , m_symbolCMap(false)
    , m_openFont(openFont)
{
    TAny *trueTypeExtension = NULL;
    m_openFont->ExtendedInterface(KUidOpenFontTrueTypeExtension, trueTypeExtension);
    m_trueTypeExtension = static_cast<MOpenFontTrueTypeExtension*>(trueTypeExtension);
    Q_ASSERT(m_trueTypeExtension);
}

QSymbianTypeFaceExtras::~QSymbianTypeFaceExtras()
{
}

QByteArray QSymbianTypeFaceExtras::getSfntTable(uint tag) const
{
    Q_ASSERT(m_trueTypeExtension->HasTrueTypeTable(tag));
    TInt error = KErrNone;
    TInt tableByteLength = 0;
    TAny *table = q_check_ptr(m_trueTypeExtension->GetTrueTypeTable(error, tag, &tableByteLength));
    QByteArray result(static_cast<const char*>(table), tableByteLength);
    m_trueTypeExtension->ReleaseTrueTypeTable(table);
    return result;
}

bool QSymbianTypeFaceExtras::getSfntTableData(uint tag, uchar *buffer, uint *length) const
{
    if (!m_trueTypeExtension->HasTrueTypeTable(tag))
        return false;

    bool result = true;
    TInt error = KErrNone;
    TInt tableByteLength;
    TAny *table =
        q_check_ptr(m_trueTypeExtension->GetTrueTypeTable(error, tag, &tableByteLength));

    if (error != KErrNone) {
        return false;
    } else if (*length > 0 && *length < tableByteLength) {
        result = false; // Caller did not allocate enough memory
    } else {
        *length = tableByteLength;
        if (buffer)
            qMemCopy(buffer, table, tableByteLength);
    }

    m_trueTypeExtension->ReleaseTrueTypeTable(table);
    return result;
}
#endif // Q_SYMBIAN_HAS_FONTTABLE_API

const uchar *QSymbianTypeFaceExtras::cmap() const
{
    if (m_cmapTable.isNull()) {
        const QByteArray cmapTable = getSfntTable(MAKE_TAG('c', 'm', 'a', 'p'));
        int size = 0;
        const uchar *cmap = QFontEngine::getCMap(reinterpret_cast<const uchar *>
                (cmapTable.constData()), cmapTable.size(), &m_symbolCMap, &size);
        m_cmapTable = QByteArray(reinterpret_cast<const char *>(cmap), size);
    }
    return reinterpret_cast<const uchar *>(m_cmapTable.constData());
}

CFont *QSymbianTypeFaceExtras::fontOwner() const
{
    return m_cFont;
}

// duplicated from qfontengine_xyz.cpp
static inline unsigned int getChar(const QChar *str, int &i, const int len)
{
    unsigned int uc = str[i].unicode();
    if (uc >= 0xd800 && uc < 0xdc00 && i < len-1) {
        uint low = str[i+1].unicode();
       if (low >= 0xdc00 && low < 0xe000) {
            uc = (uc - 0xd800)*0x400 + (low - 0xdc00) + 0x10000;
            ++i;
        }
    }
    return uc;
}

CFont *QFontEngineS60::fontWithSize(qreal size) const
{
    CFont *result = 0;
    TFontSpec fontSpec(qt_QString2TPtrC(QFontEngine::fontDef.family), TInt(size));
    fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    fontSpec.iFontStyle.SetPosture(QFontEngine::fontDef.style == QFont::StyleNormal?EPostureUpright:EPostureItalic);
    fontSpec.iFontStyle.SetStrokeWeight(QFontEngine::fontDef.weight > QFont::Normal?EStrokeWeightBold:EStrokeWeightNormal);
    const TInt errorCode = S60->screenDevice()->GetNearestFontToDesignHeightInPixels(result, fontSpec);
    Q_ASSERT(result && (errorCode == 0));
    return result;
}

void QFontEngineS60::setFontScale(qreal scale)
{
    if (qFuzzyCompare(scale, qreal(1))) {
        if (!m_originalFont)
            m_originalFont = fontWithSize(m_originalFontSizeInPixels);
        m_activeFont = m_originalFont;
    } else {
        const qreal scaledFontSizeInPixels = m_originalFontSizeInPixels * scale;
        if (!m_scaledFont ||
                (TInt(scaledFontSizeInPixels) != TInt(m_scaledFontSizeInPixels))) {
            releaseFont(m_scaledFont);
            m_scaledFontSizeInPixels = scaledFontSizeInPixels;
            m_scaledFont = fontWithSize(m_scaledFontSizeInPixels);
        }
        m_activeFont = m_scaledFont;
    }
}

void QFontEngineS60::releaseFont(CFont *&font)
{
    if (font) {
        S60->screenDevice()->ReleaseFont(font);
        font = 0;
    }
}

QFontEngineS60::QFontEngineS60(const QFontDef &request, const QSymbianTypeFaceExtras *extras)
    : m_extras(extras)
    , m_originalFont(0)
    , m_originalFontSizeInPixels((request.pixelSize >= 0)?
            request.pixelSize:pointsToPixels(request.pointSize))
    , m_scaledFont(0)
    , m_scaledFontSizeInPixels(0)
    , m_activeFont(0)
{
    QFontEngine::fontDef = request;
    setFontScale(1.0);
    cache_cost = sizeof(QFontEngineS60);
}

QFontEngineS60::~QFontEngineS60()
{
    releaseFont(m_originalFont);
    releaseFont(m_scaledFont);
}

bool QFontEngineS60::stringToCMap(const QChar *characters, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const
{
    if (*nglyphs < len) {
        *nglyphs = len;
        return false;
    }

    HB_Glyph *g = glyphs->glyphs;
    const unsigned char* cmap = m_extras->cmap();
    const bool isRtl = (flags & QTextEngine::RightToLeft);
    for (int i = 0; i < len; ++i) {
        const unsigned int uc = getChar(characters, i, len);
        *g++ = QFontEngine::getTrueTypeGlyphIndex(cmap,
        		isRtl ? QChar::mirroredChar(uc) : uc);
    }

    glyphs->numGlyphs = g - glyphs->glyphs;
    *nglyphs = glyphs->numGlyphs;

    if (flags & QTextEngine::GlyphIndicesOnly)
        return true;

    recalcAdvances(glyphs, flags);
    return true;
}

void QFontEngineS60::recalcAdvances(QGlyphLayout *glyphs, QTextEngine::ShaperFlags flags) const
{
    Q_UNUSED(flags);
    for (int i = 0; i < glyphs->numGlyphs; i++) {
        const glyph_metrics_t bbox = boundingBox_const(glyphs->glyphs[i]);
        glyphs->advances_x[i] = bbox.xoff;
        glyphs->advances_y[i] = bbox.yoff;
    }
}

#ifdef Q_SYMBIAN_HAS_GLYPHOUTLINE_API
static bool parseGlyphPathData(const char *dataStr, const char *dataEnd, QPainterPath &path,
                               qreal fontPixelSize, const QPointF &offset, bool hinted);
#endif //Q_SYMBIAN_HAS_GLYPHOUTLINE_API

void QFontEngineS60::addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions,
                                     int nglyphs, QPainterPath *path,
                                     QTextItem::RenderFlags flags)
{
#ifdef Q_SYMBIAN_HAS_GLYPHOUTLINE_API
    Q_UNUSED(flags)
    RGlyphOutlineIterator iterator;
    const TInt error = iterator.Open(*m_activeFont, glyphs, nglyphs);
    if (KErrNone != error)
        return;
    const qreal fontSizeInPixels = qreal(m_activeFont->HeightInPixels());
    int count = 0;
    do {
        const TUint8* outlineUint8 = iterator.Outline();
        const char* const outlineChar = reinterpret_cast<const char*>(outlineUint8);
        const char* const outlineEnd = outlineChar + iterator.OutlineLength();
        parseGlyphPathData(outlineChar, outlineEnd, *path, fontSizeInPixels,
                positions[count++].toPointF(), false);
    } while(KErrNone == iterator.Next() && count <= nglyphs);
#else // Q_SYMBIAN_HAS_GLYPHOUTLINE_API
    QFontEngine::addGlyphsToPath(glyphs, positions, nglyphs, path, flags);
#endif //Q_SYMBIAN_HAS_GLYPHOUTLINE_API
}

QImage QFontEngineS60::alphaMapForGlyph(glyph_t glyph)
{
    TOpenFontCharMetrics metrics;
    const TUint8 *glyphBitmapBytes;
    TSize glyphBitmapSize;
    getCharacterData(glyph, metrics, glyphBitmapBytes, glyphBitmapSize);
    QImage result(glyphBitmapBytes, glyphBitmapSize.iWidth, glyphBitmapSize.iHeight, glyphBitmapSize.iWidth, QImage::Format_Indexed8);
    result.setColorTable(grayPalette());

    // The above setColorTable() call detached the image data anyway, so why not shape tha data a bit, while we can.
    // CFont::GetCharacterData() returns 8-bit data that obviously was 4-bit data before, and converted to 8-bit incorrectly.
    // The data values are 0x00, 0x10 ... 0xe0, 0xf0. So, a real opaque 0xff is never reached, which we get punished
    // for every time we want to blit this glyph in the raster paint engine.
    // "Fix" is to convert all 0xf0 to 0xff. Is fine, quality wise, and I assume faster than correcting all values.
    // Blitting is however, evidentially faster now.
    const int bpl = result.bytesPerLine();
    for (int row = 0; row < result.height(); ++row) {
        uchar *scanLine = result.scanLine(row);
        for (int column = 0; column < bpl; ++column) {
            if (*scanLine == 0xf0)
                *scanLine = 0xff;
            scanLine++;
        }
    }

    return result;
}

glyph_metrics_t QFontEngineS60::boundingBox(const QGlyphLayout &glyphs)
{
   if (glyphs.numGlyphs == 0)
        return glyph_metrics_t();

    QFixed w = 0;
    for (int i = 0; i < glyphs.numGlyphs; ++i)
        w += glyphs.effectiveAdvance(i);

    return glyph_metrics_t(0, -ascent(), w, ascent()+descent()+1, w, 0);
}

glyph_metrics_t QFontEngineS60::boundingBox_const(glyph_t glyph) const
{
    TOpenFontCharMetrics metrics;
    const TUint8 *glyphBitmapBytes;
    TSize glyphBitmapSize;
    getCharacterData(glyph, metrics, glyphBitmapBytes, glyphBitmapSize);
    TRect glyphBounds;
    metrics.GetHorizBounds(glyphBounds);
    const glyph_metrics_t result(
        glyphBounds.iTl.iX,
        glyphBounds.iTl.iY,
        glyphBounds.Width(),
        glyphBounds.Height(),
        metrics.HorizAdvance(),
        0
    );
    return result;
}

glyph_metrics_t QFontEngineS60::boundingBox(glyph_t glyph)
{
    return boundingBox_const(glyph);
}

QFixed QFontEngineS60::ascent() const
{
    // Workaround for QTBUG-8013
    // Stroke based fonts may return an incorrect FontMaxAscent of 0.
    const QFixed ascent = m_originalFont->FontMaxAscent();
    return (ascent > 0) ? ascent : QFixed::fromReal(m_originalFontSizeInPixels) - descent();
}

QFixed QFontEngineS60::descent() const
{
    return m_originalFont->FontMaxDescent();
}

QFixed QFontEngineS60::leading() const
{
    return 0;
}

qreal QFontEngineS60::maxCharWidth() const
{
    return m_originalFont->MaxCharWidthInPixels();
}

const char *QFontEngineS60::name() const
{
    return "QFontEngineS60";
}

bool QFontEngineS60::canRender(const QChar *string, int len)
{
    const unsigned char *cmap = m_extras->cmap();
    for (int i = 0; i < len; ++i) {
        const unsigned int uc = getChar(string, i, len);
        if (QFontEngine::getTrueTypeGlyphIndex(cmap, uc) == 0)
            return false;
    }
    return true;
}

QByteArray QFontEngineS60::getSfntTable(uint tag) const
{
    return m_extras->getSfntTable(tag);
}

bool QFontEngineS60::getSfntTableData(uint tag, uchar *buffer, uint *length) const
{
    return m_extras->getSfntTableData(tag, buffer, length);
}

QFontEngine::Type QFontEngineS60::type() const
{
    return QFontEngine::S60FontEngine;
}

void QFontEngineS60::getCharacterData(glyph_t glyph, TOpenFontCharMetrics& metrics, const TUint8*& bitmap, TSize& bitmapSize) const
{
    // Setting the most significant bit tells GetCharacterData
    // that 'code' is a Glyph ID, rather than a UTF-16 value
    const TUint specialCode = (TUint)glyph | 0x80000000;

    const CFont::TCharacterDataAvailability availability =
            m_activeFont->GetCharacterData(specialCode, metrics, bitmap, bitmapSize);
    const glyph_t fallbackGlyph = '?';
    if (availability != CFont::EAllCharacterData) {
        const CFont::TCharacterDataAvailability fallbackAvailability =
                m_activeFont->GetCharacterData(fallbackGlyph, metrics, bitmap, bitmapSize);
        Q_ASSERT(fallbackAvailability == CFont::EAllCharacterData);
    }
}

#ifdef Q_SYMBIAN_HAS_GLYPHOUTLINE_API
static inline void skipSpacesAndComma(const char* &str, const char* const strEnd)
{
    while (str <= strEnd && (*str == ' ' || *str == ','))
        ++str;
}

static bool parseGlyphPathData(const char *svgPath, const char *svgPathEnd, QPainterPath &path,
                               qreal fontPixelSize, const QPointF &offset, bool hinted)
{
    Q_UNUSED(hinted)
    QPointF p1, p2, firstSubPathPoint;
    qreal *elementValues[] =
        {&p1.rx(), &p1.ry(), &p2.rx(), &p2.ry()};
    const int unitsPerEm = 2048; // See: http://en.wikipedia.org/wiki/Em_%28typography%29
    const qreal resizeFactor = fontPixelSize / unitsPerEm;

    while (svgPath < svgPathEnd) {
        skipSpacesAndComma(svgPath, svgPathEnd);
        const char pathElem = *svgPath++;
        skipSpacesAndComma(svgPath, svgPathEnd);

        if (pathElem != 'Z') {
            char *endStr = 0;
            int elementValuesCount = 0;
            for (int i = 0; i < 4; ++i) { // 4 = size of elementValues[]
                qreal coordinateValue = strtod(svgPath, &endStr);
                if (svgPath == endStr)
                    break;
                if (i % 2) // Flip vertically
                    coordinateValue = -coordinateValue;
                *elementValues[i] = coordinateValue * resizeFactor;
                elementValuesCount++;
                svgPath = endStr;
                skipSpacesAndComma(svgPath, svgPathEnd);
            }
            p1 += offset;
            if (elementValuesCount == 2)
                p2 = firstSubPathPoint;
            else
                p2 += offset;
        }

        switch (pathElem) {
        case 'M':
            firstSubPathPoint = p1;
            path.moveTo(p1);
            break;
        case 'Z':
            path.closeSubpath();
            break;
        case 'L':
            path.lineTo(p1);
            break;
        case 'Q':
            path.quadTo(p1, p2);
            break;
        default:
            return false;
        }
    }
    return true;
}
#endif // Q_SYMBIAN_HAS_GLYPHOUTLINE_API

QT_END_NAMESPACE
