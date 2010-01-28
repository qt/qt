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

#include "qmlgraphicsvideo_p.h"

#include <QtMultimedia/qmediaplayercontrol.h>
#include <QtMultimedia/qmediaservice.h>
#include <QtMultimedia/private/qpaintervideosurface_p.h>
#include <QtMultimedia/qvideooutputcontrol.h>
#include <QtMultimedia/qvideorenderercontrol.h>


QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Video,QmlGraphicsVideo);


void QmlGraphicsVideo::_q_nativeSizeChanged(const QSizeF &size)
{
    setImplicitWidth(size.width());
    setImplicitHeight(size.height());
}

void QmlGraphicsVideo::_q_error(QMediaPlayer::Error errorCode, const QString &errorString)
{
    m_error = errorCode;
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


/*!
    \qmlclass Video QmlGraphicsVideo
    \brief The Video element allows you to add videos to a scene.
    \inherits Item
*/

/*!
    \internal
    \class QmlGraphicsVideo
    \brief The QmlGraphicsVideo class provides a video item that you can add to a QmlView.
*/

QmlGraphicsVideo::QmlGraphicsVideo(QmlGraphicsItem *parent)
    : QmlGraphicsItem(parent)
    , m_graphicsItem(0)
    , m_fillMode(QmlGraphicsVideo::PreserveAspectFit)

{
    m_graphicsItem = new QGraphicsVideoItem(this);
    connect(m_graphicsItem, SIGNAL(nativeSizeChanged(QSizeF)),
            this, SLOT(_q_nativeSizeChanged(QSizeF)));

    setObject(this);

    if (m_mediaService) {
        connect(m_playerControl, SIGNAL(audioAvailableChanged(bool)),
                this, SIGNAL(hasAudioChanged()));
        connect(m_playerControl, SIGNAL(videoAvailableChanged(bool)),
                this, SIGNAL(hasVideoChanged()));

        m_graphicsItem->setMediaObject(m_mediaObject);
    }
}

QmlGraphicsVideo::~QmlGraphicsVideo()
{
    shutdown();

    delete m_graphicsItem;
}

/*!
    \qmlproperty url Video::source

    This property holds the source URL of the video.
*/

/*!
    \qmlproperty bool Video::playing

    This property holds whether the video is playing.
*/

/*!
    \qmlproperty bool Video::paused

    This property holds whether the video is paused.
*/

/*!
    \qmlsignal Video::onStarted()

    This handler is called when playback is started.
*/

/*!
    \qmlsignal Video::onResumed()

    This handler is called when playback is resumed from the paused state.
*/

/*!
    \qmlsignal Video::onPaused()

    This handler is called when playback is paused.
*/

/*!
    \qmlsignal Video::onStopped()

    This handler is called when playback is stopped.
*/

/*!
    \qmlproperty enum Video::status

    This property holds the status of video loading. It can be one of:

    \list
    \o NoMedia - no video has been set.
    \o Loading - the video is currently being loaded.
    \o Loaded - the video has been loaded.
    \o Buffering - the video is buffering data.
    \o Stalled - playback has been interrupted while the video is buffering data.
    \o Buffered - the video has buffered data.
    \o EndOfMedia - the video has played to the end.
    \o InvalidMedia - the video cannot be played.
    \o UnknownStatus - the status of the video is unknown.
    \endlist
*/

QmlGraphicsVideo::Status QmlGraphicsVideo::status() const
{
    return Status(m_status);
}

/*!
    \qmlsignal Video::onLoaded()

    This handler is called when the video source has been loaded.
*/

/*!
    \qmlsignal Video::onBuffering()

    This handler is called when the video stream starts buffering.
*/

/*!
    \qmlsignal Video::onStalled()

    This handler is called when playback has stalled while the video stream buffers.
*/

/*!
    \qmlsignal Video::onBuffered()

    This handler is called when the video stream has finished buffering.
*/

/*!
    \qmlsignal Video::onEndOfMedia

    This handler is called when playback stops because end of the video has been reached.
*/

/*!
    \qmlproperty int Video::duration

    This property holds the duration of the video in milliseconds.

    If the video doesn't have a fixed duration (a live stream for example) this will be 0.
*/

/*!
    \qmlproperty int Video::position

    This property holds the current playback position in milliseconds.
*/

/*!
    \qmlproperty qreal Video::volume

    This property holds the volume of the audio output, from 0.0 (silent) to 1.0 (maximum volume).
*/

/*!
    \qmlproperty bool Video::muted

    This property holds whether the audio output is muted.
*/

/*!
    \qmlproperty bool Audio::hasAudio

    This property holds whether the source contains audio.
*/

bool QmlGraphicsVideo::hasAudio() const
{
    return m_playerControl->isAudioAvailable();
}

/*!
    \qmlproperty bool Video::hasVideo

    This property holds whether the source contains video.
*/

bool QmlGraphicsVideo::hasVideo() const
{
    return m_playerControl->isVideoAvailable();
}

/*!
    \qmlproperty qreal Video::bufferProgress

    This property holds how much of the data buffer is currently filled, from 0.0 (empty) to 1.0
    (full).
*/

/*!
    \qmlproperty bool Video::seekable

    This property holds whether position of the video can be changed.
*/

/*!
    \qmlproperty qreal playbackRate

    This property holds the rate at which video is played at as a multiple of the normal rate.
*/

/*!
    \qmlproperty enum Video::error

    This property holds the error state of the video.  It can be one of:

    \list
    \o NoError - there is no current error.
    \o ResourceError - the video cannot be played due to a problem allocating resources.
    \o FormatError - the video format is not supported.
    \o NetworkError - the video cannot be played due to network issues.
    \o AccessDenied - the video cannot be played due to insufficient permissions.
    \o ServiceMissing -  the video cannot be played because the media service could not be
    instantiated.
    \endlist
*/


QmlGraphicsVideo::Error QmlGraphicsVideo::error() const
{
    return Error(m_error);
}

/*!
    \qmlproperty string Video::errorString

    This property holds a string describing the current error condition in more detail.
*/

/*!
    \qmlproperty Video::onError(error, errorString)

    This property is called when an \l {Error}{error} has occurred.  The errorString parameter
    may contain more detailed information about the error.
*/

/*!
    \qmlproperty enum Video::FillMode

    Set this property to define how the video is scaled to fit the target area.

    \list
    \o Stretch - the video is scaled to fit.
    \o PreserveAspectFit - the video is scaled uniformly to fit without cropping
    \o PreserveAspectCrop - the video is scaled uniformly to fill, cropping if necessary
    \endlist
*/

QmlGraphicsVideo::FillMode QmlGraphicsVideo::fillMode() const
{
    return FillMode(m_graphicsItem->aspectRatioMode());
}

void QmlGraphicsVideo::setFillMode(FillMode mode)
{
    m_graphicsItem->setAspectRatioMode(Qt::AspectRatioMode(mode));
}

/*!
    \qmlmethod Video::play()

    Starts playback of the video.
*/

void QmlGraphicsVideo::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Video::pause()

    Pauses playback of the video.
*/

void QmlGraphicsVideo::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Video::stop()

    Stops playback of the video.
*/

void QmlGraphicsVideo::stop()
{
    m_playerControl->stop();
}

void QmlGraphicsVideo::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
}

void QmlGraphicsVideo::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    m_graphicsItem->setSize(newGeometry.size());

    QmlGraphicsItem::geometryChanged(newGeometry, oldGeometry);
}

QT_END_NAMESPACE

#include "moc_qmlgraphicsvideo_p.cpp"
