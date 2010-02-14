/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlgraphicsparticles_p.h"

#include "qmlgraphicsitem_p.h"

#include <qmlpixmapcache_p.h>
#include <qfxperf_p_p.h>
#include <qmlanimation_p_p.h>

#include <QNetworkReply>
#include <QPainter>
#include <QtGui/qdrawutil.h>
#include <QVarLengthArray>

#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI_2 (M_PI / 2.)
#endif
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

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

class QmlGraphicsParticle
{
public:
    QmlGraphicsParticle(int time) : lifeSpan(1000), fadeOutAge(800)
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

/*!
    \class QmlGraphicsParticleMotion
    \ingroup group_effects
    \brief The QmlGraphicsParticleMotion class is the base class for particle motion.
    \internal

    This class causes the particles to remain static.
*/

/*!
    Constructs a QmlGraphicsParticleMotion with parent object \a parent.
*/
QmlGraphicsParticleMotion::QmlGraphicsParticleMotion(QObject *parent) :
    QObject(parent)
{
}

/*!
    Move the \a particle to its new position.  \a interval is the number of
    milliseconds elapsed since it was last moved.
*/
void QmlGraphicsParticleMotion::advance(QmlGraphicsParticle &particle, int interval)
{
    Q_UNUSED(particle);
    Q_UNUSED(interval);
}

/*!
    The \a particle has just been created.  Some motion strategies require
    additional state information.  This can be allocated by this function.
*/
void QmlGraphicsParticleMotion::created(QmlGraphicsParticle &particle)
{
    Q_UNUSED(particle);
}

/*!
    The \a particle is about to be destroyed.  Any additional memory
    that has been allocated for the particle should be freed.
*/
void QmlGraphicsParticleMotion::destroy(QmlGraphicsParticle &particle)
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
    \class QmlGraphicsParticleMotionLinear
    \ingroup group_effects
    \brief The QmlGraphicsParticleMotionLinear class moves the particles linearly.
*/

void QmlGraphicsParticleMotionLinear::advance(QmlGraphicsParticle &p, int interval)
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
    \class QmlGraphicsParticleMotionGravity
    \ingroup group_effects
    \brief The QmlGraphicsParticleMotionGravity class moves the particles towards a point.
*/

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
    \property QmlGraphicsParticleMotionGravity::xattractor
    \brief the x coordinate of the point attracting the particles.
*/

/*!
    \property QmlGraphicsParticleMotionGravity::yattractor
    \brief the y coordinate of the point attracting the particles.
*/

/*!
    \property QmlGraphicsParticleMotionGravity::acceleration
    \brief the acceleration to apply to the particles.
*/

void QmlGraphicsParticleMotionGravity::advance(QmlGraphicsParticle &p, int interval)
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
Rectangle {
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
    \class QmlGraphicsParticleMotionWander
    \ingroup group_effects
    \brief The QmlGraphicsParticleMotionWander class moves particles in a somewhat random fashion.

    The particles will continue roughly in the original direction, however will randomly
    drift to each side.
*/

/*!
    \qmlproperty int QmlGraphicsParticleMotionWander::xvariance
    \qmlproperty int QmlGraphicsParticleMotionWander::yvariance

    These properties set the amount to wander in the x and y directions.
*/

/*!
    \qmlproperty int QmlGraphicsParticleMotionWander::pace
    This property holds how quickly the paricles will move from side to side.
*/

void QmlGraphicsParticleMotionWander::advance(QmlGraphicsParticle &p, int interval)
{
    if (!particles)
        particles = qobject_cast<QmlGraphicsParticles*>(parent());
    if (particles) {
        Data *d = (Data*)p.data;
        if (_xvariance != 0.) {
            qreal xdiff = p.x_velocity - d->x_targetV;
            if ((xdiff > d->x_peak && d->x_var > 0.0) || (xdiff < -d->x_peak && d->x_var < 0.0)) {
                d->x_var = -d->x_var;
                d->x_peak = _xvariance + _xvariance * qreal(qrand()) / RAND_MAX;
            }
            p.x_velocity += d->x_var * interval;
        }
        p.x += interval * p.x_velocity;

        if (_yvariance != 0.) {
            qreal ydiff = p.y_velocity - d->y_targetV;
            if ((ydiff > d->y_peak && d->y_var > 0.0) || (ydiff < -d->y_peak && d->y_var < 0.0)) {
                d->y_var = -d->y_var;
                d->y_peak = _yvariance + _yvariance * qreal(qrand()) / RAND_MAX;
            }
            p.y_velocity += d->y_var * interval;
        }
        p.y += interval * p.y_velocity;
    }
}

void QmlGraphicsParticleMotionWander::created(QmlGraphicsParticle &p)
{
    if (!p.data) {
        Data *d = new Data;
        p.data = (void*)d;
        d->x_targetV = p.x_velocity;
        d->y_targetV = p.y_velocity;
        d->x_peak = _xvariance;
        d->y_peak = _yvariance;
        d->x_var = _pace * qreal(qrand()) / RAND_MAX / 1000.0;
        d->y_var = _pace * qreal(qrand()) / RAND_MAX / 1000.0;
    }
}

void QmlGraphicsParticleMotionWander::destroy(QmlGraphicsParticle &p)
{
    if (p.data)
        delete (Data*)p.data;
}

//---------------------------------------------------------------------------
class QmlGraphicsParticlesPainter : public QmlGraphicsItem
{
public:
    QmlGraphicsParticlesPainter(QmlGraphicsParticlesPrivate *p, QmlGraphicsItem* parent)
        : QmlGraphicsItem(parent), d(p)
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
    QmlGraphicsParticlesPrivate* d;
};

//---------------------------------------------------------------------------
class QmlGraphicsParticlesPrivate : public QmlGraphicsItemPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsParticles)
public:
    QmlGraphicsParticlesPrivate()
        : count(1), emissionRate(-1), emissionVariance(0.5), lifeSpan(1000)
        , lifeSpanDev(1000), fadeInDur(200), fadeOutDur(300)
        , angle(0), angleDev(0), velocity(0), velocityDev(0), emissionCarry(0.)
        , addParticleTime(0), addParticleCount(0), lastAdvTime(0)
        , motion(0), pendingPixmapCache(false), clock(this)
    {
    }

    ~QmlGraphicsParticlesPrivate()
    {
    }

    void init()
    {
        Q_Q(QmlGraphicsParticles);
        paintItem = new QmlGraphicsParticlesPainter(this, q);
    }

    void tick(int time);
    void createParticle(int time);
    void updateOpacity(QmlGraphicsParticle &p, int age);

    QUrl url;
    QPixmap image;
    int count;
    int emissionRate;
    qreal emissionVariance;
    int lifeSpan;
    int lifeSpanDev;
    int fadeInDur;
    int fadeOutDur;
    qreal angle;
    qreal angleDev;
    qreal velocity;
    qreal velocityDev;
    qreal emissionCarry;
    int addParticleTime;
    int addParticleCount;
    int lastAdvTime;
    QmlGraphicsParticleMotion *motion;
    QmlGraphicsParticlesPainter *paintItem;

    bool pendingPixmapCache;

    QList<QPair<int, int> > bursts;//countLeft, emissionRate pairs
    QList<QmlGraphicsParticle> particles;
    QTickAnimationProxy<QmlGraphicsParticlesPrivate, &QmlGraphicsParticlesPrivate::tick> clock;

};

void QmlGraphicsParticlesPrivate::tick(int time)
{
    Q_Q(QmlGraphicsParticles);
    if (!motion)
        motion = new QmlGraphicsParticleMotionLinear(q);

    int oldCount = particles.count();
    int removed = 0;
    int interval = time - lastAdvTime;
    for (int i = 0; i < particles.count(); ) {
        QmlGraphicsParticle &particle = particles[i];
        int age = time - particle.birthTime;
        if (age >= particle.lifeSpan)  {
            QmlGraphicsParticle part = particles.takeAt(i);
            motion->destroy(part);
            ++removed;
        } else {
            updateOpacity(particle, age);
            motion->advance(particle, interval);
            ++i;
        }
    }

    if(emissionRate == -1)//Otherwise leave emission to the emission rate
        while(removed-- && ((count == -1) || particles.count() < count))
            createParticle(time);

    if (!addParticleTime)
        addParticleTime = time;

    //Possibly emit new particles
    if (((count == -1) || particles.count() < count) && emissionRate
            && !(count==-1 && emissionRate==-1)) {
        int emissionCount = -1;
        if (emissionRate != -1){
            qreal variance = 1.;
            if (emissionVariance > 0.){
                variance += (qreal(qrand())/RAND_MAX) * emissionVariance * (qrand()%2?-1.:1.);
            }
            qreal emission = emissionRate * (qreal(interval)/1000.);
            emission = emission * variance + emissionCarry;
            double tmpDbl;
            emissionCarry = modf(emission, &tmpDbl);
            emissionCount = (int)tmpDbl;
            emissionCount = qMax(0,emissionCount);
        }
        while(((count == -1) || particles.count() < count) &&
                (emissionRate==-1 || emissionCount--))
            createParticle(time);
    }

    //Deal with emissions from requested bursts
    for(int i=0; i<bursts.size(); i++){
        int emission = 0;
        if(bursts[i].second == -1){
            emission = bursts[i].first;
        }else{
            qreal variance = 1.;
            if (emissionVariance > 0.){
                variance += (qreal(qrand())/RAND_MAX) * emissionVariance * (qrand()%2?-1.:1.);
            }
            qreal workingEmission = bursts[i].second * (qreal(interval)/1000.);
            workingEmission *= variance;
            emission = (int)workingEmission;
            emission = qMax(emission, 0);
        }
        emission = qMin(emission, bursts[i].first);
        bursts[i].first -= emission;
        while(emission--)
            createParticle(time);
    }
    for(int i=bursts.size()-1; i>=0; i--)
        if(bursts[i].first <= 0)
            bursts.removeAt(i);

    lastAdvTime = time;
    paintItem->updateSize();
    paintItem->update();
    if (!(oldCount || particles.count()) && (!count || !emissionRate) && bursts.isEmpty()) {
        lastAdvTime = 0;
        clock.stop();
    }
}

void QmlGraphicsParticlesPrivate::createParticle(int time)
{
#ifdef Q_ENABLE_PERFORMANCE_LOG
    QmlPerfTimer<QmlPerf::CreateParticle> x;
#endif
    Q_Q(QmlGraphicsParticles);
    QmlGraphicsParticle p(time);
    p.x = q->x() + q->width() * qreal(qrand()) / RAND_MAX - image.width()/2.0;
    p.y = q->y() + q->height() * qreal(qrand()) / RAND_MAX - image.height()/2.0;
    p.lifeSpan = lifeSpan;
    if (lifeSpanDev)
        p.lifeSpan += int(lifeSpanDev/2 - lifeSpanDev * qreal(qrand()) / RAND_MAX);
    p.fadeOutAge = p.lifeSpan - fadeOutDur;
    if (fadeInDur == 0.) {
        p.state= QmlGraphicsParticle::Solid;
        p.opacity = 1.0;
    }
    qreal a = angle;
    if (angleDev)
        a += angleDev/2 - angleDev * qreal(qrand()) / RAND_MAX;
    if (a > M_PI)
        a = a - 2 * M_PI;
    qreal v = velocity;
    if (velocityDev)
        v += velocityDev/2 - velocityDev * qreal(qrand()) / RAND_MAX;
    p.x_velocity = v * fastCos(a);
    p.y_velocity = v * fastSin(a);
    particles.append(p);
    motion->created(particles.last());
}

void QmlGraphicsParticlesPrivate::updateOpacity(QmlGraphicsParticle &p, int age)
{
    switch (p.state) {
    case QmlGraphicsParticle::FadeIn:
        if (age <= fadeInDur) {
            p.opacity = qreal(age) / fadeInDur;
            break;
        } else {
            p.opacity = 1.0;
            p.state = QmlGraphicsParticle::Solid;
            // Fall through
        }
    case QmlGraphicsParticle::Solid:
        if (age <= p.fadeOutAge) {
            break;
        } else {
            p.state = QmlGraphicsParticle::FadeOut;
            // Fall through
        }
    case QmlGraphicsParticle::FadeOut:
        p.opacity = qreal(p.lifeSpan - age) / fadeOutDur;
        break;
    }
}

/*!
    \qmlclass Particles
    \brief The Particles object generates and moves particles.
    \inherits Item

    This element provides preliminary support for particles in QML, and may be heavily changed or removed in later versions.

    The particles created by this object cannot be dealt with directly, they can only be controlled through the parameters of the Particles object. The particles are all the same pixmap, specified by the user.

    The particles are painted relative to the parent of the Particles object.  Moving the
    Particles object will not move the particles already emitted.

    The below example creates two differently behaving particle sources.
    The top one has particles falling from the top like snow,
    the lower one has particles expelled up like a fountain.

    \qml
Rectangle {
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
    \class QmlGraphicsParticles
    \ingroup group_effects
    \brief The QmlGraphicsParticles class generates and moves particles.
*/

QmlGraphicsParticles::QmlGraphicsParticles(QmlGraphicsItem *parent)
    : QmlGraphicsItem(*(new QmlGraphicsParticlesPrivate), parent)
{
    Q_D(QmlGraphicsParticles);
    d->init();
}

QmlGraphicsParticles::~QmlGraphicsParticles()
{
    Q_D(QmlGraphicsParticles);
    if (d->pendingPixmapCache)
        QmlPixmapCache::cancel(d->url, this);
}

/*!
    \qmlproperty string Particles::src
    This property holds the URL of the particle image.
*/

/*!
    \property QmlGraphicsParticles::source
    \brief the URL of the particle image.
*/
QUrl QmlGraphicsParticles::source() const
{
    Q_D(const QmlGraphicsParticles);
    return d->url;
}

void QmlGraphicsParticles::imageLoaded()
{
    Q_D(QmlGraphicsParticles);
    d->pendingPixmapCache = false;
    QmlPixmapCache::get(d->url, &d->image);
    d->paintItem->updateSize();
    d->paintItem->update();
}

void QmlGraphicsParticles::setSource(const QUrl &name)
{
    Q_D(QmlGraphicsParticles);

    if ((d->url.isEmpty() == name.isEmpty()) && name == d->url)
        return;

    if (d->pendingPixmapCache) {
        QmlPixmapCache::cancel(d->url, this);
        d->pendingPixmapCache = false;
    }
    if (name.isEmpty()) {
        d->url = name;
        d->image = QPixmap();
        d->paintItem->updateSize();
        d->paintItem->update();
    } else {
        d->url = name;
        Q_ASSERT(!name.isRelative());
        QmlPixmapReply::Status status = QmlPixmapCache::get(d->url, &d->image);
        if (status != QmlPixmapReply::Ready && status != QmlPixmapReply::Error) {
            QmlPixmapReply *reply = QmlPixmapCache::request(qmlEngine(this), d->url);
            connect(reply, SIGNAL(finished()), this, SLOT(imageLoaded()));
            d->pendingPixmapCache = true;
        } else {
            //### unify with imageLoaded
            d->paintItem->updateSize();
            d->paintItem->update();
        }
    }
    emit sourceChanged();
}

/*!
    \qmlproperty int Particles::count
    This property holds the maximum number of particles

    The particles element emits particles until it has count active
    particles. When this number is reached, new particles are not emitted until
    some of the current particles reach theend of their lifespan.

    If count is -1 then there is no maximum number of active particles, and
    particles will be constantly emitted at the rate specified by emissionRate.

    If both count and emissionRate are set to -1, nothing will be emitted.

*/

/*!
    \property QmlGraphicsParticles::count
    \brief the maximum number of particles
*/
int QmlGraphicsParticles::count() const
{
    Q_D(const QmlGraphicsParticles);
    return d->count;
}

void QmlGraphicsParticles::setCount(int cnt)
{
    Q_D(QmlGraphicsParticles);
    if (cnt == d->count)
        return;

    int oldCount = d->count;
    d->count = cnt;
    d->addParticleTime = 0;
    d->addParticleCount = d->particles.count();
    if (!oldCount && d->clock.state() != QAbstractAnimation::Running && d->count && d->emissionRate) {
        d->clock.start();
    }
    d->paintItem->updateSize();
    d->paintItem->update();
    emit countChanged();
}


/*!
    \qmlproperty int Particles::emissionRate
    This property holds the target number of particles to emit every second.

    The particles element will emit up to emissionRate particles every
    second. Fewer particles may be emitted per second if the maximum number of
    particles has been reached.

    If emissionRate is set to -1 there is no limit to the number of
    particles emitted per second, and particles will be instantly emitted to
    reach the maximum number of particles specified by count.

    The default value for emissionRate is -1.

    If both count and emissionRate are set to -1, nothing will be emitted.
*/

/*!
    \property QmlGraphicsParticles::emissionRate
    \brief the emission rate of particles
*/
int QmlGraphicsParticles::emissionRate() const
{
    Q_D(const QmlGraphicsParticles);
    return d->emissionRate;
}
void QmlGraphicsParticles::setEmissionRate(int er)
{
    Q_D(QmlGraphicsParticles);
    if(er == d->emissionRate)
        return;
    d->emissionRate = er;
    if (d->clock.state() != QAbstractAnimation::Running && d->count && d->emissionRate) {
            d->clock.start();
    }
    emit emissionRateChanged();
}

/*!
    \qmlproperty qreal Particles::emissionVariance
    This property holds how inconsistent the rate of particle emissions are.
    It is a number between 0 (no variance) and 1 (some variance).

    The expected number of particles emitted per second is emissionRate. If
    emissionVariance is 0 then particles will be emitted consistently throughout
    each second to reach that number. If emissionVariance is greater than 0 the
    rate of particle emission will vary randomly throughout the second, with the
    consequence that the actual number of particles emitted in one second will
    vary randomly as well.

    emissionVariance is the maximum deviation from emitting
    emissionRate particles per second. An emissionVariance of 0 means you should
    get exactly emissionRate particles emitted per second,
    and an emissionVariance of 1 means you will get between zero and two times
    emissionRate particles per second, but you should get emissionRate particles
    per second on average.

    Note that even with an emissionVariance of 0 there may be some variance due
    to performance and hardware constraints.

    The default value of emissionVariance is 0.5
*/

/*!
    \property QmlGraphicsParticles::emissionVariance
    \brief how much the particle emission amounts vary per tick
*/

qreal QmlGraphicsParticles::emissionVariance() const
{
    Q_D(const QmlGraphicsParticles);
    return d->emissionVariance;
}

void QmlGraphicsParticles::setEmissionVariance(qreal ev)
{
    Q_D(QmlGraphicsParticles);
    if(d->emissionVariance == ev)
        return;
    d->emissionVariance = ev;
    emit emissionVarianceChanged();
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
    \property QmlGraphicsParticles::lifeSpan
    \brief the life span of each particle.

    Default value is 1000ms.

    \sa QmlGraphicsParticles::lifeSpanDeviation
*/
int QmlGraphicsParticles::lifeSpan() const
{
    Q_D(const QmlGraphicsParticles);
    return d->lifeSpan;
}

void QmlGraphicsParticles::setLifeSpan(int ls)
{
    Q_D(QmlGraphicsParticles);
    if(d->lifeSpan == ls)
        return;
    d->lifeSpan = ls;
    emit lifeSpanChanged();
}

/*!
    \property QmlGraphicsParticles::lifeSpanDeviation
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

    \sa QmlGraphicsParticles::lifeSpan
*/
int QmlGraphicsParticles::lifeSpanDeviation() const
{
    Q_D(const QmlGraphicsParticles);
    return d->lifeSpanDev;
}

void QmlGraphicsParticles::setLifeSpanDeviation(int dev)
{
    Q_D(QmlGraphicsParticles);
    if(d->lifeSpanDev == dev)
        return;
    d->lifeSpanDev = dev;
    emit lifeSpanDeviationChanged();
}

/*!
    \qmlproperty int Particles::fadeInDuration
    \qmlproperty int Particles::fadeOutDuration
    These properties hold the time taken to fade the particles in and out.

    By default fade in is 200ms and fade out is 300ms.
*/

/*!
    \property QmlGraphicsParticles::fadeInDuration
    \brief the time taken to fade in the particles.

    Default value is 200ms.
*/
int QmlGraphicsParticles::fadeInDuration() const
{
    Q_D(const QmlGraphicsParticles);
    return d->fadeInDur;
}

void QmlGraphicsParticles::setFadeInDuration(int dur)
{
    Q_D(QmlGraphicsParticles);
    if (dur < 0.0 || dur == d->fadeInDur)
        return;
    d->fadeInDur = dur;
    emit fadeInDurationChanged();
}

/*!
    \property QmlGraphicsParticles::fadeOutDuration
    \brief the time taken to fade out the particles.

    Default value is 300ms.
*/
int QmlGraphicsParticles::fadeOutDuration() const
{
    Q_D(const QmlGraphicsParticles);
    return d->fadeOutDur;
}

void QmlGraphicsParticles::setFadeOutDuration(int dur)
{
    Q_D(QmlGraphicsParticles);
    if (dur < 0.0 || d->fadeOutDur == dur)
        return;
    d->fadeOutDur = dur;
    emit fadeOutDurationChanged();
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
    \property QmlGraphicsParticles::angle
    \brief the initial angle of direction.

    \sa QmlGraphicsParticles::angleDeviation
*/
qreal QmlGraphicsParticles::angle() const
{
    Q_D(const QmlGraphicsParticles);
    return d->angle * 180.0 / M_PI;
}

void QmlGraphicsParticles::setAngle(qreal angle)
{
    Q_D(QmlGraphicsParticles);
    qreal radAngle = angle * M_PI / 180.0;
    if(radAngle == d->angle)
        return;
    d->angle = radAngle;
    emit angleChanged();
}

/*!
    \property QmlGraphicsParticles::angleDeviation
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

    \sa QmlGraphicsParticles::angle
*/
qreal QmlGraphicsParticles::angleDeviation() const
{
    Q_D(const QmlGraphicsParticles);
    return d->angleDev * 180.0 / M_PI;
}

void QmlGraphicsParticles::setAngleDeviation(qreal dev)
{
    Q_D(QmlGraphicsParticles);
    qreal radDev = dev * M_PI / 180.0;
    if(radDev == d->angleDev)
        return;
    d->angleDev = radDev;
    emit angleDeviationChanged();
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
    \property QmlGraphicsParticles::velocity
    \brief the initial velocity of the particles.

    \sa QmlGraphicsParticles::velocityDeviation
*/
qreal QmlGraphicsParticles::velocity() const
{
    Q_D(const QmlGraphicsParticles);
    return d->velocity * 1000.0;
}

void QmlGraphicsParticles::setVelocity(qreal velocity)
{
    Q_D(QmlGraphicsParticles);
    qreal realVel = velocity / 1000.0;
    if(realVel == d->velocity)
        return;
    d->velocity = realVel;
    emit velocityChanged();
}

/*!
    \property QmlGraphicsParticles::velocityDeviation
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

    \sa QmlGraphicsParticles::velocity
*/
qreal QmlGraphicsParticles::velocityDeviation() const
{
    Q_D(const QmlGraphicsParticles);
    return d->velocityDev * 1000.0;
}

void QmlGraphicsParticles::setVelocityDeviation(qreal velocity)
{
    Q_D(QmlGraphicsParticles);
    qreal realDev = velocity / 1000.0;
    if(realDev == d->velocityDev)
        return;
    d->velocityDev = realDev;
    emit velocityDeviationChanged();
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
    \property QmlGraphicsParticles::motion
    \brief sets the type of motion to apply to the particles.

    When a particle is created it will have an initial direction and velocity.
    The motion of the particle during its lifeSpan is then influenced by the
    motion property.

    Default motion is QmlGraphicsParticleMotionLinear.
*/
QmlGraphicsParticleMotion *QmlGraphicsParticles::motion() const
{
    Q_D(const QmlGraphicsParticles);
    return d->motion;
}

void QmlGraphicsParticles::setMotion(QmlGraphicsParticleMotion *motion)
{
    Q_D(QmlGraphicsParticles);
    d->motion = motion;
}

/*!
    \qmlmethod Particles::burst(int count, int emissionRate)

    Initiates a burst of particles.

    This method takes two arguments. The first argument is the number
    of particles to emit and the second argument is the emissionRate for the
    burst. If the second argument is omitted, it is treated as -1. The burst
    of particles has a separate emissionRate and count to the normal emission of
    particles. The burst uses the same values as normal emission for all other
    properties, including emissionVariance.

    The normal emission of particles will continue during the burst, however
    the particles created by the burst count towards the maximum number used by
    normal emission. To avoid this behavior, use two Particles elements.

*/
void QmlGraphicsParticles::burst(int count, int emissionRate)
{
    Q_D(QmlGraphicsParticles);
    d->bursts << qMakePair(count, emissionRate);
    if (d->clock.state() != QAbstractAnimation::Running)
        d->clock.start();
}

void QmlGraphicsParticlesPainter::updateSize()
{
    if (!isComponentComplete())
        return;

    const int parentX = parentItem()->x();
    const int parentY = parentItem()->y();
    for (int i = 0; i < d->particles.count(); ++i) {
        const QmlGraphicsParticle &particle = d->particles.at(i);
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

void QmlGraphicsParticles::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    Q_UNUSED(p);
    //painting is done by the ParticlesPainter, so it can have the right size
}

void QmlGraphicsParticlesPainter::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (d->image.isNull() || d->particles.isEmpty())
        return;

    const int myX = x() + parentItem()->x();
    const int myY = y() + parentItem()->y();

    QVarLengthArray<QDrawPixmaps::Data, 256> pixmapData;
    pixmapData.resize(d->particles.count());

    const QRectF sourceRect = d->image.rect();
    qreal halfPWidth = sourceRect.width()/2.;
    qreal halfPHeight = sourceRect.height()/2.;
    for (int i = 0; i < d->particles.count(); ++i) {
        const QmlGraphicsParticle &particle = d->particles.at(i);
        pixmapData[i].point = QPointF(particle.x - myX + halfPWidth, particle.y - myY + halfPHeight);
        pixmapData[i].opacity = particle.opacity;

        //these never change
        pixmapData[i].rotation = 0;
        pixmapData[i].scaleX = 1;
        pixmapData[i].scaleY = 1;
        pixmapData[i].source = sourceRect;
    }
    qDrawPixmaps(p, pixmapData.data(), d->particles.count(), d->image);
}

void QmlGraphicsParticles::componentComplete()
{
    Q_D(QmlGraphicsParticles);
    QmlGraphicsItem::componentComplete();
    if (d->count) {
        d->paintItem->updateSize();
        d->clock.start();
    }
    if (d->lifeSpanDev > d->lifeSpan)
        d->lifeSpanDev = d->lifeSpan;
}

QT_END_NAMESPACE
