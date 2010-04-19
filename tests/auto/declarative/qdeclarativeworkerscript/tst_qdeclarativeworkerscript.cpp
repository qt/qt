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
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <QtScript/qscriptengine.h>

#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeitem.h>

#include <private/qdeclarativeworkerscript_p.h>
#include <private/qdeclarativeengine_p.h>
#include "../../../shared/util.h"

Q_DECLARE_METATYPE(QScriptValue)

class tst_QDeclarativeWorkerScript : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeWorkerScript() {}
private slots:
    void source();
    void messaging();
    void messaging_data();
    void messaging_sendQObjectList();
    void messaging_sendJsObject();

private:
    void waitForEchoMessage(QDeclarativeWorkerScript *worker) {
        QEventLoop loop;
        QVERIFY(connect(worker, SIGNAL(done()), &loop, SLOT(quit())));
        QTimer timer;
        timer.setSingleShot(true);
        connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
        timer.start(10000);
        loop.exec();
        QVERIFY(timer.isActive());
    }

    QDeclarativeEngine m_engine;
};

void tst_QDeclarativeWorkerScript::source()
{
    QUrl source = QUrl::fromLocalFile(SRCDIR "/data/worker.qml");

    QDeclarativeComponent component(&m_engine);
    component.setData("import Qt 4.7\nWorkerScript { source: '" + source.toString().toUtf8() + "'; }", QUrl());

    QDeclarativeWorkerScript *item = qobject_cast<QDeclarativeWorkerScript*>(component.create());
    QVERIFY(item != 0);

    QCOMPARE(item->source(), source);

    qApp->processEvents();
    delete item;
}

void tst_QDeclarativeWorkerScript::messaging()
{
    QFETCH(QVariant, value);

    QDeclarativeComponent component(&m_engine, SRCDIR "/data/worker.qml");
    QDeclarativeWorkerScript *worker = qobject_cast<QDeclarativeWorkerScript*>(component.create());
    QVERIFY(worker != 0);

    QVERIFY(QMetaObject::invokeMethod(worker, "testSend", Q_ARG(QVariant, value)));
    waitForEchoMessage(worker);

    const QMetaObject *mo = worker->metaObject();
    QCOMPARE(mo->property(mo->indexOfProperty("response")).read(worker).value<QVariant>(), value);

    qApp->processEvents();
    delete worker;
}

void tst_QDeclarativeWorkerScript::messaging_data()
{
    QTest::addColumn<QVariant>("value");

    QTest::newRow("invalid") << QVariant();
    QTest::newRow("bool") << qVariantFromValue(true);
    QTest::newRow("int") << qVariantFromValue(1001);
    QTest::newRow("real") << qVariantFromValue(10334.323);
    QTest::newRow("string") << qVariantFromValue(QString("More cheeeese, Gromit!"));
    QTest::newRow("variant list") << qVariantFromValue((QVariantList() << "a" << "b" << "c"));
}

void tst_QDeclarativeWorkerScript::messaging_sendQObjectList()
{
    // Not allowed to send QObjects other than QDeclarativeWorkerListModelAgent
    // instances. If objects are sent in a list, they will be sent as 'undefined'
    // js values.

    QDeclarativeComponent component(&m_engine, SRCDIR "/data/worker.qml");
    QDeclarativeWorkerScript *worker = qobject_cast<QDeclarativeWorkerScript*>(component.create());
    QVERIFY(worker != 0);

    QVariantList objects;
    for (int i=0; i<3; i++)
        objects << qVariantFromValue(new QObject(this));

    QVERIFY(QMetaObject::invokeMethod(worker, "testSend", Q_ARG(QVariant, qVariantFromValue(objects))));
    waitForEchoMessage(worker);

    const QMetaObject *mo = worker->metaObject();
    QVariantList result = mo->property(mo->indexOfProperty("response")).read(worker).value<QVariantList>();
    QCOMPARE(result, (QVariantList() << QVariant() << QVariant() << QVariant()));

    qApp->processEvents();
    delete worker;
}

void tst_QDeclarativeWorkerScript::messaging_sendJsObject()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/worker.qml");
    QDeclarativeWorkerScript *worker = qobject_cast<QDeclarativeWorkerScript*>(component.create());
    QVERIFY(worker != 0);

    QString jsObject = "{'name': 'zyz', 'spell power': 3101, 'haste': 1125}";

    QScriptEngine *engine = QDeclarativeEnginePrivate::getScriptEngine(qmlEngine(worker));
    QScriptValue sv = engine->newObject();
    sv.setProperty("name", "zyz");
    sv.setProperty("spell power", 3101);
    sv.setProperty("haste", 1125);

    QVERIFY(QMetaObject::invokeMethod(worker, "testSend", Q_ARG(QVariant, qVariantFromValue(sv))));
    waitForEchoMessage(worker);

    QVariant result = qVariantFromValue(false);
    QVERIFY(QMetaObject::invokeMethod(worker, "compareLiteralResponse", Qt::DirectConnection, 
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, jsObject)));
    QVERIFY(result.toBool());

    qApp->processEvents();
    delete worker;
}

QTEST_MAIN(tst_QDeclarativeWorkerScript)

#include "tst_qdeclarativeworkerscript.moc"
