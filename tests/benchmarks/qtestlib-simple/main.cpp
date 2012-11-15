/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
