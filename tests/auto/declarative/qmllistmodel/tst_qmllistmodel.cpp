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
#include <QtDeclarative/private/qmllistmodel_p.h>
#include <QtDeclarative/private/qmlexpression_p.h>
#include <QDebug>

class tst_QmlListModel : public QObject
{
    Q_OBJECT
public:
    tst_QmlListModel() {}

private slots:
    void dynamic_data();
    void dynamic();
};

void tst_QmlListModel::dynamic_data()
{
    QTest::addColumn<QString>("script");
    QTest::addColumn<int>("result");
    QTest::addColumn<QString>("warning");

    // Simple flat model

    QTest::newRow("count") << "count" << 0 << "";

    QTest::newRow("append1") << "{append({'foo':123});count}" << 1 << "";
    QTest::newRow("append2") << "{append({'foo':123,'bar':456});count}" << 1 << "";
    QTest::newRow("append3a") << "{append({'foo':123});append({'foo':456});get(0).foo}" << 123 << "";
    QTest::newRow("append3b") << "{append({'foo':123});append({'foo':456});get(1).foo}" << 456 << "";

    QTest::newRow("clear1") << "{append({'foo':456});clear();count}" << 0 << "";
    QTest::newRow("clear2") << "{append({'foo':123});append({'foo':456});clear();count}" << 0 << "";
    QTest::newRow("clear2") << "{append({'foo':123});clear();get(0).foo}" << 0 << "QML QmlListModel (unknown location) get: index 0 out of range";

    QTest::newRow("remove1") << "{append({'foo':123});remove(0);count}" << 0 << "";
    QTest::newRow("remove2a") << "{append({'foo':123});append({'foo':456});remove(0);count}" << 1 << "";
    QTest::newRow("remove2b") << "{append({'foo':123});append({'foo':456});remove(0);get(0).foo}" << 456 << "";
    QTest::newRow("remove2c") << "{append({'foo':123});append({'foo':456});remove(1);get(0).foo}" << 123 << "";
    QTest::newRow("remove3") << "{append({'foo':123});remove(0);get(0).foo}" << 0 << "QML QmlListModel (unknown location) get: index 0 out of range";

    QTest::newRow("insert1") << "{insert(0,{'foo':123});count}" << 1 << "";
    QTest::newRow("insert2") << "{insert(1,{'foo':123});count}" << 0 << "QML QmlListModel (unknown location) insert: index 1 out of range";
    QTest::newRow("insert3a") << "{append({'foo':123});insert(1,{'foo':456});count}" << 2 << "";
    QTest::newRow("insert3b") << "{append({'foo':123});insert(1,{'foo':456});get(0).foo}" << 123 << "";
    QTest::newRow("insert3c") << "{append({'foo':123});insert(1,{'foo':456});get(1).foo}" << 456 << "";
    QTest::newRow("insert3d") << "{append({'foo':123});insert(0,{'foo':456});get(0).foo}" << 456 << "";
    QTest::newRow("insert3e") << "{append({'foo':123});insert(0,{'foo':456});get(1).foo}" << 123 << "";
    QTest::newRow("insert4") << "{append({'foo':123});insert(-1,{'foo':456})}" << 0 << "QML QmlListModel (unknown location) insert: index -1 out of range";

    QTest::newRow("set1") << "{append({'foo':123});set(0,{'foo':456});count}" << 1 << "";
    QTest::newRow("set2") << "{append({'foo':123});set(0,{'foo':456});get(0).foo}" << 456 << "";
    QTest::newRow("set3a") << "{append({'foo':123,'bar':456});set(0,{'foo':999});get(0).foo}" << 999 << "";
    QTest::newRow("set3b") << "{append({'foo':123,'bar':456});set(0,{'foo':999});get(0).bar}" << 456 << "";

    QTest::newRow("setprop1") << "{append({'foo':123});set(0,'foo',456);count}" << 1 << "";
    QTest::newRow("setprop2") << "{append({'foo':123});set(0,'foo',456);get(0).foo}" << 456 << "";
    QTest::newRow("setprop3a") << "{append({'foo':123,'bar':456});set(0,'foo',999);get(0).foo}" << 999 << "";
    QTest::newRow("setprop3b") << "{append({'foo':123,'bar':456});set(0,'foo',999);get(0).bar}" << 456 << "";

    QTest::newRow("move1a") << "{append({'foo':123});append({'foo':456});move(0,1,1);count}" << 2 << "";
    QTest::newRow("move1b") << "{append({'foo':123});append({'foo':456});move(0,1,1);get(0).foo}" << 456 << "";
    QTest::newRow("move1c") << "{append({'foo':123});append({'foo':456});move(0,1,1);get(1).foo}" << 123 << "";
    QTest::newRow("move1d") << "{append({'foo':123});append({'foo':456});move(1,0,1);get(0).foo}" << 456 << "";
    QTest::newRow("move1e") << "{append({'foo':123});append({'foo':456});move(1,0,1);get(1).foo}" << 123 << "";
    QTest::newRow("move2a") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);count}" << 3 << "";
    QTest::newRow("move2b") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(0).foo}" << 789 << "";
    QTest::newRow("move2c") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(1).foo}" << 123 << "";
    QTest::newRow("move2d") << "{append({'foo':123});append({'foo':456});append({'foo':789});move(0,1,2);get(2).foo}" << 456 << "";

    // Structured model

    // XXX todo
}

void tst_QmlListModel::dynamic()
{
    QFETCH(QString, script);
    QFETCH(int, result);
    QFETCH(QString, warning);

    QmlEngine engine;
    QmlListModel model;
    QmlEngine::setContextForObject(&model,engine.rootContext());
    engine.rootContext()->addDefaultObject(&model);
    QmlExpression e(engine.rootContext(), script, &model);
    if (!warning.isEmpty())
        QTest::ignoreMessage(QtWarningMsg, warning.toLatin1());
    int actual = e.value().toInt();
    if (e.hasError())
        qDebug() << e.error(); // errors not expected
    QVERIFY(!e.hasError());
    QCOMPARE(actual,result);
}

QTEST_MAIN(tst_QmlListModel)

#include "tst_qmllistmodel.moc"
