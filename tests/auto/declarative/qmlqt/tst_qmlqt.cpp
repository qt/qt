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
#include <QDebug>
#include <QmlEngine>
#include <QFileInfo>
#include <QmlComponent>
#include <QDir>
#include <QVector3D>
#include <QCryptographicHash>
#include <QmlGraphicsItem>

class tst_qmlqt : public QObject
{
    Q_OBJECT
public:
    tst_qmlqt() {}

private slots:
    void enums();
    void rgba();
    void hsla();
    void rect();
    void point();
    void size();
    void vector();
    void lighter();
    void darker();
    void tint();
    void closestAngle();
    void playSound();
    void openUrlExternally();
    void md5();
    void createComponent();
    void createQmlObject();
    void consoleLog();

private:
    QmlEngine engine;
};

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_qmlqt::enums()
{
    QmlComponent component(&engine, TEST_FILE("enums.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toInt(), (int)Qt::Key_Escape);
    QCOMPARE(object->property("test2").toInt(), (int)Qt::DescendingOrder);
    QCOMPARE(object->property("test3").toInt(), (int)Qt::ElideMiddle);
    QCOMPARE(object->property("test4").toInt(), (int)Qt::AlignRight);

    delete object;
}

void tst_qmlqt::rgba()
{
    QmlComponent component(&engine, TEST_FILE("rgba.qml"));

    QString warning1 = component.url().toString() + ":6: Unable to assign null to QColor";
    QString warning2 = component.url().toString() + ":7: Unable to assign null to QColor";
    QString warning3 = component.url().toString() + ":8: Unable to assign null to QColor";
    QString warning4 = component.url().toString() + ":9: Unable to assign null to QColor";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning3));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning4));

    QObject *object = component.create();
    QVERIFY(object != 0);


    QCOMPARE(qvariant_cast<QColor>(object->property("test1")), QColor::fromRgbF(1, 0, 0, 0.8));
    QCOMPARE(qvariant_cast<QColor>(object->property("test2")), QColor::fromRgbF(1, 0.5, 0.3, 1));
    QCOMPARE(qvariant_cast<QColor>(object->property("test3")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test4")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test5")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test6")), QColor());

    delete object;
}

void tst_qmlqt::hsla()
{
    QmlComponent component(&engine, TEST_FILE("hsla.qml"));

    QString warning1 = component.url().toString() + ":6: Unable to assign null to QColor";
    QString warning2 = component.url().toString() + ":7: Unable to assign null to QColor";
    QString warning3 = component.url().toString() + ":8: Unable to assign null to QColor";
    QString warning4 = component.url().toString() + ":9: Unable to assign null to QColor";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning3));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning4));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QColor>(object->property("test1")), QColor::fromHslF(1, 0, 0, 0.8));
    QCOMPARE(qvariant_cast<QColor>(object->property("test2")), QColor::fromHslF(1, 0.5, 0.3, 1));
    QCOMPARE(qvariant_cast<QColor>(object->property("test3")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test4")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test5")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test6")), QColor());

    delete object;
}

void tst_qmlqt::rect()
{
    QmlComponent component(&engine, TEST_FILE("rect.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QRectF>(object->property("test1")), QRectF(10, 13, 100, 109));
    QCOMPARE(qvariant_cast<QRectF>(object->property("test2")), QRectF(-10, 13, 100, 109.6));
    QCOMPARE(qvariant_cast<QRectF>(object->property("test3")), QRectF());
    QCOMPARE(qvariant_cast<QRectF>(object->property("test4")), QRectF());
    QCOMPARE(qvariant_cast<QRectF>(object->property("test5")), QRectF());

    delete object;
}

void tst_qmlqt::point()
{
    QmlComponent component(&engine, TEST_FILE("point.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QPointF>(object->property("test1")), QPointF(19, 34));
    QCOMPARE(qvariant_cast<QPointF>(object->property("test2")), QPointF(-3, 109.2));
    QCOMPARE(qvariant_cast<QPointF>(object->property("test3")), QPointF());
    QCOMPARE(qvariant_cast<QPointF>(object->property("test4")), QPointF());

    delete object;
}

void tst_qmlqt::size()
{
    QmlComponent component(&engine, TEST_FILE("size.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QSizeF>(object->property("test1")), QSizeF(19, 34));
    QCOMPARE(qvariant_cast<QSizeF>(object->property("test2")), QSizeF(3, 109.2));
    QCOMPARE(qvariant_cast<QSizeF>(object->property("test3")), QSizeF(-3, 10));
    QCOMPARE(qvariant_cast<QSizeF>(object->property("test4")), QSizeF());
    QCOMPARE(qvariant_cast<QSizeF>(object->property("test5")), QSizeF());

    delete object;
}

void tst_qmlqt::vector()
{
    QmlComponent component(&engine, TEST_FILE("vector.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QVector3D>(object->property("test1")), QVector3D(1, 0, 0.9));
    QCOMPARE(qvariant_cast<QVector3D>(object->property("test2")), QVector3D(102, -10, -982.1));
    QCOMPARE(qvariant_cast<QVector3D>(object->property("test3")), QVector3D());
    QCOMPARE(qvariant_cast<QVector3D>(object->property("test4")), QVector3D());

    delete object;
}

void tst_qmlqt::lighter()
{
    QmlComponent component(&engine, TEST_FILE("lighter.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QColor>(object->property("test1")), QColor::fromRgbF(1, 0.8, 0.3).lighter());
    QCOMPARE(qvariant_cast<QColor>(object->property("test2")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test3")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test4")), QColor("red").lighter());
    QCOMPARE(qvariant_cast<QColor>(object->property("test5")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test6")), QColor());

    delete object;
}

void tst_qmlqt::darker()
{
    QmlComponent component(&engine, TEST_FILE("darker.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QColor>(object->property("test1")), QColor::fromRgbF(1, 0.8, 0.3).darker());
    QCOMPARE(qvariant_cast<QColor>(object->property("test2")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test3")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test4")), QColor("red").darker());
    QCOMPARE(qvariant_cast<QColor>(object->property("test5")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test6")), QColor());

    delete object;
}

void tst_qmlqt::tint()
{
    QmlComponent component(&engine, TEST_FILE("tint.qml"));

    QString warning1 = component.url().toString() + ":7: Unable to assign null to QColor";
    QString warning2 = component.url().toString() + ":8: Unable to assign null to QColor";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<QColor>(object->property("test1")), QColor::fromRgbF(0, 0, 1));
    QCOMPARE(qvariant_cast<QColor>(object->property("test2")), QColor::fromRgbF(1, 0, 0));
    QEXPECT_FAIL("", "QT-2424",Continue);
    QCOMPARE(qvariant_cast<QColor>(object->property("test3")), QColor::fromRgbF(1, 0, 0)); 
    QCOMPARE(qvariant_cast<QColor>(object->property("test4")), QColor());
    QCOMPARE(qvariant_cast<QColor>(object->property("test5")), QColor());

    delete object;
}

void tst_qmlqt::closestAngle()
{
    QmlComponent component(&engine, TEST_FILE("closestangle.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(qvariant_cast<qreal>(object->property("testSame")), 1.0);
    QCOMPARE(qvariant_cast<qreal>(object->property("testLess")), 1.0);
    QCOMPARE(qvariant_cast<qreal>(object->property("testMore")), 1.0);
    QCOMPARE(qvariant_cast<qreal>(object->property("testFail")), 0.0);
    QCOMPARE(qvariant_cast<qreal>(object->property("test5")), 1.0);
    QCOMPARE(qvariant_cast<qreal>(object->property("test6")), 1.11);
    QCOMPARE(qvariant_cast<qreal>(object->property("test7")), 1.11);

    delete object;
}

void tst_qmlqt::playSound()
{
    QEXPECT_FAIL("", "How do we test this?", Abort);
    QVERIFY(false);
}

void tst_qmlqt::openUrlExternally()
{
    QEXPECT_FAIL("", "How do we test this?", Abort);
    QVERIFY(false);
}

void tst_qmlqt::md5()
{
    QmlComponent component(&engine, TEST_FILE("md5.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("test1").toString(), QLatin1String(QCryptographicHash::hash(QByteArray(), QCryptographicHash::Md5).toHex()));
    QCOMPARE(object->property("test2").toString(), QLatin1String(QCryptographicHash::hash("Hello World", QCryptographicHash::Md5).toHex()));

    delete object;
}

void tst_qmlqt::createComponent()
{
    QmlComponent component(&engine, TEST_FILE("createComponent.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("incorrectArgCount1").toBool(), true);
    QCOMPARE(object->property("incorrectArgCount2").toBool(), true);
    QCOMPARE(object->property("emptyArg").toBool(), true);

    QCOMPARE(object->property("absoluteUrl").toString(), QString("http://www.example.com/test.qml"));
    QCOMPARE(object->property("relativeUrl").toString(), TEST_FILE("createComponentData.qml").toString());

    delete object;
}

void tst_qmlqt::createQmlObject()
{
    QmlComponent component(&engine, TEST_FILE("createQmlObject.qml"));

    QString warning1 = "QmlEngine::createQmlObject():";
    QString warning2 = "    " + TEST_FILE("main.qml").toString() + ":4:1: Duplicate property name";
    QString warning3 = "QmlEngine::createQmlObject(): Component is not ready";
    QString warning4 = "QmlEngine::createQmlObject():";
    QString warning5 = "    " + TEST_FILE("inline").toString() + ":3: Cannot assign object type QObject with no default method";

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning3));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning4));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning5));

    QObject *object = component.create();
    QVERIFY(object != 0);

    QCOMPARE(object->property("incorrectArgCount1").toBool(), true);
    QCOMPARE(object->property("incorrectArgCount2").toBool(), true);
    QCOMPARE(object->property("emptyArg").toBool(), true);
    QCOMPARE(object->property("errors").toBool(), true);
    QCOMPARE(object->property("noParent").toBool(), true);
    QCOMPARE(object->property("notReady").toBool(), true);
    QCOMPARE(object->property("runtimeError").toBool(), true);
    QCOMPARE(object->property("success").toBool(), true);

    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(object);
    QVERIFY(item != 0);
    QVERIFY(item->childItems().count() == 1);

    delete object;
}

void tst_qmlqt::consoleLog()
{
    QTest::ignoreMessage(QtDebugMsg, "completed ok");
    QTest::ignoreMessage(QtDebugMsg, "completed ok");
    QmlComponent component(&engine, TEST_FILE("consoleLog.qml"));
    QObject *object = component.create();
    QVERIFY(object != 0);
    delete object;
}

QTEST_MAIN(tst_qmlqt)

#include "tst_qmlqt.moc"
