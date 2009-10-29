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
#include <QtDeclarative/qmlmetaproperty.h>
#include <QtGui/QLineEdit>

class MyQmlObject : public QObject
{
    Q_OBJECT
public:
    MyQmlObject() {}
};

QML_DECLARE_TYPE(MyQmlObject);
QML_DEFINE_TYPE(Test,1,0,0,MyQmlObject,MyQmlObject);

class MyContainer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<MyQmlObject*>* children READ children)
    Q_PROPERTY(QmlList<MyQmlObject*>* qmlChildren READ qmlChildren)
public:
    MyContainer() {}

    QList<MyQmlObject*> *children() { return &m_children; }
    QmlConcreteList<MyQmlObject *> *qmlChildren() { return &m_qmlChildren; }

private:
    QList<MyQmlObject*> m_children;
    QmlConcreteList<MyQmlObject *> m_qmlChildren;
};

QML_DECLARE_TYPE(MyContainer);
QML_DEFINE_TYPE(Test,1,0,0,MyContainer,MyContainer);

class tst_QmlMetaProperty : public QObject
{
    Q_OBJECT
public:
    tst_QmlMetaProperty() {}

private slots:
    void writeObjectToList();
    void writeListToList();
    void writeObjectToQmlList();

    //writeToReadOnly();

private:
    QmlEngine engine;
};

void tst_QmlMetaProperty::writeObjectToList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->children()->size() == 1);

    MyQmlObject *object = new MyQmlObject;
    QmlMetaProperty prop(container, "children");
    prop.write(qVariantFromValue(object));
    QCOMPARE(container->children()->size(), 2);
    QCOMPARE(container->children()->at(1), object);
}

Q_DECLARE_METATYPE(QList<QObject *>);
void tst_QmlMetaProperty::writeListToList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { children: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->children()->size() == 1);

    QList<QObject*> objList;
    objList << new MyQmlObject() << new MyQmlObject() << new MyQmlObject() << new MyQmlObject();
    QmlMetaProperty prop(container, "children");
    prop.write(qVariantFromValue(objList));
    QCOMPARE(container->children()->size(), 4);

    //XXX need to try this with read/write prop (for read-only it correctly doesn't write)
    /*QList<MyQmlObject*> typedObjList;
    typedObjList << new MyQmlObject();
    prop.write(qVariantFromValue(&typedObjList));
    QCOMPARE(container->children()->size(), 1);*/
}

void tst_QmlMetaProperty::writeObjectToQmlList()
{
    QmlComponent containerComponent(&engine, "import Test 1.0\nMyContainer { qmlChildren: MyQmlObject {} }", QUrl());
    MyContainer *container = qobject_cast<MyContainer*>(containerComponent.create());
    QVERIFY(container != 0);
    QVERIFY(container->qmlChildren()->size() == 1);

    MyQmlObject *object = new MyQmlObject;
    QmlMetaProperty prop(container, "qmlChildren");
    prop.write(qVariantFromValue(object));
    QCOMPARE(container->qmlChildren()->size(), 2);
    QCOMPARE(container->qmlChildren()->at(1), object);
}

QTEST_MAIN(tst_QmlMetaProperty)

#include "tst_qmlmetaproperty.moc"
