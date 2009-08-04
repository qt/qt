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

#include <stddef.h>

QT_USE_NAMESPACE

// this test only works with
//   * GLIBC
//   * MSVC - only debug builds (we need the crtdbg.h helpers)
#if (defined(QT_NO_EXCEPTIONS) || (!defined(__GLIBC__) && !defined(Q_CC_MSVC) && !defined(Q_OS_SYMBIAN))) && !defined(Q_MOC_RUN)
    QTEST_NOOP_MAIN
#else

#include "oomsimulator.h"
#if !defined(Q_OS_SYMBIAN)
#include "3rdparty/memcheck.h"
#endif

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
    virtual void test(QObject *parent) = 0;
};

Q_DECLARE_METATYPE(AbstractObjectCreator *)

template <typename T>
struct ObjectCreator : public AbstractObjectCreator
{
    void test(QObject *)
    {
        QScopedPointer<T> ptr(new T);
    }
};

struct BitArrayCreator : public AbstractObjectCreator
{
    void test(QObject *)
    { QScopedPointer<QBitArray> bitArray(new QBitArray(100, true)); }
};

struct ByteArrayMatcherCreator : public AbstractObjectCreator
{
    void test(QObject *)
    { QScopedPointer<QByteArrayMatcher> ptr(new QByteArrayMatcher("ralf test",8)); }
};

struct CryptographicHashCreator : public AbstractObjectCreator
{
    void test(QObject *)
    {
        QScopedPointer<QCryptographicHash> ptr(new QCryptographicHash(QCryptographicHash::Sha1));
        ptr->addData("ralf test",8);
    }
};

struct DataStreamCreator : public AbstractObjectCreator
{
    void test(QObject *)
    {
        QScopedPointer<QByteArray> arr(new QByteArray("hallo, test"));
        QScopedPointer<QDataStream> ptr(new QDataStream(arr.data(), QIODevice::ReadWrite));
        ptr->writeBytes("ralf test",8);
    }
};

struct DirCreator : public AbstractObjectCreator
{
    void test(QObject *)
    {
        QDir::cleanPath("../////././");
        QScopedPointer<QDir> ptr(new QDir("."));
        while( ptr->cdUp() )
            ; // just going up
        ptr->count();
        ptr->exists(ptr->path());

        QStringList filters;
        filters << "*.cpp" << "*.cxx" << "*.cc";
        ptr->setNameFilters(filters);
    }
};

void tst_ExceptionSafetyObjects::objects_data()
{
    QTest::addColumn<AbstractObjectCreator *>("objectCreator");

#define NEWROW(T) QTest::newRow(#T) << static_cast<AbstractObjectCreator *>(new ObjectCreator<T >)
    NEWROW(QObject);
    NEWROW(QBuffer);
    NEWROW(QFile);
    NEWROW(QProcess);
    NEWROW(QSettings);
    // NEWROW(QSocketNotifier);
    NEWROW(QThread);
    NEWROW(QThreadPool);
    NEWROW(QTranslator);
    NEWROW(QFSFileEngine);

#define NEWROW2(T, CREATOR) QTest::newRow(#T) << static_cast<AbstractObjectCreator *>(new CREATOR)
    NEWROW2(QBitArray, BitArrayCreator);
    NEWROW2(QByteArrayMatcher, ByteArrayMatcherCreator);
    NEWROW2(QCryptographicHash, CryptographicHashCreator);
    NEWROW2(QDataStream, DataStreamCreator);
    NEWROW2(QDir, DirCreator);

}

// create and destructs an object, and lets each and every allocation
// during construction and destruction fail.
static void doOOMTest(AbstractObjectCreator *creator, QObject *parent, int start=0)
{
    int currentOOMIndex = start;
    bool caught = false;
    bool done = false;

    AllocFailer allocFailer(0);
    int allocCountBefore = allocFailer.currentAllocIndex();

    do {
        allocFailer.reactivateAt(++currentOOMIndex);

        caught = false;

        try {
            creator->test(parent);
        } catch (const std::bad_alloc &) {
            caught = true;
        } catch (const std::exception &ex) {
            if (strcmp(ex.what(), "autotest swallow") != 0)
                throw;
            caught = true;
        }

        if (!caught) {
            void *buf = malloc(42);
            if (buf) {
                // we got memory here - oom test is over.
                free(buf);
                done = true;
            }
        }

//#define REALLY_VERBOSE
#ifdef REALLY_VERBOSE
    fprintf(stderr, " OOM Index: %d\n", currentOOMIndex);
#endif


    } while (caught || !done);

    allocFailer.deactivate();

//#define VERBOSE
#ifdef VERBOSE
    fprintf(stderr, "OOM Test done, checked allocs: %d (range %d - %d)\n", currentOOMIndex,
                allocCountBefore, allocFailer.currentAllocIndex());
#endif
}

static int alloc1Failed = 0;
static int alloc2Failed = 0;
static int alloc3Failed = 0;
static int alloc4Failed = 0;
static int malloc1Failed = 0;
static int malloc2Failed = 0;

// Tests that new, new[] and malloc() fail at least once during OOM testing.
class SelfTestObject : public QObject
{
public:
    SelfTestObject(QObject *parent = 0)
        : QObject(parent)
    {
        try { delete new int; } catch (const std::bad_alloc &) { ++alloc1Failed; throw; }
        try { delete [] new double[5]; } catch (const std::bad_alloc &) { ++alloc2Failed; throw ;}
        void *buf = malloc(42);
        if (buf)
            free(buf);
        else
            ++malloc1Failed;
    }

    ~SelfTestObject()
    {
        try { delete new int; } catch (const std::bad_alloc &) { ++alloc3Failed; }
        try { delete [] new double[5]; } catch (const std::bad_alloc &) { ++alloc4Failed; }
        void *buf = malloc(42);
        if (buf)
            free(buf);
        else
            ++malloc2Failed = true;
    }
};

void tst_ExceptionSafetyObjects::initTestCase()
{
    QVERIFY(AllocFailer::initialize());

    // sanity check whether OOM simulation works
    AllocFailer allocFailer(0);

    // malloc fail index is 0 -> this malloc should fail.
    void *buf = malloc(42);
    allocFailer.deactivate();
    QVERIFY(!buf);

    // malloc fail index is 1 - second malloc should fail.
    allocFailer.reactivateAt(1);
    buf = malloc(42);
    void *buf2 = malloc(42);
    allocFailer.deactivate();

    QVERIFY(buf);
    free(buf);
    QVERIFY(!buf2);

#ifdef Q_OS_SYMBIAN
    // temporary workaround for INC138398
    std::new_handler nh_func = std::set_new_handler(0);
    (void) std::set_new_handler(nh_func);
#endif

    ObjectCreator<SelfTestObject> *selfTest = new ObjectCreator<SelfTestObject>;
    doOOMTest(selfTest, 0);
    delete selfTest;
    QCOMPARE(alloc1Failed, 1);
    QCOMPARE(alloc2Failed, 1);
    QCOMPARE(alloc3Failed, 2);
    QCOMPARE(alloc4Failed, 3);
    QCOMPARE(malloc1Failed, 1);
    QCOMPARE(malloc2Failed, 1);
}

void tst_ExceptionSafetyObjects::objects()
{
    QFETCH(AbstractObjectCreator *, objectCreator);

    doOOMTest(objectCreator, 0);
    
    delete objectCreator;
}

template <typename T>
struct WidgetCreator : public AbstractObjectCreator
{
    void test(QObject *parent)
    {
        Q_ASSERT(!parent || parent->isWidgetType());
        QScopedPointer<T> ptr(parent ? new T(static_cast<QWidget *>(parent)) : new T);
    }
};

// QSizeGrip doesn't have a default constructor - always pass parent (even though it might be 0)
template <> struct WidgetCreator<QSizeGrip> : public AbstractObjectCreator
{
    void test(QObject *parent)
    {
        Q_ASSERT(!parent || parent->isWidgetType());
        QScopedPointer<QSizeGrip> ptr(new QSizeGrip(static_cast<QWidget *>(parent)));
    }
};

// QDesktopWidget doesn't need a parent.
template <> struct WidgetCreator<QDesktopWidget> : public AbstractObjectCreator
{
    void test(QObject *parent)
    {
        Q_ASSERT(!parent || parent->isWidgetType());
        QScopedPointer<QDesktopWidget> ptr(new QDesktopWidget());
    }
};
void tst_ExceptionSafetyObjects::widgets_data()
{
    QTest::addColumn<AbstractObjectCreator *>("widgetCreator");

#undef NEWROW
#define NEWROW(T) QTest::newRow(#T) << static_cast<AbstractObjectCreator *>(new WidgetCreator<T >)

    NEWROW(QWidget);

    NEWROW(QButtonGroup);
    NEWROW(QDesktopWidget);
    NEWROW(QCheckBox);
    NEWROW(QComboBox);
    NEWROW(QCommandLinkButton);
    NEWROW(QDateEdit);
    NEWROW(QDateTimeEdit);
    NEWROW(QDial);
    NEWROW(QDoubleSpinBox);
    NEWROW(QFocusFrame);
    NEWROW(QFontComboBox);
    NEWROW(QFrame);
    NEWROW(QGroupBox);
    NEWROW(QLCDNumber);
    NEWROW(QLabel);
    NEWROW(QLCDNumber);
    NEWROW(QLineEdit);
    NEWROW(QMenu);
    NEWROW(QPlainTextEdit);
    NEWROW(QProgressBar);
    NEWROW(QPushButton);
    NEWROW(QRadioButton);
    NEWROW(QScrollArea);
    NEWROW(QScrollBar);
    NEWROW(QSizeGrip);
    NEWROW(QSlider);
    NEWROW(QSpinBox);
    NEWROW(QSplitter);
    NEWROW(QStackedWidget);
    NEWROW(QStatusBar);
    NEWROW(QTabBar);
    NEWROW(QTabWidget);
    NEWROW(QTextBrowser);
    NEWROW(QTextEdit);
    NEWROW(QTimeEdit);
    NEWROW(QToolBox);
    NEWROW(QToolButton);
    NEWROW(QStatusBar);
    NEWROW(QSplitter);
    NEWROW(QTextEdit);
    NEWROW(QTextBrowser);
    NEWROW(QToolBar);
    NEWROW(QMenuBar);
    NEWROW(QMainWindow);
    NEWROW(QWorkspace);
    NEWROW(QColumnView);
    NEWROW(QListView);
    NEWROW(QListWidget);
    NEWROW(QTableView);
    NEWROW(QTableWidget);
    NEWROW(QTreeView);
    NEWROW(QTreeWidget);

}

void tst_ExceptionSafetyObjects::widgets()
{
    QFETCH(AbstractObjectCreator *, widgetCreator);

    doOOMTest(widgetCreator, 0, 00000);

    QWidget parent;
    doOOMTest(widgetCreator, &parent, 00000);

    delete widgetCreator;

    // if the test reaches here without crashing, we passed :)
    QVERIFY(true);
}

QTEST_MAIN(tst_ExceptionSafetyObjects)
#include "tst_exceptionsafety_objects.moc"
#endif // QT_NO_EXCEPTIONS
