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

#ifndef QVECTOR3D_H
#define QVECTOR3D_H

#include <QtGui/qmath3dglobal.h>
#include <QtCore/qpoint.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QMatrix4x4;
class QVector2D;
class QVector4D;
class QQuaternion;

#ifndef QT_NO_VECTOR3D

class Q_GUI_EXPORT QVector3D
{
public:
    QVector3D();
    QVector3D(qreal xpos, qreal ypos, qreal zpos);
    QVector3D(int xpos, int ypos, int zpos);
    explicit QVector3D(const QPoint& point);
    explicit QVector3D(const QPointF& point);
#ifndef QT_NO_VECTOR2D
    QVector3D(const QVector2D& vector);
    QVector3D(const QVector2D& vector, qreal zpos);
#endif
#ifndef QT_NO_VECTOR4D
    explicit QVector3D(const QVector4D& vector);
#endif

    bool isNull() const;

    qreal x() const;
    qreal y() const;
    qreal z() const;

    void setX(qreal x);
    void setY(qreal y);
    void setZ(qreal z);

    qreal length() const;
    qreal lengthSquared() const;

    QVector3D normalized() const;
    void normalize();

    QVector3D &operator+=(const QVector3D &vector);
    QVector3D &operator-=(const QVector3D &vector);
    QVector3D &operator*=(qreal factor);
    QVector3D &operator*=(const QVector3D& vector);
    QVector3D &operator/=(qreal divisor);

    static qreal dotProduct(const QVector3D& v1, const QVector3D& v2);
    static QVector3D crossProduct(const QVector3D& v1, const QVector3D& v2);
    static QVector3D normal(const QVector3D& v1, const QVector3D& v2);
    static QVector3D normal
        (const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);

    qreal distanceToPlane(const QVector3D& plane, const QVector3D& normal) const;
    qreal distanceToPlane(const QVector3D& plane1, const QVector3D& plane2, const QVector3D& plane3) const;
    qreal distanceToLine(const QVector3D& point, const QVector3D& direction) const;

    friend inline bool operator==(const QVector3D &v1, const QVector3D &v2);
    friend inline bool operator!=(const QVector3D &v1, const QVector3D &v2);
    friend inline const QVector3D operator+(const QVector3D &v1, const QVector3D &v2);
    friend inline const QVector3D operator-(const QVector3D &v1, const QVector3D &v2);
    friend inline const QVector3D operator*(qreal factor, const QVector3D &vector);
    friend inline const QVector3D operator*(const QVector3D &vector, qreal factor);
    friend const QVector3D operator*(const QVector3D &v1, const QVector3D& v2);
    friend inline const QVector3D operator-(const QVector3D &vector);
    friend inline const QVector3D operator/(const QVector3D &vector, qreal divisor);

    friend inline bool qFuzzyCompare(const QVector3D& v1, const QVector3D& v2);

#ifndef QT_NO_VECTOR2D
    QVector2D toVector2D() const;
#endif
#ifndef QT_NO_VECTOR4D
    QVector4D toVector4D() const;
#endif

    QPoint toPoint() const;
    QPointF toPointF() const;

private:
    qrealinner xp, yp, zp;

    QVector3D(qrealinner xpos, qrealinner ypos, qrealinner zpos, int dummy);

    friend class QVector2D;
    friend class QVector4D;
    friend class QQuaternion;
    friend class QMatrix4x4;
    friend class QVertexArray;
    friend class QGLPainter;
#ifndef QT_NO_MATRIX4X4
    friend QVector3D operator*(const QVector3D& vector, const QMatrix4x4& matrix);
    friend QVector3D operator*(const QMatrix4x4& matrix, const QVector3D& vector);
#endif
};

inline QVector3D::QVector3D() : xp(0.0f), yp(0.0f), zp(0.0f) {}

inline QVector3D::QVector3D(qreal xpos, qreal ypos, qreal zpos) : xp(xpos), yp(ypos), zp(zpos) {}

inline QVector3D::QVector3D(qrealinner xpos, qrealinner ypos, qrealinner zpos, int) : xp(xpos), yp(ypos), zp(zpos) {}

inline QVector3D::QVector3D(int xpos, int ypos, int zpos) : xp(xpos), yp(ypos), zp(zpos) {}

inline QVector3D::QVector3D(const QPoint& point) : xp(point.x()), yp(point.y()), zp(0.0f) {}

inline QVector3D::QVector3D(const QPointF& point) : xp(point.x()), yp(point.y()), zp(0.0f) {}

inline bool QVector3D::isNull() const
{
    return qIsNull(xp) && qIsNull(yp) && qIsNull(zp);
}

inline qreal QVector3D::x() const { return qt_math3d_convert<qreal, qrealinner>(xp); }
inline qreal QVector3D::y() const { return qt_math3d_convert<qreal, qrealinner>(yp); }
inline qreal QVector3D::z() const { return qt_math3d_convert<qreal, qrealinner>(zp); }

inline void QVector3D::setX(qreal x) { xp = x; }
inline void QVector3D::setY(qreal y) { yp = y; }
inline void QVector3D::setZ(qreal z) { zp = z; }

inline QVector3D &QVector3D::operator+=(const QVector3D &vector)
{
    xp += vector.xp;
    yp += vector.yp;
    zp += vector.zp;
    return *this;
}

inline QVector3D &QVector3D::operator-=(const QVector3D &vector)
{
    xp -= vector.xp;
    yp -= vector.yp;
    zp -= vector.zp;
    return *this;
}

inline QVector3D &QVector3D::operator*=(qreal factor)
{
    qrealinner f(factor);
    xp *= f;
    yp *= f;
    zp *= f;
    return *this;
}

inline QVector3D &QVector3D::operator*=(const QVector3D& vector)
{
    xp *= vector.xp;
    yp *= vector.yp;
    zp *= vector.zp;
    return *this;
}

inline QVector3D &QVector3D::operator/=(qreal divisor)
{
    qrealinner d(divisor);
    xp /= d;
    yp /= d;
    zp /= d;
    return *this;
}

inline bool operator==(const QVector3D &v1, const QVector3D &v2)
{
    return v1.xp == v2.xp && v1.yp == v2.yp && v1.zp == v2.zp;
}

inline bool operator!=(const QVector3D &v1, const QVector3D &v2)
{
    return v1.xp != v2.xp || v1.yp != v2.yp || v1.zp != v2.zp;
}

inline const QVector3D operator+(const QVector3D &v1, const QVector3D &v2)
{
    return QVector3D(v1.xp + v2.xp, v1.yp + v2.yp, v1.zp + v2.zp, 1);
}

inline const QVector3D operator-(const QVector3D &v1, const QVector3D &v2)
{
    return QVector3D(v1.xp - v2.xp, v1.yp - v2.yp, v1.zp - v2.zp, 1);
}

inline const QVector3D operator*(qreal factor, const QVector3D &vector)
{
    qrealinner f(factor);
    return QVector3D(vector.xp * f, vector.yp * f, vector.zp * f, 1);
}

inline const QVector3D operator*(const QVector3D &vector, qreal factor)
{
    qrealinner f(factor);
    return QVector3D(vector.xp * f, vector.yp * f, vector.zp * f, 1);
}

inline const QVector3D operator*(const QVector3D &v1, const QVector3D& v2)
{
    return QVector3D(v1.xp * v2.xp, v1.yp * v2.yp, v1.zp * v2.zp, 1);
}

inline const QVector3D operator-(const QVector3D &vector)
{
    return QVector3D(-vector.xp, -vector.yp, -vector.zp, 1);
}

inline const QVector3D operator/(const QVector3D &vector, qreal divisor)
{
    qrealinner d(divisor);
    return QVector3D(vector.xp / d, vector.yp / d, vector.zp / d, 1);
}

inline bool qFuzzyCompare(const QVector3D& v1, const QVector3D& v2)
{
    return qFuzzyCompare(v1.xp, v2.xp) &&
           qFuzzyCompare(v1.yp, v2.yp) &&
           qFuzzyCompare(v1.zp, v2.zp);
}

inline QPoint QVector3D::toPoint() const
{
    return QPoint(qRound(xp), qRound(yp));
}

inline QPointF QVector3D::toPointF() const
{
    return QPointF(qt_math3d_convert<qreal, qrealinner>(xp),
                   qt_math3d_convert<qreal, qrealinner>(yp));
}

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QVector3D &vector);
#endif

#endif

QT_END_NAMESPACE

#ifndef QT_NO_VECTOR3D
Q_DECLARE_METATYPE(QVector3D)
#endif

QT_END_HEADER

#endif
