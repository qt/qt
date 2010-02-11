/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QSignalSpy>
#include <QTimer>
#include <QHostAddress>
#include <QDebug>
#include <QThread>

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlexpression.h>
#include <QtDeclarative/qmlmetatype.h>
#include <QtDeclarative/qmlmetaproperty.h>
#include <QtDeclarative/qmlbinding.h>

#include <private/qmldebug_p.h>
#include <private/qmlenginedebug_p.h>
#include <private/qmldebugclient_p.h>
#include <private/qmldebugservice_p.h>
#include <private/qmlgraphicsrectangle_p.h>

#include "../shared/debugutil_p.h"

Q_DECLARE_METATYPE(QmlDebugWatch::State)


class tst_QmlDebug : public QObject
{
    Q_OBJECT

public:
    tst_QmlDebug(QmlDebugTestData *data)
    {
        m_conn = data->conn;
        m_engine = data->engine;
        m_rootItem = data->items[0];
    }

private:
    QmlDebugObjectReference findRootObject();
    QmlDebugPropertyReference findProperty(const QList<QmlDebugPropertyReference> &props, const QString &name) const;
    void waitForQuery(QmlDebugQuery *query);

    void recursiveObjectTest(QObject *o, const QmlDebugObjectReference &oref, bool recursive) const;

    void recursiveCompareObjects(const QmlDebugObjectReference &a, const QmlDebugObjectReference &b) const;
    void recursiveCompareContexts(const QmlDebugContextReference &a, const QmlDebugContextReference &b) const;
    void compareProperties(const QmlDebugPropertyReference &a, const QmlDebugPropertyReference &b) const;
    
    QmlDebugConnection *m_conn;
    QmlEngineDebug *m_dbg;
    QmlEngine *m_engine;
    QmlGraphicsItem *m_rootItem;

private slots:
    void initTestCase();

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

    void tst_QmlDebugFileReference();
    void tst_QmlDebugEngineReference();
    void tst_QmlDebugObjectReference();
    void tst_QmlDebugContextReference();
    void tst_QmlDebugPropertyReference();
};

QmlDebugObjectReference tst_QmlDebug::findRootObject()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    if (q_engines->engines().count() == 0)
        return QmlDebugObjectReference();
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    if (q_context->rootContext().objects().count() == 0)
        return QmlDebugObjectReference();
    QmlDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QmlDebugObjectReference result = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    return result;
}

QmlDebugPropertyReference tst_QmlDebug::findProperty(const QList<QmlDebugPropertyReference> &props, const QString &name) const
{
    foreach(const QmlDebugPropertyReference &p, props) {
        if (p.name() == name)
            return p;
    }
    return QmlDebugPropertyReference();
}

void tst_QmlDebug::waitForQuery(QmlDebugQuery *query)
{
    QVERIFY(query);
    QCOMPARE(query->parent(), this);
    QVERIFY(query->state() == QmlDebugQuery::Waiting);
    if (!QmlDebugTest::waitForSignal(query, SIGNAL(stateChanged(QmlDebugQuery::State))))
        QFAIL("query timed out");
}

void tst_QmlDebug::recursiveObjectTest(QObject *o, const QmlDebugObjectReference &oref, bool recursive) const
{
    const QMetaObject *meta = o->metaObject();

    QmlType *type = QmlMetaType::qmlType(o->metaObject());
    QString className = type ? type->qmlTypeName() : QString();
    className = className.mid(className.lastIndexOf(QLatin1Char('/'))+1);

    QCOMPARE(oref.debugId(), QmlDebugService::idForObject(o));
    QCOMPARE(oref.name(), o->objectName());
    QCOMPARE(oref.className(), className);
    QCOMPARE(oref.contextDebugId(), QmlDebugService::idForObject(qmlContext(o)));

    const QObjectList &children = o->children();
    for (int i=0; i<children.count(); i++) {
        QObject *child = children[i];
        if (!qmlContext(child))
            continue;
        int debugId = QmlDebugService::idForObject(child);
        QVERIFY(debugId >= 0);

        QmlDebugObjectReference cref;
        foreach (const QmlDebugObjectReference &ref, oref.children()) {
            if (ref.debugId() == debugId) {
                cref = ref;
                break;
            }
        }
        QVERIFY(cref.debugId() >= 0);

        if (recursive)
            recursiveObjectTest(child, cref, true);
    }

    foreach (const QmlDebugPropertyReference &p, oref.properties()) {
        QCOMPARE(p.objectDebugId(), QmlDebugService::idForObject(o));

        // signal properties are fake - they are generated from QmlBoundSignal children
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
            QVERIFY(p.valueTypeName() == "QGraphicsObject*" || p.valueTypeName() == "QmlGraphicsItem*");
        else
            QCOMPARE(p.valueTypeName(), QString::fromUtf8(pmeta.typeName()));

        QmlAbstractBinding *binding = QmlMetaProperty(o, p.name()).binding();
        if (binding)
            QCOMPARE(binding->expression(), p.binding());

        QCOMPARE(p.hasNotifySignal(), pmeta.hasNotifySignal());

        QVERIFY(pmeta.isValid());
    }
}

void tst_QmlDebug::recursiveCompareObjects(const QmlDebugObjectReference &a, const QmlDebugObjectReference &b) const
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

    QList<QmlDebugPropertyReference> aprops = a.properties();
    QList<QmlDebugPropertyReference> bprops = b.properties();

    for (int i=0; i<aprops.count(); i++)
        compareProperties(aprops[i], bprops[i]);

    for (int i=0; i<a.children().count(); i++)
        recursiveCompareObjects(a.children()[i], b.children()[i]);
}

void tst_QmlDebug::recursiveCompareContexts(const QmlDebugContextReference &a, const QmlDebugContextReference &b) const
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

void tst_QmlDebug::compareProperties(const QmlDebugPropertyReference &a, const QmlDebugPropertyReference &b) const
{
    QCOMPARE(a.objectDebugId(), b.objectDebugId());
    QCOMPARE(a.name(), b.name());
    QCOMPARE(a.value(), b.value());
    QCOMPARE(a.valueTypeName(), b.valueTypeName());
    QCOMPARE(a.binding(), b.binding());
    QCOMPARE(a.hasNotifySignal(), b.hasNotifySignal());
}

void tst_QmlDebug::initTestCase()
{
    m_dbg = new QmlEngineDebug(m_conn, this);

    qRegisterMetaType<QmlDebugWatch::State>();
}

void tst_QmlDebug::watch_property()
{
    QmlDebugObjectReference obj = findRootObject();
    QmlDebugPropertyReference prop = findProperty(obj.properties(), "width");

    QmlDebugPropertyWatch *watch;
    
    QmlEngineDebug unconnected(0);
    watch = unconnected.addWatch(prop, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Dead);
    delete watch;

    watch = m_dbg->addWatch(QmlDebugPropertyReference(), this);
    QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(watch->state(), QmlDebugWatch::Inactive);
    delete watch;
    
    watch = m_dbg->addWatch(prop, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->name(), prop.name());

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));

    int origWidth = m_rootItem->property("width").toInt();
    m_rootItem->setProperty("width", origWidth*2);

    // stateChanged() is received before valueChanged()
    QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(watch->state(), QmlDebugWatch::Active);
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

void tst_QmlDebug::watch_object()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    Q_ASSERT(q_engines->engines().count() > 0);
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    Q_ASSERT(q_context->rootContext().objects().count() > 0);
    QmlDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QmlDebugObjectReference obj = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    QmlDebugWatch *watch;

    QmlEngineDebug unconnected(0);
    watch = unconnected.addWatch(obj, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Dead);
    delete watch;
    
    watch = m_dbg->addWatch(QmlDebugObjectReference(), this);
    QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(watch->state(), QmlDebugWatch::Inactive);
    delete watch;

    watch = m_dbg->addWatch(obj, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));

    int origWidth = m_rootItem->property("width").toInt();
    int origHeight = m_rootItem->property("height").toInt();
    m_rootItem->setProperty("width", origWidth*2);
    m_rootItem->setProperty("height", origHeight*2);

    // stateChanged() is received before any valueChanged() signals
    QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(watch->state(), QmlDebugWatch::Active);
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

void tst_QmlDebug::watch_expression()
{
    QFETCH(QString, expr);
    QFETCH(int, increment);
    QFETCH(int, incrementCount);

    int origWidth = m_rootItem->property("width").toInt();
    
    QmlDebugObjectReference obj = findRootObject();

    QmlDebugObjectExpressionWatch *watch;

    QmlEngineDebug unconnected(0);
    watch = unconnected.addWatch(obj, expr, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Dead);
    delete watch;
    
    watch = m_dbg->addWatch(QmlDebugObjectReference(), expr, this);
    QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(watch->state(), QmlDebugWatch::Inactive);
    delete watch;
    
    watch = m_dbg->addWatch(obj, expr, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->expression(), expr);

    QSignalSpy spyState(watch, SIGNAL(stateChanged(QmlDebugWatch::State)));

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));
    int expectedSpyCount = incrementCount + 1;  // should also get signal with expression's initial value

    int width = origWidth;
    for (int i=0; i<incrementCount+1; i++) {
        if (i > 0) {
            width += increment;
            m_rootItem->setProperty("width", width);
        }
        if (!QmlDebugTest::waitForSignal(watch, SIGNAL(valueChanged(QByteArray,QVariant))))
            QFAIL("Did not receive valueChanged() for expression");
    }

    if (spyState.count() == 0)
        QVERIFY(QmlDebugTest::waitForSignal(watch, SIGNAL(stateChanged(QmlDebugWatch::State))));
    QCOMPARE(spyState.count(), 1);
    QCOMPARE(watch->state(), QmlDebugWatch::Active);

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

void tst_QmlDebug::watch_expression_data()
{
    QTest::addColumn<QString>("expr");
    QTest::addColumn<int>("increment");
    QTest::addColumn<int>("incrementCount");

    QTest::newRow("width") << "width" << 0 << 0;
    QTest::newRow("width+10") << "width + 10" << 10 << 5;
}

void tst_QmlDebug::watch_context()
{
    QmlDebugContextReference c;
    QTest::ignoreMessage(QtWarningMsg, "QmlEngineDebug::addWatch(): Not implemented");
    QVERIFY(!m_dbg->addWatch(c, QString(), this));
}

void tst_QmlDebug::watch_file()
{
    QmlDebugFileReference f;
    QTest::ignoreMessage(QtWarningMsg, "QmlEngineDebug::addWatch(): Not implemented");
    QVERIFY(!m_dbg->addWatch(f, this));
}

void tst_QmlDebug::queryAvailableEngines()
{
    QmlDebugEnginesQuery *q_engines;

    QmlEngineDebug unconnected(0);
    q_engines = unconnected.queryAvailableEngines(0);
    QCOMPARE(q_engines->state(), QmlDebugQuery::Error);
    delete q_engines;

    q_engines = m_dbg->queryAvailableEngines(this);
    delete q_engines;

    q_engines = m_dbg->queryAvailableEngines(this);
    QVERIFY(q_engines->engines().isEmpty());
    waitForQuery(q_engines);

    // TODO test multiple engines
    QList<QmlDebugEngineReference> engines = q_engines->engines();
    QCOMPARE(engines.count(), 1);

    foreach(const QmlDebugEngineReference &e, engines) {
        QCOMPARE(e.debugId(), QmlDebugService::idForObject(m_engine));
        QCOMPARE(e.name(), m_engine->objectName());
    }

    delete q_engines;
}

void tst_QmlDebug::queryRootContexts()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    int engineId = q_engines->engines()[0].debugId();

    QmlDebugRootContextQuery *q_context;
    
    QmlEngineDebug unconnected(0);
    q_context = unconnected.queryRootContexts(engineId, this);
    QCOMPARE(q_context->state(), QmlDebugQuery::Error);
    delete q_context;

    q_context = m_dbg->queryRootContexts(engineId, this);
    delete q_context;

    q_context = m_dbg->queryRootContexts(engineId, this);
    waitForQuery(q_context);

    QmlContext *actualContext = m_engine->rootContext();
    QmlDebugContextReference context = q_context->rootContext();
    QCOMPARE(context.debugId(), QmlDebugService::idForObject(actualContext));
    QCOMPARE(context.name(), actualContext->objectName());

    QCOMPARE(context.objects().count(), 2); // 2 qml component objects created for context in main()

    // root context query sends only root object data - it doesn't fill in
    // the children or property info
    QCOMPARE(context.objects()[0].properties().count(), 0);
    QCOMPARE(context.objects()[0].children().count(), 0);

    QCOMPARE(context.contexts().count(), 1);
    QVERIFY(context.contexts()[0].debugId() >= 0);
    QCOMPARE(context.contexts()[0].name(), QString("tst_QmlDebug_childContext"));

    delete q_engines;
    delete q_context;
}

void tst_QmlDebug::queryObject()
{
    QFETCH(bool, recursive);

    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);
    QmlDebugObjectReference rootObject = q_context->rootContext().objects()[0];

    QmlDebugObjectQuery *q_obj = 0;

    QmlEngineDebug unconnected(0);
    q_obj = recursive ? unconnected.queryObjectRecursive(rootObject, this) : unconnected.queryObject(rootObject, this);
    QCOMPARE(q_obj->state(), QmlDebugQuery::Error);
    delete q_obj;

    q_obj = recursive ? m_dbg->queryObjectRecursive(rootObject, this) : m_dbg->queryObject(rootObject, this);
    delete q_obj;

    q_obj = recursive ? m_dbg->queryObjectRecursive(rootObject, this) : m_dbg->queryObject(rootObject, this);
    waitForQuery(q_obj);

    QmlDebugObjectReference obj = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    // check source as defined in main()
    QmlDebugFileReference source = obj.source();
    QCOMPARE(source.url(), QUrl::fromLocalFile(""));
    QCOMPARE(source.lineNumber(), 2);
    QCOMPARE(source.columnNumber(), 1);

    // generically test all properties, children and childrens' properties
    recursiveObjectTest(m_rootItem, obj, recursive);

    if (recursive) {
        foreach(const QmlDebugObjectReference &child, obj.children())
            QVERIFY(child.properties().count() > 0);

        QmlDebugObjectReference rect;
        QmlDebugObjectReference text;
        foreach (const QmlDebugObjectReference &child, obj.children()) {
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
        foreach(const QmlDebugObjectReference &child, obj.children())
            QCOMPARE(child.properties().count(), 0);
    }
}

void tst_QmlDebug::queryObject_data()
{
    QTest::addColumn<bool>("recursive");

    QTest::newRow("non-recursive") << false;
    QTest::newRow("recursive") << true;
}

void tst_QmlDebug::queryExpressionResult()
{
    QFETCH(QString, expr);
    QFETCH(QVariant, result);

    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);    // check immediate deletion is ok
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);
    int objectId = q_context->rootContext().objects()[0].debugId();

    QmlDebugExpressionQuery *q_expr;

    QmlEngineDebug unconnected(0);
    q_expr = unconnected.queryExpressionResult(objectId, expr, this);
    QCOMPARE(q_expr->state(), QmlDebugQuery::Error);
    delete q_expr;
    
    q_expr = m_dbg->queryExpressionResult(objectId, expr, this);
    delete q_expr;

    q_expr = m_dbg->queryExpressionResult(objectId, expr, this);
    QCOMPARE(q_expr->expression(), expr);
    waitForQuery(q_expr);

    QCOMPARE(q_expr->result(), result);

    delete q_engines;
    delete q_context;
    delete q_expr;
}

void tst_QmlDebug::queryExpressionResult_data()
{
    QTest::addColumn<QString>("expr");
    QTest::addColumn<QVariant>("result");

    QTest::newRow("width + 50") << "width + 50" << qVariantFromValue(60);
    QTest::newRow("blueRect.width") << "blueRect.width" << qVariantFromValue(500);
    QTest::newRow("bad expr") << "aeaef" << qVariantFromValue(QString("<undefined>"));
}

void tst_QmlDebug::tst_QmlDebugFileReference()
{
    QmlDebugFileReference ref;
    QVERIFY(ref.url().isEmpty());
    QCOMPARE(ref.lineNumber(), -1);
    QCOMPARE(ref.columnNumber(), -1);

    ref.setUrl(QUrl("http://test"));
    QCOMPARE(ref.url(), QUrl("http://test"));
    ref.setLineNumber(1);
    QCOMPARE(ref.lineNumber(), 1);
    ref.setColumnNumber(1);
    QCOMPARE(ref.columnNumber(), 1);

    QmlDebugFileReference copy(ref);
    QmlDebugFileReference copyAssign;
    copyAssign = ref;
    foreach (const QmlDebugFileReference &r, (QList<QmlDebugFileReference>() << copy << copyAssign)) {
        QCOMPARE(r.url(), ref.url());
        QCOMPARE(r.lineNumber(), ref.lineNumber());
        QCOMPARE(r.columnNumber(), ref.columnNumber());
    }
}

void tst_QmlDebug::tst_QmlDebugEngineReference()
{
    QmlDebugEngineReference ref;
    QCOMPARE(ref.debugId(), -1);
    QVERIFY(ref.name().isEmpty());

    ref = QmlDebugEngineReference(1);
    QCOMPARE(ref.debugId(), 1);
    QVERIFY(ref.name().isEmpty());

    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    ref = q_engines->engines()[0];
    delete q_engines;

    QmlDebugEngineReference copy(ref);
    QmlDebugEngineReference copyAssign;
    copyAssign = ref;
    foreach (const QmlDebugEngineReference &r, (QList<QmlDebugEngineReference>() << copy << copyAssign)) {
        QCOMPARE(r.debugId(), ref.debugId());
        QCOMPARE(r.name(), ref.name());
    }
}

void tst_QmlDebug::tst_QmlDebugObjectReference()
{
    QmlDebugObjectReference ref;
    QCOMPARE(ref.debugId(), -1);
    QCOMPARE(ref.className(), QString());
    QCOMPARE(ref.name(), QString());
    QCOMPARE(ref.contextDebugId(), -1);
    QVERIFY(ref.properties().isEmpty());
    QVERIFY(ref.children().isEmpty());

    QmlDebugFileReference source = ref.source();
    QVERIFY(source.url().isEmpty());
    QVERIFY(source.lineNumber() < 0);
    QVERIFY(source.columnNumber() < 0);

    ref = QmlDebugObjectReference(1);
    QCOMPARE(ref.debugId(), 1);

    QmlDebugObjectReference rootObject = findRootObject();
    QmlDebugObjectQuery *query = m_dbg->queryObjectRecursive(rootObject, this);
    waitForQuery(query);
    ref = query->object();
    delete query;

    QVERIFY(ref.debugId() >= 0);

    QmlDebugObjectReference copy(ref);
    QmlDebugObjectReference copyAssign;
    copyAssign = ref;
    foreach (const QmlDebugObjectReference &r, (QList<QmlDebugObjectReference>() << copy << copyAssign))
        recursiveCompareObjects(r, ref);
}

void tst_QmlDebug::tst_QmlDebugContextReference()
{
    QmlDebugContextReference ref;
    QCOMPARE(ref.debugId(), -1);
    QVERIFY(ref.name().isEmpty());
    QVERIFY(ref.objects().isEmpty());
    QVERIFY(ref.contexts().isEmpty());

    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    ref = q_context->rootContext();
    delete q_engines;
    delete q_context;
    QVERIFY(ref.debugId() >= 0);

    QmlDebugContextReference copy(ref);
    QmlDebugContextReference copyAssign;
    copyAssign = ref;
    foreach (const QmlDebugContextReference &r, (QList<QmlDebugContextReference>() << copy << copyAssign))
        recursiveCompareContexts(r, ref);
}

void tst_QmlDebug::tst_QmlDebugPropertyReference()
{
    QmlDebugObjectReference rootObject = findRootObject();
    QmlDebugObjectQuery *query = m_dbg->queryObject(rootObject, this);
    waitForQuery(query);
    QmlDebugObjectReference obj = query->object();
    delete query;   

    QmlDebugPropertyReference ref = findProperty(obj.properties(), "scale");
    QVERIFY(ref.objectDebugId() > 0);
    QVERIFY(!ref.name().isEmpty());
    QVERIFY(!ref.value().isNull());
    QVERIFY(!ref.valueTypeName().isEmpty());
    QVERIFY(!ref.binding().isEmpty());
    QVERIFY(ref.hasNotifySignal());
  
    QmlDebugPropertyReference copy(ref);
    QmlDebugPropertyReference copyAssign;
    copyAssign = ref;
    foreach (const QmlDebugPropertyReference &r, (QList<QmlDebugPropertyReference>() << copy << copyAssign))
        compareProperties(r, ref);
}


class tst_QmlDebug_Factory : public QmlTestFactory
{
public:
    QObject *createTest(QmlDebugTestData *data)
    {
        QmlContext *c = new QmlContext(data->engine->rootContext());
        c->setObjectName("tst_QmlDebug_childContext");
        return new tst_QmlDebug(data);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QList<QByteArray> qml;
    qml << "import Qt 4.6\n"
            "Item {"
                "width: 10; height: 20; scale: blueRect.scale;"
                "Rectangle { id: blueRect; width: 500; height: 600; color: \"blue\"; }"
                "Text { color: blueRect.color; }"
                "MouseRegion {"
                    "onEntered: { console.log('hello') }"
                "}"
            "}";
    // add second component to test multiple root contexts
    qml << "import Qt 4.6\n"
            "Item {}";
    tst_QmlDebug_Factory factory;
    return QmlDebugTest::runTests(&factory, qml);
}

//QTEST_MAIN(tst_QmlDebug)

#include "tst_qmldebug.moc"
