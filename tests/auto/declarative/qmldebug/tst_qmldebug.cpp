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
#include <QProcessEnvironment>
#include <QProcess>

#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlexpression.h>

#include <private/qmldebug_p.h>
#include <private/qmldebugclient_p.h>
#include <private/qmldebugservice_p.h>
#include <private/qmlgraphicsrectangle_p.h>
#include <private/qmlgraphicstext_p.h>


class tst_QmlDebug : public QObject
{
    Q_OBJECT

public:
    tst_QmlDebug(QmlDebugConnection *conn, QmlEngine *engine, QmlGraphicsItem *rootItem)
        : m_conn(conn), m_dbg(0), m_engine(engine), m_rootItem(rootItem) {}

protected slots:
    void saveValueChange(const QByteArray &ba, const QVariant &v)
    {
        m_savedValueChanges[ba] = v;
    }

private:
    QmlDebugConnection *m_conn;
    QmlEngineDebug *m_dbg;
    QmlEngine *m_engine;
    QmlGraphicsItem *m_rootItem;

    QHash<QByteArray, QVariant> m_savedValueChanges;

    void waitForQuery(QmlDebugQuery *query)
    {
        QCOMPARE(query->parent(), this);
        QEventLoop loop;
        QTimer timer;
        QVERIFY(query->state() == QmlDebugQuery::Waiting);
        connect(query, SIGNAL(stateChanged(State)), &loop, SLOT(quit()));
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start(5000);
        loop.exec();
        if (!timer.isActive())
            QFAIL("query timed out");
    }

    void verifyRootObject(const QmlDebugObjectReference &obj)
    {
        // verifies object according to component definition in main()

        QCOMPARE(obj.debugId(), QmlDebugService::idForObject(m_rootItem));
        QCOMPARE(obj.className(), QLatin1String("Item"));
        QCOMPARE(obj.name(), m_rootItem->objectName());
        QCOMPARE(obj.contextDebugId(), QmlDebugService::idForObject(qmlContext(m_rootItem)));

        QmlDebugFileReference source = obj.source();
        QCOMPARE(source.url(), QUrl("file://"));
        QCOMPARE(source.lineNumber(), 3);
        QCOMPARE(source.columnNumber(), 3);

        QList<QmlDebugPropertyReference> props = obj.properties();
        QHash<QString, QVariant> expected;

        expected["width"] = 10;
        expected["height"] = 20;
        verifyProperties(props, expected);
    }
    
    void verifyProperties(const QList<QmlDebugPropertyReference> &actual, const QHash<QString, QVariant> &expected)
    {
        foreach(const QmlDebugPropertyReference &p, actual) {
            if (expected.contains(p.name()))
                QCOMPARE(p.value(), expected[p.name()]);
        }
    }

    QmlDebugObjectReference findRootObject()
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

    QmlDebugPropertyReference findProperty(const QList<QmlDebugPropertyReference> &props, const QString &name)
    {
        foreach(const QmlDebugPropertyReference &p, props) {
            if (p.name() == name)
                return p;
        }
        return QmlDebugPropertyReference();
    }

    int countNotifiableProperties(const QObject *obj)
    {
        int count = 0;
        for (int i=0; i<obj->metaObject()->propertyCount(); i++) {
            QMetaProperty p = obj->metaObject()->property(i);
            if (p.hasNotifySignal())
                count++;
        }
        return count;
    }

private slots:
    void initTestCase();

    void watch_property();
    void watch_object();
    void watch_expression();
    void watch_expression_data();

    void queryAvailableEngines();
    void queryRootContexts();
    void queryObject();
    void queryObjectRecursive();
    void queryExpressionResult();
    void queryExpressionResult_data();
};

void tst_QmlDebug::initTestCase()
{
    m_dbg = new QmlEngineDebug(m_conn, this);
}

void tst_QmlDebug::watch_property()
{
    QmlDebugObjectReference obj = findRootObject();
    QmlDebugPropertyReference prop = findProperty(obj.properties(), "width");

    QmlDebugPropertyWatch *watch = m_dbg->addWatch(prop, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->name(), prop.name());

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));
    QEventLoop loop;
    QTimer timer;
    connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)), &loop, SLOT(quit()));
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));

    int origWidth = m_rootItem->property("width").toInt();
    timer.start(5000);
    m_rootItem->setProperty("width", origWidth*2);
    loop.exec();

    if (!timer.isActive())
        QFAIL("Did not receive valueChanged() for property");

    m_dbg->removeWatch(watch);
    delete watch;

    // restore original value and verify spy doesn't get a signal since watch has been removed
    m_rootItem->setProperty("width", origWidth);
    QTest::qWait(500);
    QCOMPARE(spy.count(), 1);

    QCOMPARE(spy.at(0).at(0).value<QByteArray>(), prop.name().toUtf8());
    QCOMPARE(spy.at(0).at(1).value<QVariant>(), qVariantFromValue(origWidth*2));
}

void tst_QmlDebug::watch_object()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    QmlDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QmlDebugObjectReference obj = q_obj->object();

    delete q_engines;
    delete q_context;
    delete q_obj;

    QmlDebugWatch *watch = m_dbg->addWatch(obj, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());

    m_savedValueChanges.clear();
    connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)),
            SLOT(saveValueChange(QByteArray,QVariant)));

    int origWidth = m_rootItem->property("width").toInt();
    int origHeight = m_rootItem->property("height").toInt();
    m_rootItem->setProperty("width", origWidth*2);
    m_rootItem->setProperty("height", origHeight*2);

    QEventLoop loop;
    QTimer timer;
    timer.start(5000);
    while (timer.isActive() &&
            (!m_savedValueChanges.contains("width") || !m_savedValueChanges.contains("height"))) {
        loop.processEvents(QEventLoop::AllEvents, 50);
    }

    QVariant newWidth = m_savedValueChanges["width"];
    QVariant newHeight = m_savedValueChanges["height"];

    m_dbg->removeWatch(watch);
    delete watch;

    // since watch has been removed, restoring the original values should not trigger a valueChanged()
    m_savedValueChanges.clear();
    m_rootItem->setProperty("width", origWidth);
    m_rootItem->setProperty("height", origHeight);
    QTest::qWait(500);
    QCOMPARE(m_savedValueChanges.count(), 0);

    if (newWidth.isNull() || newHeight.isNull()) {
        QString s = QString("Did not receive both width and height changes (width=%1, height=%2)")
                .arg(newWidth.toString()).arg(newHeight.toString());
        QFAIL(qPrintable(s));
    }

    QCOMPARE(newWidth, qVariantFromValue(origWidth*2));
    QCOMPARE(newHeight, qVariantFromValue(origHeight*2));
}

void tst_QmlDebug::watch_expression()
{
    QFETCH(QString, expr);
    QFETCH(int, increment);
    QFETCH(int, incrementCount);

    int origWidth = m_rootItem->property("width").toInt();
    
    QmlDebugObjectReference obj = findRootObject();
    QmlDebugPropertyReference prop;

    QmlDebugObjectExpressionWatch *watch = m_dbg->addWatch(obj, expr, this);
    QCOMPARE(watch->state(), QmlDebugWatch::Waiting);
    QCOMPARE(watch->objectDebugId(), obj.debugId());
    QCOMPARE(watch->expression(), expr);

    QSignalSpy spy(watch, SIGNAL(valueChanged(QByteArray,QVariant)));
    int expectedSpyCount = incrementCount + 1;  // should also get signal with expression's initial value

    int width = origWidth;
    for (int i=0; i<incrementCount+1; i++) {
        QTimer timer;
        timer.start(5000);
        if (i > 0) {
            width += increment;
            m_rootItem->setProperty("width", width);
        }
        QEventLoop loop;
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        connect(watch, SIGNAL(valueChanged(QByteArray,QVariant)), &loop, SLOT(quit()));
        loop.exec();
        if (!timer.isActive())
            QFAIL("Did not receive valueChanged() signal for expression");
    }

    m_dbg->removeWatch(watch);
    delete watch;

    // restore original value and verify spy doesn't get a signal since watch has been removed
    m_rootItem->setProperty("width", origWidth); // may increase spy count before QCOMPARE()
    QTest::qWait(500);
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

void tst_QmlDebug::queryAvailableEngines()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);

    // TODO have multiple engines
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
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    QmlContext *actualContext = m_engine->rootContext();
    QmlDebugContextReference context = q_context->rootContext();
    QCOMPARE(context.debugId(), QmlDebugService::idForObject(actualContext));
    QCOMPARE(context.name(), actualContext->objectName());

    QCOMPARE(context.objects().count(), 1);

    // root context query sends only root object data - it doesn't fill in
    // the children or property info
    QCOMPARE(context.objects()[0].properties().count(), 0);
    QCOMPARE(context.objects()[0].children().count(), 0);

    // TODO have multiple contexts
    QCOMPARE(context.contexts().count(), 0);

    delete q_engines;
    delete q_context;
}

void tst_QmlDebug::queryObject()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    QmlDebugObjectQuery *q_obj = m_dbg->queryObject(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QmlDebugObjectReference obj = q_obj->object();
    verifyRootObject(obj);

    QVERIFY(obj.children().count() >= 2);

    // non-recursive query, children data not available
    foreach(const QmlDebugObjectReference &child, obj.children())
        QCOMPARE(child.properties().count(), 0);

    delete q_engines;
    delete q_context;
    delete q_obj;
}

void tst_QmlDebug::queryObjectRecursive()
{
    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    QmlDebugObjectQuery *q_obj = m_dbg->queryObjectRecursive(q_context->rootContext().objects()[0], this);
    waitForQuery(q_obj);

    QmlDebugObjectReference obj = q_obj->object();
    verifyRootObject(obj);

    delete q_engines;
    delete q_context;
    delete q_obj;

    QList<QmlDebugObjectReference> children = obj.children();
    QVERIFY(children.count() >= 2); // there may be additional properties e.g. StateGroup

    QList<QmlDebugPropertyReference> props;
    QHash<QString, QVariant> expected;

    QmlDebugObjectReference rectRef;
    QmlDebugObjectReference textRef;
    foreach (const QmlDebugObjectReference &child, children) {
        if (child.className() == "Rectangle") {
            props = child.properties();
            QVERIFY(props.count() > 0);
            expected.clear();
            expected["width"] = 500;
            expected["height"] = 600;
            expected["color"] = "blue";
            verifyProperties(props, expected);
            rectRef = child;
        } else if (child.className() == "Text") {
            props = child.properties();
            QVERIFY(props.count() > 0);
            expected.clear();
            expected["color"] = "red";
            verifyProperties(props, expected);
            textRef = child;
        }
    }

    QVERIFY(!rectRef.className().isEmpty());
    QVERIFY(!textRef.className().isEmpty());

    QObject *rectObj = 0;
    QObject *textObj = 0;
    foreach (QObject *o, m_rootItem->children()) {
        if (o->metaObject()->className() == QmlGraphicsRectangle::staticMetaObject.className())
            rectObj = o;
        else if (o->metaObject()->className() == QmlGraphicsText::staticMetaObject.className())
            textObj = o;
    }

    QVERIFY(rectObj);
    QVERIFY(textObj);

    for (int i=0; i<rectObj->metaObject()->propertyCount(); i++) {
        QMetaProperty p = rectObj->metaObject()->property(i);
        QmlDebugPropertyReference pd = findProperty(rectRef.properties(), p.name());
        if (!pd.name().isEmpty()) {
            QCOMPARE(pd.name(), QString::fromUtf8(p.name()));
            if (p.type() < QVariant::UserType)
                QCOMPARE(pd.value(), p.read(rectObj));
            if (pd.name() != "parent")
                QCOMPARE(pd.valueTypeName(), QString::fromUtf8(p.typeName()));
            QCOMPARE(pd.hasNotifySignal(), p.hasNotifySignal());
        }
    }
}

void tst_QmlDebug::queryExpressionResult()
{
    QFETCH(QString, expr);
    QFETCH(QVariant, result);

    QmlDebugEnginesQuery *q_engines = m_dbg->queryAvailableEngines(this);
    waitForQuery(q_engines);
    
    QmlDebugRootContextQuery *q_context = m_dbg->queryRootContexts(q_engines->engines()[0].debugId(), this);
    waitForQuery(q_context);

    QmlDebugExpressionQuery *q_expr = m_dbg->queryExpressionResult(q_context->rootContext().objects()[0].debugId(), expr, this);
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


class TestRunnerThread : public QThread
{
    Q_OBJECT
public:
    void run() {
        QTest::qWait(1000);
        connectToEngine();
    }

    QPointer<QmlEngine> m_engine;
    QPointer<QmlGraphicsItem> m_item;

signals:
    void testsFinished();

public slots:

    void connectToEngine()
    {
        QmlDebugConnection conn;
        conn.connectToHost("127.0.0.1", 3768);
        bool ok = conn.waitForConnected(5000);
        Q_ASSERT(ok);
        while (!m_engine && !m_item)
            QTest::qWait(50);

        tst_QmlDebug test(&conn, m_engine, m_item);
        QTest::qExec(&test); 
        emit testsFinished();
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    qputenv("QML_DEBUG_SERVER_PORT", "3768");

    TestRunnerThread thread;
    QObject::connect(&thread, SIGNAL(testsFinished()), qApp, SLOT(quit()));
    thread.start();

    QmlEngine engine;  // blocks until client connects

    QmlComponent component(&engine,
                "import Qt 4.6\n"
                    "\n"                // don't remove, line number is tested
                    "  Item {\n"        // don't remove spaces, column number is tested
                        "width: 10; height: 20;\n"
                        "Rectangle { id: blueRect; width: 500; height: 600; color: \"blue\"; }"
                        "Text { color: \"red\"; }"
                    "}\n",
            QUrl("file://"));
    Q_ASSERT(component.isReady());
    QObject *o = component.create();
    QObject::connect(&thread, SIGNAL(testsFinished()), o, SLOT(deleteLater()));

    // start the test
    thread.m_engine = &engine;
    thread.m_item = qobject_cast<QmlGraphicsItem*>(o);

    return app.exec();

}

//QTEST_MAIN(tst_QmlDebug)

#include "tst_qmldebug.moc"
