/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtTest/QSignalSpy>
#include <qdeclarativeview.h>
#include <QGraphicsObject>

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

class tst_QDeclarativeParticles : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeParticles();

private slots:
    void properties();
    void motionGravity();
    void motionWander();
    void runs();
private:
    QDeclarativeView *createView(const QString &filename);

};

tst_QDeclarativeParticles::tst_QDeclarativeParticles()
{
}

void tst_QDeclarativeParticles::properties()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlestest.qml");
    QVERIFY(canvas->rootObject());

    QObject* particles = canvas->rootObject()->findChild<QObject*>("particles");
    QVERIFY(particles);

    particles->setProperty("source", QUrl::fromLocalFile(SRCDIR "/data/particle.png"));
    QCOMPARE(particles->property("source").toUrl(), QUrl::fromLocalFile(SRCDIR "/data/particle.png"));

    particles->setProperty("lifeSpanDeviation", (1000));
    QCOMPARE(particles->property("lifeSpanDeviation").toInt(), 1000);

    particles->setProperty("fadeInDuration", 1000);
    QCOMPARE(particles->property("fadeInDuration").toInt(), 1000);

    particles->setProperty("fadeOutDuration", 1000);
    QCOMPARE(particles->property("fadeOutDuration").toInt(), 1000);

    particles->setProperty("angle", 100.0);
    QCOMPARE(particles->property("angle").toDouble(), 100.0);

    particles->setProperty("angleDeviation", 100.0);
    QCOMPARE(particles->property("angleDeviation").toDouble(), 100.0);

    particles->setProperty("velocity", 100.0);
    QCOMPARE(particles->property("velocity").toDouble(), 100.0);

    particles->setProperty("velocityDeviation", 100.0);
    QCOMPARE(particles->property("velocityDeviation").toDouble(), 100.0);

    particles->setProperty("emissionVariance", 0.5);
    QCOMPARE(particles->property("emissionVariance").toDouble(),0.5);

    particles->setProperty("emissionRate", 12);
    QCOMPARE(particles->property("emissionRate").toInt(), 12);
}

void tst_QDeclarativeParticles::motionGravity()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlemotiontest.qml");
    QVERIFY(canvas->rootObject());

    QObject* particles = canvas->rootObject()->findChild<QObject*>("particles");
    QVERIFY(particles);

    QObject* motionGravity = canvas->rootObject()->findChild<QObject*>("motionGravity");
    //QCOMPARE(qvariant_cast<QObject*>(particles->property("motion")), motionGravity);

    QSignalSpy xattractorSpy(motionGravity, SIGNAL(xattractorChanged()));
    QSignalSpy yattractorSpy(motionGravity, SIGNAL(yattractorChanged()));
    QSignalSpy accelerationSpy(motionGravity, SIGNAL(accelerationChanged()));

    QCOMPARE(motionGravity->property("xattractor").toDouble(), 0.0);
    QCOMPARE(motionGravity->property("yattractor").toDouble(), 1000.0);
    QCOMPARE(motionGravity->property("acceleration").toDouble(), 25.0);

    motionGravity->setProperty("xattractor", 20.0);
    motionGravity->setProperty("yattractor", 10.0);
    motionGravity->setProperty("acceleration", 10.0);

    QCOMPARE(motionGravity->property("xattractor").toDouble(), 20.0);
    QCOMPARE(motionGravity->property("yattractor").toDouble(), 10.0);
    QCOMPARE(motionGravity->property("acceleration").toDouble(), 10.0);

    QCOMPARE(xattractorSpy.count(), 1);
    QCOMPARE(yattractorSpy.count(), 1);
    QCOMPARE(accelerationSpy.count(), 1);

    motionGravity->setProperty("xattractor", 20.0);
    motionGravity->setProperty("yattractor", 10.0);
    motionGravity->setProperty("acceleration", 10.0);

    QCOMPARE(xattractorSpy.count(), 1);
    QCOMPARE(yattractorSpy.count(), 1);
    QCOMPARE(accelerationSpy.count(), 1);
}

void tst_QDeclarativeParticles::motionWander()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlemotiontest.qml");
    QVERIFY(canvas->rootObject());

    QObject* particles = canvas->rootObject()->findChild<QObject*>("particles");
    QVERIFY(particles);

    QSignalSpy motionSpy(particles, SIGNAL(motionChanged()));
    QObject* motionWander = canvas->rootObject()->findChild<QObject*>("motionWander");

    QCOMPARE(motionSpy.count(), 0);
    particles->setProperty("motion", QVariant::fromValue(motionWander));
    //QCOMPARE(particles->property("motion"), QVariant::fromValue(motionWander));
    //QCOMPARE(motionSpy.count(), 1);

    particles->setProperty("motion", QVariant::fromValue(motionWander));
    //QCOMPARE(motionSpy.count(), 1);

    QSignalSpy xvarianceSpy(motionWander, SIGNAL(xvarianceChanged()));
    QSignalSpy yvarianceSpy(motionWander, SIGNAL(yvarianceChanged()));
    QSignalSpy paceSpy(motionWander, SIGNAL(paceChanged()));

    QCOMPARE(motionWander->property("xvariance").toDouble(), 30.0);
    QCOMPARE(motionWander->property("yvariance").toDouble(), 30.0);
    QCOMPARE(motionWander->property("pace").toDouble(), 100.0);

    motionWander->setProperty("xvariance", 20.0);
    motionWander->setProperty("yvariance", 10.0);
    motionWander->setProperty("pace", 10.0);

    QCOMPARE(motionWander->property("xvariance").toDouble(), 20.0);
    QCOMPARE(motionWander->property("yvariance").toDouble(), 10.0);
    QCOMPARE(motionWander->property("pace").toDouble(), 10.0);

    QCOMPARE(xvarianceSpy.count(), 1);
    QCOMPARE(yvarianceSpy.count(), 1);
    QCOMPARE(paceSpy.count(), 1);

    QCOMPARE(motionWander->property("xvariance").toDouble(), 20.0);
    QCOMPARE(motionWander->property("yvariance").toDouble(), 10.0);
    QCOMPARE(motionWander->property("pace").toDouble(), 10.0);

    QCOMPARE(xvarianceSpy.count(), 1);
    QCOMPARE(yvarianceSpy.count(), 1);
    QCOMPARE(paceSpy.count(), 1);
}

void tst_QDeclarativeParticles::runs()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlestest.qml");
    QVERIFY(canvas->rootObject());

    QObject* particles = canvas->rootObject()->findChild<QObject*>("particles");
    QVERIFY(particles);
    QTest::qWait(1000);//Run for one second. Test passes if it doesn't crash.
}

QDeclarativeView *tst_QDeclarativeParticles::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}
QTEST_MAIN(tst_QDeclarativeParticles)

#include "tst_qdeclarativeparticles.moc"
