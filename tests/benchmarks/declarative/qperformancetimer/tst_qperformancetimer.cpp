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
