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

#ifndef QFXPARTICLES_H
#define QFXPARTICLES_H

#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QFxParticle;
class QFxParticles;
class Q_DECLARATIVE_EXPORT QFxParticleMotion : public QObject
{
    Q_OBJECT
public:
    QFxParticleMotion(QObject *parent=0);

    virtual void advance(QFxParticle &, int interval);
    virtual void created(QFxParticle &);
    virtual void destroy(QFxParticle &);
};

class Q_DECLARATIVE_EXPORT QFxParticleMotionLinear : public QFxParticleMotion
{
    Q_OBJECT
public:
    QFxParticleMotionLinear(QObject *parent=0)
        : QFxParticleMotion(parent) {}

    virtual void advance(QFxParticle &, int interval);
};

class Q_DECLARATIVE_EXPORT QFxParticleMotionGravity : public QFxParticleMotion
{
    Q_OBJECT

    Q_PROPERTY(int xattractor READ xAttractor WRITE setXAttractor)
    Q_PROPERTY(int yattractor READ yAttractor WRITE setYAttractor)
    Q_PROPERTY(int acceleration READ acceleration WRITE setAcceleration)
public:
    QFxParticleMotionGravity(QObject *parent=0)
        : QFxParticleMotion(parent), _xAttr(0), _yAttr(0), _accel(0.00005) {}

    int xAttractor() const { return _xAttr; }
    void setXAttractor(int x) { _xAttr = x; }

    int yAttractor() const { return _yAttr; }
    void setYAttractor(int y) { _yAttr = y; }

    int acceleration() const { return int(_accel * 1000000); }
    void setAcceleration(int accel) { _accel = qreal(accel)/1000000.0; }

    virtual void advance(QFxParticle &, int interval);

private:
    int _xAttr;
    int _yAttr;
    qreal _accel;
};

class Q_DECLARATIVE_EXPORT QFxParticleMotionWander : public QFxParticleMotion
{
    Q_OBJECT
public:
    QFxParticleMotionWander()
        : QFxParticleMotion(), particles(0), _xvariance(0), _yvariance(0) {}

    virtual void advance(QFxParticle &, int interval);
    virtual void created(QFxParticle &);
    virtual void destroy(QFxParticle &);

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
    QFxParticles *particles;
    qreal _xvariance;
    qreal _yvariance;
    qreal _pace;
};

class QFxParticlesPrivate;
class Q_DECLARATIVE_EXPORT QFxParticles : public QFxItem
{
    Q_OBJECT

    Q_PROPERTY(QUrl source READ source WRITE setSource)
    Q_PROPERTY(int count READ count WRITE setCount)
    Q_PROPERTY(int lifeSpan READ lifeSpan WRITE setLifeSpan)
    Q_PROPERTY(int lifeSpanDeviation READ lifeSpanDeviation WRITE setLifeSpanDeviation)
    Q_PROPERTY(int fadeInDuration READ fadeInDuration WRITE setFadeInDuration)
    Q_PROPERTY(int fadeOutDuration READ fadeOutDuration WRITE setFadeOutDuration)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
    Q_PROPERTY(qreal angleDeviation READ angleDeviation WRITE setAngleDeviation)
    Q_PROPERTY(qreal velocity READ velocity WRITE setVelocity)
    Q_PROPERTY(qreal velocityDeviation READ velocityDeviation WRITE setVelocityDeviation)
    Q_PROPERTY(bool streamIn READ streamIn WRITE setStreamIn)
    Q_PROPERTY(bool emitting READ emitting WRITE setEmitting)
    Q_PROPERTY(QFxParticleMotion *motion READ motion WRITE setMotion)
    Q_CLASSINFO("DefaultProperty", "motion")

public:
    QFxParticles(QFxItem *parent=0);
    ~QFxParticles();

    QUrl source() const;
    void setSource(const QUrl &);

    int count() const;
    void setCount(int cnt);

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

    bool streamIn() const;
    void setStreamIn(bool);

    bool emitting() const;
    void setEmitting(bool);

    QFxParticleMotion *motion() const;
    void setMotion(QFxParticleMotion *);

    virtual QString propertyInfo() const;

    void paintContents(QPainter &p);

protected:
    virtual void componentComplete();
    QFxParticles(QFxParticlesPrivate &dd, QFxItem *parent);

private Q_SLOTS:
    void imageLoaded();

private:
    Q_DISABLE_COPY(QFxParticles)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr, QFxParticles)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxParticleMotion)
QML_DECLARE_TYPE(QFxParticleMotionLinear)
QML_DECLARE_TYPE(QFxParticleMotionGravity)
QML_DECLARE_TYPE(QFxParticleMotionWander)
QML_DECLARE_TYPE(QFxParticles)

QT_END_HEADER

#endif
