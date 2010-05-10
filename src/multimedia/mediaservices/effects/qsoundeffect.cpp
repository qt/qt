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

#include "qsoundeffect_p.h"

#if defined(QT_MULTIMEDIA_PULSEAUDIO)
#include "qsoundeffect_pulse_p.h"
#elif(QT_MULTIMEDIA_QMEDIAPLAYER)
#include "qsoundeffect_qmedia_p.h"
#else
#include "qsoundeffect_qsound_p.h"
#endif

QT_BEGIN_NAMESPACE

/*!
    \qmlclass SoundEffect QSoundEffect
    \since 4.7
    \brief The SoundEffect element provides a way to play sound effects in QML.

    This element is part of the \bold{Qt.multimedia 4.7} module.

    The following example plays a wav file on mouse click.

    \qml
    import Qt 4.7
    import Qt.multimedia 4.7

    Text {
        text: "Click Me!";
        font.pointSize: 24;
        width: 150; height: 50;

        SoundEffect {
            id: playSound
            source: "soundeffect.wav"
        }
        MouseArea {
            id: playArea
            anchors.fill: parent
            onPressed: { playSound.play() }
        }
    }
    \endqml
*/

/*!
    \qmlproperty url SoundEffect::source

    This property provides a way to control the sound to play.
*/

/*!
    \qmlproperty int SoundEffect::loops

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
    \qmlsignal SoundEffect::sourceChanged()

    This handler is called when the source has changed.
*/

/*!
    \qmlsignal SoundEffect::loopsChanged()

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


QSoundEffect::QSoundEffect(QObject *parent) :
    QObject(parent)
{
    d = new QSoundEffectPrivate(this);
    connect(d, SIGNAL(volumeChanged()), SIGNAL(volumeChanged()));
    connect(d, SIGNAL(mutedChanged()), SIGNAL(mutedChanged()));
}

QSoundEffect::~QSoundEffect()
{
    d->deleteLater();
}

QUrl QSoundEffect::source() const
{
    return d->source();
}

void QSoundEffect::setSource(const QUrl &url)
{
    if (d->source() == url)
        return;

    d->setSource(url);

    emit sourceChanged();
}

int QSoundEffect::loops() const
{
    return d->loopCount();
}

void QSoundEffect::setLoops(int loopCount)
{
    if (d->loopCount() == loopCount)
        return;

    d->setLoopCount(loopCount);
    emit loopsChanged();
}

int QSoundEffect::volume() const
{
    return d->volume();
}

void QSoundEffect::setVolume(int volume)
{
    if (d->volume() == volume)
        return;

    d->setVolume(volume);
    emit volumeChanged();
}

bool QSoundEffect::isMuted() const
{
    return d->isMuted();
}

void QSoundEffect::setMuted(bool muted)
{
    if (d->isMuted() == muted)
        return;

    d->setMuted(muted);
    emit mutedChanged();
}

void QSoundEffect::play()
{
    d->play();
}

QT_END_NAMESPACE
