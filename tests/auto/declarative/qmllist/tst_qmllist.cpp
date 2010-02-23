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
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include <QmlEngine>
#include <QmlComponent>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlprivate.h>
#include <QDebug>

inline QUrl TEST_FILE(const QString &filename)
{
    QFileInfo fileInfo(__FILE__);
    return QUrl::fromLocalFile(fileInfo.absoluteDir().filePath("data/" + filename));
}

inline QUrl TEST_FILE(const char *filename)
{
    return TEST_FILE(QLatin1String(filename));
}

class tst_QmlList : public QObject
{
    Q_OBJECT
public:
    tst_QmlList() {}

private slots:
    void qmllistreference();
    void qmllistreference_invalid();
    void isValid();
    void object();
    void listElementType();
    void canAppend();
    void canAt();
    void canClear();
    void canCount();
    void append();
    void at();
    void clear();
    void count();
    void copy();
    void qmlmetaproperty();
    void engineTypes();
    void variantToList();
};

class TestType : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QmlListProperty<TestType> data READ dataProperty);
    Q_PROPERTY(int intProperty READ intProperty);

public:
    TestType() : property(this, data) {}
    QmlListProperty<TestType> dataProperty() { return property; }
    int intProperty() const { return 10; }

    QList<TestType *> data;
    QmlListProperty<TestType> property;
};
QML_DECLARE_TYPE(TestType);
QML_DEFINE_NOCREATE_TYPE(TestType);

void tst_QmlList::qmllistreference()
{
    TestType tt;

    QmlListReference r(&tt, "data");
    QVERIFY(r.isValid() == true);
    QCOMPARE(r.count(), 0);

    tt.data.append(&tt);
    QCOMPARE(r.count(), 1);
}

void tst_QmlList::qmllistreference_invalid()
{
    TestType tt;

    // Invalid
    {
    QmlListReference r;
    QVERIFY(r.isValid() == false);
    QVERIFY(r.object() == 0);
    QVERIFY(r.listElementType() == 0);
    QVERIFY(r.canAt() == false);
    QVERIFY(r.canClear() == false);
    QVERIFY(r.canCount() == false);
    QVERIFY(r.append(0) == false);
    QVERIFY(r.at(10) == 0);
    QVERIFY(r.clear() == false);
    QVERIFY(r.count() == 0);
    }

    // Non-property
    {
    QmlListReference r(&tt, "blah");
    QVERIFY(r.isValid() == false);
    QVERIFY(r.object() == 0);
    QVERIFY(r.listElementType() == 0);
    QVERIFY(r.canAt() == false);
    QVERIFY(r.canClear() == false);
    QVERIFY(r.canCount() == false);
    QVERIFY(r.append(0) == false);
    QVERIFY(r.at(10) == 0);
    QVERIFY(r.clear() == false);
    QVERIFY(r.count() == 0);
    }

    // Non-list property
    {
    QmlListReference r(&tt, "intProperty");
    QVERIFY(r.isValid() == false);
    QVERIFY(r.object() == 0);
    QVERIFY(r.listElementType() == 0);
    QVERIFY(r.canAt() == false);
    QVERIFY(r.canClear() == false);
    QVERIFY(r.canCount() == false);
    QVERIFY(r.append(0) == false);
    QVERIFY(r.at(10) == 0);
    QVERIFY(r.clear() == false);
    QVERIFY(r.count() == 0);
    }
}

void tst_QmlList::isValid()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.isValid() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.isValid() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.isValid() == true);
    delete tt;
    QVERIFY(ref.isValid() == false);
    }
}

void tst_QmlList::object()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.object() == 0);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.object() == 0);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.object() == tt);
    delete tt;
    QVERIFY(ref.object() == 0);
    }
}

void tst_QmlList::listElementType()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.listElementType() == 0);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.listElementType() == 0);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.listElementType() == &TestType::staticMetaObject);
    delete tt;
    QVERIFY(ref.listElementType() == 0);
    }
}

void tst_QmlList::canAppend()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.canAppend() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.canAppend() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.canAppend() == true);
    delete tt;
    QVERIFY(ref.canAppend() == false);
    }

    {
    TestType tt;
    tt.property.append = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.canAppend() == false);
    }
}

void tst_QmlList::canAt()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.canAt() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.canAt() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.canAt() == true);
    delete tt;
    QVERIFY(ref.canAt() == false);
    }

    {
    TestType tt;
    tt.property.at = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.canAt() == false);
    }
}

void tst_QmlList::canClear()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.canClear() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.canClear() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.canClear() == true);
    delete tt;
    QVERIFY(ref.canClear() == false);
    }

    {
    TestType tt;
    tt.property.clear = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.canClear() == false);
    }
}

void tst_QmlList::canCount()
{
    TestType *tt = new TestType;

    {
    QmlListReference ref;
    QVERIFY(ref.canCount() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.canCount() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.canCount() == true);
    delete tt;
    QVERIFY(ref.canCount() == false);
    }

    {
    TestType tt;
    tt.property.count = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.canCount() == false);
    }
}

void tst_QmlList::append()
{
    TestType *tt = new TestType;
    QObject object;

    {
    QmlListReference ref;
    QVERIFY(ref.append(tt) == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.append(tt) == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.append(tt) == true);
    QVERIFY(tt->data.count() == 1);
    QVERIFY(tt->data.at(0) == tt);
    QVERIFY(ref.append(&object) == false);
    QVERIFY(tt->data.count() == 1);
    QVERIFY(tt->data.at(0) == tt);
    QVERIFY(ref.append(0) == true);
    QVERIFY(tt->data.count() == 2);
    QVERIFY(tt->data.at(0) == tt);
    QVERIFY(tt->data.at(1) == 0);
    delete tt;
    QVERIFY(ref.append(0) == false);
    }

    {
    TestType tt;
    tt.property.append = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.append(&tt) == false);
    }
}

void tst_QmlList::at()
{
    TestType *tt = new TestType;
    tt->data.append(tt);
    tt->data.append(0);
    tt->data.append(tt);

    {
    QmlListReference ref;
    QVERIFY(ref.at(0) == 0);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.at(0) == 0);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.at(0) == tt);
    QVERIFY(ref.at(1) == 0);
    QVERIFY(ref.at(2) == tt);
    delete tt;
    QVERIFY(ref.at(0) == 0);
    }

    {
    TestType tt;
    tt.data.append(&tt);
    tt.property.at = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.at(0) == 0);
    }
}

void tst_QmlList::clear()
{
    TestType *tt = new TestType;
    tt->data.append(tt);
    tt->data.append(0);
    tt->data.append(tt);

    {
    QmlListReference ref;
    QVERIFY(ref.clear() == false);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.clear() == false);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.clear() == true);
    QVERIFY(tt->data.count() == 0);
    delete tt;
    QVERIFY(ref.clear() == false);
    }

    {
    TestType tt;
    tt.property.clear = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.clear() == false);
    }
}

void tst_QmlList::count()
{
    TestType *tt = new TestType;
    tt->data.append(tt);
    tt->data.append(0);
    tt->data.append(tt);

    {
    QmlListReference ref;
    QVERIFY(ref.count() == 0);
    }

    {
    QmlListReference ref(tt, "blah");
    QVERIFY(ref.count() == 0);
    }

    {
    QmlListReference ref(tt, "data");
    QVERIFY(ref.count() == 3);
    tt->data.removeAt(1);
    QVERIFY(ref.count() == 2);
    delete tt;
    QVERIFY(ref.count() == 0);
    }

    {
    TestType tt;
    tt.data.append(&tt);
    tt.property.count = 0;
    QmlListReference ref(&tt, "data");
    QVERIFY(ref.count() == 0);
    }
}

void tst_QmlList::copy()
{
    TestType tt;
    tt.data.append(&tt);
    tt.data.append(0);
    tt.data.append(&tt);

    QmlListReference *r1 = new QmlListReference(&tt, "data");
    QVERIFY(r1->count() == 3);

    QmlListReference r2(*r1);
    QmlListReference r3;
    r3 = *r1;

    QVERIFY(r2.count() == 3);
    QVERIFY(r3.count() == 3);

    delete r1;

    QVERIFY(r2.count() == 3);
    QVERIFY(r3.count() == 3);

    tt.data.removeAt(2);

    QVERIFY(r2.count() == 2);
    QVERIFY(r3.count() == 2);
}

void tst_QmlList::qmlmetaproperty()
{
    TestType tt;
    tt.data.append(&tt);
    tt.data.append(0);
    tt.data.append(&tt);

    QmlMetaProperty prop(&tt, QLatin1String("data"));
    QVariant v = prop.read();
    QVERIFY(v.userType() == qMetaTypeId<QmlListReference>());
    QmlListReference ref = qvariant_cast<QmlListReference>(v);
    QVERIFY(ref.count() == 3);
    QVERIFY(ref.listElementType() == &TestType::staticMetaObject);
}

void tst_QmlList::engineTypes()
{
    QmlEngine engine;
    QmlComponent component(&engine, TEST_FILE("engineTypes.qml"));

    QObject *o = component.create();
    QVERIFY(o);

    QmlMetaProperty p1(o, QLatin1String("myList"));
    QVERIFY(p1.propertyCategory() == QmlMetaProperty::Normal);

    QmlMetaProperty p2(o, QLatin1String("myList"), engine.rootContext());
    QVERIFY(p2.propertyCategory() == QmlMetaProperty::List);
    QVariant v = p2.read();
    QVERIFY(v.userType() == qMetaTypeId<QmlListReference>());
    QmlListReference ref = qvariant_cast<QmlListReference>(v);
    QVERIFY(ref.count() == 2);
    QVERIFY(ref.listElementType());
    QVERIFY(ref.listElementType() != &QObject::staticMetaObject);

    delete o;
}

void tst_QmlList::variantToList()
{
    QmlEngine engine;
    QmlComponent component(&engine, TEST_FILE("variantToList.qml"));

    QObject *o = component.create();
    QVERIFY(o);

    QVERIFY(o->property("value").userType() == qMetaTypeId<QmlListReference>());
    QCOMPARE(o->property("test").toInt(), 1);

    delete o;
}

QTEST_MAIN(tst_QmlList)

#include "tst_qmllist.moc"
