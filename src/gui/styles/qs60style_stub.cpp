/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qs60style.h"
#include "qdebug.h"

#if defined(QT_NO_STYLE_S60)
QT_BEGIN_NAMESPACE

QS60Style::QS60Style()
{
    qWarning() << "QS60Style stub created";
}

QS60Style::~QS60Style()
{
}

void QS60Style::drawComplexControl(ComplexControl , const QStyleOptionComplex *, QPainter *, const QWidget *) const
{
}

void QS60Style::drawControl(ControlElement , const QStyleOption *, QPainter *, const QWidget *) const
{
}

void QS60Style::drawPrimitive(PrimitiveElement , const QStyleOption *, QPainter *, const QWidget *) const
{
}

int QS60Style::pixelMetric(PixelMetric , const QStyleOption *, const QWidget *) const
{
    return 0;
}

QSize QS60Style::sizeFromContents(ContentsType , const QStyleOption *, const QSize &, const QWidget *) const
{
    return QSize();
}

int QS60Style::styleHint(StyleHint , const QStyleOption *, const QWidget *, QStyleHintReturn *) const
{
    return 0;
}

QRect QS60Style::subControlRect(ComplexControl , const QStyleOptionComplex *, SubControl , const QWidget *) const
{
    return QRect();
}

QRect QS60Style::subElementRect(SubElement , const QStyleOption *, const QWidget *) const
{
    return QRect();
}

void QS60Style::polish(QWidget *)
{
}

void QS60Style::unpolish(QWidget *)
{
}

void QS60Style::polish(QApplication *)
{
}

void QS60Style::unpolish(QApplication *)
{
}

bool QS60Style::event(QEvent *)
{
    return false;
}

QIcon QS60Style::standardIconImplementation(StandardPixmap , const QStyleOption *, const QWidget *) const
{
    return QIcon();
}

void QS60Style::timerEvent(QTimerEvent *)
{
}

bool QS60Style::eventFilter(QObject *, QEvent *)
{
    return false;
}

QT_END_NAMESPACE

#endif // QT_NO_STYLE_S60
