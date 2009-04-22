#include "rectanimation.h"
#include "dummyobject.h"

static inline int interpolateInteger(int from, int to, qreal progress)
{
   return from + (to - from) * progress;
}


RectAnimation::RectAnimation(DummyObject *obj) : object(obj), dura(250)
{
}

void RectAnimation::setEndValue(const QRect &rect)
{
    end = rect;
}

void RectAnimation::setStartValue(const QRect &rect)
{
    start = rect;
}

void RectAnimation::setDuration(int d)
{
    dura = d;
}

int RectAnimation::duration() const
{
    return dura;
}


void RectAnimation::updateCurrentTime(int msecs)
{
    qreal progress = easing.valueForProgress( qreal(msecs) / qreal(dura) );
    QRect now;
    now.setCoords(interpolateInteger(start.left(), end.left(), progress),
                  interpolateInteger(start.top(), end.top(), progress),
                  interpolateInteger(start.right(), end.right(), progress),
                  interpolateInteger(start.bottom(), end.bottom(), progress));

    bool changed = (now != current);
    if (changed)
        current = now;

    if (state() == Stopped)
        return;

    if (object)
        object->setRect(current);
}

void RectAnimation::updateState(QAbstractAnimation::State state)
{
    Q_UNUSED(state);
}
