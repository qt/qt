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
    QImage getBaseline(const QString &fileName, bool *created);
    QImage render(const QString &fileName, QImage::Format format);
    QStringList loadScriptFile(const QString &filePath);
    QString computeMismatchScore(const QImage& baseline, const QImage& rendered);
    void runTestSuite();

    QString errorMsg;
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
    int i = 0;
    foreach(const QString& file, files) {
        baseList[i].scriptName = file;
        // tbd: also load, split and generate checksums for scripts
        i++;
    }
}


void tst_Lancelot::testRasterARGB32PM_data()
{
    QTest::addColumn<ImageItem>("item");

    ImageItemList itemList(baseList);

    for(int i = 0; i < itemList.size(); i++) {
        itemList[i].engine = ImageItem::Raster;
        itemList[i].renderFormat = QImage::Format_ARGB32_Premultiplied;
    }

    if (!proto.requestBaselineChecksums(&itemList)) {
        QWARN(qPrintable(proto.errorMessage()));
        QSKIP("Communication with baseline image server failed.", SkipAll);
    }

    qDebug() << "items:" << itemList.count();
    foreach(const ImageItem& item, itemList) {
        if (item.scriptName != QLatin1String("sizes.qps")) // Hardcoded blacklisting for this enigine/format
            QTest::newRow(item.scriptName.toLatin1()) << item;
    }
}


void tst_Lancelot::testRasterARGB32PM()
{
    runTestSuite();
}


void tst_Lancelot::runTestSuite()
{
    errorMsg.clear();
    QFETCH(ImageItem, item);

    if (item.status == ImageItem::IgnoreItem)
        QSKIP("Blacklisted by baseline server.", SkipSingle);

    QImage rendered = render(item.scriptName, item.renderFormat);
    if (!errorMsg.isNull() || rendered.isNull()) {  // Assume an error in the test environment, not Qt
        QWARN("Error: Failed to render image. " + errorMsg.toLatin1());
        QSKIP("Aborted due to errors.", SkipSingle);
    }

    quint64 checksum = ImageItem::computeChecksum(rendered);

    if (item.status == ImageItem::BaselineNotFound) {
        item.image = rendered;
        item.imageChecksum = checksum;
        proto.submitNewBaseline(item);
        QSKIP("Baseline not found; new baseline created.", SkipSingle);
    }

    qDebug() << "rendered" << QString::number(checksum, 16).prepend("0x") << "baseline" << QString::number(item.imageChecksum, 16).prepend("0x");
    if (checksum != item.imageChecksum) {
        /* TBD
           - Get baseline image
           - compute diffscore
           - submit mismatching

    getBaseline(...)
    if (!errorMsg.isNull() || baseline.isNull()) {
        QWARN("Error: Failed to get baseline image. " + errorMsg.toLatin1());
        QSKIP("Aborted due to errors.", SkipSingle);
    }


    if (rendered.format() != baseline.format())
        rendered = rendered.convertToFormat(baseline.format());  //### depending on format

        QString scoreMsg = computeMismatchScore(baseline, rendered);
        QByteArray serverMsg;
        proto.submitMismatch(fileName, rendered, &serverMsg);
    */
        QByteArray failMsg = QByteArray("Rendered image differs from baseline. ");
                             //+ scoreMsg.toLatin1() + '\n' + serverMsg;
        QFAIL(failMsg.constData());
    }
}


QString tst_Lancelot::computeMismatchScore(const QImage &baseline, const QImage &rendered)
{
    if (baseline.size() != rendered.size() || baseline.format() != rendered.format())
        return QLatin1String("[No score, incomparable images.]");
    if (baseline.depth() != 32)
        return QLatin1String("[Score computation not implemented for format.]");

    int w = baseline.width();
    int h = baseline.height();

    uint ncd = 0; // number of differing color pixels
    uint nad = 0; // number of differing alpha pixels
    uint scd = 0; // sum of color pixel difference
    uint sad = 0; // sum of alpha pixel difference

    for (int y=0; y<h; ++y) {
        const QRgb *bl = (const QRgb *) baseline.constScanLine(y);
        const QRgb *rl = (const QRgb *) rendered.constScanLine(y);
        for (int x=0; x<w; ++x) {
            QRgb b = bl[x];
            QRgb r = rl[x];
            if (r != b) {
                int dr = qAbs(qRed(b) - qRed(r));
                int dg = qAbs(qGreen(b) - qGreen(r));
                int db = qAbs(qBlue(b) - qBlue(r));
                int ds = dr + dg + db;
                int da = qAbs(qAlpha(b) - qAlpha(r));
                if (ds) {
                    ncd++;
                    scd += ds;
                }
                if (da) {
                    nad++;
                    sad += da;
                }
            }
        }
    }

    double pcd = 100.0 * ncd / (w*h);  // percent of pixels that differ
    double acd = ncd ? double(scd) / (3*ncd) : 0;         // avg. difference
    QString res = QString(QLatin1String("Diffscore: %1% (Num:%2 Avg:%3)")).arg(pcd, 0, 'g', 2).arg(ncd).arg(acd, 0, 'g', 2);
    if (baseline.hasAlphaChannel()) {
        double pad = 100.0 * nad / (w*h);  // percent of pixels that differ
        double aad = nad ? double(sad) / (3*nad) : 0;         // avg. difference
        res += QString(QLatin1String(" Alpha-diffscore: %1% (Num:%2 Avg:%3)")).arg(pad, 0, 'g', 2).arg(nad).arg(aad, 0, 'g', 2);
    }
    return res;
}


QImage tst_Lancelot::getBaseline(const QString &fileName, bool *created)
{
    QImage baseline;
    //### TBD: rewrite to use ImageItem
    /*
    BaselineProtocol::Command response;
    if (!proto.requestBaseline(fileName, &response, &baseline)) {
        errorMsg = QLatin1String("Error downloading baseline from server. ") + proto.errorMessage();
        return QImage();
    }

    if (response == BaselineProtocol::BaselineNotPresent) {
        // Here: Create new baseline!
        baseline = render(fileName);
        if (!errorMsg.isNull() || baseline.isNull()) {
            errorMsg.prepend(QLatin1String("Failed to render new baseline. "));
            return QImage();
        }
        if (!proto.submitNewBaseline(fileName, baseline)) {
            errorMsg = QLatin1String("Failed to submit new baseline to server. ") + proto.errorMessage();
            return QImage();
        }
        if (created)
            *created = true;
    }
    else if (response == BaselineProtocol::IgnoreCase) {
        // Blacklisted or similar, signal back that this item should be skipped
        // Will effectively happen with this:
        errorMsg = QLatin1String("Baseline server says skip this");
        return QImage();
    }
    else if (response != BaselineProtocol::AcceptBaseline) {
        errorMsg = QLatin1String("Unknown response from baseline server. ");
        return QImage();
    }
    */
    return baseline;
}


QImage tst_Lancelot::render(const QString &fileName, QImage::Format format)
{
    QString filePath = scriptsDir + fileName;
    QStringList script = loadScriptFile(filePath);
    if (script.isEmpty())
        return QImage();

    QImage img(800, 800, format);
    QPainter p(&img);
    PaintCommands pcmd(script, 800, 800);
    pcmd.setPainter(&p);
    pcmd.setFilePath(QFileInfo(filePath).absoluteFilePath()); // eh yuck
    pcmd.runCommands();
    p.end();

    return img;
}


QStringList tst_Lancelot::loadScriptFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) {
        errorMsg = QLatin1String("Failed to open script file ") + filePath;
        return QStringList();
    }
    //# use the new readscript function instead?
    QTextStream stream(&file);
    return stream.readAll().split(QLatin1Char('\n'), QString::SkipEmptyParts);
}

QTEST_MAIN(tst_Lancelot)
#include "tst_lancelot.moc"
