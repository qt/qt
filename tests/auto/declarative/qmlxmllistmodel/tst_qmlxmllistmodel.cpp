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
#include <QtTest/qsignalspy.h>
#include <QtCore/qtimer.h>

#ifdef QTEST_XMLPATTERNS
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlxmllistmodel_p.h>
#include "../../../shared/util.h"

typedef QPair<int, int> QmlXmlListRange;
typedef QList<QVariantList> QmlXmlModelData;

Q_DECLARE_METATYPE(QList<QmlXmlListRange>)
Q_DECLARE_METATYPE(QmlXmlModelData)

class tst_qmlxmllistmodel : public QObject

{
    Q_OBJECT
public:
    tst_qmlxmllistmodel() {}

private slots:
    void buildModel();
    void missingFields();
    void cdata();
    void attributes();
    void roles();
    void roleErrors();
    void uniqueRoleNames();
    void useKeys();
    void useKeys_data();
    void noKeysValueChanges();
    void keysChanged();

private:
    QString makeItemXmlAndData(const QString &data, QmlXmlModelData *modelData = 0) const
    {
        if (modelData)
            modelData->clear();
        QString xml;

        if (!data.isEmpty()) {
            QStringList items = data.split(";");
            foreach(const QString &item, items) {
                QVariantList variants;
                xml += QLatin1String("<item>");
                QStringList fields = item.split(",");
                foreach(const QString &field, fields) {
                    QStringList values = field.split("=");
                    Q_ASSERT(values.count() == 2);
                    xml += QString("<%1>%2</%1>").arg(values[0], values[1]);
                    if (!modelData)
                        continue;
                    bool isNum = false;
                    int number = values[1].toInt(&isNum);
                    if (isNum)
                        variants << number;
                    else
                        variants << values[1];
                }
                xml += QLatin1String("</item>");
                if (modelData)
                    modelData->append(variants);
            }
        }

        QString decl = "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>";
        return decl + QLatin1String("<data>") + xml + QLatin1String("</data>");
    }

    QmlEngine engine;
};

void tst_qmlxmllistmodel::buildModel()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/model.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 9);

    QList<int> roles;
    roles << Qt::UserRole << Qt::UserRole + 1 << Qt::UserRole + 2 << Qt::UserRole + 3;
    QHash<int, QVariant> data = listModel->data(3, roles);
    QVERIFY(data.count() == 4);
    QCOMPARE(data.value(Qt::UserRole).toString(), QLatin1String("Spot"));
    QCOMPARE(data.value(Qt::UserRole+1).toString(), QLatin1String("Dog"));
    QCOMPARE(data.value(Qt::UserRole+2).toInt(), 9);
    QCOMPARE(data.value(Qt::UserRole+3).toString(), QLatin1String("Medium"));

    delete listModel;
}

void tst_qmlxmllistmodel::missingFields()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/model2.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 9);

    QList<int> roles;
    roles << Qt::UserRole << Qt::UserRole + 1 << Qt::UserRole + 2 << Qt::UserRole + 3 << Qt::UserRole + 4;
    QHash<int, QVariant> data = listModel->data(5, roles);
    QVERIFY(data.count() == 5);
    QCOMPARE(data.value(Qt::UserRole+3).toString(), QLatin1String(""));
    QCOMPARE(data.value(Qt::UserRole+4).toString(), QLatin1String(""));

    data = listModel->data(7, roles);
    QVERIFY(data.count() == 5);
    QCOMPARE(data.value(Qt::UserRole+2).toString(), QLatin1String(""));

    delete listModel;
}

void tst_qmlxmllistmodel::cdata()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/recipes.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 5);

    QList<int> roles;
    roles << Qt::UserRole + 2;
    QHash<int, QVariant> data = listModel->data(2, roles);
    QVERIFY(data.count() == 1);
    QVERIFY(data.value(Qt::UserRole+2).toString().startsWith(QLatin1String("<html>")));

    delete listModel;
}

void tst_qmlxmllistmodel::attributes()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/recipes.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 5);

    QList<int> roles;
    roles << Qt::UserRole;
    QHash<int, QVariant> data = listModel->data(2, roles);
    QVERIFY(data.count() == 1);
    QCOMPARE(data.value(Qt::UserRole).toString(), QLatin1String("Vegetable Soup"));

    delete listModel;
}

void tst_qmlxmllistmodel::roles()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/model.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 9);

    QList<int> roles = listModel->roles();
    QCOMPARE(roles.count(), 4);
    QCOMPARE(listModel->toString(roles.at(0)), QLatin1String("name"));
    QCOMPARE(listModel->toString(roles.at(1)), QLatin1String("type"));
    QCOMPARE(listModel->toString(roles.at(2)), QLatin1String("age"));
    QCOMPARE(listModel->toString(roles.at(3)), QLatin1String("size"));

    delete listModel;
}

void tst_qmlxmllistmodel::roleErrors()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/roleErrors.qml"));
    QTest::ignoreMessage(QtWarningMsg, QString("QML XmlRole (" + QUrl::fromLocalFile(SRCDIR "/data/roleErrors.qml").toString() + ":6:5) An XmlRole query must not start with '/'").toUtf8().constData());
    //### make sure we receive all expected warning messages.
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 9);

    QList<int> roles;
    roles << Qt::UserRole << Qt::UserRole + 1 << Qt::UserRole + 2 << Qt::UserRole + 3;
    QHash<int, QVariant> data = listModel->data(3, roles);
    QVERIFY(data.count() == 4);

    //### should any of these return valid values?
    QCOMPARE(data.value(Qt::UserRole), QVariant());
    QCOMPARE(data.value(Qt::UserRole+1), QVariant());
    QCOMPARE(data.value(Qt::UserRole+2), QVariant());

    QEXPECT_FAIL("", "QT-2456", Continue);
    QCOMPARE(data.value(Qt::UserRole+3), QVariant());

    delete listModel;
}

void tst_qmlxmllistmodel::uniqueRoleNames()
{
    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/unique.qml"));
    QTest::ignoreMessage(QtWarningMsg, QString("QML XmlRole (" + QUrl::fromLocalFile(SRCDIR "/data/unique.qml").toString() + ":7:5) \"name\" duplicates a previous role name and will be disabled.").toUtf8().constData());
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 9);

    QList<int> roles = listModel->roles();
    QCOMPARE(roles.count(), 1);

    delete listModel;
}

void tst_qmlxmllistmodel::useKeys()
{
    // If using incremental updates through keys, the model should only
    // insert & remove some of the items, instead of throwing everything
    // away and causing the view to repaint the whole view.

    QFETCH(QString, oldXml);
    QFETCH(int, oldCount);
    QFETCH(QString, newXml);
    QFETCH(QmlXmlModelData, newData);
    QFETCH(QList<QmlXmlListRange>, insertRanges);
    QFETCH(QList<QmlXmlListRange>, removeRanges);

    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/roleKeys.qml"));
    QmlXmlListModel *model = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(model != 0);
    
    model->setXml(oldXml);
    QTRY_COMPARE(model->count(), oldCount);

    QSignalSpy spyInsert(model, SIGNAL(itemsInserted(int,int)));
    QSignalSpy spyRemove(model, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy spyCount(model, SIGNAL(countChanged()));

    model->setXml(newXml);

    if (oldCount != newData.count()) {
        QTRY_COMPARE(model->count(), newData.count());
        QCOMPARE(spyCount.count(), 1);
    } else {
        QTRY_VERIFY(spyInsert.count() > 0 || spyRemove.count() > 0);
        QCOMPARE(spyCount.count(), 0);
    }

    QList<int> roles = model->roles();
    for (int i=0; i<model->count(); i++) {
        for (int j=0; j<roles.count(); j++)
            QCOMPARE(model->data(i, roles[j]), newData[i][j]);
    }

    QCOMPARE(spyInsert.count(), insertRanges.count());
    for (int i=0; i<spyInsert.count(); i++) {
        QCOMPARE(spyInsert[i][0].toInt(), insertRanges[i].first);
        QCOMPARE(spyInsert[i][1].toInt(), insertRanges[i].second);
    }

    QCOMPARE(spyRemove.count(), removeRanges.count());
    for (int i=0; i<spyRemove.count(); i++) {
        QCOMPARE(spyRemove[i][0].toInt(), removeRanges[i].first);
        QCOMPARE(spyRemove[i][1].toInt(), removeRanges[i].second);
    }
}

void tst_qmlxmllistmodel::useKeys_data()
{
    QTest::addColumn<QString>("oldXml");
    QTest::addColumn<int>("oldCount");
    QTest::addColumn<QString>("newXml");
    QTest::addColumn<QmlXmlModelData>("newData");
    QTest::addColumn<QList<QmlXmlListRange> >("insertRanges");
    QTest::addColumn<QList<QmlXmlListRange> >("removeRanges");

    QmlXmlModelData modelData;

    QTest::newRow("append 1")
        << makeItemXmlAndData("name=A,age=25,sport=Football") << 1
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(1, 1))
        << QList<QmlXmlListRange>();

    QTest::newRow("append multiple")
        << makeItemXmlAndData("name=A,age=25,sport=Football") << 1
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(1, 2))
        << QList<QmlXmlListRange>();

    QTest::newRow("insert in different spots")
        << makeItemXmlAndData("name=B,age=35,sport=Athletics") << 1
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling;name=D,age=55,sport=Golf", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(0, 1) << qMakePair(2,2))
        << QList<QmlXmlListRange>();

    QTest::newRow("insert in middle")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=D,age=55,sport=Golf") << 2
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling;name=D,age=55,sport=Golf", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(1, 2))
        << QList<QmlXmlListRange>();

    QTest::newRow("remove first")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics") << 2
        << makeItemXmlAndData("name=B,age=35,sport=Athletics", &modelData)
        << modelData
        << QList<QmlXmlListRange>()
        << (QList<QmlXmlListRange>() << qMakePair(0, 1));

    QTest::newRow("remove last")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics") << 2
        << makeItemXmlAndData("name=A,age=25,sport=Football", &modelData)
        << modelData
        << QList<QmlXmlListRange>()
        << (QList<QmlXmlListRange>() << qMakePair(1, 1));

    QTest::newRow("remove from multiple spots")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling;name=D,age=55,sport=Golf;name=E,age=65,sport=Fencing") << 5
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=C,age=45,sport=Curling", &modelData)
        << modelData
        << QList<QmlXmlListRange>()
        << (QList<QmlXmlListRange>() << qMakePair(1, 1) << qMakePair(3,2));

    QTest::newRow("remove all")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling") << 3
        << makeItemXmlAndData("", &modelData)
        << modelData
        << QList<QmlXmlListRange>()
        << (QList<QmlXmlListRange>() << qMakePair(0, 3));

    QTest::newRow("replace item")
        << makeItemXmlAndData("name=A,age=25,sport=Football") << 1
        << makeItemXmlAndData("name=ZZZ,age=25,sport=Football", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(0, 1))
        << (QList<QmlXmlListRange>() << qMakePair(0, 1));

    QTest::newRow("add and remove simultaneously")
        << makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics;name=C,age=45,sport=Curling;name=D,age=55,sport=Golf") << 4
        << makeItemXmlAndData("name=B,age=35,sport=Athletics;name=E,age=65,sport=Fencing", &modelData)
        << modelData
        << (QList<QmlXmlListRange>() << qMakePair(1, 1))
        << (QList<QmlXmlListRange>() << qMakePair(0, 1) << qMakePair(2,2));
}

void tst_qmlxmllistmodel::noKeysValueChanges()
{
    // The 'key' roles are 'name' and 'age', as defined in roleKeys.qml.
    // If a 'sport' value is changed, the model should not be reloaded,
    // since 'sport' is not marked as a key.

    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/roleKeys.qml"));
    QmlXmlListModel *model = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(model != 0);
    
    QString xml;
    
    xml = makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics");
    model->setXml(xml);
    QTRY_COMPARE(model->count(), 2);

    QSignalSpy spyInsert(model, SIGNAL(itemsInserted(int,int)));
    QSignalSpy spyRemove(model, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy spyCount(model, SIGNAL(countChanged()));

    xml = makeItemXmlAndData("name=A,age=25,sport=AussieRules;name=B,age=35,sport=Athletics");
    model->setXml(xml);

    // wait for the new xml data to be set, and verify no signals were emitted
    for (int i=0; i<50; i++) {
        QTest::qWait(100);
        if (model->data(0, model->roles()[2]).toString() != QLatin1String("AussieRules"))
            break;
    }
    QCOMPARE(model->data(0, model->roles()[2]).toString(), QLatin1String("AussieRules"));

    QVERIFY(spyInsert.count() == 0);
    QVERIFY(spyRemove.count() == 0);
    QVERIFY(spyCount.count() == 0);
    
    QCOMPARE(model->count(), 2);
}

void tst_qmlxmllistmodel::keysChanged()
{
    // If the key roles change, the next time the data is reloaded, it should
    // delete all its data and build a clean model (i.e. same behaviour as
    // if no keys are set).

    QmlComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/roleKeys.qml"));
    QmlXmlListModel *model = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(model != 0);

    QString xml = makeItemXmlAndData("name=A,age=25,sport=Football;name=B,age=35,sport=Athletics");
    model->setXml(xml);
    QTRY_COMPARE(model->count(), 2);

    QSignalSpy spyInsert(model, SIGNAL(itemsInserted(int,int)));
    QSignalSpy spyRemove(model, SIGNAL(itemsRemoved(int,int)));
    QSignalSpy spyCount(model, SIGNAL(countChanged()));

    QVERIFY(QMetaObject::invokeMethod(model, "disableNameKey"));
    model->setXml(xml);

    QTRY_VERIFY(spyInsert.count() > 0 && spyRemove.count() > 0);

    QCOMPARE(spyInsert.count(), 1);
    QCOMPARE(spyInsert[0][0].toInt(), 0);
    QCOMPARE(spyInsert[0][1].toInt(), 2);

    QCOMPARE(spyRemove.count(), 1);
    QCOMPARE(spyRemove[0][0].toInt(), 0);
    QCOMPARE(spyRemove[0][1].toInt(), 2);

    QCOMPARE(spyCount.count(), 0);
}

QTEST_MAIN(tst_qmlxmllistmodel)

#include "tst_qmlxmllistmodel.moc"

#else
QTEST_NOOP_MAIN
#endif
