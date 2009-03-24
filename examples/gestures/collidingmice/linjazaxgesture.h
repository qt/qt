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
