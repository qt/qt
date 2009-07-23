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

#ifndef QQUATERNION_H
#define QQUATERNION_H

#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

#ifndef QT_NO_QUATERNION

class QMatrix4x4;

class Q_GUI_EXPORT QQuaternion
{
public:
    QQuaternion();
    QQuaternion(qreal scalar, qreal xpos, qreal ypos, qreal zpos);
#ifndef QT_NO_VECTOR3D
    QQuaternion(qreal scalar, const QVector3D& vector);
#endif
#ifndef QT_NO_VECTOR4D
    explicit QQuaternion(const QVector4D& vector);
#endif

    bool isNull() const;
    bool isIdentity() const;

#ifndef QT_NO_VECTOR3D
    QVector3D vector() const;
    void setVector(const QVector3D& vector);
#endif
    void setVector(qreal x, qreal y, qreal z);

    qreal x() const;
    qreal y() const;
    qreal z() const;
    qreal scalar() const;

    void setX(qreal x);
    void setY(qreal y);
    void setZ(qreal z);
    void setScalar(qreal scalar);

    qreal length() const;
    qreal lengthSquared() const;

    QQuaternion normalized() const;
    void normalize();

    QQuaternion conjugate() const;

    QVector3D rotateVector(const QVector3D& vector) const;

    QQuaternion &operator+=(const QQuaternion &quaternion);
    QQuaternion &operator-=(const QQuaternion &quaternion);
    QQuaternion &operator*=(qreal factor);
    QQuaternion &operator*=(const QQuaternion &quaternion);
    QQuaternion &operator/=(qreal divisor);

    friend inline bool operator==(const QQuaternion &q1, const QQuaternion &q2);
    friend inline bool operator!=(const QQuaternion &q1, const QQuaternion &q2);
    friend inline const QQuaternion operator+(const QQuaternion &q1, const QQuaternion &q2);
    friend inline const QQuaternion operator-(const QQuaternion &q1, const QQuaternion &q2);
    friend inline const QQuaternion operator*(qreal factor, const QQuaternion &quaternion);
    friend inline const QQuaternion operator*(const QQuaternion &quaternion, qreal factor);
    friend inline const QQuaternion operator*(const QQuaternion &q1, const QQuaternion& q2);
    friend inline const QQuaternion operator-(const QQuaternion &quaternion);
    friend inline const QQuaternion operator/(const QQuaternion &quaternion, qreal divisor);

    friend inline bool qFuzzyCompare(const QQuaternion& q1, const QQuaternion& q2);

#ifndef QT_NO_VECTOR4D
    QVector4D toVector4D() const;
#endif

#ifndef QT_NO_VECTOR3D
    static QQuaternion fromAxisAndAngle(const QVector3D& axis, qreal angle);
#endif
    static QQuaternion fromAxisAndAngle
            (qreal x, qreal y, qreal z, qreal angle);

    static QQuaternion slerp
        (const QQuaternion& q1, const QQuaternion& q2, qreal t);
    static QQuaternion nlerp
        (const QQuaternion& q1, const QQuaternion& q2, qreal t);

private:
    float wp, xp, yp, zp;

    friend class QMatrix4x4;

    QQuaternion(float scalar, float xpos, float ypos, float zpos, int dummy);
};

inline QQuaternion::QQuaternion() : wp(1.0f), xp(0.0f), yp(0.0f), zp(0.0f) {}

inline QQuaternion::QQuaternion(qreal scalar, qreal xpos, qreal ypos, qreal zpos) : wp(scalar), xp(xpos), yp(ypos), zp(zpos) {}


inline QQuaternion::QQuaternion(float scalar, float xpos, float ypos, float zpos, int) : wp(scalar), xp(xpos), yp(ypos), zp(zpos) {}

inline bool QQuaternion::isNull() const
{
    return qIsNull(xp) && qIsNull(yp) && qIsNull(zp) && qIsNull(wp);
}

inline bool QQuaternion::isIdentity() const
{
    return qIsNull(xp) && qIsNull(yp) && qIsNull(zp) && wp == 1.0f;
}

inline qreal QQuaternion::x() const { return qreal(xp); }
inline qreal QQuaternion::y() const { return qreal(yp); }
inline qreal QQuaternion::z() const { return qreal(zp); }
inline qreal QQuaternion::scalar() const { return qreal(wp); }

inline void QQuaternion::setX(qreal x) { xp = x; }
inline void QQuaternion::setY(qreal y) { yp = y; }
inline void QQuaternion::setZ(qreal z) { zp = z; }
inline void QQuaternion::setScalar(qreal scalar) { wp = scalar; }

inline QQuaternion QQuaternion::conjugate() const
{
    return QQuaternion(wp, -xp, -yp, -zp, 1);
}

inline QQuaternion &QQuaternion::operator+=(const QQuaternion &quaternion)
{
    xp += quaternion.xp;
    yp += quaternion.yp;
    zp += quaternion.zp;
    wp += quaternion.wp;
    return *this;
}

inline QQuaternion &QQuaternion::operator-=(const QQuaternion &quaternion)
{
    xp -= quaternion.xp;
    yp -= quaternion.yp;
    zp -= quaternion.zp;
    wp -= quaternion.wp;
    return *this;
}

inline QQuaternion &QQuaternion::operator*=(qreal factor)
{
    xp *= factor;
    yp *= factor;
    zp *= factor;
    wp *= factor;
    return *this;
}

inline const QQuaternion operator*(const QQuaternion &q1, const QQuaternion& q2)
{
    // Algorithm from:
    // http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q53
    float x = q1.wp * q2.xp +
                    q1.xp * q2.wp +
                    q1.yp * q2.zp -
                    q1.zp * q2.yp;
    float y = q1.wp * q2.yp +
                    q1.yp * q2.wp +
                    q1.zp * q2.xp -
                    q1.xp * q2.zp;
    float z = q1.wp * q2.zp +
                    q1.zp * q2.wp +
                    q1.xp * q2.yp -
                    q1.yp * q2.xp;
    float w = q1.wp * q2.wp -
                    q1.xp * q2.xp -
                    q1.yp * q2.yp -
                    q1.zp * q2.zp;
    return QQuaternion(w, x, y, z, 1);
}

inline QQuaternion &QQuaternion::operator*=(const QQuaternion &quaternion)
{
    *this = *this * quaternion;
    return *this;
}

inline QQuaternion &QQuaternion::operator/=(qreal divisor)
{
    xp /= divisor;
    yp /= divisor;
    zp /= divisor;
    wp /= divisor;
    return *this;
}

inline bool operator==(const QQuaternion &q1, const QQuaternion &q2)
{
    return q1.xp == q2.xp && q1.yp == q2.yp && q1.zp == q2.zp && q1.wp == q2.wp;
}

inline bool operator!=(const QQuaternion &q1, const QQuaternion &q2)
{
    return q1.xp != q2.xp || q1.yp != q2.yp || q1.zp != q2.zp || q1.wp != q2.wp;
}

inline const QQuaternion operator+(const QQuaternion &q1, const QQuaternion &q2)
{
    return QQuaternion(q1.wp + q2.wp, q1.xp + q2.xp, q1.yp + q2.yp, q1.zp + q2.zp, 1);
}

inline const QQuaternion operator-(const QQuaternion &q1, const QQuaternion &q2)
{
    return QQuaternion(q1.wp - q2.wp, q1.xp - q2.xp, q1.yp - q2.yp, q1.zp - q2.zp, 1);
}

inline const QQuaternion operator*(qreal factor, const QQuaternion &quaternion)
{
    return QQuaternion(quaternion.wp * factor, quaternion.xp * factor, quaternion.yp * factor, quaternion.zp * factor, 1);
}

inline const QQuaternion operator*(const QQuaternion &quaternion, qreal factor)
{
    return QQuaternion(quaternion.wp * factor, quaternion.xp * factor, quaternion.yp * factor, quaternion.zp * factor, 1);
}

inline const QQuaternion operator-(const QQuaternion &quaternion)
{
    return QQuaternion(-quaternion.wp, -quaternion.xp, -quaternion.yp, -quaternion.zp, 1);
}

inline const QQuaternion operator/(const QQuaternion &quaternion, qreal divisor)
{
    return QQuaternion(quaternion.wp / divisor, quaternion.xp / divisor, quaternion.yp / divisor, quaternion.zp / divisor, 1);
}

inline bool qFuzzyCompare(const QQuaternion& q1, const QQuaternion& q2)
{
    return qFuzzyCompare(q1.xp, q2.xp) &&
           qFuzzyCompare(q1.yp, q2.yp) &&
           qFuzzyCompare(q1.zp, q2.zp) &&
           qFuzzyCompare(q1.wp, q2.wp);
}

#ifndef QT_NO_VECTOR3D

inline QQuaternion::QQuaternion(qreal scalar, const QVector3D& vector)
    : wp(scalar), xp(vector.xp), yp(vector.yp), zp(vector.zp) {}

inline void QQuaternion::setVector(const QVector3D& vector)
{
    xp = vector.xp;
    yp = vector.yp;
    zp = vector.zp;
}

inline QVector3D QQuaternion::vector() const
{
    return QVector3D(xp, yp, zp, 1);
}

#endif

inline void QQuaternion::setVector(qreal x, qreal y, qreal z)
{
    xp = x;
    yp = y;
    zp = z;
}

#ifndef QT_NO_VECTOR4D

inline QQuaternion::QQuaternion(const QVector4D& vector)
    : wp(vector.wp), xp(vector.xp), yp(vector.yp), zp(vector.zp) {}

inline QVector4D QQuaternion::toVector4D() const
{
    return QVector4D(xp, yp, zp, wp, 1);
}

#endif

#ifndef QT_NO_DEBUG_STREAM
Q_GUI_EXPORT QDebug operator<<(QDebug dbg, const QQuaternion &q);
#endif

#ifndef QT_NO_DATASTREAM
Q_GUI_EXPORT QDataStream &operator<<(QDataStream &, const QQuaternion &);
Q_GUI_EXPORT QDataStream &operator>>(QDataStream &, QQuaternion &);
#endif

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif
