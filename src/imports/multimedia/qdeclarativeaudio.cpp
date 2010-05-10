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

#include "qdeclarativeaudio_p.h"

#include <QtMediaServices/qmediaplayercontrol.h>

QT_BEGIN_NAMESPACE


/*!
    \qmlclass Audio QDeclarativeAudio
    \since 4.7
    \brief The Audio element allows you to add audio playback to a scene.

    This element is part of the \bold{Qt.multimedia 4.7} module.

    \qml
    import Qt 4.7
    import Qt.multimedia 4.7

    Text {
        text: "Click Me!";
        font.pointSize: 24;
        width: 150; height: 50;

        Audio {
            id: playMusic
            source: "music.wav"
        }
        MouseArea {
            id: playArea
            anchors.fill: parent
            onPressed:  { playMusic.play() }
        }
    }
    \endqml

    \sa Video
*/

/*!
    \internal
    \class QDeclarativeAudio
    \brief The QDeclarativeAudio class provides an audio item that you can add to a QDeclarativeView.
*/

void QDeclarativeAudio::_q_error(int errorCode, const QString &errorString)
{
    m_error = QMediaPlayer::Error(errorCode);
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


QDeclarativeAudio::QDeclarativeAudio(QObject *parent)
    : QObject(parent)
{
}

QDeclarativeAudio::~QDeclarativeAudio()
{
    shutdown();
}

/*!
    \qmlmethod Audio::play()

    Starts playback of the media.

    Sets the \l playing property to true, and the \l paused property to false.
*/

void QDeclarativeAudio::play()
{
    if (m_playerControl == 0)
        return;

    setPaused(false);
    setPlaying(true);
}

/*!
    \qmlmethod Audio::pause()

    Pauses playback of the media.

    Sets the \l playing and \l paused properties to true.
*/

void QDeclarativeAudio::pause()
{
    if (m_playerControl == 0)
        return;

    setPaused(true);
    setPlaying(true);
}

/*!
    \qmlmethod Audio::stop()

    Stops playback of the media.

    Sets the \l playing and \l paused properties to false.
*/

void QDeclarativeAudio::stop()
{
    if (m_playerControl == 0)
        return;

    setPlaying(false);
    setPaused(false);
}

/*!
    \qmlproperty url Audio::source

    This property holds the source URL of the media.
*/

/*!
    \qmlproperty url Audio::autoLoad

    This property indicates if loading of media should begin immediately.

    Defaults to true, if false media will not be loaded until playback is started.
*/

/*!
    \qmlproperty bool Audio::playing

    This property holds whether the media is playing.

    Defaults to false, and can be set to true to start playback.
*/

/*!
    \qmlproperty bool Audio::paused

    This property holds whether the media is paused.

    Defaults to false, and can be set to true to pause playback.
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
    \qmlproperty enumeration Audio::status

    This property holds the status of media loading. It can be one of:

    \list
    \o NoMedia - no media has been set.
    \o Loading - the media is currently being loaded.
    \o Loaded - the media has been loaded.
    \o Buffering - the media is buffering data.
    \o Stalled - playback has been interrupted while the media is buffering data.
    \o Buffered - the media has buffered data.
    \o EndOfMedia - the media has played to the end.
    \o InvalidMedia - the media cannot be played.
    \o UnknownStatus - the status of the media is unknown.
    \endlist
*/

QDeclarativeAudio::Status QDeclarativeAudio::status() const
{
    return Status(m_status);
}

/*!
    \qmlsignal Audio::onLoaded()

    This handler is called when the media source has been loaded.
*/

/*!
    \qmlsignal Audio::onBuffering()

    This handler is called when the media  starts buffering.
*/

/*!
    \qmlsignal Audio::onStalled()

    This handler is called when playback has stalled while the media buffers.
*/

/*!
    \qmlsignal Audio::onBuffered()

    This handler is called when the media has finished buffering.
*/

/*!
    \qmlsignal Audio::onEndOfMedia()

    This handler is called when playback stops because end of the media has been reached.
*/
/*!
    \qmlproperty int Audio::duration

    This property holds the duration of the media in milliseconds.

    If the media doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Audio::position

    This property holds the current playback position in milliseconds.

    If the \l seekable property is true, this property can be set to seek to a new position.
*/

/*!
    \qmlproperty real Audio::volume

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Audio::muted

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty real Audio::bufferProgress

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Audio::seekable

    This property holds whether position of the audio can be changed.

    If true; setting a \l position value will cause playback to seek to the new position.
*/

/*!
    \qmlproperty real Audio::playbackRate

    This property holds the rate at which audio is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enumeration Audio::error

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

QDeclarativeAudio::Error QDeclarativeAudio::error() const
{
    return Error(m_error);
}

void QDeclarativeAudio::componentComplete()
{
    setObject(this);
}


/*!
    \qmlproperty string Audio::errorString

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlsignal Audio::onError(error, errorString)

    This handler is called when an \l {QMediaPlayer::Error}{error} has
    occurred.  The errorString parameter may contain more detailed
    information about the error.
*/

QT_END_NAMESPACE

#include "moc_qdeclarativeaudio_p.cpp"


