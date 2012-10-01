/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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


#include <QtTest/QtTest>
#include <qdir.h>
#include <qpluginloader.h>
#include "theplugin/plugininterface.h"

// Helper macros to let us know if some suffixes are valid
#define bundle_VALID    false
#define dylib_VALID     false
#define sl_VALID        false
#define a_VALID         false
#define so_VALID        false
#define dll_VALID       false

#if defined(Q_OS_DARWIN)
# undef bundle_VALID
# undef dylib_VALID
# undef so_VALID
# define bundle_VALID   true
# define dylib_VALID    true
# define so_VALID       true
# define SUFFIX         ".dylib"
# define PREFIX         "lib"

#elif defined(Q_OS_HPUX) && !defined(__ia64)
# undef sl_VALID
# define sl_VALID       true
# define SUFFIX         ".sl"
# define PREFIX         "lib"

#elif defined(Q_OS_AIX)
# undef a_VALID
# undef so_VALID
# define a_VALID        true
# define so_VALID       true
# define SUFFIX         ".so"
# define PREFIX         "lib"

#elif defined(Q_OS_WIN)
# undef dll_VALID
# define dll_VALID      true
# define SUFFIX         ".dll"
# define PREFIX         ""

#elif defined(Q_OS_SYMBIAN)
# undef dll_VALID
# define dll_VALID      true
# define SUFFIX         ".dll"
# define PREFIX         ""

#else  // all other Unix
# undef so_VALID
# define so_VALID       true
# define SUFFIX         ".so"
# define PREFIX         "lib"
#endif

//TESTED_CLASS=
//TESTED_FILES=

QT_FORWARD_DECLARE_CLASS(QPluginLoader)
class tst_QPluginLoader : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void errorString();
    void loadHints();
    void deleteinstanceOnUnload();
    void checkingStubsFromDifferentDrives();
    void loadDebugObj();
    void loadCorruptElf_data();
    void loadCorruptElf();
    void loadGarbage();

private:
    QString qualifiedLibraryName(const QString &fileName) const;

    QString m_bin;
};

void tst_QPluginLoader::initTestCase()
{
    QDir workingDirectory = QDir::current();
#ifdef Q_OS_WIN
    // cd up to be able to locate the binaries of the sub-processes.
    if (workingDirectory.absolutePath().endsWith(QLatin1String("/debug"), Qt::CaseInsensitive)
            || workingDirectory.absolutePath().endsWith(QLatin1String("/release"), Qt::CaseInsensitive)) {
        QVERIFY(workingDirectory.cdUp());
        QVERIFY(QDir::setCurrent(workingDirectory.absolutePath()));
    }
#endif
    m_bin = workingDirectory.absoluteFilePath(QLatin1String("bin"));
    QVERIFY(QFileInfo(m_bin).isDir());
}

QString tst_QPluginLoader::qualifiedLibraryName(const QString &fileName) const
{
    return m_bin + QLatin1Char('/') + QLatin1String(PREFIX)
           + fileName + QLatin1String(SUFFIX);
}

static inline QByteArray msgCannotLoadPlugin(const QString &plugin, const QString &why)
{
    return QString::fromLatin1("Cannot load plugin '%1' from '%2': %3")
                               .arg(QDir::toNativeSeparators(plugin),
                                    QDir::toNativeSeparators(QDir::currentPath()),
                                    why).toLocal8Bit();
}

//#define SHOW_ERRORS 1

void tst_QPluginLoader::errorString()
{
#if defined(Q_OS_WINCE)
    // On WinCE we need an QCoreApplication object for current dir
    int argc = 0;
    QCoreApplication app(argc,0);
#endif
    const QString unknown(QLatin1String("Unknown error"));

    {
    QPluginLoader loader; // default constructed
    bool loaded = loader.load();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(loaded, false);
    QCOMPARE(loader.errorString(), unknown);

    QObject *obj = loader.instance();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(obj, static_cast<QObject*>(0));
    QCOMPARE(loader.errorString(), unknown);

    bool unloaded = loader.unload();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(unloaded, false);
    QCOMPARE(loader.errorString(), unknown);
    }
    {
    QPluginLoader loader(qualifiedLibraryName(QLatin1String("tst_qpluginloaderlib")));     //not a plugin
    bool loaded = loader.load();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(loaded, false);
    QVERIFY(loader.errorString() != unknown);

    QObject *obj = loader.instance();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(obj, static_cast<QObject*>(0));
    QVERIFY(loader.errorString() != unknown);

    bool unloaded = loader.unload();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(unloaded, false);
    QVERIFY(loader.errorString() != unknown);
    }

    {
    QPluginLoader loader(qualifiedLibraryName(QLatin1String("nosuchfile")));     //not a file
    bool loaded = loader.load();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(loaded, false);
    QVERIFY(loader.errorString() != unknown);

    QObject *obj = loader.instance();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(obj, static_cast<QObject*>(0));
    QVERIFY(loader.errorString() != unknown);

    bool unloaded = loader.unload();
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QCOMPARE(unloaded, false);
    QVERIFY(loader.errorString() != unknown);
    }

#if !defined Q_OS_WIN && !defined Q_OS_MAC && !defined Q_OS_HPUX && !defined Q_OS_SYMBIAN && !defined Q_OS_QNX
    {
    QPluginLoader loader(qualifiedLibraryName(QLatin1String("almostplugin")));     //a plugin with unresolved symbols
    loader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
    QCOMPARE(loader.load(), false);
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QVERIFY(loader.errorString() != unknown);

    QCOMPARE(loader.instance(), static_cast<QObject*>(0));
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QVERIFY(loader.errorString() != unknown);

    QCOMPARE(loader.unload(), false);
#ifdef SHOW_ERRORS
    qDebug() << loader.errorString();
#endif
    QVERIFY(loader.errorString() != unknown);
    }
#endif

    {
    const QString plugin = qualifiedLibraryName(QLatin1String("theplugin"));
    QPluginLoader loader(plugin);     //a plugin
    QVERIFY2(loader.load(), msgCannotLoadPlugin(plugin, loader.errorString()).constData());
    QCOMPARE(loader.errorString(), unknown);

    QVERIFY(loader.instance() !=  static_cast<QObject*>(0));
    QCOMPARE(loader.errorString(), unknown);

    // Make sure that plugin really works
    PluginInterface* theplugin = qobject_cast<PluginInterface*>(loader.instance());
    QString pluginName = theplugin->pluginName();
    QCOMPARE(pluginName, QLatin1String("Plugin ok"));

    QCOMPARE(loader.unload(), true);
    QCOMPARE(loader.errorString(), unknown);
    }
}

void tst_QPluginLoader::loadHints()
{
    QPluginLoader loader;
    QCOMPARE(loader.loadHints(), (QLibrary::LoadHints)0);   //Do not crash
    loader.setLoadHints(QLibrary::ResolveAllSymbolsHint);
    loader.setFileName(qualifiedLibraryName(QLatin1String("theplugin")));     //a plugin
    QCOMPARE(loader.loadHints(), QLibrary::ResolveAllSymbolsHint);
}

void tst_QPluginLoader::deleteinstanceOnUnload()
{
    for (int pass = 0; pass < 4; ++pass) {
        QPluginLoader loader1;
        const QString plugin = qualifiedLibraryName(QLatin1String("theplugin"));
        loader1.setFileName(plugin);     //a plugin
        if (pass < 2)
            loader1.load(); // not recommended, instance() should do the job.
        PluginInterface *instance1 = qobject_cast<PluginInterface*>(loader1.instance());
        QVERIFY2(instance1, msgCannotLoadPlugin(plugin, loader1.errorString()).constData());
        QCOMPARE(instance1->pluginName(), QLatin1String("Plugin ok"));

        QPluginLoader loader2;
        loader2.setFileName(qualifiedLibraryName(QLatin1String("theplugin")));     //a plugin
        if (pass < 2)
            loader2.load(); // not recommended, instance() should do the job.
        PluginInterface *instance2 = qobject_cast<PluginInterface*>(loader2.instance());
        QCOMPARE(instance2->pluginName(), QLatin1String("Plugin ok"));

        QSignalSpy spy1(loader1.instance(), SIGNAL(destroyed()));
        QSignalSpy spy2(loader2.instance(), SIGNAL(destroyed()));

        // refcount not reached 0, not really unloaded
        if (pass % 2)
            QCOMPARE(loader1.unload(), false);
        else
            QCOMPARE(loader2.unload(), false);

        QCOMPARE(spy1.count(), 0);
        QCOMPARE(spy2.count(), 0);

        QCOMPARE(instance1->pluginName(), QLatin1String("Plugin ok"));
        QCOMPARE(instance2->pluginName(), QLatin1String("Plugin ok"));

        // refcount reached 0, did really unload
        if (pass % 2)
            QVERIFY(loader2.unload());
        else
            QVERIFY(loader1.unload());

        QCOMPARE(spy1.count(), 1);
        QCOMPARE(spy2.count(), 1);
    }
}

void tst_QPluginLoader::checkingStubsFromDifferentDrives()
{
#if defined(Q_OS_SYMBIAN)

    // This test needs C-drive + some additional drive (driveForStubs)

    const QString driveForStubs("E:/");// != "C:/"
    const QString stubDir("system/temp/stubtest/");
    const QString stubName("dummyStub.qtplugin");
    const QString fullStubFileName(stubDir + stubName);
    QDir dir(driveForStubs);
    bool test1(false); bool test2(false);

    // initial clean up
    QFile::remove(driveForStubs + fullStubFileName);
    dir.rmdir(driveForStubs + stubDir);

    // create a stub dir and do stub drive check
    if (!dir.mkpath(stubDir))
        QSKIP("Required drive not available for this test", SkipSingle);

    {// test without stub, should not be found
    QPluginLoader loader("C:/" + fullStubFileName);
    test1 = !loader.fileName().length();
    }

    // create a stub to defined drive
    QFile tempFile(driveForStubs + fullStubFileName);
    tempFile.open(QIODevice::ReadWrite);
    QFileInfo fileInfo(tempFile);

    {// now should be found even tried to find from C:
    QPluginLoader loader("C:/" + fullStubFileName);
    test2 = (loader.fileName() == fileInfo.absoluteFilePath());
    }

    // clean up
    tempFile.close();
    if (!QFile::remove(driveForStubs + fullStubFileName))
        QWARN("Could not remove stub file");
    if (!dir.rmdir(driveForStubs + stubDir))
        QWARN("Could not remove stub directory");

    // test after cleanup
    QVERIFY(test1);
    QVERIFY(test2);

#endif//Q_OS_SYMBIAN
}

void tst_QPluginLoader::loadDebugObj()
{
#ifdef __ELF__
    const QString file = QString::fromLatin1(SRCDIR "elftest/debugobj.so");
    QVERIFY(QFile::exists(file));
    QPluginLoader lib1(file);
    QCOMPARE(lib1.load(), false);
#else
    QSKIP("Test requires __ELF", SkipAll);
#endif
}

/* loadCorruptElf() verifies that the library loader returns the correct error message.
 * Note that Qt's plugin cache interferes here: on a fresh checkout, the "real" error
 * message will be reported, since no cache exists or the timestamp is different. When
 * run for the 2nd time, "not a valid Qt plugin" will be reported from the plugin cache.
 * "Plugin verification data mismatch" has also been observed.
 * This could arguably be fixed by copying the file to a temporary file each time, but
 * that would grow the cache on the target machine. */

void tst_QPluginLoader::loadCorruptElf_data()
{
#ifdef __ELF__
    QTest::addColumn<QString>("file");
    QTest::addColumn<QString>("error");
    const QString folder = QLatin1String(SRCDIR "elftest/");
    const QString invalidElfMessage = QLatin1String("is an invalid ELF object");
    if (sizeof(void*) == 8) {
        QTest::newRow("64bit-corrupt1")
            << (folder + QLatin1String("corrupt1.elf64.so"))
            << QString::fromLatin1("is not an ELF object");
        QTest::newRow("64bit-corrupt2")
            << (folder + QLatin1String("corrupt2.elf64.so")) << invalidElfMessage;
        QTest::newRow("64bit-corrupt3")
            << (folder + QLatin1String("corrupt3.elf64.so")) << invalidElfMessage;
    } else if (sizeof(void*) == 4) {
        QTest::newRow("32bit-corrupt3")
            << (folder + QLatin1String("corrupt3.elf64.so"))
            << QString::fromLatin1("architecture");
    } else {
        Q_ASSERT_X(false, Q_FUNC_INFO, "Please port QElfParser to this platform or blacklist this test.");
    }
#endif
}

void tst_QPluginLoader::loadCorruptElf()
{
#ifdef __ELF__
    QFETCH(QString, file);
    QFETCH(QString, error);

    QVERIFY(QFile::exists(file));
    QPluginLoader lib(file);
    QCOMPARE(lib.load(), false);
    const QString errorString = lib.errorString();
    QVERIFY2(errorString.contains(error) || errorString.contains("not a valid Qt plugin") || errorString.contains("Plugin verification data mismatch"),
             qPrintable(lib.errorString()));
#endif
}

void tst_QPluginLoader::loadGarbage()
{
#if defined (Q_OS_UNIX) && !defined(Q_OS_SYMBIAN)
    for (int i = 1; i <= 5; ++i) {
        QPluginLoader lib(QString::fromLatin1(SRCDIR "elftest/garbage%1.so").arg(i));
        QCOMPARE(lib.load(), false);
#ifdef SHOW_ERRORS
        qDebug() << lib.errorString();
#endif
    }
#endif
}

QTEST_APPLESS_MAIN(tst_QPluginLoader)
#include "tst_qpluginloader.moc"
