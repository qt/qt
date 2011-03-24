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
#include "testtypes.h"
#include <QWidget>
#include <QPlainTextEdit>
#include <QDeclarativeEngine>
#include <QScriptEngine>

static QScriptValue script_api(QDeclarativeEngine *engine, QScriptEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    static int testProperty = 13;
    QScriptValue v = scriptEngine->newObject();
    v.setProperty("scriptTestProperty", testProperty++);
    return v;
}

static QObject *qobject_api(QDeclarativeEngine *engine, QScriptEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    testQObjectApi *o = new testQObjectApi();
    o->setQObjectTestProperty(20);
    return o;
}

static QObject *qobject_api_engine_parent(QDeclarativeEngine *engine, QScriptEngine *scriptEngine)
{
    Q_UNUSED(scriptEngine)

    static int testProperty = 26;
    testQObjectApi *o = new testQObjectApi(engine);
    o->setQObjectTestProperty(testProperty++);
    return o;
}

void registerTypes()
{
    qmlRegisterType<MyQmlObject>("Qt.test", 1,0, "MyQmlObjectAlias");
    qmlRegisterType<MyQmlObject>("Qt.test", 1,0, "MyQmlObject");

    qmlRegisterType<QPlainTextEdit>("Qt.test",1,0,"QPlainTextEdit");
    qRegisterMetaType<MyQmlObject::MyType>("MyQmlObject::MyType");

    qmlRegisterType<ScarceResourceProvider>("Qt.test", 1,0, "MyScarceResourceProvider");
    qmlRegisterType<ArbitraryVariantProvider>("Qt.test", 1,0, "MyArbitraryVariantProvider");

    qmlRegisterModuleApi("Qt.test",1,0,script_api);             // register (script) module API for an existing uri which contains elements
    qmlRegisterModuleApi("Qt.test",1,0,qobject_api);            // register (qobject) for an existing uri for which another module API was previously regd.  Should replace!
    qmlRegisterModuleApi("Qt.test.scriptApi",1,0,script_api);   // register (script) module API for a uri which doesn't contain elements
    qmlRegisterModuleApi("Qt.test.qobjectApi",1,0,qobject_api); // register (qobject) module API for a uri which doesn't contain elements
    qmlRegisterModuleApi("Qt.test.qobjectApi",1,3,qobject_api); // register (qobject) module API for a uri which doesn't contain elements, minor version set
    qmlRegisterModuleApi("Qt.test.qobjectApi",2,0,qobject_api); // register (qobject) module API for a uri which doesn't contain elements, major version set
    qmlRegisterModuleApi("Qt.test.qobjectApiParented",1,0,qobject_api_engine_parent); // register (parented qobject) module API for a uri which doesn't contain elements
}

//#include "testtypes.moc"
