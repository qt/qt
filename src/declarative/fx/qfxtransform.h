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

#include <QObject>
#include <QTransform>
#if defined(QFX_RENDER_OPENGL)
#include <QtGui/qmatrix4x4.h>
#endif
#include <qfxitem.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_EXPORT QFxTransform : public QObject
{
    Q_OBJECT
public:
    QFxTransform(QObject *parent=0);

    void update();

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;
};
QML_DECLARE_TYPE(QFxTransform);

class Q_DECLARATIVE_EXPORT QFxAxis : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(qreal xStart READ xStart WRITE setXStart)
    Q_PROPERTY(qreal yStart READ yStart WRITE setYStart)
    Q_PROPERTY(qreal xEnd READ xEnd WRITE setXEnd)
    Q_PROPERTY(qreal yEnd READ yEnd WRITE setYEnd)
    Q_PROPERTY(qreal zEnd READ zEnd WRITE setZEnd)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
    Q_PROPERTY(qreal translation READ translation WRITE setTranslation)
    Q_PROPERTY(qreal distanceToPlane READ distanceToPlane WRITE setDistanceToPlane)
public:
    QFxAxis(QObject *parent=0);

    qreal xStart() const;
    void setXStart(qreal);
    qreal yStart() const;
    void setYStart(qreal);

    qreal xEnd() const;
    void setXEnd(qreal);
    qreal yEnd() const;
    void setYEnd(qreal);
    qreal zEnd() const;
    void setZEnd(qreal);

    qreal rotation() const;
    void setRotation(qreal);
    qreal translation() const;
    void setTranslation(qreal);

    qreal distanceToPlane() const;
    void setDistanceToPlane(qreal);

    virtual bool isIdentity() const;
    virtual QSimpleCanvas::Matrix transform() const;

private:
    void update();

    qreal _xStart;
    qreal _yStart;
    qreal _xEnd;
    qreal _yEnd;
    qreal _zEnd;
    qreal _rotation;
    qreal _translation;
    qreal _distanceToPlane;

    mutable bool _dirty;
    mutable QSimpleCanvas::Matrix _transform;
};
QML_DECLARE_TYPE(QFxAxis);

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
QML_DECLARE_TYPE(QFxPerspective);

class Q_DECLARATIVE_EXPORT QFxSquish : public QFxTransform
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(qreal width READ width WRITE setWidth)
    Q_PROPERTY(qreal height READ height WRITE setHeight)
    Q_PROPERTY(qreal topLeft_x READ topLeft_x WRITE settopLeft_x)
    Q_PROPERTY(qreal topLeft_y READ topLeft_y WRITE settopLeft_y)
    Q_PROPERTY(qreal topRight_x READ topRight_x WRITE settopRight_x)
    Q_PROPERTY(qreal topRight_y READ topRight_y WRITE settopRight_y)
    Q_PROPERTY(qreal bottomLeft_x READ bottomLeft_x WRITE setbottomLeft_x)
    Q_PROPERTY(qreal bottomLeft_y READ bottomLeft_y WRITE setbottomLeft_y)
    Q_PROPERTY(qreal bottomRight_y READ bottomRight_y WRITE setbottomRight_y)
    Q_PROPERTY(qreal bottomRight_x READ bottomRight_x WRITE setbottomRight_x)
public:
    QFxSquish(QObject *parent=0);

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

#if defined(QFX_RENDER_OPENGL)
    virtual bool isIdentity() const;
    virtual QMatrix4x4 transform() const;
#endif

private:
    void update();

    QPointF p;
    QSizeF s;
    QPointF p1, p2, p3, p4;
};

QML_DECLARE_TYPE(QFxSquish);

class QFxFlipablePrivate;
class Q_DECLARATIVE_EXPORT QFxFlipable : public QFxItem
{
    Q_OBJECT

    Q_ENUMS(Side);
    Q_PROPERTY(QFxItem *front READ front WRITE setFront)
    Q_PROPERTY(QFxItem *back READ back WRITE setBack)
    Q_PROPERTY(Side side READ side NOTIFY sideChanged)
public:
    QFxFlipable(QFxItem *parent=0);
    ~QFxFlipable();

    QFxItem *front();
    void setFront(QFxItem *);

    QFxItem *back();
    void setBack(QFxItem *);

    enum Side { Front, Back };
    Side side() const;

protected:
    virtual void transformChanged(const QSimpleCanvas::Matrix &);

Q_SIGNALS:
    void sideChanged();

private:
    Q_DISABLE_COPY(QFxFlipable)
    Q_DECLARE_PRIVATE(QFxFlipable)
};
QML_DECLARE_TYPE(QFxFlipable);


QT_END_NAMESPACE

QT_END_HEADER
#endif // _TRANSFORM_H_
