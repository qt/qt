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


#include <QtTest/QtTest>

#include <QApplication>
#include <QDebug>
#include <QPointer>
#include <QWidget>

class tst_QPointer : public QObject
{
    Q_OBJECT
public:
    tst_QPointer();
    ~tst_QPointer();

    inline tst_QPointer *me() const
    { return const_cast<tst_QPointer *>(this); }

public slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
private slots:
    void constructors();
    void destructor();
    void assignment_operators();
    void equality_operators();
    void isNull();
    void dereference_operators();
    void disconnect();
    void castDuringDestruction();
    void data() const;
    void dataSignature() const;
};

tst_QPointer::tst_QPointer()
{ }

tst_QPointer::~tst_QPointer()
{ }

void tst_QPointer::initTestCase()
{ }

void tst_QPointer::cleanupTestCase()
{ }

void tst_QPointer::init()
{ }

void tst_QPointer::cleanup()
{ }

void tst_QPointer::constructors()
{
    QPointer<QObject> p1;
    QPointer<QObject> p2(this);
    QPointer<QObject> p3(p2);
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(this));
    QCOMPARE(p3, QPointer<QObject>(this));
}

void tst_QPointer::destructor()
{
    QObject *object = new QObject;
    QPointer<QObject> p = object;
    QCOMPARE(p, QPointer<QObject>(object));
    delete object;
    QCOMPARE(p, QPointer<QObject>(0));
}

void tst_QPointer::assignment_operators()
{
    QPointer<QObject> p1;
    QPointer<QObject> p2;

    p1 = this;
    p2 = p1;

    QCOMPARE(p1, QPointer<QObject>(this));
    QCOMPARE(p2, QPointer<QObject>(this));
    QCOMPARE(p1, QPointer<QObject>(p2));

    p1 = 0;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(0));
    QCOMPARE(p1, QPointer<QObject>(p2));

    QObject *object = new QObject;

    p1 = object;
    p2 = p1;
    QCOMPARE(p1, QPointer<QObject>(object));
    QCOMPARE(p2, QPointer<QObject>(object));
    QCOMPARE(p1, QPointer<QObject>(p2));

    delete object;
    QCOMPARE(p1, QPointer<QObject>(0));
    QCOMPARE(p2, QPointer<QObject>(0));
    QCOMPARE(p1, QPointer<QObject>(p2));
}

void tst_QPointer::equality_operators()
{
    QPointer<QObject> p1;
    QPointer<QObject> p2;

    QVERIFY(p1 == p2);

    QObject *object = 0;
    QWidget *widget = 0;

    p1 = object;
    QVERIFY(p1 == p2);
    QVERIFY(p1 == object);
    p2 = object;
    QVERIFY(p2 == p1);
    QVERIFY(p2 == object);

    p1 = this;
    QVERIFY(p1 != p2);
    p2 = p1;
    QVERIFY(p1 == p2);

    // compare to zero
    p1 = 0;
    QVERIFY(p1 == 0);
    QVERIFY(0 == p1);
    QVERIFY(p2 != 0);
    QVERIFY(0 != p2);
    QVERIFY(p1 == object);
    QVERIFY(object == p1);
    QVERIFY(p2 != object);
    QVERIFY(object != p2);
    QVERIFY(p1 == widget);
    QVERIFY(widget == p1);
    QVERIFY(p2 != widget);
    QVERIFY(widget != p2);
}

void tst_QPointer::isNull()
{
    QPointer<QObject> p1;
    QVERIFY(p1.isNull());
    p1 = this;
    QVERIFY(!p1.isNull());
    p1 = 0;
    QVERIFY(p1.isNull());
}

void tst_QPointer::dereference_operators()
{
    QPointer<tst_QPointer> p1 = this;

    QObject *object = p1->me();
    QVERIFY(object == this);

    QObject &ref = *p1;
    QVERIFY(&ref == this);

    object = static_cast<QObject *>(p1);
    QVERIFY(object == this);
}

void tst_QPointer::disconnect()
{
    QPointer<QObject> p1 = new QObject;
    QVERIFY(!p1.isNull());
    p1->disconnect();
    QVERIFY(!p1.isNull());
    delete static_cast<QObject *>(p1);
    QVERIFY(p1.isNull());
}

class ChildObject : public QObject
{
    QPointer<QObject> guardedPointer;

public:
    ChildObject(QObject *parent)
        : QObject(parent), guardedPointer(parent)
    { }
    ~ChildObject();
};

ChildObject::~ChildObject()
{
    QCOMPARE(static_cast<QObject *>(guardedPointer), static_cast<QObject *>(0));
    QCOMPARE(qobject_cast<QObject *>(guardedPointer), static_cast<QObject *>(0));
}

class ChildWidget : public QWidget
{
    QPointer<QWidget> guardedPointer;

public:
    ChildWidget(QWidget *parent)
        : QWidget(parent), guardedPointer(parent)
    { }
    ~ChildWidget();
};

ChildWidget::~ChildWidget()
{
    QCOMPARE(static_cast<QWidget *>(guardedPointer), static_cast<QWidget *>(0));
    QCOMPARE(qobject_cast<QWidget *>(guardedPointer), static_cast<QWidget *>(0));
}

class DerivedChild;

class DerivedParent : public QObject
{
    Q_OBJECT

    DerivedChild *derivedChild;

public:
    DerivedParent();
    ~DerivedParent();
};

class DerivedChild : public QObject
{
    Q_OBJECT

    DerivedParent *parentPointer;
    QPointer<DerivedParent> guardedParentPointer;

public:
    DerivedChild(DerivedParent *parent)
        : QObject(parent), parentPointer(parent), guardedParentPointer(parent)
    { }
    ~DerivedChild();
};

DerivedParent::DerivedParent()
    : QObject()
{
    derivedChild = new DerivedChild(this);
}

DerivedParent::~DerivedParent()
{
    delete derivedChild;
}

DerivedChild::~DerivedChild()
{
    QCOMPARE(static_cast<DerivedParent *>(guardedParentPointer), parentPointer);
    QCOMPARE(qobject_cast<DerivedParent *>(guardedParentPointer), parentPointer);
}

void tst_QPointer::castDuringDestruction()
{
    {
        QObject *parentObject = new QObject();
        (void) new ChildObject(parentObject);
        delete parentObject;
    }

    {
        QWidget *parentWidget = new QWidget();
        (void) new ChildWidget(parentWidget);
        delete parentWidget;
    }

    {
        delete new DerivedParent();
    }
}

void tst_QPointer::data() const
{
    /* Check value of a default constructed object. */
    {
        QPointer<QObject> p;
        QCOMPARE(p.data(), static_cast<QObject *>(0));
    }

    /* Check value of a default constructed object. */
    {
        QObject *const object = new QObject();
        QPointer<QObject> p(object);
        QCOMPARE(p.data(), object);
    }
}

void tst_QPointer::dataSignature() const
{
    /* data() should be const. */
    {
        const QPointer<QObject> p;
        p.data();
    }

    /* The return type should be T. */
    {
        const QPointer<QWidget> p;
        /* If the types differs, the QCOMPARE will fail to instansiate. */
        QCOMPARE(p.data(), static_cast<QWidget *>(0));
    }
}

QTEST_MAIN(tst_QPointer)
#include "tst_qpointer.moc"
