/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testlupdate.h"
#if CHECK_SIMTEXTH
#include "../shared/simtexth.h"
#endif

#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QByteArray>

#include <QtTest/QtTest>

class tst_lupdate : public QObject
{
    Q_OBJECT
public:
    tst_lupdate() { m_basePath = QDir::currentPath() + QLatin1String("/testdata/"); }

private slots:
    void good_data();
    void good();
    void output_ts();
    void commandline_data();
    void commandline();
#if CHECK_SIMTEXTH
    void simtexth();
    void simtexth_data();
#endif

private:
    TestLUpdate m_lupdate;
    QString m_basePath;

    void doCompare(const QStringList &actual, const QString &expectedFn, bool err);
    void doCompare(const QString &actualFn, const QString &expectedFn, bool err);
};


void tst_lupdate::doCompare(const QStringList &actual, const QString &expectedFn, bool err)
{
    QFile file(expectedFn);
    QVERIFY(file.open(QIODevice::ReadOnly));
    QStringList expected = QString(file.readAll()).trimmed().remove('\r').split('\n');

    int i = 0, ei = expected.size(), gi = actual.size();
    for (; ; i++) {
        if (i == gi) {
            if (i == ei)
                return;
            gi = 0;
            break;
        } else if (i == ei) {
            ei = 0;
            break;
        } else {
            QString act = actual.at(i);
            act.remove('\r');
            if (err ? !QRegExp(expected.at(i)).exactMatch(act) :
                         (act != expected.at(i))) {
                bool cond = true;
                while (cond) {
                    act = actual.at(gi - 1);
                    act.remove('\r');
                    cond = (ei - 1) >= i && (gi - 1) >= i &&
                         (err ? QRegExp(expected.at(ei - 1)).exactMatch(act) :
                                (act == expected.at(ei - 1)));
                    if (cond) {
                        ei--, gi--;
                    }
                }
                break;
            }
        }
    }
    QByteArray diff;
    for (int j = qMax(0, i - 3); j < i; j++)
        diff += expected.at(j) + '\n';
    diff += "<<<<<<< got\n";
    for (int j = i; j < gi; j++) {
        diff += actual.at(j) + '\n';
        if (j >= i + 5) {
            diff += "...\n";
            break;
        }
    }
    diff += "=========\n";
    for (int j = i; j < ei; j++) {
        diff += expected.at(j) + '\n';
        if (j >= i + 5) {
            diff += "...\n";
            break;
        }
    }
    diff += ">>>>>>> expected\n";
    for (int j = ei; j < qMin(ei + 3, expected.size()); j++)
        diff += expected.at(j) + '\n';
    QFAIL(qPrintable((err ? "Output for " : "Result for ") + expectedFn + " does not meet expectations:\n" + diff));
}

void tst_lupdate::doCompare(const QString &actualFn, const QString &expectedFn, bool err)
{
    QFile afile(actualFn);
    QVERIFY(afile.open(QIODevice::ReadOnly));
    QStringList actual = QString(afile.readAll()).trimmed().remove('\r').split('\n');

    doCompare(actual, expectedFn, err);
}

void tst_lupdate::good_data()
{
    QTest::addColumn<QString>("directory");

    QDir parsingDir(m_basePath + "good");
    QStringList dirs = parsingDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

#ifndef Q_OS_WIN
    dirs.removeAll(QLatin1String("backslashes"));
#endif

    foreach (const QString &dir, dirs)
        QTest::newRow(dir.toLocal8Bit()) << dir;
}

void tst_lupdate::good()
{
    QFETCH(QString, directory);

    QString dir = m_basePath + "good/" + directory;
    QString expectedFile = dir + QLatin1String("/project.ts.result");

    qDebug() << "Checking...";

    // qmake will delete the previous one, to ensure that we don't do any merging....
    QString generatedtsfile(QLatin1String("project.ts"));

    m_lupdate.setWorkingDirectory(dir);
    m_lupdate.qmake();
    // look for a command
    QString lupdatecmd;
    QFile file(dir + "/lupdatecmd");
    if (file.exists()) {
        QVERIFY(file.open(QIODevice::ReadOnly));
        while (!file.atEnd()) {
            QByteArray cmdstring = file.readLine().simplified();
            if (cmdstring.startsWith('#'))
                continue;
            if (cmdstring.startsWith("lupdate")) {
                cmdstring.remove(0, 8);
                lupdatecmd.append(cmdstring);
                break;
            } else if (cmdstring.startsWith("TRANSLATION:")) {
                cmdstring.remove(0, 12);
                generatedtsfile = dir + QLatin1Char('/') + cmdstring.trimmed();
            }
        }
        file.close();
    }

    if (lupdatecmd.isEmpty()) {
        lupdatecmd = QLatin1String("project.pro -ts project.ts");
    }
    lupdatecmd.prepend("-silent ");
    m_lupdate.updateProFile(lupdatecmd);

    // If the file expectedoutput.txt exists, compare the
    // console output with the content of that file
    QFile outfile(dir + "/expectedoutput.txt");
    if (outfile.exists()) {
        QString errs = m_lupdate.getErrorMessages().at(1).trimmed();
        QStringList errslist = errs.split(QLatin1Char('\n'));
        doCompare(errslist, outfile.fileName(), true);
        if (QTest::currentTestFailed())
            return;
    }

    doCompare(generatedtsfile, expectedFile, false);
}

void tst_lupdate::output_ts()
{
    QString dir = m_basePath + "output_ts";
    m_lupdate.setWorkingDirectory(dir);

    // look for a command
    QString lupdatecmd;
    QFile file(dir + "/lupdatecmd");
    if (file.exists()) {
        QVERIFY(file.open(QIODevice::ReadOnly));
        while (!file.atEnd()) {
        QByteArray cmdstring = file.readLine().simplified();
            if (cmdstring.startsWith('#'))
                continue;
            if (cmdstring.startsWith("lupdate")) {
                cmdstring.remove(0, 8);
                lupdatecmd.append(cmdstring);
                break;
            }
        }
        file.close();
    }

    QDir parsingDir(m_basePath + "output_ts");

    QString generatedtsfile =
        dir + QLatin1String("/toplevel/library/tools/translations/project.ts");

    QFile::remove(generatedtsfile);

    lupdatecmd.prepend("-silent ");
    m_lupdate.qmake();
    m_lupdate.updateProFile(lupdatecmd);

    // If the file expectedoutput.txt exists, compare the
    // console output with the content of that file
    QFile outfile(dir + "/expectedoutput.txt");
    if (outfile.exists()) {
        QString errs = m_lupdate.getErrorMessages().at(1).trimmed();
        QStringList errslist = errs.split(QLatin1Char('\n'));
        doCompare(errslist, outfile.fileName(), true);
        if (QTest::currentTestFailed())
            return;
    }

    doCompare(generatedtsfile, dir + QLatin1String("/project.ts.result"), false);
}

void tst_lupdate::commandline_data()
{
    QTest::addColumn<QString>("currentPath");
    QTest::addColumn<QString>("commandline");
    QTest::addColumn<QString>("generatedtsfile");
    QTest::addColumn<QString>("expectedtsfile");

    QTest::newRow("Recursive scan") << QString("recursivescan")
       << QString(". -ts foo.ts") << QString("foo.ts") << QString("foo.ts.result");
    QTest::newRow("Deep path argument") << QString("recursivescan")
       << QString("sub/finddialog.cpp -ts bar.ts") << QString("bar.ts") << QString("bar.ts.result");
}

void tst_lupdate::commandline()
{
    QFETCH(QString, currentPath);
    QFETCH(QString, commandline);
    QFETCH(QString, generatedtsfile);
    QFETCH(QString, expectedtsfile);

    m_lupdate.setWorkingDirectory(m_basePath + currentPath);
    QString generated =
        m_basePath + currentPath + QLatin1Char('/') + generatedtsfile;
    QFile gen(generated);
    if (gen.exists())
        QVERIFY(gen.remove());
    if (!m_lupdate.run("-silent " + commandline))
        qDebug() << m_lupdate.getErrorMessages().last();

    doCompare(generated, m_basePath + currentPath + QLatin1Char('/') + expectedtsfile, false);
}

#if CHECK_SIMTEXTH
void tst_lupdate::simtexth()
{
    QFETCH(QString, one);
    QFETCH(QString, two);
    QFETCH(int, expected);

    int measured = getSimilarityScore(one, two.toLatin1());
    QCOMPARE(measured, expected);
}


void tst_lupdate::simtexth_data()
{
    using namespace QTest;

    addColumn<QString>("one");
    addColumn<QString>("two");
    addColumn<int>("expected");

    newRow("00") << "" << "" << 1024;
    newRow("01") << "a" << "a" << 1024;
    newRow("02") << "ab" << "ab" << 1024;
    newRow("03") << "abc" << "abc" << 1024;
    newRow("04") << "abcd" << "abcd" << 1024;
}
#endif

QTEST_MAIN(tst_lupdate)
#include "tst_lupdate.moc"
