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

#include <QtCore/qpropertyanimation.h>
#include <QtGui/qwidget.h>
#include <QtGui/private/qmainwindowlayout_p.h>

#include "qwidgetanimator_p.h"

QT_BEGIN_NAMESPACE

QWidgetAnimator::QWidgetAnimator(QMainWindowLayout *layout) : m_mainWindowLayout(layout)
{
}

void QWidgetAnimator::abort(QWidget *w)
{
    AnimationMap::iterator it = m_animation_map.find(w);
    if (it == m_animation_map.end())
        return;
    QPropertyAnimation *anim = *it;
    m_animation_map.erase(it);
    anim->stop();
    m_mainWindowLayout->animationFinished(w);
}

void QWidgetAnimator::animationFinished()
{
    QPropertyAnimation *anim = qobject_cast<QPropertyAnimation*>(sender());
    abort(static_cast<QWidget*>(anim->targetObject()));
}

void QWidgetAnimator::animate(QWidget *widget, const QRect &_final_geometry, bool animate)
{
    QRect final_geometry = _final_geometry;

    QRect r = widget->geometry();
    if (r.right() < 0 || r.bottom() < 0)
        r = QRect();

    if (r.isNull() || final_geometry.isNull() || r == final_geometry)
        animate = false;

    AnimationMap::const_iterator it = m_animation_map.constFind(widget);
    if (it != m_animation_map.constEnd() && (*it)->endValue().toRect() == final_geometry)
        return;

    if (animate) {
        QPropertyAnimation *anim = new QPropertyAnimation(widget, "geometry");
        anim->setDuration(200);
        anim->setEasingCurve(QEasingCurve::InOutQuad);
        anim->setEndValue(final_geometry);
        m_animation_map[widget] = anim;
        connect(anim, SIGNAL(finished()), SLOT(animationFinished()));
        anim->start(QPropertyAnimation::DeleteWhenStopped);
    } else {
        if (!final_geometry.isValid() && !widget->isWindow()) {
            // Make the wigdet go away by sending it to negative space
            QSize s = widget->size();
            final_geometry = QRect(-500 - s.width(), -500 - s.height(), s.width(), s.height());
        }
        widget->setGeometry(final_geometry);
    }
}

bool QWidgetAnimator::animating() const
{
    return !m_animation_map.isEmpty();
}

bool QWidgetAnimator::animating(QWidget *widget) const
{
    return m_animation_map.contains(widget);
}

QT_END_NAMESPACE
