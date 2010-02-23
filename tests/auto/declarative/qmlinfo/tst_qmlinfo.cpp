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
#include <QmlEngine>
#include <QmlComponent>
#include <QPushButton>
#include <QmlContext>
#include <qmlinfo.h>

class tst_qmlinfo : public QObject
{
    Q_OBJECT
public:
    tst_qmlinfo() {}

private slots:
    void qmlObject();
    void nestedQmlObject();
    void nonQmlObject();
    void nullObject();
    void nonQmlContextedObject();

private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_qmlinfo::qmlObject()
{
    QmlComponent component(&engine, TEST_FILE("qmlObject.qml"));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QString message = "QML QObject_QML_0 (" + component.url().toString() + ":3:1) Test Message";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(object) << "Test Message";

    QObject *nested = qvariant_cast<QObject *>(object->property("nested"));
    QVERIFY(nested != 0);

    message = "QML QtObject (" + component.url().toString() + ":6:13) Second Test Message";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested) << "Second Test Message";
}

void tst_qmlinfo::nestedQmlObject()
{
    QmlComponent component(&engine, TEST_FILE("nestedQmlObject.qml"));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QObject *nested = qvariant_cast<QObject *>(object->property("nested"));
    QVERIFY(nested != 0);
    QObject *nested2 = qvariant_cast<QObject *>(object->property("nested2"));
    QVERIFY(nested2 != 0);

    QString message = "QML NestedObject (" + component.url().toString() + ":5:13) Outer Object";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested) << "Outer Object";

    message = "QML QtObject (" + TEST_FILE("NestedObject.qml").toString() + ":6:14) Inner Object";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested2) << "Inner Object";
}

void tst_qmlinfo::nonQmlObject()
{
    QObject object;
    QTest::ignoreMessage(QtWarningMsg, "QML QtObject (unknown location) Test Message");
    qmlInfo(&object) << "Test Message";

    QPushButton pbObject;
    QTest::ignoreMessage(QtWarningMsg, "QML QPushButton (unknown location) Test Message");
    qmlInfo(&pbObject) << "Test Message";
}

void tst_qmlinfo::nullObject()
{
    QTest::ignoreMessage(QtWarningMsg, "QML (unknown location) Null Object Test Message");
    qmlInfo(0) << "Null Object Test Message";
}

void tst_qmlinfo::nonQmlContextedObject()
{
    QObject object;
    QmlContext context(&engine);
    QmlEngine::setContextForObject(&object, &context);
    QTest::ignoreMessage(QtWarningMsg, "QML QtObject (unknown location) Test Message");
    qmlInfo(&object) << "Test Message";
}

QTEST_MAIN(tst_qmlinfo)

#include "tst_qmlinfo.moc"
