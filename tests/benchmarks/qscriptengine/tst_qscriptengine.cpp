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
#include <QtScript>

//TESTED_FILES=

class tst_QScriptEngine : public QObject
{
    Q_OBJECT

public:
    tst_QScriptEngine();
    virtual ~tst_QScriptEngine();

public slots:
    void init();
    void cleanup();

private slots:
    void constructor();
    void evaluate_data();
    void evaluate();
    void evaluateProgram_data();
    void evaluateProgram();
    void connectAndDisconnect();
    void newObject();
    void newQObject();
    void newFunction();
    void newVariant();
    void collectGarbage();
    void pushAndPopContext();
    void toStringHandle();
    void castValueToQreal();
    void nativeCall();
    void translation_data();
    void translation();
};

tst_QScriptEngine::tst_QScriptEngine()
{
}

tst_QScriptEngine::~tst_QScriptEngine()
{
}

void tst_QScriptEngine::init()
{
}

void tst_QScriptEngine::cleanup()
{
}

void tst_QScriptEngine::constructor()
{
    QBENCHMARK {
        QScriptEngine engine;
        (void)engine.parent();
    }
}

void tst_QScriptEngine::evaluate_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("empty script") << QString::fromLatin1("");
    QTest::newRow("number literal") << QString::fromLatin1("123");
    QTest::newRow("string literal") << QString::fromLatin1("'ciao'");
    QTest::newRow("regexp literal") << QString::fromLatin1("/foo/gim");
    QTest::newRow("null literal") << QString::fromLatin1("null");
    QTest::newRow("undefined literal") << QString::fromLatin1("undefined");
    QTest::newRow("null literal") << QString::fromLatin1("null");
    QTest::newRow("empty object literal") << QString::fromLatin1("{}");
    QTest::newRow("this") << QString::fromLatin1("this");
    QTest::newRow("object literal with one property") << QString::fromLatin1("{ foo: 123 }");
    QTest::newRow("object literal with two properties") << QString::fromLatin1("{ foo: 123, bar: 456 }");
    QTest::newRow("object literal with many properties") << QString::fromLatin1("{ a: 1, b: 2, c: 3, d: 4, e: 5, f: 6, g: 7, h: 8, i: 9, j: 10 }");
    QTest::newRow("empty array literal") << QString::fromLatin1("[]");
    QTest::newRow("array literal with one element") << QString::fromLatin1("[1]");
    QTest::newRow("array literal with two elements") << QString::fromLatin1("[1,2]");
    QTest::newRow("array literal with many elements") << QString::fromLatin1("[1,2,3,4,5,6,7,8,9,10,9,8,7,6,5,4,3,2,1]");
    QTest::newRow("empty function definition") << QString::fromLatin1("function foo() { }");
    QTest::newRow("function definition") << QString::fromLatin1("function foo() { return 123; }");
    QTest::newRow("for loop with empty body (1000 iterations)") << QString::fromLatin1("for (i = 0; i < 1000; ++i) {}");
    QTest::newRow("for loop with empty body (10000 iterations)") << QString::fromLatin1("for (i = 0; i < 10000; ++i) {}");
    QTest::newRow("for loop with empty body (100000 iterations)") << QString::fromLatin1("for (i = 0; i < 100000; ++i) {}");
    QTest::newRow("for loop with empty body (1000000 iterations)") << QString::fromLatin1("for (i = 0; i < 1000000; ++i) {}");
    QTest::newRow("for loop (1000 iterations)") << QString::fromLatin1("j = 0; for (i = 0; i < 1000; ++i) { j += i; }; j");
    QTest::newRow("for loop (10000 iterations)") << QString::fromLatin1("j = 0; for (i = 0; i < 10000; ++i) { j += i; }; j");
    QTest::newRow("for loop (100000 iterations)") << QString::fromLatin1("j = 0; for (i = 0; i < 100000; ++i) { j += i; }; j");
    QTest::newRow("for loop (1000000 iterations)") << QString::fromLatin1("j = 0; for (i = 0; i < 1000000; ++i) { j += i; }; j");
    QTest::newRow("assignments") << QString::fromLatin1("a = 1; b = 2; c = 3; d = 4");
    QTest::newRow("while loop (1000 iterations)") << QString::fromLatin1("i = 0; while (i < 1000) { ++i; }; i");
    QTest::newRow("while loop (10000 iterations)") << QString::fromLatin1("i = 0; while (i < 10000) { ++i; }; i");
    QTest::newRow("while loop (100000 iterations)") << QString::fromLatin1("i = 0; while (i < 100000) { ++i; }; i");
    QTest::newRow("while loop (1000000 iterations)") << QString::fromLatin1("i = 0; while (i < 1000000) { ++i; }; i");
    QTest::newRow("function expression") << QString::fromLatin1("(function(a, b, c){ return a + b + c; })(1, 2, 3)");
}

void tst_QScriptEngine::evaluate()
{
    QFETCH(QString, code);
    QScriptEngine engine;

    QBENCHMARK {
        (void)engine.evaluate(code);
    }
}

void tst_QScriptEngine::connectAndDisconnect()
{
    QScriptEngine engine;
    QScriptValue fun = engine.evaluate("(function() { })");
    QBENCHMARK {
        qScriptConnect(&engine, SIGNAL(destroyed()), QScriptValue(), fun);
        qScriptDisconnect(&engine, SIGNAL(destroyed()), QScriptValue(), fun);
    }
}

void tst_QScriptEngine::evaluateProgram_data()
{
    evaluate_data();
}

void tst_QScriptEngine::evaluateProgram()
{
    QFETCH(QString, code);
    QScriptEngine engine;
    QScriptProgram program(code);

    QBENCHMARK {
        (void)engine.evaluate(program);
    }
}

void tst_QScriptEngine::newObject()
{
    QScriptEngine engine;
    QBENCHMARK {
        (void)engine.newObject();
    }
}

void tst_QScriptEngine::newQObject()
{
    QScriptEngine engine;
    QBENCHMARK {
        (void)engine.newQObject(QCoreApplication::instance());
    }
}

static QScriptValue testFunction(QScriptContext *, QScriptEngine *)
{
    return 0;
}

void tst_QScriptEngine::newFunction()
{
    QScriptEngine engine;
    QBENCHMARK {
        (void)engine.newFunction(testFunction);
    }
}

void tst_QScriptEngine::newVariant()
{
    QScriptEngine engine;
    QVariant var(123);
    QBENCHMARK {
        (void)engine.newVariant(var);
    }
}

void tst_QScriptEngine::collectGarbage()
{
    QScriptEngine engine;
    QBENCHMARK {
        engine.collectGarbage();
    }
}

void tst_QScriptEngine::pushAndPopContext()
{
    QScriptEngine engine;
    QBENCHMARK {
        (void)engine.pushContext();
        engine.popContext();
    }
}

void tst_QScriptEngine::toStringHandle()
{
    QScriptEngine engine;
    QString str = QString::fromLatin1("foobarbaz");
    QBENCHMARK {
        (void)engine.toStringHandle(str);
    }
}

void tst_QScriptEngine::castValueToQreal()
{
    QScriptEngine engine;
    QScriptValue val(123);
    QBENCHMARK {
        (void)qscriptvalue_cast<qreal>(val);
    }
}

static QScriptValue native_function(QScriptContext *, QScriptEngine *)
{
    return 42;
}

void tst_QScriptEngine::nativeCall()
{
    QScriptEngine eng;
    eng.globalObject().setProperty("fun", eng.newFunction(native_function));
    QBENCHMARK{
        eng.evaluate("var w = 0; for (i = 0; i < 100000; ++i) {\n"
                     "  w += fun() + fun(); w -= fun(); fun(); w -= fun(); }");
    }
}

void tst_QScriptEngine::translation_data()
{
    QTest::addColumn<QString>("text");
    QTest::newRow("no translation") << "\"hello world\"";
    QTest::newRow("qsTr") << "qsTr(\"hello world\")";
    QTest::newRow("qsTranslate") << "qsTranslate(\"\", \"hello world\")";
}

void tst_QScriptEngine::translation()
{
    QFETCH(QString, text);
    QScriptEngine engine;
    engine.installTranslatorFunctions();

    QBENCHMARK {
        (void)engine.evaluate(text);
    }
}

QTEST_MAIN(tst_QScriptEngine)
#include "tst_qscriptengine.moc"
