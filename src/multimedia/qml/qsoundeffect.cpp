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

#include "qmediacontent.h"
#include "qmediaplayer.h"

#include "qsoundeffect_p.h"

#if defined(QT_MULTIMEDIA_PULSEAUDIO)
#include "qsoundeffect_pulse_p.h"
#elif(QT_MULTIMEDIA_QMEDIAPLAYER)
#include "qsoundeffect_qmedia_p.h"
#else
#include "qsoundeffect_qsound_p.h"
#endif

QML_DEFINE_TYPE(Qt,4,6,SoundEffect,QSoundEffect)

/*!
    \qmlclass QSoundEffect
    \brief The SoundEffect element provides a way to play sound effects in qml.

    The following example plays a wav file on mouse click.

    \qml
    SoundEffect {
        id: playSound
        source: "test.wav"
    }
    MouseRegion {
        id: playArea
        anchors.fill: parent
        onPressed: {
            playSound.play()
        }
    }
    \endeml

    \sa SoundEffect
*/

/*!
    \qmlproperty QUrl SoundEffect::source

    This property provides a way to control the sound to play.
*/

/*!
    \qmlproperty int SoundEffect::loopCount

    This property provides a way to control the number of times to repeat the sound on each play().
*/

/*!
    \qmlproperty int SoundEffect::volume

    This property provides a way to control the volume for playback.
*/

/*!
    \qmlproperty bool SoundEffect::muted

    This property provides a way to control muting.
*/

/*!
    \qmlproperty int SoundEffect::duration

    This property holds the duration in milliseconds of the current source audio.
*/

/*!
    \qmlsignal SoundEffect::sourceChanged()

    This handler is called when the source has changed.
*/

/*!
    \qmlsignal SoundEffect::loopCountChanged()

    This handler is called when the number of loops has changes.
*/

/*!
    \qmlsignal SoundEffect::volumeChanged()

    This handler is called when the volume has changed.
*/

/*!
    \qmlsignal SoundEffect::mutedChanged()

    This handler is called when the mute state has changed.
*/

/*!
    \qmlsignal SoundEffect::durationChanged()

    This handler is called when the duration has changed.
*/

QSoundEffect::QSoundEffect(QObject *parent) :
    QObject(parent),
    m_loopCount(1),
    m_volume(100),
    m_muted(false),
    m_runningCount(0)
{
    d = new QSoundEffectPrivate(this);
    connect(d, SIGNAL(volumeChanged(int)), SIGNAL(volumeChanged()));
    connect(d, SIGNAL(mutedChanged(bool)), SIGNAL(mutedChanged()));
    connect(d, SIGNAL(durationChanged(qint64)), SIGNAL(durationChanged()));
    connect(d, SIGNAL(stateChanged(QMediaPlayer::State)), SLOT(repeat()));
}

QSoundEffect::~QSoundEffect()
{
    delete d;
}

QUrl QSoundEffect::source() const
{
    return d != 0 ? d->media().canonicalUrl() : QUrl();
}

void QSoundEffect::setSource(const QUrl &url)
{
    if (d != 0 && d->media().canonicalUrl() == url)
        return;

    d->setVolume(m_volume);
    d->setMuted(m_muted);
    d->setMedia(url);

    if (url.isEmpty())
        return;

    emit sourceChanged();
}

int QSoundEffect::loopCount() const
{
    return m_loopCount;
}

void QSoundEffect::setLoopCount(int loopCount)
{
    if (m_loopCount == loopCount)
        return;

    m_loopCount = loopCount;
    emit loopCountChanged();
}

int QSoundEffect::volume() const
{
    return d != 0 ? d->volume() : m_volume;
}

void QSoundEffect::setVolume(int volume)
{
    if (m_volume == volume)
        return;

    m_volume = volume;
    if (d != 0)
        d->setVolume(volume);
    else
        emit volumeChanged();
}

bool QSoundEffect::isMuted() const
{
    return d !=  0 ? d->isMuted() : m_muted;
}

void QSoundEffect::setMuted(bool muted)
{
    if (m_muted == muted)
        return;

    m_muted = muted;
    if (d != 0)
        d->setMuted(muted);
    else
        emit mutedChanged();
}

int QSoundEffect::duration() const
{
    return d != 0 ? d->duration() : 0;
}

void QSoundEffect::play()
{
    m_runningCount = 0;

    if (d != 0)
        d->play();
}

void QSoundEffect::stop()
{
    if (d != 0)
        d->stop();
}

void QSoundEffect::repeat()
{
    if (d->state() == QMediaPlayer::StoppedState) {
        if (++m_runningCount < m_loopCount)
            d->play();
    }
}

