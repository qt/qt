/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtCore/QtCore>
#include <QtTest/QtTest>

class tst_Headers: public QObject
{
    Q_OBJECT
public:
    tst_Headers();

private slots:
    void initTestCase();

    void licenseCheck_data() { allSourceFilesData(); }
    void licenseCheck();

    void privateSlots_data() { allHeadersData(); }
    void privateSlots();

    void macros_data() { allHeadersData(); }
    void macros();

private:
    static QStringList getFiles(const QString &path,
                                const QStringList dirFilters,
                                const QRegExp &exclude);
    static QStringList getHeaders(const QString &path);
    static QStringList getQmlFiles(const QString &path);
    static QStringList getCppFiles(const QString &path);
    static QStringList getQDocFiles(const QString &path);

    void allSourceFilesData();
    void allHeadersData();
    QStringList headers;
    const QRegExp copyrightPattern;
    const QRegExp licensePattern;
    const QRegExp moduleTest;
    QString qtSrcDir;
};

tst_Headers::tst_Headers() :
    copyrightPattern("\\*\\* Copyright \\(C\\) 20[0-9][0-9] Nokia Corporation and/or its subsidiary\\(-ies\\)."),
    licensePattern("\\*\\* \\$QT_BEGIN_LICENSE:(LGPL|BSD|3RDPARTY|LGPL-ONLY|FDL)\\$"),
    moduleTest(QLatin1String("\\*\\* This file is part of the .+ of the Qt Toolkit."))
{
}

QStringList tst_Headers::getFiles(const QString &path,
                                  const QStringList dirFilters,
                                  const QRegExp &excludeReg)
{
    const QDir dir(path);
    const QStringList dirs(dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot));
    QStringList result;

    foreach (QString subdir, dirs)
        result += getFiles(path + "/" + subdir, dirFilters, excludeReg);

    QStringList entries = dir.entryList(dirFilters, QDir::Files);
    entries = entries.filter(excludeReg);
    foreach (QString entry, entries)
        result += path + "/" + entry;

    return result;
}

QStringList tst_Headers::getHeaders(const QString &path)
{
    return getFiles(path, QStringList("*.h"), QRegExp("^(?!ui_)"));
}

QStringList tst_Headers::getCppFiles(const QString &path)
{
    return getFiles(path, QStringList("*.cpp"), QRegExp("^(?!(moc_|qrc_))"));
}

QStringList tst_Headers::getQmlFiles(const QString &path)
{
    return getFiles(path, QStringList("*.qml"), QRegExp("."));
}

QStringList tst_Headers::getQDocFiles(const QString &path)
{
    return getFiles(path, QStringList("*.qdoc"), QRegExp("."));
}

void tst_Headers::initTestCase()
{
    qtSrcDir = QString::fromLocal8Bit(qgetenv("QTSRCDIR").isEmpty()
               ? qgetenv("QTDIR")
               : qgetenv("QTSRCDIR"));

    headers = getHeaders(qtSrcDir + "/src");

#ifndef Q_OS_WINCE
    // Windows CE does not have any headers on the test device
    QVERIFY2(!headers.isEmpty(), "No headers were found, something is wrong with the auto test setup.");
#endif

    QVERIFY(copyrightPattern.isValid());
    QVERIFY(licensePattern.isValid());
}

void tst_Headers::allSourceFilesData()
{
    QTest::addColumn<QString>("sourceFile");

    QStringList sourceFiles;
    static char const * const subdirs[] = {
        "/config.tests",
        "/demos",
        "/doc",
        "/examples",
        "/mkspecs",
        "/qmake",
        "/src",
        "/tests",
        "/tools",
        "/util"
    };

    for (int i = 0; i < sizeof(subdirs) / sizeof(subdirs[0]); ++i) {
        sourceFiles << getCppFiles(qtSrcDir + subdirs[i]);
        if (subdirs[i] != QLatin1String("/tests"))
            sourceFiles << getQmlFiles(qtSrcDir + subdirs[i]);
        sourceFiles << getHeaders(qtSrcDir + subdirs[i]);
        sourceFiles << getQDocFiles(qtSrcDir + subdirs[i]);
    }

    foreach (QString sourceFile, sourceFiles) {
        if (sourceFile.contains("/3rdparty/")
            || sourceFile.contains("/tests/auto/qmake/testdata/bundle-spaces/main.cpp")
            || sourceFile.contains("/demos/embedded/fluidlauncher/pictureflow.cpp")
            || sourceFile.contains("/tools/porting/src/")
            || sourceFile.contains("/tools/assistant/lib/fulltextsearch/")
            || sourceFile.endsWith("_pch.h.cpp")
            || sourceFile.endsWith(".ui.h")
            || sourceFile.endsWith("/src/corelib/global/qconfig.h")
            || sourceFile.endsWith("/src/corelib/global/qconfig.cpp")
            || sourceFile.endsWith("/src/tools/uic/qclass_lib_map.h"))
            continue;

        QTest::newRow(qPrintable(sourceFile)) << sourceFile;
    }
}

void tst_Headers::allHeadersData()
{
    QTest::addColumn<QString>("header");

    if (headers.isEmpty())
        QSKIP("can't find any headers in your $QTDIR/src", SkipAll);

    foreach (QString hdr, headers) {
        if (hdr.contains("/3rdparty/") || hdr.endsWith("/src/tools/uic/qclass_lib_map.h"))
            continue;

        QTest::newRow(qPrintable(hdr)) << hdr;
    }
}

void tst_Headers::licenseCheck()
{
    QFETCH(QString, sourceFile);

    QFile f(sourceFile);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QByteArray data = f.readAll();
    data.replace("\r\n", "\n"); // Windows
    data.replace('\r', '\n'); // Mac OS9
    QStringList content = QString::fromLocal8Bit(data).split("\n", QString::SkipEmptyParts);

    if (content.count() <= 2) // likely a #include line and empty line only. Not a copyright issue.
        return;

    if (content.first().contains("generated")) {
        content.takeFirst();
    }

    if (sourceFile.endsWith("/tests/auto/linguist/lupdate/testdata/good/merge_ordering/foo.cpp")
        || sourceFile.endsWith("/tests/auto/linguist/lupdate/testdata/good/mergecpp/finddialog.cpp"))
    {
        // These files are meant to start with empty lines.
        while (content.first().startsWith("//"))
            content.takeFirst();
    }

    if (sourceFile.endsWith("/doc/src/classes/phonon-api.qdoc")) {
        // This is an external file
        return;
    }

    QVERIFY(licensePattern.exactMatch(content.value(8)) ||
            licensePattern.exactMatch(content.value(5)));
    QString licenseType = licensePattern.cap(1);

    int i = 0;

    QCOMPARE(content.at(i++), QString("/****************************************************************************"));
    if (licenseType != "3RDPARTY") {
        QCOMPARE(content.at(i++), QString("**"));
        if (sourceFile.endsWith("/tests/auto/modeltest/dynamictreemodel.cpp")
            || sourceFile.endsWith("/tests/auto/modeltest/dynamictreemodel.h")
            || sourceFile.endsWith("/src/network/kernel/qnetworkproxy_p.h"))
        {
            // These files are not copyrighted by Nokia.
            ++i;
        } else {
            QVERIFY(copyrightPattern.exactMatch(content.at(i++)));
        }
        i++;
        QCOMPARE(content.at(i++), QString("** Contact: Nokia Corporation (qt-info@nokia.com)"));
    }
    QCOMPARE(content.at(i++), QString("**"));
    QVERIFY(moduleTest.exactMatch(content.at(i++)));
    QCOMPARE(content.at(i++), QString("**"));
}

void tst_Headers::privateSlots()
{
    QFETCH(QString, header);

    if (header.endsWith("/qobjectdefs.h"))
        return;

    QFile f(header);
    QVERIFY(f.open(QIODevice::ReadOnly));

    QStringList content = QString::fromLocal8Bit(f.readAll()).split("\n");
    foreach (QString line, content) {
        if (line.contains("Q_PRIVATE_SLOT("))
            QVERIFY(line.contains("_q_"));
    }
}

void tst_Headers::macros()
{
    QFETCH(QString, header);

    if (header.endsWith("_p.h") || header.endsWith("_pch.h")
        || header.contains("global/qconfig-") || header.endsWith("/qconfig.h")
        || header.contains("/src/tools/") || header.contains("/src/plugins/")
        || header.contains("/src/imports/")
        || header.endsWith("/qiconset.h") || header.endsWith("/qfeatures.h")
        || header.endsWith("qt_windows.h"))
        return;

    QFile f(header);
    QVERIFY(f.open(QIODevice::ReadOnly));

    QByteArray data = f.readAll();
    QStringList content = QString::fromLocal8Bit(data.replace('\r', "")).split("\n");

    int beginHeader = content.indexOf("QT_BEGIN_HEADER");
    int endHeader = content.lastIndexOf("QT_END_HEADER");

    QVERIFY(beginHeader >= 0);
    QVERIFY(endHeader >= 0);
    QVERIFY(beginHeader < endHeader);

    QVERIFY(content.indexOf(QRegExp("\\bslots\\s*:")) == -1);
    QVERIFY(content.indexOf(QRegExp("\\bsignals\\s*:")) == -1);

    if (header.contains("/sql/drivers/") || header.contains("/arch/qatomic")
        || header.endsWith("qglobal.h")
        || header.endsWith("qwindowdefs_win.h"))
        return;

    int qtmodule = content.indexOf(QRegExp("^QT_MODULE\\(.*\\)$"));
    QVERIFY(qtmodule != -1);
    QVERIFY(qtmodule > beginHeader);
    QVERIFY(qtmodule < endHeader);

    int beginNamespace = content.indexOf("QT_BEGIN_NAMESPACE");
    int endNamespace = content.lastIndexOf("QT_END_NAMESPACE");
    QVERIFY(beginNamespace != -1);
    QVERIFY(endNamespace != -1);
    QVERIFY(beginHeader < beginNamespace);
    QVERIFY(beginNamespace < endNamespace);
    QVERIFY(endNamespace < endHeader);
}

QTEST_MAIN(tst_Headers)
#include "tst_headers.moc"
