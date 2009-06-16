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

class tst_ExceptionSafetyObjects: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();

private slots:
    void widgets_data();
    void widgets();
};

void tst_ExceptionSafetyObjects::initTestCase()
{
}

// helper structs to create an arbitrary widget
struct AbstractWidgetCreator
{
    virtual QWidget *create(QWidget *parent) = 0;
};

Q_DECLARE_METATYPE(AbstractWidgetCreator *)

template <typename T>
struct WidgetCreator : public AbstractWidgetCreator
{
    QWidget *create(QWidget *parent)
    {
        return parent ? new T(parent) : new T;
    }
};

void tst_ExceptionSafetyObjects::widgets_data()
{
    QTest::addColumn<AbstractWidgetCreator *>("widgetCreator");

#define NEWROW(T) QTest::newRow(#T) << static_cast<AbstractWidgetCreator *>(new WidgetCreator<T >)
    NEWROW(QWidget);
    NEWROW(QPushButton);
    NEWROW(QLabel);
}

void tst_ExceptionSafetyObjects::widgets()
{
    QFETCH(AbstractWidgetCreator *, widgetCreator);

    mallocCount = freeCount = 0;

    int currentOOMIndex = 0;

    // activate mallocFail - WE'RE HOT!
    AllocFailActivator allocFailActivator;

    do {
        // start after first alloc (the first alloc is creation of the widget itself)
        mallocFailIndex = ++currentOOMIndex;

        // first, create without a parent
        try {
            QScopedPointer<QWidget> ptr(widgetCreator->create(0));
            // QScopedPointer deletes the widget again here.
        } catch (const std::bad_alloc &) {
            // ignore all std::bad_alloc - note: valgrind should show no leaks
        }

        // repeat the loop until we the malloc fail index indicates that
        // there was no OOM simulation happening
    } while (mallocFailIndex <= 0);

    // reset counting
    currentOOMIndex = 0;

    do {
        mallocFailIndex = ++currentOOMIndex;

        // create the widget with a parent
        try {
            QWidget parent;
            widgetCreator->create(&parent);
            // parent goes out of scope - widget should be deleted as well
        } catch (const std::bad_alloc &) {
        }
    } while (mallocFailIndex <= 0);

#ifdef VERBOSE
    allocFailActivator.deactivate();

    qDebug() << "mallocCount" << mallocCount << "freeCount" << freeCount <<
                "simulated alloc fails" << currentOOMIndex;
#endif
}

QTEST_MAIN(tst_ExceptionSafetyObjects)
#include "tst_exceptionsafety_objects.moc"
#endif // QT_NO_EXCEPTIONS
