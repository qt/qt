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

#include <QDir>
#include <QFile>
#include <QRegExp>
#include <QStringList>
#include <QTest>
#include <QSet>
#include <QProcess>
#include <QDebug>

enum FindSubdirsMode {
    Flat = 0,
    Recursive
};

class tst_MakeTestSelfTest: public QObject
{
    Q_OBJECT

private slots:
    void tests_auto_pro();

    void tests_pro_files();
    void tests_pro_files_data();

private:
    QStringList find_subdirs(QString const&, FindSubdirsMode);
};

/*
    Verify that auto.pro only contains other .pro files (and not directories).
    We enforce this so that we can process every .pro file other than auto.pro
    independently and get all the tests.
    If tests were allowed to appear directly in auto.pro, we'd have the problem
    that we need to somehow run these tests from auto.pro while preventing
    recursion into the other .pro files.
*/
void tst_MakeTestSelfTest::tests_auto_pro()
{
    QStringList subdirsList = find_subdirs(SRCDIR "/../auto.pro", Flat);
    if (QTest::currentTestFailed()) {
        return;
    }

    foreach (QString const& subdir, subdirsList) {
        QVERIFY2(subdir.endsWith(".pro"), qPrintable(QString(
            "auto.pro contains a subdir `%1'.\n"
            "auto.pro must _only_ contain other .pro files, not actual subdirs.\n"
            "Please move `%1' into some other .pro file referenced by auto.pro."
        ).arg(subdir)));
    }
}

/* Verify that all tests are listed somewhere in one of the autotest .pro files */
void tst_MakeTestSelfTest::tests_pro_files()
{
    static QStringList lines;

    if (lines.isEmpty()) {
        QDir dir(SRCDIR "/..");
        QStringList proFiles = dir.entryList(QStringList() << "*.pro");
        foreach (QString const& proFile, proFiles) {
            QString filename = QString("%1/../%2").arg(SRCDIR).arg(proFile);
            QFile file(filename);
            if (!file.open(QIODevice::ReadOnly)) {
                QFAIL(qPrintable(QString("open %1: %2").arg(filename).arg(file.errorString())));
            }
            while (!file.atEnd()) {
                lines << file.readLine().trimmed();
            }
        }
    }

    QFETCH(QString, subdir);
    QRegExp re(QString("( |=|^|#)%1( |\\\\|$)").arg(QRegExp::escape(subdir)));
    foreach (const QString& line, lines) {
        if (re.indexIn(line) != -1) {
            return;
        }
    }



    QFAIL(qPrintable(QString(
        "Subdir `%1' is missing from tests/auto/*.pro\n"
        "This means the test won't be compiled or run on any platform.\n"
        "If this is intentional, please put the test name in a comment in one of the .pro files.").arg(subdir))
    );

}

void tst_MakeTestSelfTest::tests_pro_files_data()
{
    QTest::addColumn<QString>("subdir");
    QDir dir(SRCDIR "/..");
    QStringList subdirs = dir.entryList(QDir::AllDirs|QDir::NoDotAndDotDot);

    foreach (const QString& subdir, subdirs) {
        if (subdir == QString::fromLatin1("tmp")
            || subdir.startsWith("."))
        {
            continue;
        }
        QTest::newRow(qPrintable(subdir)) << subdir;
    }
}

/*
    Returns a list of all subdirs in a given .pro file
*/
QStringList tst_MakeTestSelfTest::find_subdirs(QString const& pro_file, FindSubdirsMode mode)
{
    QStringList out;

    QByteArray features = qgetenv("QMAKEFEATURES");

    if (features.isEmpty()) {
        features = SRCDIR "/features";
    }
    else {
        features.prepend(SRCDIR "/features:");
    }

    QStringList args;
    args << pro_file << "-o" << SRCDIR "/dummy_output" << "CONFIG+=dump_subdirs";

    QString cmd_with_args = QString("qmake %1").arg(args.join(" "));

    QProcess proc;

    proc.setProcessChannelMode(QProcess::MergedChannels);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("QMAKEFEATURES", features);
    proc.setProcessEnvironment(env);

    proc.start("qmake", args);
    if (!proc.waitForStarted(10000)) {
        QTest::qFail(qPrintable(QString("Failed to run qmake: %1\nCommand: %2")
            .arg(proc.errorString())
            .arg(cmd_with_args)),
            __FILE__, __LINE__
        );
        return out;
    }
    if (!proc.waitForFinished(30000)) {
        QTest::qFail(qPrintable(QString("qmake did not finish within 30 seconds\nCommand: %1\nOutput: %2")
            .arg(proc.errorString())
            .arg(cmd_with_args)
            .arg(QString::fromLocal8Bit(proc.readAll()))),
            __FILE__, __LINE__
        );
        return out;
    }

    if (proc.exitStatus() != QProcess::NormalExit) {
        QTest::qFail(qPrintable(QString("qmake crashed\nCommand: %1\nOutput: %2")
            .arg(cmd_with_args)
            .arg(QString::fromLocal8Bit(proc.readAll()))),
            __FILE__, __LINE__
        );
        return out;
    }

    if (proc.exitCode() != 0) {
        QTest::qFail(qPrintable(QString("qmake exited with code %1\nCommand: %2\nOutput: %3")
            .arg(proc.exitCode())
            .arg(cmd_with_args)
            .arg(QString::fromLocal8Bit(proc.readAll()))),
            __FILE__, __LINE__
        );
        return out;
    }

    QList<QByteArray> lines = proc.readAll().split('\n');
    if (!lines.count()) {
        QTest::qFail(qPrintable(QString("qmake seems to have not output anything\nCommand: %1\n")
            .arg(cmd_with_args)),
            __FILE__, __LINE__
        );
        return out;
    }

    foreach (QByteArray const& line, lines) {
        static const QByteArray marker = "Project MESSAGE: subdir: ";
        if (line.startsWith(marker)) {
            QString subdir = QString::fromLocal8Bit(line.mid(marker.size()));
            out << subdir;

            if (mode == Flat) {
                continue;
            }

            // Add subdirs recursively
            if (subdir.endsWith(".pro")) {
                // Need full path to .pro file
                QString subdir_pro = subdir;
                subdir_pro.prepend(QFileInfo(pro_file).path() + "/");
                out << find_subdirs(subdir_pro, mode);
            }
        }
    }

    return out;
}

QTEST_MAIN(tst_MakeTestSelfTest)
#include "tst_maketestselftest.moc"
