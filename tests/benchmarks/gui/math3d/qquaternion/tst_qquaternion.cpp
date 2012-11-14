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

#include <qtest.h>
#include <QQuaternion>

//TESTED_FILES=

class tst_QQuaternion : public QObject
{
    Q_OBJECT

public:
    tst_QQuaternion();
    virtual ~tst_QQuaternion();

public slots:
    void init();
    void cleanup();

private slots:
    void multiply_data();
    void multiply();
};

tst_QQuaternion::tst_QQuaternion()
{
}

tst_QQuaternion::~tst_QQuaternion()
{
}

void tst_QQuaternion::init()
{
}

void tst_QQuaternion::cleanup()
{
}

void tst_QQuaternion::multiply_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("w1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");
    QTest::addColumn<qreal>("w2");

    QTest::newRow("null")
        << (qreal)0.0f << (qreal)0.0f << (qreal)0.0f << (qreal)0.0f
        << (qreal)0.0f << (qreal)0.0f << (qreal)0.0f << (qreal)0.0f;

    QTest::newRow("unitvec")
        << (qreal)1.0f << (qreal)0.0f << (qreal)0.0f << (qreal)1.0f
        << (qreal)0.0f << (qreal)1.0f << (qreal)0.0f << (qreal)1.0f;

    QTest::newRow("complex")
        << (qreal)1.0f << (qreal)2.0f << (qreal)3.0f << (qreal)7.0f
        << (qreal)4.0f << (qreal)5.0f << (qreal)6.0f << (qreal)8.0f;
}

void tst_QQuaternion::multiply()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, w1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(qreal, w2);

    QQuaternion q1(w1, x1, y1, z1);
    QQuaternion q2(w2, x2, y2, z2);

    QBENCHMARK {
        QQuaternion q3 = q1 * q2;
    }
}

QTEST_MAIN(tst_QQuaternion)
#include "tst_qquaternion.moc"
