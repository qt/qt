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
#include <QtTest/QSignalSpy>
#include <qdeclarativeview.h>
#include <private/qdeclarativeparticles_p.h>

class tst_QmlGraphicsParticles : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsParticles();

private slots:
    void properties();
    void motionGravity();
    void motionWander();
    void runs();
private:
    QDeclarativeView *createView(const QString &filename);

};

tst_QmlGraphicsParticles::tst_QmlGraphicsParticles()
{
}

void tst_QmlGraphicsParticles::properties()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particles.qml");
    QVERIFY(canvas->rootObject());
    QDeclarativeParticles* particles = canvas->rootObject()->findChild<QDeclarativeParticles*>("particles");
    QVERIFY(particles);

    particles->setSource(QUrl::fromLocalFile(SRCDIR "/data/particle.png"));
    QCOMPARE(particles->source(), QUrl::fromLocalFile(SRCDIR "/data/particle.png"));

    particles->setLifeSpanDeviation(1000);
    QCOMPARE(particles->lifeSpanDeviation(), 1000);

    particles->setFadeInDuration(1000);
    QCOMPARE(particles->fadeInDuration(), 1000);

    particles->setFadeOutDuration(1000);
    QCOMPARE(particles->fadeOutDuration(), 1000);

    particles->setAngle(100.0);
    QCOMPARE(particles->angle(), 100.0);

    particles->setAngleDeviation(100.0);
    QCOMPARE(particles->angleDeviation(), 100.0);

    particles->setVelocity(100.0);
    QCOMPARE(particles->velocity(), 100.0);

    particles->setVelocityDeviation(100.0);
    QCOMPARE(particles->velocityDeviation(), 100.0);

    particles->setEmissionVariance(0.5);
    QCOMPARE(particles->emissionVariance(),0.5);

    particles->setEmissionRate(12);
    QCOMPARE(particles->emissionRate(), 12);
}

void tst_QmlGraphicsParticles::motionGravity()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlemotion.qml");
    QVERIFY(canvas->rootObject());
    QDeclarativeParticles* particles = canvas->rootObject()->findChild<QDeclarativeParticles*>("particles");
    QVERIFY(particles);

    QDeclarativeParticleMotionGravity* motionGravity = canvas->rootObject()->findChild<QDeclarativeParticleMotionGravity*>("motionGravity");
    QCOMPARE(particles->motion(), motionGravity);

    QSignalSpy xattractorSpy(motionGravity, SIGNAL(xattractorChanged()));
    QSignalSpy yattractorSpy(motionGravity, SIGNAL(yattractorChanged()));
    QSignalSpy accelerationSpy(motionGravity, SIGNAL(accelerationChanged()));

    QCOMPARE(motionGravity->xAttractor(), 0.0);
    QCOMPARE(motionGravity->yAttractor(), 1000.0);
    QCOMPARE(motionGravity->acceleration(), 25.0);

    motionGravity->setXAttractor(20.0);
    motionGravity->setYAttractor(10.0);
    motionGravity->setAcceleration(10.0);

    QCOMPARE(motionGravity->xAttractor(), 20.0);
    QCOMPARE(motionGravity->yAttractor(), 10.0);
    QCOMPARE(motionGravity->acceleration(), 10.0);

    QCOMPARE(xattractorSpy.count(), 1);
    QCOMPARE(yattractorSpy.count(), 1);
    QCOMPARE(accelerationSpy.count(), 1);

    motionGravity->setXAttractor(20.0);
    motionGravity->setYAttractor(10.0);
    motionGravity->setAcceleration(10.0);

    QCOMPARE(xattractorSpy.count(), 1);
    QCOMPARE(yattractorSpy.count(), 1);
    QCOMPARE(accelerationSpy.count(), 1);
}

void tst_QmlGraphicsParticles::motionWander()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particlemotion.qml");
    QVERIFY(canvas->rootObject());
    QDeclarativeParticles* particles = canvas->rootObject()->findChild<QDeclarativeParticles*>("particles");
    QVERIFY(particles);
    
    QSignalSpy motionSpy(particles, SIGNAL(motionChanged()));
    QDeclarativeParticleMotionWander* motionWander = canvas->rootObject()->findChild<QDeclarativeParticleMotionWander*>("motionWander");
    
    particles->setMotion(motionWander);
    QCOMPARE(particles->motion(),motionWander);
    QCOMPARE(motionSpy.count(), 1);
    
    particles->setMotion(motionWander);
    QCOMPARE(motionSpy.count(), 1);

    QSignalSpy xvarianceSpy(motionWander, SIGNAL(xvarianceChanged()));
    QSignalSpy yvarianceSpy(motionWander, SIGNAL(yvarianceChanged()));
    QSignalSpy paceSpy(motionWander, SIGNAL(paceChanged()));

    QCOMPARE(motionWander->xVariance(), 30.0);
    QCOMPARE(motionWander->yVariance(), 30.0);
    QCOMPARE(motionWander->pace(), 100.0);

    motionWander->setXVariance(20.0);
    motionWander->setYVariance(10.0);
    motionWander->setPace(10.0);

    QCOMPARE(motionWander->xVariance(), 20.0);
    QCOMPARE(motionWander->yVariance(), 10.0);
    QCOMPARE(motionWander->pace(), 10.0);

    QCOMPARE(xvarianceSpy.count(), 1);
    QCOMPARE(yvarianceSpy.count(), 1);
    QCOMPARE(paceSpy.count(), 1);

    motionWander->setXVariance(20.0);
    motionWander->setYVariance(10.0);
    motionWander->setPace(10.0);

    QCOMPARE(xvarianceSpy.count(), 1);
    QCOMPARE(yvarianceSpy.count(), 1);
    QCOMPARE(paceSpy.count(), 1);
}

void tst_QmlGraphicsParticles::runs()
{
    QDeclarativeView *canvas = createView(SRCDIR "/data/particles.qml");
    QVERIFY(canvas->rootObject());
    QDeclarativeParticles* particles = canvas->rootObject()->findChild<QDeclarativeParticles*>("particles");
    QVERIFY(particles);
    QTest::qWait(1000);//Run for one second. Test passes if it doesn't crash.
}

QDeclarativeView *tst_QmlGraphicsParticles::createView(const QString &filename)
{
    QDeclarativeView *canvas = new QDeclarativeView(0);
    canvas->setFixedSize(240,320);

    canvas->setSource(QUrl::fromLocalFile(filename));

    return canvas;
}
QTEST_MAIN(tst_QmlGraphicsParticles)

#include "tst_qdeclarativeparticles.moc"
