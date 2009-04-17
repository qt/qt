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
Q_DECLARE_METATYPE(IntList)

void tst_QEasingCurve::valueForProgress_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<IntList>("at");
    QTest::addColumn<IntList>("expected");
    // automatically generated.
    // note that values are scaled from range [0,1] to range [0, 100] in order to store them as
    // integer values and avoid fp inaccuracies

    QTest::newRow("Linear") << int(QEasingCurve::Linear)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100);

    QTest::newRow("InQuad") << int(QEasingCurve::InQuad)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 4 << 16 << 36 << 64 << 100);

    QTest::newRow("OutQuad") << int(QEasingCurve::OutQuad)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 36 << 64 << 84 << 96 << 100);

    QTest::newRow("InOutQuad") << int(QEasingCurve::InOutQuad)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 8 << 32 << 68 << 92 << 100);

    QTest::newRow("OutInQuad") << int(QEasingCurve::OutInQuad)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 32 << 48 << 52 << 68 << 100);

    QTest::newRow("InCubic") << int(QEasingCurve::InCubic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 6 << 21 << 51 << 100);

    QTest::newRow("OutCubic") << int(QEasingCurve::OutCubic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 48 << 78 << 93 << 99 << 100);

    QTest::newRow("InOutCubic") << int(QEasingCurve::InOutCubic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 3 << 25 << 74 << 96 << 100);

    QTest::newRow("OutInCubic") << int(QEasingCurve::OutInCubic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 39 << 49 << 50 << 60 << 100);

    QTest::newRow("InQuart") << int(QEasingCurve::InQuart)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 2 << 12 << 40 << 100);

    QTest::newRow("OutQuart") << int(QEasingCurve::OutQuart)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 59 << 87 << 97 << 99 << 100);

    QTest::newRow("InOutQuart") << int(QEasingCurve::InOutQuart)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 1 << 20 << 79 << 98 << 100);

    QTest::newRow("OutInQuart") << int(QEasingCurve::OutInQuart)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 43 << 49 << 50 << 56 << 100);

    QTest::newRow("InQuint") << int(QEasingCurve::InQuint)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 1 << 7 << 32 << 100);

    QTest::newRow("OutQuint") << int(QEasingCurve::OutQuint)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 67 << 92 << 98 << 99 << 100);

    QTest::newRow("InOutQuint") << int(QEasingCurve::InOutQuint)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 16 << 83 << 99 << 100);

    QTest::newRow("OutInQuint") << int(QEasingCurve::OutInQuint)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 46 << 49 << 50 << 53 << 100);

    QTest::newRow("InSine") << int(QEasingCurve::InSine)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 4 << 19 << 41 << 69 << 100);

    QTest::newRow("OutSine") << int(QEasingCurve::OutSine)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 30 << 58 << 80 << 95 << 100);

    QTest::newRow("InOutSine") << int(QEasingCurve::InOutSine)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 9 << 34 << 65 << 90 << 100);

    QTest::newRow("OutInSine") << int(QEasingCurve::OutInSine)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 29 << 47 << 52 << 70 << 100);

    QTest::newRow("InExpo") << int(QEasingCurve::InExpo)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 1 << 6 << 24 << 100);

    QTest::newRow("OutExpo") << int(QEasingCurve::OutExpo)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 75 << 93 << 98 << 99 << 100);

    QTest::newRow("InOutExpo") << int(QEasingCurve::InOutExpo)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 12 << 87 << 99 << 100);

    QTest::newRow("OutInExpo") << int(QEasingCurve::OutInExpo)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 46 << 49 << 50 << 53 << 100);

    QTest::newRow("InCirc") << int(QEasingCurve::InCirc)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 2 << 8 << 19 << 40 << 100);

    QTest::newRow("OutCirc") << int(QEasingCurve::OutCirc)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 59 << 80 << 91 << 97 << 100);

    QTest::newRow("InOutCirc") << int(QEasingCurve::InOutCirc)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 4 << 20 << 80 << 95 << 100);

    QTest::newRow("OutInCirc") << int(QEasingCurve::OutInCirc)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 40 << 48 << 51 << 60 << 100);

    QTest::newRow("InElastic") << int(QEasingCurve::InElastic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << 1 << -3 << -12 << 100);

    QTest::newRow("OutElastic") << int(QEasingCurve::OutElastic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 112 << 103 << 98 << 100 << 100);

    QTest::newRow("InOutElastic") << int(QEasingCurve::InOutElastic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 0 << -6 << 106 << 99 << 100);

    QTest::newRow("OutInElastic") << int(QEasingCurve::OutInElastic)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 56 << 49 << 49 << 53 << 100);

    QTest::newRow("InBack") << int(QEasingCurve::InBack)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << -4 << -9 << -2 << 29 << 100);

    QTest::newRow("OutBack") << int(QEasingCurve::OutBack)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 70 << 102 << 109 << 104 << 100);

    QTest::newRow("InOutBack") << int(QEasingCurve::InOutBack)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << -9 << 8 << 91 << 109 << 100);

    QTest::newRow("OutInBack") << int(QEasingCurve::OutInBack)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 51 << 52 << 47 << 48 << 100);

    QTest::newRow("InBounce") << int(QEasingCurve::InBounce)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 6 << 22 << 8 << 69 << 100);

    QTest::newRow("OutBounce") << int(QEasingCurve::OutBounce)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 30 << 91 << 77 << 93 << 100);

    QTest::newRow("InOutBounce") << int(QEasingCurve::InOutBounce)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 11 << 34 << 65 << 88 << 100);

    QTest::newRow("OutInBounce") << int(QEasingCurve::OutInBounce)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 41 << 43 << 56 << 58 << 100);

    QTest::newRow("InCurve") << int(QEasingCurve::InCurve)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 10 << 37 << 60 << 80 << 100);

    QTest::newRow("OutCurve") << int(QEasingCurve::OutCurve)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 20 << 39 << 62 << 89 << 100);

    QTest::newRow("SineCurve") << int(QEasingCurve::SineCurve)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 0 << 34 << 90 << 90 << 34 << 0);

    QTest::newRow("CosineCurve") << int(QEasingCurve::CosineCurve)
                 << (IntList()  << 0 << 20 << 40 << 60 << 80 << 100)
                 << (IntList()  << 50 << 97 << 79 << 20 << 2 << 49);


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
        
        for (int t = 0; t <= 100; t+= 20) {
            qreal ease = curve.valueForProgress(t/qreal(100));
            strInputs += QString::fromAscii(" << %1").arg(t);
            strOutputs += QString::fromAscii(" << %1").arg(int(100*ease));
            
        }
        QString str = QString::fromAscii("    QTest::newRow(\"%1\") << int(QEasingCurve::%2)\n"
                                                "\t\t << (IntList() %3)\n"
                                                "\t\t << (IntList() %4);\n\n")
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
    QFETCH(IntList, expected);

    QEasingCurve curve((QEasingCurve::Type)type);
    for (int i = 0; i < at.count(); ++i) {
        qreal ease = curve.valueForProgress(at.at(i)/qreal(100));
        int ex = expected.at(i);
        QCOMPARE(int(100*ease), ex);
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
