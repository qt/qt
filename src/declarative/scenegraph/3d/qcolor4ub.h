/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QCOLOR4UB_H
#define QCOLOR4UB_H

#include "qt3dglobal.h"
#include <QtGui/qcolor.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_QT3D_EXPORT QColor4ub
{
public:
    QColor4ub();
    QColor4ub(int red, int green, int blue, int alpha = 255);
    QColor4ub(const QColor& color);
    QColor4ub(Qt::GlobalColor color);
    QColor4ub(QRgb rgba);

    QColor4ub& operator=(const QColor& color);
    QColor4ub& operator=(Qt::GlobalColor color);

    int red() const { return m_red; }
    int green() const { return m_green; }
    int blue() const { return m_blue; }
    int alpha() const { return m_alpha; }

    void setRed(int value) { m_red = uchar(value); }
    void setGreen(int value) { m_green = uchar(value); }
    void setBlue(int value) { m_blue = uchar(value); }
    void setAlpha(int value) { m_alpha = uchar(value); }

    qreal redF() const { return m_red / 255.0f; }
    qreal greenF() const { return m_green / 255.0f; }
    qreal blueF() const { return m_blue / 255.0f; }
    qreal alphaF() const { return m_alpha / 255.0f; }

    void setRedF(qreal value) { m_red = uchar(qRound(value * 255.0f)); }
    void setGreenF(qreal value) { m_green = uchar(qRound(value * 255.0f)); }
    void setBlueF(qreal value) { m_blue = uchar(qRound(value * 255.0f)); }
    void setAlphaF(qreal value) { m_alpha = uchar(qRound(value * 255.0f)); }

    void setRgb(int red, int green, int blue, int alpha = 255);
    void setRgbF(qreal red, qreal green, qreal blue, qreal alpha = 1.0f);

    static QColor4ub fromRgb(int red, int green, int blue, int alpha = 255);
    static QColor4ub fromRgbF
        (qreal red, qreal green, qreal blue, qreal alpha = 1.0f);
    static QColor4ub fromRaw(const uchar *data);

    QColor toColor() const;

    bool operator==(const QColor4ub& other) const;
    bool operator!=(const QColor4ub& other) const;

private:
    QColor4ub(const uchar *data);

    uchar m_red;
    uchar m_green;
    uchar m_blue;
    uchar m_alpha;
};

inline QColor4ub::QColor4ub() : m_red(0), m_green(0), m_blue(0), m_alpha(255) {}

inline QColor4ub::QColor4ub(int red, int green, int blue, int alpha)
    : m_red(uchar(red)), m_green(uchar(green)),
      m_blue(uchar(blue)), m_alpha(uchar(alpha)) {}

inline QColor4ub::QColor4ub(const QColor& color)
    : m_red(uchar(color.red())), m_green(uchar(color.green())),
      m_blue(uchar(color.blue())), m_alpha(uchar(color.alpha())) {}

inline QColor4ub::QColor4ub(Qt::GlobalColor color)
{
    QColor c(color);
    m_red = uchar(c.red());
    m_green = uchar(c.green());
    m_blue = uchar(c.blue());
    m_alpha = uchar(c.alpha());
}

inline QColor4ub::QColor4ub(QRgb rgba)
    : m_red(uchar(qRed(rgba))), m_green(uchar(qGreen(rgba))),
      m_blue(uchar(qBlue(rgba))), m_alpha(uchar(qAlpha(rgba))) {}

inline QColor4ub::QColor4ub(const uchar *data)
    : m_red(data[0]), m_green(data[1]), m_blue(data[2]), m_alpha(data[3]) {}

inline QColor4ub& QColor4ub::operator=(const QColor& color)
{
    m_red = uchar(color.red());
    m_green = uchar(color.green());
    m_blue = uchar(color.blue());
    m_alpha = uchar(color.alpha());
    return *this;
}

inline QColor4ub& QColor4ub::operator=(Qt::GlobalColor color)
{
    QColor c(color);
    m_red = uchar(c.red());
    m_green = uchar(c.green());
    m_blue = uchar(c.blue());
    m_alpha = uchar(c.alpha());
    return *this;
}

inline void QColor4ub::setRgb(int red, int green, int blue, int alpha)
{
    m_red = uchar(red);
    m_green = uchar(green);
    m_blue = uchar(blue);
    m_alpha = uchar(alpha);
}

inline void QColor4ub::setRgbF(qreal red, qreal green, qreal blue, qreal alpha)
{
    m_red = uchar(qRound(red * 255.0f));
    m_green = uchar(qRound(green * 255.0f));
    m_blue = uchar(qRound(blue * 255.0f));
    m_alpha = uchar(qRound(alpha * 255.0f));
}

inline QColor4ub QColor4ub::fromRgb(int red, int green, int blue, int alpha)
{
    return QColor4ub(red, green, blue, alpha);
}

inline QColor4ub QColor4ub::fromRgbF
    (qreal red, qreal green, qreal blue, qreal alpha)
{
    return QColor4ub(qRound(red * 255.0f), qRound(green * 255.0f),
                    qRound(blue * 255.0f), qRound(alpha * 255.0f));
}

inline QColor4ub QColor4ub::fromRaw(const uchar *data)
{
    return QColor4ub(data);
}

inline QColor QColor4ub::toColor() const
{
    return QColor(m_red, m_green, m_blue, m_alpha);
}

inline bool QColor4ub::operator==(const QColor4ub& other) const
{
    return m_red == other.m_red && m_green == other.m_green &&
           m_blue == other.m_blue && m_alpha == other.m_alpha;
}

inline bool QColor4ub::operator!=(const QColor4ub& other) const
{
    return m_red != other.m_red || m_green != other.m_green ||
           m_blue != other.m_blue || m_alpha != other.m_alpha;
}

#ifndef QT_NO_DEBUG_STREAM
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QColor4ub &color);
#endif

Q_DECLARE_TYPEINFO(QColor4ub, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QColor4ub)

QT_END_HEADER

#endif
