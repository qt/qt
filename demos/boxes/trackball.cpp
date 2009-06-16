/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#include "trackball.h"

//============================================================================//
//                                  TrackBall                                 //
//============================================================================//

TrackBall::TrackBall(TrackMode mode)
    : m_angularVelocity(0)
    , m_paused(false)
    , m_pressed(false)
    , m_mode(mode)
{
    m_axis = gfx::Vector3f::vector(0, 1, 0);
    m_rotation = gfx::Quaternionf::quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    m_lastTime = QTime::currentTime();
}

TrackBall::TrackBall(float angularVelocity, const gfx::Vector3f& axis, TrackMode mode)
    : m_axis(axis)
    , m_angularVelocity(angularVelocity)
    , m_paused(false)
    , m_pressed(false)
    , m_mode(mode)
{
    m_rotation = gfx::Quaternionf::quaternion(1.0f, 0.0f, 0.0f, 0.0f);
    m_lastTime = QTime::currentTime();
}

void TrackBall::push(const QPointF& p, const gfx::Quaternionf &)
{
    m_rotation = rotation();
    m_pressed = true;
    m_lastTime = QTime::currentTime();
    m_lastPos = p;
    m_angularVelocity = 0.0f;
}

void TrackBall::move(const QPointF& p, const gfx::Quaternionf &transformation)
{
    if (!m_pressed)
        return;

    QTime currentTime = QTime::currentTime();
    int msecs = m_lastTime.msecsTo(currentTime);
    if (msecs <= 20)
        return;

    switch (m_mode) {
    case Plane:
        {
            QLineF delta(m_lastPos, p);
            m_angularVelocity = delta.length() / msecs;
            m_axis = gfx::Vector3f::vector(delta.dy(), -delta.dx(), 0.0f).normalized();
            m_axis = transformation.transform(m_axis);
            m_rotation *= gfx::Quaternionf::rotation(delta.length(), m_axis);
        }
        break;
    case Sphere:
        {
            gfx::Vector3f lastPos3D = gfx::Vector3f::vector(m_lastPos.x(), m_lastPos.y(), 0);
            float sqrZ = 1 - lastPos3D.sqrNorm();
            if (sqrZ > 0)
                lastPos3D[2] = sqrt(sqrZ);
            else
                lastPos3D.normalize();

            gfx::Vector3f currentPos3D = gfx::Vector3f::vector(p.x(), p.y(), 0);
            sqrZ = 1 - currentPos3D.sqrNorm();
            if (sqrZ > 0)
                currentPos3D[2] = sqrt(sqrZ);
            else
                currentPos3D.normalize();

            m_axis = gfx::Vector3f::cross(currentPos3D, lastPos3D);
            float angle = asin(sqrt(m_axis.sqrNorm()));

            m_angularVelocity = angle / msecs;
            m_axis.normalize();
            m_axis = transformation.transform(m_axis);
            m_rotation *= gfx::Quaternionf::rotation(angle, m_axis);
        }
        break;
    }

    m_lastPos = p;
    m_lastTime = currentTime;
}

void TrackBall::release(const QPointF& p, const gfx::Quaternionf &transformation)
{
    // Calling move() caused the rotation to stop if the framerate was too low.
    move(p, transformation);
    m_pressed = false;
}

void TrackBall::start()
{
    m_lastTime = QTime::currentTime();
    m_paused = false;
}

void TrackBall::stop()
{
    m_rotation = rotation();
    m_paused = true;
}

gfx::Quaternionf TrackBall::rotation() const
{
    if (m_paused || m_pressed)
        return m_rotation;

    QTime currentTime = QTime::currentTime();
    float angle = m_angularVelocity * m_lastTime.msecsTo(currentTime);
    return m_rotation * gfx::Quaternionf::rotation(angle, m_axis);
}

