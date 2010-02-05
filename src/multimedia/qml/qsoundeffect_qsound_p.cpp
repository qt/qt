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
#include <QtCore/qtimer.h>
#include <QtCore/qfile.h>
#include <QtGui/qsound.h>
#include <QtMultimedia/qaudioformat.h>
#include <QDebug>

#include "qmediacontent.h"
#include "qmediaplayer.h"
#include "qsoundeffect_p.h"

#include "wavedecoder_p.h"

#include "qsoundeffect_qsound_p.h"


QSoundEffectPrivate::QSoundEffectPrivate(QObject* parent):
    QObject(parent),
    m_queued(false),
    m_muted(false),
    m_state(QMediaPlayer::StoppedState),
    m_status(QMediaPlayer::NoMedia),
    m_file(0),
    m_sound(0)
{
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),SLOT(checkPlayTime()));
    m_media = QMediaContent();
}

QSoundEffectPrivate::~QSoundEffectPrivate()
{
    if (m_sound) delete m_sound;
    if (m_waveDecoder) delete m_waveDecoder;
    m_file->close();
}

qint64 QSoundEffectPrivate::duration() const
{
    if (m_waveDecoder)
        return m_waveDecoder->size();

    return 0;
}

int QSoundEffectPrivate::volume() const
{
    return 100;
}

bool QSoundEffectPrivate::isMuted() const
{
    return m_muted;
}

QMediaContent QSoundEffectPrivate::media() const
{
    return m_media;
}

QMediaPlayer::State QSoundEffectPrivate::state() const
{
    return m_state;
}

QMediaPlayer::MediaStatus QSoundEffectPrivate::mediaStatus() const
{
    return m_status;
}

void QSoundEffectPrivate::play()
{
    if (m_sound && !m_muted) {
        m_queued = false;
        m_timer->start(20);
        m_playbackTime.start();
        m_sound->play();
        emit stateChanged(m_state = QMediaPlayer::PlayingState);
    } else if (m_status == QMediaPlayer::LoadingMedia)
        m_queued = true;
}

void QSoundEffectPrivate::stop()
{
    m_timer->stop();

    if (m_sound) {
        m_sound->stop();
        emit stateChanged(m_state = QMediaPlayer::StoppedState);
    }
}

void QSoundEffectPrivate::setVolume(int volume)
{
    Q_UNUSED(volume)
}

void QSoundEffectPrivate::setMuted(bool muted)
{
    m_muted = muted;
}

void QSoundEffectPrivate::setMedia(const QMediaContent &media)
{
    m_queued = false;

    if (media.isNull() || media.canonicalUrl().scheme() != "file") {
        m_media = QMediaContent();
        return;
    }
    if (m_media == media)
        return;

    m_media = media;
    m_file = new QFile(m_media.canonicalUrl().toLocalFile());
    m_file->open(QIODevice::ReadOnly|QIODevice::Unbuffered);

    unloadSample();
    loadSample();

    emit mediaChanged(m_media);
}

void QSoundEffectPrivate::decoderReady()
{
    m_file->close();
    m_sound = new QSound(m_media.canonicalUrl().toLocalFile());
    emit mediaStatusChanged(m_status = QMediaPlayer::LoadedMedia);

    if (m_queued)
        play();
}

void QSoundEffectPrivate::decoderError()
{
    m_file->close();
    emit mediaStatusChanged(m_status = QMediaPlayer::InvalidMedia);
}

void QSoundEffectPrivate::checkPlayTime()
{
    if (m_sound->isFinished()) {
        m_timer->stop();
        m_state = QMediaPlayer::StoppedState;
        emit stateChanged(m_state);
    }
}

void QSoundEffectPrivate::loadSample()
{
    m_waveDecoder = new WaveDecoder(m_file);
    connect(m_waveDecoder, SIGNAL(formatKnown()), SLOT(decoderReady()));
    connect(m_waveDecoder, SIGNAL(invalidFormat()), SLOT(decoderError()));

    m_status = QMediaPlayer::LoadingMedia;
    emit mediaStatusChanged(m_status);
}

void QSoundEffectPrivate::unloadSample()
{
    if (m_sound == 0)
        return;

    m_status = QMediaPlayer::NoMedia;

    if (m_sound)
        delete m_sound;

    m_sound = 0;
}

