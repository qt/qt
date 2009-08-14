/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "private/qfxitem_p.h"

#include <stdlib.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI_2 (M_PI / 2.)
#endif
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif
#include <qfxpixmapcache.h>
#include <private/qfxperf_p.h>
#include <private/qmlanimation_p.h>
#include <QNetworkReply>

#include "qfxparticles.h"
#include <QPainter>

QT_BEGIN_NAMESPACE
#define PI_SQR 9.8696044
// parabolic approximation
inline qreal fastSin(qreal theta)
{
    const qreal b = 4 / M_PI;
    const qreal c = -4 / PI_SQR;

    qreal y = b * theta + c * theta * qAbs(theta);
    return y;
}

inline qreal fastCos(qreal theta)
{
    theta += M_PI_2;
    if (theta > M_PI)
        theta -= 2 * M_PI;

    return fastSin(theta);
}

class QFxParticle
{
public:
    QFxParticle(int time) : lifeSpan(1000), fadeOutAge(800)
        , opacity(0), birthTime(time), x_velocity(0), y_velocity(0)
        , state(FadeIn), data(0)
    {
    }

    int lifeSpan;
    int fadeOutAge;
    qreal x;
    qreal y;
    qreal opacity;
    int birthTime;
    qreal x_velocity;
    qreal y_velocity;
    enum State { FadeIn, Solid, FadeOut };
    State state;
    void *data;
};

//---------------------------------------------------------------------------

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ParticleMotion,QFxParticleMotion)

/*!
    \class QFxParticleMotion
    \ingroup group_effects
    \brief The QFxParticleMotion class is the base class for particle motion.

    This class causes the particles to remain static.
*/

/*!
    Constructs a QFxParticleMotion with parent object \a parent.
*/
QFxParticleMotion::QFxParticleMotion(QObject *parent) :
    QObject(parent)
{
}

/*!
    Move the \a particle to its new position.  \a interval is the number of
    milliseconds elapsed since it was last moved.
*/
void QFxParticleMotion::advance(QFxParticle &particle, int interval)
{
    Q_UNUSED(particle);
    Q_UNUSED(interval);
}

/*!
    The \a particle has just been created.  Some motion strategies require
    additional state information.  This can be allocated by this function.
*/
void QFxParticleMotion::created(QFxParticle &particle)
{
    Q_UNUSED(particle);
}

/*!
    The \a particle is about to be destroyed.  Any additional memory
    that has been allocated for the particle should be freed.
*/
void QFxParticleMotion::destroy(QFxParticle &particle)
{
    Q_UNUSED(particle);
}

/*!
    \qmlclass ParticleMotionLinear
    \brief The ParticleMotionLinear object moves particles linearly.

    \sa Particles
*/

/*!
    \internal
    \class QFxParticleMotionLinear
    \ingroup group_effects
    \brief The QFxParticleMotionLinear class moves the particles linearly.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ParticleMotionLinear,QFxParticleMotionLinear)

void QFxParticleMotionLinear::advance(QFxParticle &p, int interval)
{
    p.x += interval * p.x_velocity;
    p.y += interval * p.y_velocity;
}

/*!
    \qmlclass ParticleMotionGravity
    \brief The ParticleMotionGravity object moves particles towards a point.

    \sa Particles
*/

/*!
    \internal
    \class QFxParticleMotionGravity
    \ingroup group_effects
    \brief The QFxParticleMotionGravity class moves the particles towards a point.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ParticleMotionGravity,QFxParticleMotionGravity)

/*!
    \qmlproperty int ParticleMotionGravity::xattractor
    \qmlproperty int ParticleMotionGravity::yattractor
    These properties hold the x and y coordinates of the point attracting the particles.
*/

/*!
    \qmlproperty int ParticleMotionGravity::acceleration
    This property holds the acceleration to apply to the particles.
*/

/*!
    \property QFxParticleMotionGravity::xattractor
    \brief the x coordinate of the point attracting the particles.
*/

/*!
    \property QFxParticleMotionGravity::yattractor
    \brief the y coordinate of the point attracting the particles.
*/

/*!
    \property QFxParticleMotionGravity::acceleration
    \brief the acceleration to apply to the particles.
*/

void QFxParticleMotionGravity::advance(QFxParticle &p, int interval)
{
    qreal xdiff = p.x - _xAttr;
    qreal ydiff = p.y - _yAttr;

    qreal xcomp = xdiff / (xdiff + ydiff);
    qreal ycomp = ydiff / (xdiff + ydiff);

    p.x_velocity += xcomp * _accel * interval;
    p.y_velocity += ycomp * _accel * interval;

    p.x += interval * p.x_velocity;
    p.y += interval * p.y_velocity;
}

/*!
    \qmlclass ParticleMotionWander
    \brief The ParticleMotionWander object moves particles in a somewhat random fashion.

    The particles will continue roughly in the original direction, however will randomly
    drift to each side.

    The code below produces an effect similar to falling snow.

    \qml
Rect {
    width: 240
    height: 320
    color: "black"

    Particles {
        y: 0
        width: parent.width
        height: 30
        source: "star.png"
        lifeSpan: 5000
        count: 50
        angle: 70
        angleDeviation: 36
        velocity: 30
        velocityDeviation: 10
        ParticleMotionWander {
            xvariance: 30
            pace: 100
        }
    }
}
    \endqml

    \sa Particles
*/

/*!
    \internal
    \class QFxParticleMotionWander
    \ingroup group_effects
    \brief The QFxParticleMotionWander class moves particles in a somewhat random fashion.

    The particles will continue roughly in the original direction, however will randomly
    drift to each side.
*/

/*!
    \qmlproperty int QFxParticleMotionWander::xvariance
    \qmlproperty int QFxParticleMotionWander::yvariance

    These properties set the amount to wander in the x and y directions.
*/

/*!
    \qmlproperty int QFxParticleMotionWander::pace
    This property holds how quickly the paricles will move from side to side.
*/

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,ParticleMotionWander,QFxParticleMotionWander)

void QFxParticleMotionWander::advance(QFxParticle &p, int interval)
{
    if (!particles)
        particles = qobject_cast<QFxParticles*>(parent());
    if (particles) {
        Data *d = (Data*)p.data;
        if (_xvariance != 0.) {
            qreal xdiff = p.x_velocity - d->x_targetV;
            if ((xdiff > d->x_peak && d->x_var > 0.0) || (xdiff < -d->x_peak && d->x_var < 0.0)) {
                d->x_var = -d->x_var;
                d->x_peak = _xvariance + _xvariance * qreal(rand()) / RAND_MAX;
            }
            p.x_velocity += d->x_var * interval;
        }
        p.x += interval * p.x_velocity;

        if (_yvariance != 0.) {
            qreal ydiff = p.y_velocity - d->y_targetV;
            if ((ydiff > d->y_peak && d->y_var > 0.0) || (ydiff < -d->y_peak && d->y_var < 0.0)) {
                d->y_var = -d->y_var;
                d->y_peak = _yvariance + _yvariance * qreal(rand()) / RAND_MAX;
            }
            p.y_velocity += d->y_var * interval;
        }
        p.y += interval * p.y_velocity;
    }
}

void QFxParticleMotionWander::created(QFxParticle &p)
{
    if (!p.data) {
        Data *d = new Data;
        p.data = (void*)d;
        d->x_targetV = p.x_velocity;
        d->y_targetV = p.y_velocity;
        d->x_peak = _xvariance;
        d->y_peak = _yvariance;
        d->x_var = _pace * qreal(rand()) / RAND_MAX / 1000.0;
        d->y_var = _pace * qreal(rand()) / RAND_MAX / 1000.0;
    }
}

void QFxParticleMotionWander::destroy(QFxParticle &p)
{
    if (p.data)
        delete (Data*)p.data;
}

//---------------------------------------------------------------------------
class QFxParticlesPainter : public QFxItem
{
public:
    QFxParticlesPainter(QFxParticlesPrivate *p, QFxItem* parent)
        : QFxItem(parent), d(p)
    {
        setFlag(QGraphicsItem::ItemHasNoContents, false);
        maxX = minX = maxY = minY = 0;
    }

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    void updateSize();

    qreal maxX;
    qreal minX;
    qreal maxY;
    qreal minY;
    QFxParticlesPrivate* d;
};

//---------------------------------------------------------------------------
class QFxParticlesPrivate : public QFxItemPrivate
{
    Q_DECLARE_PUBLIC(QFxParticles)
public:
    QFxParticlesPrivate()
        : count(1), lifeSpan(1000), lifeSpanDev(1000), fadeInDur(200), fadeOutDur(300)
        , angle(0), angleDev(0), velocity(0), velocityDev(0)
        , addParticleTime(0), addParticleCount(0), lastAdvTime(0), stream(false), streamDelay(0)
        , emitting(true), motion(0), clock(this)
    {
    }

    ~QFxParticlesPrivate()
    {
    }

    void init()
    {
        Q_Q(QFxParticles);
        paintItem = new QFxParticlesPainter(this, q);
    }

    void tick(int time);
    void createParticle(int time);
    void updateOpacity(QFxParticle &p, int age);

    QUrl url;
    QPixmap image;
    int count;
    int lifeSpan;
    int lifeSpanDev;
    int fadeInDur;
    int fadeOutDur;
    qreal angle;
    qreal angleDev;
    qreal velocity;
    qreal velocityDev;
    int addParticleTime;
    int addParticleCount;
    int lastAdvTime;
    bool stream;
    int streamDelay;
    bool emitting;
    QFxParticleMotion *motion;
    QFxParticlesPainter *paintItem;
    QPointer<QNetworkReply> reply;

    QList<QFxParticle> particles;
    QTickAnimationProxy<QFxParticlesPrivate, &QFxParticlesPrivate::tick> clock;

};

void QFxParticlesPrivate::tick(int time)
{
    Q_Q(QFxParticles);
    if (!motion)
        motion = new QFxParticleMotionLinear(q);

    int oldCount = particles.count();
    int removed = 0;
    int interval = time - lastAdvTime;
    for (int i = 0; i < particles.count(); ) {
        QFxParticle &particle = particles[i];
        int age = time - particle.birthTime;
        if (age >= particle.lifeSpan)  {
            QFxParticle part = particles.takeAt(i);
            motion->destroy(part);
            ++removed;
        } else {
            updateOpacity(particle, age);
            motion->advance(particle, interval);
            ++i;
        }
    }

    while(removed-- && particles.count() < count && emitting)
        createParticle(time);

    if (!addParticleTime)
        addParticleTime = time;

    if (particles.count() < count && emitting) {
        qreal perc = (lifeSpanDev <= 0)?(1.):(qreal(time - addParticleTime) / qreal(lifeSpanDev));
        int percCount = addParticleCount + (int)perc * (count - addParticleCount);
        int streamWidth = -1;
        if (stream){
            if (streamDelay > time){
                streamWidth = 0;
            }else{
                int missed = time - streamDelay;
                qreal streamWidthReal = qreal(count)/qreal(lifeSpan);
                if (streamWidthReal < 1){
                    streamDelay = time + (int)(1.0/streamWidthReal);
                    streamWidth = 1;
                    streamWidth += missed/streamDelay;
                }else{
                    streamWidth = qRound(streamWidthReal * (time-lastAdvTime));
               }
            }
        }
        while(particles.count() < count &&
                (!stream || (particles.count() < percCount && streamWidth--)))
            createParticle(time);
    }

    lastAdvTime = time;
    paintItem->updateSize();
    paintItem->update();
    if (!(oldCount || particles.count()) && (!count || !emitting)) {
        lastAdvTime = 0;
        clock.stop();
    }
}

void QFxParticlesPrivate::createParticle(int time)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::CreateParticle> x;
#endif
    Q_Q(QFxParticles);
    QFxParticle p(time);
    p.x = q->x() + q->width() * qreal(rand()) / RAND_MAX - image.width()/2.0;
    p.y = q->y() + q->height() * qreal(rand()) / RAND_MAX - image.height()/2.0;
    p.lifeSpan = lifeSpan;
    if (lifeSpanDev)
        p.lifeSpan += int(lifeSpanDev/2 - lifeSpanDev * qreal(rand()) / RAND_MAX);
    p.fadeOutAge = p.lifeSpan - fadeOutDur;
    if (fadeInDur == 0.) {
        p.state= QFxParticle::Solid;
        p.opacity = 1.0;
    }
    qreal a = angle;
    if (angleDev)
        a += angleDev/2 - angleDev * qreal(rand()) / RAND_MAX;
    if (a > M_PI)
        a = a - 2 * M_PI;
    qreal v = velocity;
    if (velocityDev)
        v += velocityDev/2 - velocityDev * qreal(rand()) / RAND_MAX;
    p.x_velocity = v * fastCos(a);
    p.y_velocity = v * fastSin(a);
    particles.append(p);
    motion->created(particles.last());
}

void QFxParticlesPrivate::updateOpacity(QFxParticle &p, int age)
{
    switch (p.state) {
    case QFxParticle::FadeIn:
        if (age <= fadeInDur) {
            p.opacity = qreal(age) / fadeInDur;
            break;
        } else {
            p.opacity = 1.0;
            p.state = QFxParticle::Solid;
            // Fall through
        }
    case QFxParticle::Solid:
        if (age <= p.fadeOutAge) {
            break;
        } else {
            p.state = QFxParticle::FadeOut;
            // Fall through
        }
    case QFxParticle::FadeOut:
        p.opacity = qreal(p.lifeSpan - age) / fadeOutDur;
        break;
    }
}

QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,Particles,QFxParticles)

/*!
    \qmlclass Particles
    \brief The Particles object generates and moves particles.
    \inherits Item

    The particles created by this object cannot be dealt with directly, they can only be controlled through the parameters of the Particles object. The particles are all the same pixmap, specified by the user.

    The particles are painted relative to the parent of the Particles object.  Moving the
    Particles object will not move the particles already emitted.

    The below example creates two differently behaving particle sources.
    The top one has particles falling from the top like snow,
    the lower one has particles expelled up like a fountain.

    \qml
Rect {
    width: 240
    height: 320
    color: "black"
    Particles {
        y: 0
        width: parent.width
        height: 30
        source: "star.png"
        lifeSpan: 5000
        count: 50
        angle: 70
        angleDeviation: 36
        velocity: 30
        velocityDeviation: 10
        ParticleMotionWander {
            xvariance: 30
            pace: 100
        }
    }
    Particles {
        y: 300
        x: 120
        width: 1
        height: 1
        source: "star.png"
        lifeSpan: 5000
        count: 200
        angle: 270
        angleDeviation: 45
        velocity: 50
        velocityDeviation: 30
        ParticleMotionGravity {
            yattractor: 1000
            xattractor: 0
            acceleration: 25
        }
    }
}
    \endqml
    \image particles.gif
*/

/*!
    \internal
    \class QFxParticles
    \ingroup group_effects
    \brief The QFxParticles class generates and moves particles.
*/

QFxParticles::QFxParticles(QFxItem *parent)
    : QFxItem(*(new QFxParticlesPrivate), parent)
{
    Q_D(QFxParticles);
    d->init();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxParticles::QFxParticles(QFxParticlesPrivate &dd, QFxItem *parent)
    : QFxItem(dd, parent)
{
    Q_D(QFxParticles);
    d->init();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QFxParticles::~QFxParticles()
{
    Q_D(QFxParticles);
    if (!d->url.isEmpty())
        QFxPixmapCache::cancelGet(d->url, this);
}

/*!
    \qmlproperty string Particles::src
    This property holds the URL of the particle image.
*/

/*!
    \property QFxParticles::source
    \brief the URL of the particle image.
*/
QUrl QFxParticles::source() const
{
    Q_D(const QFxParticles);
    return d->url;
}

void QFxParticles::imageLoaded()
{
    Q_D(QFxParticles);
    QFxPixmapCache::find(d->url, &d->image);
    d->paintItem->updateSize();
    d->paintItem->update();
}

void QFxParticles::setSource(const QUrl &name)
{
    Q_D(QFxParticles);

    if ((d->url.isEmpty() == name.isEmpty()) && name == d->url)
        return;

    if (!d->url.isEmpty())
        QFxPixmapCache::cancelGet(d->url, this);
    if (name.isEmpty()) {
        d->url = name;
        d->image = QPixmap();
        d->paintItem->updateSize();
        d->paintItem->update();
    } else {
        d->url = name;
        Q_ASSERT(!name.isRelative());
        d->reply = QFxPixmapCache::get(qmlEngine(this), d->url, &d->image);
        if (d->reply)
            connect(d->reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
        else {
            //### unify with imageLoaded
            d->paintItem->updateSize();
            d->paintItem->update();
        }
    }
}

/*!
    \qmlproperty int Particles::count
    This property holds the target number of particles
*/

/*!
    \property QFxParticles::count
    \brief the target number of particles
*/
int QFxParticles::count() const
{
    Q_D(const QFxParticles);
    return d->count;
}

void QFxParticles::setCount(int cnt)
{
    Q_D(QFxParticles);
    if (cnt == d->count)
        return;

    int oldCount = d->count;
    d->count = cnt;
    d->addParticleTime = 0;
    d->addParticleCount = d->particles.count();
    if (!oldCount && d->clock.state() != QAbstractAnimation::Running && d->count) {
        d->clock.start();
    }
    d->paintItem->updateSize();
    d->paintItem->update();
}

/*!
    \qmlproperty int Particles::lifeSpan
    \qmlproperty int Particles::lifeSpanDeviation

    These properties describe the life span of each particle.

    The default lifespan for a particle is 1000ms.

    lifeSpanDeviation randomly varies the lifeSpan up to the specified variation.  For
    example, the following creates particles whose lifeSpan will vary
    from 150ms to 250ms:

    \qml
Particles {
    source: "star.png"
    lifeSpan: 200
    lifeSpanDeviation: 100
}
    \endqml
*/

/*!
    \property QFxParticles::lifeSpan
    \brief the life span of each particle.

    Default value is 1000ms.

    \sa QFxParticles::lifeSpanDeviation
*/
int QFxParticles::lifeSpan() const
{
    Q_D(const QFxParticles);
    return d->lifeSpan;
}

void QFxParticles::setLifeSpan(int ls)
{
    Q_D(QFxParticles);
    d->lifeSpan = ls;
}

/*!
    \property QFxParticles::lifeSpanDeviation
    \brief the maximum possible deviation from the set lifeSpan.

    Randomly varies the lifeSpan up to the specified variation.  For
    example, the following creates particles whose lifeSpan will vary
    from 150ms to 250ms:

\qml
Particles {
    source: "star.png"
    lifeSpan: 200
    lifeSpanDeviation: 100
}
\endqml

    \sa QFxParticles::lifeSpan
*/
int QFxParticles::lifeSpanDeviation() const
{
    Q_D(const QFxParticles);
    return d->lifeSpanDev;
}

void QFxParticles::setLifeSpanDeviation(int dev)
{
    Q_D(QFxParticles);
    d->lifeSpanDev = dev;
}

/*!
    \qmlproperty int Particles::fadeInDuration
    \qmlproperty int Particles::fadeOutDuration
    These properties hold the time taken to fade the particles in and out.

    By default fade in is 200ms and fade out is 300ms.
*/

/*!
    \property QFxParticles::fadeInDuration
    \brief the time taken to fade in the particles.

    Default value is 200ms.
*/
int QFxParticles::fadeInDuration() const
{
    Q_D(const QFxParticles);
    return d->fadeInDur;
}

void QFxParticles::setFadeInDuration(int dur)
{
    Q_D(QFxParticles);
    if (dur >= 0.0)
        d->fadeInDur = dur;
}

/*!
    \property QFxParticles::fadeOutDuration
    \brief the time taken to fade out the particles.

    Default value is 300ms.
*/
int QFxParticles::fadeOutDuration() const
{
    Q_D(const QFxParticles);
    return d->fadeOutDur;
}

void QFxParticles::setFadeOutDuration(int dur)
{
    Q_D(QFxParticles);
    if (dur >= 0.0)
        d->fadeOutDur = dur;
}

/*!
    \qmlproperty real Particles::angle
    \qmlproperty real Particles::angleDeviation

    These properties control particle direction.

    angleDeviation randomly varies the direction up to the specified variation.  For
    example, the following creates particles whose initial direction will
    vary from 15 degrees to 105 degrees:

    \qml
Particles {
    source: "star.png"
    angle: 60
    angleDeviation: 90
}
    \endqml
*/

/*!
    \property QFxParticles::angle
    \brief the initial angle of direction.

    \sa QFxParticles::angleDeviation
*/
qreal QFxParticles::angle() const
{
    Q_D(const QFxParticles);
    return d->angle * 180.0 / M_PI;
}

void QFxParticles::setAngle(qreal angle)
{
    Q_D(QFxParticles);
    d->angle = angle * M_PI / 180.0;
}

/*!
    \property QFxParticles::angleDeviation
    \brief the maximum possible deviation from the set angle.

    Randomly varies the direction up to the specified variation.  For
    example, the following creates particles whose initial direction will
    vary from 15 degrees to 105 degrees:

\qml
Particles {
    source: "star.png"
    angle: 60
    angleDeviation: 90
}
\endqml

    \sa QFxParticles::angle
*/
qreal QFxParticles::angleDeviation() const
{
    Q_D(const QFxParticles);
    return d->angleDev * 180.0 / M_PI;
}

void QFxParticles::setAngleDeviation(qreal dev)
{
    Q_D(QFxParticles);
    d->angleDev = dev * M_PI / 180.0;;
}

/*!
    \qmlproperty real Particles::velocity
    \qmlproperty real Particles::velocityDeviation

    These properties control the velocity of the particles.

    velocityDeviation randomly varies the velocity up to the specified variation.  For
    example, the following creates particles whose initial velocity will
    vary from 40 to 60.

    \qml
Particles {
    source: "star.png"
    velocity: 50
    velocityDeviation: 20
}
    \endqml
*/

/*!
    \property QFxParticles::velocity
    \brief the initial velocity of the particles.

    \sa QFxParticles::velocityDeviation
*/
qreal QFxParticles::velocity() const
{
    Q_D(const QFxParticles);
    return d->velocity * 1000.0;
}

void QFxParticles::setVelocity(qreal velocity)
{
    Q_D(QFxParticles);
    d->velocity = velocity / 1000.0;
}

/*!
    \property QFxParticles::velocityDeviation
    \brief the maximum possible deviation from the set velocity.

    Randomly varies the velocity up to the specified variation.  For
    example, the following creates particles whose initial velocity will
    vary from 40 to 60.

\qml
Particles {
    source: "star.png"
    velocity: 50
    velocityDeviation: 20
}
\endqml

    \sa QFxParticles::velocity
*/
qreal QFxParticles::velocityDeviation() const
{
    Q_D(const QFxParticles);
    return d->velocityDev * 1000.0;
}

void QFxParticles::setVelocityDeviation(qreal velocity)
{
    Q_D(QFxParticles);
    d->velocityDev = velocity / 1000.0;
}

/*!
  \qmlproperty bool Particles::streamIn
  This property determines whether the particles stream in at a constant rate

  When stream is set to true the particles will stream in at a constant rate.
  Otherwise the particles will appear as a clump. Note that this only affects the
  start of the particle effect, variables such as lifespan deviation can cause the
  particles to unclump over time.
*/
/*!
  \property QFxParticles::streamIn
  \brief determines whether the particles stream in at a constant rate

  When stream is set to true the particles will stream in at a constant rate.
  Otherwise the particles will appear as a clump. Note that this only affects the
  start of the particle effect, variables such as lifespan deviation can cause the

*/
//The name may need a rethink
bool QFxParticles::streamIn() const
{
    Q_D(const QFxParticles);
    return d->stream;
}

void QFxParticles::setStreamIn(bool b)
{
    Q_D(QFxParticles);
    d->stream = b;
}

/*!
  \qmlproperty bool Particles::emitting
  This property determines whether new particles are created

  If emitting is set to false no new particles will be created. This means that
  when a particle reaches the end of its lifespan it is not replaced. This
  property can be used to turn particles on and off with a more natural look.

  The default setting is true. Note that if it initialized to false no particles
  will be produced until it is set to true.
*/
/*!
  \property QFxParticles::emitting
  If emitting is set to false no new particles will be created. This means that
  when a particle reaches the end of its lifespan it is not replaced. This
  property can be used to turn particles on and off with a more natural look.

  The default setting is true. Note that if it initialized to false no particles
  will be produced until it is set to true.
*/
bool QFxParticles::emitting() const
{
    Q_D(const QFxParticles);
    return d->emitting;
}

void QFxParticles::setEmitting(bool r)
{
    Q_D(QFxParticles);
    d->emitting = r;
    if (d->count && r)
        d->clock.start();
}
/*!
    \qmlproperty ParticleMotion Particles::motion
    This property sets the type of motion to apply to the particles.

    When a particle is created it will have an initial direction and velocity.
    The motion of the particle during its lifeSpan is then influenced by the
    motion property.

    Default motion is ParticleMotionLinear.
*/

/*!
    \property QFxParticles::motion
    \brief sets the type of motion to apply to the particles.

    When a particle is created it will have an initial direction and velocity.
    The motion of the particle during its lifeSpan is then influenced by the
    motion property.

    Default motion is QFxParticleMotionLinear.
*/
QFxParticleMotion *QFxParticles::motion() const
{
    Q_D(const QFxParticles);
    return d->motion;
}

void QFxParticles::setMotion(QFxParticleMotion *motion)
{
    Q_D(QFxParticles);
    d->motion = motion;
}

void QFxParticlesPainter::updateSize()
{
    if (!isComponentComplete())
        return;

    const int parentX = parentItem()->x();
    const int parentY = parentItem()->y();
    for (int i = 0; i < d->particles.count(); ++i) {
        const QFxParticle &particle = d->particles.at(i);
        if(particle.x > maxX)
            maxX = particle.x;
        if(particle.x < minX)
            minX = particle.x;
        if(particle.y > maxY)
            maxY = particle.y;
        if(particle.y < minY)
            minY = particle.y;
    }

    int myWidth = (int)(maxX-minX+0.5)+d->image.width();
    int myX = (int)(minX - parentX);
    int myHeight = (int)(maxY-minY+0.5)+d->image.height();
    int myY = (int)(minY - parentY);
    setWidth(myWidth);
    setHeight(myHeight);
    setX(myX);
    setY(myY);
}

void QFxParticles::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_UNUSED(p);
    //painting is done by the ParticlesPainter, so it can have the right size
}

void QFxParticlesPainter::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (d->image.isNull())
        return;

    const int myX = x() + parentItem()->x();
    const int myY = y() + parentItem()->y();

    for (int i = 0; i < d->particles.count(); ++i) {
        const QFxParticle &particle = d->particles.at(i);
        p->setOpacity(particle.opacity);
        p->drawPixmap(particle.x - myX, particle.y - myY, d->image);
    }
}

void QFxParticles::componentComplete()
{
    Q_D(QFxParticles);
    QFxItem::componentComplete();
    if (d->count) {
        d->paintItem->updateSize();
        d->clock.start();
    }
    if (d->lifeSpanDev > d->lifeSpan)
        d->lifeSpanDev = d->lifeSpan;
}

QT_END_NAMESPACE
