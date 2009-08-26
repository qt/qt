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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT_NO_ANIMATION

#include <QtCore/qvariantanimation.h>
#include <private/qvariantanimation_p.h>

#include <QtGui/qcolor.h>
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

template<> Q_INLINE_TEMPLATE QColor _q_interpolate(const QColor &f,const QColor &t, qreal progress)
{
    return QColor(_q_interpolate(f.red(), t.red(), progress),
                  _q_interpolate(f.green(), t.green(), progress),
                  _q_interpolate(f.blue(), t.blue(), progress),
                  _q_interpolate(f.alpha(), t.alpha(), progress));
}

template<> Q_INLINE_TEMPLATE QQuaternion _q_interpolate(const QQuaternion &f,const QQuaternion &t, qreal progress)
{
    return QQuaternion::slerp(f, t, progress);
}

static int qRegisterGuiGetInterpolator()
{
    qRegisterAnimationInterpolator<QColor>(_q_interpolateVariant<QColor>);
    qRegisterAnimationInterpolator<QVector2D>(_q_interpolateVariant<QVector2D>);
    qRegisterAnimationInterpolator<QVector3D>(_q_interpolateVariant<QVector3D>);
    qRegisterAnimationInterpolator<QVector4D>(_q_interpolateVariant<QVector4D>);
    qRegisterAnimationInterpolator<QQuaternion>(_q_interpolateVariant<QQuaternion>);
    return 1;
}
Q_CONSTRUCTOR_FUNCTION(qRegisterGuiGetInterpolator)

static int qUnregisterGuiGetInterpolator()
{
    // casts required by Sun CC 5.5
    qRegisterAnimationInterpolator<QColor>(
        (QVariant (*)(const QColor &, const QColor &, qreal))0);
    qRegisterAnimationInterpolator<QVector2D>(
        (QVariant (*)(const QVector2D &, const QVector2D &, qreal))0);
    qRegisterAnimationInterpolator<QVector3D>(
        (QVariant (*)(const QVector3D &, const QVector3D &, qreal))0);
    qRegisterAnimationInterpolator<QVector4D>(
        (QVariant (*)(const QVector4D &, const QVector4D &, qreal))0);
    qRegisterAnimationInterpolator<QQuaternion>(
        (QVariant (*)(const QQuaternion &, const QQuaternion &, qreal))0);

    return 1;
}
Q_DESTRUCTOR_FUNCTION(qUnregisterGuiGetInterpolator)

QT_END_NAMESPACE

#endif //QT_NO_ANIMATION
