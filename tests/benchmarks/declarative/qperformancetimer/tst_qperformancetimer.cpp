/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include <private/qperformancetimer_p.h>
#include <QObject>
#include <qtest.h>

class tst_qperformancetimer : public QObject
{
    Q_OBJECT

public:
    tst_qperformancetimer() {}

private slots:
    void all();
    void startElapsed();
    void doubleElapsed();
    void trace();
};

void tst_qperformancetimer::all()
{
    QBENCHMARK {
        QPerformanceTimer t;
        t.start();
        t.elapsed();
    }
}

void tst_qperformancetimer::startElapsed()
{
    QPerformanceTimer t;
    QBENCHMARK {
        t.start();
        t.elapsed();
    }
}

void tst_qperformancetimer::doubleElapsed()
{
    QPerformanceTimer t;
    t.start();
    QBENCHMARK {
        t.elapsed();
        t.elapsed();
    }
}

void tst_qperformancetimer::trace()
{
    QString s("A decent sized string of text here.");
    QBENCHMARK {
        QByteArray data;
        QDataStream ds(&data, QIODevice::WriteOnly);
        ds << (qint64)100 << (int)5 << (int)5 << s;
    }
}

QTEST_MAIN(tst_qperformancetimer)

#include "tst_qperformancetimer.moc"
