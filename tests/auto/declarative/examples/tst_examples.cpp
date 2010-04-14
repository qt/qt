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
#include <qtest.h>
#include <QLibraryInfo>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include <QFutureSynchronizer>
#include <QtConcurrentRun>

class tst_examples : public QObject
{
    Q_OBJECT
public:
    tst_examples();

private slots:
    void examples();

    void namingConvention();
private:
    QString qmlruntime;
    QStringList excludedDirs;

    void namingConvention(const QDir &);
    QStringList findQmlFiles(const QDir &);
};

tst_examples::tst_examples()
{
    QString binaries = QLibraryInfo::location(QLibraryInfo::BinariesPath);

#if defined(Q_WS_MAC)
    qmlruntime = QDir(binaries).absoluteFilePath("qml.app/Contents/MacOS/qml");
#elif defined(Q_WS_WIN)
    qmlruntime = QDir(binaries).absoluteFilePath("qml.exe");
#else
    qmlruntime = QDir(binaries).absoluteFilePath("qml");
#endif


    // Add directories you want excluded here
    excludedDirs << "examples/declarative/extending";
    excludedDirs << "examples/declarative/plugins";
    excludedDirs << "examples/declarative/proxywidgets";
    excludedDirs << "examples/declarative/gestures";

    excludedDirs << "examples/declarative/imageprovider";
    excludedDirs << "demos/declarative/minehunt";

#ifdef QT_NO_WEBKIT
    excludedDirs << "examples/declarative/webview";
    excludedDirs << "demos/declarative/webbrowser";
#endif

#ifdef QT_NO_XMLPATTERNS
    excludedDirs << "examples/declarative/xmldata";
    excludedDirs << "demos/declarative/twitter";
    excludedDirs << "demos/declarative/flickr";
    excludedDirs << "demos/declarative/photoviewer";
#endif
}

/*
This tests that the demos and examples follow the naming convention required
to have them tested by the examples() test.
*/
void tst_examples::namingConvention(const QDir &d)
{
    for (int ii = 0; ii < excludedDirs.count(); ++ii) {
        QString s = excludedDirs.at(ii);
        if (d.absolutePath().endsWith(s))
            return;
    }

    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"),
                                    QDir::Files);

    bool seenQml = !files.isEmpty();
    bool seenLowercase = false;

    foreach (const QString &file, files) {
        if (file.at(0).isLower())
            seenLowercase = true;
    }

    if (!seenQml) {
        QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot |
                QDir::NoSymLinks);
        foreach (const QString &dir, dirs) {
            QDir sub = d;
            sub.cd(dir);
            namingConvention(sub);
        }
    } else if(!seenLowercase) {
        QTest::qFail(QString("Directory " + d.absolutePath() + " violates naming convention").toLatin1().constData(), __FILE__, __LINE__);
    }
}

void tst_examples::namingConvention()
{
    QString examples = QLibraryInfo::location(QLibraryInfo::ExamplesPath);
    QString demos = QLibraryInfo::location(QLibraryInfo::DemosPath);

    namingConvention(QDir(examples));
    namingConvention(QDir(demos));
}

QStringList tst_examples::findQmlFiles(const QDir &d)
{
    for (int ii = 0; ii < excludedDirs.count(); ++ii) {
        QString s = excludedDirs.at(ii);
        if (d.absolutePath().endsWith(s))
            return QStringList();
    }

    QStringList rv;

    QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"),
                                    QDir::Files);
    foreach (const QString &file, files) {
        if (file.at(0).isLower()) {
            rv << d.absoluteFilePath(file);
        }
    }

    QStringList dirs = d.entryList(QDir::Dirs | QDir::NoDotAndDotDot |
                                   QDir::NoSymLinks);
    foreach (const QString &dir, dirs) {
        QDir sub = d;
        sub.cd(dir);
        rv << findQmlFiles(sub);
    }

    return rv;
}



/*
This test runs all the examples in the declarative UI source tree and ensures
that they start and exit cleanly.

Examples are any .qml files under the examples/ or demos/ directory that start
with a lower case letter.
*/
#define THREADS 5

struct Example {
public:
    Example() : result(Unknown) {}

    enum Result { Pass, Unknown, Fail };
    Result result;
    QString file;
    QString qmlruntime;

    void run();
};

void Example::run()
{
    QFileInfo fi(file);
    QFileInfo dir(fi.path());
    QString script = SRCDIR "/data/"+dir.baseName()+"/"+fi.baseName();
    QFileInfo testdata(script+".qml");
    QStringList arguments;
    arguments << "-script" << (testdata.exists() ? script : QLatin1String(SRCDIR "/data/dummytest"))
              << "-scriptopts" << "play,testerror,exitoncomplete,exitonfailure"
              << file;
#ifdef Q_WS_QWS
    arguments << "-qws";
#endif

    QProcess p;
    p.start(qmlruntime, arguments);
    if (!p.waitForFinished()) {
        result = Fail;
        return;
    }

    if (p.exitStatus() != QProcess::NormalExit || p.exitCode() != 0)
        qWarning() << p.readAllStandardOutput() << p.readAllStandardError();

    if (p.exitStatus() != QProcess::NormalExit ||
        p.exitCode() != 0) {
        result = Fail;
        return;
    } else {
        result = Pass;
        return;
    }
}

void tst_examples::examples()
{
    QThreadPool::globalInstance()->setMaxThreadCount(5);

    QString examples = QLatin1String(SRCDIR) + "/../../../../demos/declarative/";
    QString demos = QLatin1String(SRCDIR) + "/../../../../examples/declarative/";
    QString snippets = QLatin1String(SRCDIR) + "/../../../../doc/src/snippets/";

    QStringList files;
    files << findQmlFiles(QDir(examples));
    files << findQmlFiles(QDir(demos));
    files << findQmlFiles(QDir(snippets));

    QList<Example> tests;

    for (int ii = 0; ii < files.count(); ++ii) {
        Example e;
        e.file = files.at(ii);
        e.qmlruntime = qmlruntime;
        tests << e;
    }

    QFutureSynchronizer<void> sync;

    for (int ii = 0; ii < tests.count(); ++ii) {
        Example *e = &tests[ii];
        QFuture<void> r = QtConcurrent::run(e, &Example::run);
        sync.addFuture(r);
    }

    sync.waitForFinished();

    QStringList failures;
    for (int ii = 0; ii < tests.count(); ++ii) {
        if (tests.at(ii).result != Example::Pass) 
            failures << QDir::cleanPath(tests.at(ii).file);
    }

    if (failures.count()) {
        QString error("Failed examples: ");
        error += failures.join(", ");

        QFAIL(qPrintable(error));
    }

}

QTEST_MAIN(tst_examples)

#include "tst_examples.moc"
