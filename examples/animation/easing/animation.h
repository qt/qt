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

#ifndef ANIMATION_H
#define ANIMATION_H

#include <QtGui>

#include <QtCore/qpropertyanimation.h>

class Animation : public QPropertyAnimation {
public:
    enum PathType {
        LinearPath,
        CirclePath,
        NPathTypes
    };
    Animation(QObject *target, const QByteArray &prop)
        : QPropertyAnimation(target, prop)
    {
        setPathType(LinearPath);
    }

    void setPathType(PathType pathType)
    {
        if (pathType >= NPathTypes)
            qWarning("Unknown pathType %d", pathType);

        m_pathType = pathType;
        m_path = QPainterPath();
    }

    void updateCurrentTime(int msecs)
    {
        if (m_pathType == CirclePath) {
            if (m_path.isEmpty()) {
                QPointF to = endValue().toPointF();
                QPointF from = startValue().toPointF();
                m_path.moveTo(from);
                m_path.addEllipse(QRectF(from, to));
            }
            int dura = duration();
            const qreal progress = ((dura == 0) ? 1 : ((((currentTime() - 1) % dura) + 1) / qreal(dura)));

            qreal easedProgress = easingCurve().valueForProgress(progress);
            if (easedProgress > 1.0) {
                easedProgress -= 1.0;
            } else if (easedProgress < 0) {
                easedProgress += 1.0;
            }
            QPointF pt = m_path.pointAtPercent(easedProgress);
            updateCurrentValue(pt);
            emit valueChanged(pt);
        } else {
            QPropertyAnimation::updateCurrentTime(msecs);
        }
    }

    QPainterPath m_path;
    PathType m_pathType;
};

#endif // ANIMATION_H
