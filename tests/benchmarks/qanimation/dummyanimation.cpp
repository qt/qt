#include "dummyanimation.h"
#include "dummyobject.h"


DummyAnimation::DummyAnimation(DummyObject *d) : m_dummy(d)
{
}

void DummyAnimation::updateCurrentValue(const QVariant &value)
{
    if (state() == Stopped)
        return;
    if (m_dummy)
        m_dummy->setRect(value.toRect());
}

void DummyAnimation::updateState(State state)
{
    Q_UNUSED(state);
}
