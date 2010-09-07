/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <qtest.h>
#include <QSignalSpy>
#include <QTimer>
#include <QHostAddress>
#include <QDebug>
#include <QThread>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qdeclarativeproperty.h>

#include <private/qdeclarativebinding_p.h>
#include <private/qdeclarativedebug_p.h>
#include <private/qdeclarativeenginedebug_p.h>
#include <private/qdeclarativedebugclient_p.h>
#include <private/qdeclarativedebugservice_p.h>
#include <private/qdeclarativerectangle_p.h>
#include <private/qdeclarativemetatype_p.h>
#include <private/qdeclarativeproperty_p.h>

#include "../../../shared/util.h"
#include "../shared/debugutil_p.h"

Q_DECLARE_METATYPE(QDeclarativeDebugWatch::State)


class tst_QDeclarativeDebug : public QObject
{
    Q_OBJECT

private:
    QDeclarativeDebugObjectReference findRootObject(int context = 0);
    QDeclarativeDebugPropertyReference findProperty(const QList<QDeclarativeDebugPropertyReference> &props, const QString &name) const;
    void waitForQuery(QDeclarativeDebugQuery *query);

    void recursiveObjectTest(QObject *o, const QDeclarativeDebugObjectReference &oref, bool recursive) const;

    void recursiveCompareObjects(const QDeclarativeDebugObjectReference &a, const QDeclarativeDebugObjectReference &b) const;
    void recursiveCompareContexts(const QDeclarativeDebugContextReference &a, const QDeclarativeDebugContextReference &b) const;
    void compareProperties(const QDeclarativeDebugPropertyReference &a, const QDeclarativeDebugPropertyReference &b) const;
    
    QDeclarativeDebugConnection *m_conn;
    QDeclarativeEngineDebug *m_dbg;
    QDeclarativeEngine *m_engine;
    QDeclarativeItem *m_rootItem;

    QObjectList m_components;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void watch_property();
    void watch_object();
    void watch_expression();
    void watch_expression_data();
    void watch_context();
    void watch_file();

    void queryAvailableEngines();
    void queryRootContexts();
    void queryObject();
    void queryObject_data();
    void queryExpressionResult();
    void queryExpressionResult_data();

    void tst_QDeclarativeDebugFileReference();
    void tst_QDeclarativeDebugEngineReference();
    void tst_QDeclarativeDebugObjectReference();
    void tst_QDeclarativeDebugContextReference();
    void tst_QDeclarativeDebugPropertyReference();

    void setMethodBody();
};

QDeclarativeDebugObjectReference tst_QDeclarativeDebug::findRootObject(int context)
{
    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    if (q_engines->engines().count() == 0)
        return QDeclarativeDebugObjectReference();
    QDeclarativeDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    if (q_context->rootContext().objects().count() == 0)
        return QDeclarativeDebugObjectReference();
    QDeclarativeDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[context], this);
    waitForQuery(q_obj);

    QDeclarativeDebugObjectReference result = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    return result;
}

QDeclarativeDebugPropertyReference tst_QDeclarativeDebug::findProperty(const QList<QDeclarativeDebugPropertyReference> &props, const QString &name) const
{
    foreach(const QDeclarativeDebugPropertyReference &p, props) {
        if (p.name() == name)
            return p;
    }
    return QDeclarativeDebugPropertyReference();
}

void tst_QDeclarativeDebug::waitForQuery(QDeclarativeDebugQuery *query)
{
    QVERIFY(query);
    QCOMPARE(query->parent(), qobject_cast<QObject*>(this));
    QVERIFY(query->state() == QDeclarativeDebugQuery::Waiting);
    if (!QDeclarativeDebugTest::waitForSignal(query, SIGNAL(stateChanged(QDeclarativeDebugQuery::State))))
        QFAIL("query timed out");
}

void tst_QDeclarativeDebug::recursiveObjectTest(QObject *o, const QDeclarativeDebugObjectReference &oref, bool recursive) const
{
    const QMetaObject *meta = o->metaObject();

    QDeclarativeType *type = QDeclarativeMetaType::qmlType(o->metaObject());
    QString className = type ? type->qmlTypeName() : QString();
    className = className.mid(className.lastIndexOf(QLatin1Char('/'))+1);

    QCOMPARE(oref.debugId(), QDeclarativeDebugService::idForObject(o));
    QCOMPARE(oref.name(), o->objectName());
    QCOMPARE(oref.className(), className);
    QCOMPARE(oref.contextDebugId(), QDeclarativeDebugService::idForObject(qmlContext(o)));

    const QObjectList &children = o->children();
    for (int i=0; i<children.count(); i++) {
        QObject *child = children[i];
        if (!qmlContext(child))
            continue;
        int debugId = QDeclarativeDebugService::idForObject(child);
        QVERIFY(debugId >= 0);

        QDeclarativeDebugObjectReference cref;
        foreach (const QDeclarativeDebugObjectReference &ref, oref.children()) {
            if (ref.debugId() == debugId) {
                cref = ref;
                break;
            }
        }
        QVERIFY(cref.debugId() >= 0);

        if (recursive)
            recursiveObjectTest(child, cref, true);
    }

    foreach (const QDeclarativeDebugPropertyReference &p, oref.properties()) {
        QCOMPARE(p.objectDebugId(), QDeclarativeDebugService::idForObject(o));

        // signal properties are fake - they are generated from QDeclarativeBoundSignal children
        if (p.name().startsWith("on") && p.name().length() > 2 && p.name()[2].isUpper()) {
            QVERIFY(p.value().toString().startsWith('{') && p.value().toString().endsWith('}'));
            QVERIFY(p.valueTypeName().isEmpty());
            QVERIFY(p.binding().isEmpty());
            QVERIFY(!p.hasNotifySignal());
            continue;
        }

        QMetaProperty pmeta = meta->property(meta->indexOfProperty(p.name().toUtf8().constData()));

        QCOMPARE(p.name(), QString::fromUtf8(pmeta.name()));

        if (pmeta.type() < QVariant::UserType) // TODO test complex types
            QCOMPARE(p.value(), pmeta.read(o));

        if (p.name() == "parent")
            QVERIFY(p.valueTypeName() == "QGraphicsObject*" || p.valueTypeName() == "QDeclarativeItem*");
        else
            QCOMPARE(p.valueTypeName(), QString::fromUtf8(pmeta.typeName()));

        QDeclarativeAbstractBinding *binding = 
            QDeclarativePropertyPrivate::binding(QDeclarativeProperty(o, p.name()));
        if (binding)
            QCOMPARE(binding->expression(), p.binding());

        QCOMPARE(p.hasNotifySignal(), pmeta.hasNotifySignal());

        QVERIFY(pmeta.isValid());
    }
}

void tst_QDeclarativeDebug::recursiveCompareObjects(const QDeclarativeDebugObjectReference &a, const QDeclarativeDebugObjectReference &b) const
{
    QCOMPARE(a.debugId(), b.debugId());
    QCOMPARE(a.className(), b.className());
    QCOMPARE(a.name(), b.name());
    QCOMPARE(a.contextDebugId(), b.contextDebugId());

    QCOMPARE(a.source().url(), b.source().url());
    QCOMPARE(a.source().lineNumber(), b.source().lineNumber());
    QCOMPARE(a.source().columnNumber(), b.source().columnNumber());

    QCOMPARE(a.properties().count(), b.properties().count());
    QCOMPARE(a.children().count(), b.children().count());

    QList<QDeclarativeDebugPropertyReference> aprops = a.properties();
    QList<QDeclarativeDebugPropertyReference> bprops = b.properties();

    for (int i=0; i<aprops.count(); i++)
        compareProperties(aprops[i], bprops[i]);

    for (int i=0; i<a.children().count(); i++)
        recursiveCompareObjects(a.children()[i], b.children()[i]);
}

void tst_QDeclarativeDebug::recursiveCompareContexts(const QDeclarativeDebugContextReference &a, const QDeclarativeDebugContextReference &b) const
{
    QCOMPARE(a.debugId(), b.debugId());
    QCOMPARE(a.name(), b.name());
    QCOMPARE(a.objects().count(), b.objects().count());
    QCOMPARE(a.contexts().count(), b.contexts().count());

    for (int i=0; i<a.objects().count(); i++)
        recursiveCompareObjects(a.objects()[i], b.objects()[i]);

    for (int i=0; i<a.contexts().count(); i++)
        recursiveCompareContexts(a.contexts()[i], b.contexts()[i]);
}

void tst_QDeclarativeDebug::compareProperties(const QDeclarativeDebugPropertyReference &a, const QDeclarativeDebugPropertyReference &b) const
{
    QCOMPARE(a.objectDebugId(), b.objectDebugId());
    QCOMPARE(a.name(), b.name());
    QCOMPARE(a.value(), b.value());
    QCOMPARE(a.valueTypeName(), b.valueTypeName());
    QCOMPARE(a.binding(), b.binding());
    QCOMPARE(a.hasNotifySignal(), b.hasNotifySignal());
}

void tst_QDeclarativeDebug::initTestCase()
{
    qRegisterMetaType<QDeclarativeDebugWatch::State>();

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Waiting for connection on port 3768...");
    qputenv("QML_DEBUG_SERVER_PORT", "3768");
    m_engine = new QDeclarativeEngine(this);

    QList<QByteArray> qml;
    qml << "import Qt 4.7\n"
            "Item {"
                "width: 10; height: 20; scale: blueRect.scale;"
                "Rectangle { id: blueRect; width: 500; height: 600; color: \"blue\"; }"
                "Text { color: blueRect.color; }"
                "MouseArea {"
                    "onEntered: { console.log('hello') }"
                "}"
            "}";

    // add second component to test multiple root contexts
    qml << "import Qt 4.7\n"
            "Item {}";

    // and a third to test methods
    qml << "import Qt 4.7\n"
            "Item {"
                "function myMethodNoArgs() { return 3; }\n"
                "function myMethod(a) { return a + 9; }\n"
                "function myMethodIndirect() { myMethod(3); }\n"
            "}";

    for (int i=0; i<qml.count(); i++) {
        QDeclarativeComponent component(m_engine);
        component.setData(qml[i], QUrl::fromLocalFile(""));
        Q_ASSERT(component.isReady());  // fails if bad syntax
        m_components << qobject_cast<QDeclarativeItem*>(component.create());
    }
    m_rootItem = qobject_cast<QDeclarativeItem*>(m_components.first());

    // add an extra context to test for multiple contexts
    QDeclarativeContext *context = new QDeclarativeContext(m_engine->rootContext(), this);
    context->setObjectName("tst_QDeclarativeDebug_childContext");

    m_conn = new QDeclarativeDebugConnection(this);
    m_conn->connectToHost("127.0.0.1", 3768);

    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeDebugServer: Connection established");
    bool ok = m_conn->waitForConnected();
    Q_ASSERT(ok);
    QTRY_VERIFY(QDeclarativeDebugService::hasDebuggingClient());

    m_dbg = new QDeclarativeEngineDebug(m_conn, this);
}

void tst_QDeclarativeDebug::cleanupTestCase()
{
    qDeleteAll(m_components);
}

void tst_QDeclarativeDebug::setMethodBody()
{
    QDeclarativeDebugObjectReference obj = findRootObject(2);

    QObject *root = m_components.at(2);
    // Without args
    {
    QVariant rv;
    QVERIFY(QMetaObject::invokeMethod(root, "myMethodNoArgs", Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, rv)));
    QVERIFY(rv == QVariant(qreal(3)));


    QVERIFY(m_dbg->setMethodBody(obj.debugId(), "myMethodNoArgs", "return 7"));
    QTest::qWait(100);

    QVERIFY(QMetaObject::invokeMethod(root, "myMethodNoArgs", Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, rv)));
    QVERIFY(rv == QVariant(qreal(7)));
    }

    // With args
    {
    QVariant rv;
    QVERIFY(QMetaObject::invokeMethod(root, "myMethod", Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, rv), Q_ARG(QVariant, QVariant(19))));
    QVERIFY(rv == QVariant(qreal(28)));

    QVERIFY(m_dbg->setMethodBody(obj.debugId(), "myMethod", "return a + 7"));
    QTest::qWait(100);

    QVERIFY(QMetaObject::invokeMethod(root, "myMethod", Qt::DirectConnection,
                                      Q_RETURN_ARG(QVariant, rv), Q_ARG(QVariant, QVariant(19))));
    QVERIFY(rv == QVariant(qreal(26)));
    }
}

void tst_QDeclarativeDebug::watch_property()
{
    QDeclarativeDebugObjectReference obj = findRootObject();
    QDeclarativeDebugPropertyReference prop = findProperty(obj.properties(), "width");

    QDeclarativeDebugPropertyWatch *watch;
    
    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    watch = unconnected->addWatch(prop, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Dead);
    delete watch;
    delete unconnected;

    watch = m_dbg->addWatch(QDeclarativeDebugPropertyReference(), this);
    QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Inactive);
    delete watch;
    
    watch = m_dbg->addWatch(prop, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->name(), prop.name());

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));

    int origWidth = m_rootItem->property("width").toInt();
    m_rootItem->setProperty("width", origWidth*2);

    // stateChanged() is received before valueChanged()
    QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Active);
    QCOMPARE(spy.count(), 1);

    m_dbg->removeWatch(watch);
    delete watch;

    // restore original value and verify spy doesn't get additional signal since watch has been removed
    m_rootItem->setProperty("width", origWidth);
    QTest::qWait(100);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(spy.at(0).at(0).value<QByteArray>(), prop.name().toUtf8());
    QCOMPARE(spy.at(0).at(1).value<QVariant>(), qVariantFromValue(origWidth*2));
}

void tst_QDeclarativeDebug::watch_object()
{
    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    Q_ASSERT(q_engines->engines().count() > 0);
    QDeclarativeDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    Q_ASSERT(q_context->rootContext().objects().count() > 0);
    QDeclarativeDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QDeclarativeDebugObjectReference obj = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    QDeclarativeDebugWatch *watch;

    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    watch = unconnected->addWatch(obj, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Dead);
    delete watch;
    delete unconnected;
    
    watch = m_dbg->addWatch(QDeclarativeDebugObjectReference(), this);
    QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Inactive);
    delete watch;

    watch = m_dbg->addWatch(obj, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));

    int origWidth = m_rootItem->property("width").toInt();
    int origHeight = m_rootItem->property("height").toInt();
    m_rootItem->setProperty("width", origWidth*2);
    m_rootItem->setProperty("height", origHeight*2);

    // stateChanged() is received before any valueChanged() signals
    QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Active);
    QVERIFY(spy.count() > 0);

    int newWidth = -1;
    int newHeight = -1;
    for (int i=0; i<spy.count(); i++) {
        const QVariantList &values = spy[i];
        if (values[0].value<QByteArray>() == "width")
            newWidth = values[1].value<QVariant>().toInt();
        else if (values[0].value<QByteArray>() == "height")
            newHeight = values[1].value<QVariant>().toInt();

    }

    m_dbg->removeWatch(watch);
    delete watch;

    // since watch has been removed, restoring the original values should not trigger a valueChanged()
    spy.clear();
    m_rootItem->setProperty("width", origWidth);
    m_rootItem->setProperty("height", origHeight);
    QTest::qWait(100);
    QCOMPARE(spy.count(), 0);

    QCOMPARE(newWidth, origWidth * 2);
    QCOMPARE(newHeight, origHeight * 2);
}

void tst_QDeclarativeDebug::watch_expression()
{
    QFETCH(QString, expr);
    QFETCH(int, increment);
    QFETCH(int, incrementCount);

    int origWidth = m_rootItem->property("width").toInt();
    
    QDeclarativeDebugObjectReference obj = findRootObject();

    QDeclarativeDebugObjectExpressionWatch *watch;

    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    watch = unconnected->addWatch(obj, expr, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Dead);
    delete watch;
    delete unconnected;
    
    watch = m_dbg->addWatch(QDeclarativeDebugObjectReference(), expr, this);
    QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Inactive);
    delete watch;
    
    watch = m_dbg->addWatch(obj, expr, this);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->expression(), expr);

    QSignalSpy spyState(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State)));

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));
    int expectedSpyCount = incrementCount + 1;  // should also get signal with expression's initial value

    int width = origWidth;
    for (int i=0; i<incrementCount+1; i++) {
        if (i > 0) {
            width += increment;
            m_rootItem->setProperty("width", width);
        }
        if (!QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(valueChanged(QByteArray,QVariant))))
            QFAIL("Did not receive valueChanged() for expression");
    }

    if (spyState.count() == 0)
        QVERIFY(QDeclarativeDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QDeclarativeDebugWatch::State))));
    QCOMPARE(spyState.count(), 1);
    QCOMPARE(watch->state(), QDeclarativeDebugWatch::Active);

    m_dbg->removeWatch(watch);
    delete watch;

    // restore original value and verify spy doesn't get a signal since watch has been removed
    m_rootItem->setProperty("width", origWidth); 
    QTest::qWait(100);
    QCOMPARE(spy.count(), expectedSpyCount);

    width = origWidth + increment;
    for (int i=0; i<spy.count(); i++) {
        QCOMPARE(spy.at(i).at(1).value<QVariant>().toInt(), width);
        width += increment;
    }
}

void tst_QDeclarativeDebug::watch_expression_data()
{
    QTest::addColumn<QString>("expr");
    QTest::addColumn<int>("increment");
    QTest::addColumn<int>("incrementCount");

    QTest::newRow("width") << "width" << 0 << 0;
    QTest::newRow("width+10") << "width + 10" << 10 << 5;
}

void tst_QDeclarativeDebug::watch_context()
{
    QDeclarativeDebugContextReference c;
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeEngineDebug::addWatch(): Not implemented");
    QVERIFY(!m_dbg->addWatch(c, QString(), this));
}

void tst_QDeclarativeDebug::watch_file()
{
    QDeclarativeDebugFileReference f;
    QTest::ignoreMessage(QtWarningMsg, "QDeclarativeEngineDebug::addWatch(): Not implemented");
    QVERIFY(!m_dbg->addWatch(f, this));
}

void tst_QDeclarativeDebug::queryAvailableEngines()
{
    QDeclarativeDebugEnginesQuery *q_engines;

    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    q_engines = unconnected->queryAvailableEngines(0);
    QCOMPARE(q_engines->state(), QDeclarativeDebugQuery::Error);
    delete q_engines;
    delete unconnected;

    q_engines = m_dbg->queryAvailableEngines(this);
    delete q_engines;

    q_engines = m_dbg->queryAvailableEngines(this);
    QVERIFY(q_engines->engines().isEmpty());
    waitForQuery(q_engines);

    // TODO test multiple engines
    QList<QDeclarativeDebugEngineReference> engines = q_engines->engines();
    QCOMPARE(engines.count(), 1);

    foreach(const QDeclarativeDebugEngineReference &e, engines) {
        QCOMPARE(e.debugId(), QDeclarativeDebugService::idForObject(m_engine));
        QCOMPARE(e.name(), m_engine->objectName());
    }

    delete q_engines;
}

void tst_QDeclarativeDebug::queryRootContexts()
{
    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    int engineId = q_engines->engines()[0].debugId();

    QDeclarativeDebugRootContextQuery *q_context;
    
    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    q_context = unconnected->queryRootContexts(engineId, this);
    QCOMPARE(q_context->state(), QDeclarativeDebugQuery::Error);
    delete q_context;
    delete unconnected;

    q_context = m_dbg->queryRootContexts(engineId, this);
    delete q_context;

    q_context = m_dbg->queryRootContexts(engineId, this);
    waitForQuery(q_context);

    QDeclarativeContext *actualContext = m_engine->rootContext();
    QDeclarativeDebugContextReference context = q_context->rootContext();
    QCOMPARE(context.debugId(), QDeclarativeDebugService::idForObject(actualContext));
    QCOMPARE(context.name(), actualContext->objectName());

    QCOMPARE(context.objects().count(), 3); // 3 qml component objects created for context in main()

    // root context query sends only root object data - it doesn't fill in
    // the children or property info
    QCOMPARE(context.objects()[0].properties().count(), 0);
    QCOMPARE(context.objects()[0].children().count(), 0);

    QCOMPARE(context.contexts().count(), 1);
    QVERIFY(context.contexts()[0].debugId() >= 0);
    QCOMPARE(context.contexts()[0].name(), QString("tst_QDeclarativeDebug_childContext"));

    delete q_engines;
    delete q_context;
}

void tst_QDeclarativeDebug::queryObject()
{
    QFETCH(bool, recursive);

    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QDeclarativeDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);
    QDeclarativeDebugObjectReference rootObject = q_context->rootContext().objects()[0];

    QDeclarativeDebugObjectQuery *q_obj = 0;

    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    q_obj = recursive ? unconnected->queryObjectRecursive(rootObject, this) : unconnected->queryObject(rootObject, this);
    QCOMPARE(q_obj->state(), QDeclarativeDebugQuery::Error);
    delete q_obj;
    delete unconnected;

    q_obj = recursive ? m_dbg->queryObjectRecursive(rootObject, this) : m_dbg->queryObject(rootObject, this);
    delete q_obj;

    q_obj = recursive ? m_dbg->queryObjectRecursive(rootObject, this) : m_dbg->queryObject(rootObject, this);
    waitForQuery(q_obj);

    QDeclarativeDebugObjectReference obj = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    // check source as defined in main()
    QDeclarativeDebugFileReference source = obj.source();
    QCOMPARE(source.url(), QUrl::fromLocalFile(""));
    QCOMPARE(source.lineNumber(), 2);
    QCOMPARE(source.columnNumber(), 1);

    // generically test all properties, children and childrens' properties
    recursiveObjectTest(m_rootItem, obj, recursive);

    if (recursive) {
        foreach(const QDeclarativeDebugObjectReference &child, obj.children())
            QVERIFY(child.properties().count() > 0);

        QDeclarativeDebugObjectReference rect;
        QDeclarativeDebugObjectReference text;
        foreach (const QDeclarativeDebugObjectReference &child, obj.children()) {
            if (child.className() == "Rectangle")
                rect = child;
            else if (child.className() == "Text")
                text = child;
        }

        // test specific property values
        QCOMPARE(findProperty(rect.properties(), "width").value(), qVariantFromValue(500));
        QCOMPARE(findProperty(rect.properties(), "height").value(), qVariantFromValue(600));
        QCOMPARE(findProperty(rect.properties(), "color").value(), qVariantFromValue(QColor("blue")));

        QCOMPARE(findProperty(text.properties(), "color").value(), qVariantFromValue(QColor("blue")));

    } else {
        foreach(const QDeclarativeDebugObjectReference &child, obj.children())
            QCOMPARE(child.properties().count(), 0);
    }
}

void tst_QDeclarativeDebug::queryObject_data()
{
    QTest::addColumn<bool>("recursive");

    QTest::newRow("non-recursive") << false;
    QTest::newRow("recursive") << true;
}

void tst_QDeclarativeDebug::queryExpressionResult()
{
    QFETCH(QString, expr);
    QFETCH(QVariant, result);

    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);    // check immediate deletion is ok
    
    QDeclarativeDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);
    int objectId = q_context->rootContext().objects()[0].debugId();

    QDeclarativeDebugExpressionQuery *q_expr;

    QDeclarativeEngineDebug *unconnected = new QDeclarativeEngineDebug(0);
    q_expr = unconnected->queryExpressionResult(objectId, expr, this);
    QCOMPARE(q_expr->state(), QDeclarativeDebugQuery::Error);
    delete q_expr;
    delete unconnected;
    
    q_expr = m_dbg->queryExpressionResult(objectId, expr, this);
    delete q_expr;

    q_expr = m_dbg->queryExpressionResult(objectId, expr, this);
    QCOMPARE(q_expr->expression().toString(), expr);
    waitForQuery(q_expr);

    QCOMPARE(q_expr->result(), result);

    delete q_engines;
    delete q_context;
    delete q_expr;
}

void tst_QDeclarativeDebug::queryExpressionResult_data()
{
    QTest::addColumn<QString>("expr");
    QTest::addColumn<QVariant>("result");

    QTest::newRow("width + 50") << "width + 50" << qVariantFromValue(60);
    QTest::newRow("blueRect.width") << "blueRect.width" << qVariantFromValue(500);
    QTest::newRow("bad expr") << "aeaef" << qVariantFromValue(QString("<undefined>"));
}

void tst_QDeclarativeDebug::tst_QDeclarativeDebugFileReference()
{
    QDeclarativeDebugFileReference ref;
    QVERIFY(ref.url().isEmpty());
    QCOMPARE(ref.lineNumber(), -1);
    QCOMPARE(ref.columnNumber(), -1);

    ref.setUrl(QUrl("http://test"));
    QCOMPARE(ref.url(), QUrl("http://test"));
    ref.setLineNumber(1);
    QCOMPARE(ref.lineNumber(), 1);
    ref.setColumnNumber(1);
    QCOMPARE(ref.columnNumber(), 1);

    QDeclarativeDebugFileReference copy(ref);
    QDeclarativeDebugFileReference copyAssign;
    copyAssign = ref;
    foreach (const QDeclarativeDebugFileReference &r, (QList<QDeclarativeDebugFileReference>() << copy << copyAssign)) {
        QCOMPARE(r.url(), ref.url());
        QCOMPARE(r.lineNumber(), ref.lineNumber());
        QCOMPARE(r.columnNumber(), ref.columnNumber());
    }
}

void tst_QDeclarativeDebug::tst_QDeclarativeDebugEngineReference()
{
    QDeclarativeDebugEngineReference ref;
    QCOMPARE(ref.debugId(), -1);
    QVERIFY(ref.name().isEmpty());

    ref = QDeclarativeDebugEngineReference(1);
    QCOMPARE(ref.debugId(), 1);
    QVERIFY(ref.name().isEmpty());

    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    ref = q_engines->engines()[0];
    delete q_engines;

    QDeclarativeDebugEngineReference copy(ref);
    QDeclarativeDebugEngineReference copyAssign;
    copyAssign = ref;
    foreach (const QDeclarativeDebugEngineReference &r, (QList<QDeclarativeDebugEngineReference>() << copy << copyAssign)) {
        QCOMPARE(r.debugId(), ref.debugId());
        QCOMPARE(r.name(), ref.name());
    }
}

void tst_QDeclarativeDebug::tst_QDeclarativeDebugObjectReference()
{
    QDeclarativeDebugObjectReference ref;
    QCOMPARE(ref.debugId(), -1);
    QCOMPARE(ref.className(), QString());
    QCOMPARE(ref.name(), QString());
    QCOMPARE(ref.contextDebugId(), -1);
    QVERIFY(ref.properties().isEmpty());
    QVERIFY(ref.children().isEmpty());

    QDeclarativeDebugFileReference source = ref.source();
    QVERIFY(source.url().isEmpty());
    QVERIFY(source.lineNumber() < 0);
    QVERIFY(source.columnNumber() < 0);

    ref = QDeclarativeDebugObjectReference(1);
    QCOMPARE(ref.debugId(), 1);

    QDeclarativeDebugObjectReference rootObject = findRootObject();
    QDeclarativeDebugObjectQuery *query = m_dbg->queryObjectRecursive(rootObject, this);
    waitForQuery(query);
    ref = query->object();
    delete query;

    QVERIFY(ref.debugId() >= 0);

    QDeclarativeDebugObjectReference copy(ref);
    QDeclarativeDebugObjectReference copyAssign;
    copyAssign = ref;
    foreach (const QDeclarativeDebugObjectReference &r, (QList<QDeclarativeDebugObjectReference>() << copy << copyAssign))
        recursiveCompareObjects(r, ref);
}

void tst_QDeclarativeDebug::tst_QDeclarativeDebugContextReference()
{
    QDeclarativeDebugContextReference ref;
    QCOMPARE(ref.debugId(), -1);
    QVERIFY(ref.name().isEmpty());
    QVERIFY(ref.objects().isEmpty());
    QVERIFY(ref.contexts().isEmpty());

    QDeclarativeDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    QDeclarativeDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    ref = q_context->rootContext();
    delete q_engines;
    delete q_context;
    QVERIFY(ref.debugId() >= 0);

    QDeclarativeDebugContextReference copy(ref);
    QDeclarativeDebugContextReference copyAssign;
    copyAssign = ref;
    foreach (const QDeclarativeDebugContextReference &r, (QList<QDeclarativeDebugContextReference>() << copy << copyAssign))
        recursiveCompareContexts(r, ref);
}

void tst_QDeclarativeDebug::tst_QDeclarativeDebugPropertyReference()
{
    QDeclarativeDebugObjectReference rootObject = findRootObject();
    QDeclarativeDebugObjectQuery *query = m_dbg->queryObject(rootObject, this);
    waitForQuery(query);
    QDeclarativeDebugObjectReference obj = query->object();
    delete query;   

    QDeclarativeDebugPropertyReference ref = findProperty(obj.properties(), "scale");
    QVERIFY(ref.objectDebugId() > 0);
    QVERIFY(!ref.name().isEmpty());
    QVERIFY(!ref.value().isNull());
    QVERIFY(!ref.valueTypeName().isEmpty());
    QVERIFY(!ref.binding().isEmpty());
    QVERIFY(ref.hasNotifySignal());
  
    QDeclarativeDebugPropertyReference copy(ref);
    QDeclarativeDebugPropertyReference copyAssign;
    copyAssign = ref;
    foreach (const QDeclarativeDebugPropertyReference &r, (QList<QDeclarativeDebugPropertyReference>() << copy << copyAssign))
        compareProperties(r, ref);
}

QTEST_MAIN(tst_QDeclarativeDebug)

#include "tst_qdeclarativedebug.moc"
