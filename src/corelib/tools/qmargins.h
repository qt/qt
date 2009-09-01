/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#ifndef QMARGINS_H
#define QMARGINS_H

#include <QtCore/qnamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

class Q_CORE_EXPORT QMargins
{
public:
    QMargins();
    QMargins(int left, int top, int right, int bottom);

    bool isNull() const;

    int left() const;
    int top() const;
    int right() const;
    int bottom() const;

    void setLeft(int left);
    void setTop(int top);
    void setRight(int right);
    void setBottom(int bottom);

private:
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

    friend inline bool operator==(const QMargins &, const QMargins &);
    friend inline bool operator!=(const QMargins &, const QMargins &);
};

Q_DECLARE_TYPEINFO(QMargins, Q_MOVABLE_TYPE);

/*****************************************************************************
  QMargins inline functions
 *****************************************************************************/

inline QMargins::QMargins()
{ m_top = m_bottom = m_left = m_right = 0; }

inline QMargins::QMargins(int left, int top, int right, int bottom)
{ m_left = left; m_top = top; m_right = right; m_bottom = bottom; }

inline bool QMargins::isNull() const
{ return m_left==0 && m_top==0 && m_right==0 && m_bottom==0; }

inline int QMargins::left() const
{ return m_left; }

inline int QMargins::top() const
{ return m_top; }

inline int QMargins::right() const
{ return m_right; }

inline int QMargins::bottom() const
{ return m_bottom; }


inline void QMargins::setLeft(int left)
{ m_left = left; }

inline void QMargins::setTop(int top)
{ m_top = top; }

inline void QMargins::setRight(int right)
{ m_right = right; }

inline void QMargins::setBottom(int bottom)
{ m_bottom = bottom; }

inline bool operator==(const QMargins &m1, const QMargins &m2)
{
    return
            m1.m_left == m2.m_left &&
            m1.m_top == m2.m_top &&
            m1.m_right == m2.m_right &&
            m1.m_bottom == m2.m_bottom;
}

inline bool operator!=(const QMargins &m1, const QMargins &m2)
{
    return
            m1.m_left != m2.m_left ||
            m1.m_top != m2.m_top ||
            m1.m_right != m2.m_right ||
            m1.m_bottom != m2.m_bottom;
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QMargins &);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMARGINS_H
