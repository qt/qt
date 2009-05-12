#include <QtGui>

#ifndef _RECTANIMATION_H__

class DummyObject;

//this class is even simpler than the dummy
//and uses no QVariant at all
class RectAnimation : public QAbstractAnimation
{
public:
    RectAnimation(DummyObject *obj);

    void setEndValue(const QRect &rect);
    void setStartValue(const QRect &rect);

    void setDuration(int d);
    int duration() const;

    virtual void updateCurrentTime(int msecs);
    virtual void updateState(QAbstractAnimation::State state);

private:
    DummyObject *m_object;
    QEasingCurve m_easing;
    QRect m_start, m_end, m_current;
    int m_dura;
};

#endif
