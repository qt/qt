/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "translator.h"
#include "profileevaluator.h"

#ifndef QT_BOOTSTRAPPED
#include <QtCore/QCoreApplication>
#include <QtCore/QTranslator>
#endif
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include <iostream>

QT_USE_NAMESPACE

#ifdef QT_BOOTSTRAPPED
static void initBinaryDir(
#ifndef Q_OS_WIN
        const char *argv0
#endif
        );

struct LR {
    static inline QString tr(const char *sourceText, const char *comment = 0)
    {
        return QCoreApplication::translate("LRelease", sourceText, comment);
    }
};
#else
class LR {
    Q_DECLARE_TR_FUNCTIONS(LRelease)
};
#endif

static void printOut(const QString & out)
{
    QTextStream stream(stdout);
    stream << out;
}

static void printUsage()
{
    printOut(LR::tr(
        "Usage:\n"
        "    lrelease [options] project-file\n"
        "    lrelease [options] ts-files [-qm qm-file]\n\n"
        "lrelease is part of Qt's Linguist tool chain. It can be used as a\n"
        "stand-alone tool to convert XML-based translations files in the TS\n"
        "format into the 'compiled' QM format used by QTranslator objects.\n\n"
        "Options:\n"
        "    -help  Display this information and exit\n"
        "    -idbased\n"
        "           Use IDs instead of source strings for message keying\n"
        "    -compress\n"
        "           Compress the QM files\n"
        "    -nounfinished\n"
        "           Do not include unfinished translations\n"
        "    -removeidentical\n"
        "           If the translated text is the same as\n"
        "           the source text, do not include the message\n"
        "    -markuntranslated <prefix>\n"
        "           If a message has no real translation, use the source text\n"
        "           prefixed with the given string instead\n"
        "    -silent\n"
        "           Do not explain what is being done\n"
        "    -version\n"
        "           Display the version of lrelease and exit\n"
    ));
}

static bool loadTsFile(Translator &tor, const QString &tsFileName, bool /* verbose */)
{
    ConversionData cd;
    bool ok = tor.load(tsFileName, cd, QLatin1String("auto"));
    if (!ok) {
        std::cerr << qPrintable(LR::tr("lrelease error: %1").arg(cd.error()));
    } else {
        if (!cd.errors().isEmpty())
            printOut(cd.error());
    }
    cd.clearErrors();
    return ok;
}

static bool releaseTranslator(Translator &tor, const QString &qmFileName,
    ConversionData &cd, bool removeIdentical)
{
    tor.reportDuplicates(tor.resolveDuplicates(), qmFileName, cd.isVerbose());

    if (cd.isVerbose())
        printOut(LR::tr("Updating '%1'...\n").arg(qmFileName));
    if (removeIdentical) {
        if (cd.isVerbose())
            printOut(LR::tr("Removing translations equal to source text in '%1'...\n").arg(qmFileName));
        tor.stripIdenticalSourceTranslations();
    }

    QFile file(qmFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        std::cerr << qPrintable(LR::tr("lrelease error: cannot create '%1': %2\n")
                                .arg(qmFileName, file.errorString()));
        return false;
    }

    tor.normalizeTranslations(cd);
    bool ok = tor.release(&file, cd);
    file.close();

    if (!ok) {
        std::cerr << qPrintable(LR::tr("lrelease error: cannot save '%1': %2")
                                .arg(qmFileName, cd.error()));
    } else if (!cd.errors().isEmpty()) {
        printOut(cd.error());
    }
    cd.clearErrors();
    return ok;
}

static bool releaseTsFile(const QString& tsFileName,
    ConversionData &cd, bool removeIdentical)
{
    Translator tor;
    if (!loadTsFile(tor, tsFileName, cd.isVerbose()))
        return false;

    QString qmFileName = tsFileName;
    foreach (const Translator::FileFormat &fmt, Translator::registeredFileFormats()) {
        if (qmFileName.endsWith(QLatin1Char('.') + fmt.extension)) {
            qmFileName.chop(fmt.extension.length() + 1);
            break;
        }
    }
    qmFileName += QLatin1String(".qm");

    return releaseTranslator(tor, qmFileName, cd, removeIdentical);
}

int main(int argc, char **argv)
{
#ifdef QT_BOOTSTRAPPED
    initBinaryDir(
#ifndef Q_OS_WIN
            argv[0]
#endif
            );
#else
    QCoreApplication app(argc, argv);
#ifndef Q_OS_WIN32
    QTranslator translator;
    QTranslator qtTranslator;
    QString sysLocale = QLocale::system().name();
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    if (translator.load(QLatin1String("linguist_") + sysLocale, resourceDir)
        && qtTranslator.load(QLatin1String("qt_") + sysLocale, resourceDir)) {
        app.installTranslator(&translator);
        app.installTranslator(&qtTranslator);
    }
#endif // Q_OS_WIN32
#endif // QT_BOOTSTRAPPED

    ConversionData cd;
    cd.m_verbose = true; // the default is true starting with Qt 4.2
    bool removeIdentical = false;
    Translator tor;
    QStringList inputFiles;
    QString outputFile;

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "-compress")) {
            cd.m_saveMode = SaveStripped;
            continue;
        } else if (!strcmp(argv[i], "-idbased")) {
            cd.m_idBased = true;
            continue;
        } else if (!strcmp(argv[i], "-nocompress")) {
            cd.m_saveMode = SaveEverything;
            continue;
        } else if (!strcmp(argv[i], "-removeidentical")) {
            removeIdentical = true;
            continue;
        } else if (!strcmp(argv[i], "-nounfinished")) {
            cd.m_ignoreUnfinished = true;
            continue;
        } else if (!strcmp(argv[i], "-markuntranslated")) {
            if (i == argc - 1) {
                printUsage();
                return 1;
            }
            cd.m_unTrPrefix = QString::fromLocal8Bit(argv[++i]);
        } else if (!strcmp(argv[i], "-silent")) {
            cd.m_verbose = false;
            continue;
        } else if (!strcmp(argv[i], "-verbose")) {
            cd.m_verbose = true;
            continue;
        } else if (!strcmp(argv[i], "-version")) {
            printOut(LR::tr("lrelease version %1\n").arg(QLatin1String(QT_VERSION_STR)));
            return 0;
        } else if (!strcmp(argv[i], "-qm")) {
            if (i == argc - 1) {
                printUsage();
                return 1;
            }
            outputFile = QString::fromLocal8Bit(argv[++i]);
        } else if (!strcmp(argv[i], "-help")) {
            printUsage();
            return 0;
        } else if (argv[i][0] == '-') {
            printUsage();
            return 1;
        } else {
            inputFiles << QString::fromLocal8Bit(argv[i]);
        }
    }

    if (inputFiles.isEmpty()) {
        printUsage();
        return 1;
    }

    foreach (const QString &inputFile, inputFiles) {
        if (inputFile.endsWith(QLatin1String(".pro"), Qt::CaseInsensitive)
            || inputFile.endsWith(QLatin1String(".pri"), Qt::CaseInsensitive)) {
            QFileInfo fi(inputFile);
            ProFile pro(fi.absoluteFilePath());

            ProFileEvaluator visitor;
            visitor.setVerbose(cd.isVerbose());

            if (!visitor.queryProFile(&pro)) {
                std::cerr << qPrintable(LR::tr(
                          "lrelease error: cannot read project file '%1'.\n")
                          .arg(inputFile));
                continue;
            }
            if (!visitor.accept(&pro)) {
                std::cerr << qPrintable(LR::tr(
                          "lrelease error: cannot process project file '%1'.\n")
                          .arg(inputFile));
                continue;
            }

            QStringList translations = visitor.values(QLatin1String("TRANSLATIONS"));
            if (translations.isEmpty()) {
                std::cerr << qPrintable(LR::tr(
                          "lrelease warning: Met no 'TRANSLATIONS' entry in project file '%1'\n")
                          .arg(inputFile));
            } else {
                QDir proDir(fi.absolutePath());
                foreach (const QString &trans, translations)
                    if (!releaseTsFile(QFileInfo(proDir, trans).filePath(), cd, removeIdentical))
                        return 1;
            }
        } else {
            if (outputFile.isEmpty()) {
                if (!releaseTsFile(inputFile, cd, removeIdentical))
                    return 1;
            } else {
                if (!loadTsFile(tor, inputFile, cd.isVerbose()))
                    return 1;
            }
        }
    }

    if (!outputFile.isEmpty())
        return releaseTranslator(tor, outputFile, cd, removeIdentical) ? 0 : 1;

    return 0;
}

#ifdef QT_BOOTSTRAPPED

#ifdef Q_OS_WIN
# include <windows.h>
#endif

static QString binDir;

static void initBinaryDir(
#ifndef Q_OS_WIN
        const char *_argv0
#endif
        )
{
#ifdef Q_OS_WIN
    wchar_t module_name[MAX_PATH];
    GetModuleFileName(0, module_name, MAX_PATH);
    QFileInfo filePath = QString::fromWCharArray(module_name);
    binDir = filePath.filePath();
#else
    QString argv0 = QFile::decodeName(QByteArray(_argv0));
    QString absPath;

    if (!argv0.isEmpty() && argv0.at(0) == QLatin1Char('/')) {
        /*
          If argv0 starts with a slash, it is already an absolute
          file path.
        */
        absPath = argv0;
    } else if (argv0.contains(QLatin1Char('/'))) {
        /*
          If argv0 contains one or more slashes, it is a file path
          relative to the current directory.
        */
        absPath = QDir::current().absoluteFilePath(argv0);
    } else {
        /*
          Otherwise, the file path has to be determined using the
          PATH environment variable.
        */
        QByteArray pEnv = qgetenv("PATH");
        QDir currentDir = QDir::current();
        QStringList paths = QString::fromLocal8Bit(pEnv.constData()).split(QLatin1String(":"));
        for (QStringList::const_iterator p = paths.constBegin(); p != paths.constEnd(); ++p) {
            if ((*p).isEmpty())
                continue;
            QString candidate = currentDir.absoluteFilePath(*p + QLatin1Char('/') + argv0);
            QFileInfo candidate_fi(candidate);
            if (candidate_fi.exists() && !candidate_fi.isDir()) {
                binDir = candidate_fi.canonicalPath();
                return;
            }
        }
        return;
    }

    QFileInfo fi(absPath);
    if (fi.exists())
        binDir = fi.canonicalPath();
#endif
}

QT_BEGIN_NAMESPACE

// The name is hard-coded in QLibraryInfo
QString qmake_libraryInfoFile()
{
    if (binDir.isEmpty())
        return QString();
    return QDir(binDir).filePath(QString::fromLatin1("qt.conf"));
}

QT_END_NAMESPACE

#endif // QT_BOOTSTRAPPED
