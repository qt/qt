/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QGRAPHICSTRANSFORM_H
#define QGRAPHICSTRANSFORM_H

#include <QtCore/QObject>
#include <QtGui/QTransform>
#include <QtGui/QVector3D>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGraphicsItem;
class QGraphicsTransformPrivate;

class Q_GUI_EXPORT QGraphicsTransform : public QObject
{
    Q_OBJECT
public:
    QGraphicsTransform(QObject *parent = 0);
    ~QGraphicsTransform();

    QTransform transform() const;
    virtual void applyTo(QTransform *transform) const = 0;

protected Q_SLOTS:
    void update();

protected:
    QGraphicsTransform(QGraphicsTransformPrivate &p, QObject *parent);
private:
    friend class QGraphicsItem;
    friend class QGraphicsItemPrivate;
    Q_DECLARE_PRIVATE(QGraphicsTransform)
};

class QGraphicsScalePrivate;

class Q_GUI_EXPORT QGraphicsScale : public QGraphicsTransform
{
    Q_OBJECT

    Q_PROPERTY(QPointF origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(qreal xScale READ xScale WRITE setXScale NOTIFY scaleChanged)
    Q_PROPERTY(qreal yScale READ yScale WRITE setYScale NOTIFY scaleChanged)
public:
    QGraphicsScale(QObject *parent = 0);
    ~QGraphicsScale();

    QPointF origin() const;
    void setOrigin(const QPointF &point);

    qreal xScale() const;
    void setXScale(qreal);

    qreal yScale() const;
    void setYScale(qreal);

    void applyTo(QTransform *transform) const;

Q_SIGNALS:
    void originChanged();
    void scaleChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsScale)
};

class QGraphicsRotationPrivate;

class Q_GUI_EXPORT QGraphicsRotation : public QGraphicsTransform
{
    Q_OBJECT

    Q_PROPERTY(QPointF origin READ origin WRITE setOrigin NOTIFY originChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
public:
    QGraphicsRotation(QObject *parent = 0);
    ~QGraphicsRotation();

    QPointF origin() const;
    void setOrigin(const QPointF &point);

    qreal angle() const;
    void setAngle(qreal);

    void applyTo(QTransform *transform) const;

Q_SIGNALS:
    void originChanged();
    void angleChanged();

protected:
    QGraphicsRotation(QGraphicsRotationPrivate &p, QObject *parent);
private:
    Q_DECLARE_PRIVATE(QGraphicsRotation)
};

class QGraphicsRotation3DPrivate;

class Q_GUI_EXPORT QGraphicsRotation3D : public QGraphicsRotation
{
    Q_OBJECT

    Q_PROPERTY(QVector3D axis READ axis WRITE setAxis NOTIFY axisChanged)
public:
    QGraphicsRotation3D(QObject *parent = 0);
    ~QGraphicsRotation3D();

    QVector3D axis();
    void setAxis(const QVector3D &axis);

    void applyTo(QTransform *transform) const;

Q_SIGNALS:
    void axisChanged();

private:
    Q_DECLARE_PRIVATE(QGraphicsRotation3D)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QFXTRANSFORM_H
