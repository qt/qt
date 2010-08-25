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
    QImage render(const QString &fileName);
    QStringList loadScriptFile(const QString &filePath);

    QString errorMsg;
    BaselineProtocol proto;

private slots:
    void initTestCase();
    void cleanupTestCase() {}

    void testRendering_data();
    void testRendering();
};

tst_Lancelot::tst_Lancelot()
{
}

void tst_Lancelot::initTestCase()
{
#if !defined(Q_OS_LINUX)
    QSKIP("For the moment, this test is only supported on Linux.", SkipAll);
#endif

    // Check the environment here, so that we can just skip on trouble!
    if (!proto.connect()) {
        QWARN(qPrintable(proto.errorMessage()));
        QSKIP("Communication with baseline image server failed.", SkipAll);
    }

    // TBD: preload scripts, to confirm reading and generate checksums
}

void tst_Lancelot::testRendering_data()
{
    // search the data directory for qps files and make a row for each.
    // filter out those marked as blacklisted (in the file itself?)
    // Rather do the above in initTestCase. Can load & split them all also.
    // And baselines too?

    QTest::addColumn<QString>("fileName");

    // ### no, don't do it like this, we want opt-in, not opt-out, for stability
    QDir qpsDir(scriptsDir);
    QStringList files = qpsDir.entryList(QStringList() << QLatin1String("*.qps"), QDir::Files | QDir::Readable);
    files.removeOne(QLatin1String("porter_duff.qps"));
    files.removeOne(QLatin1String("porter_duff2.qps"));
    files.removeOne(QLatin1String("images.qps"));
    files.removeOne(QLatin1String("sizes.qps"));

    if (files.isEmpty()) {
        QWARN("No qps files found in " + qpsDir.path().toLatin1());
        QSKIP("Aborted due to errors.", SkipAll);
    }

    foreach(QString fileName, files) {
        QString baseName = fileName.section('.', 0, -2);
        QTest::newRow(baseName.toLatin1()) << fileName;
    }
}


// We treat failures because of test environment
// (e.g. script files not found) as just warnings, and not
// QFAILs, to avoid false negatives caused by environment
// or server instability

void tst_Lancelot::testRendering()
{
    errorMsg.clear();
    QFETCH(QString, fileName);

    bool newBaselineCreated = false;
    QImage baseline = getBaseline(fileName, &newBaselineCreated);
    if (newBaselineCreated)
        QSKIP("Baseline not found; new baseline created.", SkipSingle);
    if (!errorMsg.isNull() || baseline.isNull()) {
        QWARN("Error: Failed to get baseline image. " + errorMsg.toLatin1());
        QSKIP("Aborted due to errors.", SkipSingle);
    }

    QImage rendered = render(fileName);
    if (!errorMsg.isNull() || rendered.isNull()) {
        // Indicates that it's an error in the test environment, not Qt
        QWARN("Error: Failed to render image. " + errorMsg.toLatin1());
        QSKIP("Aborted due to errors.", SkipSingle);
    }

    if (rendered.format() != baseline.format())
        baseline = baseline.convertToFormat(rendered.format());

    // The actual check:
    if (rendered != baseline) {
        QByteArray failMsg;
        proto.submitMismatch(fileName, rendered, &failMsg);
        failMsg.prepend("Rendered image differs from baseline. ");
        QFAIL(failMsg.constData());
    }
}


QImage tst_Lancelot::getBaseline(const QString &fileName, bool *created)
{
    QImage baseline;
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
    return baseline;
}


QImage tst_Lancelot::render(const QString &fileName)
{
    QString filePath = scriptsDir + fileName;
    QStringList script = loadScriptFile(filePath);
    if (script.isEmpty())
        return QImage();

    QImage img(800, 800, QImage::Format_ARGB32_Premultiplied);
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
