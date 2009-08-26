/*  This file is part of the KDE project.

Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mmf_medianode.h"

QT_BEGIN_NAMESPACE

using namespace Phonon;
using namespace Phonon::MMF;

MMF::MediaNode::MediaNode(QObject *parent) : QObject::QObject(parent)
                                           , m_source(0)
                                           , m_target(0)
{
}

bool MMF::MediaNode::connectMediaNode(MediaNode *target)
{
    m_target = target;
    m_target->setSource(this);
    return true;
}

bool MMF::MediaNode::disconnectMediaNode(MediaNode *target)
{
    Q_UNUSED(target);
    m_target = 0;
    return false;
}

void MMF::MediaNode::setSource(MediaNode *source)
{
    m_source = source;
}

MMF::MediaNode *MMF::MediaNode::source() const
{
    return m_source;
}

MMF::MediaNode *MMF::MediaNode::target() const
{
    return m_target;
}


QT_END_NAMESPACE

