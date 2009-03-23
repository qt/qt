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

#ifndef QFIXEDPT_H
#define QFIXEDPT_H

#include <QtCore/qglobal.h>
#include <QtCore/qdebug.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

Q_GUI_EXPORT qint64 qt_math3d_fixed_sqrt(qint64 value);

// Should be called QFixed or QFixedPoint, but both of those
// are already in use in src/gui/painting/qfixed_p.h.
template <int PrecBits>
class QFixedPt
{
public:
    inline QFixedPt() {} // Deliberately not initialized - don't change this.
    inline QFixedPt(int value) : val(value << PrecBits) {}
    inline QFixedPt(qreal value) : val(int(value * (1 << PrecBits))) {}

    inline QFixedPt<PrecBits>& operator=(int value)
        { val = value << PrecBits; return *this; }
    inline QFixedPt<PrecBits>& operator=(qreal value)
        { val = int(value * (1 << PrecBits)); return *this; }

    inline int bits() const { return val; }
    inline void setBits(int value) { val = value; }

#if !defined(QT_NO_MEMBER_TEMPLATES) || defined(Q_QDOC)
    template <int Prec>
    inline QFixedPt<Prec> toPrecision() const
    {
        QFixedPt<Prec> result;
        if (Prec < PrecBits)
            result.setBits(shiftRight(val, (PrecBits - Prec)));
        else
            result.setBits(shiftLeft(val, (Prec - PrecBits)));
        return result;
    }
#endif

    inline QFixedPt<PrecBits>& operator+=(const QFixedPt<PrecBits>& value)
        { val += value.val; return *this; }
    inline QFixedPt<PrecBits>& operator+=(int value)
        { val += (value << PrecBits); return *this; }
    inline QFixedPt<PrecBits>& operator+=(qreal value)
        { val += int(value * (1 << PrecBits)); return *this; }

    inline QFixedPt<PrecBits>& operator-=(const QFixedPt<PrecBits>& value)
        { val -= value.val; return *this; }
    inline QFixedPt<PrecBits>& operator-=(int value)
        { val -= (value << PrecBits); return *this; }
    inline QFixedPt<PrecBits>& operator-=(qreal value)
        { val -= int(value * (1 << PrecBits)); return *this; }

    inline QFixedPt<PrecBits>& operator*=(const QFixedPt<PrecBits>& value)
        { val = mul(val, value.val); return *this; }
    inline QFixedPt<PrecBits>& operator*=(int value)
        { val = mul(val, (value << PrecBits)); return *this; }
    inline QFixedPt<PrecBits>& operator*=(qreal value)
        { val = mul(val, int(value * (1 << PrecBits))); return *this; }

    inline QFixedPt<PrecBits>& operator/=(const QFixedPt<PrecBits>& value)
        { val = div(val, value.val); return *this; }
    inline QFixedPt<PrecBits>& operator/=(int value)
        { val = div(val, (value << PrecBits)); return *this; }
    inline QFixedPt<PrecBits>& operator/=(qreal value)
        { val = div(val, int(value * (1 << PrecBits))); return *this; }

    inline QFixedPt<PrecBits>& operator<<=(int value)
        { val <<= value; return *this; }
    inline QFixedPt<PrecBits>& operator>>=(int value)
        { val >>= value; return *this; }

    inline QFixedPt<PrecBits> operator<<(int value) const
        { QFixedPt<PrecBits> result; result.val = val << value; return result; }
    inline QFixedPt<PrecBits> operator>>(int value) const
        { QFixedPt<PrecBits> result; result.val = val >> value; return result; }

    inline bool operator==(const QFixedPt<PrecBits>& value) const
        { return val == value.val; }
    inline bool operator!=(const QFixedPt<PrecBits>& value) const
        { return val != value.val; }
    inline bool operator<=(const QFixedPt<PrecBits>& value) const
        { return val <= value.val; }
    inline bool operator<(const QFixedPt<PrecBits>& value) const
        { return val < value.val; }
    inline bool operator>=(const QFixedPt<PrecBits>& value) const
        { return val >= value.val; }
    inline bool operator>(const QFixedPt<PrecBits>& value) const
        { return val > value.val; }

    inline QFixedPt<PrecBits> operator+(const QFixedPt<PrecBits>& value) const
        { QFixedPt<PrecBits> result;
          result.val = val + value.val; return result; }
    inline QFixedPt<PrecBits> operator+(int value) const
        { QFixedPt<PrecBits> result;
          result.val = val + (value << PrecBits); return result; }
    inline QFixedPt<PrecBits> operator+(qreal value) const
        { QFixedPt<PrecBits> result;
          result.val = val + int(value * (1 << PrecBits)); return result; }

    inline QFixedPt<PrecBits> operator-(const QFixedPt<PrecBits>& value) const
        { QFixedPt<PrecBits> result;
          result.val = val - value.val; return result; }
    inline QFixedPt<PrecBits> operator-(int value) const
        { QFixedPt<PrecBits> result;
          result.val = val - (value << PrecBits); return result; }
    inline QFixedPt<PrecBits> operator-(qreal value) const
        { QFixedPt<PrecBits> result;
          result.val = val - int(value * (1 << PrecBits)); return result; }

    inline QFixedPt<PrecBits> operator*(const QFixedPt<PrecBits>& value) const
        { QFixedPt<PrecBits> result;
          result.val = mul(val, value.val); return result; }
    inline QFixedPt<PrecBits> operator*(int value) const
        { QFixedPt<PrecBits> result;
          result.val = mul(val, (value << PrecBits)); return result; }
    inline QFixedPt<PrecBits> operator*(qreal value) const
        { QFixedPt<PrecBits> result;
          result.val = mul(val, int(value * (1 << PrecBits))); return result; }

    inline QFixedPt<PrecBits> operator/(const QFixedPt<PrecBits>& value) const
        { QFixedPt<PrecBits> result;
          result.val = div(val, value.val); return result; }
    inline QFixedPt<PrecBits> operator/(int value) const
        { QFixedPt<PrecBits> result;
          result.val = div(val, (value << PrecBits)); return result; }
    inline QFixedPt<PrecBits> operator/(qreal value) const
        { QFixedPt<PrecBits> result;
          result.val = div(val, int(value * (1 << PrecBits))); return result; }

    inline QFixedPt<PrecBits> operator-() const
        { QFixedPt<PrecBits> result; result.val = -val; return result; }

    inline QFixedPt<PrecBits> sqrt() const;
    inline qreal sqrtF() const;
    inline QFixedPt<PrecBits> round() const;
    inline QFixedPt<PrecBits> floor() const;
    inline QFixedPt<PrecBits> ceil() const;
    inline int truncate() const { return val >> PrecBits; }

    inline int toInt() const { return (val + (1 << (PrecBits - 1))) >> PrecBits; }
    inline qreal toReal() const { return qreal(val) / qreal(1 << PrecBits); }

#if !defined(Q_NO_TEMPLATE_FRIENDS)
    template <int Prec>
    friend QFixedPt<Prec> operator/(int v1, const QFixedPt<Prec>& v2);
    template <int Prec>
    friend QFixedPt<Prec> operator/(qreal v1, const QFixedPt<Prec>& v2);

private:
#endif
    int val;

    inline static int mul(int v1, int v2)
    {
        return int((qint64(v1) * qint64(v2)) >> PrecBits);
    }

    inline static int div(int v1, int v2)
    {
        if (v2)
            return int((qint64(v1) << PrecBits) / qint64(v2));
        else
            return 0;
    }

    // These are used by toPrecision() to avoid a silly gcc compiler warning
    // related to negative shift values that will never actually be used.
    inline static int shiftRight(int val, int shift)
    {
        return val >> shift;
    }
    inline static int shiftLeft(int val, int shift)
    {
        return val << shift;
    }

#if !defined(Q_NO_TEMPLATE_FRIENDS)
    template <int Prec, int Prec2>
    friend QFixedPt<Prec> qFixedPtToPrecision(const QFixedPt<Prec2>& value);
#endif
};

template <int PrecBits>
inline bool operator==(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() == (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator==(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) == v2.bits();
}

template <int PrecBits>
inline bool operator==(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() == int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator==(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) == v2.bits();
}

template <int PrecBits>
inline bool operator!=(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() != (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator!=(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) != v2.bits();
}

template <int PrecBits>
inline bool operator!=(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() != int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator!=(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) != v2.bits();
}

template <int PrecBits>
inline bool operator<=(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() <= (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator<=(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) <= v2.bits();
}

template <int PrecBits>
inline bool operator<=(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() <= int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator<=(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) <= v2.bits();
}

template <int PrecBits>
inline bool operator<(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() < (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator<(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) < v2.bits();
}

template <int PrecBits>
inline bool operator<(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() < int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator<(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) < v2.bits();
}

template <int PrecBits>
inline bool operator>=(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() >= (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator>=(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) >= v2.bits();
}

template <int PrecBits>
inline bool operator>=(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() >= int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator>=(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) >= v2.bits();
}

template <int PrecBits>
inline bool operator>(const QFixedPt<PrecBits>& v1, int v2)
{
    return v1.bits() > (v2 << PrecBits);
}

template <int PrecBits>
inline bool operator>(int v1, const QFixedPt<PrecBits>& v2)
{
    return (v1 << PrecBits) > v2.bits();
}

template <int PrecBits>
inline bool operator>(const QFixedPt<PrecBits>& v1, qreal v2)
{
    return v1.bits() > int(v2 * (1 << PrecBits));
}

template <int PrecBits>
inline bool operator>(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return int(v1 * (1 << PrecBits)) > v2.bits();
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator+(int v1, const QFixedPt<PrecBits>& v2)
{
    return v2 + v1;
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator+(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return v2 + v1;
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator-(int v1, const QFixedPt<PrecBits>& v2)
{
    return -(v2 - v1);
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator-(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return -(v2 - v1);
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator*(int v1, const QFixedPt<PrecBits>& v2)
{
    return v2 * v1;
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator*(qreal v1, const QFixedPt<PrecBits>& v2)
{
    return v2 * v1;
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator/(int v1, const QFixedPt<PrecBits>& v2)
{
    QFixedPt<PrecBits> result;
    result.val = QFixedPt<PrecBits>::div(v1 << PrecBits, v2.val);
    return result;
}

template <int PrecBits>
inline QFixedPt<PrecBits> operator/(qreal v1, const QFixedPt<PrecBits>& v2)
{
    QFixedPt<PrecBits> result;
    result.val = QFixedPt<PrecBits>::div(int(v1 * (1 << PrecBits)), v2.val);
    return result;
}

template <int PrecBits>
inline QFixedPt<PrecBits> QFixedPt<PrecBits>::sqrt() const
{
    QFixedPt<PrecBits> result;
    result.val = int(qt_math3d_fixed_sqrt
        (qint64(val) << (PrecBits * 2)) >> (PrecBits / 2));
    return result;
}

template <int PrecBits>
inline qreal QFixedPt<PrecBits>::sqrtF() const
{
    return qt_math3d_fixed_sqrt
        (qint64(val) << (PrecBits * 2)) / (qreal)(1 << (PrecBits + (PrecBits / 2)));
}

template <int PrecBits>
inline QFixedPt<PrecBits> QFixedPt<PrecBits>::round() const
{
    QFixedPt<PrecBits> result;
    result.val = (val + (1 << (PrecBits - 1))) & ~((1 << PrecBits) - 1);
    return result;
}

template <int PrecBits>
inline QFixedPt<PrecBits> QFixedPt<PrecBits>::floor() const
{
    QFixedPt<PrecBits> result;
    result.val = val & ~((1 << PrecBits) - 1);
    return result;
}

template <int PrecBits>
inline QFixedPt<PrecBits> QFixedPt<PrecBits>::ceil() const
{
    QFixedPt<PrecBits> result;
    result.val = (val + (1 << PrecBits) - 1) & ~((1 << PrecBits) - 1);
    return result;
}

template <int PrecBits>
inline int qCeil(const QFixedPt<PrecBits>& value)
{
    return value.ceil().bits() >> PrecBits;
}

template <int PrecBits>
inline int qFloor(const QFixedPt<PrecBits>& value)
{
    return value.floor().bits() >> PrecBits;
}

template <int PrecBits>
inline int qRound(const QFixedPt<PrecBits>& value)
{
    return value.round().bits() >> PrecBits;
}

template <int PrecBits>
inline bool qFuzzyCompare(const QFixedPt<PrecBits>& v1, const QFixedPt<PrecBits>& v2, int compareBits = (PrecBits / 4))
{
    return ((v1.bits() ^ v2.bits()) & ~((1 << compareBits) - 1)) == 0;
}

template <int PrecBits>
inline bool qIsNull(const QFixedPt<PrecBits>& v)
{
    return v.bits() == 0;
}

template <int Prec, int PrecBits>
QFixedPt<Prec> qFixedPtToPrecision(const QFixedPt<PrecBits>& value)
{
    QFixedPt<Prec> result;
    if (Prec < PrecBits)
        result.setBits(QFixedPt<PrecBits>::shiftRight(value.bits(), (PrecBits - Prec)));
    else
        result.setBits(QFixedPt<PrecBits>::shiftLeft(value.bits(), (Prec - PrecBits)));
    return result;
}

template <int PrecBits>
inline QDebug &operator<<(QDebug &dbg, const QFixedPt<PrecBits> &f)
{
    return dbg << f.toReal();
}

Q_DECLARE_TYPEINFO(QFixedPt<0>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<1>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<2>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<3>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<4>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<5>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<6>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<7>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<8>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<9>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<10>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<11>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<12>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<13>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<14>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<15>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<16>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<17>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<18>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<19>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<20>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<21>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<22>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<23>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<24>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<25>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<26>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<27>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<28>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<29>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<30>, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QFixedPt<31>, Q_PRIMITIVE_TYPE);

QT_END_NAMESPACE

QT_END_HEADER

#endif
