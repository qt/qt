#include "spritestate.h"

SpriteState::SpriteState(QObject *parent) :
    QObject(parent)
    , m_frames(1)
    , m_duration(1000)
{
}
