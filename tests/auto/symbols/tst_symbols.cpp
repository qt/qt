/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtCore/QtCore>
#include <QtTest/QtTest>

#ifdef QT_NAMESPACE
#define STRINGIFY_HELPER(s) #s
#define STRINGIFY(s) STRINGIFY_HELPER(s)
QString ns = STRINGIFY(QT_NAMESPACE) + QString("::");
#else
QString ns;
#endif

class tst_Symbols: public QObject
{
    Q_OBJECT
private slots:
    void prefix();
    void globalObjects();
};

/* Computes the line number from a symbol */
static QString symbolToLine(const QString &symbol, const QString &lib)
{
    // nm outputs the symbol name, the type, the address and the size
    QRegExp re("global constructors keyed to ([a-zA-Z_0-9.]*) (.) ([0-f]*) ([0-f]*)");
    if (re.indexIn(symbol) == -1)
        return QString();

    // address and symbolSize are in hex. Convert to integers
    bool ok;
    int symbolAddress = re.cap(3).toInt(&ok, 16);
    if (!ok)
        return QString();
    int symbolSize = re.cap(4).toInt(&ok, 16);
    if (!ok)
        return QString();

    // now, find the start address, which is the address - size
    QString startAddress = QString::number(symbolAddress - symbolSize, 16);

    QProcess proc;
    proc.start("addr2line", QStringList() << "-e" << lib << startAddress);
    if (!proc.waitForFinished())
        return QString();

    QString result = QString::fromLocal8Bit(proc.readLine());
    result.chop(1); // chop tailing newline
    return result;
}

/* This test searches through all Qt libraries and searches for symbols
   starting with "global constructors keyed to "

   These indicate static global objects, which should not be used in shared
   libraries - use Q_GLOBAL_STATIC instead.
*/
void tst_Symbols::globalObjects()
{
#ifndef Q_OS_LINUX
    QSKIP("Linux-specific test", SkipAll);
#endif

    // these are regexps for global objects that are allowed in Qt
    QStringList whitelist = QStringList()
        // ignore qInitResources - they are safe to use
        << "^_Z[0-9]*qInitResources_"
        << "qrc_.*\\.cpp"
        // ignore qRegisterGuiVariant - it's a safe fallback to register GUI Variants
        << "qRegisterGuiVariant";

    bool isFailed = false;

    QDir dir(QLibraryInfo::location(QLibraryInfo::LibrariesPath), "*.so");
    QStringList files = dir.entryList();
    QVERIFY(!files.isEmpty());

    foreach (QString lib, files) {
        if (lib == "libQtCLucene.so") {
            // skip this library, it's 3rd-party C++
            continue;
        }
        if (lib == "libQt3Support.so") {
            // we're not going to fix these issues anyway, so skip this library
            continue;
        }

        QProcess proc;
        proc.start("nm",
           QStringList() << "-C" << "--format=posix"
                         << dir.absolutePath() + "/" + lib);
        QVERIFY(proc.waitForFinished());
        QCOMPARE(proc.exitCode(), 0);
        QCOMPARE(QString::fromLocal8Bit(proc.readAllStandardError()), QString());

        QStringList symbols = QString::fromLocal8Bit(proc.readAll()).split("\n");
        QVERIFY(!symbols.isEmpty());
        foreach (QString symbol, symbols) {
            if (symbol.isEmpty())
                continue;

            if (!symbol.startsWith("global constructors keyed to "))
                continue;

            QRegExp re("global constructors keyed to ([a-zA-Z_0-9.]*)");
            QVERIFY(re.indexIn(symbol) != -1);

            QString cap = re.cap(1);

            bool whitelisted = false;
            foreach (QString white, whitelist) {
                if (cap.indexOf(QRegExp(white)) != -1) {
                    whitelisted = true;
                    break;
                }
            }
            if (whitelisted)
                continue;

            QString line = symbolToLine(symbol, dir.absolutePath() + "/" + lib);

            if (cap.contains('.'))
                QWARN(qPrintable("Static global object(s) found in " + lib + " in file " + cap + " (" + line + ")"));
            else
                QWARN(qPrintable("Static global object found in " + lib + " near symbol " + cap + " (" + line + ")"));

            isFailed = true;
        }
    }

    if (isFailed) {
#if QT_VERSION >= 0x040600
        QVERIFY2(!isFailed, "Libraries contain static global objects. See Debug output above.");
#else
        QSKIP("Libraries contains static global objects. See Debug output above. [These errors cannot be fixed in 4.5 in time]", SkipSingle);
#endif
    }
}

void tst_Symbols::prefix()
{
#if defined(QT_CROSS_COMPILED)
    QSKIP("Probably no compiler on the target", SkipAll);
#elif defined(Q_OS_LINUX)
    QStringList qtTypes;
    qtTypes << "QString" << "QChar" << "QWidget" << "QObject" << "QVariant" << "QList"
            << "QMap" << "QHash" << "QVector" << "QRect" << "QSize" << "QPoint"
            << "QTextFormat" << "QTextLength" << "QPen" << "QFont" << "QIcon"
            << "QPixmap" << "QImage" << "QRegion" << "QPolygon";
    QStringList qAlgorithmFunctions;
    qAlgorithmFunctions << "qBinaryFind" << "qLowerBound" << "qUpperBound"
                        << "qAbs" << "qMin" << "qMax" << "qBound" << "qSwap"
                        << "qHash" << "qDeleteAll" << "qCopy" << "qSort";

    QStringList exceptionalSymbols;
    exceptionalSymbols << "XRectangle::~XRectangle()"
                       << "XChar2b::~XChar2b()"
                       << "XPoint::~XPoint()"
                       << "glyph_metrics_t::"; // #### Qt 4.2

    QStringList stupidCSymbols;
    stupidCSymbols << "Add_Glyph_Property"
                   << "Check_Property"
                   << "Coverage_Index"
                   << "Get_Class"
                   << "Get_Device"
                   << "rcsid3"
                   << "sfnt_module_class"
                   << "t1cid_driver_class"
                   << "t42_driver_class"
                   << "winfnt_driver_class"
                   << "pshinter_module_class"
                   << "psnames_module_class"
                   // C symbols from Qt
                   << "qt_addObject"
                   << "qt_removeObject"
                   << "qt_startup_hook"
                   ;

    QHash<QString,QStringList> excusedPrefixes;
    excusedPrefixes[QString()] =
        QStringList() << "Ui_Q"; // uic generated, like Ui_QPrintDialog

    excusedPrefixes["QtCore"] =
        QStringList() << "hb_"
                      << "HB_"
                      // zlib symbols, for -qt-zlib ;(
                      << "deflate"
                      << "compress"
                      << "uncompress"
                      << "adler32"
                      << "gz"
                      << "inflate"
                      << "zlib"
                      << "zError"
                      << "get_crc_table"
                      << "crc32";

    excusedPrefixes["QtGui"] =
        QStringList() << "ftglue_"
                      << "Load_"
                      << "otl_"
                      << "TT_"
                      << "tt_"
                      << "t1_"
                      << "Free_"
                      << "FT_"
                      << "FTC_"
                      << "ft_"
                      << "ftc_"
                      << "af_autofitter"
                      << "af_dummy"
                      << "af_latin"
                      << "autofit_"
                      << "XPanorami"
                      << "Xinerama"
                      << "bdf_"
                      << "ccf_"
                      << "gray_raster"
                      << "pcf_"
                      << "cff_"
                      << "otv_"
                      << "pfr_"
                      << "ps_"
                      << "psaux"
                      << "png_";

    excusedPrefixes["QtSql"] =
        QStringList() << "sqlite3";

    excusedPrefixes["QtWebKit"] =
        QStringList() << "WebCore::"
                      << "KJS::"
                      << "kjs"
                      << "kJS"
                      << "JS"
//                      << "OpaqueJS"
                      << "WTF"
                      << "wtf_"
                      << "SVG::"
                      << "NPN_"
                      << "cti"  // ctiTrampoline and ctiVMThrowTrampoline from the JIT
#ifdef QT_NAMESPACE
                      << "QWeb" // Webkit is only 'namespace aware'
                      << "qWeb"
                      << "qt"
                      << "QGraphicsWebView"
                      << "operator"
#endif
        ;

    excusedPrefixes["phonon"] =
        QStringList() << ns + "Phonon";

    QDir dir(qgetenv("QTDIR") + "/lib", "*.so");
    QStringList files = dir.entryList();
    QVERIFY(!files.isEmpty());

    bool isFailed = false;
    foreach (QString lib, files) {
        if (lib.contains("Designer") || lib.contains("QtCLucene") || lib.contains("XmlPatternsSDK"))
            continue;

        bool isPhonon = lib.contains("phonon");

        QProcess proc;
        proc.start("nm",
           QStringList() << "-g" << "-C" << "-D" << "--format=posix"
                         << "--defined-only" << dir.absolutePath() + "/" + lib);
        QVERIFY(proc.waitForFinished());
        QCOMPARE(proc.exitCode(), 0);
        QCOMPARE(QString::fromLocal8Bit(proc.readAllStandardError()), QString());

        QStringList symbols = QString::fromLocal8Bit(proc.readAll()).split("\n");
        QVERIFY(!symbols.isEmpty());
        foreach (QString symbol, symbols) {
            if (symbol.isEmpty())
                continue;

            if (symbol.startsWith("unsigned "))
                // strip modifiers
                symbol = symbol.mid(symbol.indexOf(' ') + 1);
            if (symbol.startsWith("long long ")) {
                symbol = symbol.mid(10);
            } else if (symbol.startsWith("bool ") || symbol.startsWith("bool* ")
                || symbol.startsWith("char ") || symbol.startsWith("char* ")
                || symbol.startsWith("int ") || symbol.startsWith("int* ") || symbol.startsWith("int*&")
                || symbol.startsWith("short") || symbol.startsWith("long ")
                || symbol.startsWith("void ") || symbol.startsWith("void* ")
                || symbol.startsWith("double ") || symbol.startsWith("double* ")
                || symbol.startsWith("float ") || symbol.startsWith("float* ")) {
                // partial templates have the return type in their demangled name, strip it
                symbol = symbol.mid(symbol.indexOf(' ') + 1);
            }
            if (symbol.startsWith("const ") || symbol.startsWith("const* ") ||
                symbol.startsWith("const& ")) {
                // strip modifiers
                symbol = symbol.mid(symbol.indexOf(' ') + 1);
            }

            if (symbol.mid(symbol.indexOf(' ')+1).startsWith("std::"))
                continue;
            if (symbol.startsWith("_") || symbol.startsWith("std::"))
                continue;
            if (symbol.startsWith("vtable ") || symbol.startsWith("VTT for ") ||
                symbol.startsWith("construction vtable for"))
                continue;
            if (symbol.startsWith("typeinfo "))
                continue;
            if (symbol.startsWith("non-virtual thunk ") || symbol.startsWith("virtual thunk"))
                continue;
            if (symbol.startsWith(ns + "operator"))
                continue;
            if (symbol.startsWith("operator new") || symbol.startsWith("operator delete"))
                continue;
            if (symbol.startsWith("guard variable for "))
                continue;
            if (symbol.contains("(" + ns + "QTextStream"))
                // QTextStream is excused.
                continue;
            if (symbol.contains("(" + ns + "Q3TextStream"))
                // Q3TextStream is excused.
                continue;
            if (symbol.startsWith(ns + "bitBlt") || symbol.startsWith(ns + "copyBlt"))
                // you're excused, too
                continue;

            bool symbolOk = false;

            QHash<QString,QStringList>::ConstIterator it = excusedPrefixes.constBegin();
            for ( ; it != excusedPrefixes.constEnd(); ++it) {
                if (!lib.contains(it.key()))
                    continue;
                foreach (QString prefix, it.value())
                    if (symbol.startsWith(prefix)) {
                        symbolOk = true;
                        break;
                    }
            }

            if (symbolOk)
                continue;

            foreach (QString cSymbolPattern, stupidCSymbols)
                if (symbol.contains(cSymbolPattern)) {
                    symbolOk = true;
                    break;
                }

            if (symbolOk)
                continue;

            QStringList fields = symbol.split(' ');
            // the last two fields are address and size and the third last field is the symbol type
            QVERIFY(fields.count() > 3);
            QString type = fields.at(fields.count() - 3);
            // weak symbol
            if (type == QLatin1String("W")) {
                if (symbol.contains("qAtomic"))
                    continue;

                if (symbol.contains("fstat")
                    || symbol.contains("lstat")
                    || symbol.contains("stat64")
                   )
                    continue;

                foreach (QString acceptedPattern, qAlgorithmFunctions + exceptionalSymbols)
                    if (symbol.contains(acceptedPattern)) {
                        symbolOk = true;
                        break;
                    }

                if (symbolOk)
                    continue;

                QString plainSymbol;
                for (int i = 0; i < fields.count() - 3; ++i) {
                    if (i > 0)
                        plainSymbol += QLatin1Char(' ');
                    plainSymbol += fields.at(i);
                }
                foreach (QString qtType, qtTypes)
                    if (plainSymbol.contains(qtType)) {
                        symbolOk = true;
                        break;
                    }

                if (symbolOk)
                    continue;
            }

            QString prefix = ns + "q";
            if (!symbol.startsWith(prefix, Qt::CaseInsensitive)
                && !(isPhonon && symbol.startsWith("Phonon"))) {
                qDebug("symbol in '%s' does not start with prefix '%s': '%s'",
                    qPrintable(lib), qPrintable(prefix), qPrintable(symbol));
                isFailed = true;
            }
        }
    }

#  if defined(Q_OS_LINUX) && defined(Q_CC_INTEL)
    QEXPECT_FAIL("", "linux-icc* incorrectly exports some QtWebkit symbols, waiting for a fix from Intel.", Continue);
#  endif
    QVERIFY2(!isFailed, "Libraries contain non-prefixed symbols. See Debug output above.");
#else
    QSKIP("Linux-specific test", SkipAll);
#endif
}

QTEST_MAIN(tst_Symbols)
#include "tst_symbols.moc"
