/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef LINJAZAXGESTURE_H
#define LINJAZAXGESTURE_H

#include <QGesture>

class LinjaZaxGesture : public QGesture
{
public:
    enum DirectionType
    {
        None = 0,
        LeftDown = 1,
        DownLeft = LeftDown,
        Down = 2,
        RightDown = 3,
        DownRight = RightDown,
        Left = 4,
        Right = 6,
        LeftUp = 7,
        UpLeft = LeftUp,
        Up = 8,
        RightUp = 9,
        UpRight = RightUp
    };

    enum ZoomState
    {
        NoZoom,
        ZoomingIn,
        ZoomingOut
    };

public:
    explicit LinjaZaxGesture(QObject *parent,
                             Qt::GestureState state = Qt::GestureStarted)
        : QGesture(parent, QLatin1String("LinjaZax"), state), lastDirection_(None),
        direction_(None), zoomState_(NoZoom) { }
    LinjaZaxGesture(QObject *parent, const QPoint &startPos,
                    const QPoint &lastPos, const QPoint &pos, const QRect &rect,
                    const QPoint &hotSpot, const QDateTime &startTime,
                    uint duration, Qt::GestureState state)
        : QGesture(parent, QLatin1String("LinjaZax"), startPos, lastPos,
                   pos, rect, hotSpot, startTime, duration, state) { }
    ~LinjaZaxGesture() { }

    DirectionType lastDirection() const
    { return lastDirection_; }
    DirectionType direction() const
    { return direction_; }

    ZoomState zoomState() const
    { return zoomState_; }

private:
    DirectionType lastDirection_;
    DirectionType direction_;
    ZoomState zoomState_;
    friend class GestureRecognizerLinjaZax;
};

#endif
