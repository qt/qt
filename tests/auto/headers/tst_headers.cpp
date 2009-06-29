/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
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
** contact the sales department at http://www.qtsoftware.com/contact.
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

    void licenseCheck_data() { allHeadersData(); }
    void licenseCheck();

    void privateSlots_data() { allHeadersData(); }
    void privateSlots();

    void macros_data() { allHeadersData(); }
    void macros();

private:
    void allHeadersData();
    QStringList headers;
    const QRegExp copyrightPattern;
    const QRegExp licensePattern;
    const QRegExp moduleTest;
};

tst_Headers::tst_Headers() :
    copyrightPattern("\\*\\* Copyright \\(C\\) 20[0-9][0-9] Nokia Corporation and/or its subsidiary\\(-ies\\)."),
    licensePattern("\\*\\* \\$QT_BEGIN_LICENSE:(LGPL|BSD|3RDPARTY)\\$"),
    moduleTest(QLatin1String("\\*\\* This file is part of the .+ of the Qt Toolkit."))
{
}

QStringList getHeaders(const QString &path)
{
    QStringList headers;

    QDir dir(path);
    QStringList dirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    foreach (QString subdir, dirs)
        headers += getHeaders(path + "/" + subdir);

    QStringList entries = dir.entryList(QStringList("*.h"), QDir::Files);
    QRegExp reg("^(?!ui_)");
    entries = entries.filter(reg);
    foreach (QString entry, entries)
        headers += path + "/" + entry;

    return headers;
}

void tst_Headers::initTestCase()
{
    QString qtSrcDir = QString::fromLocal8Bit(qgetenv("QTSRCDIR").isEmpty()
            ? qgetenv("QTDIR") : qgetenv("QTSRCDIR"));
    headers = getHeaders(qtSrcDir + "/src");

#ifndef Q_OS_WINCE
    // Windows CE does not have any headers on the test device
    QVERIFY2(!headers.isEmpty(), "No headers were found, something is wrong with the auto test setup.");
#endif

    QVERIFY(copyrightPattern.isValid());
    QVERIFY(licensePattern.isValid());
}

void tst_Headers::allHeadersData()
{
    QTest::addColumn<QString>("header");

    if (headers.isEmpty())
        QSKIP("can't find any headers in your $QTDIR/src", SkipAll);

    foreach (QString hdr, headers) {
        if (hdr.contains("/3rdparty/") || hdr.endsWith("/qclass_lib_map.h"))
            continue;

        QTest::newRow(qPrintable(hdr)) << hdr;
    }
}

void tst_Headers::licenseCheck()
{
    QFETCH(QString, header);

    if (header.endsWith("/qgifhandler.h") || header.endsWith("/qconfig.h"))
        return;

    QFile f(header);
    QVERIFY(f.open(QIODevice::ReadOnly));
    QByteArray data = f.readAll();
    QStringList content = QString::fromLocal8Bit(data.replace('\r',"")).split("\n");

    QVERIFY(licensePattern.exactMatch(content.at(7)) ||
            licensePattern.exactMatch(content.at(4)));
    QString licenseType = licensePattern.cap(1);

    int i = 0;

    QCOMPARE(content.at(i++), QString("/****************************************************************************"));
    if (licenseType != "3RDPARTY") {
        QCOMPARE(content.at(i++), QString("**"));
        QVERIFY(copyrightPattern.exactMatch(content.at(i++)));
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
