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
#include <QtCore/qmetatype.h>

//TESTED_FILES=

class tst_QMetaType : public QObject
{
    Q_OBJECT

public:
    tst_QMetaType();
    virtual ~tst_QMetaType();

private slots:
    void typeBuiltin_data();
    void typeBuiltin();
    void typeBuiltinNotNormalized_data();
    void typeBuiltinNotNormalized();
    void typeCustom();
    void typeCustomNotNormalized();
    void typeNotRegistered();
    void typeNotRegisteredNotNormalized();

    void typeNameBuiltin_data();
    void typeNameBuiltin();
    void typeNameCustom();
    void typeNameNotRegistered();

    void isRegisteredBuiltin_data();
    void isRegisteredBuiltin();
    void isRegisteredCustom();
    void isRegisteredNotRegistered();
};

tst_QMetaType::tst_QMetaType()
{
}

tst_QMetaType::~tst_QMetaType()
{
}

void tst_QMetaType::typeBuiltin_data()
{
    QTest::addColumn<QByteArray>("typeName");
    for (int i = 0; i < QMetaType::User; ++i) {
        const char *name = QMetaType::typeName(i);
        if (name)
            QTest::newRow(name) << QByteArray(name);
    }
}

void tst_QMetaType::typeBuiltin()
{
    QFETCH(QByteArray, typeName);
    const char *nm = typeName.constData();
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            QMetaType::type(nm);
    }
}

void tst_QMetaType::typeBuiltinNotNormalized_data()
{
    QTest::addColumn<QByteArray>("typeName");
    for (int i = 0; i < QMetaType::User; ++i) {
        const char *name = QMetaType::typeName(i);
        if (name)
            QTest::newRow(name) << QByteArray(name).append(" ");
    }
}

void tst_QMetaType::typeBuiltinNotNormalized()
{
    QFETCH(QByteArray, typeName);
    const char *nm = typeName.constData();
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            QMetaType::type(nm);
    }
}

struct Foo { int i; };

void tst_QMetaType::typeCustom()
{
    qRegisterMetaType<Foo>("Foo");
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            QMetaType::type("Foo");
    }
}

void tst_QMetaType::typeCustomNotNormalized()
{
    qRegisterMetaType<Foo>("Foo");
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            QMetaType::type("Foo ");
    }
}

void tst_QMetaType::typeNotRegistered()
{
    Q_ASSERT(QMetaType::type("Bar") == 0);
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            QMetaType::type("Bar");
    }
}

void tst_QMetaType::typeNotRegisteredNotNormalized()
{
    Q_ASSERT(QMetaType::type("Bar") == 0);
    QBENCHMARK {
        for (int i = 0; i < 10000; ++i)
            QMetaType::type("Bar ");
    }
}

void tst_QMetaType::typeNameBuiltin_data()
{
    QTest::addColumn<int>("type");
    for (int i = 0; i < QMetaType::User; ++i) {
        const char *name = QMetaType::typeName(i);
        if (name)
            QTest::newRow(name) << i;
    }
}

void tst_QMetaType::typeNameBuiltin()
{
    QFETCH(int, type);
    QBENCHMARK {
        for (int i = 0; i < 500000; ++i)
            QMetaType::typeName(type);
    }
}

void tst_QMetaType::typeNameCustom()
{
    int type = qRegisterMetaType<Foo>("Foo");
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            QMetaType::typeName(type);
    }
}

void tst_QMetaType::typeNameNotRegistered()
{
    // We don't care much about this case, but test it anyway.
    Q_ASSERT(QMetaType::typeName(-1) == 0);
    QBENCHMARK {
        for (int i = 0; i < 500000; ++i)
            QMetaType::typeName(-1);
    }
}

void tst_QMetaType::isRegisteredBuiltin_data()
{
    typeNameBuiltin_data();
}

void tst_QMetaType::isRegisteredBuiltin()
{
    QFETCH(int, type);
    QBENCHMARK {
        for (int i = 0; i < 500000; ++i)
            QMetaType::isRegistered(type);
    }
}

void tst_QMetaType::isRegisteredCustom()
{
    int type = qRegisterMetaType<Foo>("Foo");
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            QMetaType::isRegistered(type);
    }
}

void tst_QMetaType::isRegisteredNotRegistered()
{
    Q_ASSERT(QMetaType::typeName(-1) == 0);
    QBENCHMARK {
        for (int i = 0; i < 100000; ++i)
            QMetaType::isRegistered(-1);
    }
}

QTEST_MAIN(tst_QMetaType)
#include "tst_qmetatype.moc"
