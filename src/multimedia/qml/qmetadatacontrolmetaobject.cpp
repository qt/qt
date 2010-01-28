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

#include <QtMultimedia/private/qmetadatacontrolmetaobject_p.h>

#include <QtMultimedia>/qmetadatacontrol.h>


QT_BEGIN_NAMESPACE

namespace
{
    struct MetaDataKey
    {
        QtMedia::MetaData key;
        const char *name;
    };

    const MetaDataKey qt_metaDataKeys[] =
    {
        { QtMedia::Title, "title" },
        { QtMedia::SubTitle, "subTitle" },
        { QtMedia::Author, "author" },
        { QtMedia::Comment, "comment" },
        { QtMedia::Description, "description" },
        { QtMedia::Category, "category" },
        { QtMedia::Genre, "genre" },
        { QtMedia::Year, "year" },
        { QtMedia::Date, "date" },
        { QtMedia::UserRating, "userRating" },
        { QtMedia::Keywords, "keywords" },
        { QtMedia::Language, "language" },
        { QtMedia::Publisher, "publisher" },
        { QtMedia::Copyright, "copyright" },
        { QtMedia::ParentalRating, "parentalRating" },
        { QtMedia::RatingOrganisation, "ratingOrganisation" },

        // Media
        { QtMedia::Size, "size" },
        { QtMedia::MediaType, "mediaType" },
        { QtMedia::Duration, "duration" },

        // Audio
        { QtMedia::AudioBitRate, "audioBitRate" },
        { QtMedia::AudioCodec, "audioCodec" },
        { QtMedia::AverageLevel, "averageLevel" },
        { QtMedia::ChannelCount, "channelCount" },
        { QtMedia::PeakValue, "peakValue" },
        { QtMedia::SampleRate, "frequency" },

        // Music
        { QtMedia::AlbumTitle, "albumTitle" },
        { QtMedia::AlbumArtist, "albumArtist" },
        { QtMedia::ContributingArtist, "contributingArtist" },
        { QtMedia::Composer, "composer" },
        { QtMedia::Conductor, "conductor" },
        { QtMedia::Lyrics, "lyrics" },
        { QtMedia::Mood, "mood" },
        { QtMedia::TrackNumber, "trackNumber" },
        { QtMedia::TrackCount, "trackCount" },

        { QtMedia::CoverArtUrlSmall, "coverArtUrlSmall" },
        { QtMedia::CoverArtUrlLarge, "coverArtUrlLarge" },

        // Image/Video
        { QtMedia::Resolution, "resolution" },
        { QtMedia::PixelAspectRatio, "pixelAspectRatio" },

        // Video
        { QtMedia::VideoFrameRate, "videoFrameRate" },
        { QtMedia::VideoBitRate, "videoBitRate" },
        { QtMedia::VideoCodec, "videoCodec" },

        { QtMedia::PosterUrl, "posterUrl" },

        // Movie
        { QtMedia::ChapterNumber, "chapterNumber" },
        { QtMedia::Director, "director" },
        { QtMedia::LeadPerformer, "leadPerformer" },
        { QtMedia::Writer, "writer" },

        // Photos
        { QtMedia::CameraManufacturer, "cameraManufacturer" },
        { QtMedia::CameraModel, "cameraModel" },
        { QtMedia::Event, "event" },
        { QtMedia::Subject, "subject" },
        { QtMedia::Orientation, "orientation" },
        { QtMedia::ExposureTime, "exposureTime" },
        { QtMedia::FNumber, "fNumber" },
        { QtMedia::ExposureProgram, "exposureProgram" },
        { QtMedia::ISOSpeedRatings, "isoSpeedRatings" },
        { QtMedia::ExposureBiasValue, "exposureBiasValue" },
        { QtMedia::DateTimeOriginal, "dateTimeOriginal" },
        { QtMedia::DateTimeDigitized, "dateTimeDigitized" },
        { QtMedia::SubjectDistance, "subjectDistance" },
        { QtMedia::MeteringMode, "meteringMode" },
        { QtMedia::LightSource, "lightSource" },
        { QtMedia::Flash, "flash" },
        { QtMedia::FocalLength, "focalLength" },
        { QtMedia::ExposureMode, "exposureMode" },
        { QtMedia::WhiteBalance, "whiteBalance" },
        { QtMedia::DigitalZoomRatio, "digitalZoomRatio" },
        { QtMedia::FocalLengthIn35mmFilm, "focalLengthIn35mmFilm" },
        { QtMedia::SceneCaptureType, "sceneCaptureType" },
        { QtMedia::GainControl, "gainControl" },
        { QtMedia::Contrast, "contrast" },
        { QtMedia::Saturation, "saturation" },
        { QtMedia::Sharpness, "sharpness" },
        { QtMedia::DeviceSettingDescription, "deviceSettingDescription" }
    };
}

QMetaDataControlMetaObject::QMetaDataControlMetaObject(QMetaDataControl *control, QObject *object)
    : m_control(control)
    , m_object(object)
    , m_mem(0)
    , m_propertyOffset(0)
    , m_signalOffset(0)
{
    m_builder.setSuperClass(m_object->metaObject());
    m_builder.setClassName(m_object->metaObject()->className());
    m_builder.setFlags(QMetaObjectBuilder::DynamicMetaObject);

    QObjectPrivate *op = QObjectPrivate::get(m_object);
    if (op->metaObject)
        m_builder.setSuperClass(op->metaObject);

    m_mem = m_builder.toMetaObject();
    *static_cast<QMetaObject *>(this) = *m_mem;

    op->metaObject = this;
    m_propertyOffset = propertyOffset();
    m_signalOffset = methodOffset();
}

QMetaDataControlMetaObject::~QMetaDataControlMetaObject()
{
    qFree(m_mem);

    QObjectPrivate *op = QObjectPrivate::get(m_object);
    op->metaObject = 0;
}

int QMetaDataControlMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (c == QMetaObject::ReadProperty && id >= m_propertyOffset) {
        int propId = id - m_propertyOffset;

        *reinterpret_cast<QVariant *>(a[0]) = m_control->metaData(m_keys.at(propId));

        return -1;
    } else if (c == QMetaObject::WriteProperty && id >= m_propertyOffset) {
        int propId = id - m_propertyOffset;

        m_control->setMetaData(m_keys.at(propId), *reinterpret_cast<QVariant *>(a[0]));

        activate(m_object, m_signalOffset + propId, 0);

        return -1;
    } else {
        return m_object->qt_metacall(c, id, a);
    }
}

int QMetaDataControlMetaObject::createProperty(const char *name, const char *)
{
    const int count = sizeof(qt_metaDataKeys) / sizeof(MetaDataKey);

    for (int i = 0; i < count; ++i) {
        if (qstrcmp(name, qt_metaDataKeys[i].name) == 0) {
            int id = m_keys.count();
            m_keys.append(qt_metaDataKeys[i].key);

            m_builder.addSignal("__" + QByteArray::number(id) + "()");

            QMetaPropertyBuilder build = m_builder.addProperty(name, "QVariant", id);
            build.setDynamic(true);

            qFree(m_mem);
            m_mem = m_builder.toMetaObject();
            *static_cast<QMetaObject *>(this) = *m_mem;

            return m_propertyOffset + id;
        }
    }

    return -1;
}

void QMetaDataControlMetaObject::metaDataChanged()
{
    for (int i = 0; i < m_keys.count(); ++i)
        activate(m_object, m_signalOffset + i, 0);
}

QT_END_NAMESPACE
