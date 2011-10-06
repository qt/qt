/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
// This file contains benchmarks for QLineF functions.

#include <QDebug>
#include <qtest.h>
#include <QLine>

class tst_qline : public QObject
{
    Q_OBJECT
private slots:
    void fromPolar();
    void intersect_data();
    void intersect();
    void length();
    void setLength();
    void angle();
    void setAngle();
    void angleTo();
    void unitVector();
    void normalVector();
    void angle2();
};

void tst_qline::fromPolar()
{
    QBENCHMARK {
        QLineF::fromPolar(10, 2);
    }
}

void tst_qline::intersect_data()
{
    QTest::addColumn<QLineF>("l1");
    QTest::addColumn<QLineF>("l2");
    QTest::newRow("cross") << QLineF(-1,-1,1,1) << QLineF(-1,1,1,-1);
    QTest::newRow("miss") << QLineF(1,1,2,2) << QLineF(1,11,2,12);
}

void tst_qline::intersect()
{
    QFETCH(QLineF, l1);
    QFETCH(QLineF, l2);
    QPointF intersection;
    QBENCHMARK {
        l1.intersect(l2, &intersection);
    }
}

void tst_qline::length()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.length();
    }
}

void tst_qline::setLength()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.setLength(5);
    }
}

void tst_qline::angle()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.angle();
    }
}

void tst_qline::setAngle()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.setAngle(1);
    }
}

void tst_qline::angleTo()
{
    QLineF line1(1,2,3,4);
    QLineF line2(8,7,6,5);
    QBENCHMARK {
        line1.angleTo(line2);
    }
}

void tst_qline::unitVector()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.unitVector();
    }
}

void tst_qline::normalVector()
{
    QLineF line(1,2,3,4);
    QBENCHMARK {
        line.normalVector();
    }
}

void tst_qline::angle2()
{
    QLineF line1(1,2,3,4);
    QLineF line2(8,7,6,5);
    QBENCHMARK {
        line1.angle(line2);
    }
}

QTEST_MAIN(tst_qline)

#include "main.moc"
