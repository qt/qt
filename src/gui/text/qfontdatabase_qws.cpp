/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdir.h"
#include "qscreen_qws.h" //so we can check for rotation
#include "qwindowsystem_qws.h"
#include "qlibraryinfo.h"
#include "qabstractfileengine.h"
#include <QtCore/qsettings.h>
#if !defined(QT_NO_FREETYPE)
#include "qfontengine_ft_p.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#endif
#include "qfontengine_qpf_p.h"
#include "private/qfactoryloader_p.h"
#include "qabstractfontengine_qws.h"
#include "qabstractfontengine_p.h"
#include <qdatetime.h>

// for mmap
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#ifdef QT_FONTS_ARE_RESOURCES
#include <qresource.h>
#endif

QT_BEGIN_NAMESPACE

#ifndef QT_NO_LIBRARY
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QFontEngineFactoryInterface_iid, QLatin1String("/fontengines"), Qt::CaseInsensitive))
#endif

const quint8 DatabaseVersion = 4;

void QFontDatabasePrivate::addFont(const QString &familyname, const char *foundryname, int weight, bool italic, int pixelSize,
                                   const QByteArray &file, int fileIndex, bool antialiased,
                                   const QList<QFontDatabase::WritingSystem> &writingSystems)
{
//    qDebug() << "Adding font" << familyname << weight << italic << pixelSize << file << fileIndex << antialiased;
    QtFontStyle::Key styleKey;
    styleKey.style = italic ? QFont::StyleItalic : QFont::StyleNormal;
    styleKey.weight = weight;
    styleKey.stretch = 100;
    QtFontFamily *f = family(familyname, true);

    if (writingSystems.isEmpty()) {
        for (int ws = 1; ws < QFontDatabase::WritingSystemsCount; ++ws) {
            f->writingSystems[ws] = QtFontFamily::Supported;
        }
        f->bogusWritingSystems = true;
    } else {
        for (int i = 0; i < writingSystems.count(); ++i) {
            f->writingSystems[writingSystems.at(i)] = QtFontFamily::Supported;
        }
    }

    QtFontFoundry *foundry = f->foundry(QString::fromLatin1(foundryname), true);
    QtFontStyle *style = foundry->style(styleKey,  true);
    style->smoothScalable = (pixelSize == 0);
    style->antialiased = antialiased;
    QtFontSize *size = style->pixelSize(pixelSize?pixelSize:SMOOTH_SCALABLE, true);
    size->fileName = file;
    size->fileIndex = fileIndex;

    if (stream) {
        *stream << familyname << foundry->name << weight << quint8(italic) << pixelSize
                << file << fileIndex << quint8(antialiased);
        *stream << quint8(writingSystems.count());
        for (int i = 0; i < writingSystems.count(); ++i)
            *stream << quint8(writingSystems.at(i));
    }
}

#ifndef QT_NO_QWS_QPF2
void QFontDatabasePrivate::addQPF2File(const QByteArray &file)
{
#ifndef QT_FONTS_ARE_RESOURCES
    struct stat st;
    if (stat(file.constData(), &st))
        return;
    int f = ::open(file, O_RDONLY);
    if (f < 0)
        return;
    const uchar *data = (const uchar *)mmap(0, st.st_size, PROT_READ, MAP_SHARED, f, 0);
    const int dataSize = st.st_size;
#else
    QResource res(QLatin1String(file.constData()));
    const uchar *data = res.data();
    const int dataSize = res.size();
    //qDebug() << "addQPF2File" << file << data;
#endif
    if (data && data != (const uchar *)MAP_FAILED) {
        if (QFontEngineQPF::verifyHeader(data, dataSize)) {
            QString fontName = QFontEngineQPF::extractHeaderField(data, QFontEngineQPF::Tag_FontName).toString();
            int pixelSize = QFontEngineQPF::extractHeaderField(data, QFontEngineQPF::Tag_PixelSize).toInt();
            QVariant weight = QFontEngineQPF::extractHeaderField(data, QFontEngineQPF::Tag_Weight);
            QVariant style = QFontEngineQPF::extractHeaderField(data, QFontEngineQPF::Tag_Style);
            QByteArray writingSystemBits = QFontEngineQPF::extractHeaderField(data, QFontEngineQPF::Tag_WritingSystems).toByteArray();

            if (!fontName.isEmpty() && pixelSize) {
                int fontWeight = 50;
                if (weight.type() == QVariant::Int || weight.type() == QVariant::UInt)
                    fontWeight = weight.toInt();

                bool italic = static_cast<QFont::Style>(style.toInt()) & QFont::StyleItalic;

                QList<QFontDatabase::WritingSystem> writingSystems;
                for (int i = 0; i < writingSystemBits.count(); ++i) {
                    uchar currentByte = writingSystemBits.at(i);
                    for (int j = 0; j < 8; ++j) {
                        if (currentByte & 1)
                            writingSystems << QFontDatabase::WritingSystem(i * 8 + j);
                        currentByte >>= 1;
                    }
                }

                addFont(fontName, /*foundry*/ "prerendered", fontWeight, italic,
                        pixelSize, file, /*fileIndex*/ 0,
                        /*antialiased*/ true, writingSystems);
            }
        } else {
            qDebug() << "header verification of QPF2 font" << file << "failed. maybe it is corrupt?";
        }
#ifndef QT_FONTS_ARE_RESOURCES
        munmap((void *)data, st.st_size);
#endif
    }
#ifndef QT_FONTS_ARE_RESOURCES
    ::close(f);
#endif
}
#endif

#ifndef QT_NO_FREETYPE
QStringList QFontDatabasePrivate::addTTFile(const QByteArray &file, const QByteArray &fontData)
{
    QStringList families;
    extern FT_Library qt_getFreetype();
    FT_Library library = qt_getFreetype();

    int index = 0;
    int numFaces = 0;
    do {
        FT_Face face;
        FT_Error error;
        if (!fontData.isEmpty()) {
            error = FT_New_Memory_Face(library, (const FT_Byte *)fontData.constData(), fontData.size(), index, &face);
        } else {
            error = FT_New_Face(library, file, index, &face);
        }
        if (error != FT_Err_Ok) {
            qDebug() << "FT_New_Face failed with index" << index << ":" << hex << error;
            break;
        }
        numFaces = face->num_faces;

        int weight = QFont::Normal;
        bool italic = face->style_flags & FT_STYLE_FLAG_ITALIC;

        if (face->style_flags & FT_STYLE_FLAG_BOLD)
            weight = QFont::Bold;

        QList<QFontDatabase::WritingSystem> writingSystems;
        // detect symbol fonts
        for (int i = 0; i < face->num_charmaps; ++i) {
            FT_CharMap cm = face->charmaps[i];
            if (cm->encoding == ft_encoding_adobe_custom
                    || cm->encoding == ft_encoding_symbol) {
                writingSystems.append(QFontDatabase::Symbol);
                break;
            }
        }
        if (writingSystems.isEmpty()) {
            TT_OS2 *os2 = (TT_OS2 *)FT_Get_Sfnt_Table(face, ft_sfnt_os2);
            if (os2) {
                quint32 unicodeRange[4] = {
                    os2->ulUnicodeRange1, os2->ulUnicodeRange2, os2->ulUnicodeRange3, os2->ulUnicodeRange4
                };
                quint32 codePageRange[2] = {
                    os2->ulCodePageRange1, os2->ulCodePageRange2
                };

                writingSystems = determineWritingSystemsFromTrueTypeBits(unicodeRange, codePageRange);
                //for (int i = 0; i < writingSystems.count(); ++i)
                //    qDebug() << QFontDatabase::writingSystemName(writingSystems.at(i));
            }
        }

        QString family = QString::fromAscii(face->family_name);
        families.append(family);
        addFont(family, /*foundry*/ "", weight, italic,
                /*pixelsize*/ 0, file, index, /*antialias*/ true, writingSystems);

        FT_Done_Face(face);
        ++index;
    } while (index < numFaces);
    return families;
}
#endif

static void registerFont(QFontDatabasePrivate::ApplicationFont *fnt);

extern QString qws_fontCacheDir();

#ifndef QT_FONTS_ARE_RESOURCES
bool QFontDatabasePrivate::loadFromCache(const QString &fontPath)
{
    const bool weAreTheServer = QWSServer::instance();

    QString fontDirFile = fontPath + QLatin1String("/fontdir");

    QFile binaryDb(qws_fontCacheDir() + QLatin1String("/fontdb"));

    if (weAreTheServer) {
        QDateTime dbTimeStamp = QFileInfo(binaryDb.fileName()).lastModified();

        QDateTime fontPathTimeStamp = QFileInfo(fontPath).lastModified();
        if (dbTimeStamp < fontPathTimeStamp)
            return false; // let the caller create the cache

        if (QFile::exists(fontDirFile)) {
            QDateTime fontDirTimeStamp = QFileInfo(fontDirFile).lastModified();
            if (dbTimeStamp < fontDirTimeStamp)
                return false;
        }
    }

    if (!binaryDb.open(QIODevice::ReadOnly)) {
        if (weAreTheServer)
            return false; // let the caller create the cache
        qFatal("QFontDatabase::loadFromCache: Could not open font database cache!");
    }

    QDataStream stream(&binaryDb);
    quint8 version = 0;
    quint8 dataStreamVersion = 0;
    stream >> version >> dataStreamVersion;
    if (version != DatabaseVersion || dataStreamVersion != stream.version()) {
        if (weAreTheServer)
            return false; // let the caller create the cache
        qFatal("QFontDatabase::loadFromCache: Wrong version of the font database cache detected. Found %d/%d expected %d/%d",
               version, dataStreamVersion, DatabaseVersion, stream.version());
    }

    QString originalFontPath;
    stream >> originalFontPath;
    if (originalFontPath != fontPath) {
        if (weAreTheServer)
            return false; // let the caller create the cache
        qFatal("QFontDatabase::loadFromCache: Font path doesn't match. Found %s in database, expected %s", qPrintable(originalFontPath), qPrintable(fontPath));
    }

    QString familyname;
    stream >> familyname;
    //qDebug() << "populating database from" << binaryDb.fileName();
    while (!familyname.isEmpty() && !stream.atEnd()) {
        QString foundryname;
        int weight;
        quint8 italic;
        int pixelSize;
        QByteArray file;
        int fileIndex;
        quint8 antialiased;
        quint8 writingSystemCount;

        QList<QFontDatabase::WritingSystem> writingSystems;

        stream >> foundryname >> weight >> italic >> pixelSize
               >> file >> fileIndex >> antialiased >> writingSystemCount;

        for (quint8 i = 0; i < writingSystemCount; ++i) {
            quint8 ws;
            stream >> ws;
            writingSystems.append(QFontDatabase::WritingSystem(ws));
        }

        addFont(familyname, foundryname.toLatin1().constData(), weight, italic, pixelSize, file, fileIndex, antialiased,
                writingSystems);

        stream >> familyname;
    }

    stream >> fallbackFamilies;
    //qDebug() << "fallback families from cache:" << fallbackFamilies;
    return true;
}
#endif // QT_FONTS_ARE_RESOURCES

/*!
    \internal
*/

static QString qwsFontPath()
{
    QString fontpath = QString::fromLocal8Bit(qgetenv("QT_QWS_FONTDIR"));
    if (fontpath.isEmpty()) {
#ifdef QT_FONTS_ARE_RESOURCES
        fontpath = QLatin1String(":/qt/fonts");
#else
#ifndef QT_NO_SETTINGS
        fontpath = QLibraryInfo::location(QLibraryInfo::LibrariesPath);
        fontpath += QLatin1String("/fonts");
#else
        fontpath = QLatin1String("/lib/fonts");
#endif
#endif //QT_FONTS_ARE_RESOURCES
    }

    return fontpath;
}

#ifdef QFONTDATABASE_DEBUG
class FriendlyResource : public QResource
{
public:
    bool isDir () const { return QResource::isDir(); }
    bool isFile () const { return QResource::isFile(); }
    QStringList children () const { return QResource::children(); }
};
#endif
/*!
    \internal
*/
static void initializeDb()
{
    QFontDatabasePrivate *db = privateDb();
    if (!db || db->count)
        return;

    QString fontpath = qwsFontPath();
#ifndef QT_FONTS_ARE_RESOURCES
    QString fontDirFile = fontpath + QLatin1String("/fontdir");

    if(!QFile::exists(fontpath)) {
        qFatal("QFontDatabase: Cannot find font directory %s - is Qt installed correctly?",
               fontpath.toLocal8Bit().constData());
    }

    const bool loaded = db->loadFromCache(fontpath);

    if (db->reregisterAppFonts) {
        db->reregisterAppFonts = false;
        for (int i = 0; i < db->applicationFonts.count(); ++i)
            if (!db->applicationFonts.at(i).families.isEmpty()) {
                registerFont(&db->applicationFonts[i]);
            }
    }

    if (loaded)
        return;

    QString dbFileName = qws_fontCacheDir() + QLatin1String("/fontdb");

    QFile binaryDb(dbFileName + QLatin1String(".tmp"));
    binaryDb.open(QIODevice::WriteOnly | QIODevice::Truncate);
    db->stream = new QDataStream(&binaryDb);
    *db->stream << DatabaseVersion << quint8(db->stream->version()) << fontpath;
//    qDebug() << "creating binary database at" << binaryDb.fileName();

    // Load in font definition file
    FILE* fontdef=fopen(fontDirFile.toLocal8Bit().constData(),"r");
    if (fontdef) {
        char buf[200]="";
        char name[200]="";
        char render[200]="";
        char file[200]="";
        char isitalic[10]="";
        char flags[10]="";
        do {
            fgets(buf,200,fontdef);
            if (buf[0] != '#') {
                int weight=50;
                int size=0;
                sscanf(buf,"%s %s %s %s %d %d %s",name,file,render,isitalic,&weight,&size,flags);
                QString filename;
                if (file[0] != '/')
                    filename.append(fontpath).append(QLatin1Char('/'));
                filename += QLatin1String(file);
                bool italic = isitalic[0] == 'y';
                bool smooth = QByteArray(flags).contains('s');
                if (file[0] && QFile::exists(filename))
                    db->addFont(QString::fromUtf8(name), /*foundry*/"", weight, italic, size/10, QFile::encodeName(filename), /*fileIndex*/ 0, smooth);
            }
        } while (!feof(fontdef));
        fclose(fontdef);
    }


    QDir dir(fontpath, QLatin1String("*.qpf"));
    for (int i=0; i<int(dir.count()); i++) {
        int u0 = dir[i].indexOf(QLatin1Char('_'));
        int u1 = dir[i].indexOf(QLatin1Char('_'), u0+1);
        int u2 = dir[i].indexOf(QLatin1Char('_'), u1+1);
        int u3 = dir[i].indexOf(QLatin1Char('.'), u1+1);
        if (u2 < 0) u2 = u3;

        QString familyname = dir[i].left(u0);
        int pixelSize = dir[i].mid(u0+1,u1-u0-1).toInt()/10;
        bool italic = dir[i].mid(u2-1,1) == QLatin1String("i");
        int weight = dir[i].mid(u1+1,u2-u1-1-(italic?1:0)).toInt();

        db->addFont(familyname, /*foundry*/ "qt", weight, italic, pixelSize, QFile::encodeName(dir.absoluteFilePath(dir[i])),
                    /*fileIndex*/ 0, /*antialiased*/ true);
    }

#ifndef QT_NO_FREETYPE
    dir.setNameFilters(QStringList() << QLatin1String("*.ttf")
                       << QLatin1String("*.ttc") << QLatin1String("*.pfa")
                       << QLatin1String("*.pfb"));
    dir.refresh();
    for (int i = 0; i < int(dir.count()); ++i) {
        const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
//        qDebug() << "looking at" << file;
        db->addTTFile(file);
    }
#endif

#ifndef QT_NO_QWS_QPF2
    dir.setNameFilters(QStringList() << QLatin1String("*.qpf2"));
    dir.refresh();
    for (int i = 0; i < int(dir.count()); ++i) {
        const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
//        qDebug() << "looking at" << file;
        db->addQPF2File(file);
    }
#endif

#else //QT_FONTS_ARE_RESOURCES
#ifdef QFONTDATABASE_DEBUG
    {
        QResource fontdir(fontpath);
        FriendlyResource *fr = static_cast<FriendlyResource*>(&fontdir);
        qDebug() << "fontdir" << fr->isValid() << fr->isDir() << fr->children();

    }
#endif
    QDir dir(fontpath, QLatin1String("*.qpf2"));
    for (int i = 0; i < int(dir.count()); ++i) {
        const QByteArray file = QFile::encodeName(dir.absoluteFilePath(dir[i]));
        //qDebug() << "looking at" << file;
        db->addQPF2File(file);
    }
#endif //QT_FONTS_ARE_RESOURCES


#ifdef QFONTDATABASE_DEBUG
    // print the database
    for (int f = 0; f < db->count; f++) {
        QtFontFamily *family = db->families[f];
        FD_DEBUG("'%s' %s", qPrintable(family->name), (family->fixedPitch ? "fixed" : ""));
#if 0
        for (int i = 0; i < QFont::LastPrivateScript; ++i) {
            FD_DEBUG("\t%s: %s", qPrintable(QFontDatabase::scriptName((QFont::Script) i)),
                     ((family->scripts[i] & QtFontFamily::Supported) ? "Supported" :
                      (family->scripts[i] & QtFontFamily::UnSupported) == QtFontFamily::UnSupported ?
                      "UnSupported" : "Unknown"));
        }
#endif

        for (int fd = 0; fd < family->count; fd++) {
            QtFontFoundry *foundry = family->foundries[fd];
            FD_DEBUG("\t\t'%s'", qPrintable(foundry->name));
            for (int s = 0; s < foundry->count; s++) {
                QtFontStyle *style = foundry->styles[s];
                FD_DEBUG("\t\t\tstyle: style=%d weight=%d\n"
                         "\t\t\tstretch=%d",
                         style->key.style, style->key.weight,
                         style->key.stretch);
                if (style->smoothScalable)
                    FD_DEBUG("\t\t\t\tsmooth scalable");
                else if (style->bitmapScalable)
                    FD_DEBUG("\t\t\t\tbitmap scalable");
                if (style->pixelSizes) {
                    FD_DEBUG("\t\t\t\t%d pixel sizes",  style->count);
                    for (int z = 0; z < style->count; ++z) {
                        QtFontSize *size = style->pixelSizes + z;
                        FD_DEBUG("\t\t\t\t  size %5d",
                                  size->pixelSize);
                    }
                }
            }
        }
    }
#endif // QFONTDATABASE_DEBUG

#ifndef QT_NO_LIBRARY
    QStringList pluginFoundries = loader()->keys();
//    qDebug() << "plugin foundries:" << pluginFoundries;
    for (int i = 0; i < pluginFoundries.count(); ++i) {
        const QString foundry(pluginFoundries.at(i));

        QFontEngineFactoryInterface *factory = qobject_cast<QFontEngineFactoryInterface *>(loader()->instance(foundry));
        if (!factory) {
            qDebug() << "Could not load plugin for foundry" << foundry;
            continue;
        }

        QList<QFontEngineInfo> fonts = factory->availableFontEngines();
        for (int i = 0; i < fonts.count(); ++i) {
            QFontEngineInfo info = fonts.at(i);

            int weight = info.weight();
            if (weight <= 0)
                weight = QFont::Normal;

            db->addFont(info.family(), foundry.toLatin1().constData(),
                        weight, info.style() != QFont::StyleNormal,
                        qRound(info.pixelSize()), /*file*/QByteArray(),
                        /*fileIndex*/0, /*antiAliased*/true,
                        info.writingSystems());
        }
    }
#endif

#ifndef QT_FONTS_ARE_RESOURCES
    // the empty string/familyname signifies the end of the font list.
    *db->stream << QString();
#endif
    {
        bool coveredWritingSystems[QFontDatabase::WritingSystemsCount] = { 0 };

        db->fallbackFamilies.clear();

        for (int i = 0; i < db->count; ++i) {
            QtFontFamily *family = db->families[i];
            bool add = false;
            if (family->count == 0)
                continue;
            if (family->bogusWritingSystems)
                continue;
            for (int ws = 1; ws < QFontDatabase::WritingSystemsCount; ++ws) {
                if (coveredWritingSystems[ws])
                    continue;
                if (family->writingSystems[ws] & QtFontFamily::Supported) {
                    coveredWritingSystems[ws] = true;
                    add = true;
                }
            }
            if (add)
                db->fallbackFamilies << family->name;
        }
        //qDebug() << "fallbacks on the server:" << db->fallbackFamilies;
#ifndef QT_FONTS_ARE_RESOURCES
        *db->stream << db->fallbackFamilies;
#endif
    }
#ifndef QT_FONTS_ARE_RESOURCES
    delete db->stream;
    db->stream = 0;
    QFile::remove(dbFileName);
    binaryDb.rename(dbFileName);
#endif
}

// called from qwindowsystem_qws.cpp
void qt_qws_init_fontdb()
{
    initializeDb();
}

#ifndef QT_NO_SETTINGS
// called from qapplication_qws.cpp
void qt_applyFontDatabaseSettings(const QSettings &settings)
{
    initializeDb();
    QFontDatabasePrivate *db = privateDb();
    for (int i = 0; i < db->count; ++i) {
        QtFontFamily *family = db->families[i];
        if (settings.contains(family->name))
            family->fallbackFamilies = settings.value(family->name).toStringList();
    }

    if (settings.contains(QLatin1String("Global Fallbacks")))
        db->fallbackFamilies = settings.value(QLatin1String("Global Fallbacks")).toStringList();
}
#endif // QT_NO_SETTINGS

static inline void load(const QString & = QString(), int = -1)
{
    initializeDb();
}

#ifndef QT_NO_FREETYPE

#if (FREETYPE_MAJOR*10000+FREETYPE_MINOR*100+FREETYPE_PATCH) >= 20105
#define X_SIZE(face,i) ((face)->available_sizes[i].x_ppem)
#define Y_SIZE(face,i) ((face)->available_sizes[i].y_ppem)
#else
#define X_SIZE(face,i) ((face)->available_sizes[i].width << 6)
#define Y_SIZE(face,i) ((face)->available_sizes[i].height << 6)
#endif

#endif // QT_NO_FREETYPE

static
QFontEngine *loadSingleEngine(int script, const QFontPrivate *fp,
                              const QFontDef &request,
                              QtFontFamily *family, QtFontFoundry *foundry,
                              QtFontStyle *style, QtFontSize *size)
{
    Q_UNUSED(script);
    Q_UNUSED(fp);
#ifdef QT_NO_FREETYPE
    Q_UNUSED(foundry);
#endif
#ifdef QT_NO_QWS_QPF
    Q_UNUSED(family);
#endif
    Q_ASSERT(size);

    int pixelSize = size->pixelSize;
    if (!pixelSize || (style->smoothScalable && pixelSize == SMOOTH_SCALABLE))
        pixelSize = request.pixelSize;

#ifndef QT_NO_QWS_QPF2
    if (foundry->name == QLatin1String("prerendered")) {
#ifdef QT_FONTS_ARE_RESOURCES
        QResource res(QLatin1String(size->fileName.constData()));
        if (res.isValid()) {
            QFontEngineQPF *fe = new QFontEngineQPF(request, res.data(), res.size());
            if (fe->isValid())
                return fe;
            qDebug() << "fontengine is not valid! " << size->fileName;
            delete fe;
        } else {
            qDebug() << "Resource not valid" << size->fileName;
        }
#else
        int f = ::open(size->fileName, O_RDONLY);
        if (f >= 0) {
            QFontEngineQPF *fe = new QFontEngineQPF(request, f);
            if (fe->isValid())
                return fe;
            qDebug() << "fontengine is not valid!";
            delete fe; // will close f
        }
#endif
    } else
#endif
    if ( foundry->name != QLatin1String("qt") ) { ///#### is this the best way????
        QString file = QFile::decodeName(size->fileName);

        QFontDef def = request;
        def.pixelSize = pixelSize;

        static bool dontShareFonts = !qgetenv("QWS_NO_SHARE_FONTS").isEmpty();
        bool shareFonts = !dontShareFonts;

        QFontEngine *engine = 0;

#ifndef QT_NO_LIBRARY
        QFontEngineFactoryInterface *factory = qobject_cast<QFontEngineFactoryInterface *>(loader()->instance(foundry->name));
            if (factory) {
                QFontEngineInfo info;
                info.setFamily(request.family);
                info.setPixelSize(request.pixelSize);
                info.setStyle(QFont::Style(request.style));
                info.setWeight(request.weight);
                // #### antialiased

                QAbstractFontEngine *customEngine = factory->create(info);
                if (customEngine) {
                    engine = new QProxyFontEngine(customEngine, def);

                    if (shareFonts) {
                        QVariant hint = customEngine->fontProperty(QAbstractFontEngine::CacheGlyphsHint);
                        if (hint.isValid())
                            shareFonts = hint.toBool();
                        else
                            shareFonts = (pixelSize < 64);
                    }
                }
        }
#endif // QT_NO_LIBRARY
            if (!engine && !file.isEmpty() && QFile::exists(file) || privateDb()->isApplicationFont(file)) {
            QFontEngine::FaceId faceId;
            faceId.filename = file.toLocal8Bit();
            faceId.index = size->fileIndex;

#ifndef QT_NO_FREETYPE

            QFontEngineFT *fte = new QFontEngineFT(def);
            if (fte->init(faceId, style->antialiased,
                          style->antialiased ? QFontEngineFT::Format_A8 : QFontEngineFT::Format_Mono)) {
#ifdef QT_NO_QWS_QPF2
                return fte;
#else
                engine = fte;
                // try to distinguish between bdf and ttf fonts we can pre-render
                // and don't try to share outline fonts
                shareFonts = shareFonts
                             && !fte->defaultGlyphs()->outline_drawing
                             && !fte->getSfntTable(MAKE_TAG('h', 'e', 'a', 'd')).isEmpty();
#endif
            } else {
                delete fte;
            }
#endif // QT_NO_FREETYPE
        }
        if (engine) {
#if !defined(QT_NO_QWS_QPF2) && !defined(QT_FONTS_ARE_RESOURCES)
            if (shareFonts) {
                QFontEngineQPF *fe = new QFontEngineQPF(def, -1, engine);
                engine = 0;
                if (fe->isValid())
                    return fe;
                qWarning("Initializing QFontEngineQPF failed for %s", qPrintable(file));
                engine = fe->takeRenderingEngine();
                delete fe;
            }
#endif
            return engine;
        }
    } else
    {
#ifndef QT_NO_QWS_QPF
        QString fn = qwsFontPath();
        fn += QLatin1Char('/');
        fn += family->name.toLower()
              + QLatin1String("_") + QString::number(pixelSize*10)
              + QLatin1String("_") + QString::number(style->key.weight)
              + (style->key.style == QFont::StyleItalic ?
                 QLatin1String("i.qpf") : QLatin1String(".qpf"));
        //###rotation ###

        QFontEngine *fe = new QFontEngineQPF1(request, fn);
        return fe;
#endif // QT_NO_QWS_QPF
    }
    return new QFontEngineBox(pixelSize);
}

static
QFontEngine *loadEngine(int script, const QFontPrivate *fp,
                        const QFontDef &request,
                        QtFontFamily *family, QtFontFoundry *foundry,
                        QtFontStyle *style, QtFontSize *size)
{
    QFontEngine *fe = loadSingleEngine(script, fp, request, family, foundry,
                                       style, size);
    if (fe
        && script == QUnicodeTables::Common
        && !(request.styleStrategy & QFont::NoFontMerging) && !fe->symbol) {

        QStringList fallbacks = privateDb()->fallbackFamilies;

        if (family && !family->fallbackFamilies.isEmpty())
            fallbacks = family->fallbackFamilies;

        fe = new QFontEngineMultiQWS(fe, script, fallbacks);
    }
    return fe;
}

static void registerFont(QFontDatabasePrivate::ApplicationFont *fnt)
{
    QFontDatabasePrivate *db = privateDb();
#ifdef QT_NO_FREETYPE
    Q_UNUSED(fnt);
#else
    fnt->families = db->addTTFile(QFile::encodeName(fnt->fileName), fnt->data);
    db->fallbackFamilies += fnt->families;
#endif
    db->reregisterAppFonts = true;
}

bool QFontDatabase::removeApplicationFont(int handle)
{
    QMutexLocker locker(fontDatabaseMutex());

    QFontDatabasePrivate *db = privateDb();
    if (handle < 0 || handle >= db->applicationFonts.count())
        return false;

    db->applicationFonts[handle] = QFontDatabasePrivate::ApplicationFont();

    db->reregisterAppFonts = true;
    db->invalidate();
    return true;
}

bool QFontDatabase::removeAllApplicationFonts()
{
    QMutexLocker locker(fontDatabaseMutex());

    QFontDatabasePrivate *db = privateDb();
    if (db->applicationFonts.isEmpty())
        return false;

    db->applicationFonts.clear();
    db->invalidate();
    return true;
}

bool QFontDatabase::supportsThreadedFontRendering()
{
    return true;
}

/*!
    \internal
*/
QFontEngine *
QFontDatabase::findFont(int script, const QFontPrivate *fp,
                        const QFontDef &request)
{
    QMutexLocker locker(fontDatabaseMutex());

    const int force_encoding_id = -1;

    if (!privateDb()->count)
        initializeDb();

    QFontEngine *fe = 0;
    if (fp) {
        if (fp->rawMode) {
            fe = loadEngine(script, fp, request, 0, 0, 0, 0
                );

            // if we fail to load the rawmode font, use a 12pixel box engine instead
            if (! fe) fe = new QFontEngineBox(12);
            return fe;
        }

        QFontCache::Key key(request, script);
        fe = QFontCache::instance()->findEngine(key);
        if (fe)
            return fe;
    }

    QString family_name, foundry_name;
    QtFontStyle::Key styleKey;
    styleKey.style = request.style;
    styleKey.weight = request.weight;
    styleKey.stretch = request.stretch;
    char pitch = request.ignorePitch ? '*' : request.fixedPitch ? 'm' : 'p';

    parseFontName(request.family, foundry_name, family_name);

    FM_DEBUG("QFontDatabase::findFont\n"
             "  request:\n"
             "    family: %s [%s], script: %d\n"
             "    weight: %d, style: %d\n"
             "    stretch: %d\n"
             "    pixelSize: %d\n"
             "    pitch: %c",
             family_name.isEmpty() ? "-- first in script --" : family_name.toLatin1().constData(),
             foundry_name.isEmpty() ? "-- any --" : foundry_name.toLatin1().constData(),
             script, request.weight, request.style, request.stretch, request.pixelSize, pitch);

    if (qt_enable_test_font && request.family == QLatin1String("__Qt__Box__Engine__")) {
        fe = new QTestFontEngine(request.pixelSize);
        fe->fontDef = request;
    }

    if (!fe)
    {
        QtFontDesc desc;
        match(script, request, family_name, foundry_name, force_encoding_id, &desc);

        if (desc.family != 0 && desc.foundry != 0 && desc.style != 0
            ) {
            FM_DEBUG("  BEST:\n"
                     "    family: %s [%s]\n"
                     "    weight: %d, style: %d\n"
                     "    stretch: %d\n"
                     "    pixelSize: %d\n"
                     "    pitch: %c\n"
                     "    encoding: %d\n",
                     desc.family->name.toLatin1().constData(),
                     desc.foundry->name.isEmpty() ? "-- none --" : desc.foundry->name.toLatin1().constData(),
                     desc.style->key.weight, desc.style->key.style,
                     desc.style->key.stretch, desc.size ? desc.size->pixelSize : 0xffff,
                     'p', 0
                );

            fe = loadEngine(script, fp, request, desc.family, desc.foundry, desc.style, desc.size
                );
        } else {
            FM_DEBUG("  NO MATCH FOUND\n");
        }
        if (fe)
            initFontDef(desc, request, &fe->fontDef);
    }

    if (fe) {
        if (fp) {
            QFontDef def = request;
            if (def.family.isEmpty()) {
                def.family = fp->request.family;
                def.family = def.family.left(def.family.indexOf(QLatin1Char(',')));
            }
            QFontCache::Key key(def, script);
            QFontCache::instance()->insertEngine(key, fe);
        }

#ifndef QT_NO_FREETYPE
        if (scriptRequiresOpenType(script) && fe->type() == QFontEngine::Freetype) {
            HB_Face hbFace = static_cast<QFontEngineFT *>(fe)->harfbuzzFace();
            if (!hbFace || !hbFace->supported_scripts[script]) {
                FM_DEBUG("  OpenType support missing for script\n");
                delete fe;
                fe = 0;
            }
        }
#endif
    }

    if (!fe) {
        if (!request.family.isEmpty())
            return 0;

        FM_DEBUG("returning box engine");

        fe = new QFontEngineBox(request.pixelSize);

        if (fp) {
            QFontCache::Key key(request, script);
            QFontCache::instance()->insertEngine(key, fe);
        }
    }

    if (fp && fp->dpi > 0) {
        fe->fontDef.pointSize = qreal(double((fe->fontDef.pixelSize * 72) / fp->dpi));
    } else {
        fe->fontDef.pointSize = request.pointSize;
    }

    return fe;
}

void QFontDatabase::load(const QFontPrivate *d, int script)
{
    QFontDef req = d->request;

    if (req.pixelSize == -1)
        req.pixelSize = qRound(req.pointSize*d->dpi/72);
    if (req.pointSize < 0)
        req.pointSize = req.pixelSize*72.0/d->dpi;

    if (!d->engineData) {
        QFontCache::Key key(req, script);

        // look for the requested font in the engine data cache
        d->engineData = QFontCache::instance()->findEngineData(key);

        if (!d->engineData) {
            // create a new one
            d->engineData = new QFontEngineData;
            QFontCache::instance()->insertEngineData(key, d->engineData);
        } else {
            d->engineData->ref.ref();
        }
    }

    // the cached engineData could have already loaded the engine we want
    if (d->engineData->engines[script]) return;

    // load the font
    QFontEngine *engine = 0;
    //    double scale = 1.0; // ### TODO: fix the scale calculations

    // list of families to try
    QStringList family_list;

    if (!req.family.isEmpty()) {
        family_list = req.family.split(QLatin1Char(','));

        // append the substitute list for each family in family_list
        QStringList subs_list;
        QStringList::ConstIterator it = family_list.constBegin(), end = family_list.constEnd();
        for (; it != end; ++it)
            subs_list += QFont::substitutes(*it);
        family_list += subs_list;

        // append the default fallback font for the specified script
        // family_list << ... ; ###########

        // add the default family
        QString defaultFamily = QApplication::font().family();
        if (! family_list.contains(defaultFamily))
            family_list << defaultFamily;

        // add QFont::defaultFamily() to the list, for compatibility with
        // previous versions
        family_list << QApplication::font().defaultFamily();
    }

    // null family means find the first font matching the specified script
    family_list << QString();

    QStringList::ConstIterator it = family_list.constBegin(), end = family_list.constEnd();
    for (; ! engine && it != end; ++it) {
        req.family = *it;

        engine = QFontDatabase::findFont(script, d, req);
        if (engine) {
            if (engine->type() != QFontEngine::Box)
                break;

            if (! req.family.isEmpty())
                engine = 0;

            continue;
        }
    }

    engine->ref.ref();
    d->engineData->engines[script] = engine;
}


QT_END_NAMESPACE
