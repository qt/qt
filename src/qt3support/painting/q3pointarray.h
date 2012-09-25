/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q3POINTARRAY_H
#define Q3POINTARRAY_H

#include <QtGui/qpolygon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q_COMPAT_EXPORT Q3PointArray : public QPolygon
{
public:
    inline Q3PointArray() : QPolygon() {}
    inline Q3PointArray(const QRect &r, bool closed=false) : QPolygon(r, closed) {}
    inline Q3PointArray(const QPolygon& a) : QPolygon(a) {}

    inline Q3PointArray copy() const { return *this; }
    inline bool isNull() { return isEmpty(); }
    void makeEllipse(int x, int y, int w, int h);
#ifndef QT_NO_WMATRIX
    void makeArc(int x, int y, int w, int h, int a1, int a2);
    void makeArc(int x, int y, int w, int h, int a1, int a2, const QMatrix &matrix);
#endif
    Q3PointArray cubicBezier() const;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3POINTARRAY_H
