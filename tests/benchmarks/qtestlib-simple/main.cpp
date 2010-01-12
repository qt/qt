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
#include <QtGui>
#include <QString>

#include <qtest.h>


class tst_QHash : public QObject
{
    Q_OBJECT
private slots:
    void foo1_data();
    void foo1();
    void foo2_data();
    void foo2();
    void foo3();
};

void tst_QHash::foo1_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::newRow("tag1.1") << 16 << 17;
    QTest::newRow("tag2.1") << 18 << 19;
}

void tst_QHash::foo1()
{
    QFETCH(int, x);
    QFETCH(int, y);
    Q_UNUSED(x);
    Q_UNUSED(y);

    QHash<int, int> testHash;

    QBENCHMARK {
        testHash.insertMulti(1, 1);
    }
}

void tst_QHash::foo2_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::newRow("tag1.1") << 16 << 17;
    QTest::newRow("tag2.1") << 18 << 19;
}

void tst_QHash::foo2()
{
    QFETCH(int, x);
    QFETCH(int, y);
    Q_UNUSED(x);
    Q_UNUSED(y);

    QHash<int, int> testHash;

    QBENCHMARK {
        testHash.insertMulti(1, 1);
    }

    QBENCHMARK {
        testHash.insertMulti(1, 1);
    }
}

void tst_QHash::foo3()
{
    QHash<int, int> testHash;

    QBENCHMARK {
        testHash.insertMulti(1, 1);
    }
}


QTEST_MAIN(tst_QHash)
#include "main.moc"
