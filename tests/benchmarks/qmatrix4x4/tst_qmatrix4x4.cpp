/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtOpenGL module of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QtGui/qmatrix4x4.h>

class tst_QMatrix4x4 : public QObject
{
    Q_OBJECT
public:
    tst_QMatrix4x4() {}
    ~tst_QMatrix4x4() {}

private slots:
    void multiply_data();
    void multiply();

    void multiplyInPlace_data();
    void multiplyInPlace();

    void multiplyDirect_data();
    void multiplyDirect();

    void mapVector3D_data();
    void mapVector3D();

    void mapVector2D_data();
    void mapVector2D();

    void mapVectorDirect_data();
    void mapVectorDirect();
};

static qreal const generalValues[16] =
    {1.0f, 2.0f, 3.0f, 4.0f,
     5.0f, 6.0f, 7.0f, 8.0f,
     9.0f, 10.0f, 11.0f, 12.0f,
     13.0f, 14.0f, 15.0f, 16.0f};

void tst_QMatrix4x4::multiply_data()
{
    QTest::addColumn<QMatrix4x4>("m1");
    QTest::addColumn<QMatrix4x4>("m2");

    QTest::newRow("identity * identity")
        << QMatrix4x4() << QMatrix4x4();
    QTest::newRow("identity * general")
        << QMatrix4x4() << QMatrix4x4(generalValues);
    QTest::newRow("general * identity")
        << QMatrix4x4(generalValues) << QMatrix4x4();
    QTest::newRow("general * general")
        << QMatrix4x4(generalValues) << QMatrix4x4(generalValues);
}

QMatrix4x4 mresult;

void tst_QMatrix4x4::multiply()
{
    QFETCH(QMatrix4x4, m1);
    QFETCH(QMatrix4x4, m2);

    QMatrix4x4 m3;

    QBENCHMARK {
        m3 = m1 * m2;
    }

    // Force the result to be stored so the compiler doesn't
    // optimize away the contents of the benchmark loop.
    mresult = m3;
}

void tst_QMatrix4x4::multiplyInPlace_data()
{
    multiply_data();
}

void tst_QMatrix4x4::multiplyInPlace()
{
    QFETCH(QMatrix4x4, m1);
    QFETCH(QMatrix4x4, m2);

    QMatrix4x4 m3;

    QBENCHMARK {
        m3 = m1;
        m3 *= m2;
    }

    // Force the result to be stored so the compiler doesn't
    // optimize away the contents of the benchmark loop.
    mresult = m3;
}

// Use a direct naive multiplication algorithm.  This is used
// to compare against the optimized routines to see if they are
// actually faster than the naive implementation.
void tst_QMatrix4x4::multiplyDirect_data()
{
    multiply_data();
}
void tst_QMatrix4x4::multiplyDirect()
{
    QFETCH(QMatrix4x4, m1);
    QFETCH(QMatrix4x4, m2);

    QMatrix4x4 m3;

    const float *m1data = m1.constData();
    const float *m2data = m2.constData();
    float *m3data = m3.data();

    QBENCHMARK {
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                m3data[col * 4 + row] = 0.0f;
                for (int j = 0; j < 4; ++j) {
                    m3data[col * 4 + row] +=
                        m1data[j * 4 + row] * m2data[col * 4 + j];
                }
            }
        }
    }
}

QVector3D vresult;

void tst_QMatrix4x4::mapVector3D_data()
{
    QTest::addColumn<QMatrix4x4>("m1");

    QTest::newRow("identity") << QMatrix4x4();
    QTest::newRow("general") << QMatrix4x4(generalValues);

    QMatrix4x4 t1;
    t1.translate(-100.5f, 64.0f, 75.25f);
    QTest::newRow("translate3D") << t1;

    QMatrix4x4 t2;
    t2.translate(-100.5f, 64.0f);
    QTest::newRow("translate2D") << t2;

    QMatrix4x4 s1;
    s1.scale(-100.5f, 64.0f, 75.25f);
    QTest::newRow("scale3D") << s1;

    QMatrix4x4 s2;
    s2.scale(-100.5f, 64.0f);
    QTest::newRow("scale2D") << s2;
}
void tst_QMatrix4x4::mapVector3D()
{
    QFETCH(QMatrix4x4, m1);

    QVector3D v(10.5f, -2.0f, 3.0f);
    QVector3D result;

    m1.inferSpecialType();

    QBENCHMARK {
        result = m1 * v;
    }

    // Force the result to be stored so the compiler doesn't
    // optimize away the contents of the benchmark loop.
    vresult = result;
}

QPointF vresult2;

void tst_QMatrix4x4::mapVector2D_data()
{
    mapVector3D_data();
}
void tst_QMatrix4x4::mapVector2D()
{
    QFETCH(QMatrix4x4, m1);

    QPointF v(10.5f, -2.0f);
    QPointF result;

    m1.inferSpecialType();

    QBENCHMARK {
        result = m1 * v;
    }

    // Force the result to be stored so the compiler doesn't
    // optimize away the contents of the benchmark loop.
    vresult2 = result;
}

// Use a direct naive multiplication algorithm.  This is used
// to compare against the optimized routines to see if they are
// actually faster than the naive implementation.
void tst_QMatrix4x4::mapVectorDirect_data()
{
    mapVector3D_data();
}
void tst_QMatrix4x4::mapVectorDirect()
{
    QFETCH(QMatrix4x4, m1);

    const float *m1data = m1.constData();
    float v[4] = {10.5f, -2.0f, 3.0f, 1.0f};
    float result[4];

    QBENCHMARK {
        for (int row = 0; row < 4; ++row) {
            result[row] = 0.0f;
            for (int col = 0; col < 4; ++col) {
                result[row] += m1data[col * 4 + row] * v[col];
            }
        }
        result[0] /= result[3];
        result[1] /= result[3];
        result[2] /= result[3];
    }
}

QTEST_MAIN(tst_QMatrix4x4)

#include "tst_qmatrix4x4.moc"
