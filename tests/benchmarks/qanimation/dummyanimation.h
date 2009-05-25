#include <QtGui>

#ifndef _DUMMYANIMATION_H__

class DummyObject;

class DummyAnimation : public QVariantAnimation
{
public:
    DummyAnimation(DummyObject *d);

    void updateCurrentValue(const QVariant &value);
    void updateState(State state);

private:
    DummyObject *m_dummy;
};

#endif