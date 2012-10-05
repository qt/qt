/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QLibraryInfo>
#include <QFileInfo>
#include <QDir>
#include <QProcess>
#include <QDebug>
#include "../../../shared/util.h"
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
    QStringList excludedDirs;

    void namingConvention(const QDir &);
    QStringList findQmlFiles(const QDir &);
};

tst_examples::tst_examples()
{
    // Add directories you want excluded here
    excludedDirs << "doc/src/snippets/declarative/visualdatamodel_rootindex"
                 << "doc/src/snippets/declarative/qtbinding";
    // Known to violate naming conventions, QTQAINFRA-428
    excludedDirs << "demos/mobile/qtbubblelevel/qml"
                 << "demos/mobile/quickhit";
    // Layouts do not install, QTQAINFRA-428
    excludedDirs << "examples/declarative/cppextensions/qgraphicslayouts/qgraphicsgridlayout/qml/qgraphicsgridlayout"
                 << "examples/declarative/cppextensions/qgraphicslayouts/qgraphicslinearlayout/qml/qgraphicslinearlayout";
    // Various QML errors, QTQAINFRA-428
    excludedDirs << "doc/src/snippets/declarative/imports";

    // Check shaders which are not present for configurations without OpenGL or when not built.
    const QString shaderExample = QLatin1String("examples/declarative/shadereffects");
#ifdef QT_NO_OPENGL
    excludedDirs << shaderExample;
#else
    const QString importPaths = QLibraryInfo::location(QLibraryInfo::ImportsPath);
    if (!QFileInfo(importPaths + QLatin1String("/Qt/labs/shaders")).isDir())
        excludedDirs << shaderExample;
#endif // QT_NO_OPENGL

#ifdef QT_NO_WEBKIT
    excludedDirs << "examples/declarative/modelviews/webview"
                 << "demos/declarative/webbrowser"
                 << "doc/src/snippets/declarative/webview";
#endif // QT_NO_WEBKIT

#ifdef QT_NO_XMLPATTERNS
    excludedDirs << "examples/declarative/xml/xmldata"
                 << "demos/declarative/twitter"
                 << "demos/declarative/flickr"
                 << "demos/declarative/photoviewer"
                 << "demos/declarative/rssnews/qml/rssnews"
                 << "doc/src/snippets/declarative";
#endif // QT_NO_XMLPATTERNS
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
    const QString absolutePath = d.absolutePath();
#ifdef Q_OS_MAC // Mac: Do not recurse into bundle folders of built examples.
    if (absolutePath.endsWith(QLatin1String(".app")))
        return QStringList();
#endif
    foreach (const QString &excludedDir, excludedDirs)
        if (absolutePath.endsWith(excludedDir))
            return QStringList();

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

static inline QByteArray msgViewerErrors(const QList<QDeclarativeError> &l)
{
    QString errors;
    QDebug(&errors) << '\n' << l;
    return errors.toLocal8Bit();
}

void tst_examples::examples()
{
    QFETCH(QString, file);
    QVERIFY2(QFileInfo(file).exists(),
             qPrintable(QString::fromLatin1("'%1' does not exist.").arg(QDir::toNativeSeparators(file))));

    QDeclarativeViewer viewer;

    QtMsgHandler old = qInstallMsgHandler(silentErrorsMsgHandler);
    QVERIFY(viewer.open(file));
    qInstallMsgHandler(old);
    QVERIFY2(viewer.view()->status() != QDeclarativeView::Error,
             msgViewerErrors(viewer.view()->errors()).constData());
    QTRY_VERIFY(viewer.view()->status() != QDeclarativeView::Loading);
    QVERIFY2(viewer.view()->status() == QDeclarativeView::Ready,
             msgViewerErrors(viewer.view()->errors()).constData());

    viewer.show();

    QVERIFY(QTest::qWaitForWindowShown(&viewer));
}

QTEST_MAIN(tst_examples)

#include "tst_examples.moc"
