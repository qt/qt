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

#include "s60mediametadataprovider.h"
#include "s60mediaplayersession.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

S60MediaMetaDataProvider::S60MediaMetaDataProvider(MS60MediaPlayerResolver& mediaPlayerResolver, QObject *parent)
    : QMetaDataControl(parent)
    , m_mediaPlayerResolver(mediaPlayerResolver)
    , m_session(NULL)
{
}

S60MediaMetaDataProvider::~S60MediaMetaDataProvider()
{
}

bool S60MediaMetaDataProvider::isMetaDataAvailable() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session)
       return m_session->isMetadataAvailable();
    return false;
}

bool S60MediaMetaDataProvider::isWritable() const
{
    return false;
}

QVariant S60MediaMetaDataProvider::metaData(QtMediaServices::MetaData key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(metaDataKeyAsString(key));
    return QVariant();
}

void S60MediaMetaDataProvider::setMetaData(QtMediaServices::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}
QList<QtMediaServices::MetaData> S60MediaMetaDataProvider::availableMetaData() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    QList<QtMediaServices::MetaData> metaDataTags;
    if (m_session && m_session->isMetadataAvailable()) {
        for (int i = QtMediaServices::Title; i <= QtMediaServices::DeviceSettingDescription; i++) {
            QString metaData = metaDataKeyAsString((QtMediaServices::MetaData)i);
            if (!metaData.isEmpty()) {
                if (!m_session->metaData(metaData).toString().isEmpty()) {
                    metaDataTags.append((QtMediaServices::MetaData)i);
                }
            }
        }
    }
    return metaDataTags;
}

QVariant S60MediaMetaDataProvider::extendedMetaData(const QString &key) const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->metaData(key);
    return QVariant();
}

void S60MediaMetaDataProvider::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QStringList S60MediaMetaDataProvider::availableExtendedMetaData() const
{
    m_session = m_mediaPlayerResolver.PlayerSession();
    if (m_session && m_session->isMetadataAvailable())
        return m_session->availableMetaData().keys();
    return QStringList();
}

QString S60MediaMetaDataProvider::metaDataKeyAsString(QtMediaServices::MetaData key) const
{
    switch(key) {
        case QtMediaServices::Title: return "title";
        case QtMediaServices::AlbumArtist: return "artist";
        case QtMediaServices::Comment: return "comment";
        case QtMediaServices::Genre: return "genre";
        case QtMediaServices::Year: return "year";
        case QtMediaServices::Copyright: return "copyright";
        case QtMediaServices::AlbumTitle: return "album";
        case QtMediaServices::Composer: return "composer";
        case QtMediaServices::TrackNumber: return "albumtrack";
        case QtMediaServices::AudioBitRate: return "audiobitrate";
        case QtMediaServices::VideoBitRate: return "videobitrate";
        case QtMediaServices::Duration: return "duration";
        case QtMediaServices::MediaType: return "contenttype";
        case QtMediaServices::SubTitle: // TODO: Find the matching metadata keys
        case QtMediaServices::Description:
        case QtMediaServices::Category:
        case QtMediaServices::Date:
        case QtMediaServices::UserRating:
        case QtMediaServices::Keywords:
        case QtMediaServices::Language:
        case QtMediaServices::Publisher:
        case QtMediaServices::ParentalRating:
        case QtMediaServices::RatingOrganisation:
        case QtMediaServices::Size:
        case QtMediaServices::AudioCodec:
        case QtMediaServices::AverageLevel:
        case QtMediaServices::ChannelCount:
        case QtMediaServices::PeakValue:
        case QtMediaServices::SampleRate:
        case QtMediaServices::Author:
        case QtMediaServices::ContributingArtist:
        case QtMediaServices::Conductor:
        case QtMediaServices::Lyrics:
        case QtMediaServices::Mood:
        case QtMediaServices::TrackCount:
        case QtMediaServices::CoverArtUrlSmall:
        case QtMediaServices::CoverArtUrlLarge:
        case QtMediaServices::Resolution:
        case QtMediaServices::PixelAspectRatio:
        case QtMediaServices::VideoFrameRate:
        case QtMediaServices::VideoCodec:
        case QtMediaServices::PosterUrl:
        case QtMediaServices::ChapterNumber:
        case QtMediaServices::Director:
        case QtMediaServices::LeadPerformer:
        case QtMediaServices::Writer:
        case QtMediaServices::CameraManufacturer:
        case QtMediaServices::CameraModel:
        case QtMediaServices::Event:
        case QtMediaServices::Subject:
        default:
            break;
    }

    return QString();
}

QT_END_NAMESPACE

