/****************************************************************************
**
** Copyright (C) 2012 Research In Motion
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <qdeclarative.h>
#include <QMetaMethod>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif


class ExportedClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int selfProp READ selfProp NOTIFY selfPropChanged)
    Q_PROPERTY(int qmlObjectProp READ qmlObjectProp NOTIFY qmlObjectPropChanged)
    Q_PROPERTY(int cppObjectProp READ cppObjectProp NOTIFY cppObjectPropChanged)
    Q_PROPERTY(int unboundProp READ unboundProp NOTIFY unboundPropChanged)
    Q_PROPERTY(int normalBindingProp READ normalBindingProp NOTIFY normalBindingPropChanged)
    Q_PROPERTY(int compiledBindingProp READ compiledBindingProp NOTIFY compiledBindingPropChanged)
    Q_PROPERTY(int compiledBindingPropShared READ compiledBindingPropShared NOTIFY compiledBindingPropSharedChanged)
public:
    int selfPropConnections;
    int qmlObjectPropConnections;
    int cppObjectPropConnections;
    int unboundPropConnections;
    int normalBindingPropConnections;
    int compiledBindingPropConnections;
    int compiledBindingPropSharedConnections;
    int boundSignalConnections;
    int unusedSignalConnections;

    ExportedClass()
        : selfPropConnections(0), qmlObjectPropConnections(0), cppObjectPropConnections(0),
          unboundPropConnections(0), normalBindingPropConnections(0), compiledBindingPropConnections(0),
          compiledBindingPropSharedConnections(0), boundSignalConnections(0),
          unusedSignalConnections(0)
    {}

    ~ExportedClass()
    {
        QCOMPARE(selfPropConnections, 0);
        QCOMPARE(qmlObjectPropConnections, 0);
        QCOMPARE(cppObjectPropConnections, 0);
        QCOMPARE(unboundPropConnections, 0);
        QCOMPARE(normalBindingPropConnections, 0);
        QCOMPARE(compiledBindingPropConnections, 0);
        QCOMPARE(compiledBindingPropSharedConnections, 0);
        QCOMPARE(boundSignalConnections, 0);
        QCOMPARE(unusedSignalConnections, 0);
    }

    int selfProp() const { return 42; }
    int qmlObjectProp() const { return 42; }
    int unboundProp() const { return 42; }
    int normalBindingProp() const { return 42; }
    int compiledBindingProp() const { return 42; }
    int compiledBindingPropShared() const { return 42; }
    int cppObjectProp() const { return 42; }
protected:
    void connectNotify(const char *signal)
    {
        const QString signalName(signal);
        if (signalName == "selfPropChanged()") selfPropConnections++;
        if (signalName == "qmlObjectPropChanged()") qmlObjectPropConnections++;
        if (signalName == "cppObjectPropChanged()") cppObjectPropConnections++;
        if (signalName == "unboundPropChanged()") unboundPropConnections++;
        if (signalName == "normalBindingPropChanged()") normalBindingPropConnections++;
        if (signalName == "compiledBindingPropChanged()") compiledBindingPropConnections++;
        if (signalName == "compiledBindingPropSharedChanged()") compiledBindingPropSharedConnections++;
        if (signalName == "boundSignal()")   boundSignalConnections++;
        if (signalName == "unusedSignal()") unusedSignalConnections++;
        //qDebug() << Q_FUNC_INFO << this << signalName;
    }

    void disconnectNotify(const char *signal)
    {
        const QString signalName(signal);
        if (signalName == "selfPropChanged()") selfPropConnections--;
        if (signalName == "qmlObjectPropChanged()") qmlObjectPropConnections--;
        if (signalName == "cppObjectPropChanged()") cppObjectPropConnections--;
        if (signalName == "unboundPropChanged()") unboundPropConnections--;
        if (signalName == "normalBindingPropChanged()") normalBindingPropConnections--;
        if (signalName == "compiledBindingPropChanged()") compiledBindingPropConnections--;
        if (signalName == "compiledBindingPropSharedChanged()") compiledBindingPropSharedConnections--;
        if (signalName == "boundSignal()")   boundSignalConnections--;
        if (signalName == "unusedSignal()") unusedSignalConnections--;
        //qDebug() << Q_FUNC_INFO << this << signalName;
    }

signals:
    void selfPropChanged();
    void qmlObjectPropChanged();
    void cppObjectPropChanged();
    void unboundPropChanged();
    void normalBindingPropChanged();
    void compiledBindingPropChanged();
    void compiledBindingPropSharedChanged();
    void boundSignal();
    void unusedSignal();
};

class tst_qdeclarativenotifier : public QObject
{
    Q_OBJECT
public:
    tst_qdeclarativenotifier()
        : root(0), exportedClass(0), exportedObject(0)
    {}

private slots:
    void initTestCase();
    void cleanupTestCase();
    void connectNotify();

    void removeCompiledBinding();
    void removeCompiledBindingShared();
    void removeNormalBinding();
    // No need to test value type proxy bindings - the user can't override disconnectNotify() anyway,
    // as the classes are private to the QML engine

    void readProperty();
    void propertyChange();
    void disconnectOnDestroy();

private:
    void createObjects();

    QDeclarativeEngine engine;
    QObject *root;
    ExportedClass *exportedClass;
    ExportedClass *exportedObject;
};

void tst_qdeclarativenotifier::initTestCase()
{
    qmlRegisterType<ExportedClass>("Test", 1, 0, "ExportedClass");
}

void tst_qdeclarativenotifier::createObjects()
{
    delete root;
    root = 0;
    exportedClass = exportedObject = 0;

    QDeclarativeComponent component(&engine, QUrl::fromLocalFile(SRCDIR "/data/connectnotify.qml"));
    exportedObject = new ExportedClass();
    exportedObject->setObjectName("exportedObject");
    engine.rootContext()->setContextProperty("_exportedObject", exportedObject);
    root = component.create();
    QVERIFY(root != 0);
    exportedClass = qobject_cast<ExportedClass *>(
                root->findChild<ExportedClass*>("exportedClass"));
    QVERIFY(exportedClass != 0);
}

void tst_qdeclarativenotifier::cleanupTestCase()
{
    delete root;
    root = 0;
    delete exportedObject;
    exportedObject = 0;
}

void tst_qdeclarativenotifier::connectNotify()
{
    createObjects();

    QCOMPARE(exportedClass->selfPropConnections, 1);
    QCOMPARE(exportedClass->qmlObjectPropConnections, 1);
    QCOMPARE(exportedClass->cppObjectPropConnections, 0);
    QCOMPARE(exportedClass->unboundPropConnections, 0);
    QCOMPARE(exportedClass->normalBindingPropConnections, 1);
    QCOMPARE(exportedClass->compiledBindingPropConnections, 1);
    QCOMPARE(exportedClass->compiledBindingPropSharedConnections, 1);
    QCOMPARE(exportedClass->boundSignalConnections, 1);
    QCOMPARE(exportedClass->unusedSignalConnections, 0);

    QCOMPARE(exportedObject->selfPropConnections, 0);
    QCOMPARE(exportedObject->qmlObjectPropConnections, 0);
    QCOMPARE(exportedObject->cppObjectPropConnections, 1);
    QCOMPARE(exportedObject->unboundPropConnections, 0);
    QCOMPARE(exportedObject->normalBindingPropConnections, 0);
    QCOMPARE(exportedObject->compiledBindingPropConnections, 0);
    QCOMPARE(exportedObject->compiledBindingPropSharedConnections, 0);
    QCOMPARE(exportedObject->boundSignalConnections, 0);
    QCOMPARE(exportedObject->unusedSignalConnections, 0);
}

void tst_qdeclarativenotifier::removeCompiledBinding()
{
    createObjects();

    // Removing a binding should disconnect all of its guarded properties
    QVERIFY(QMetaObject::invokeMethod(root, "removeCompiledBinding"));
    QCOMPARE(exportedClass->compiledBindingPropConnections, 0);
}

void tst_qdeclarativenotifier::removeCompiledBindingShared()
{
    createObjects();

    // In this case, the compiledBindingPropShared property is used by two compiled bindings.
    // Make sure that removing one binding doesn't by accident disconnect all. Behind the scenes,
    // the subscription is shared between multiple bindings.
    QVERIFY(QMetaObject::invokeMethod(root, "removeCompiledBindingShared_1"));
    QCOMPARE(exportedClass->compiledBindingPropSharedConnections, 1);

    // Removing the second binding should trigger a disconnect now.
    QVERIFY(QMetaObject::invokeMethod(root, "removeCompiledBindingShared_2"));
    QCOMPARE(exportedClass->compiledBindingPropSharedConnections, 0);
}

void tst_qdeclarativenotifier::removeNormalBinding()
{
    createObjects();

    // Removing a binding should disconnect all of its guarded properties
    QVERIFY(QMetaObject::invokeMethod(root, "removeNormalBinding"));
    QCOMPARE(exportedClass->normalBindingPropConnections, 0);
}

void tst_qdeclarativenotifier::readProperty()
{
    createObjects();

    // Reading a property should not connect to it
    QVERIFY(QMetaObject::invokeMethod(root, "readProperty"));
    QCOMPARE(exportedClass->unboundPropConnections, 0);
}

void tst_qdeclarativenotifier::propertyChange()
{
    createObjects();

    // Changing the state will trigger the PropertyChange to overwrite a value with a binding.
    // For this, the new binding needs to be connected, and afterwards disconnected.
    QVERIFY(QMetaObject::invokeMethod(root, "changeState"));
    QCOMPARE(exportedClass->unboundPropConnections, 1);
    QVERIFY(QMetaObject::invokeMethod(root, "changeState"));
    QCOMPARE(exportedClass->unboundPropConnections, 0);
}

void tst_qdeclarativenotifier::disconnectOnDestroy()
{
    createObjects();

    // Deleting a QML object should remove all connections. For exportedClass, this is tested in
    // the destructor, and for exportedObject, it is tested below.
    delete root;
    root = 0;
    QCOMPARE(exportedObject->cppObjectPropConnections, 0);
}

QTEST_MAIN(tst_qdeclarativenotifier)

#include "tst_qdeclarativenotifier.moc"

