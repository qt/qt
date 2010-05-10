/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediaplayercontrol.h"
#include "s60mediaplayersession.h"
#include "s60mediaplayeraudioendpointselector.h"

#include <QtGui/QIcon>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

S60MediaPlayerAudioEndpointSelector::S60MediaPlayerAudioEndpointSelector(QObject *control, QObject *parent)
   :QMediaControl(parent)
    , m_control(0)
    , m_audioEndpointNames(0)
{
    m_control = qobject_cast<S60MediaPlayerControl*>(control);
}

S60MediaPlayerAudioEndpointSelector::~S60MediaPlayerAudioEndpointSelector()
{
    delete m_audioEndpointNames;
}

QList<QString> S60MediaPlayerAudioEndpointSelector::availableEndpoints() const
{
    if(m_audioEndpointNames->count() == 0) {
        m_audioEndpointNames->append("Default");
        m_audioEndpointNames->append("All");
        m_audioEndpointNames->append("None");
        m_audioEndpointNames->append("Earphone");
        m_audioEndpointNames->append("Speaker");
    }
    return *m_audioEndpointNames;
}

QString S60MediaPlayerAudioEndpointSelector::endpointDescription(const QString& name) const
{
    if (name == QString("Default")) //ENoPreference
        return QString("Used to indicate that the playing audio can be routed to"
            "any speaker. This is the default value for audio.");
    else if (name == QString("All")) //EAll
        return QString("Used to indicate that the playing audio should be routed to all speakers.");
    else if (name == QString("None")) //ENoOutput
        return QString("Used to indicate that the playing audio should not be routed to any output.");
    else if (name == QString("Earphone")) //EPrivate
        return QString("Used to indicate that the playing audio should be routed to"
            "the default private speaker. A private speaker is one that can only"
            "be heard by one person.");
    else if (name == QString("Speaker")) //EPublic
        return QString("Used to indicate that the playing audio should be routed to"
            "the default public speaker. A public speaker is one that can "
            "be heard by multiple people.");

    return QString();
}

QString S60MediaPlayerAudioEndpointSelector::activeEndpoint() const
{
    if (m_control->session())
        return m_control->session()->activeEndpoint();
    else
        return m_control->mediaControlSettings().audioEndpoint();
}

QString S60MediaPlayerAudioEndpointSelector::defaultEndpoint() const
{
    if (m_control->session())
        return m_control->session()->defaultEndpoint();
    else
        return m_control->mediaControlSettings().audioEndpoint();
}

void S60MediaPlayerAudioEndpointSelector::setActiveEndpoint(const QString& name)
{
    QString oldEndpoint = m_control->mediaControlSettings().audioEndpoint();

    if (name != oldEndpoint && (name == QString("Default") || name == QString("All") ||
        name == QString("None") || name == QString("Earphone") || name == QString("Speaker"))) {

        if (m_control->session()) {
            m_control->session()->setActiveEndpoint(name);
        }
        m_control->setAudioEndpoint(name);
    }
}

QT_END_NAMESPACE
