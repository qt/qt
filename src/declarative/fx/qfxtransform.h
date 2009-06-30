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

#ifndef QFXTRANSFORM_H
#define QFXTRANSFORM_H

#include <QtCore/QObject>
#include <QtGui/QTransform>
#if defined(QFX_RENDER_OPENGL)
#include <QtGui/qmatrix4x4.h>
#endif
#include <QtDeclarative/qfxitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QFxTransform : public QObject
{
    Q_OBJECT
public:
    QFxTransform(QObject *parent=0);
    ~QFxTransform();

    void update();

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;
};

class Q_DECLARATIVE_EXPORT QFxAxis : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal startX READ startX WRITE setStartX)
    Q_PROPERTY(qreal startY READ startY WRITE setStartY)
    Q_PROPERTY(qreal endX READ endX WRITE setEndX)
    Q_PROPERTY(qreal endY READ endY WRITE setEndY)
    Q_PROPERTY(qreal endZ READ endZ WRITE setEndZ)
public:
    QFxAxis(QObject *parent=0);
    ~QFxAxis();

    qreal startX() const;
    void setStartX(qreal);

    qreal startY() const;
    void setStartY(qreal);

    qreal endX() const;
    void setEndX(qreal);

    qreal endY() const;
    void setEndY(qreal);

    qreal endZ() const;
    void setEndZ(qreal);

Q_SIGNALS:
    void updated();

private:
    qreal _startX;
    qreal _startY;
    qreal _endX;
    qreal _endY;
    qreal _endZ;
};

class Q_DECLARATIVE_EXPORT QFxRotation : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(qreal originX READ originX WRITE setOriginX)
    Q_PROPERTY(qreal originY READ originY WRITE setOriginY)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged())
public:
    QFxRotation(QObject *parent=0);
    ~QFxRotation();

    qreal originX() const;
    void setOriginX(qreal);

    qreal originY() const;
    void setOriginY(qreal);

    qreal angle() const;
    void setAngle(qreal);

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;

Q_SIGNALS:
    void angleChanged();

private Q_SLOTS:
    void update();
private:
    qreal _originX;
    qreal _originY;
    qreal _angle;

    mutable bool _dirty;
    mutable QSimpleCanvas::Matrix _transform;
};

class Q_DECLARATIVE_EXPORT QFxRotation3D : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(QFxAxis *axis READ axis)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
public:
    QFxRotation3D(QObject *parent=0);
    ~QFxRotation3D();

    QFxAxis *axis();

    qreal angle() const;
    void setAngle(qreal);

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;

private Q_SLOTS:
    void update();
private:
    QFxAxis _axis;
    qreal _angle;

    mutable bool _dirty;
    mutable QSimpleCanvas::Matrix _transform;
};

class Q_DECLARATIVE_EXPORT QFxTranslation3D : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(QFxAxis *axis READ axis)
    Q_PROPERTY(qreal distance READ distance WRITE setDistance)
public:
    QFxTranslation3D(QObject *parent=0);
    ~QFxTranslation3D();

    QFxAxis *axis();

    qreal distance() const;
    void setDistance(qreal);

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;

private Q_SLOTS:
    void update();
private:
    QFxAxis _axis;
    qreal _distance;

    mutable bool _dirty;
    mutable QSimpleCanvas::Matrix _transform;
};

class Q_DECLARATIVE_EXPORT QFxPerspective : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(qreal angle READ angle WRITE setAngle)
    Q_PROPERTY(qreal aspect READ aspect WRITE setAspect)
    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
public:
    QFxPerspective(QObject *parent=0);
    ~QFxPerspective();

    qreal angle() const { return _angle; }
    void setAngle(qreal v) { _angle = v; update(); }

    qreal aspect() const { return _aspect; }
    void setAspect(qreal v) { _aspect = v; update(); }

    qreal x() const { return _x; }
    void setX(qreal v) { _x = v; update(); }

    qreal y() const { return _y; }
    void setY(qreal v) { _y = v; update(); }

    qreal scale() const { return _scale; }
    void setScale(qreal v) { _scale = v; update(); }

#if defined(QFX_RENDER_OPENGL)
    virtual bool isIdentity() const;
    virtual QMatrix4x4 transform() const;
#endif
private:
    qreal _scale;
    qreal _x;
    qreal _y;
    qreal _angle;
    qreal _aspect;
};

class Q_DECLARATIVE_EXPORT QFxSquish : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(qreal height READ height WRITE setHeight)
    Q_PROPERTY(qreal topLeftX READ topLeft_x WRITE settopLeft_x)
    Q_PROPERTY(qreal topLeftY READ topLeft_y WRITE settopLeft_y)
    Q_PROPERTY(qreal topRightX READ topRight_x WRITE settopRight_x)
    Q_PROPERTY(qreal topRightY READ topRight_y WRITE settopRight_y)
    Q_PROPERTY(qreal bottomLeftX READ bottomLeft_x WRITE setbottomLeft_x)
    Q_PROPERTY(qreal bottomLeftY READ bottomLeft_y WRITE setbottomLeft_y)
    Q_PROPERTY(qreal bottomRightX READ bottomRight_x WRITE setbottomRight_x)
    Q_PROPERTY(qreal bottomRightY READ bottomRight_y WRITE setbottomRight_y)
public:
    QFxSquish(QObject *parent=0);
    ~QFxSquish();

    qreal x() const;
    void setX(qreal);

    qreal y() const;
    void setY(qreal);

    qreal width() const;
    void setWidth(qreal);

    qreal height() const;
    void setHeight(qreal);

    qreal topLeft_x() const;
    void settopLeft_x(qreal);

    qreal topLeft_y() const;
    void settopLeft_y(qreal);

    qreal topRight_x() const;
    void settopRight_x(qreal);

    qreal topRight_y() const;
    void settopRight_y(qreal);

    qreal bottomLeft_x() const;
    void setbottomLeft_x(qreal);

    qreal bottomLeft_y() const;
    void setbottomLeft_y(qreal);

    qreal bottomRight_y() const;
    void setbottomRight_y(qreal);

    qreal bottomRight_x() const;
    void setbottomRight_x(qreal);

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;

private:
    QPointF p;
    QSizeF s;
    QPointF p1, p2, p3, p4;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QFxTransform)
QML_DECLARE_TYPE(QFxAxis)
QML_DECLARE_TYPE(QFxRotation)
QML_DECLARE_TYPE(QFxRotation3D)
QML_DECLARE_TYPE(QFxTranslation3D)
QML_DECLARE_TYPE(QFxPerspective)
QML_DECLARE_TYPE(QFxSquish)

QT_END_HEADER

#endif // QFXTRANSFORM_H
