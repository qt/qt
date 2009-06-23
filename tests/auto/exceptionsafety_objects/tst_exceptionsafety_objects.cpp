/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui/QtGui>
#include <QtTest/QtTest>

QT_USE_NAMESPACE

// this test only works with GLIBC (let moc run regardless, because it doesn't know about __GLIBC__)
#if defined(QT_NO_EXCEPTIONS) || (!defined(__GLIBC__) && !defined(Q_MOC_RUN))
    QTEST_NOOP_MAIN
#else

#include "oomsimulator.h"
#include "3rdparty/memcheck.h"

class tst_ExceptionSafetyObjects: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();

private slots:
    void objects_data();
    void objects();

    void widgets_data();
    void widgets();
};

// helper structs to create an arbitrary widget
struct AbstractObjectCreator
{
    virtual QObject *create(QObject *parent) = 0;
};

Q_DECLARE_METATYPE(AbstractObjectCreator *)

template <typename T>
struct ObjectCreator : public AbstractObjectCreator
{
    QObject *create(QObject *parent)
    {
        return parent ? new T(parent) : new T;
    }
};

void tst_ExceptionSafetyObjects::objects_data()
{
    QTest::addColumn<AbstractObjectCreator *>("objectCreator");

#define NEWROW(T) QTest::newRow(#T) << static_cast<AbstractObjectCreator *>(new ObjectCreator<T >)
    NEWROW(QObject);
}

// create and destructs an object, and lets each and every allocation
// during construction and destruction fail.
static void doOOMTest(AbstractObjectCreator *creator, QObject *parent)
{
    AllocFailer allocFailer;
    int currentOOMIndex = 0;
    bool caught = false;

    int allocStartIndex = 0;
    int allocEndIndex = 0;
    int lastAllocCount = 0;

    do {
        allocFailer.setAllocFailIndex(++currentOOMIndex);

        caught = false;
        lastAllocCount = allocEndIndex - allocStartIndex;
        allocStartIndex = allocFailer.currentAllocIndex();

        try {
            QScopedPointer<QObject> ptr(creator->create(parent));
        } catch (const std::bad_alloc &) {
            caught = true;
        }

        allocEndIndex = allocFailer.currentAllocIndex();

    } while (caught || allocEndIndex - allocStartIndex != lastAllocCount);

    allocFailer.deactivate();
}

static bool alloc1Failed = false;
static bool alloc2Failed = false;
static bool alloc3Failed = false;
static bool alloc4Failed = false;
static bool malloc1Failed = false;
static bool malloc2Failed = false;

// Tests that new, new[] and malloc() fail at least once during OOM testing.
class SelfTestObject : public QObject
{
public:
    SelfTestObject(QObject *parent = 0)
        : QObject(parent)
    {
        try { delete new int; } catch (const std::bad_alloc &) { alloc1Failed = true; }
        try { delete [] new double[5]; } catch (const std::bad_alloc &) { alloc2Failed = true; }
        void *buf = malloc(42);
        if (buf)
            free(buf);
        else
            malloc1Failed = true;
    }

    ~SelfTestObject()
    {
        try { delete new int; } catch (const std::bad_alloc &) { alloc3Failed = true; }
        try { delete [] new double[5]; } catch (const std::bad_alloc &) { alloc4Failed = true; }
        void *buf = malloc(42);
        if (buf)
            free(buf);
        else
            malloc2Failed = true;
    }
};

void tst_ExceptionSafetyObjects::initTestCase()
{
    if (RUNNING_ON_VALGRIND) {
        QVERIFY2(VALGRIND_GET_ALLOC_INDEX != -1u,
                 "You must use a valgrind with oom simulation support");
        // running in valgrind - don't use glibc hooks
        disableHooks();
    }

    // sanity check whether OOM simulation works
    AllocFailer allocFailer;

    // malloc fail index is 0 -> this malloc should fail.
    void *buf = malloc(42);
    QVERIFY(!buf);

    // malloc fail index is 1 - second malloc should fail.
    allocFailer.setAllocFailIndex(1);
    buf = malloc(42);
    QVERIFY(buf);
    free(buf);
    buf = malloc(42);
    QVERIFY(!buf);

    allocFailer.deactivate();

    doOOMTest(new ObjectCreator<SelfTestObject>, 0);
}

void tst_ExceptionSafetyObjects::objects()
{
    QFETCH(AbstractObjectCreator *, objectCreator);

    doOOMTest(objectCreator, 0);
}

template <typename T>
struct WidgetCreator : public AbstractObjectCreator
{
    QObject *create(QObject *parent)
    {
        return parent ? new T(static_cast<QWidget *>(parent)) : new T;
    }
};

void tst_ExceptionSafetyObjects::widgets_data()
{
    QTest::addColumn<AbstractObjectCreator *>("widgetCreator");

#undef NEWROW
#define NEWROW(T) QTest::newRow(#T) << static_cast<AbstractObjectCreator *>(new WidgetCreator<T >)
    NEWROW(QWidget);
    NEWROW(QPushButton);
    NEWROW(QLabel);
    NEWROW(QFrame);
    NEWROW(QStackedWidget);
}

void tst_ExceptionSafetyObjects::widgets()
{
    QFETCH(AbstractObjectCreator *, widgetCreator);

    doOOMTest(widgetCreator, 0);

    QWidget parent;
    doOOMTest(widgetCreator, &parent);
}

QTEST_MAIN(tst_ExceptionSafetyObjects)
#include "tst_exceptionsafety_objects.moc"
#endif // QT_NO_EXCEPTIONS
