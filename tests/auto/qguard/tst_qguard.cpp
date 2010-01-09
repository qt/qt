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

// NOTE: This is identical to the QPointer autotest

#include <QtTest/QtTest>

#include <QApplication>
#include <QDebug>
#include <QWidget>
#include <private/qguard_p.h>

class tst_QGuard : public QObject
{
    Q_OBJECT
public:
    tst_QGuard();
    ~tst_QGuard();

    inline tst_QGuard *me() const
    { return const_cast<tst_QGuard *>(this); }

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

tst_QGuard::tst_QGuard()
{ }

tst_QGuard::~tst_QGuard()
{ }

void tst_QGuard::initTestCase()
{ }

void tst_QGuard::cleanupTestCase()
{ }

void tst_QGuard::init()
{ }

void tst_QGuard::cleanup()
{ }

void tst_QGuard::constructors()
{
    QGuard<QObject> p1;
    QGuard<QObject> p2(this);
    QGuard<QObject> p3(p2);
    QCOMPARE(p1, QGuard<QObject>(0));
    QCOMPARE(p2, QGuard<QObject>(this));
    QCOMPARE(p3, QGuard<QObject>(this));
}

void tst_QGuard::destructor()
{
    QObject *object = new QObject;
    QGuard<QObject> p = object;
    QCOMPARE(p, QGuard<QObject>(object));
    delete object;
    QCOMPARE(p, QGuard<QObject>(0));
}

void tst_QGuard::assignment_operators()
{
    QGuard<QObject> p1;
    QGuard<QObject> p2;

    p1 = this;
    p2 = p1;

    QCOMPARE(p1, QGuard<QObject>(this));
    QCOMPARE(p2, QGuard<QObject>(this));
    QCOMPARE(p1, QGuard<QObject>(p2));

    p1 = 0;
    p2 = p1;
    QCOMPARE(p1, QGuard<QObject>(0));
    QCOMPARE(p2, QGuard<QObject>(0));
    QCOMPARE(p1, QGuard<QObject>(p2));

    QObject *object = new QObject;

    p1 = object;
    p2 = p1;
    QCOMPARE(p1, QGuard<QObject>(object));
    QCOMPARE(p2, QGuard<QObject>(object));
    QCOMPARE(p1, QGuard<QObject>(p2));

    delete object;
    QCOMPARE(p1, QGuard<QObject>(0));
    QCOMPARE(p2, QGuard<QObject>(0));
    QCOMPARE(p1, QGuard<QObject>(p2));
}

void tst_QGuard::equality_operators()
{
    QGuard<QObject> p1;
    QGuard<QObject> p2;

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

void tst_QGuard::isNull()
{
    QGuard<QObject> p1;
    QVERIFY(p1.isNull());
    p1 = this;
    QVERIFY(!p1.isNull());
    p1 = 0;
    QVERIFY(p1.isNull());
}

void tst_QGuard::dereference_operators()
{
    QGuard<tst_QGuard> p1 = this;

    QObject *object = p1->me();
    QVERIFY(object == this);

    QObject &ref = *p1;
    QVERIFY(&ref == this);

    object = static_cast<QObject *>(p1);
    QVERIFY(object == this);
}

void tst_QGuard::disconnect()
{
    QGuard<QObject> p1 = new QObject;
    QVERIFY(!p1.isNull());
    p1->disconnect();
    QVERIFY(!p1.isNull());
    delete static_cast<QObject *>(p1);
    QVERIFY(p1.isNull());
}

class ChildObject : public QObject
{
    QGuard<QObject> guardedPointer;

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
    QGuard<QWidget> guardedPointer;

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
    QGuard<DerivedParent> guardedParentPointer;

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

void tst_QGuard::castDuringDestruction()
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

void tst_QGuard::data() const
{
    /* Check value of a default constructed object. */
    {
        QGuard<QObject> p;
        QCOMPARE(p.data(), static_cast<QObject *>(0));
    }

    /* Check value of a default constructed object. */
    {
        QObject *const object = new QObject();
        QGuard<QObject> p(object);
        QCOMPARE(p.data(), object);
    }
}

void tst_QGuard::dataSignature() const
{
    /* data() should be const. */
    {
        const QGuard<QObject> p;
        p.data();
    }

    /* The return type should be T. */
    {
        const QGuard<QWidget> p;
        /* If the types differs, the QCOMPARE will fail to instansiate. */
        QCOMPARE(p.data(), static_cast<QWidget *>(0));
    }
}

QTEST_MAIN(tst_QGuard)
#include "tst_qguard.moc"
