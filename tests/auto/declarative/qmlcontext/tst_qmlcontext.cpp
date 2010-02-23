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
#include <QDebug>
#include <QmlEngine>
#include <QmlContext>
#include <QmlComponent>
#include <QmlExpression>

class tst_qmlcontext : public QObject
{
    Q_OBJECT
public:
    tst_qmlcontext() {}

private slots:
    void baseUrl();
    void resolvedUrl();
    void engineMethod();
    void parentContext();
    void setContextProperty();
    void addDefaultObject();
    void destruction();

private:
    QmlEngine engine;
};

void tst_qmlcontext::baseUrl()
{
    QmlContext ctxt(&engine);

    QCOMPARE(ctxt.baseUrl(), QUrl());

    ctxt.setBaseUrl(QUrl("http://www.nokia.com/"));

    QCOMPARE(ctxt.baseUrl(), QUrl("http://www.nokia.com/"));
}

void tst_qmlcontext::resolvedUrl()
{
    // Relative to the component
    {
        QmlContext ctxt(&engine);
        ctxt.setBaseUrl(QUrl("http://www.nokia.com/"));

        QCOMPARE(ctxt.resolvedUrl(QUrl("main.qml")), QUrl("http://www.nokia.com/main.qml"));
    }

    // Relative to a parent
    {
        QmlContext ctxt(&engine);
        ctxt.setBaseUrl(QUrl("http://www.nokia.com/"));

        QmlContext ctxt2(&ctxt);
        QCOMPARE(ctxt2.resolvedUrl(QUrl("main2.qml")), QUrl("http://www.nokia.com/main2.qml"));
    }

    // Relative to the engine
    {
        QmlContext ctxt(&engine);
        QCOMPARE(ctxt.resolvedUrl(QUrl("main.qml")), engine.baseUrl().resolved(QUrl("main.qml")));
    }

    // Relative to a deleted parent
    {
        QmlContext *ctxt = new QmlContext(&engine);
        ctxt->setBaseUrl(QUrl("http://www.nokia.com/"));

        QmlContext ctxt2(ctxt);
        QCOMPARE(ctxt2.resolvedUrl(QUrl("main2.qml")), QUrl("http://www.nokia.com/main2.qml"));

        delete ctxt; ctxt = 0;

        QCOMPARE(ctxt2.resolvedUrl(QUrl("main2.qml")), QUrl());
    }

    // Absolute
    {
        QmlContext ctxt(&engine);

        QCOMPARE(ctxt.resolvedUrl(QUrl("http://www.nokia.com/main2.qml")), QUrl("http://www.nokia.com/main2.qml"));
        QCOMPARE(ctxt.resolvedUrl(QUrl("file:///main2.qml")), QUrl("file:///main2.qml"));
    }
}

void tst_qmlcontext::engineMethod()
{
    QmlEngine *engine = new QmlEngine;

    QmlContext ctxt(engine);
    QmlContext ctxt2(&ctxt);
    QmlContext ctxt3(&ctxt2);
    QmlContext ctxt4(&ctxt2);

    QCOMPARE(ctxt.engine(), engine);
    QCOMPARE(ctxt2.engine(), engine);
    QCOMPARE(ctxt3.engine(), engine);
    QCOMPARE(ctxt4.engine(), engine);

    delete engine; engine = 0;

    QCOMPARE(ctxt.engine(), engine);
    QCOMPARE(ctxt2.engine(), engine);
    QCOMPARE(ctxt3.engine(), engine);
    QCOMPARE(ctxt4.engine(), engine);
}

void tst_qmlcontext::parentContext()
{
    QmlEngine *engine = new QmlEngine;

    QCOMPARE(engine->rootContext()->parentContext(), (QmlContext *)0);

    QmlContext *ctxt = new QmlContext(engine);
    QmlContext *ctxt2 = new QmlContext(ctxt);
    QmlContext *ctxt3 = new QmlContext(ctxt2);
    QmlContext *ctxt4 = new QmlContext(ctxt2);
    QmlContext *ctxt5 = new QmlContext(ctxt);
    QmlContext *ctxt6 = new QmlContext(engine);
    QmlContext *ctxt7 = new QmlContext(engine->rootContext());

    QCOMPARE(ctxt->parentContext(), engine->rootContext());
    QCOMPARE(ctxt2->parentContext(), ctxt);
    QCOMPARE(ctxt3->parentContext(), ctxt2);
    QCOMPARE(ctxt4->parentContext(), ctxt2);
    QCOMPARE(ctxt5->parentContext(), ctxt);
    QCOMPARE(ctxt6->parentContext(), engine->rootContext());
    QCOMPARE(ctxt7->parentContext(), engine->rootContext());

    delete ctxt2; ctxt2 = 0;

    QCOMPARE(ctxt->parentContext(), engine->rootContext());
    QCOMPARE(ctxt3->parentContext(), ctxt2);
    QCOMPARE(ctxt4->parentContext(), ctxt2);
    QCOMPARE(ctxt5->parentContext(), ctxt);
    QCOMPARE(ctxt6->parentContext(), engine->rootContext());
    QCOMPARE(ctxt7->parentContext(), engine->rootContext());

    delete engine; engine = 0;

    QCOMPARE(ctxt->parentContext(), (QmlContext *)0);
    QCOMPARE(ctxt3->parentContext(), ctxt2);
    QCOMPARE(ctxt4->parentContext(), ctxt2);
    QCOMPARE(ctxt5->parentContext(), ctxt);
    QCOMPARE(ctxt6->parentContext(), (QmlContext *)0);
    QCOMPARE(ctxt7->parentContext(), (QmlContext *)0);

    delete ctxt7;
    delete ctxt6;
    delete ctxt5;
    delete ctxt4;
    delete ctxt3;
    delete ctxt;
}

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int a READ a NOTIFY aChanged)
    Q_PROPERTY(int b READ b NOTIFY bChanged)
    Q_PROPERTY(int c READ c NOTIFY cChanged)

public:
    TestObject() : _a(10), _b(10), _c(10) {}

    int a() const { return _a; }
    void setA(int a) { _a = a; emit aChanged(); }

    int b() const { return _b; }
    void setB(int b) { _b = b; emit bChanged(); }

    int c() const { return _c; }
    void setC(int c) { _c = c; emit cChanged(); }

signals:
    void aChanged();
    void bChanged();
    void cChanged();

private:
    int _a;
    int _b;
    int _c;
};

class TestObject2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int b READ b NOTIFY bChanged)

public:
    TestObject2() : _b(10) {}

    int b() const { return _b; }
    void setB(int b) { _b = b; emit bChanged(); }

signals:
    void bChanged();

private:
    int _b;
};

#define TEST_CONTEXT_PROPERTY(ctxt, name, value) \
{ \
    QmlComponent component(&engine); \
    component.setData("import Qt 4.6; QtObject { property var test: " #name " }", QUrl()); \
\
    QObject *obj = component.create(ctxt); \
\
    QCOMPARE(obj->property("test"), value); \
\
    delete obj; \
} 

void tst_qmlcontext::setContextProperty()
{
    QmlContext ctxt(&engine);
    QmlContext ctxt2(&ctxt);

    TestObject obj1;
    obj1.setA(3345);
    TestObject obj2;
    obj2.setA(-19);

    // Static context properties
    ctxt.setContextProperty("a", QVariant(10));
    ctxt.setContextProperty("b", QVariant(9));
    ctxt2.setContextProperty("d", &obj2);
    ctxt2.setContextProperty("b", QVariant(19));
    ctxt2.setContextProperty("c", QVariant(QString("Hello World!")));
    ctxt.setContextProperty("d", &obj1);
    ctxt.setContextProperty("e", &obj1);

    TEST_CONTEXT_PROPERTY(&ctxt2, a, QVariant(10));
    TEST_CONTEXT_PROPERTY(&ctxt2, b, QVariant(19));
    TEST_CONTEXT_PROPERTY(&ctxt2, c, QVariant(QString("Hello World!")));
    TEST_CONTEXT_PROPERTY(&ctxt2, d.a, QVariant(-19));
    TEST_CONTEXT_PROPERTY(&ctxt2, e.a, QVariant(3345));

    ctxt.setContextProperty("a", QVariant(13));
    ctxt.setContextProperty("b", QVariant(4));
    ctxt2.setContextProperty("b", QVariant(8));
    ctxt2.setContextProperty("c", QVariant(QString("Hi World!")));
    ctxt2.setContextProperty("d", &obj1);
    obj1.setA(12);

    TEST_CONTEXT_PROPERTY(&ctxt2, a, QVariant(13));
    TEST_CONTEXT_PROPERTY(&ctxt2, b, QVariant(8));
    TEST_CONTEXT_PROPERTY(&ctxt2, c, QVariant(QString("Hi World!")));
    TEST_CONTEXT_PROPERTY(&ctxt2, d.a, QVariant(12));
    TEST_CONTEXT_PROPERTY(&ctxt2, e.a, QVariant(12));

    // Changes in context properties
    {
        QmlComponent component(&engine); 
        component.setData("import Qt 4.6; QtObject { property var test: a }", QUrl()); 

        QObject *obj = component.create(&ctxt2); 

        QCOMPARE(obj->property("test"), QVariant(13)); 
        ctxt.setContextProperty("a", QVariant(19));
        QCOMPARE(obj->property("test"), QVariant(19)); 

        delete obj; 
    }
    {
        QmlComponent component(&engine); 
        component.setData("import Qt 4.6; QtObject { property var test: b }", QUrl()); 

        QObject *obj = component.create(&ctxt2); 

        QCOMPARE(obj->property("test"), QVariant(8)); 
        ctxt.setContextProperty("b", QVariant(5));
        QCOMPARE(obj->property("test"), QVariant(8)); 
        ctxt2.setContextProperty("b", QVariant(1912));
        QCOMPARE(obj->property("test"), QVariant(1912)); 

        delete obj; 
    }
    {
        QmlComponent component(&engine); 
        component.setData("import Qt 4.6; QtObject { property var test: e.a }", QUrl()); 

        QObject *obj = component.create(&ctxt2); 

        QCOMPARE(obj->property("test"), QVariant(12)); 
        obj1.setA(13);
        QCOMPARE(obj->property("test"), QVariant(13)); 

        delete obj; 
    }

    // New context properties
    {
        QmlComponent component(&engine); 
        component.setData("import Qt 4.6; QtObject { property var test: a }", QUrl()); 

        QObject *obj = component.create(&ctxt2); 

        QCOMPARE(obj->property("test"), QVariant(19)); 
        ctxt2.setContextProperty("a", QVariant(1945));
        QCOMPARE(obj->property("test"), QVariant(1945)); 

        delete obj; 
    }

    // Setting an object-variant context property
    {
        QmlComponent component(&engine);
        component.setData("import Qt 4.6; QtObject { id: root; property int a: 10; property int test: ctxtProp.a; property var obj: root; }", QUrl());

        QmlContext ctxt(engine.rootContext());
        ctxt.setContextProperty("ctxtProp", QVariant());

        QTest::ignoreMessage(QtWarningMsg, "<Unknown File>:1: TypeError: Result of expression 'ctxtProp' [undefined] is not an object.");
        QObject *obj = component.create(&ctxt);

        QVariant v = obj->property("obj");

        ctxt.setContextProperty("ctxtProp", v);

        QCOMPARE(obj->property("test"), QVariant(10));

        delete obj;
    }
}

void tst_qmlcontext::addDefaultObject()
{
    QmlContext ctxt(&engine);

    TestObject to;
    TestObject2 to2;

    to.setA(2);
    to.setB(192);
    to.setC(18);
    to2.setB(111999);

    ctxt.addDefaultObject(&to2);
    ctxt.addDefaultObject(&to);
    ctxt.setContextProperty("c", QVariant(9));

    // Static context properties
    TEST_CONTEXT_PROPERTY(&ctxt, a, QVariant(2));
    TEST_CONTEXT_PROPERTY(&ctxt, b, QVariant(111999));
    TEST_CONTEXT_PROPERTY(&ctxt, c, QVariant(9));

    to.setA(12);
    to.setB(100);
    to.setC(7);
    to2.setB(1612);
    ctxt.setContextProperty("c", QVariant(3));

    TEST_CONTEXT_PROPERTY(&ctxt, a, QVariant(12));
    TEST_CONTEXT_PROPERTY(&ctxt, b, QVariant(1612));
    TEST_CONTEXT_PROPERTY(&ctxt, c, QVariant(3));

    // Changes in context properties
    {
        QmlComponent component(&engine); 
        component.setData("import Qt 4.6; QtObject { property var test: a }", QUrl()); 

        QObject *obj = component.create(&ctxt); 

        QCOMPARE(obj->property("test"), QVariant(12)); 
        to.setA(14);
        QCOMPARE(obj->property("test"), QVariant(14)); 

        delete obj; 
    }
}

void tst_qmlcontext::destruction()
{
    QmlContext *ctxt = new QmlContext(&engine);

    QObject obj;
    QmlEngine::setContextForObject(&obj, ctxt);
    QmlExpression expr(ctxt, "a", 0);

    QCOMPARE(ctxt, QmlEngine::contextForObject(&obj));
    QCOMPARE(ctxt, expr.context());

    delete ctxt; ctxt = 0;

    QCOMPARE(ctxt, QmlEngine::contextForObject(&obj));
    QCOMPARE(ctxt, expr.context());
}

QTEST_MAIN(tst_qmlcontext)

#include "tst_qmlcontext.moc"
