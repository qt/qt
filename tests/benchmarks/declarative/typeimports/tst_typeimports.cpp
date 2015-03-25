/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <qtest.h>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDebug>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_typeimports : public QObject
{
    Q_OBJECT
public:
    tst_typeimports();

private slots:
    void cpp();
    void qml();

private:
    QDeclarativeEngine engine;
};

class TestType1 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QDeclarativeListProperty<QObject> resources READ resources);
    Q_CLASSINFO("DefaultProperty", "resources");
public:
    TestType1(QObject *parent = 0) : QObject(parent) {}

    QDeclarativeListProperty<QObject> resources() {
        return QDeclarativeListProperty<QObject>(this, 0, resources_append);
    }

    static void resources_append(QDeclarativeListProperty<QObject> *p, QObject *o) {
        o->setParent(p->object);
    }
};

class TestType2 : public TestType1
{
    Q_OBJECT
public:
    TestType2(QObject *parent = 0) : TestType1(parent) {}
};


class TestType3 : public TestType1
{
    Q_OBJECT
public:
    TestType3(QObject *parent = 0) : TestType1(parent) {}
};

class TestType4 : public TestType1
{
    Q_OBJECT
public:
    TestType4(QObject *parent = 0) : TestType1(parent) {}
};


tst_typeimports::tst_typeimports()
{
    qmlRegisterType<TestType1>("Qt.test", 1, 0, "TestType1");
    qmlRegisterType<TestType2>("Qt.test", 1, 0, "TestType2");
    qmlRegisterType<TestType3>("Qt.test", 2, 0, "TestType3");
    qmlRegisterType<TestType4>("Qt.test", 2, 0, "TestType4");
}

inline QUrl TEST_FILE(const QString &filename)
{
    return QUrl::fromLocalFile(QLatin1String(SRCDIR) + QLatin1String("/data/") + filename);
}

void tst_typeimports::cpp()
{
    QBENCHMARK {
        QDeclarativeComponent component(&engine, TEST_FILE("cpp.qml"));
        QVERIFY(component.isReady());
    }
}

void tst_typeimports::qml()
{
    //get rid of initialization effects
    { QDeclarativeComponent component(&engine, TEST_FILE("qml.qml")); }

    QBENCHMARK {
        QDeclarativeComponent component(&engine, TEST_FILE("qml.qml"));
        QVERIFY(component.isReady());
    }
}

QTEST_MAIN(tst_typeimports)

#include "tst_typeimports.moc"
