/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/


#include <QtTest/QtTest>

#include <QtScript/qscriptengineagent.h>
#include <QtScript/qscriptengine.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QScriptEngineAgent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double testProperty READ testProperty WRITE setTestProperty)

public:
    tst_QScriptEngineAgent();
    virtual ~tst_QScriptEngineAgent();

    double testProperty() const { return m_testProperty; }
    void setTestProperty(double val) { m_testProperty = val; }

public slots:
    double testSlot(double arg) { return arg; }

signals:
    void testSignal(double arg);

private slots:
    void scriptLoadAndUnload();
    void contextPushAndPop();
    void functionEntryAndExit();
    void positionChange();
    void exceptionThrowAndCatch();
    void eventOrder();
    void recursiveObserve();
    void multipleAgents();
    void syntaxError();
    void extension();

private:
    double m_testProperty;
};

tst_QScriptEngineAgent::tst_QScriptEngineAgent()
{
}

tst_QScriptEngineAgent::~tst_QScriptEngineAgent()
{
}

struct ScriptEngineEvent
{
    enum Type {
        ScriptLoad,
        ScriptUnload,
        ContextPush,
        ContextPop,
        FunctionEntry,
        FunctionExit,
        PositionChange,
        ExceptionThrow,
        ExceptionCatch,
        DebuggerInvocationRequest
    };

    Type type;

    qint64 scriptId;
    QString script;
    QString fileName;
    int lineNumber;
    int columnNumber;
    QScriptValue value;
    bool hasExceptionHandler;

    ScriptEngineEvent(qint64 scriptId,
                      const QString &script, const QString &fileName,
                      int lineNumber)
        : type(ScriptLoad), scriptId(scriptId),
          script(script), fileName(fileName),
          lineNumber(lineNumber)
        { }

    ScriptEngineEvent(Type type, qint64 scriptId = -1)
        : type(type), scriptId(scriptId)
        { }

    ScriptEngineEvent(Type type, qint64 scriptId,
                      const QScriptValue &value)
        : type(type), scriptId(scriptId),
          value(value)
        { }

    ScriptEngineEvent(qint64 scriptId,
                      int lineNumber, int columnNumber)
        : type(PositionChange), scriptId(scriptId),
          lineNumber(lineNumber), columnNumber(columnNumber)
        { }

    ScriptEngineEvent(qint64 scriptId,
                      const QScriptValue &exception, bool hasHandler)
        : type(ExceptionThrow), scriptId(scriptId),
          value(exception), hasExceptionHandler(hasHandler)
        { }

};

class ScriptEngineSpy : public QScriptEngineAgent, public QList<ScriptEngineEvent>
{
public:
    enum IgnoreFlag {
        IgnoreScriptLoad = 0x001,
        IgnoreScriptUnload = 0x002,
        IgnoreFunctionEntry = 0x004,
        IgnoreFunctionExit = 0x008,
        IgnorePositionChange = 0x010,
        IgnoreExceptionThrow = 0x020,
        IgnoreExceptionCatch = 0x040,
        IgnoreContextPush = 0x0100,
        IgnoreContextPop = 0x0200,
        IgnoreDebuggerInvocationRequest = 0x0400
    };

    ScriptEngineSpy(QScriptEngine *engine, int ignores = 0);
    ~ScriptEngineSpy();

    void enableIgnoreFlags(int flags)
        { m_ignores |= flags; }
    void disableIgnoreFlags(int flags)
        { m_ignores &= ~flags; }

protected:
    void scriptLoad(qint64 id, const QString &script,
                    const QString &fileName, int lineNumber);
    void scriptUnload(qint64 id);

    void contextPush();
    void contextPop();

    void functionEntry(qint64 scriptId);
    void functionExit(qint64 scriptId, const QScriptValue &returnValue);

    void positionChange(qint64 scriptId,
                        int lineNumber, int columnNumber);

    void exceptionThrow(qint64 scriptId, const QScriptValue &exception,
                        bool hasHandler);
    void exceptionCatch(qint64 scriptId, const QScriptValue &exception);

    bool supportsExtension(Extension ext) const;
    QVariant extension(Extension ext, const QVariant &arg);

private:
    int m_ignores;
};

ScriptEngineSpy::ScriptEngineSpy(QScriptEngine *engine, int ignores)
    : QScriptEngineAgent(engine)
{
    m_ignores = ignores;
    engine->setAgent(this);
}

ScriptEngineSpy::~ScriptEngineSpy()
{
}

void ScriptEngineSpy::scriptLoad(qint64 id, const QString &script,
                                 const QString &fileName, int lineNumber)
{
    if (!(m_ignores & IgnoreScriptLoad))
        append(ScriptEngineEvent(id, script, fileName, lineNumber));
}

void ScriptEngineSpy::scriptUnload(qint64 id)
{
    if (!(m_ignores & IgnoreScriptUnload))
        append(ScriptEngineEvent(ScriptEngineEvent::ScriptUnload, id));
}

void ScriptEngineSpy::contextPush()
{
    if (!(m_ignores & IgnoreContextPush))
        append(ScriptEngineEvent(ScriptEngineEvent::ContextPush));
}

void ScriptEngineSpy::contextPop()
{
    if (!(m_ignores & IgnoreContextPop))
        append(ScriptEngineEvent(ScriptEngineEvent::ContextPop));
}

void ScriptEngineSpy::functionEntry(qint64 scriptId)
{
    if (!(m_ignores & IgnoreFunctionEntry))
        append(ScriptEngineEvent(ScriptEngineEvent::FunctionEntry, scriptId));
}

void ScriptEngineSpy::functionExit(qint64 scriptId,
                                   const QScriptValue &returnValue)
{
    if (!(m_ignores & IgnoreFunctionExit))
        append(ScriptEngineEvent(ScriptEngineEvent::FunctionExit, scriptId, returnValue));
}

void ScriptEngineSpy::positionChange(qint64 scriptId,
                                     int lineNumber, int columnNumber)
{
    if (!(m_ignores & IgnorePositionChange))
        append(ScriptEngineEvent(scriptId, lineNumber, columnNumber));
}

void ScriptEngineSpy::exceptionThrow(qint64 scriptId,
                                     const QScriptValue &exception, bool hasHandler)
{
    if (!(m_ignores & IgnoreExceptionThrow))
        append(ScriptEngineEvent(scriptId, exception, hasHandler));
}

void ScriptEngineSpy::exceptionCatch(qint64 scriptId,
                                     const QScriptValue &exception)
{
    if (!(m_ignores & IgnoreExceptionCatch))
        append(ScriptEngineEvent(ScriptEngineEvent::ExceptionCatch, scriptId, exception));
}

bool ScriptEngineSpy::supportsExtension(Extension ext) const
{
    if (ext == DebuggerInvocationRequest)
        return !(m_ignores & IgnoreDebuggerInvocationRequest);
    return false;
}

QVariant ScriptEngineSpy::extension(Extension ext, const QVariant &arg)
{
    if (ext == DebuggerInvocationRequest) {
        QVariantList lst = arg.toList();
        qint64 scriptId = lst.at(0).toLongLong();
        int lineNumber = lst.at(1).toInt();
        int columnNumber = lst.at(2).toInt();
        ScriptEngineEvent evt(scriptId, lineNumber, columnNumber);
        evt.type = ScriptEngineEvent::DebuggerInvocationRequest;
        append(evt);
        return QString::fromLatin1("extension(DebuggerInvocationRequest)");
    }
    return QVariant();
}

void tst_QScriptEngineAgent::scriptLoadAndUnload()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnoreScriptLoad
                                                       | ScriptEngineSpy::IgnoreScriptUnload));
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)spy);
    {
        spy->clear();
        QString code = ";";
        QString fileName = "foo.qs";
        int lineNumber = 123;
        eng.evaluate(code, fileName, lineNumber);

        QCOMPARE(spy->count(), 2);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).script, code);
        QCOMPARE(spy->at(0).fileName, fileName);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
    }

    {
        spy->clear();
        QString code = ";";
        QString fileName = "bar.qs";
        int lineNumber = 456;
        eng.evaluate(code, fileName, lineNumber);

        QCOMPARE(spy->count(), 2);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).script, code);
        QCOMPARE(spy->at(0).fileName, fileName);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
    }

    {
        spy->clear();
        QString code = "function foo() { print('ciao'); }";
        QString fileName = "baz.qs";
        int lineNumber = 789;
        eng.evaluate(code, fileName, lineNumber);

        QCOMPARE(spy->count(), 1);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).script, code);
        QCOMPARE(spy->at(0).fileName, fileName);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);

        code = "foo = null";
        eng.evaluate(code);
        QCOMPARE(spy->count(), 3);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(1).scriptId != -1);
        QVERIFY(spy->at(1).scriptId != spy->at(0).scriptId);
        QCOMPARE(spy->at(1).script, code);
        QCOMPARE(spy->at(1).lineNumber, 1);

        QCOMPARE(spy->at(2).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(2).scriptId, spy->at(1).scriptId);

        eng.collectGarbage(); // foo() is GC'ed
        QCOMPARE(spy->count(), 4);
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
    }

    {
        spy->clear();
        QString code = "function foo() { return function() { print('ciao'); } }";
        QString fileName = "foo.qs";
        int lineNumber = 123;
        eng.evaluate(code, fileName, lineNumber);

        QCOMPARE(spy->count(), 1);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).script, code);
        QCOMPARE(spy->at(0).fileName, fileName);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);

        code = "bar = foo(); foo = null";
        eng.evaluate(code);
        QCOMPARE(spy->count(), 3);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(1).scriptId != -1);
        QVERIFY(spy->at(1).scriptId != spy->at(0).scriptId);
        QCOMPARE(spy->at(1).script, code);

        QCOMPARE(spy->at(2).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(2).scriptId, spy->at(1).scriptId);

        eng.collectGarbage(); // foo() is not GC'ed
        QCOMPARE(spy->count(), 3);

        code = "bar = null";
        eng.evaluate(code);
        QCOMPARE(spy->count(), 5);

        eng.collectGarbage(); // foo() is GC'ed
        QCOMPARE(spy->count(), 6);
    }

    {
        spy->clear();
        eng.evaluate("eval('function foo() { print(123); }')");

        QCOMPARE(spy->count(), 3);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(1).scriptId != -1);
        QVERIFY(spy->at(1).scriptId != spy->at(0).scriptId);

        QCOMPARE(spy->at(2).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
    }

}

void tst_QScriptEngineAgent::contextPushAndPop()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnoreContextPush
                                                       | ScriptEngineSpy::IgnoreContextPop));

    {
        spy->clear();
        eng.pushContext();
        eng.popContext();
        QCOMPARE(spy->count(), 2);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ContextPush);
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ContextPop);
    }
}

static QScriptValue nativeFunctionReturningArg(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->argument(0);
}

static QScriptValue nativeFunctionThrowingError(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->throwError(ctx->argument(0).toString());
}

static QScriptValue nativeFunctionCallingArg(QScriptContext *ctx, QScriptEngine *)
{
    return ctx->argument(0).call();
}

void tst_QScriptEngineAgent::functionEntryAndExit()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnoreFunctionEntry
                                                       | ScriptEngineSpy::IgnoreFunctionExit));

    {
        spy->clear();
        eng.evaluate(";");

        QCOMPARE(spy->count(), 2);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId != -1);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.isUndefined());
    }

    {
        spy->clear();
        eng.evaluate("1 + 2");

        QCOMPARE(spy->count(), 2);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId != -1);

        // evaluate() exit
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.isNumber());
        QCOMPARE(spy->at(1).value.toNumber(), qsreal(3));
    }

    {
        spy->clear();
        eng.evaluate("(function() { return 123; } )()");

        QCOMPARE(spy->count(), 4);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId != -1);

        // anonymous function entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);

        // anonymous function exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(2).value.isNumber());
        QCOMPARE(spy->at(2).value.toNumber(), qsreal(123));

        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(3).value.isNumber());
        QCOMPARE(spy->at(3).value.toNumber(), qsreal(123));
    }

    {
        spy->clear();
        eng.evaluate("function foo() { return 456; }");
        QCOMPARE(spy->count(), 2);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId != -1);

        // evaluate() exit
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.isUndefined());

        eng.evaluate("foo()");
        QCOMPARE(spy->count(), 6);

        // evaluate() entry
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(2).scriptId != spy->at(0).scriptId);

        // foo() entry
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);

        // foo() exit
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(4).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(4).value.isNumber());
        QCOMPARE(spy->at(4).value.toNumber(), qsreal(456));

        // evaluate() exit
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(5).scriptId, spy->at(2).scriptId);
        QVERIFY(spy->at(5).value.isNumber());
        QCOMPARE(spy->at(5).value.toNumber(), qsreal(456));
    }

    // native functions

    {
        QScriptValue fun = eng.newFunction(nativeFunctionReturningArg);
        eng.globalObject().setProperty("nativeFunctionReturningArg", fun);

        spy->clear();
        eng.evaluate("nativeFunctionReturningArg(123)");
        QCOMPARE(spy->count(), 4);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);

        // native function entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));

        // native function exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isNumber());
        QCOMPARE(spy->at(2).value.toNumber(), qsreal(123));

        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(3).value.isNumber());
        QCOMPARE(spy->at(3).value.toNumber(), qsreal(123));
    }

    {
        QScriptValue fun = eng.newFunction(nativeFunctionCallingArg);
        eng.globalObject().setProperty("nativeFunctionCallingArg", fun);

        spy->clear();
        eng.evaluate("nativeFunctionCallingArg(function() { return 123; })");
        QCOMPARE(spy->count(), 6);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);

        // native function entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));

        // script function entry
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);

        // script function exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);

        // native function exit
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(4).scriptId, qint64(-1));
        QVERIFY(spy->at(4).value.isNumber());
        QCOMPARE(spy->at(4).value.toNumber(), qsreal(123));

        // evaluate() exit
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(5).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(5).value.isNumber());
        QCOMPARE(spy->at(5).value.toNumber(), qsreal(123));
    }

    {
        QScriptValue fun = eng.newFunction(nativeFunctionThrowingError);
        eng.globalObject().setProperty("nativeFunctionThrowingError", fun);

        spy->clear();
        eng.evaluate("nativeFunctionThrowingError('ciao')");
        QCOMPARE(spy->count(), 4);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);

        // native function entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));

        // native function exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isError());

        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(3).value.isError());
    }

    {
        spy->clear();
        eng.evaluate("'ciao'.toString()");
        QCOMPARE(spy->count(), 4);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);

        // built-in native function entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));

        // built-in native function exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isString());
        QCOMPARE(spy->at(2).value.toString(), QString("ciao"));

        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(3).value.isString());
        QCOMPARE(spy->at(3).value.toString(), QString("ciao"));
    }

    {
        spy->clear();
        eng.evaluate("Array(); Boolean(); Date(); Function(); Number(); Object(); RegExp(); String()");
        QCOMPARE(spy->count(), 20);

        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);

        // Array constructor entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));

        // Array constructor exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isArray());

        // Boolean constructor entry
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(3).scriptId, qint64(-1));

        // Boolean constructor exit
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(4).scriptId, qint64(-1));
        QVERIFY(spy->at(4).value.isBoolean());

        // Date constructor entry
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(5).scriptId, qint64(-1));

        // Date constructor exit
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(6).scriptId, qint64(-1));
        QVERIFY(spy->at(6).value.isString());

        // Function constructor entry
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(7).scriptId, qint64(-1));

        // evaluate() entry
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(8).scriptId != -1);

        // evaluate() exit
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(9).scriptId, spy->at(8).scriptId);
        QVERIFY(spy->at(9).value.isFunction());

        // Function constructor exit
        QCOMPARE(spy->at(10).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(10).scriptId, qint64(-1));
        QVERIFY(spy->at(10).value.isFunction());

        // Number constructor entry
        QCOMPARE(spy->at(11).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(11).scriptId, qint64(-1));

        // Number constructor exit
        QCOMPARE(spy->at(12).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(12).scriptId, qint64(-1));
        QVERIFY(spy->at(12).value.isNumber());

        // Object constructor entry
        QCOMPARE(spy->at(13).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(13).scriptId, qint64(-1));

        // Object constructor exit
        QCOMPARE(spy->at(14).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(14).scriptId, qint64(-1));
        QVERIFY(spy->at(14).value.isObject());

        // RegExp constructor entry
        QCOMPARE(spy->at(15).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(15).scriptId, qint64(-1));

        // RegExp constructor exit
        QCOMPARE(spy->at(16).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(16).scriptId, qint64(-1));
        QVERIFY(spy->at(16).value.isRegExp());

        // String constructor entry
        QCOMPARE(spy->at(17).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(17).scriptId, qint64(-1));

        // String constructor exit
        QCOMPARE(spy->at(18).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(18).scriptId, qint64(-1));
        QVERIFY(spy->at(18).value.isString());

        // evaluate() exit
        QCOMPARE(spy->at(19).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(19).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(19).value.isString());
        QCOMPARE(spy->at(19).value.toString(), QString());
    }

    // slots
    {
        eng.globalObject().setProperty("qobj", eng.newQObject(this));
        spy->clear();
        eng.evaluate("qobj.testSlot(123)");
        QCOMPARE(spy->count(), 4);
        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        // testSlot() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));
        // testSlot() exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isNumber());
        QCOMPARE(spy->at(2).value.toNumber(), qsreal(123));
        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
    }

    // property accessors
    {
        eng.globalObject().setProperty("qobj", eng.newQObject(this));
        // set
        spy->clear();
        eng.evaluate("qobj.testProperty = 456");
        QCOMPARE(spy->count(), 4);
        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        // setTestProperty() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));
        // setTestProperty() exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isNumber());
        QCOMPARE(spy->at(2).value.toNumber(), testProperty());
        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QVERIFY(spy->at(3).value.strictlyEquals(spy->at(2).value));

        // get
        spy->clear();
        eng.evaluate("qobj.testProperty");
        QCOMPARE(spy->count(), 4);
        // evaluate() entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        // testProperty() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, qint64(-1));
        // testProperty() exit
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(2).scriptId, qint64(-1));
        QVERIFY(spy->at(2).value.isNumber());
        QCOMPARE(spy->at(2).value.toNumber(), testProperty());
        // evaluate() exit
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QVERIFY(spy->at(3).value.strictlyEquals(spy->at(2).value));
    }

    // calling script functions from C++

    {
        QScriptValue fun = eng.evaluate("function foo() { return 123; }; foo");
        QVERIFY(fun.isFunction());

        spy->clear();
        fun.call();
        QCOMPARE(spy->count(), 2);

        // entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId != -1);

        // exit
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.isNumber());
        QCOMPARE(spy->at(1).value.toNumber(), qsreal(123));
    }

    for (int x = 0; x < 2; ++x) {
        QScriptValue fun = eng.newFunction(nativeFunctionReturningArg);

        spy->clear();
        QScriptValueList args;
        args << QScriptValue(&eng, 123);
        if (x)
            fun.construct(args);
        else
            fun.call(QScriptValue(), args);
        QCOMPARE(spy->count(), 2);

        // entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId == -1);

        // exit
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.strictlyEquals(args.at(0)));
    }

    for (int x = 0; x < 2; ++x) {
        QScriptValue fun = eng.evaluate("Boolean");

        spy->clear();
        QScriptValueList args;
        args << QScriptValue(&eng, true);
        if (x)
            fun.construct(args);
        else
            fun.call(QScriptValue(), args);
        QCOMPARE(spy->count(), 2);

        // entry
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::FunctionEntry);
        QVERIFY(spy->at(0).scriptId == -1);

        // exit
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.equals(args.at(0)));
    }
}

void tst_QScriptEngineAgent::positionChange()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnorePositionChange));
    {
        spy->clear();
        eng.evaluate(";");
        QCOMPARE(spy->count(), 1);
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        int lineNumber = 123;
        eng.evaluate("1 + 2", "foo.qs", lineNumber);
        QCOMPARE(spy->count(), 1);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        int lineNumber = 123;
        eng.evaluate("var i = 0", "foo.qs", lineNumber);
        QCOMPARE(spy->count(), 1);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        int lineNumber = 456;
        eng.evaluate("1 + 2; 3 + 4;\n5 + 6", "foo.qs", lineNumber);
        QCOMPARE(spy->count(), 3);

        // 1 + 2
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, lineNumber);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // 3 + 4
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, lineNumber);
        QCOMPARE(spy->at(1).columnNumber, 8);

        // 5 + 6
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, lineNumber + 1);
        QCOMPARE(spy->at(2).columnNumber, 1);
    }

    {
        spy->clear();
        int lineNumber = 789;
        eng.evaluate("function foo() { return 123; }", "foo.qs", lineNumber);
        QCOMPARE(spy->count(), 0);

        eng.evaluate("foo()");
        QCOMPARE(spy->count(), 2);

        // foo()
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // return 123
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(1).scriptId != spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, lineNumber);
        QCOMPARE(spy->at(1).columnNumber, 18);
    }

    {
        spy->clear();
        eng.evaluate("if (true) i = 1; else i = 0;");
        QCOMPARE(spy->count(), 2);

        // if
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // i = 1
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 11);
    }

    {
        spy->clear();
        eng.evaluate("for (var i = 0; i < 2; ++i) { }");
        QCOMPARE(spy->count(), 1);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        eng.evaluate("for (var i = 0; i < 2; ++i) { void(i); }");
        QCOMPARE(spy->count(), 3);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // void(i)
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 31);

        // void(i)
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 31);
    }

    {
        spy->clear();
        eng.evaluate("var i = 0; while (i < 2) { ++i; }");
        QCOMPARE(spy->count(), 4);

        // i = 0
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // while
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 12);

        // ++i
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 28);

        // ++i
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(3).lineNumber, 1);
        QCOMPARE(spy->at(3).columnNumber, 28);
    }

    {
        spy->clear();
        eng.evaluate("var i = 0; do { ++i; } while (i < 2)");
        QCOMPARE(spy->count(), 5);

        // i = 0
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // do
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 12);

        // ++i
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 17);

        // do
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(3).lineNumber, 1);
        QCOMPARE(spy->at(3).columnNumber, 12);

        // ++i
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(4).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(4).lineNumber, 1);
        QCOMPARE(spy->at(4).columnNumber, 17);
    }

    {
        spy->clear();
        eng.evaluate("for (var i in { }) { void(i); }");
        QCOMPARE(spy->count(), 1);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        eng.evaluate("for (var i in { a: 10, b: 20 }) { void(i); }");
        QCOMPARE(spy->count(), 5);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // a: 10
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 20);

        // b: 20
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 27);

        // void(i)
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(3).lineNumber, 1);
        QCOMPARE(spy->at(3).columnNumber, 35);

        // void(i)
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(4).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(4).lineNumber, 1);
        QCOMPARE(spy->at(4).columnNumber, 35);
    }

    {
        spy->clear();
        eng.evaluate("for ( ; ; ) { break; }");
        QCOMPARE(spy->count(), 2);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // break
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 15);
    }

    {
        spy->clear();
        eng.evaluate("for (var i = 0 ; i < 2; ++i) { continue; }");
        QCOMPARE(spy->count(), 3);

        // for
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // continue
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 32);

        // continue
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 32);
    }

    {
        spy->clear();
        eng.evaluate("with (this) { }");
        QCOMPARE(spy->count(), 1);

        // with
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        eng.evaluate("switch (undefined) { }");
        QCOMPARE(spy->count(), 1);

        // switch
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        eng.evaluate("switch (undefined) { default: i = 5; }");
        QCOMPARE(spy->count(), 2);

        // switch
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // i = 5
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 31);
    }

    {
        spy->clear();
        eng.evaluate("switch (undefined) { case undefined: i = 5; break; }");
        QCOMPARE(spy->count(), 3);

        // switch
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);

        // i = 5
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 38);

        // break
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 45);
    }

    {
        spy->clear();
        eng.evaluate("throw 1");
        QCOMPARE(spy->count(), 1);

        // throw
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 1);
    }

    {
        spy->clear();
        eng.evaluate("try { i = 1; } catch(e) { i = 2; } finally { i = 3; }");
        QCOMPARE(spy->count(), 2);

        // i = 1
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 7);

        // i = 3
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 46);
    }

    {
        spy->clear();
        eng.evaluate("try { throw 1; } catch(e) { i = e; } finally { i = 2; }");
        QCOMPARE(spy->count(), 3);

        // throw 1
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::PositionChange);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(0).lineNumber, 1);
        QCOMPARE(spy->at(0).columnNumber, 7);

        // i = e
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, 1);
        QCOMPARE(spy->at(1).columnNumber, 29);

        // i = 2
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).lineNumber, 1);
        QCOMPARE(spy->at(2).columnNumber, 48);
    }
}

void tst_QScriptEngineAgent::exceptionThrowAndCatch()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnoreExceptionThrow
                                                       | ScriptEngineSpy::IgnoreExceptionCatch));
    {
        spy->clear();
        eng.evaluate(";");
        QCOMPARE(spy->count(), 0);
    }

    {
        spy->clear();
        eng.evaluate("try { i = 5; } catch (e) { }");
        QCOMPARE(spy->count(), 0);
    }

    {
        spy->clear();
        eng.evaluate("throw new Error('ciao');");
        QCOMPARE(spy->count(), 1);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ExceptionThrow);
        QVERIFY(spy->at(0).scriptId != -1);
        QVERIFY(!spy->at(0).hasExceptionHandler);
        QVERIFY(spy->at(0).value.isError());
        QCOMPARE(spy->at(0).value.toString(), QString("Error: ciao"));
    }

    {
        spy->clear();
        eng.evaluate("try { throw new Error('ciao'); } catch (e) { }");
        QCOMPARE(spy->count(), 2);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ExceptionThrow);
        QVERIFY(spy->at(0).scriptId != -1);
        QVERIFY(spy->at(0).hasExceptionHandler);
        QVERIFY(spy->at(0).value.isError());
        QCOMPARE(spy->at(0).value.toString(), QString("Error: ciao"));
        QVERIFY(spy->at(0).scriptId != -1);

        QCOMPARE(spy->at(1).type, ScriptEngineEvent::ExceptionCatch);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(1).value.strictlyEquals(spy->at(0).value));
    }
}

void tst_QScriptEngineAgent::eventOrder()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng);

    {
        spy->clear();
        eng.evaluate("i = 3; i = 5");
        QCOMPARE(spy->count(), 6);
        // load
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        // i = 3
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        // i = 5
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        // evaluate() exit
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(4).scriptId, spy->at(0).scriptId);
        // unload
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(5).scriptId, spy->at(0).scriptId);
    }

    {
        spy->clear();
        eng.evaluate("function foo(arg) { void(arg); }");
        QCOMPARE(spy->count(), 3);
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::FunctionExit);

        eng.evaluate("foo(123)");
        QCOMPARE(spy->count(), 13);
        // load
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(3).scriptId != spy->at(0).scriptId);
        // evaluate() entry
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(4).scriptId, spy->at(3).scriptId);
        // foo()
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(5).scriptId, spy->at(3).scriptId);
        // new context
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::ContextPush);
        // foo() entry
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(7).scriptId, spy->at(0).scriptId);
        // void(arg)
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(8).scriptId, spy->at(0).scriptId);
        // foo() exit
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(9).scriptId, spy->at(0).scriptId);
        // restore context
        QCOMPARE(spy->at(10).type, ScriptEngineEvent::ContextPop);
        // evaluate() exit
        QCOMPARE(spy->at(11).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(11).scriptId, spy->at(3).scriptId);
        // unload
        QCOMPARE(spy->at(12).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(12).scriptId, spy->at(3).scriptId);

        eng.evaluate("foo = null");
        eng.collectGarbage();
    }

    {
        spy->clear();
        eng.evaluate("throw new Error('ciao')");
        QCOMPARE(spy->count(), 10);
        // load
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        // throw
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        // new context
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::ContextPush);
        // Error constructor entry
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionEntry);
        // Error constructor exit
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionExit);
        // restore context
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::ContextPop);
        // exception
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::ExceptionThrow);
        QVERIFY(!spy->at(7).hasExceptionHandler);
        // evaluate() exit
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::FunctionExit);
        // unload
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::ScriptUnload);
    }

    {
        spy->clear();
        eng.evaluate("try { throw new Error('ciao') } catch (e) { void(e); }");
        QCOMPARE(spy->count(), 12);
        // load
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        // throw
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        // new context
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::ContextPush);
        // Error constructor entry
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::FunctionEntry);
        // Error constructor exit
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionExit);
        // restore context
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::ContextPop);
        // exception
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::ExceptionThrow);
        QVERIFY(spy->at(7).value.isError());
        QVERIFY(spy->at(7).hasExceptionHandler);
        // catch
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::ExceptionCatch);
        QVERIFY(spy->at(8).value.isError());
        // void(e)
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::PositionChange);
        // evaluate() exit
        QCOMPARE(spy->at(10).type, ScriptEngineEvent::FunctionExit);
        QVERIFY(spy->at(10).value.isUndefined());
        // unload
        QCOMPARE(spy->at(11).type, ScriptEngineEvent::ScriptUnload);
    }

    {
        spy->clear();
        eng.evaluate("function foo(arg) { return bar(arg); }");
        eng.evaluate("function bar(arg) { return arg; }");
        QCOMPARE(spy->count(), 6);

        eng.evaluate("foo(123)");
        QCOMPARE(spy->count(), 21);

        // load
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::FunctionEntry);
        // foo(123)
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::PositionChange);
        // new context
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::ContextPush);
        // foo() entry
        QCOMPARE(spy->at(10).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(10).scriptId, spy->at(0).scriptId);
        // return bar(arg)
        QCOMPARE(spy->at(11).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(11).scriptId, spy->at(0).scriptId);
        // new context
        QCOMPARE(spy->at(12).type, ScriptEngineEvent::ContextPush);
        // bar() entry
        QCOMPARE(spy->at(13).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(13).scriptId, spy->at(3).scriptId);
        // return arg
        QCOMPARE(spy->at(14).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(14).scriptId, spy->at(3).scriptId);
        // bar() exit
        QCOMPARE(spy->at(15).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(15).scriptId, spy->at(3).scriptId);
        QVERIFY(spy->at(15).value.isNumber());
        // restore context
        QCOMPARE(spy->at(16).type, ScriptEngineEvent::ContextPop);
        // foo() exit
        QCOMPARE(spy->at(17).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(17).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(17).value.isNumber());
        // restore context
        QCOMPARE(spy->at(18).type, ScriptEngineEvent::ContextPop);
        // evaluate() exit
        QCOMPARE(spy->at(19).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(19).scriptId, spy->at(6).scriptId);
        QVERIFY(spy->at(19).value.isNumber());
        // unload
        QCOMPARE(spy->at(20).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(20).scriptId, spy->at(6).scriptId);

        // redefine bar()
        eng.evaluate("function bar(arg) { throw new Error(arg); }");
        QCOMPARE(spy->count(), 25);
        QCOMPARE(spy->at(21).type, ScriptEngineEvent::ScriptLoad);
        QCOMPARE(spy->at(22).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(23).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(24).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(24).scriptId, spy->at(3).scriptId);

        eng.evaluate("foo('ciao')");
        QCOMPARE(spy->count(), 45);

        // load
        QCOMPARE(spy->at(25).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(26).type, ScriptEngineEvent::FunctionEntry);
        // foo('ciao')
        QCOMPARE(spy->at(27).type, ScriptEngineEvent::PositionChange);
        // new context
        QCOMPARE(spy->at(28).type, ScriptEngineEvent::ContextPush);
        // foo() entry
        QCOMPARE(spy->at(29).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(29).scriptId, spy->at(0).scriptId);
        // return bar(arg)
        QCOMPARE(spy->at(30).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(30).scriptId, spy->at(0).scriptId);
        // new context
        QCOMPARE(spy->at(31).type, ScriptEngineEvent::ContextPush);
        // bar() entry
        QCOMPARE(spy->at(32).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(32).scriptId, spy->at(21).scriptId);
        // throw
        QCOMPARE(spy->at(33).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(33).scriptId, spy->at(21).scriptId);
        // new context
        QCOMPARE(spy->at(34).type, ScriptEngineEvent::ContextPush);
        // Error constructor entry
        QCOMPARE(spy->at(35).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(35).scriptId, qint64(-1));
        // Error constructor exit
        QCOMPARE(spy->at(36).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(36).scriptId, qint64(-1));
        // restore context
        QCOMPARE(spy->at(37).type, ScriptEngineEvent::ContextPop);
        // exception
        QCOMPARE(spy->at(38).type, ScriptEngineEvent::ExceptionThrow);
        QCOMPARE(spy->at(38).scriptId, spy->at(21).scriptId);
        QVERIFY(!spy->at(38).hasExceptionHandler);
        // bar() exit
        QCOMPARE(spy->at(39).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(39).scriptId, spy->at(21).scriptId);
        QVERIFY(spy->at(39).value.isError());
        // restore context
        QCOMPARE(spy->at(40).type, ScriptEngineEvent::ContextPop);
        // foo() exit
        QCOMPARE(spy->at(41).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(41).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(41).value.isError());
        // restore context
        QCOMPARE(spy->at(42).type, ScriptEngineEvent::ContextPop);
        // evaluate() exit
        QCOMPARE(spy->at(43).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(43).scriptId, spy->at(26).scriptId);
        QVERIFY(spy->at(43).value.isError());
        // unload
        QCOMPARE(spy->at(44).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(44).scriptId, spy->at(25).scriptId);
    }

    {
        spy->clear();
        eng.evaluate("try { throw 1; } catch(e) { i = e; } finally { i = 2; }");
        QCOMPARE(spy->count(), 9);

        // load
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        // evaluate() entry
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        // throw 1
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        // i = e
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::ExceptionThrow);
        // catch
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::ExceptionCatch);
        // i = e
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::PositionChange);
        // i = 2
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::PositionChange);
        // evaluate() exit
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::FunctionExit);
        // unload
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::ScriptUnload);
    }

    // signal handling
    {
        spy->clear();
        QScriptValue fun = eng.evaluate("function(arg) { throw Error(arg); }");
        QVERIFY(fun.isFunction());
        QCOMPARE(spy->count(), 4);
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);

        qScriptConnect(this, SIGNAL(testSignal(double)),
                       QScriptValue(), fun);

        emit testSignal(123);

        QCOMPARE(spy->count(), 14);
        // new context
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::ContextPush);
        // anonymous function entry
        QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(5).scriptId, spy->at(0).scriptId);
        // throw
        QCOMPARE(spy->at(6).type, ScriptEngineEvent::PositionChange);
        QCOMPARE(spy->at(6).scriptId, spy->at(0).scriptId);
        // new context
        QCOMPARE(spy->at(7).type, ScriptEngineEvent::ContextPush);
        // Error constructor entry
        QCOMPARE(spy->at(8).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(8).scriptId, qint64(-1));
        // Error constructor exit
        QCOMPARE(spy->at(9).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(9).scriptId, qint64(-1));
        // restore context
        QCOMPARE(spy->at(10).type, ScriptEngineEvent::ContextPop);
        // exception
        QCOMPARE(spy->at(11).type, ScriptEngineEvent::ExceptionThrow);
        QCOMPARE(spy->at(11).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(11).value.isError());
        QVERIFY(!spy->at(11).hasExceptionHandler);
        // anonymous function exit
        QCOMPARE(spy->at(12).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(12).scriptId, spy->at(0).scriptId);
        QVERIFY(spy->at(12).value.isError());
        // restore context
        QCOMPARE(spy->at(13).type, ScriptEngineEvent::ContextPop);
    }
}

class DoubleAgent : public ScriptEngineSpy
{
public:
    DoubleAgent(QScriptEngine *engine) : ScriptEngineSpy(engine) { }
    ~DoubleAgent() { }

    void positionChange(qint64 scriptId, int lineNumber, int columnNumber)
    {
        if (lineNumber == 123)
            engine()->evaluate("1 + 2");
        ScriptEngineSpy::positionChange(scriptId, lineNumber, columnNumber);
    }
};

void tst_QScriptEngineAgent::recursiveObserve()
{
    QScriptEngine eng;
    DoubleAgent *spy = new DoubleAgent(&eng);

    eng.evaluate("3 + 4", "foo.qs", 123);

    QCOMPARE(spy->count(), 10);
    // load "3 + 4"
    QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
    // evaluate() entry
    QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
    // load "1 + 2"
    QCOMPARE(spy->at(2).type, ScriptEngineEvent::ScriptLoad);
    // evaluate() entry
    QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionEntry);
    // 1 + 2
    QCOMPARE(spy->at(4).type, ScriptEngineEvent::PositionChange);
    QCOMPARE(spy->at(4).scriptId, spy->at(2).scriptId);
    // evaluate() exit
    QCOMPARE(spy->at(5).type, ScriptEngineEvent::FunctionExit);
    // unload "1 + 2"
    QCOMPARE(spy->at(6).type, ScriptEngineEvent::ScriptUnload);
    QCOMPARE(spy->at(6).scriptId, spy->at(2).scriptId);
    // 3 + 4
    QCOMPARE(spy->at(7).type, ScriptEngineEvent::PositionChange);
    QCOMPARE(spy->at(7).scriptId, spy->at(0).scriptId);
    // evaluate() exit
    QCOMPARE(spy->at(8).type, ScriptEngineEvent::FunctionExit);
    // unload "3 + 4"
    QCOMPARE(spy->at(9).type, ScriptEngineEvent::ScriptUnload);
    QCOMPARE(spy->at(9).scriptId, spy->at(0).scriptId);
}

void tst_QScriptEngineAgent::multipleAgents()
{
    QScriptEngine eng;
    QCOMPARE(eng.agent(), (QScriptEngineAgent *)0);
    ScriptEngineSpy *spy1 = new ScriptEngineSpy(&eng);
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)spy1);
    ScriptEngineSpy *spy2 = new ScriptEngineSpy(&eng);
    QCOMPARE(eng.agent(), (QScriptEngineAgent*)spy2);

    eng.evaluate("1 + 2");
    QCOMPARE(spy1->count(), 0);
    QCOMPARE(spy2->count(), 5);

    spy2->clear();
    eng.setAgent(spy1);
    eng.evaluate("1 + 2");
    QCOMPARE(spy2->count(), 0);
    QCOMPARE(spy1->count(), 5);
}

void tst_QScriptEngineAgent::syntaxError()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng);
    {
        spy->clear();
        eng.evaluate("{");
        QCOMPARE(spy->count(), 5);

        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QVERIFY(spy->at(0).scriptId != -1);
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::FunctionEntry);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(2).type, ScriptEngineEvent::ExceptionThrow);
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QVERIFY(!spy->at(2).hasExceptionHandler);
        QVERIFY(spy->at(2).value.isError());
        QCOMPARE(spy->at(2).value.toString(), QString("SyntaxError: Expected `}'"));
        QCOMPARE(spy->at(2).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(3).type, ScriptEngineEvent::FunctionExit);
        QCOMPARE(spy->at(3).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(4).type, ScriptEngineEvent::ScriptUnload);
        QCOMPARE(spy->at(4).scriptId, spy->at(0).scriptId);
    }
}

void tst_QScriptEngineAgent::extension()
{
    QScriptEngine eng;
    ScriptEngineSpy *spy = new ScriptEngineSpy(&eng, ~(ScriptEngineSpy::IgnoreDebuggerInvocationRequest
                                                   | ScriptEngineSpy::IgnoreScriptLoad));
    // DebuggerInvocationRequest
    {
        spy->clear();

        QString fileName = "foo.qs";
        int lineNumber = 123;
        QScriptValue ret = eng.evaluate("debugger", fileName, lineNumber);

        QCOMPARE(spy->count(), 2);
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);
        QCOMPARE(spy->at(1).type, ScriptEngineEvent::DebuggerInvocationRequest);
        QCOMPARE(spy->at(1).scriptId, spy->at(0).scriptId);
        QCOMPARE(spy->at(1).lineNumber, lineNumber);
        QCOMPARE(spy->at(1).columnNumber, 1);

        QVERIFY(ret.isString());
        QCOMPARE(ret.toString(), QString::fromLatin1("extension(DebuggerInvocationRequest)"));
    }
    {
        spy->clear();
        spy->enableIgnoreFlags(ScriptEngineSpy::IgnoreDebuggerInvocationRequest);

        QScriptValue ret = eng.evaluate("debugger");

        QCOMPARE(spy->count(), 1);
        QCOMPARE(spy->at(0).type, ScriptEngineEvent::ScriptLoad);

        QVERIFY(ret.isUndefined());
    }
}

QTEST_MAIN(tst_QScriptEngineAgent)
#include "tst_qscriptengineagent.moc"
