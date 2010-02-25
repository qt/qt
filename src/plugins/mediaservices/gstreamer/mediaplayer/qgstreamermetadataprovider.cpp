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

struct QGstreamerMetaDataKeyLookup
{
    QtMultimedia::MetaData key;
    const char *token;
};

static const QGstreamerMetaDataKeyLookup qt_gstreamerMetaDataKeys[] =
{
    { QtMultimedia::Title, GST_TAG_TITLE },
    //{ QtMultimedia::SubTitle, 0 },
    //{ QtMultimedia::Author, 0 },
    { QtMultimedia::Comment, GST_TAG_COMMENT },
    { QtMultimedia::Description, GST_TAG_DESCRIPTION },
    //{ QtMultimedia::Category, 0 },
    { QtMultimedia::Genre, GST_TAG_GENRE },
    { QtMultimedia::Year, "year" },
    //{ QtMultimedia::UserRating, 0 },

    { QtMultimedia::Language, GST_TAG_LANGUAGE_CODE },

    { QtMultimedia::Publisher, GST_TAG_ORGANIZATION },
    { QtMultimedia::Copyright, GST_TAG_COPYRIGHT },
    //{ QtMultimedia::ParentalRating, 0 },
    //{ QtMultimedia::RatingOrganisation, 0 },

    // Media
    //{ QtMultimedia::Size, 0 },
    //{ QtMultimedia::MediaType, 0 },
    { QtMultimedia::Duration, GST_TAG_DURATION },

    // Audio
    { QtMultimedia::AudioBitRate, GST_TAG_BITRATE },
    { QtMultimedia::AudioCodec, GST_TAG_AUDIO_CODEC },
    //{ QtMultimedia::ChannelCount, 0 },
    //{ QtMultimedia::Frequency, 0 },

    // Music
    { QtMultimedia::AlbumTitle, GST_TAG_ALBUM },
    { QtMultimedia::AlbumArtist,  GST_TAG_ARTIST},
    { QtMultimedia::ContributingArtist, GST_TAG_PERFORMER },
#if (GST_VERSION_MAJOR >= 0) && (GST_VERSION_MINOR >= 10) && (GST_VERSION_MICRO >= 19)
    { QtMultimedia::Composer, GST_TAG_COMPOSER },
#endif
    //{ QtMultimedia::Conductor, 0 },
    //{ QtMultimedia::Lyrics, 0 },
    //{ QtMultimedia::Mood, 0 },
    { QtMultimedia::TrackNumber, GST_TAG_TRACK_NUMBER },

    //{ QtMultimedia::CoverArtUrlSmall, 0 },
    //{ QtMultimedia::CoverArtUrlLarge, 0 },

    // Image/Video
    //{ QtMultimedia::Resolution, 0 },
    //{ QtMultimedia::PixelAspectRatio, 0 },

    // Video
    //{ QtMultimedia::VideoFrameRate, 0 },
    //{ QtMultimedia::VideoBitRate, 0 },
    { QtMultimedia::VideoCodec, GST_TAG_VIDEO_CODEC },

    //{ QtMultimedia::PosterUrl, 0 },

    // Movie
    //{ QtMultimedia::ChapterNumber, 0 },
    //{ QtMultimedia::Director, 0 },
    { QtMultimedia::LeadPerformer, GST_TAG_PERFORMER },
    //{ QtMultimedia::Writer, 0 },

    // Photos
    //{ QtMultimedia::CameraManufacturer, 0 },
    //{ QtMultimedia::CameraModel, 0 },
    //{ QtMultimedia::Event, 0 },
    //{ QtMultimedia::Subject, 0 }
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

QVariant QGstreamerMetaDataProvider::metaData(QtMultimedia::MetaData key) const
{
    static const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);

    for (int i = 0; i < count; ++i) {
        if (qt_gstreamerMetaDataKeys[i].key == key) {
            return m_session->tags().value(QByteArray(qt_gstreamerMetaDataKeys[i].token));
        }
    }
    return QVariant();
}

void QGstreamerMetaDataProvider::setMetaData(QtMultimedia::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QList<QtMultimedia::MetaData> QGstreamerMetaDataProvider::availableMetaData() const
{
    static QMap<QByteArray, QtMultimedia::MetaData> keysMap;
    if (keysMap.isEmpty()) {
        const int count = sizeof(qt_gstreamerMetaDataKeys) / sizeof(QGstreamerMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            keysMap[QByteArray(qt_gstreamerMetaDataKeys[i].token)] = qt_gstreamerMetaDataKeys[i].key;
        }
    }

    QList<QtMultimedia::MetaData> res;
    foreach (const QByteArray &key, m_session->tags().keys()) {
        QtMultimedia::MetaData tag = keysMap.value(key, QtMultimedia::MetaData(-1));
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
