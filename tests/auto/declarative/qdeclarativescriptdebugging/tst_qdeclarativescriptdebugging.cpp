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
#include <QtDeclarative/qdeclarativecomponent.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeitem.h>
#include <QtDeclarative/qdeclarativecontext.h>
#include <QtCore/QDir>
#include <QtScript/QScriptEngineAgent>
#include <private/qdeclarativeengine_p.h>

class MyTestObject : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString someProperty READ someProperty WRITE setSomeProperty NOTIFY somePropertyChanged)

public:
    QString someProperty() { return _someProperty; }
    void setSomeProperty(const QString &p) { _someProperty = p; }
    QString _someProperty;

    void emitSignal(const QString &value) { emit signaled(value); }

signals:
    void signaled(const QString &value);
    void somePropertyChanged();
};


class BtAgent : public QScriptEngineAgent {
public:
    BtAgent(QScriptEngine *engine) : QScriptEngineAgent(engine)
    { count = 0; engine->setAgent(this); }

    QStringList bt;
    int count;
    int breakpoint;
    void positionChange(qint64 , int lineNumber, int )
    {
        if(lineNumber == breakpoint) {
            count++;
            bt = engine()->currentContext()->backtrace();
        }
    }
};



/*
This test covers evaluation of ECMAScript expressions and bindings from within
QML.  This does not include static QML language issues.

Static QML language issues are covered in qmllanguage
*/
inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath("data/" + filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}

class tst_qdeclarativescriptdebugging : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativescriptdebugging() {}

private slots:
    void initTestCase();
    void backtrace1();
};

void tst_qdeclarativescriptdebugging::initTestCase()
{
        qmlRegisterType<MyTestObject>("Qt.test", 1,0, "MyTestObject");
}

void tst_qdeclarativescriptdebugging::backtrace1()
{
    {
    QDeclarativeEngine engine;
    QUrl file = TEST_FILE("backtrace1.qml");
    QDeclarativeComponent component(&engine, file);
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(component.create());
    QVERIFY(item);
    MyTestObject *obj = item->findChild<MyTestObject *>();
    QVERIFY(obj);
    QCOMPARE(obj->someProperty(), QString("Hello Default"));

    QDeclarativeEnginePrivate *ep = QDeclarativeEnginePrivate::get(&engine);
    BtAgent agent(&ep->scriptEngine);
    agent.breakpoint = 16;

    obj->emitSignal("blah");
    QCOMPARE(obj->someProperty(), QString("Hello blahb"));
    QCOMPARE(agent.count, 1);

    QStringList expected;
    expected << "append(a = 'Hello', b = 'blahb') at @PREFIX@/backtrace1.qml:16"
             << "$someProperty() at @PREFIX@/backtrace1.qml:21"
             << "function2InScript(a = 'blahb') at @PREFIX@/backtrace1.js:4"
             << "functionInScript(a = 'blah', b = 'b') at @PREFIX@/backtrace1.js:10"
             << "onSignaled() at @PREFIX@/backtrace1.qml:24"
             << "<global>() at -1";
    expected.replaceInStrings("@PREFIX@", file.toString().section('/', 0, -2));
    QCOMPARE(agent.bt, expected);
    }
}


QTEST_MAIN(tst_qdeclarativescriptdebugging)

#include "tst_qdeclarativescriptdebugging.moc"
