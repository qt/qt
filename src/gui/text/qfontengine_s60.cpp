/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_EMBEDDED_LICENSE$
**
****************************************************************************/

#include "qfontengine_s60_p.h"
#include "qtextengine_p.h"
#include "qglobal.h"
#include <private/qapplication_p.h>
#include <private/qwindowsurface_s60_p.h>
#include "qimage.h"
#include "qt_s60_p.h"

#include <e32base.h>
#include <e32std.h>
#include <EIKENV.H>
#include <GDI.H>

static const int maxFontSizeInPixels = 60;

QFontEngineS60Extensions::QFontEngineS60Extensions(COpenFont *font)
    : m_font(font)
    , m_cmap(0)
    , m_symbolCMap(false)
{
    TAny *shapingExtension = NULL;
    m_font->ExtendedInterface(KUidOpenFontShapingExtension, shapingExtension);
    m_shapingExtension = static_cast<MOpenFontShapingExtension*>(shapingExtension);
    TAny *trueTypeExtension = NULL;
    m_font->ExtendedInterface(KUidOpenFontTrueTypeExtension, trueTypeExtension);
    m_trueTypeExtension = static_cast<MOpenFontTrueTypeExtension*>(trueTypeExtension);
    Q_ASSERT(m_shapingExtension && m_trueTypeExtension);
}

QByteArray QFontEngineS60Extensions::getSfntTable(uint tag) const
{
    Q_ASSERT(m_trueTypeExtension->HasTrueTypeTable(tag));
    TInt error = KErrNone;
    TInt tableByteLength = 0;
    TAny *table = m_trueTypeExtension->GetTrueTypeTable(error, tag, &tableByteLength);
    QByteArray result(static_cast<const char*>(table), tableByteLength);
    m_trueTypeExtension->ReleaseTrueTypeTable(table);
    return result;
}

const unsigned char *QFontEngineS60Extensions::cmap() const
{
    if (!m_cmap) {
        m_cmapTable = getSfntTable(MAKE_TAG('c', 'm', 'a', 'p'));
        int size = 0;
        m_cmap = QFontEngineS60::getCMap(reinterpret_cast<const uchar *>(m_cmapTable.constData()), m_cmapTable.size(), &m_symbolCMap, &size);
    }
    return m_cmap;
}

QPainterPath QFontEngineS60Extensions::glyphOutline(glyph_t glyph) const
{
    QPainterPath result;
    QPolygonF polygon;
    TInt glyphIndex = glyph;
    TInt pointNumber = 0;
    TInt x, y;
    while (m_shapingExtension->GlyphPointInFontUnits(glyphIndex, pointNumber++, x, y)) {
        const QPointF point(qreal(x) / 0xffff, qreal(y) / 0xffff);
        if (polygon.contains(point)) {
            result.addPolygon(polygon);
            result.closeSubpath();
            polygon.clear();
        } else {
            polygon.append(point);
        }
    }
    return result;
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

QFontEngineS60::QFontEngineS60(const QFontDef &request, const QFontEngineS60Extensions *extensions)
    : m_extensions(extensions)
{
    QFontEngine::fontDef = request;
    m_fontSizeInPixels = (request.pixelSize >= 0)?
        request.pixelSize:pointsToPixels(request.pointSize);
    QS60WindowSurface::unlockBitmapHeap();
    m_textRenderBitmap = new (ELeave) CFbsBitmap();
    const TSize bitmapSize(1, 1); // It is just a dummy bitmap that I need to keep the font alive (or maybe not)
    User::LeaveIfError(m_textRenderBitmap->Create(bitmapSize, EGray256));
    m_textRenderBitmapDevice = CFbsBitmapDevice::NewL(m_textRenderBitmap);
    User::LeaveIfError(m_textRenderBitmapDevice->CreateContext(m_textRenderBitmapGc));
    cache_cost = sizeof(QFontEngineS60) + bitmapSize.iHeight * bitmapSize.iWidth * 4;

    TFontSpec fontSpec(qt_QString2TPtrC(request.family), m_fontSizeInPixels);
    fontSpec.iFontStyle.SetBitmapType(EAntiAliasedGlyphBitmap);
    fontSpec.iFontStyle.SetPosture(request.style == QFont::StyleNormal?EPostureUpright:EPostureItalic);
    fontSpec.iFontStyle.SetStrokeWeight(request.weight > QFont::Normal?EStrokeWeightBold:EStrokeWeightNormal);
    const TInt errorCode = m_textRenderBitmapDevice->GetNearestFontInPixels(m_font, fontSpec);
    Q_ASSERT(errorCode == 0);
    m_textRenderBitmapGc->UseFont(m_font);
    QS60WindowSurface::lockBitmapHeap();
}

QFontEngineS60::~QFontEngineS60()
{
    QS60WindowSurface::unlockBitmapHeap();
    m_textRenderBitmapGc->DiscardFont();
    delete m_textRenderBitmapGc;
    m_textRenderBitmapGc = NULL;
    m_textRenderBitmapDevice->ReleaseFont(m_font);
    delete m_textRenderBitmapDevice;
    m_textRenderBitmapDevice = NULL;
    delete m_textRenderBitmap;
    m_textRenderBitmap = NULL;
    QS60WindowSurface::lockBitmapHeap();
}

bool QFontEngineS60::stringToCMap(const QChar *characters, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const
{
    if (*nglyphs < len) {
        *nglyphs = len;
        return false;
    }

    HB_Glyph *g = glyphs->glyphs;
    const unsigned char* cmap = m_extensions->cmap();
    for (int i = 0; i < len; ++i) {
        const unsigned int uc = getChar(characters, i, len);
        *g++ = QFontEngine::getTrueTypeGlyphIndex(cmap, uc);
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
        glyphs->advances_x[i] = glyphs->offsets[i].x = bbox.xoff;
        glyphs->advances_y[i] = glyphs->offsets[i].y = bbox.yoff;
    }
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
    glyph_metrics_t overall;
    // initialize with line height, we get the same behaviour on all platforms
    overall.y = -ascent();
    overall.height = ascent() + descent() + 1;

    QFixed ymax = 0;
    QFixed xmax = 0;
    for (int i = 0; i < glyphs.numGlyphs; i++) {
        const glyph_metrics_t glyphBounds = boundingBox_const(glyphs.glyphs[i]);
        QFixed x = overall.xoff + glyphs.offsets[i].x + glyphBounds.x;
        QFixed y = overall.yoff + glyphs.offsets[i].y - glyphBounds.y;
        overall.x = qMin(overall.x, x);
        overall.y = qMin(overall.y, y);
        xmax = qMax(xmax, x + glyphBounds.width);
        ymax = qMax(ymax, y + glyphBounds.height);
        overall.xoff += glyphBounds.xoff;
    }
    overall.height = qMax(overall.height, ymax - overall.y);
    overall.width = xmax - overall.x;

    return overall;
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
        metrics.VertAdvance()
    );
    return result;
}

glyph_metrics_t QFontEngineS60::boundingBox(glyph_t glyph)
{
    return boundingBox_const(glyph);
}

QFixed QFontEngineS60::ascent() const
{
    return m_font->FontMaxAscent();
}

QFixed QFontEngineS60::descent() const
{
    return m_font->FontMaxDescent();
}

QFixed QFontEngineS60::leading() const
{
    return 0;
}

qreal QFontEngineS60::maxCharWidth() const
{
    return m_font->MaxCharWidthInPixels();
}

const char *QFontEngineS60::name() const
{
    return "QFontEngineS60";
}

bool QFontEngineS60::canRender(const QChar *string, int len)
{
    const unsigned char *cmap = m_extensions->cmap();
    for (int i = 0; i < len; ++i) {
        const unsigned int uc = getChar(string, i, len);
        if (QFontEngine::getTrueTypeGlyphIndex(cmap, uc) == 0)
            return false;
    }
    return true;
}

QByteArray QFontEngineS60::getSfntTable(uint tag) const
{
    return m_extensions->getSfntTable(tag);
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
        m_font->GetCharacterData(specialCode, metrics, bitmap, bitmapSize);
    const glyph_t fallbackGlyph = '?';
    if (availability != CFont::EAllCharacterData) {
        const CFont::TCharacterDataAvailability fallbackAvailability =
            m_font->GetCharacterData(fallbackGlyph, metrics, bitmap, bitmapSize);
        Q_ASSERT(fallbackAvailability == CFont::EAllCharacterData);
    }
}

QT_END_NAMESPACE
