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

#include "qgstreamermetadataprovider.h"
#include "qgstreamerplayersession.h"
#include <QtCore/qdebug.h>

#include <gst/gstversion.h>

QT_BEGIN_NAMESPACE

struct QGstreamerMetaDataKeyLookup
{
    QtMediaservices::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QtMediaservices::Title, GST_TAG_TITLE },
    //{ QtMediaservices::SubTitle, 0 },
    //{ QtMediaservices::Author, 0 },
    { QtMediaservices::Comment, GST_TAG_COMMENT },
    { QtMediaservices::Description, GST_TAG_DESCRIPTION },
    //{ QtMediaservices::Category, 0 },
    { QtMediaservices::Genre, GST_TAG_GENRE },
    { QtMediaservices::Year, "year" },
    //{ QtMediaservices::UserRating, 0 },

    { QtMediaservices::Language, GST_TAG_LANGUAGE_CODE },

    { QtMediaservices::Publisher, GST_TAG_ORGANIZATION },
    { QtMediaservices::Copyright, GST_TAG_COPYRIGHT },
    //{ QtMediaservices::ParentalRating, 0 },
    //{ QtMediaservices::RatingOrganisation, 0 },

    // Media
    //{ QtMediaservices::Size, 0 },
    //{ QtMediaservices::MediaType, 0 },
    { QtMediaservices::Duration, GST_TAG_DURATION },

    // Audio
    { QtMediaservices::AudioBitRate, GST_TAG_BITRATE },
    { QtMediaservices::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QtMediaservices::ChannelCount, 0 },
    //{ QtMediaservices::Frequency, 0 },

    // Music
    { QtMediaservices::AlbumTitle, GST_TAG_ALBUM },
    { QtMediaservices::AlbumArtist,  GST_TAG_ARTIST},
    { QtMediaservices::ContributingArtist, GST_TAG_PERFORMER },
#if (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
    { QtMediaservices::Composer, GST_TAG_COMPOSER },
#endif
    //{ QtMediaservices::Conductor, 0 },
    //{ QtMediaservices::Lyrics, 0 },
    //{ QtMediaservices::Mood, 0 },
    { QtMediaservices::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QtMediaservices::CoverArtUrlSmall, 0 },
    //{ QtMediaservices::CoverArtUrlLarge, 0 },

    // Image/Video
    //{ QtMediaservices::Resolution, 0 },
    //{ QtMediaservices::PixelAspectRatio, 0 },

    // Video
    //{ QtMediaservices::VideoFrameRate, 0 },
    //{ QtMediaservices::VideoBitRate, 0 },
    { QtMediaservices::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QtMediaservices::PosterUrl, 0 },

    // Movie
    //{ QtMediaservices::ChapterNumber, 0 },
    //{ QtMediaservices::Director, 0 },
    { QtMediaservices::LeadPerformer, GST_TAG_PERFORMER },
    //{ QtMediaservices::Writer, 0 },

    // Photos
    //{ QtMediaservices::CameraManufacturer, 0 },
    //{ QtMediaservices::CameraModel, 0 },
    //{ QtMediaservices::Event, 0 },
    //{ QtMediaservices::Subject, 0 }
};

QGstreamerMetaDataProvider::QGstreamerMetaDataProvider(QGstreamerPlayerSession *session, QObject *parent)
    :QMetaDataControl(parent), m_session(session)
{
    connect(m_session, SIGNAL(tagsChanged()), SLOT(updateTags()));
}

QGstreamerMetaDataProvider::~QGstreamerMetaDataProvider()
{
}

bool QGstreamerMetaDataProvider::isMetaDataAvailable() const
{
    return !m_session->tags().isEmpty();
}

bool QGstreamerMetaDataProvider::isWritable() const
{
    return false;
}

QVariant QGstreamerMetaDataProvider::metaData(QtMediaservices::MetaData key) const
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            return m_session->tags().value(QByteArray(qt_gstreamerMetaDataKeys[i].token));
        }
    }
    return QVariant();
}

void QGstreamerMetaDataProvider::setMetaData(QtMediaservices::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QList<QtMediaservices::MetaData> QGstreamerMetaDataProvider::availableMetaData() const
{
    static QMap<QByteArray, QtMediaservices::MetaData> keysMap;
    if (keysMap.isEmpty()) {
        const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            keysMap[QByteArray(qt_gstreamerMetaDataKeys[i].token)] = qt_gstreamerMetaDataKeys[i].key;
        }
    }

    QList<QtMediaservices::MetaData> res;
    foreach (const QByteArray &key, m_session->tags().keys()) {
        QtMediaservices::MetaData tag = keysMap.value(key, QtMultimedia::MetaData(-1));
        if (tag != -1)
            res.append(tag);
    }

    return res;
}

QVariant QGstreamerMetaDataProvider::extendedMetaData(const QString &key) const
{
    return m_session->tags().value(key.toLatin1());
}

void QGstreamerMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QStringList QGstreamerMetaDataProvider::availableExtendedMetaData() const
{
    QStringList res;
    foreach (const QByteArray &key, m_session->tags().keys())
        res.append(QString(key));

    return res;
}

void QGstreamerMetaDataProvider::updateTags()
{
    emit metaDataChanged();
}

QT_END_NAMESPACE

