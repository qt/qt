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

#include "abstracttestsuite.h"
#include <QtTest/QtTest>
#include <QtCore/qset.h>
#include <QtCore/qtextstream.h>

/*!
   AbstractTestSuite provides a way of building QtTest test objects
   dynamically. The use case is integration of JavaScript test suites
   into QtTest autotests.

   Subclasses add their tests functions with addTestFunction() in the
   constructor, and must reimplement runTestFunction(). Additionally,
   subclasses can reimplement initTestCase() and cleanupTestCase()
   (but make sure to call the base implementation).

   AbstractTestSuite uses configuration files for getting information
   about skipped tests (skip.txt) and expected test failures
   (expect_fail.txt). Subclasses must reimplement
   createSkipConfigFile() and createExpectFailConfigFile() for
   creating these files, and configData() for processing an entry of
   such a file.

   The config file format is as follows:
   - Lines starting with '#' are skipped.
   - Lines of the form [SYMBOL] means that the upcoming data
     should only be processed if the given SYMBOL is defined on
     this platform.
   - Any other line is split on ' | ' and handed off to the client.

   Subclasses must provide a default tests directory (where the
   subclass expects to find the script files to run as tests), and a
   default config file directory. Some environment variables can be
   used to affect where AbstractTestSuite will look for files:

   - QTSCRIPT_TEST_CONFIG_DIR: Overrides the default test config path.

   - QTSCRIPT_TEST_CONFIG_SUFFIX: Is appended to "skip" and
   "expect_fail" to create the test config name. This makes it easy to
   maintain skip- and expect_fail-files corresponding to different
   revisions of a test suite, and switch between them.

   - QTSCRIPT_TEST_DIR: Overrides the default test dir.

   AbstractTestSuite does _not_ define how the test dir itself is
   processed or how tests are run; this is left up to the subclass.

   If no config files are found, AbstractTestSuite will ask the
   subclass to create a default skip file. Also, the
   shouldGenerateExpectedFailures variable will be set to true. The
   subclass should check for this when a test fails, and add an entry
   to its set of expected failures. When all tests have been run,
   AbstractTestSuite will ask the subclass to create the expect_fail
   file based on the tests that failed. The next time the autotest is
   run, the created config files will be used.

   The reason for skipping a test is usually that it takes a very long
   time to complete (or even hangs completely), or it crashes. It's
   not possible for the test runner to know in advance which tests are
   problematic, which is why the entries to the skip file are
   typically added manually. When running tests for the first time, it
   can be useful to run the autotest with the -v1 command line option,
   so you can see the name of each test before it's run, and can add a
   skip entry if appropriate.
*/

// Helper class for constructing the test class's QMetaObject contents
// at runtime.
class TestMetaObjectBuilder
{
public:
    TestMetaObjectBuilder(const QByteArray &className,
                      const QMetaObject *superClass);

    void appendPrivateVoidSlot(const char *signature);
    void appendPrivateVoidSlot(const QString &signature)
    { appendPrivateVoidSlot(signature.toLatin1().constData()); }

    void assignContents(QMetaObject &);

private:
    void appendString(const char *);
    void finalize();

    const QByteArray m_className;
    const QMetaObject *m_superClass;
    QVector<uint> m_data;
    QVector<char> m_stringdata;
    int m_emptyStringOffset;
    bool m_finalized;
};

TestMetaObjectBuilder::TestMetaObjectBuilder(
    const QByteArray &className,
    const QMetaObject *superClass)
    : m_className(className), m_superClass(superClass),
      m_finalized(false)
{
    // header
    m_data << 1 // revision
          << 0 // classname
          << 0 << 0 // classinfo
          << 0 << 10 // methods (backpatched later)
          << 0 << 0 // properties
          << 0 << 0 // enums/sets
        ;

    appendString(className.constData());
    m_emptyStringOffset = m_stringdata.size();
    appendString("");
}

void TestMetaObjectBuilder::appendString(const char *s)
{
    char c;
    do {
        c = *(s++);
        m_stringdata << c;
    } while (c != '\0');
}

void TestMetaObjectBuilder::appendPrivateVoidSlot(const char *signature)
{
    static const int methodCountOffset = 4;
    // signature, parameters, type, tag, flags
    m_data << m_stringdata.size()
           << m_emptyStringOffset
           << m_emptyStringOffset
           << m_emptyStringOffset
           << 0x08;
    appendString(signature);
    ++m_data[methodCountOffset];
}

void TestMetaObjectBuilder::finalize()
{
    if (m_finalized)
        return;
    m_data << 0; // eod
    m_finalized = true;
}

/**
  Assigns this builder's contents to the meta-object \a mo.  It's up
  to the caller to ensure that this builder (and hence, its data)
  stays alive as long as needed.
*/
void TestMetaObjectBuilder::assignContents(QMetaObject &mo)
{
    finalize();
    mo.d.superdata = m_superClass;
    mo.d.stringdata = m_stringdata.constData();
    mo.d.data = m_data.constData();
    mo.d.extradata = 0;
}


class TestConfigClientInterface;
// For parsing information about skipped tests and expected failures.
class TestConfigParser
{
public:
    static void parse(const QString &path,
                      TestConfig::Mode mode,
                      TestConfigClientInterface *client);

private:
    static QString unescape(const QString &);
    static bool isKnownSymbol(const QString &);
    static bool isDefined(const QString &);

    static QSet<QString> knownSymbols;
    static QSet<QString> definedSymbols;
};

QSet<QString> TestConfigParser::knownSymbols;
QSet<QString> TestConfigParser::definedSymbols;

/**
   Parses the config file at the given \a path in the given \a mode.
   Handling of errors and data is delegated to the given \a client.
*/
void TestConfigParser::parse(const QString &path,
                             TestConfig::Mode mode,
                             TestConfigClientInterface *client)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QTextStream stream(&file);
    int lineNumber = 0;
    QString predicate;
    const QString separator = QString::fromLatin1(" | ");
    while (!stream.atEnd()) {
        ++lineNumber;
        QString line = stream.readLine();
        if (line.isEmpty())
            continue;
        if (line.startsWith('#')) // Comment
            continue;
        if (line.startsWith('[')) { // Predicate
            if (!line.endsWith(']')) {
                client->configError(path, "malformed predicate", lineNumber);
                return;
            }
            QString symbol = line.mid(1, line.size()-2);
            if (isKnownSymbol(symbol)) {
                predicate = symbol;
            } else {
                qWarning("symbol %s is not known -- add it to TestConfigParser!", qPrintable(symbol));
                predicate = QString();
            }
        } else {
            if (predicate.isEmpty() || isDefined(predicate)) {
                QStringList parts = line.split(separator, QString::KeepEmptyParts);
                for (int i = 0; i < parts.size(); ++i)
                    parts[i] = unescape(parts[i]);
                client->configData(mode, parts);
            }
        }
    }
}

QString TestConfigParser::unescape(const QString &str)
{
    return QString(str).replace("\\n", "\n");
}

bool TestConfigParser::isKnownSymbol(const QString &symbol)
{
    if (knownSymbols.isEmpty()) {
        knownSymbols
            // If you add a symbol here, add a case for it in
            // isDefined() as well.
            << "Q_OS_LINUX"
            << "Q_OS_SOLARIS"
            << "Q_OS_WINCE"
            << "Q_OS_SYMBIAN"
            << "Q_OS_MAC"
            << "Q_OS_WIN"
            << "Q_CC_MSVC"
            << "Q_CC_MINGW"
            << "Q_CC_INTEL"
            ;
    }
    return knownSymbols.contains(symbol);
}

bool TestConfigParser::isDefined(const QString &symbol)
{
    if (definedSymbols.isEmpty()) {
        definedSymbols
#ifdef Q_OS_LINUX
            << "Q_OS_LINUX"
#endif
#ifdef Q_OS_SOLARIS
            << "Q_OS_SOLARIS"
#endif
#ifdef Q_OS_WINCE
            << "Q_OS_WINCE"
#endif
#ifdef Q_OS_SYMBIAN
            << "Q_OS_SYMBIAN"
#endif
#ifdef Q_OS_MAC
            << "Q_OS_MAC"
#endif
#ifdef Q_OS_WIN
            << "Q_OS_WIN"
#endif
#ifdef Q_CC_MSVC
            << "Q_CC_MSVC"
#endif
#ifdef Q_CC_MINGW
            << "Q_CC_MINGW"
#endif
#ifdef Q_CC_INTEL
            << "Q_CC_INTEL"
#endif
            ;
    }
    return definedSymbols.contains(symbol);
}


QMetaObject AbstractTestSuite::staticMetaObject;

const QMetaObject *AbstractTestSuite::metaObject() const
{
    return &staticMetaObject;
}

void *AbstractTestSuite::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, staticMetaObject.d.stringdata))
        return static_cast<void*>(const_cast<AbstractTestSuite*>(this));
    return QObject::qt_metacast(_clname);
}

int AbstractTestSuite::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0:
            initTestCase();
            break;
        case 1:
            cleanupTestCase();
            break;
        default:
            // If another method is added above, this offset must be adjusted.
            runTestFunction(_id - 2);
        }
        _id -= staticMetaObject.methodCount() - staticMetaObject.methodOffset();
    }
    return _id;
}

AbstractTestSuite::AbstractTestSuite(const QByteArray &className,
                                     const QString &defaultTestsPath,
                                     const QString &defaultConfigPath)
    : shouldGenerateExpectedFailures(false),
      metaBuilder(new TestMetaObjectBuilder(className, &QObject::staticMetaObject))
{
    QString testConfigPath = qgetenv("QTSCRIPT_TEST_CONFIG_DIR");
    if (testConfigPath.isEmpty())
        testConfigPath = defaultConfigPath;
    QString configSuffix = qgetenv("QTSCRIPT_TEST_CONFIG_SUFFIX");
    skipConfigPath = QString::fromLatin1("%0/skip%1.txt")
                     .arg(testConfigPath).arg(configSuffix);
    expectFailConfigPath = QString::fromLatin1("%0/expect_fail%1.txt")
                           .arg(testConfigPath).arg(configSuffix);

    QString testsPath = qgetenv("QTSCRIPT_TEST_DIR");
    if (testsPath.isEmpty())
        testsPath = defaultTestsPath;
    testsDir = QDir(testsPath);

    addTestFunction("initTestCase");
    addTestFunction("cleanupTestCase");

    // Subclass constructors should add their custom test functions to
    // the meta-object and call finalizeMetaObject().
}

AbstractTestSuite::~AbstractTestSuite()
{
    delete metaBuilder;
}

void AbstractTestSuite::addTestFunction(const QString &name,
                                        DataFunctionCreation dfc)
{
    if (dfc == CreateDataFunction) {
        QString dataSignature = QString::fromLatin1("%0_data()").arg(name);
        metaBuilder->appendPrivateVoidSlot(dataSignature);
    }
    QString signature = QString::fromLatin1("%0()").arg(name);
    metaBuilder->appendPrivateVoidSlot(signature);
}

void AbstractTestSuite::finalizeMetaObject()
{
    metaBuilder->assignContents(staticMetaObject);
}

void AbstractTestSuite::initTestCase()
{
    if (!testsDir.exists()) {
        QString message = QString::fromLatin1("tests directory (%0) doesn't exist.")
                          .arg(testsDir.path());
        QFAIL(qPrintable(message));
        return;
    }

    if (QFileInfo(skipConfigPath).exists())
        TestConfigParser::parse(skipConfigPath, TestConfig::Skip, this);
    else
        createSkipConfigFile();

    if (QFileInfo(expectFailConfigPath).exists())
        TestConfigParser::parse(expectFailConfigPath, TestConfig::ExpectFail, this);
    else
        shouldGenerateExpectedFailures = true;
}

void AbstractTestSuite::cleanupTestCase()
{
    if (shouldGenerateExpectedFailures)
        createExpectFailConfigFile();
}

void AbstractTestSuite::configError(const QString &path, const QString &message, int lineNumber)
{
    QString output;
    output.append(path);
    if (lineNumber != -1)
        output.append(":").append(QString::number(lineNumber));
    output.append(": ").append(message);
    QFAIL(qPrintable(output));
}

void AbstractTestSuite::createSkipConfigFile()
{
    QFile file(skipConfigPath);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QWARN(qPrintable(QString::fromLatin1("creating %0").arg(skipConfigPath)));
    QTextStream stream(&file);

    writeSkipConfigFile(stream);

    file.close();
}

void AbstractTestSuite::createExpectFailConfigFile()
{
    QFile file(expectFailConfigPath);
    if (!file.open(QFile::WriteOnly))
        return;
    QWARN(qPrintable(QString::fromLatin1("creating %0").arg(expectFailConfigPath)));
    QTextStream stream(&file);

    writeExpectFailConfigFile(stream);

    file.close();
}

/*!
  Convenience function for reading all contents of a file.
 */
QString AbstractTestSuite::readFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

/*!
  Escapes characters in the string \a str so it's suitable for writing
  to a config file.
 */
QString AbstractTestSuite::escape(const QString &str)
{
    return QString(str).replace("\n", "\\n");
}
