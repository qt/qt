#ifndef LINJAZAXGESTURE_H
#define LINJAZAXGESTURE_H

#include <QGesture>

class Q_GUI_EXPORT LinjaZaxGesture : public QGesture
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
    explicit LinjaZaxGesture(const Qt::GestureType &type, Qt::GestureState state = Qt::GestureStarted)
        : QGesture(type, state), lastDirection_(None), direction_(None), zoomState_(NoZoom) { }
    LinjaZaxGesture(const Qt::GestureType &type, const QPoint &startPos,
                     const QPoint &lastPos, const QPoint &pos, const QRect &rect,
                     const QPoint &hotSpot, const QDateTime &startTime,
                     uint duration, Qt::GestureState state)
        : QGesture(type, startPos, lastPos, pos, rect, hotSpot, startTime, duration, state) { }
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
