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

#ifndef GFXEASING_H
#define GFXEASING_H

#include <qfxglobal.h>
#include <QList>
#include <QPair>
#include <QWidget>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class GfxEasingFunction;
class Q_DECLARATIVE_EXPORT GfxEasing
{
public:
    enum Curve {
                 None,
                 InQuad, OutQuad, InOutQuad, OutInQuad,
                 InCubic, OutCubic, InOutCubic, OutInCubic,
                 InQuart, OutQuart, InOutQuart, OutInQuart,
                 InQuint, OutQuint, InOutQuint, OutInQuint,
                 InSine, OutSine, InOutSine, OutInSine,
                 InExpo, OutExpo, InOutExpo, OutInExpo,
                 InCirc, OutCirc, InOutCirc, OutInCirc,
                 InElastic, OutElastic, InOutElastic, OutInElastic,
                 InBack, OutBack, InOutBack, OutInBack,
                 InBounce, OutBounce, InOutBounce, OutInBounce
               };

    GfxEasing();
    GfxEasing(Curve);
    GfxEasing(const QString &);
    GfxEasing(const GfxEasing &);
    GfxEasing &operator=(const GfxEasing &);

    bool isLinear() const;

    qreal from() const;
    void setFrom(qreal);
    qreal to() const;
    void setTo(qreal);
    qreal length() const;
    void setLength(qreal);

    qreal valueAt(qreal t) const;
    qreal valueAt(qreal t, qreal from, qreal to, qreal length) const;

    static QStringList curves();

    typedef float (*Function)(float t, float b, float c, float d);
private:
    GfxEasingFunction *_config;
    Function _func;

    qreal _b, _c, _d;
};


QT_END_NAMESPACE

QT_END_HEADER
#endif
