/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtScript/qscriptengine.h>
#include <QtScript/qscriptvalue.h>

//TESTED_FILES=

static QString readFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

class tst_V8 : public QObject
{
    Q_OBJECT

public:
    tst_V8();
    virtual ~tst_V8();

public slots:
    void init();
    void cleanup();

private slots:
    void benchmark_data();
    void benchmark();

private:
    QDir testsDir;
};

tst_V8::tst_V8()
{
    testsDir = QDir(":/tests");
    if (!testsDir.exists())
        qWarning("*** no tests/ dir!");
}

tst_V8::~tst_V8()
{
}

void tst_V8::init()
{
}

void tst_V8::cleanup()
{
}

void tst_V8::benchmark_data()
{
    QTest::addColumn<QString>("testName");
    QFileInfoList testFileInfos = testsDir.entryInfoList(QStringList() << "*.js", QDir::Files);
    foreach (QFileInfo tfi, testFileInfos) {
        QString name = tfi.baseName();
        if (name == QString::fromLatin1("base")) {
            // base.js contains the benchmark library, it's not a test.
            continue;
        }
        QTest::newRow(name.toLatin1().constData()) << name;
    }
}

void tst_V8::benchmark()
{
    QFETCH(QString, testName);

    QString baseDotJsContents = readFile(testsDir.filePath("base.js"));
    QVERIFY(!baseDotJsContents.isEmpty());

    QString testContents = readFile(testsDir.filePath(testName + ".js"));
    QVERIFY(!testContents.isEmpty());

    QScriptEngine engine;
    engine.evaluate(baseDotJsContents);
    QVERIFY(!engine.hasUncaughtException());
    engine.evaluate(testContents);
    QVERIFY(!engine.hasUncaughtException());

    QBENCHMARK {
        engine.evaluate("BenchmarkSuite.RunSuites({})");
    }
    QVERIFY(!engine.hasUncaughtException());
}

QTEST_MAIN(tst_V8)
#include "tst_v8.moc"
