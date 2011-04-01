#include "spritestate.h"

QT_BEGIN_NAMESPACE

SpriteState::SpriteState(QObject *parent) :
    QObject(parent)
    , m_frames(1)
    , m_duration(1000)
{
}

QT_END_NAMESPACE
