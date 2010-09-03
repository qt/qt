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

// Generalization dimensions TBD:
// ImageFormat: Rendering to different bit formats
// Engine: Rendering with different engines
// Painter renderhints? (set in qps itself)

#include <QtTest/QtTest>
#include <paintcommands.h>
#include <QPainter>
#include <QLibraryInfo>
#include <baselineprotocol.h>

#ifndef SRCDIR
#define SRCDIR "."
#endif

static const QLatin1String scriptsDir(SRCDIR "/scripts/");

class tst_Lancelot : public QObject
{
Q_OBJECT

public:
    tst_Lancelot();

private:
    ImageItem render(const ImageItem &item);
    QStringList loadScriptFile(const QString &filePath);
    void runTestSuite();

    BaselineProtocol proto;
    ImageItemList baseList;

private slots:
    void initTestCase();
    void cleanupTestCase() {}

    void testRasterARGB32PM_data();
    void testRasterARGB32PM();
};

tst_Lancelot::tst_Lancelot()
{
}

void tst_Lancelot::initTestCase()
{
    // Check and setup the environment. We treat failures because of test environment
    // (e.g. script files not found) as just warnings, and not QFAILs, to avoid false negatives
    // caused by environment or server instability

#if !defined(Q_OS_LINUX)
    QSKIP("For the moment, this test is only supported on Linux.", SkipAll);
#endif
    if (!proto.connect()) {
        QWARN(qPrintable(proto.errorMessage()));
        QSKIP("Communication with baseline image server failed.", SkipAll);
    }

    QDir qpsDir(scriptsDir);
    QStringList files = qpsDir.entryList(QStringList() << QLatin1String("*.qps"), QDir::Files | QDir::Readable);
    if (files.isEmpty()) {
        QWARN("No qps script files found in " + qpsDir.path().toLatin1());
        QSKIP("Aborted due to errors.", SkipAll);
    }

    baseList.resize(files.count());
    ImageItemList::iterator it = baseList.begin();
    foreach(const QString& file, files) {
        it->scriptName = file;
        // tbd: also load, split and generate checksums for scripts
        it++;
    }
}


void tst_Lancelot::testRasterARGB32PM_data()
{
    QTest::addColumn<ImageItem>("baseline");

    ImageItemList itemList(baseList);

    for(ImageItemList::iterator it = itemList.begin(); it != itemList.end(); it++) {
        it->engine = ImageItem::Raster;
        it->renderFormat = QImage::Format_ARGB32_Premultiplied;
    }

    if (!proto.requestBaselineChecksums(&itemList)) {
        QWARN(qPrintable(proto.errorMessage()));
        QSKIP("Communication with baseline image server failed.", SkipAll);
    }

    foreach(const ImageItem& item, itemList) {
        if (item.scriptName != QLatin1String("sizes.qps")) // Example of hardcoded blacklisting for this enigine/format
            QTest::newRow(item.scriptName.toLatin1()) << item;
    }
}


void tst_Lancelot::testRasterARGB32PM()
{
    runTestSuite();
}


void tst_Lancelot::runTestSuite()
{
    QFETCH(ImageItem, baseline);

    if (baseline.status == ImageItem::IgnoreItem)
        QSKIP("Blacklisted by baseline server.", SkipSingle);

    ImageItem rendered = render(baseline);
    if (rendered.image.isNull()) {    // Assume an error in the test environment, not Qt
        QWARN("Error: Failed to render image.");
        QSKIP("Aborted due to errors.", SkipSingle);
    }

    if (baseline.status == ImageItem::BaselineNotFound) {
        proto.submitNewBaseline(rendered, 0);
        QSKIP("Baseline not found; new baseline created.", SkipSingle);
    }

    if (rendered.imageChecksum != baseline.imageChecksum) {
        QByteArray serverMsg;
        if (!proto.submitMismatch(rendered, &serverMsg))
            serverMsg = "Failed to submit mismatching image to server.";
        QFAIL("Rendered image differs from baseline.\n" + serverMsg);
    }
}


ImageItem tst_Lancelot::render(const ImageItem &item)
{
    ImageItem res = item;
    QString filePath = scriptsDir + item.scriptName;
    QStringList script = loadScriptFile(filePath);
    if (script.isEmpty()) {
        res.image = QImage();
        res.imageChecksum = 0;
    } else {
        QImage img(800, 800, item.renderFormat);
        QPainter p(&img);
        PaintCommands pcmd(script, 800, 800);
        pcmd.setPainter(&p);
        pcmd.setFilePath(QFileInfo(filePath).absoluteFilePath()); // eh yuck
        pcmd.runCommands();
        p.end();
        res.image = img;
        res.imageChecksum = ImageItem::computeChecksum(img);
    }
    return res;
}


QStringList tst_Lancelot::loadScriptFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        return QStringList();
    }
    //# use the new readscript function instead?
    QTextStream stream(&file);
    return stream.readAll().split(QLatin1Char('\n'), QString::SkipEmptyParts);
}

QTEST_MAIN(tst_Lancelot)
#include "tst_lancelot.moc"
