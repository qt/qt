/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#ifndef QVECTOR2D_H
#define QVECTOR2D_H

#include <QtGui/qmath3dglobal.h>
#include <QtCore/qpoint.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QVector3D;
class QVector4D;

#ifndef QT_NO_VECTOR2D

class Q_GUI_EXPORT QVector2D
{
public:
    QVector2D();
    QVector2D(qreal xpos, qreal ypos);
    QVector2D(int xpos, int ypos);
    explicit QVector2D(const QPoint& point);
    explicit QVector2D(const QPointF& point);
#ifndef QT_NO_VECTOR3D
    explicit QVector2D(const QVector3D& vector);
#endif
#ifndef QT_NO_VECTOR4D
    explicit QVector2D(const QVector4D& vector);
#endif

    bool isNull() const;

    qreal x() const;
    qreal y() const;

    void setX(qreal x);
    void setY(qreal y);

    qreal length() const;
    qreal lengthSquared() const;

    QVector2D normalized() const;
    void normalize();

    QVector2D &operator+=(const QVector2D &vector);
    QVector2D &operator-=(const QVector2D &vector);
    QVector2D &operator*=(qreal factor);
    QVector2D &operator*=(const QVector2D &vector);
    QVector2D &operator/=(qreal divisor);

    static qreal dotProduct(const QVector2D& v1, const QVector2D& v2);

    friend inline bool operator==(const QVector2D &v1, const QVector2D &v2);
    friend inline bool operator!=(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator+(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator-(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator*(qreal factor, const QVector2D &vector);
    friend inline const QVector2D operator*(const QVector2D &vector, qreal factor);
    friend inline const QVector2D operator*(const QVector2D &v1, const QVector2D &v2);
    friend inline const QVector2D operator-(const QVector2D &vector);
    friend inline const QVector2D operator/(const QVector2D &vector, qreal divisor);

    friend inline bool qFuzzyCompare(const QVector2D& v1, const QVector2D& v2);

#ifndef QT_NO_VECTOR3D
    QVector3D toVector3D() const;
#endif
#ifndef QT_NO_VECTOR4D
    QVector4D toVector4D() const;
#endif

    QPoint toPoint() const;
    QPointF toPointF() const;

private:
    qrealinner xp, yp;

    QVector2D(qrealinner xpos, qrealinner ypos, int dummy);

    friend class QVector3D;
    friend class QVector4D;
    friend class QVertexArray;
};

inline QVector2D::QVector2D() : xp(0.0f), yp(0.0f) {}

inline QVector2D::QVector2D(qrealinner xpos, qrealinner ypos, int) : xp(xpos), yp(ypos) {}

inline QVector2D::QVector2D(qreal xpos, qreal ypos) : xp(xpos), yp(ypos) {}

inline QVector2D::QVector2D(int xpos, int ypos) : xp(xpos), yp(ypos) {}

inline QVector2D::QVector2D(const QPoint& point) : xp(point.x()), yp(point.y()) {}

inline QVector2D::QVector2D(const QPointF& point) : xp(point.x()), yp(point.y()) {}

inline bool QVector2D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp);
}

inline qreal QVector2D::x() const { return qt_math3d_convert<qreal, qrealinner>(xp); }
inline qreal QVector2D::y() const { return qt_math3d_convert<qreal, qrealinner>(yp); }

inline void QVector2D::setX(qreal x) { xp = x; }
inline void QVector2D::setY(qreal y) { yp = y; }

inline QVector2D &QVector2D::operator+=(const QVector2D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator-=(const QVector2D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator*=(qreal factor)
{
    qrealinner f(factor);
    xp *= f;
    yp *= f;
    return *this;
}

inline QVector2D &QVector2D::operator*=(const QVector2D &vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    return *this;
}

inline QVector2D &QVector2D::operator/=(qreal divisor)
{
    qrealinner d(divisor);
    xp /= d;
    yp /= d;
    return *this;
}

inline bool operator==(const QVector2D &v1, const QVector2D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp;
}

inline bool operator!=(const QVector2D &v1, const QVector2D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp;
}

inline const QVector2D operator+(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp + v2.xp, v1.yp + v2.yp, 1);
}

inline const QVector2D operator-(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp - v2.xp, v1.yp - v2.yp, 1);
}

inline const QVector2D operator*(qreal factor, const QVector2D &vector)
{
    qrealinner f(factor);
    return QVector2D(vector.xp * f, vector.yp * f, 1);
}

inline const QVector2D operator*(const QVector2D &vector, qreal factor)
{
    qrealinner f(factor);
    return QVector2D(vector.xp * f, vector.yp * f, 1);
}

inline const QVector2D operator*(const QVector2D &v1, const QVector2D &v2)
{
    return QVector2D(v1.xp * v2.xp, v1.yp * v2.yp, 1);
}

inline const QVector2D operator-(const QVector2D &vector)
{
    return QVector2D(-vector.xp, -vector.yp, 1);
}

inline const QVector2D operator/(const QVector2D &vector, qreal divisor)
{
    qrealinner d(divisor);
    return QVector2D(vector.xp / d, vector.yp / d, 1);
}

inline bool qFuzzyCompare(const QVector2D& v1, const QVector2D& v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) && qFuzzyCompare(v1.yp, v2.yp);
}

inline QPoint QVector2D::toPoint() const
{
    return QPoint(qRound(xp), qRound(yp));
}

inline QPointF QVector2D::toPointF() const
{
    return QPointF(qt_math3d_convert<qreal, qrealinner>(xp),
                   qt_math3d_convert<qreal, qrealinner>(yp));
}

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QVector2D &vector);
#endif

#endif

QT_END_NAMESPACE

#ifndef QT_NO_VECTOR2D
Q_DECLARE_METATYPE(QVector2D)
#endif

QT_END_HEADER

#endif
