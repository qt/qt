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

#include "qmlaudio_p.h"

#include <QtMultimedia/qmediaplayercontrol.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Audio,QmlAudio);

/*!
    \qmlclass Audio QmlAudio
    \brief The Audio element allows you to add audio to a scene.
*/

/*!
    \internal
    \class QmlAudio
    \brief The QmlAudio class provides a audio item that you can add to a QmlView.
*/

void QmlAudio::_q_error(QMediaPlayer::Error errorCode, const QString &errorString)
{
    m_error = errorCode;
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


QmlAudio::QmlAudio(QObject *parent)
    : QObject(parent)
{
    setObject(this);
}

QmlAudio::~QmlAudio()
{
    shutdown();
}

/*!
    \qmlmethod Audio::play()

    Starts playback of the audio.
*/

void QmlAudio::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Audio::pause()

    Pauses playback of the audio.
*/

void QmlAudio::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Audio::stop()

    Stops playback of the audio.
*/

void QmlAudio::stop()
{
    m_playerControl->stop();
}

/*!
    \qmlproperty url Audio::source

    This property holds the source URL of the audio.
*/

/*!
    \qmlproperty bool Audio::playing

    This property holds whether the audio is playing.
*/

/*!
    \qmlproperty bool Audio::paused

    This property holds whether the audio is paused.
*/

/*!
    \qmlsignal Audio::onStarted()

    This handler is called when playback is started.
*/

/*!
    \qmlsignal Audio::onResumed()

    This handler is called when playback is resumed from the paused state.
*/

/*!
    \qmlsignal Audio::onPaused()

    This handler is called when playback is paused.
*/

/*!
    \qmlsignal Audio::onStopped()

    This handler is called when playback is stopped.
*/

/*!
    \qmlproperty enum Audio::status

    This property holds the status of audio loading. It can be one of:

    \list
    \o NoMedia - no audio has been set.
    \o Loading - the audio is currently being loaded.
    \o Loaded - the audio has been loaded.
    \o Buffering - the audio is buffering data.
    \o Stalled - playback has been interrupted while the audio is buffering data.
    \o Buffered - the audio has buffered data.
    \o EndOfMedia - the audio has played to the end.
    \o InvalidMedia - the audio cannot be played.
    \o UnknownStatus - the status of the audio is unknown.
    \endlist
*/

QmlAudio::Status QmlAudio::status() const
{
    return Status(m_status);
}

/*!
    \qmlsignal Audio::onLoaded()

    This handler is called when the video source has been loaded.
*/

/*!
    \qmlsignal Audio::onBuffering()

    This handler is called when the video stream starts buffering.
*/

/*!
    \qmlsignal Audio::onStalled()

    This handler is called when playback has stalled while the video stream buffers.
*/

/*!
    \qmlsignal Audio::onBuffered()

    This handler is called when the video stream has finished buffering.
*/

/*!
    \qmlsignal Audio::onEndOfMedia

    This handler is called when playback stops because end of the video has been reached.
*/
/*!
    \qmlproperty int Audio::duration

    This property holds the duration of the audio in milliseconds.

    If the audio doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Audio::position

    This property holds the current playback position in milliseconds.
*/

/*!
    \qmlproperty qreal Audio::volume

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Audio::muted

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty qreal Audio::bufferProgress

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Audio::seekable

    This property holds whether position of the audio can be changed.
*/

/*!
    \qmlproperty qreal playbackRate

    This property holds the rate at which audio is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enum Audio::error

    This property holds the error state of the audio.  It can be one of:

    \list
    \o NoError - there is no current error.
    \o ResourceError - the audio cannot be played due to a problem allocating resources.
    \o FormatError - the audio format is not supported.
    \o NetworkError - the audio cannot be played due to network issues.
    \o AccessDenied - the audio cannot be played due to insufficient permissions.
    \o ServiceMissing -  the audio cannot be played because the media service could not be
    instantiated.
    \endlist
*/

QmlAudio::Error QmlAudio::error() const
{
    return Error(m_error);
}

/*!
    \qmlproperty string Audio::errorString

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlproperty Audio::onError(error, errorString)

    This property is called when an \l {Error}{error} has occurred.  The errorString parameter
    may contain more detailed information about the error.
*/

QT_END_NAMESPACE

#include "moc_qmlaudio_p.cpp"


