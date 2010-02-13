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

#ifndef QMLGRAPHICSPARTICLES_H
#define QMLGRAPHICSPARTICLES_H

#include "qmlgraphicsitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlGraphicsParticle;
class QmlGraphicsParticles;
class Q_DECLARATIVE_EXPORT QmlGraphicsParticleMotion : public QObject
{
    Q_OBJECT
public:
    QmlGraphicsParticleMotion(QObject *parent=0);

    virtual void advance(QmlGraphicsParticle &, int interval);
    virtual void created(QmlGraphicsParticle &);
    virtual void destroy(QmlGraphicsParticle &);
};

class Q_DECLARATIVE_EXPORT QmlGraphicsParticleMotionLinear : public QmlGraphicsParticleMotion
{
    Q_OBJECT
public:
    QmlGraphicsParticleMotionLinear(QObject *parent=0)
        : QmlGraphicsParticleMotion(parent) {}

    virtual void advance(QmlGraphicsParticle &, int interval);
};

class Q_DECLARATIVE_EXPORT QmlGraphicsParticleMotionGravity : public QmlGraphicsParticleMotion
{
    Q_OBJECT

    Q_PROPERTY(int xattractor READ xAttractor WRITE setXAttractor)
    Q_PROPERTY(int yattractor READ yAttractor WRITE setYAttractor)
    Q_PROPERTY(int acceleration READ acceleration WRITE setAcceleration)
public:
    QmlGraphicsParticleMotionGravity(QObject *parent=0)
        : QmlGraphicsParticleMotion(parent), _xAttr(0), _yAttr(0), _accel(0.00005) {}

    int xAttractor() const { return _xAttr; }
    void setXAttractor(int x) { _xAttr = x; }

    int yAttractor() const { return _yAttr; }
    void setYAttractor(int y) { _yAttr = y; }

    int acceleration() const { return int(_accel * 1000000); }
    void setAcceleration(int accel) { _accel = qreal(accel)/1000000.0; }

    virtual void advance(QmlGraphicsParticle &, int interval);

private:
    int _xAttr;
    int _yAttr;
    qreal _accel;
};

class Q_DECLARATIVE_EXPORT QmlGraphicsParticleMotionWander : public QmlGraphicsParticleMotion
{
    Q_OBJECT
public:
    QmlGraphicsParticleMotionWander()
        : QmlGraphicsParticleMotion(), particles(0), _xvariance(0), _yvariance(0) {}

    virtual void advance(QmlGraphicsParticle &, int interval);
    virtual void created(QmlGraphicsParticle &);
    virtual void destroy(QmlGraphicsParticle &);

    struct Data {
        qreal x_targetV;
        qreal y_targetV;
        qreal x_peak;
        qreal y_peak;
        qreal x_var;
        qreal y_var;
    };

    Q_PROPERTY(int xvariance READ xVariance WRITE setXVariance)
    int xVariance() const { return int(_xvariance * 1000); }
    void setXVariance(int var) { _xvariance = var / 1000.0; }

    Q_PROPERTY(int yvariance READ yVariance WRITE setYVariance)
    int yVariance() const { return int(_yvariance * 1000); }
    void setYVariance(int var) { _yvariance = var / 1000.0; }

    Q_PROPERTY(int pace READ pace WRITE setPace)
    int pace() const { return int(_pace * 1000); }
    void setPace(int pace) { _pace = pace / 1000.0; }

private:
    QmlGraphicsParticles *particles;
    qreal _xvariance;
    qreal _yvariance;
    qreal _pace;
};

class QmlGraphicsParticlesPrivate;
class Q_DECLARATIVE_EXPORT QmlGraphicsParticles : public QmlGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(int emissionRate READ emissionRate WRITE setEmissionRate NOTIFY emissionRateChanged)
    Q_PROPERTY(qreal emissionVariance READ emissionVariance WRITE setEmissionVariance NOTIFY emissionVarianceChanged)
    Q_PROPERTY(int lifeSpan READ lifeSpan WRITE setLifeSpan NOTIFY lifeSpanChanged)
    Q_PROPERTY(int lifeSpanDeviation READ lifeSpanDeviation WRITE setLifeSpanDeviation NOTIFY lifeSpanDeviationChanged)
    Q_PROPERTY(int fadeInDuration READ fadeInDuration WRITE setFadeInDuration NOTIFY fadeInDurationChanged)
    Q_PROPERTY(int fadeOutDuration READ fadeOutDuration WRITE setFadeOutDuration NOTIFY fadeOutDurationChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
    Q_PROPERTY(qreal angleDeviation READ angleDeviation WRITE setAngleDeviation NOTIFY angleDeviationChanged)
    Q_PROPERTY(qreal velocity READ velocity WRITE setVelocity NOTIFY velocityChanged)
    Q_PROPERTY(qreal velocityDeviation READ velocityDeviation WRITE setVelocityDeviation NOTIFY velocityDeviationChanged)
    Q_PROPERTY(QmlGraphicsParticleMotion *motion READ motion WRITE setMotion)
    Q_CLASSINFO("DefaultProperty", "motion")

public:
    QmlGraphicsParticles(QmlGraphicsItem *parent=0);
    ~QmlGraphicsParticles();

    QUrl source() const;
    void setSource(const QUrl &);

    int count() const;
    void setCount(int cnt);

    int emissionRate() const;
    void setEmissionRate(int);

    qreal emissionVariance() const;
    void setEmissionVariance(qreal);

    int lifeSpan() const;
    void setLifeSpan(int);

    int lifeSpanDeviation() const;
    void setLifeSpanDeviation(int);

    int fadeInDuration() const;
    void setFadeInDuration(int);

    int fadeOutDuration() const;
    void setFadeOutDuration(int);

    qreal angle() const;
    void setAngle(qreal);

    qreal angleDeviation() const;
    void setAngleDeviation(qreal);

    qreal velocity() const;
    void setVelocity(qreal);

    qreal velocityDeviation() const;
    void setVelocityDeviation(qreal);

    QmlGraphicsParticleMotion *motion() const;
    void setMotion(QmlGraphicsParticleMotion *);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

public Q_SLOTS:
    void burst(int count, int emissionRate=-1);

protected:
    virtual void componentComplete();

Q_SIGNALS:
    void sourceChanged();
    void countChanged();
    void emissionRateChanged();
    void emissionVarianceChanged();
    void lifeSpanChanged();
    void lifeSpanDeviationChanged();
    void fadeInDurationChanged();
    void fadeOutDurationChanged();
    void angleChanged();
    void angleDeviationChanged();
    void velocityChanged();
    void velocityDeviationChanged();
    void emittingChanged();

private Q_SLOTS:
    void imageLoaded();

private:
    Q_DISABLE_COPY(QmlGraphicsParticles)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QmlGraphicsParticles)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlGraphicsParticleMotion)
QML_DECLARE_TYPE(QmlGraphicsParticleMotionLinear)
QML_DECLARE_TYPE(QmlGraphicsParticleMotionGravity)
QML_DECLARE_TYPE(QmlGraphicsParticleMotionWander)
QML_DECLARE_TYPE(QmlGraphicsParticles)

QT_END_HEADER

#endif
