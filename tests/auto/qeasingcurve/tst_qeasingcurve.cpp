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


#include <QtTest/QtTest>

#if QT_VERSION < 0x040200
QTEST_NOOP_MAIN
#else

#include <qeasingcurve.h>

//TESTED_CLASS=
//TESTED_FILES=

class tst_QEasingCurve : public QObject {
  Q_OBJECT

public:
    tst_QEasingCurve();
    virtual ~tst_QEasingCurve();

public Q_SLOTS:
    void init();
    void cleanup();

private slots:
    void type();
    void propertyDefaults();
    void valueForProgress_data();
    void valueForProgress();
    void setCustomType();
    void operators();

protected:
};

tst_QEasingCurve::tst_QEasingCurve()
{
}

tst_QEasingCurve::~tst_QEasingCurve()
{
}

void tst_QEasingCurve::init()
{
}

void tst_QEasingCurve::cleanup()
{
}
#include <qdebug.h>

void tst_QEasingCurve::type()
{
    {
    QEasingCurve curve(QEasingCurve::Linear);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);

    curve.setPeriod(5);
    curve.setAmplitude(3);
    QCOMPARE(curve.period(), 5.0);
    QCOMPARE(curve.amplitude(), 3.0);

    curve.setType(QEasingCurve::InElastic);
    QCOMPARE(curve.period(), 5.0);
    QCOMPARE(curve.amplitude(), 3.0);
    }

    {
    QEasingCurve curve(QEasingCurve::InElastic);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);
    curve.setAmplitude(2);
    QCOMPARE(curve.type(), QEasingCurve::InElastic);
    curve.setType(QEasingCurve::Linear);
    }

    {
    // check bounaries
    QEasingCurve curve(QEasingCurve::InCubic);
    QTest::ignoreMessage(QtWarningMsg, "QEasingCurve: Invalid curve type 9999");
    curve.setType((QEasingCurve::Type)9999);
    QCOMPARE(curve.type(), QEasingCurve::InCubic);
    QTest::ignoreMessage(QtWarningMsg, "QEasingCurve: Invalid curve type -9999");
    curve.setType((QEasingCurve::Type)-9999);
    QCOMPARE(curve.type(), QEasingCurve::InCubic);
    QTest::ignoreMessage(QtWarningMsg, QString::fromAscii("QEasingCurve: Invalid curve type %1")
                        .arg(QEasingCurve::NCurveTypes).toLatin1().constData());
    curve.setType(QEasingCurve::NCurveTypes);
    QCOMPARE(curve.type(), QEasingCurve::InCubic);
    QTest::ignoreMessage(QtWarningMsg, QString::fromAscii("QEasingCurve: Invalid curve type %1")
                        .arg(QEasingCurve::Custom).toLatin1().constData());
    curve.setType(QEasingCurve::Custom);
    QCOMPARE(curve.type(), QEasingCurve::InCubic);
    QTest::ignoreMessage(QtWarningMsg, QString::fromAscii("QEasingCurve: Invalid curve type %1")
                        .arg(-1).toLatin1().constData());
    curve.setType((QEasingCurve::Type)-1);
    QCOMPARE(curve.type(), QEasingCurve::InCubic);
    curve.setType(QEasingCurve::Linear);
    QCOMPARE(curve.type(), QEasingCurve::Linear);
    curve.setType(QEasingCurve::CosineCurve);
    QCOMPARE(curve.type(), QEasingCurve::CosineCurve);
    }
}

void tst_QEasingCurve::propertyDefaults()
{
    {
    // checks if the defaults are correct, but also demonstrates a weakness with the API.
    QEasingCurve curve(QEasingCurve::InElastic);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);
    QCOMPARE(curve.overshoot(), qreal(1.70158f));
    curve.setType(QEasingCurve::InBounce);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);
    QCOMPARE(curve.overshoot(), qreal(1.70158f));
    curve.setType(QEasingCurve::Linear);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);
    QCOMPARE(curve.overshoot(), qreal(1.70158f));
    curve.setType(QEasingCurve::InElastic);
    QCOMPARE(curve.period(), 0.3);
    QCOMPARE(curve.amplitude(), 1.0);
    QCOMPARE(curve.overshoot(), qreal(1.70158f));
    curve.setPeriod(0.4);
    curve.setAmplitude(0.6);
    curve.setOvershoot(1.0);
    curve.setType(QEasingCurve::Linear);
    QCOMPARE(curve.period(), 0.4);
    QCOMPARE(curve.amplitude(), 0.6);
    QCOMPARE(curve.overshoot(), 1.0);
    curve.setType(QEasingCurve::InElastic);
    QCOMPARE(curve.period(), 0.4);
    QCOMPARE(curve.amplitude(), 0.6);
    QCOMPARE(curve.overshoot(), 1.0);
    }
}

typedef QList<int> IntList;
typedef QList<qreal> RealList;
Q_DECLARE_METATYPE(IntList)
Q_DECLARE_METATYPE(RealList)

void tst_QEasingCurve::valueForProgress_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<IntList>("at");
    QTest::addColumn<RealList>("expected");
    // automatically generated.
    // note that values are scaled from range [0,1] to range [0, 100] in order to store them as
    // integer values and avoid fp inaccuracies
    QTest::newRow("Linear") << int(QEasingCurve::Linear)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.1 << 0.2 << 0.3 << 0.4 << 0.5 << 0.6 << 0.7 << 0.8 << 0.9 << 1);

    QTest::newRow("InQuad") << int(QEasingCurve::InQuad)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.01 << 0.04 << 0.09 << 0.16 << 0.25 << 0.36 << 0.49 << 0.64 << 0.81 << 1);

    QTest::newRow("OutQuad") << int(QEasingCurve::OutQuad)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.19 << 0.36 << 0.51 << 0.64 << 0.75 << 0.84 << 0.91 << 0.96 << 0.99 << 1);

    QTest::newRow("InOutQuad") << int(QEasingCurve::InOutQuad)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.02 << 0.08 << 0.18 << 0.32 << 0.5 << 0.68 << 0.82 << 0.92 << 0.98 << 1);

    QTest::newRow("OutInQuad") << int(QEasingCurve::OutInQuad)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.18 << 0.32 << 0.42 << 0.48 << 0.5 << 0.52 << 0.58 << 0.68 << 0.82 << 1);

    QTest::newRow("InCubic") << int(QEasingCurve::InCubic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.001 << 0.008 << 0.027 << 0.064 << 0.125 << 0.216 << 0.343 << 0.512 << 0.729 << 1);

    QTest::newRow("OutCubic") << int(QEasingCurve::OutCubic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.271 << 0.488 << 0.657 << 0.784 << 0.875 << 0.936 << 0.973 << 0.992 << 0.999 << 1);

    QTest::newRow("InOutCubic") << int(QEasingCurve::InOutCubic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.004 << 0.032 << 0.108 << 0.256 << 0.5 << 0.744 << 0.892 << 0.968 << 0.996 << 1);

    QTest::newRow("OutInCubic") << int(QEasingCurve::OutInCubic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.244 << 0.392 << 0.468 << 0.496 << 0.5 << 0.504 << 0.532 << 0.608 << 0.756 << 1);

    QTest::newRow("InQuart") << int(QEasingCurve::InQuart)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.0001 << 0.0016 << 0.0081 << 0.0256 << 0.0625 << 0.1296 << 0.2401 << 0.4096 << 0.6561 << 1);

    QTest::newRow("OutQuart") << int(QEasingCurve::OutQuart)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.3439 << 0.5904 << 0.7599 << 0.8704 << 0.9375 << 0.9744 << 0.9919 << 0.9984 << 0.9999 << 1);

    QTest::newRow("InOutQuart") << int(QEasingCurve::InOutQuart)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.0008 << 0.0128 << 0.0648 << 0.2048 << 0.5 << 0.7952 << 0.9352 << 0.9872 << 0.9992 << 1);

    QTest::newRow("OutInQuart") << int(QEasingCurve::OutInQuart)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.2952 << 0.4352 << 0.4872 << 0.4992 << 0.5 << 0.5008 << 0.5128 << 0.5648 << 0.7048 << 1);

    QTest::newRow("InQuint") << int(QEasingCurve::InQuint)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 1e-05 << 0.00032 << 0.00243 << 0.01024 << 0.03125 << 0.07776 << 0.1681 << 0.3277 << 0.5905 << 1);

    QTest::newRow("OutQuint") << int(QEasingCurve::OutQuint)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.4095 << 0.6723 << 0.8319 << 0.9222 << 0.9688 << 0.9898 << 0.9976 << 0.9997 << 1 << 1);

    QTest::newRow("InOutQuint") << int(QEasingCurve::InOutQuint)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.00016 << 0.00512 << 0.03888 << 0.1638 << 0.5 << 0.8362 << 0.9611 << 0.9949 << 0.9998 << 1);

    QTest::newRow("OutInQuint") << int(QEasingCurve::OutInQuint)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.3362 << 0.4611 << 0.4949 << 0.4998 << 0.5 << 0.5002 << 0.5051 << 0.5389 << 0.6638 << 1);

    QTest::newRow("InSine") << int(QEasingCurve::InSine)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.01231 << 0.04894 << 0.109 << 0.191 << 0.2929 << 0.4122 << 0.546 << 0.691 << 0.8436 << 1);

    QTest::newRow("OutSine") << int(QEasingCurve::OutSine)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.1564 << 0.309 << 0.454 << 0.5878 << 0.7071 << 0.809 << 0.891 << 0.9511 << 0.9877 << 1);

    QTest::newRow("InOutSine") << int(QEasingCurve::InOutSine)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.02447 << 0.09549 << 0.2061 << 0.3455 << 0.5 << 0.6545 << 0.7939 << 0.9045 << 0.9755 << 1);

    QTest::newRow("OutInSine") << int(QEasingCurve::OutInSine)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.1545 << 0.2939 << 0.4045 << 0.4755 << 0.5 << 0.5245 << 0.5955 << 0.7061 << 0.8455 << 1);

    QTest::newRow("InExpo") << int(QEasingCurve::InExpo)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.0009531 << 0.002906 << 0.006812 << 0.01462 << 0.03025 << 0.0615 << 0.124 << 0.249 << 0.499 << 1);

    QTest::newRow("OutExpo") << int(QEasingCurve::OutExpo)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.5005 << 0.7507 << 0.8759 << 0.9384 << 0.9697 << 0.9854 << 0.9932 << 0.9971 << 0.999 << 1);

    QTest::newRow("InOutExpo") << int(QEasingCurve::InOutExpo)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.001453 << 0.007312 << 0.03075 << 0.1245 << 0.5002 << 0.8754 << 0.9692 << 0.9927 << 0.9985 << 1);

    QTest::newRow("OutInExpo") << int(QEasingCurve::OutInExpo)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.3754 << 0.4692 << 0.4927 << 0.4985 << 0.5 << 0.5015 << 0.5073 << 0.5308 << 0.6245 << 1);

    QTest::newRow("InCirc") << int(QEasingCurve::InCirc)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.005013 << 0.0202 << 0.04606 << 0.08348 << 0.134 << 0.2 << 0.2859 << 0.4 << 0.5641 << 1);

    QTest::newRow("OutCirc") << int(QEasingCurve::OutCirc)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.4359 << 0.6 << 0.7141 << 0.8 << 0.866 << 0.9165 << 0.9539 << 0.9798 << 0.995 << 1);

    QTest::newRow("InOutCirc") << int(QEasingCurve::InOutCirc)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.0101 << 0.04174 << 0.1 << 0.2 << 0.5 << 0.8 << 0.9 << 0.9583 << 0.9899 << 1);

    QTest::newRow("OutInCirc") << int(QEasingCurve::OutInCirc)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.3 << 0.4 << 0.4583 << 0.4899 << 0.5 << 0.5101 << 0.5417 << 0.6 << 0.7 << 1);

    QTest::newRow("InElastic") << int(QEasingCurve::InElastic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.001953 << -0.001953 << -0.003906 << 0.01562 << -0.01562 << -0.03125 << 0.125 << -0.125 << -0.25 << 1);

    QTest::newRow("OutElastic") << int(QEasingCurve::OutElastic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 1.25 << 1.125 << 0.875 << 1.031 << 1.016 << 0.9844 << 1.004 << 1.002 << 0.998 << 1);

    QTest::newRow("InOutElastic") << int(QEasingCurve::InOutElastic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << -0.0009766 << 0.007812 << -0.01563 << -0.0625 << 0.5 << 1.062 << 1.016 << 0.9922 << 1.001 << 1);

    QTest::newRow("OutInElastic") << int(QEasingCurve::OutInElastic)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.375 << 0.5625 << 0.4922 << 0.498 << 0.5 << 0.4961 << 0.5078 << 0.5313 << 0.25 << 1);

    QTest::newRow("InBack") << int(QEasingCurve::InBack)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << -0.01431 << -0.04645 << -0.0802 << -0.09935 << -0.0877 << -0.02903 << 0.09287 << 0.2942 << 0.5912 << 1);

    QTest::newRow("OutBack") << int(QEasingCurve::OutBack)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.4088 << 0.7058 << 0.9071 << 1.029 << 1.088 << 1.099 << 1.08 << 1.046 << 1.014 << 1);

    QTest::newRow("InOutBack") << int(QEasingCurve::InOutBack)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << -0.03752 << -0.09256 << -0.07883 << 0.08993 << 0.5 << 0.9101 << 1.079 << 1.093 << 1.038 << 1);

    QTest::newRow("OutInBack") << int(QEasingCurve::OutInBack)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.3529 << 0.5145 << 0.5497 << 0.5232 << 0.5 << 0.4768 << 0.4503 << 0.4855 << 0.6471 << 1);

    QTest::newRow("InBounce") << int(QEasingCurve::InBounce)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.01188 << 0.06 << 0.06937 << 0.2275 << 0.2344 << 0.09 << 0.3194 << 0.6975 << 0.9244 << 1);

    QTest::newRow("OutBounce") << int(QEasingCurve::OutBounce)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.07563 << 0.3025 << 0.6806 << 0.91 << 0.7656 << 0.7725 << 0.9306 << 0.94 << 0.9881 << 1);

    QTest::newRow("InOutBounce") << int(QEasingCurve::InOutBounce)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.03 << 0.1138 << 0.045 << 0.3488 << 0.5 << 0.6512 << 0.955 << 0.8862 << 0.97 << 1);

    QTest::newRow("OutInBounce") << int(QEasingCurve::OutInBounce)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.1513 << 0.41 << 0.2725 << 0.44 << 0.5 << 0.56 << 0.7275 << 0.59 << 0.8488 << 1);

    QTest::newRow("InCurve") << int(QEasingCurve::InCurve)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.02447 << 0.1059 << 0.2343 << 0.3727 << 0.5 << 0.6055 << 0.7 << 0.8 << 0.9 << 1);

    QTest::newRow("OutCurve") << int(QEasingCurve::OutCurve)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.1 << 0.2 << 0.3 << 0.3945 << 0.5 << 0.6273 << 0.7657 << 0.8941 << 0.9755 << 1);

    QTest::newRow("SineCurve") << int(QEasingCurve::SineCurve)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0 << 0.09549 << 0.3455 << 0.6545 << 0.9045 << 1 << 0.9045 << 0.6545 << 0.3455 << 0.09549 << 0);

    QTest::newRow("CosineCurve") << int(QEasingCurve::CosineCurve)
         << (IntList()  << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100)
         << (RealList() << 0.5 << 0.7939 << 0.9755 << 0.9755 << 0.7939 << 0.5 << 0.2061 << 0.02447 << 0.02447 << 0.2061 << 0.5);

}


void tst_QEasingCurve::valueForProgress()
{
#if 0
    // used to generate data tables...
    QFile out;
    out.open(stdout, QIODevice::WriteOnly);
    for (int c = QEasingCurve::Linear; c < QEasingCurve::NCurveTypes - 1; ++c) {
        QEasingCurve curve((QEasingCurve::Type)c);
        QMetaObject mo = QEasingCurve::staticMetaObject;
        QString strCurve = QLatin1String(mo.enumerator(mo.indexOfEnumerator("Type")).key(c));
        QString strInputs;
        QString strOutputs;

        for (int t = 0; t <= 100; t+= 10) {
            qreal ease = curve.valueForProgress(t/qreal(100));
            strInputs += QString::fromAscii(" << %1").arg(t);
            strOutputs += " << " + QString().setNum(ease, 'g', 4);
        }
        QString str = QString::fromAscii("    QTest::newRow(\"%1\") << int(QEasingCurve::%2)\n"
                                                "         << (IntList() %3)\n"
                                                "         << (RealList()%4);\n\n")
                                      .arg(strCurve)
                                      .arg(strCurve)
                                      .arg(strInputs)
                                      .arg(strOutputs);
        out.write(str.toLatin1().constData());
    }
    out.close();
    exit(1);
#else
    QFETCH(int, type);
    QFETCH(IntList, at);
    QFETCH(RealList, expected);

    QEasingCurve curve((QEasingCurve::Type)type);
    for (int i = 0; i < at.count(); ++i) {
        qreal ease = curve.valueForProgress(at.at(i)/qreal(100));
        // converting ease to 4 precision qreal to match the generated samples
        qreal easeConv = qreal(QString().setNum(ease, 'g', 4).toDouble());
        qreal ex = expected.at(i);

        // the least significant digit it is still subject to rounding errors
        qreal error = easeConv - ex;
        qreal errorbound = 0.00001;                
#if defined( Q_OS_WINCE ) || defined( Q_OS_SYMBIAN )
        // exception values for WINCE(this test should be rewritten, as it only freezes the status quo of QEasingCurve
        // The failing (2) values are explicitly excepted here:
        // The source values for the comparison table should remain untruncated double and the
        // error bound checking function dynamic. Also the source values should come from a "trusted" source and not
        // from QEasingCurve itself.        
        if ((type == int(QEasingCurve::InOutBounce) && (i == 8 || i == 6) ) || (type == int(QEasingCurve::OutExpo) && i == 2))
            errorbound = 0.0002;         
#endif 
        // accept the potential rounding error in the least significant digit
        QVERIFY(error <= errorbound );          
    }
#endif
}

static qreal discreteEase(qreal progress)
{
    return qFloor(progress * 10) / qreal(10.0);
}

void tst_QEasingCurve::setCustomType()
{
    QEasingCurve curve;
    curve.setCustomType(&discreteEase);
    QCOMPARE(curve.type(), QEasingCurve::Custom);
    QCOMPARE(curve.valueForProgress(0.0), 0.0);
    QCOMPARE(curve.valueForProgress(0.05), 0.0);
    QCOMPARE(curve.valueForProgress(0.10), 0.1);
    QCOMPARE(curve.valueForProgress(0.15), 0.1);
    QCOMPARE(curve.valueForProgress(0.20), 0.2);
    QCOMPARE(curve.valueForProgress(0.25), 0.2);
    QCOMPARE(curve.valueForProgress(0.30), 0.3);
    QCOMPARE(curve.valueForProgress(0.35), 0.3);
    QCOMPARE(curve.valueForProgress(0.999999), 0.9);

    curve.setType(QEasingCurve::Linear);
    QCOMPARE(curve.type(), QEasingCurve::Linear);
    QCOMPARE(curve.valueForProgress(0.0), 0.0);
    QCOMPARE(curve.valueForProgress(0.1), 0.1);
    QCOMPARE(curve.valueForProgress(0.5), 0.5);
    QCOMPARE(curve.valueForProgress(0.99), 0.99);
}

void tst_QEasingCurve::operators()
{
    // operator=
    QEasingCurve curve;
    QEasingCurve curve2;
    curve.setCustomType(&discreteEase);
    curve2 = curve;
    QCOMPARE(curve2.type(), QEasingCurve::Custom);
    QCOMPARE(curve2.valueForProgress(0.0), 0.0);
    QCOMPARE(curve2.valueForProgress(0.05), 0.0);
    QCOMPARE(curve2.valueForProgress(0.15), 0.1);
    QCOMPARE(curve2.valueForProgress(0.25), 0.2);
    QCOMPARE(curve2.valueForProgress(0.35), 0.3);
    QCOMPARE(curve2.valueForProgress(0.999999), 0.9);

    // operator==
    curve.setType(QEasingCurve::InBack);
    curve2 = curve;
    curve2.setOvershoot(qreal(1.70158f));
    QCOMPARE(curve.overshoot(), curve2.overshoot());
    QVERIFY(curve2 == curve);

    curve.setOvershoot(3.0);
    QVERIFY(curve2 != curve);
    curve2.setOvershoot(3.0);
    QVERIFY(curve2 == curve);

    curve2.setType(QEasingCurve::Linear);
    QCOMPARE(curve.overshoot(), curve2.overshoot());
    QVERIFY(curve2 != curve);
    curve2.setType(QEasingCurve::InBack);
    QCOMPARE(curve.overshoot(), curve2.overshoot());
    QVERIFY(curve2 == curve);
}


QTEST_MAIN(tst_QEasingCurve)
#include "tst_qeasingcurve.moc"

#endif //QT_VERSION
