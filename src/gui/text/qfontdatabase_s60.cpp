/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <private/qapplication_p.h>
#include "qdir.h"
#include "qfont_p.h"
#include "qfontengine_s60_p.h"
#include "qabstractfileengine.h"
#include "qdesktopservices.h"
#include <private/qpixmap_s60_p.h>
#include <private/qt_s60_p.h>
#include "qendian.h"
#include <private/qcore_symbian_p.h>
#ifdef QT_NO_FREETYPE
#include <openfont.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <graphics/openfontrasterizer.h> // COpenFontRasterizer has moved to a new header file
#endif // SYMBIAN_ENABLE_SPLIT_HEADERS
#endif // QT_NO_FREETYPE

QT_BEGIN_NAMESPACE

QStringList qt_symbian_fontFamiliesOnFontServer() // Also used in qfont_s60.cpp
{
    QStringList result;
    QSymbianFbsHeapLock lock(QSymbianFbsHeapLock::Unlock);
    const int numTypeFaces = S60->screenDevice()->NumTypefaces();
    for (int i = 0; i < numTypeFaces; i++) {
        TTypefaceSupport typefaceSupport;
        S60->screenDevice()->TypefaceSupport(typefaceSupport, i);
        const QString familyName((const QChar *)typefaceSupport.iTypeface.iName.Ptr(), typefaceSupport.iTypeface.iName.Length());
        result.append(familyName);
    }
    lock.relock();
    return result;
}

QFileInfoList alternativeFilePaths(const QString &path, const QStringList &nameFilters,
    QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort,
    bool uniqueFileNames = true)
{
    QFileInfoList result;

    // Prepare a 'soft to hard' drive list: W:, X: ... A:, Z:
    QStringList driveStrings;
    foreach (const QFileInfo &drive, QDir::drives())
        driveStrings.append(drive.absolutePath());
    driveStrings.sort();
    const QString zDriveString(QLatin1String("Z:/"));
    driveStrings.removeAll(zDriveString);
    driveStrings.prepend(zDriveString);

    QStringList uniqueFileNameList;
    for (int i = driveStrings.count() - 1; i >= 0; --i) {
        const QDir dirOnDrive(driveStrings.at(i) + path);
        const QFileInfoList entriesOnDrive = dirOnDrive.entryInfoList(nameFilters, filters, sort);
        if (uniqueFileNames) {
            foreach(const QFileInfo &entry, entriesOnDrive) {
                if (!uniqueFileNameList.contains(entry.fileName())) {
                    uniqueFileNameList.append(entry.fileName());
                    result.append(entry);
                }
            }
        } else {
            result.append(entriesOnDrive);
        }
    }
    return result;
}

#ifdef QT_NO_FREETYPE
class QSymbianFontDatabaseExtrasImplementation : public QSymbianFontDatabaseExtras
{
public:
    QSymbianFontDatabaseExtrasImplementation();
    ~QSymbianFontDatabaseExtrasImplementation();

    const QSymbianTypeFaceExtras *extras(const QString &typeface, bool bold, bool italic) const;
    void addFontFileToFontStore(const QFileInfo &fontFileInfo);

#ifndef Q_SYMBIAN_HAS_FONTTABLE_API
    struct CFontFromFontStoreReleaser {
        static inline void cleanup(CFont *font)
        {
            if (!font)
                return;
            const QSymbianFontDatabaseExtrasImplementation *dbExtras =
                    static_cast<const QSymbianFontDatabaseExtrasImplementation*>(privateDb()->symbianExtras);
            dbExtras->m_store->ReleaseFont(font);
        }
    };
#endif // !Q_SYMBIAN_HAS_FONTTABLE_API

    struct CFontFromScreenDeviceReleaser {
        static inline void cleanup(CFont *font)
        {
            if (!font)
                return;
            S60->screenDevice()->ReleaseFont(font);
        }
    };

#ifndef Q_SYMBIAN_HAS_FONTTABLE_API
    RHeap* m_heap;
    CFontStore *m_store;
    COpenFontRasterizer *m_rasterizer;
    mutable QList<const QSymbianTypeFaceExtras *> m_extras;
#endif // !Q_SYMBIAN_HAS_FONTTABLE_API
    mutable QHash<QString, const QSymbianTypeFaceExtras *> m_extrasHash;
};

QSymbianFontDatabaseExtrasImplementation::QSymbianFontDatabaseExtrasImplementation()
{
#ifndef Q_SYMBIAN_HAS_FONTTABLE_API
    QStringList filters;
    filters.append(QLatin1String("*.ttf"));
    filters.append(QLatin1String("*.ccc"));
    filters.append(QLatin1String("*.ltt"));
    const QFileInfoList fontFiles = alternativeFilePaths(QLatin1String("resource\\Fonts"), filters);

    const TInt heapMinLength = 0x1000;
    const TInt heapMaxLength = qMax(0x20000 * fontFiles.count(), heapMinLength);
    m_heap = User::ChunkHeap(NULL, heapMinLength, heapMaxLength);
    QT_TRAP_THROWING(
        m_store = CFontStore::NewL(m_heap);
        m_rasterizer = COpenFontRasterizer::NewL(TUid::Uid(0x101F7F5E));
        CleanupStack::PushL(m_rasterizer);
        m_store->InstallRasterizerL(m_rasterizer);
        CleanupStack::Pop(m_rasterizer););

    foreach (const QFileInfo &fontFileInfo, fontFiles)
        addFontFileToFontStore(fontFileInfo);
#endif // !Q_SYMBIAN_HAS_FONTTABLE_API
}

void qt_cleanup_symbianFontDatabaseExtras()
{
    const QSymbianFontDatabaseExtrasImplementation *dbExtras =
            static_cast<const QSymbianFontDatabaseExtrasImplementation*>(privateDb()->symbianExtras);
    if (!dbExtras)
        return; // initializeDb() has never been called
#ifdef Q_SYMBIAN_HAS_FONTTABLE_API
    qDeleteAll(dbExtras->m_extrasHash);
#else // Q_SYMBIAN_HAS_FONTTABLE_API
    typedef QList<const QSymbianTypeFaceExtras *>::iterator iterator;
    for (iterator p = dbExtras->m_extras.begin(); p != dbExtras->m_extras.end(); ++p) {
        dbExtras->m_store->ReleaseFont((*p)->fontOwner());
        delete *p;
    }
    dbExtras->m_extras.clear();
#endif // Q_SYMBIAN_HAS_FONTTABLE_API
    dbExtras->m_extrasHash.clear();
}

QSymbianFontDatabaseExtrasImplementation::~QSymbianFontDatabaseExtrasImplementation()
{
    qt_cleanup_symbianFontDatabaseExtras();
#ifndef Q_SYMBIAN_HAS_FONTTABLE_API
    delete m_store;
    m_heap->Close();
#endif // !Q_SYMBIAN_HAS_FONTTABLE_API
}

#ifndef FNTSTORE_H_INLINES_SUPPORT_FMM
/*
 Workaround: fntstore.h has an inlined function 'COpenFont* CBitmapFont::OpenFont()'
 that returns a private data member. The header will change between SDKs. But Qt has
 to build on any SDK version and run on other versions of Symbian OS.
 This function performs the needed pointer arithmetic to get the right COpenFont*
*/
COpenFont* OpenFontFromBitmapFont(const CBitmapFont* aBitmapFont)
{
    const TInt offsetIOpenFont = 92; // '_FOFF(CBitmapFont, iOpenFont)' ..if iOpenFont weren't private
    const TUint valueIOpenFont = *(TUint*)PtrAdd(aBitmapFont, offsetIOpenFont);
    return (valueIOpenFont & 1) ?
            (COpenFont*)PtrAdd(aBitmapFont, valueIOpenFont & ~1) : // New behavior: iOpenFont is offset
            (COpenFont*)valueIOpenFont; // Old behavior: iOpenFont is pointer
}
#endif // FNTSTORE_H_INLINES_SUPPORT_FMM

const QSymbianTypeFaceExtras *QSymbianFontDatabaseExtrasImplementation::extras(const QString &typeface,
                                                                               bool bold, bool italic) const
{
    const QString searchKey = typeface + QString::number(int(bold)) + QString::number(int(italic));
    if (!m_extrasHash.contains(searchKey)) {
        TFontSpec searchSpec(qt_QString2TPtrC(typeface), 1);
        if (bold)
            searchSpec.iFontStyle.SetStrokeWeight(EStrokeWeightBold);
        if (italic)
            searchSpec.iFontStyle.SetPosture(EPostureItalic);

        CFont* font = NULL;
#ifdef Q_SYMBIAN_HAS_FONTTABLE_API
        const TInt err = S60->screenDevice()->GetNearestFontToDesignHeightInPixels(font, searchSpec);
        Q_ASSERT(err == KErrNone && font);
        QScopedPointer<CFont, CFontFromScreenDeviceReleaser> sFont(font);
        QSymbianTypeFaceExtras *extras = new QSymbianTypeFaceExtras(font);
        sFont.take();
        m_extrasHash.insert(searchKey, extras);
#else // Q_SYMBIAN_HAS_FONTTABLE_API
        const TInt err = m_store->GetNearestFontToDesignHeightInPixels(font, searchSpec);
        Q_ASSERT(err == KErrNone && font);
        const CBitmapFont *bitmapFont = static_cast<CBitmapFont*>(font);
        COpenFont *openFont =
#ifdef FNTSTORE_H_INLINES_SUPPORT_FMM
            bitmapFont->OpenFont();
#else // FNTSTORE_H_INLINES_SUPPORT_FMM
            OpenFontFromBitmapFont(bitmapFont);
#endif // FNTSTORE_H_INLINES_SUPPORT_FMM
        const TOpenFontFaceAttrib* const attrib = openFont->FaceAttrib();
        const QString foundKey =
                QString((const QChar*)attrib->FullName().Ptr(), attrib->FullName().Length());
        if (!m_extrasHash.contains(foundKey)) {
            QScopedPointer<CFont, CFontFromFontStoreReleaser> sFont(font);
            QSymbianTypeFaceExtras *extras = new QSymbianTypeFaceExtras(font, openFont);
            sFont.take();
            m_extras.append(extras);
            m_extrasHash.insert(searchKey, extras);
            m_extrasHash.insert(foundKey, extras);
        } else {
            m_store->ReleaseFont(font);
            m_extrasHash.insert(searchKey, m_extrasHash.value(foundKey));
        }
#endif // Q_SYMBIAN_HAS_FONTTABLE_API
    }
    return m_extrasHash.value(searchKey);
}

void QSymbianFontDatabaseExtrasImplementation::addFontFileToFontStore(const QFileInfo &fontFileInfo)
{
    const QString fontFile = QDir::toNativeSeparators(fontFileInfo.absoluteFilePath());
    TPtrC fontFilePtr(qt_QString2TPtrC(fontFile));
    QT_TRAP_THROWING(m_store->AddFileL(fontFilePtr));
}

#else // QT_NO_FREETYPE
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
#endif // QT_NO_FREETYPE

/*
 QFontEngineS60::pixelsToPoints, QFontEngineS60::pointsToPixels, QFontEngineMultiS60::QFontEngineMultiS60
 and QFontEngineMultiS60::QFontEngineMultiS60 should be in qfontengine_s60.cpp. But since also the
 Freetype based font rendering need them, they are here.
*/
qreal QFontEngineS60::pixelsToPoints(qreal pixels, Qt::Orientation orientation)
{
    CWsScreenDevice* device = S60->screenDevice();
    return (orientation == Qt::Horizontal?
        device->HorizontalPixelsToTwips(pixels)
        :device->VerticalPixelsToTwips(pixels)) / KTwipsPerPoint;
}

qreal QFontEngineS60::pointsToPixels(qreal points, Qt::Orientation orientation)
{
    CWsScreenDevice* device = S60->screenDevice();
    const int twips = points * KTwipsPerPoint;
    return orientation == Qt::Horizontal?
        device->HorizontalTwipsToPixels(twips)
        :device->VerticalTwipsToPixels(twips);
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

static bool addFontToScreenDevice(int screenDeviceFontIndex,
                                  const QSymbianFontDatabaseExtrasImplementation *dbExtras)
{    
    TTypefaceSupport typefaceSupport;
        S60->screenDevice()->TypefaceSupport(typefaceSupport, screenDeviceFontIndex);
    CFont *font; // We have to get a font instance in order to know all the details
    TFontSpec fontSpec(typefaceSupport.iTypeface.iName, 11);
    if (S60->screenDevice()->GetNearestFontInPixels(font, fontSpec) != KErrNone)
        return false;
    QScopedPointer<CFont, QSymbianFontDatabaseExtrasImplementation::CFontFromScreenDeviceReleaser> sFont(font);
    if (font->TypeUid() != KCFbsFontUid)
        return false;
    TOpenFontFaceAttrib faceAttrib;
    const CFbsFont *cfbsFont = static_cast<const CFbsFont *>(font);
    cfbsFont->GetFaceAttrib(faceAttrib);

    QtFontStyle::Key styleKey;
    styleKey.style = faceAttrib.IsItalic()?QFont::StyleItalic:QFont::StyleNormal;
    styleKey.weight = faceAttrib.IsBold()?QFont::Bold:QFont::Normal;

    QString familyName((const QChar *)typefaceSupport.iTypeface.iName.Ptr(), typefaceSupport.iTypeface.iName.Length());
    QtFontFamily *family = privateDb()->family(familyName, true);
    family->fixedPitch = faceAttrib.IsMonoWidth();
    QtFontFoundry *foundry = family->foundry(QString(), true);
    QtFontStyle *style = foundry->style(styleKey, true);
    style->smoothScalable = typefaceSupport.iIsScalable;
    style->pixelSize(0, true);

    const QSymbianTypeFaceExtras *typeFaceExtras =
            dbExtras->extras(familyName, faceAttrib.IsBold(), faceAttrib.IsItalic());
    const QByteArray os2Table = typeFaceExtras->getSfntTable(MAKE_TAG('O', 'S', '/', '2'));
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
    return true;
}

static void initializeDb()
{
    QFontDatabasePrivate *db = privateDb();
    if(!db || db->count)
        return;

#ifdef QT_NO_FREETYPE
    if (!db->symbianExtras)
        db->symbianExtras = new QSymbianFontDatabaseExtrasImplementation;

    QSymbianFbsHeapLock lock(QSymbianFbsHeapLock::Unlock);

    const int numTypeFaces = S60->screenDevice()->NumTypefaces();
    const QSymbianFontDatabaseExtrasImplementation *dbExtras =
            static_cast<const QSymbianFontDatabaseExtrasImplementation*>(db->symbianExtras);
    for (int i = 0; i < numTypeFaces; i++)
        addFontToScreenDevice(i, dbExtras);

    lock.relock();

#else // QT_NO_FREETYPE
    QDir dir(QDesktopServices::storageLocation(QDesktopServices::FontsLocation));
    dir.setNameFilters(QStringList() << QLatin1String("*.ttf")
                       << QLatin1String("*.ttc") << QLatin1String("*.pfa")
                       << QLatin1String("*.pfb"));
    for (int i = 0; i < int(dir.count()); ++i) {
        const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
        db->addTTFile(file);
    }
#endif // QT_NO_FREETYPE
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

bool QFontDatabase::removeApplicationFont(int handle)
{
    Q_UNUSED(handle);
    return false;
}

bool QFontDatabase::removeAllApplicationFonts()
{
    return false;
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

QFontEngine *QFontDatabase::findFont(int script, const QFontPrivate *d, const QFontDef &req)
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
#ifdef QT_NO_FREETYPE
        const QSymbianFontDatabaseExtrasImplementation *dbExtras =
                static_cast<const QSymbianFontDatabaseExtrasImplementation*>(db->symbianExtras);
        const QSymbianTypeFaceExtras *typeFaceExtras =
                dbExtras->extras(fontFamily, request.weight > QFont::Normal, request.style != QFont::StyleNormal);

        // We need a valid pixelSize, e.g. for lineThickness()
        if (request.pixelSize < 0)
            request.pixelSize = request.pointSize * d->dpi / 72;

        fe = new QFontEngineS60(request, typeFaceExtras);
#else // QT_NO_FREETYPE
        Q_UNUSED(d)
        QFontEngine::FaceId faceId;
        const QtFontFamily * const reqQtFontFamily = db->family(fontFamily);
        faceId.filename = reqQtFontFamily->fontFilename;
        faceId.index = reqQtFontFamily->fontFileIndex;

        QFontEngineFTS60 *fte = new QFontEngineFTS60(cleanedFontDef(request));
        if (fte->init(faceId, true, QFontEngineFT::Format_A8))
            fe = fte;
        else
            delete fte;
#endif // QT_NO_FREETYPE

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
