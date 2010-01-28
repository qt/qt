/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include "qmlsound_p.h"

#include <QtMultimedia/qmediaplayer.h>


QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Sound,QmlSound)

QmlSound::QmlSound(QObject *parent) :
    QObject(parent),
    m_loopCount(1),
    m_volume(100),
    m_muted(false),
    m_runningCount(0),
    m_player(0)
{
}

QmlSound::~QmlSound()
{
    delete m_player;
}

QUrl QmlSound::source() const
{
    return m_player != 0 ? m_player->media().canonicalUrl() : QUrl();
}

void QmlSound::setSource(const QUrl &url)
{
    if (m_player != 0 && m_player->media().canonicalUrl() == url)
        return;

    if (m_player == 0) {
        m_player = new QMediaPlayer(this, QMediaPlayer::LowLatency);
        m_player->setVolume(m_volume);
        m_player->setMuted(m_muted);

        connect(m_player, SIGNAL(volumeChanged()), SIGNAL(volumeChanged()));
        connect(m_player, SIGNAL(mutedChanged()), SIGNAL(mutedChanged()));
        connect(m_player, SIGNAL(durationChanged()), SIGNAL(durationChanged()));
        connect(m_player, SIGNAL(stateChanged()), SLOT(repeat()));
    }

    m_player->setMedia(url);
    if (url.isEmpty())
        return;

    emit sourceChanged();
}

int QmlSound::loopCount() const
{
    return m_loopCount;
}

void QmlSound::setLoopCount(int loopCount)
{
    if (m_loopCount == loopCount)
        return;

    m_loopCount = loopCount;
    emit loopCountChanged();
}

int QmlSound::volume() const
{
    return m_player != 0 ? m_player->volume() : m_volume;
}

void QmlSound::setVolume(int volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;
    if (m_player != 0)
        m_player->setVolume(volume);
    else
        emit volumeChanged();
}

bool QmlSound::isMuted() const
{
    return m_player !=  0 ? m_player->isMuted() : m_muted;
}

void QmlSound::setMuted(bool muted)
{
    if (m_muted == muted)
        return;

    m_muted = muted;
    if (m_player != 0)
        m_player->setMuted(muted);
    else
        emit mutedChanged();
}

int QmlSound::duration() const
{
    return m_player != 0 ? m_player->duration() : 0;
}

void QmlSound::play()
{
    m_runningCount = 0;
    if (m_player != 0)
        m_player->play();
}

void QmlSound::stop()
{
    if (m_player != 0)
        m_player->stop();
}

void QmlSound::repeat()
{
    if (m_player->state() == QMediaPlayer::StoppedState) {
        if (++m_runningCount < m_loopCount)
            m_player->play();
    }
}

QT_END_NAMESPACE
