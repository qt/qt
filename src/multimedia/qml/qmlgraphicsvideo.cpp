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


void QmlGraphicsVideo::_q_nativeSizeChanged(const QSizeF &size)
{
    setImplicitWidth(size.width());
    setImplicitHeight(size.height());
}

void QmlGraphicsVideo::_q_error(int errorCode, const QString &errorString)
{
    m_error = QMediaPlayer::Error(errorCode);
    m_errorString = errorString;

    emit error(Error(errorCode), errorString);
    emit errorChanged();
}


/*!
    \qmlclass Video QmlGraphicsVideo
    \brief The Video element allows you to add videos to a scene.
    \inherits Item

    \qml
    Video { source: "video/movie.mpg" }
    \endqml

    The video item supports untransformed, stretched, and uniformly scaled video presentation.
    For a description of stretched uniformly scaled presentation, see the \l fillMode property
    description.

    The video item is only visible when the \l hasVideo property is true and the video is playing.

    \sa Audio
*/

/*!
    \internal
    \class QmlGraphicsVideo
    \brief The QmlGraphicsVideo class provides a video item that you can add to a QmlView.
*/

QmlGraphicsVideo::QmlGraphicsVideo(QmlGraphicsItem *parent)
    : QmlGraphicsItem(parent)
    , m_graphicsItem(0)

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

    This property holds the source URL of the media.
*/

/*!
    \qmlproperty bool Video::playing

    This property holds whether the media is playing.

    Defaults to false, and can be set to true to start playback.
*/

/*!
    \qmlproperty bool Video::paused

    This property holds whether the media is paused.

    Defaults to false, and can be set to true to pause playback.
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
    \o UnknownStatus - the status of the media is cannot be determined.
    \endlist
*/

QmlGraphicsVideo::Status QmlGraphicsVideo::status() const
{
    return Status(m_status);
}

/*!
    \qmlsignal Video::onLoaded()

    This handler is called when the media source has been loaded.
*/

/*!
    \qmlsignal Video::onBuffering()

    This handler is called when the media starts buffering.
*/

/*!
    \qmlsignal Video::onStalled()

    This handler is called when playback has stalled while the media buffers.
*/

/*!
    \qmlsignal Video::onBuffered()

    This handler is called when the media has finished buffering.
*/

/*!
    \qmlsignal Video::onEndOfMedia()

    This handler is called when playback stops because end of the media has been reached.
*/

/*!
    \qmlproperty int Video::duration

    This property holds the duration of the media in milliseconds.

    If the media doesn't have a fixed duration (a live stream for example) this will be 0.
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
    \qmlproperty bool Video::hasAudio

    This property holds whether the media contains audio.
*/

bool QmlGraphicsVideo::hasAudio() const
{
    return m_playerControl->isAudioAvailable();
}

/*!
    \qmlproperty bool Video::hasVideo

    This property holds whether the media contains video.
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
    \qmlproperty enum Video::fillMode

    Set this property to define how the video is scaled to fit the target area.

    \list
    \o Stretch - the video is scaled to fit.
    \o PreserveAspectFit - the video is scaled uniformly to fit without cropping
    \o PreserveAspectCrop - the video is scaled uniformly to fill, cropping if necessary
    \endlist

    The default fill mode is PreserveAspectFit.
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

    Starts playback of the media.

    Sets the \l playing property to true, and the \l paused property to false.
*/

void QmlGraphicsVideo::play()
{
    m_playerControl->play();

    if (m_paused) {
        m_paused = false;
        emit pausedChanged();
    }
}

/*!
    \qmlmethod Video::pause()

    Pauses playback of the media.

    Sets the \l playing and \l paused properties to true.
*/

void QmlGraphicsVideo::pause()
{
    m_playerControl->pause();

    if (!m_paused && m_state == QMediaPlayer::PausedState) {
        m_paused = true;
        emit pausedChanged();
    }
}

/*!
    \qmlmethod Video::stop()

    Stops playback of the media.

    Sets the \l playing and \l paused properties to false.
*/

void QmlGraphicsVideo::stop()
{
    m_playerControl->stop();

    if (m_paused) {
        m_paused = false;
        emit pausedChanged();
    }
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

// ***************************************
// Documentation for meta-data properties.
// ***************************************

/*!
    \qmlproperty variant Video::title

    This property holds the tile of the media.

    \sa {QtMedia::Title}
*/

/*!
    \qmlproperty variant Video::subTitle

    This property holds the sub-title of the media.

    \sa {QtMedia::SubTitle}
*/

/*!
    \qmlproperty variant Video::author

    This property holds the author of the media.

    \sa {QtMedia::Author}
*/

/*!
    \qmlproperty variant Video::comment

    This property holds a user comment about the media.

    \sa {QtMedia::Comment}
*/

/*!
    \qmlproperty variant Video::description

    This property holds a description of the media.

    \sa {QtMedia::Description}
*/

/*!
    \qmlproperty variant Video::category

    This property holds the category of the media

    \sa {QtMedia::Category}
*/

/*!
    \qmlproperty variant Video::genre

    This property holds the genre of the media.

    \sa {QtMedia::Genre}
*/

/*!
    \qmlproperty variant Video::year

    This property holds the year of release of the media.

    \sa {QtMedia::Year}
*/

/*!
    \qmlproperty variant Video::date

    This property holds the date of the media.

    \sa {QtMedia::Date}
*/

/*!
    \qmlproperty variant Video::userRating

    This property holds a user rating of the media in the range of 0 to 100.

    \sa {QtMedia::UserRating}
*/

/*!
    \qmlproperty variant Video::keywords

    This property holds a list of keywords describing the media.

    \sa {QtMedia::Keywords}
*/

/*!
    \qmlproperty variant Video::language

    This property holds the language of the media, as an ISO 639-2 code.

    \sa {QtMedia::Language}
*/

/*!
    \qmlproperty variant Video::publisher

    This property holds the publisher of the media.

    \sa {QtMedia::Publisher}
*/

/*!
    \qmlproperty variant Video::copyright

    This property holds the media's copyright notice.

    \sa {QtMedia::Copyright}
*/

/*!
    \qmlproperty variant Video::parentalRating

    This property holds the parental rating of the media.

    \sa {QtMedia::ParentalRating}
*/

/*!
    \qmlproperty variant Video::ratingOrganisation

    This property holds the name of the rating organisation responsible for the
    parental rating of the media.

    \sa {QtMedia::RatingOrganisation}
*/

/*!
    \qmlproperty variant Video::size

    This property property holds the size of the media in bytes.

    \sa {QtMedia::Size}
*/

/*!
    \qmlproperty variant Video::mediaType

    This property holds the type of the media.

    \sa {QtMedia::MediaType}
*/

/*!
    \qmlproperty variant Video::audioBitRate

    This property holds the bit rate of the media's audio stream ni bits per
    second.

    \sa {QtMedia::AudioBitRate}
*/

/*!
    \qmlproperty variant Video::audioCodec

    This property holds the encoding of the media audio stream.

    \sa {QtMedia::AudioCodec}
*/

/*!
    \qmlproperty variant Video::averageLevel

    This property holds the average volume level of the media.

    \sa {QtMedia::AverageLevel}
*/

/*!
    \qmlproperty variant Video::channelCount

    This property holds the number of channels in the media's audio stream.

    \sa {QtMedia::ChannelCount}
*/

/*!
    \qmlproperty variant Video::peakValue

    This property holds the peak volume of media's audio stream.

    \sa {QtMedia::PeakValue}
*/

/*!
    \qmlproperty variant Video::sampleRate

    This property holds the sample rate of the media's audio stream in hertz.

    \sa {QtMedia::SampleRate}
*/

/*!
    \qmlproperty variant Video::albumTitle

    This property holds the title of the album the media belongs to.

    \sa {QtMedia::AlbumTitle}
*/

/*!
    \qmlproperty variant Video::albumArtist

    This property holds the name of the principal artist of the album the media
    belongs to.

    \sa {QtMedia::AlbumArtist}
*/

/*!
    \qmlproperty variant Video::contributingArtist

    This property holds the names of artists contributing to the media.

    \sa {QtMedia::ContributingArtist}
*/

/*!
    \qmlproperty variant Video::composer

    This property holds the composer of the media.

    \sa {QtMedia::Composer}
*/

/*!
    \qmlproperty variant Video::conductor

    This property holds the conductor of the media.

    \sa {QtMedia::Conductor}
*/

/*!
    \qmlproperty variant Video::lyrics

    This property holds the lyrics to the media.

    \sa {QtMedia::Lyrics}
*/

/*!
    \qmlproperty variant Video::mood

    This property holds the mood of the media.

    \sa {QtMedia::Mood}
*/

/*!
    \qmlproperty variant Video::trackNumber

    This property holds the track number of the media.

    \sa {QtMedia::TrackNumber}
*/

/*!
    \qmlproperty variant Video::trackCount

    This property holds the number of track on the album containing the media.

    \sa {QtMedia::TrackNumber}
*/

/*!
    \qmlproperty variant Video::coverArtUrlSmall

    This property holds the URL of a small cover art image.

    \sa {QtMedia::CoverArtUrlSmall}
*/

/*!
    \qmlproperty variant Video::coverArtUrlLarge

    This property holds the URL of a large cover art image.

    \sa {QtMedia::CoverArtUrlLarge}
*/

/*!
    \qmlproperty variant Video::resolution

    This property holds the dimension of an image or video.

    \sa {QtMedia::Resolution}
*/

/*!
    \qmlproperty variant Video::pixelAspectRatio

    This property holds the pixel aspect ratio of an image or video.

    \sa {QtMedia::PixelAspectRatio}
*/

/*!
    \qmlproperty variant Video::videoFrameRate

    This property holds the frame rate of the media's video stream.

    \sa {QtMedia::VideoFrameRate}
*/

/*!
    \qmlproperty variant Video::videoBitRate

    This property holds the bit rate of the media's video stream in bits per
    second.

    \sa {QtMedia::VideoBitRate}
*/

/*!
    \qmlproperty variant Video::videoCodec

    This property holds the encoding of the media's video stream.

    \sa {QtMedia::VideoCodec}
*/

/*!
    \qmlproperty variant Video::posterUrl

    This property holds the URL of a poster image.

    \sa {QtMedia::PosterUrl}
*/

/*!
    \qmlproperty variant Video::chapterNumber

    This property holds the chapter number of the media.

    \sa {QtMedia::ChapterNumber}
*/

/*!
    \qmlproperty variant Video::director

    This property holds the director of the media.

    \sa {QtMedia::Director}
*/

/*!
    \qmlproperty variant Video::leadPerformer

    This property holds the lead performer in the media.

    \sa {QtMedia::LeadPerformer}
*/

/*!
    \qmlproperty variant Video::writer

    This property holds the writer of the media.

    \sa {QtMedia::Writer}
*/

// The remaining properties are related to photos, and are technically
// available but will certainly never have values.
#ifndef Q_QDOC

/*!
    \qmlproperty variant Video::cameraManufacturer

    \sa {QtMedia::CameraManufacturer}
*/

/*!
    \qmlproperty variant Video::cameraModel

    \sa {QtMedia::CameraModel}
*/

/*!
    \qmlproperty variant Video::event

    \sa {QtMedia::Event}
*/

/*!
    \qmlproperty variant Video::subject

    \sa {QtMedia::Subject}
*/

/*!
    \qmlproperty variant Video::orientation

    \sa {QtMedia::Orientation}
*/

/*!
    \qmlproperty variant Video::exposureTime

    \sa {QtMedia::ExposureTime}
*/

/*!
    \qmlproperty variant Video::fNumber

    \sa {QtMedia::FNumber}
*/

/*!
    \qmlproperty variant Video::exposureProgram

    \sa {QtMedia::ExposureProgram}
*/

/*!
    \qmlproperty variant Video::isoSpeedRatings

    \sa {QtMedia::ISOSpeedRatings}
*/

/*!
    \qmlproperty variant Video::exposureBiasValue

    \sa {QtMedia::ExposureBiasValue}
*/

/*!
    \qmlproperty variant Video::dateTimeDigitized

    \sa {QtMedia::DateTimeDigitized}
*/

/*!
    \qmlproperty variant Video::subjectDistance

    \sa {QtMedia::SubjectDistance}
*/

/*!
    \qmlproperty variant Video::meteringMode

    \sa {QtMedia::MeteringMode}
*/

/*!
    \qmlproperty variant Video::lightSource

    \sa {QtMedia::LightSource}
*/

/*!
    \qmlproperty variant Video::flash

    \sa {QtMedia::Flash}
*/

/*!
    \qmlproperty variant Video::focalLength

    \sa {QtMedia::FocalLength}
*/

/*!
    \qmlproperty variant Video::exposureMode

    \sa {QtMedia::ExposureMode}
*/

/*!
    \qmlproperty variant Video::whiteBalance

    \sa {QtMedia::WhiteBalance}
*/

/*!
    \qmlproperty variant Video::DigitalZoomRatio

    \sa {QtMedia::DigitalZoomRatio}
*/

/*!
    \qmlproperty variant Video::focalLengthIn35mmFilm

    \sa {QtMedia::FocalLengthIn35mmFile}
*/

/*!
    \qmlproperty variant Video::sceneCaptureType

    \sa {QtMedia::SceneCaptureType}
*/

/*!
    \qmlproperty variant Video::gainControl

    \sa {QtMedia::GainControl}
*/

/*!
    \qmlproperty variant Video::contrast

    \sa {QtMedia::contrast}
*/

/*!
    \qmlproperty variant Video::saturation

    \sa {QtMedia::Saturation}
*/

/*!
    \qmlproperty variant Video::sharpness

    \sa {QtMedia::Sharpness}
*/

/*!
    \qmlproperty variant Video::deviceSettingDescription

    \sa {QtMedia::DeviceSettingDescription}
*/

#endif

#include "moc_qmlgraphicsvideo_p.cpp"
