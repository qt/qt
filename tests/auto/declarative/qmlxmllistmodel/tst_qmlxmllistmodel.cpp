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
#include <QtDeclarative/qmlengine.h>
#include <QtDeclarative/qmlcomponent.h>
#include <private/qmlxmllistmodel_p.h>
#include "../../../shared/util.h"

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

private:
    QmlEngine engine;
};

void tst_qmlxmllistmodel::buildModel()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/data/model.qml"));
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
}

void tst_qmlxmllistmodel::missingFields()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/data/model2.qml"));
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
}

void tst_qmlxmllistmodel::cdata()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/data/recipes.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 5);

    QList<int> roles;
    roles << Qt::UserRole + 2;
    QHash<int, QVariant> data = listModel->data(2, roles);
    QVERIFY(data.count() == 1);
    QVERIFY(data.value(Qt::UserRole+2).toString().startsWith(QLatin1String("<html>")));
}

void tst_qmlxmllistmodel::attributes()
{
    QmlComponent component(&engine, QUrl("file://" SRCDIR "/data/recipes.qml"));
    QmlXmlListModel *listModel = qobject_cast<QmlXmlListModel*>(component.create());
    QVERIFY(listModel != 0);
    QTRY_COMPARE(listModel->count(), 5);

    QList<int> roles;
    roles << Qt::UserRole;
    QHash<int, QVariant> data = listModel->data(2, roles);
    QVERIFY(data.count() == 1);
    QCOMPARE(data.value(Qt::UserRole).toString(), QLatin1String("Vegetable Soup"));
}


QTEST_MAIN(tst_qmlxmllistmodel)

#include "tst_qmlxmllistmodel.moc"
