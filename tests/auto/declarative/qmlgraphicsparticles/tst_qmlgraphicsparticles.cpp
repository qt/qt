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
#include <qmlview.h>
#include <private/qmlgraphicsparticles_p.h>

class tst_QmlGraphicsParticles : public QObject
{
    Q_OBJECT
public:
    tst_QmlGraphicsParticles();

private slots:
    void properties();
    void runs();
private:
    QmlView *createView(const QString &filename);

};

tst_QmlGraphicsParticles::tst_QmlGraphicsParticles()
{
}

void tst_QmlGraphicsParticles::properties()
{
    QmlView *canvas = createView(SRCDIR "/data/particles.qml");
    QVERIFY(canvas->root());
    QmlGraphicsParticles* particles = canvas->root()->findChild<QmlGraphicsParticles*>("particles");
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

void tst_QmlGraphicsParticles::runs()
{
    QmlView *canvas = createView(SRCDIR "/data/particles.qml");
    QVERIFY(canvas->root());
    QmlGraphicsParticles* particles = canvas->root()->findChild<QmlGraphicsParticles*>("particles");
    QVERIFY(particles);
    QTest::qWait(1000);//Run for one second. Test passes if it doesn't crash.
}

QmlView *tst_QmlGraphicsParticles::createView(const QString &filename)
{
    QmlView *canvas = new QmlView(0);
    canvas->setFixedSize(240,320);

    QFile file(filename);
    file.open(QFile::ReadOnly);
    QString qml = file.readAll();
    canvas->setQml(qml, filename);
    canvas->execute();

    return canvas;
}
QTEST_MAIN(tst_QmlGraphicsParticles)

#include "tst_qmlgraphicsparticles.moc"
