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

#include <private/qapplication_p.h>
#include "qdir.h"
#include "qfont_p.h"
#include "qfontengine_s60_p.h"
#include "qabstractfileengine.h"
#include "qdesktopservices.h"
#include "qt_s60_p.h"
#include "qendian.h"
#include <private/qwindowsurface_s60_p.h>
#include <private/qcore_symbian_p.h>
#if defined(QT_NO_FREETYPE)
#include <OPENFONT.H>
#endif

QT_BEGIN_NAMESPACE

#if defined(QT_NO_FREETYPE)
class QFontDatabaseS60StoreImplementation : public QFontDatabaseS60Store
{
public:
    QFontDatabaseS60StoreImplementation();
    ~QFontDatabaseS60StoreImplementation();

    const QFontEngineS60Extensions *extension(const QString &typeface) const;

private:
    RHeap* m_heap;
    CFontStore *m_store;
    COpenFontRasterizer *m_rasterizer;
    mutable QHash<QString, const QFontEngineS60Extensions *> m_extensions;
};

QFontDatabaseS60StoreImplementation::QFontDatabaseS60StoreImplementation()
{
    m_heap = User::ChunkHeap(NULL, 0x1000, 0x100000);
    m_store = CFontStore::NewL(m_heap);
    m_rasterizer = COpenFontRasterizer::NewL(TUid::Uid(0x101F7F5E));
    m_store->InstallRasterizerL(m_rasterizer);
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::FontsLocation));
    dir.setNameFilters(QStringList() << QLatin1String("*.ttf") << QLatin1String("*.ccc"));
    for (int i = 0; i < int(dir.count()); ++i) {
        const QString fontFile = QDir::toNativeSeparators(dir.absoluteFilePath(dir[i]));
        m_store->AddFileL(qt_QString2TPtrC(fontFile));
    }
}
QFontDatabaseS60StoreImplementation::~QFontDatabaseS60StoreImplementation()
{
    qDeleteAll(m_extensions);
    delete m_rasterizer;
    delete m_store;
    m_heap->Close();
}

const QFontEngineS60Extensions *QFontDatabaseS60StoreImplementation::extension(const QString &typeface) const
{
    if (!m_extensions.contains(typeface)) {
        CFont* font = NULL;
        TFontSpec spec(qt_QString2TPtrC(typeface), 1);
        spec.iHeight = 1;
        const TInt err = m_store->GetNearestFontToDesignHeightInPixels(font, spec);
        Q_ASSERT(err == KErrNone && font);
        CBitmapFont *bitmapFont = static_cast<CBitmapFont*>(font);
        m_extensions.insert(typeface, new QFontEngineS60Extensions(bitmapFont->OpenFont()));
    }
    return m_extensions.value(typeface);
}
#else
class QFontEngineFTS60 : public QFontEngineFT
{
public:
    QFontEngineFTS60(const QFontDef &fd);
};

QFontEngineFTS60::QFontEngineFTS60(const QFontDef &fd)
    : QFontEngineFT(fd)
{
    default_hint_style = HintFull;
}
#endif // defined(QT_NO_FREETYPE)

/*
 QFontEngineS60::pixelsToPoints, QFontEngineS60::pointsToPixels, QFontEngineMultiS60::QFontEngineMultiS60
 and QFontEngineMultiS60::QFontEngineMultiS60 should be in qfontengine_s60.cpp. But since also the
 Freetype based font rendering need them, they are here.
*/
qreal QFontEngineS60::pixelsToPoints(qreal pixels, Qt::Orientation orientation)
{
    return (orientation == Qt::Horizontal?
        S60->screenDevice()->HorizontalPixelsToTwips(pixels)
        :S60->screenDevice()->VerticalPixelsToTwips(pixels)) / KTwipsPerPoint;
}

qreal QFontEngineS60::pointsToPixels(qreal points, Qt::Orientation orientation)
{
    const int twips = points * KTwipsPerPoint;
    return orientation == Qt::Horizontal?
        S60->screenDevice()->HorizontalTwipsToPixels(twips)
        :S60->screenDevice()->VerticalTwipsToPixels(twips);
}

QFontEngineMultiS60::QFontEngineMultiS60(QFontEngine *first, int script, const QStringList &fallbackFamilies)
    : QFontEngineMulti(fallbackFamilies.size() + 1)
    , m_script(script)
    , m_fallbackFamilies(fallbackFamilies)
{
    engines[0] = first;
    first->ref.ref();
    fontDef = engines[0]->fontDef;
}

void QFontEngineMultiS60::loadEngine(int at)
{
    Q_ASSERT(at < engines.size());
    Q_ASSERT(engines.at(at) == 0);

    QFontDef request = fontDef;
    request.styleStrategy |= QFont::NoFontMerging;
    request.family = m_fallbackFamilies.at(at-1);
    engines[at] = QFontDatabase::findFont(m_script,
                                          /*fontprivate*/0,
                                          request);
    Q_ASSERT(engines[at]);
}

static void initializeDb()
{
    QFontDatabasePrivate *db = privateDb();
    if(!db || db->count)
        return;

#if defined(QT_NO_FREETYPE)
    if (!db->s60Store)
        db->s60Store = new QFontDatabaseS60StoreImplementation;

    QS60WindowSurface::unlockBitmapHeap();
    const int numTypeFaces = QS60Data::screenDevice()->NumTypefaces();
    const QFontDatabaseS60StoreImplementation *store = dynamic_cast<const QFontDatabaseS60StoreImplementation*>(db->s60Store);
    Q_ASSERT(store);
    for (int i = 0; i < numTypeFaces; i++) {
        TTypefaceSupport typefaceSupport;
        QS60Data::screenDevice()->TypefaceSupport(typefaceSupport, i);
        CFont *font; // We have to get a font instance in order to know all the details
        TFontSpec fontSpec(typefaceSupport.iTypeface.iName, 11);
        QS60Data::screenDevice()->GetNearestFontInPixels(font, fontSpec);
        if (font->TypeUid() == KCFbsFontUid) {
            TOpenFontFaceAttrib faceAttrib;
            const CFbsFont *cfbsFont = dynamic_cast<const CFbsFont *>(font);
            Q_ASSERT(cfbsFont);
            cfbsFont->GetFaceAttrib(faceAttrib);

            QtFontStyle::Key styleKey;
            styleKey.style = faceAttrib.IsItalic()?QFont::StyleItalic:QFont::StyleNormal;
            styleKey.weight = faceAttrib.IsBold()?QFont::Bold:QFont::Normal;

            QString familyName((const QChar *)typefaceSupport.iTypeface.iName.Ptr(), typefaceSupport.iTypeface.iName.Length());
            QtFontFamily *family = db->family(familyName, true);
            family->fixedPitch = faceAttrib.IsMonoWidth();
            QtFontFoundry *foundry = family->foundry(QString(), true);
            QtFontStyle *style = foundry->style(styleKey, true);
            style->smoothScalable = typefaceSupport.iIsScalable;
            style->pixelSize(0, true);

            const QFontEngineS60Extensions *extension = store->extension(familyName);
            const QByteArray os2Table = extension->getSfntTable(MAKE_TAG('O', 'S', '/', '2'));
            const unsigned char* data = reinterpret_cast<const unsigned char*>(os2Table.constData());
            const unsigned char* ulUnicodeRange = data + 42;
            quint32 unicodeRange[4] = {
                qFromBigEndian<quint32>(ulUnicodeRange),
                qFromBigEndian<quint32>(ulUnicodeRange + 4),
                qFromBigEndian<quint32>(ulUnicodeRange + 8),
                qFromBigEndian<quint32>(ulUnicodeRange + 12)
            };
            const unsigned char* ulCodePageRange = data + 78;
            quint32 codePageRange[2] = {
                qFromBigEndian<quint32>(ulCodePageRange),
                qFromBigEndian<quint32>(ulCodePageRange + 4)
            };
            const QList<QFontDatabase::WritingSystem> writingSystems =
                determineWritingSystemsFromTrueTypeBits(unicodeRange, codePageRange);
            foreach (const QFontDatabase::WritingSystem system, writingSystems)
                family->writingSystems[system] = QtFontFamily::Supported;
        }
        QS60Data::screenDevice()->ReleaseFont(font);
    }
    QS60WindowSurface::lockBitmapHeap();
#else // defined(QT_NO_FREETYPE)
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::FontsLocation));
    dir.setNameFilters(QStringList() << QLatin1String("*.ttf")
                       << QLatin1String("*.ttc") << QLatin1String("*.pfa")
                       << QLatin1String("*.pfb"));
    for (int i = 0; i < int(dir.count()); ++i) {
        const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
        db->addTTFile(file);
    }
#endif // defined(QT_NO_FREETYPE)
}

static inline void load(const QString &family = QString(), int script = -1)
{
    Q_UNUSED(family)
    Q_UNUSED(script)
    initializeDb();
}

static void registerFont(QFontDatabasePrivate::ApplicationFont *fnt)
{
    Q_UNUSED(fnt);
}

bool QFontDatabase::supportsThreadedFontRendering()
{
    return false;
}

static
QFontDef cleanedFontDef(const QFontDef &req)
{
    QFontDef result = req;
    if (result.pixelSize <= 0) {
        result.pixelSize = QFontEngineS60::pointsToPixels(qMax(qreal(1.0), result.pointSize));
        result.pointSize = 0;
    }
    return result;
}

QFontEngine *QFontDatabase::findFont(int script, const QFontPrivate *, const QFontDef &req)
{
    const QFontCache::Key key(cleanedFontDef(req), script);

    if (!privateDb()->count)
        initializeDb();

    QFontEngine *fe = QFontCache::instance()->findEngine(key);
    if (!fe) {
        // Making sure that fe->fontDef.family will be an existing font.
        initializeDb();
        QFontDatabasePrivate *db = privateDb();
        QtFontDesc desc;
        QList<int> blacklistedFamilies;
        match(script, req, req.family, QString(), -1, &desc, blacklistedFamilies);
        if (!desc.family) // falling back to application font
            desc.family = db->family(QApplication::font().defaultFamily());
        Q_ASSERT(desc.family);

        // Making sure that desc.family supports the requested script
        QtFontDesc mappedDesc;
        bool supportsScript = false;
        do {
            match(script, req, QString(), QString(), -1, &mappedDesc, blacklistedFamilies);
            if (mappedDesc.family == desc.family) {
                supportsScript = true;
                break;
            }
            blacklistedFamilies.append(mappedDesc.familyIndex);
        } while (mappedDesc.family);
        if (!supportsScript) {
            blacklistedFamilies.clear();
            match(script, req, QString(), QString(), -1, &mappedDesc, blacklistedFamilies);
            if (mappedDesc.family)
                desc = mappedDesc;
        }

        const QString fontFamily = desc.family->name;
        QFontDef request = req;
        request.family = fontFamily;
#if defined(QT_NO_FREETYPE)
        const QFontDatabaseS60StoreImplementation *store = dynamic_cast<const QFontDatabaseS60StoreImplementation*>(db->s60Store);
        Q_ASSERT(store);
        const QFontEngineS60Extensions *extension = store->extension(fontFamily);
        fe = new QFontEngineS60(request, extension);
#else
        QFontEngine::FaceId faceId;
        const QtFontFamily * const reqQtFontFamily = db->family(fontFamily);
        faceId.filename = reqQtFontFamily->fontFilename;
        faceId.index = reqQtFontFamily->fontFileIndex;

        QFontEngineFTS60 *fte = new QFontEngineFTS60(cleanedFontDef(request));
        if (fte->init(faceId, true, QFontEngineFT::Format_A8))
            fe = fte;
        else
            delete fte;
#endif

        Q_ASSERT(fe);
        if (script == QUnicodeTables::Common
            && !(req.styleStrategy & QFont::NoFontMerging)
            && !fe->symbol) {

            QStringList commonFonts;
            for (int ws = 1; ws < QFontDatabase::WritingSystemsCount; ++ws) {
                if (scriptForWritingSystem[ws] != script)
                    continue;
                for (int i = 0; i < db->count; ++i) {
                    if (db->families[i]->writingSystems[ws] & QtFontFamily::Supported)
                        commonFonts.append(db->families[i]->name);
                }
            }

            // Hack: Prioritize .ccc fonts
            const QString niceEastAsianFont(QLatin1String("Sans MT 936_S60"));
            if (commonFonts.removeAll(niceEastAsianFont) > 0)
                commonFonts.prepend(niceEastAsianFont);

            fe = new QFontEngineMultiS60(fe, script, commonFonts);
        }
    }
    fe->ref.ref();
    QFontCache::instance()->insertEngine(key, fe);
    return fe;
}

void QFontDatabase::load(const QFontPrivate *d, int script)
{
    QFontEngine *fe = 0;
    QFontDef req = d->request;
 
    if (!d->engineData) {
        const QFontCache::Key key(cleanedFontDef(req), script);
        getEngineData(d, key);
    }

    // the cached engineData could have already loaded the engine we want
    if (d->engineData->engines[script])
        fe = d->engineData->engines[script];

    if (!fe) {
        if (qt_enable_test_font && req.family == QLatin1String("__Qt__Box__Engine__")) {
            fe = new QTestFontEngine(req.pixelSize);
            fe->fontDef = req;
        } else {
            fe = findFont(script, d, req);
        }
        d->engineData->engines[script] = fe;
    }
}

QT_END_NAMESPACE
