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

class tst_QScriptValue : public QObject
{
    Q_OBJECT

public:
    tst_QScriptValue();
    virtual ~tst_QScriptValue();

public slots:
    void init();
    void cleanup();

private slots:
    void numberConstructor();
    void stringConstructor();
    void call_data();
    void call();
    void construct_data();
    void construct();
    void toString_data();
    void toString();
    void toQObject();
    void property();
    void setProperty();
    void propertyFlags();
    void readMetaProperty();
    void writeMetaProperty();
};

tst_QScriptValue::tst_QScriptValue()
{
}

tst_QScriptValue::~tst_QScriptValue()
{
}

void tst_QScriptValue::init()
{
}

void tst_QScriptValue::cleanup()
{
}

void tst_QScriptValue::numberConstructor()
{
    QBENCHMARK {
        (void)QScriptValue(123);
    }
}

void tst_QScriptValue::stringConstructor()
{
    QString str = QString::fromLatin1("ciao");
    QBENCHMARK {
        (void)QScriptValue(str);
    }
}

void tst_QScriptValue::call_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("empty function") << QString::fromLatin1("(function(){})");
    QTest::newRow("function returning number") << QString::fromLatin1("(function(){ return 123; })");
    QTest::newRow("closure") << QString::fromLatin1("(function(a, b){ return function() { return a + b; }; })(1, 2)");
}

void tst_QScriptValue::call()
{
    QFETCH(QString, code);
    QScriptEngine engine;
    QScriptValue fun = engine.evaluate(code);
    QVERIFY(fun.isFunction());
    QBENCHMARK {
        (void)fun.call();
    }
}

void tst_QScriptValue::construct_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("empty function") << QString::fromLatin1("(function(){})");
    QTest::newRow("simple constructor") << QString::fromLatin1("(function(){ this.x = 10; this.y = 20; })");
}

void tst_QScriptValue::construct()
{
    QFETCH(QString, code);
    QScriptEngine engine;
    QScriptValue fun = engine.evaluate(code);
    QVERIFY(fun.isFunction());
    QBENCHMARK {
        (void)fun.construct();
    }
}

void tst_QScriptValue::toString_data()
{
    QTest::addColumn<QString>("code");
    QTest::newRow("number") << QString::fromLatin1("123");
    QTest::newRow("string") << QString::fromLatin1("'ciao'");
    QTest::newRow("null") << QString::fromLatin1("null");
    QTest::newRow("undefined") << QString::fromLatin1("undefined");
    QTest::newRow("function") << QString::fromLatin1("(function foo(a, b, c) { return a + b + c; })");
}

void tst_QScriptValue::toString()
{
    QFETCH(QString, code);
    QScriptEngine engine;
    QScriptValue val = engine.evaluate(code);
    QBENCHMARK {
        (void)val.toString();
    }
}

void tst_QScriptValue::toQObject()
{
    QScriptEngine engine;
    QScriptValue obj = engine.newQObject(QCoreApplication::instance());
    QBENCHMARK {
        (void)obj.toQObject();
    }
}

void tst_QScriptValue::property()
{
    QScriptEngine engine;
    QScriptValue obj = engine.newObject();
    QString propertyName = QString::fromLatin1("foo");
    obj.setProperty(propertyName, 123);
    QBENCHMARK {
        (void)obj.property(propertyName);
    }
}

void tst_QScriptValue::setProperty()
{
    QScriptEngine engine;
    QScriptValue obj = engine.newObject();
    QString propertyName = QString::fromLatin1("foo");
    QScriptValue val(123);
    QBENCHMARK {
        obj.setProperty(propertyName, val);
    }
}

void tst_QScriptValue::propertyFlags()
{
    QScriptEngine engine;
    QScriptValue obj = engine.newObject();
    QString propertyName = QString::fromLatin1("foo");
    obj.setProperty(propertyName, 123, QScriptValue::SkipInEnumeration | QScriptValue::ReadOnly);
    QBENCHMARK {
        (void)obj.propertyFlags(propertyName);
    }
}

void tst_QScriptValue::readMetaProperty()
{
    QScriptEngine engine;
    QScriptValue object = engine.newQObject(QCoreApplication::instance());
    QScriptString propertyName = engine.toStringHandle("objectName");
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            object.property(propertyName);
    }
}

void tst_QScriptValue::writeMetaProperty()
{
    QScriptEngine engine;
    QScriptValue object = engine.newQObject(QCoreApplication::instance());
    QScriptString propertyName = engine.toStringHandle("objectName");
    QScriptValue value(&engine, "foo");
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            object.setProperty(propertyName, value);
    }
}

QTEST_MAIN(tst_QScriptValue)
#include "tst_qscriptvalue.moc"
