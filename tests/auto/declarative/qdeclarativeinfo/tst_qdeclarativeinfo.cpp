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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QPushButton>
#include <QDeclarativeContext>
#include <qdeclarativeinfo.h>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_qdeclarativeinfo : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativeinfo() {}

private slots:
    void qmlObject();
    void nestedQmlObject();
    void nonQmlObject();
    void nullObject();
    void nonQmlContextedObject();

private:
    QDeclarativeEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_qdeclarativeinfo::qmlObject()
{
    QDeclarativeComponent component(&engine, TEST_FILE("qmlObject.qml"));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QString message = component.url().toString() + ":3:1: QML QObject_QML_0: Test Message";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(object) << "Test Message";

    QObject *nested = qvariant_cast<QObject *>(object->property("nested"));
    QVERIFY(nested != 0);

    message = component.url().toString() + ":6:13: QML QtObject: Second Test Message";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested) << "Second Test Message";
}

void tst_qdeclarativeinfo::nestedQmlObject()
{
    QDeclarativeComponent component(&engine, TEST_FILE("nestedQmlObject.qml"));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QObject *nested = qvariant_cast<QObject *>(object->property("nested"));
    QVERIFY(nested != 0);
    QObject *nested2 = qvariant_cast<QObject *>(object->property("nested2"));
    QVERIFY(nested2 != 0);

    QString message = component.url().toString() + ":5:13: QML NestedObject: Outer Object";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested) << "Outer Object";

    message = TEST_FILE("NestedObject.qml").toString() + ":6:14: QML QtObject: Inner Object";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(message));
    qmlInfo(nested2) << "Inner Object";
}

void tst_qdeclarativeinfo::nonQmlObject()
{
    QObject object;
    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML QtObject: Test Message");
    qmlInfo(&object) << "Test Message";

    QPushButton pbObject;
    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML QPushButton: Test Message");
    qmlInfo(&pbObject) << "Test Message";
}

void tst_qdeclarativeinfo::nullObject()
{
    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: Null Object Test Message");
    qmlInfo(0) << "Null Object Test Message";
}

void tst_qdeclarativeinfo::nonQmlContextedObject()
{
    QObject object;
    QDeclarativeContext context(&engine);
    QDeclarativeEngine::setContextForObject(&object, &context);
    QTest::ignoreMessage(QtWarningMsg, "<Unknown File>: QML QtObject: Test Message");
    qmlInfo(&object) << "Test Message";
}

QTEST_MAIN(tst_qdeclarativeinfo)

#include "tst_qdeclarativeinfo.moc"
