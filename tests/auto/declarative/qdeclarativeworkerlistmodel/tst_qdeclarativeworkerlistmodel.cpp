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

#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeitem.h>

#include <private/qdeclarativeworkerscript_p.h>
#include <private/qdeclarativelistmodel_p.h>
#include "../../../shared/util.h"



class tst_QDeclarativeWorkerListModel : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeWorkerListModel() {}
private slots:
    void clear();
    void remove();
    void append();
    void insert();
    void get();
    void set();

private:
    QByteArray modificationWarning() const {
        QString file = QUrl::fromLocalFile(SRCDIR "/data/model.qml").toString();
        return QString("QML WorkerListModel (" + file + ":6:5) List can only be modified from a WorkerScript").toUtf8();
    }

    QDeclarativeEngine m_engine;
};

void tst_QDeclarativeWorkerListModel::clear()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QCOMPARE(model->count(), 0);
    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "append({'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);

    QTest::ignoreMessage(QtWarningMsg, modificationWarning().constData());
    model->clear();
    QCOMPARE(model->count(), 1);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "clear()")));
    QTRY_COMPARE(model->count(), 0);

    qApp->processEvents();
}

void tst_QDeclarativeWorkerListModel::remove()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "append({'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);

    QTest::ignoreMessage(QtWarningMsg, modificationWarning().constData());
    model->remove(0);
    QCOMPARE(model->count(), 1);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "remove(0)")));
    QTRY_COMPARE(model->count(), 0);

    qApp->processEvents();
}

void tst_QDeclarativeWorkerListModel::append()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "append({'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);
    
    QTest::ignoreMessage(QtWarningMsg, modificationWarning().constData());
    model->append(QScriptValue(1));
    QCOMPARE(model->count(), 1);

    qApp->processEvents();
}

void tst_QDeclarativeWorkerListModel::insert()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "insert(0, {'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);

    QTest::ignoreMessage(QtWarningMsg, modificationWarning().constData());
    model->insert(0, QScriptValue(1));
    QCOMPARE(model->count(), 1);

    qApp->processEvents();
}

void tst_QDeclarativeWorkerListModel::get()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "append({'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);
    QCOMPARE(model->get(0).property("name").toString(), QString("A"));

    qApp->processEvents();
}

void tst_QDeclarativeWorkerListModel::set()
{
    QDeclarativeComponent component(&m_engine, SRCDIR "/data/model.qml");
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(component.create());
    QVERIFY(item != 0);
    QDeclarativeWorkerListModel *model = item->property("model").value<QDeclarativeWorkerListModel*>();
    QVERIFY(model != 0);

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "append({'name': 'A'})")));
    QTRY_COMPARE(model->count(), 1);

    QTest::ignoreMessage(QtWarningMsg, modificationWarning().constData());
    model->set(0, QScriptValue(1));

    QVERIFY(QMetaObject::invokeMethod(item, "workerModifyModel", Q_ARG(QVariant, "set(0, {'name': 'Z'})")));
    QTRY_COMPARE(model->get(0).property("name").toString(), QString("Z"));

    qApp->processEvents();
}

QTEST_MAIN(tst_QDeclarativeWorkerListModel)

#include "tst_qdeclarativeworkerlistmodel.moc"

