/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef TESTTYPES_H
#define TESTTYPES_H

#include <QObject>
#include <QPoint>
#include <QPointF>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QFont>
#include <qdeclarative.h>
#include <QDeclarativePropertyValueSource>
#include <QDeclarativeProperty>
#include <private/qdeclarativeproperty_p.h>

class MyTypeObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint point READ point WRITE setPoint NOTIFY changed)
    Q_PROPERTY(QPointF pointf READ pointf WRITE setPointf NOTIFY changed)
    Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY changed)
    Q_PROPERTY(QSizeF sizef READ sizef WRITE setSizef NOTIFY changed)
    Q_PROPERTY(QSize sizereadonly READ size NOTIFY changed)
    Q_PROPERTY(QRect rect READ rect WRITE setRect NOTIFY changed)
    Q_PROPERTY(QRectF rectf READ rectf WRITE setRectf NOTIFY changed)
    Q_PROPERTY(QVector2D vector2 READ vector2 WRITE setVector2 NOTIFY changed)
    Q_PROPERTY(QVector3D vector READ vector WRITE setVector NOTIFY changed)
    Q_PROPERTY(QVector4D vector4 READ vector4 WRITE setVector4 NOTIFY changed)
    Q_PROPERTY(QQuaternion quaternion READ quaternion WRITE setQuaternion NOTIFY changed)
    Q_PROPERTY(QMatrix4x4 matrix READ matrix WRITE setMatrix NOTIFY changed)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY changed)
    Q_PROPERTY(QVariant variant READ variant NOTIFY changed)

public:
    MyTypeObject() :
        m_point(10, 4),
        m_pointf(11.3, -10.9),
        m_size(1912, 1913),
        m_sizef(0.1, 100923.2),
        m_rect(2, 3, 109, 102),
        m_rectf(103.8, 99.2, 88.1, 77.6),
        m_vector2(32.88, 1.3),
        m_vector(23.88, 3.1, 4.3),
        m_vector4(54.2, 23.88, 3.1, 4.3),
        m_quaternion(4.3, 54.2, 23.88, 3.1),
        m_matrix(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)
    {
        m_font.setFamily("Arial");
        m_font.setBold(true);
        m_font.setWeight(QFont::DemiBold);
        m_font.setItalic(true);
        m_font.setUnderline(true);
        m_font.setOverline(true);
        m_font.setStrikeOut(true);
        m_font.setPointSize(29);
        m_font.setCapitalization(QFont::AllLowercase);
        m_font.setLetterSpacing(QFont::AbsoluteSpacing, 10.2);
        m_font.setWordSpacing(19.7);
    }

    QPoint m_point;
    QPoint point() const { return m_point; }
    void setPoint(const QPoint &v) { m_point = v; emit changed(); }

    QPointF m_pointf;
    QPointF pointf() const { return m_pointf; }
    void setPointf(const QPointF &v) { m_pointf = v; emit changed(); }

    QSize m_size;
    QSize size() const { return m_size; }
    void setSize(const QSize &v) { m_size = v; emit changed(); }

    QSizeF m_sizef;
    QSizeF sizef() const { return m_sizef; }
    void setSizef(const QSizeF &v) { m_sizef = v; emit changed(); }

    QRect m_rect;
    QRect rect() const { return m_rect; }
    void setRect(const QRect &v) { m_rect = v; emit changed(); }

    QRectF m_rectf;
    QRectF rectf() const { return m_rectf; }
    void setRectf(const QRectF &v) { m_rectf = v; emit changed(); }

    QVector2D m_vector2;
    QVector2D vector2() const { return m_vector2; }
    void setVector2(const QVector2D &v) { m_vector2 = v; emit changed(); }

    QVector3D m_vector;
    QVector3D vector() const { return m_vector; }
    void setVector(const QVector3D &v) { m_vector = v; emit changed(); }

    QVector4D m_vector4;
    QVector4D vector4() const { return m_vector4; }
    void setVector4(const QVector4D &v) { m_vector4 = v; emit changed(); }

    QQuaternion m_quaternion;
    QQuaternion quaternion() const { return m_quaternion; }
    void setQuaternion(const QQuaternion &v) { m_quaternion = v; emit changed(); }

    QMatrix4x4 m_matrix;
    QMatrix4x4 matrix() const { return m_matrix; }
    void setMatrix(const QMatrix4x4 &v) { m_matrix = v; emit changed(); }

    QFont m_font;
    QFont font() const { return m_font; }
    void setFont(const QFont &v) { m_font = v; emit changed(); }

    QVariant variant() const { return sizef(); }

    void emitRunScript() { emit runScript(); }

signals:
    void changed();
    void runScript();

public slots:
    QSize method() { return QSize(13, 14); }
};

class MyConstantValueSource : public QObject, public QDeclarativePropertyValueSource
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativePropertyValueSource)
public:
    virtual void setTarget(const QDeclarativeProperty &p) { p.write(3345); }
};

class MyOffsetValueInterceptor : public QObject, public QDeclarativePropertyValueInterceptor
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativePropertyValueInterceptor)
public:
    virtual void setTarget(const QDeclarativeProperty &p) { prop = p; }
    virtual void write(const QVariant &value) { QDeclarativePropertyPrivate::write(prop, value.toInt() + 13, QDeclarativePropertyPrivate::BypassInterceptor); }

private:
    QDeclarativeProperty prop;
};

void registerTypes();

#endif // TESTTYPES_H
