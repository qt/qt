/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qcoreapplication.h>

#include "qmediacontent.h"
#include "qmediaplayer.h"

#include "qsoundeffect_p.h"
#include "qsoundeffect_qmedia_p.h"


QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_muted(false),
    m_volume(100),
    m_player(0)
{
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
    if (m_player) delete m_player;
}

qint64 QSoundEffectPrivate::duration() const
{
    if (m_player) return m_player->duration();

    return 0;
}

int QSoundEffectPrivate::volume() const
{
    if (m_player) return m_player->volume();

    return m_volume;
}

bool QSoundEffectPrivate::isMuted() const
{
    if (m_player) return m_player->isMuted();

    return m_muted;
}

QMediaContent QSoundEffectPrivate::media() const
{
    if (m_player) return m_player->media();

    return QMediaContent();
}

QMediaPlayer::State QSoundEffectPrivate::state() const
{
    if (m_player) return m_player->state();

    return QMediaPlayer::StoppedState;
}

QMediaPlayer::MediaStatus QSoundEffectPrivate::mediaStatus() const
{
    if (m_player) return m_player->mediaStatus();

    return QMediaPlayer::UnknownMediaStatus;
}

void QSoundEffectPrivate::play()
{
    if (m_player && !m_player->isMuted())
        m_player->play();
}

void QSoundEffectPrivate::stop()
{
    if (m_player)
        m_player->stop();
}

void QSoundEffectPrivate::setVolume(int volume)
{
    m_volume = volume;

    if (m_player)
        m_player->setVolume(volume);
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_muted = muted;

    if (m_player)
        m_player->setMuted(muted);
}

void QSoundEffectPrivate::setMedia(const QMediaContent &media)
{
    if (media.isNull())
        return;

    if (m_player == 0) {
        m_player = new QMediaPlayer(this, QMediaPlayer::LowLatency);
        m_player->setVolume(m_volume);
        m_player->setMuted(m_muted);

        connect(m_player, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged(int)));
        connect(m_player, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged(bool)));
        connect(m_player, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged(qint64)));
        connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), SIGNAL(stateChanged(QMediaPlayer::State)));
    }

    m_player->setMedia(media.canonicalUrl());
}

