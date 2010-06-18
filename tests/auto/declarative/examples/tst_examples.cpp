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
#include "qmlruntime.h"
#include <QDeclarativeView>
#include <QDeclarativeError>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_examples : public QObject
{
    Q_OBJECT
public:
    tst_examples();

private slots:
    void examples_data();
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
    excludedDirs << "doc/src/snippets/declarative/visualdatamodel_rootindex";

#ifdef QT_NO_WEBKIT
    excludedDirs << "examples/declarative/modelviews/webview";
    excludedDirs << "demos/declarative/webbrowser";
#endif

#ifdef QT_NO_XMLPATTERNS
    excludedDirs << "examples/declarative/xml/xmldata";
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

    QStringList cppfiles = d.entryList(QStringList() << QLatin1String("*.cpp"), QDir::Files);
    if (cppfiles.isEmpty()) {
        QStringList files = d.entryList(QStringList() << QLatin1String("*.qml"),
                                        QDir::Files);
        foreach (const QString &file, files) {
            if (file.at(0).isLower()) {
                rv << d.absoluteFilePath(file);
            }
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
void tst_examples::examples_data()
{
    QTest::addColumn<QString>("file");

    QString examples = QLatin1String(SRCDIR) + "/../../../../demos/declarative/";
    QString demos = QLatin1String(SRCDIR) + "/../../../../examples/declarative/";
    QString snippets = QLatin1String(SRCDIR) + "/../../../../doc/src/snippets/";

    QStringList files;
    files << findQmlFiles(QDir(examples));
    files << findQmlFiles(QDir(demos));
    files << findQmlFiles(QDir(snippets));

    foreach (const QString &file, files)
        QTest::newRow(qPrintable(file)) << file;
}

static void silentErrorsMsgHandler(QtMsgType, const char *)
{
}

void tst_examples::examples()
{
    QFETCH(QString, file);

    QDeclarativeViewer viewer;

    QtMsgHandler old = qInstallMsgHandler(silentErrorsMsgHandler);
    QVERIFY(viewer.open(file));
    qInstallMsgHandler(old);

    if (viewer.view()->status() == QDeclarativeView::Error)
        qWarning() << viewer.view()->errors();

    QCOMPARE(viewer.view()->status(), QDeclarativeView::Ready);
    viewer.show();

    QTest::qWaitForWindowShown(&viewer);
}

QTEST_MAIN(tst_examples)

#include "tst_examples.moc"
