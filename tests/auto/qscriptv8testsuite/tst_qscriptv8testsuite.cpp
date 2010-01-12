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


#include <QtTest/QtTest>
#include <QByteArray>

#include <QtScript>

#if defined(Q_OS_SYMBIAN)
# define SRCDIR ""
#endif

//TESTED_CLASS=
//TESTED_FILES=

// Uncomment the following define to have the autotest generate
// addExpectedFailure() code for all the tests that fail.
// This is useful when a whole new test (sub)suite is added.
// The code is stored in addexpectedfailures.cpp.
// Paste the contents into this file after the existing
// addExpectedFailure() calls.

//#define GENERATE_ADDEXPECTEDFAILURE_CODE

static QString readFile(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly))
        return QString();
    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    return stream.readAll();
}

static void appendCString(QVector<char> *v, const char *s)
{
    char c;
    do {
        c = *(s++);
        *v << c;
    } while (c != '\0');
}

struct ExpectedFailure
{
    ExpectedFailure(const QString &name, const QString &act,
                    const QString &exp, const QString &msg)
        : testName(name), actual(act), expected(exp), message(msg)
        { }

    QString testName;
    QString actual;
    QString expected;
    QString message;
};

class tst_Suite : public QObject
{

public:
    tst_Suite();
    virtual ~tst_Suite();

    static QMetaObject staticMetaObject;
    virtual const QMetaObject *metaObject() const;
    virtual void *qt_metacast(const char *);
    virtual int qt_metacall(QMetaObject::Call, int, void **argv);

private:
    void addExpectedFailure(const QString &testName, const QString &actual,
                            const QString &expected, const QString &message);
    bool isExpectedFailure(const QString &testName, const QString &actual,
                           const QString &expected, QString *message) const;
    void addTestExclusion(const QString &testName, const QString &message);
    void addTestExclusion(const QRegExp &rx, const QString &message);
    bool isExcludedTest(const QString &testName, QString *message) const;

    QDir testsDir;
    QStringList testNames;
    QList<ExpectedFailure> expectedFailures;
    QList<QPair<QRegExp, QString> > testExclusions;
    QString mjsunitContents;
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
    QString generatedAddExpectedFailureCode;
#endif
};

QMetaObject tst_Suite::staticMetaObject;

Q_GLOBAL_STATIC(QVector<uint>, qt_meta_data_tst_Suite)
Q_GLOBAL_STATIC(QVector<char>, qt_meta_stringdata_tst_Suite)

const QMetaObject *tst_Suite::metaObject() const
{
    return &staticMetaObject;
}

void *tst_Suite::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_tst_Suite()->constData()))
        return static_cast<void*>(const_cast<tst_Suite*>(this));
    return QObject::qt_metacast(_clname);
}

static QScriptValue qscript_fail(QScriptContext *ctx, QScriptEngine *eng)
{
    QScriptValue realFail = ctx->callee().data();
    Q_ASSERT(realFail.isFunction());
    QScriptValue ret = realFail.call(ctx->thisObject(), ctx->argumentsObject());
    Q_ASSERT(eng->hasUncaughtException());
    ret.setProperty("expected", ctx->argument(0));
    ret.setProperty("actual", ctx->argument(1));
    QScriptContextInfo info(ctx->parentContext()->parentContext());
    ret.setProperty("lineNumber", info.lineNumber());
    return ret;
}

int tst_Suite::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        QString name = testNames.at(_id);
        QString path = testsDir.absoluteFilePath(name + ".js");
        QString excludeMessage;
        if (isExcludedTest(name, &excludeMessage)) {
            QTest::qSkip(excludeMessage.toLatin1(), QTest::SkipAll, path.toLatin1(), -1);
        } else {
            QScriptEngine engine;
            engine.evaluate(mjsunitContents).toString();
            if (engine.hasUncaughtException()) {
                QStringList bt = engine.uncaughtExceptionBacktrace();
                QString err = engine.uncaughtException().toString();
                qWarning("%s\n%s", qPrintable(err), qPrintable(bt.join("\n")));
            } else {
                QScriptValue fakeFail = engine.newFunction(qscript_fail);
                fakeFail.setData(engine.globalObject().property("fail"));
                engine.globalObject().setProperty("fail", fakeFail);
                QString contents = readFile(path);
                QScriptValue ret = engine.evaluate(contents);
                if (engine.hasUncaughtException()) {
                    if (!ret.isError()) {
                        Q_ASSERT(ret.instanceOf(engine.globalObject().property("MjsUnitAssertionError")));
                        QString actual = ret.property("actual").toString();
                        QString expected = ret.property("expected").toString();
                        int lineNumber = ret.property("lineNumber").toInt32();
                        QString failMessage;
                        if (isExpectedFailure(name, actual, expected, &failMessage)) {
                            QTest::qExpectFail("", failMessage.toLatin1(),
                                               QTest::Continue, path.toLatin1(),
                                               lineNumber);
                        }
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
                        else {
                            generatedAddExpectedFailureCode.append(
                                "    addExpectedFailure(\"" + name
                                + "\", \"" + actual + "\", \"" + expected
                                + "\", willFixInNextReleaseMessage);\n");
                        }
#endif
                        QTest::qCompare(actual, expected, "actual", "expect",
                                        path.toLatin1(), lineNumber);
                    } else {
                        int lineNumber = ret.property("lineNumber").toInt32();
                        QTest::qExpectFail("", ret.toString().toLatin1(),
                                           QTest::Continue, path.toLatin1(), lineNumber);
                        QTest::qVerify(false, ret.toString().toLatin1(), "", path.toLatin1(), lineNumber);
                    }
                }
            }
        }
        _id -= testNames.size();
    }
    return _id;
}

tst_Suite::tst_Suite()
{
    testsDir = QDir(SRCDIR);
    bool testsFound = testsDir.cd("tests");
    if (!testsFound) {
        qWarning("*** no tests/ dir!");
    } else {
        if (!testsDir.exists("mjsunit.js"))
            qWarning("*** no tests/mjsunit.js file!");
        else {
            mjsunitContents = readFile(testsDir.absoluteFilePath("mjsunit.js"));
            if (mjsunitContents.isEmpty())
                qWarning("*** tests/mjsunit.js is empty!");
        }
    }
    QString willFixInNextReleaseMessage = QString::fromLatin1("Will fix in next release");
    addExpectedFailure("arguments-enum", "2", "0", willFixInNextReleaseMessage);
    addExpectedFailure("const-redecl", "undefined", "TypeError", willFixInNextReleaseMessage);
    addExpectedFailure("global-const-var-conflicts", "false", "true", willFixInNextReleaseMessage);
    addExpectedFailure("string-lastindexof", "0", "-1", "test is wrong?");

    addTestExclusion("debug-*", "not applicable");
    addTestExclusion("mirror-*", "not applicable");

    addTestExclusion("array-concat", "Hangs on JSC backend");
    addTestExclusion("array-splice", "Hangs on JSC backend");
    addTestExclusion("sparse-array-reverse", "Hangs on JSC backend");

    addTestExclusion("string-case", "V8-specific behavior? (Doesn't pass on SpiderMonkey either)");

#ifdef Q_OS_WINCE
    addTestExclusion("deep-recursion", "Demands too much memory on WinCE");
    addTestExclusion("nested-repetition-count-overflow", "Demands too much memory on WinCE");
    addTestExclusion("unicode-test", "Demands too much memory on WinCE");
    addTestExclusion("mul-exhaustive", "Demands too much memory on WinCE");
#endif

#ifdef Q_OS_SYMBIAN
    addTestExclusion("nested-repetition-count-overflow", "Demands too much memory on Symbian");
    addTestExclusion("unicode-test", "Demands too much memory on Symbian");
#endif
    // Failures due to switch to JSC as back-end
    addExpectedFailure("date-parse", "NaN", "946713600000", willFixInNextReleaseMessage);
    addExpectedFailure("delete-global-properties", "true", "false", willFixInNextReleaseMessage);
    addExpectedFailure("delete", "false", "true", willFixInNextReleaseMessage);
    addExpectedFailure("function-arguments-null", "false", "true", willFixInNextReleaseMessage);
    addExpectedFailure("function-caller", "null", "function eval() {\n    [native code]\n}", willFixInNextReleaseMessage);
    addExpectedFailure("function-prototype", "prototype", "disconnectconnect", willFixInNextReleaseMessage);
    addExpectedFailure("number-tostring", "0", "0.0000a7c5ac471b4788", willFixInNextReleaseMessage);
    addExpectedFailure("parse-int-float", "1e+21", "1", willFixInNextReleaseMessage);
    addExpectedFailure("regexp", "false", "true", willFixInNextReleaseMessage);
    addExpectedFailure("smi-negative-zero", "-Infinity", "Infinity", willFixInNextReleaseMessage);
    addExpectedFailure("string-split", "4", "3", willFixInNextReleaseMessage);
    addExpectedFailure("substr", "abcdefghijklmn", "", willFixInNextReleaseMessage);

    static const char klass[] = "tst_QScriptV8TestSuite";

    QVector<uint> *data = qt_meta_data_tst_Suite();
    // content:
    *data << 1 // revision
          << 0 // classname
          << 0 << 0 // classinfo
          << 0 << 10 // methods (backpatched later)
          << 0 << 0 // properties
          << 0 << 0 // enums/sets
        ;

    QVector<char> *stringdata = qt_meta_stringdata_tst_Suite();
    appendCString(stringdata, klass);
    appendCString(stringdata, "");

    QFileInfoList testFileInfos;
    if (testsFound)
        testFileInfos = testsDir.entryInfoList(QStringList() << "*.js", QDir::Files);
    foreach (QFileInfo tfi, testFileInfos) {
        QString name = tfi.baseName();
        // slot: signature, parameters, type, tag, flags
        QString slot = QString::fromLatin1("%0()").arg(name);
        static const int nullbyte = sizeof(klass);
        *data << stringdata->size() << nullbyte << nullbyte << nullbyte << 0x08;
        appendCString(stringdata, slot.toLatin1());
        testNames.append(name);
    }

    (*data)[4] = testFileInfos.size();

    *data << 0; // eod

    // initialize staticMetaObject
    staticMetaObject.d.superdata = &QObject::staticMetaObject;
    staticMetaObject.d.stringdata = stringdata->constData();
    staticMetaObject.d.data = data->constData();
    staticMetaObject.d.extradata = 0;
}

tst_Suite::~tst_Suite()
{
#ifdef GENERATE_ADDEXPECTEDFAILURE_CODE
    if (!generatedAddExpectedFailureCode.isEmpty()) {
        QFile file("addexpectedfailures.cpp");
        file.open(QFile::WriteOnly);
        QTextStream ts(&file);
        ts << generatedAddExpectedFailureCode;
    }
#endif
}

void tst_Suite::addExpectedFailure(const QString &testName, const QString &actual,
                                   const QString &expected, const QString &message)
{
    expectedFailures.append(ExpectedFailure(testName, actual, expected, message));
}

bool tst_Suite::isExpectedFailure(const QString &testName, const QString &actual,
                                  const QString &expected, QString *message) const
{
    for (int i = 0; i < expectedFailures.size(); ++i) {
        const ExpectedFailure &ef = expectedFailures.at(i);
        if ((testName == ef.testName) && (actual == ef.actual) && (expected == ef.expected)) {
            if (message)
                *message = ef.message;
            return true;
        }
    }
    return false;
}

void tst_Suite::addTestExclusion(const QString &testName, const QString &message)
{
    testExclusions.append(qMakePair(QRegExp(testName), message));
}

void tst_Suite::addTestExclusion(const QRegExp &rx, const QString &message)
{
    testExclusions.append(qMakePair(rx, message));
}

bool tst_Suite::isExcludedTest(const QString &testName, QString *message) const
{
    for (int i = 0; i < testExclusions.size(); ++i) {
        if (testExclusions.at(i).first.indexIn(testName) != -1) {
            if (message)
                *message = testExclusions.at(i).second;
            return true;
        }
    }
    return false;
}

QTEST_MAIN(tst_Suite)
