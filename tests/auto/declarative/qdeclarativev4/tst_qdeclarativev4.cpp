/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtCore/qobject.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qdir.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtCore/qdebug.h>

#include <private/qdeclarativev4compiler_p.h>

#include "testtypes.h"

inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath("data/" + filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}

class tst_qdeclarativev4 : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativev4() {}

private slots:
    void initTestCase();

    void unnecessaryReeval();
    void logicalOr();
    void qtscript();
    void qtscript_data();
    void nestedObjectAccess();
    void subscriptionsInConditionalExpressions();

private:
    QDeclarativeEngine engine;
};

void tst_qdeclarativev4::initTestCase()
{
    registerTypes();
}

static int v4ErrorsMsgCount = 0;
static void v4ErrorsMsgHandler(QtMsgType, const char *message)
{
    QByteArray m(message);
    if (m.contains("QDeclarativeV4"))
        v4ErrorsMsgCount++;
}

void tst_qdeclarativev4::qtscript()
{
    QFETCH(QString, file);
    QDeclarativeV4Compiler::enableBindingsTest(true);

    QDeclarativeComponent component(&engine, TEST_FILE(file));

    v4ErrorsMsgCount = 0;
    QtMsgHandler old = qInstallMsgHandler(v4ErrorsMsgHandler);

    QObject *o = component.create();
    delete o;

    qInstallMsgHandler(old);

    QCOMPARE(v4ErrorsMsgCount, 0);

    QDeclarativeV4Compiler::enableBindingsTest(false);
}

void tst_qdeclarativev4::qtscript_data()
{
    QTest::addColumn<QString>("file");

    QTest::newRow("qreal -> int rounding") << "qrealToIntRounding.qml";
    QTest::newRow("exception on fetch") << "fetchException.qml";
    QTest::newRow("logical or") << "logicalOr.qml";
    QTest::newRow("double bool jump") << "doubleBoolJump.qml";
    QTest::newRow("unary minus") << "unaryMinus.qml";
    QTest::newRow("null qobject") << "nullQObject.qml";
}

void tst_qdeclarativev4::unnecessaryReeval()
{
    QDeclarativeComponent component(&engine, TEST_FILE("unnecessaryReeval.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    ResultObject *ro = qobject_cast<ResultObject *>(o);
    QVERIFY(ro != 0);

    QCOMPARE(ro->resultCounter(),  1);
    QCOMPARE(ro->result(), 19);
    ro->resetResultCounter();

    ro->setProperty("b", 6);

    QCOMPARE(ro->resultCounter(),  1);
    QCOMPARE(ro->result(), 6);
    ro->resetResultCounter();

    ro->setProperty("a", 14);

    QCOMPARE(ro->resultCounter(),  1);
    QCOMPARE(ro->result(), 7);
    ro->resetResultCounter();

    ro->setProperty("b", 14);
    QCOMPARE(ro->resultCounter(),  0);
    QCOMPARE(ro->result(), 7);

    delete o;
}

void tst_qdeclarativev4::logicalOr()
{
    {
        QDeclarativeComponent component(&engine, TEST_FILE("logicalOr.qml"));

        QObject *o = component.create();
        QVERIFY(o != 0);

        ResultObject *ro = qobject_cast<ResultObject *>(o);
        QVERIFY(ro != 0);

        QCOMPARE(ro->result(), 0);
        delete o;
    }

    {
        QDeclarativeComponent component(&engine, TEST_FILE("logicalOr.2.qml"));

        QObject *o = component.create();
        QVERIFY(o != 0);

        ResultObject *ro = qobject_cast<ResultObject *>(o);
        QVERIFY(ro != 0);

        QCOMPARE(ro->result(), 1);
        delete o;
    }
}

// This would previously use the metaObject of the root element to result the nested access.
// That is, the index for accessing "result" would have been RootObject::result, instead of
// NestedObject::result.
void tst_qdeclarativev4::nestedObjectAccess()
{
    QDeclarativeComponent component(&engine, TEST_FILE("nestedObjectAccess.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    ResultObject *ro = qobject_cast<ResultObject *>(o);
    QVERIFY(ro != 0);

    QCOMPARE(ro->result(), 37);

    delete o;
}

void tst_qdeclarativev4::subscriptionsInConditionalExpressions()
{
    QDeclarativeComponent component(&engine, TEST_FILE("subscriptionsInConditionalExpressions.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    QObject *ro = qobject_cast<QObject *>(o);
    QVERIFY(ro != 0);

    QCOMPARE(ro->property("result").toReal(), qreal(2));

    delete o;
}

QTEST_MAIN(tst_qdeclarativev4)

#include "tst_qdeclarativev4.moc"
