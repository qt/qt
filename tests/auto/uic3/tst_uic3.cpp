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


#include <QtCore/QDir>
#include <QtTest/QtTest>
#include <QtCore/QProcess>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QLibraryInfo>

#ifndef Q_OS_WINCE

class tst_uic3 : public QObject
{
    Q_OBJECT

public:
    tst_uic3();
    
private Q_SLOTS:
    void initTestCase();
    
    void convert();
    void convert_data() const;

private:
    QString workingDir() const;

private:
    bool uic3Exists;
    QString command;
};


tst_uic3::tst_uic3()
    : uic3Exists(true)
    , command(QLibraryInfo::location(QLibraryInfo::BinariesPath) + QLatin1String("/uic3"))
{
}

void tst_uic3::initTestCase()
{
    QProcess process;
    process.start(command, QStringList(QLatin1String("-help")));
    if (!process.waitForFinished()) {
        uic3Exists = false;
        const QString path = QString::fromLocal8Bit(qgetenv("PATH"));
        QString message = QString::fromLatin1("'%1' could not be found when run from '%2'. Path: '%3' ").
                          arg(command, QDir::currentPath(), path);
        QFAIL(qPrintable(message));
    }
    // Print version
    const QString out = QString::fromLocal8Bit(process.readAllStandardError()).remove(QLatin1Char('\r'));
    const QStringList outLines = out.split(QLatin1Char('\n'));
    QString msg = QString::fromLatin1("uic3 test built %1 running in '%2' using: ").
                  arg(QString::fromAscii(__DATE__), QDir::currentPath());
    if (!outLines.empty())
        msg += outLines.front();
    qDebug() << msg;
    process.terminate();

    QCOMPARE(QFileInfo(QLatin1String("baseline")).exists(), true);
    QCOMPARE(QFileInfo(QLatin1String("generated")).exists(), true);
}

void tst_uic3::convert()
{
    if (!uic3Exists)
        QSKIP("uic3 not found in the path...", SkipAll);

    QFETCH(QString, originalFile);
    QFETCH(QString, generatedFile);
    QFETCH(QString, baseFile);
    QFETCH(QString, errorFile);

    QProcess process;
    process.setWorkingDirectory(workingDir());

    process.start(command, QStringList("-convert") << originalFile
                  << QString(QLatin1String("-o")) << generatedFile);


    QCOMPARE(process.exitStatus(), QProcess::NormalExit);

    if (process.waitForFinished()) {
        QCOMPARE(process.exitCode(), 0);
        QCOMPARE(QFileInfo(generatedFile).exists(), true);
    } else {
        QString error(QLatin1String("could not convert file: "));
        QFAIL(error.append(generatedFile).toUtf8().constData());
    }

    QByteArray errorOutput = process.readAllStandardError();

    // Compare generated file to baseline

    QFile bFile(baseFile);
    QFile gFile(generatedFile);
    QFile eFile(errorFile);

    if (!bFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString err(QLatin1String("Could not read file: %1..."));
        QFAIL(err.arg(bFile.fileName()).toUtf8());
    }

    if (!gFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString err(QLatin1String("Could not read file: %1..."));
        QFAIL(err.arg(gFile.fileName()).toUtf8());
    }

    if (!eFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString err(QLatin1String("Could not read file: %1..."));
        QFAIL(err.arg(eFile.fileName()).toUtf8());
    }

    QCOMPARE(QString(bFile.readAll()), QString(gFile.readAll()));
    QCOMPARE(QString(eFile.readAll()), QString::fromLocal8Bit(errorOutput).remove(QLatin1Char('\r')));
}

void tst_uic3::convert_data() const
{
    QTest::addColumn<QString>("originalFile");
    QTest::addColumn<QString>("generatedFile");
    QTest::addColumn<QString>("baseFile");
    QTest::addColumn<QString>("errorFile");

    QString cwd = workingDir().append(QDir::separator());

    QDir dir(cwd + QLatin1String("baseline"));
    QFileInfoList originalFiles = dir.entryInfoList(QStringList("*.ui"), QDir::Files);

    dir.setPath(cwd + QLatin1String("generated"));
    for (int i = 0; i < originalFiles.count(); ++i) {
        QTest::newRow(qPrintable(originalFiles.at(i).baseName()))
            << originalFiles.at(i).absoluteFilePath()
            << dir.absolutePath() + QDir::separator()
                + originalFiles.at(i).fileName().append(QLatin1String(".4"))
            << originalFiles.at(i).absoluteFilePath().append(QLatin1String(".4"))
            << originalFiles.at(i).absoluteFilePath().append(QLatin1String(".err"));
    }
}

QString tst_uic3::workingDir() const
{
    return QDir::cleanPath(SRCDIR);
}

QTEST_APPLESS_MAIN(tst_uic3)
#include "tst_uic3.moc"
#else
QTEST_NOOP_MAIN
#endif
