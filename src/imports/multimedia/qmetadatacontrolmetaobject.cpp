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

#include "qmetadatacontrolmetaobject_p.h"
#include <QtMediaServices/qmetadatacontrol.h>


QT_BEGIN_NAMESPACE

// copied from qmetaobject.cpp
// do not touch without touching the moc as well
enum PropertyFlags  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
//    Override = 0x00000200,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000,
    Notify = 0x00400000,
    Dynamic = 0x00800000
};

enum MethodFlags  {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    AccessMask = 0x03, //mask

    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodConstructor = 0x0c,
    MethodTypeMask = 0x0c,

    MethodCompatibility = 0x10,
    MethodCloned = 0x20,
    MethodScriptable = 0x40
};

struct QMetaObjectPrivate
{
    int revision;
    int className;
    int classInfoCount, classInfoData;
    int methodCount, methodData;
    int propertyCount, propertyData;
    int enumeratorCount, enumeratorData;
    int constructorCount, constructorData;
    int flags;
};

static inline const QMetaObjectPrivate *priv(const uint* m_data)
{ return reinterpret_cast<const QMetaObjectPrivate*>(m_data); }
// end of copied lines from qmetaobject.cpp

namespace
{
    struct MetaDataKey
    {
        QtMediaServices::MetaData key;
        const char *name;
    };

    const MetaDataKey qt_metaDataKeys[] =
    {
        { QtMediaServices::Title, "title" },
        { QtMediaServices::SubTitle, "subTitle" },
        { QtMediaServices::Author, "author" },
        { QtMediaServices::Comment, "comment" },
        { QtMediaServices::Description, "description" },
        { QtMediaServices::Category, "category" },
        { QtMediaServices::Genre, "genre" },
        { QtMediaServices::Year, "year" },
        { QtMediaServices::Date, "date" },
        { QtMediaServices::UserRating, "userRating" },
        { QtMediaServices::Keywords, "keywords" },
        { QtMediaServices::Language, "language" },
        { QtMediaServices::Publisher, "publisher" },
        { QtMediaServices::Copyright, "copyright" },
        { QtMediaServices::ParentalRating, "parentalRating" },
        { QtMediaServices::RatingOrganisation, "ratingOrganisation" },

        // Media
        { QtMediaServices::Size, "size" },
        { QtMediaServices::MediaType, "mediaType" },
//        { QtMediaServices::Duration, "duration" },

        // Audio
        { QtMediaServices::AudioBitRate, "audioBitRate" },
        { QtMediaServices::AudioCodec, "audioCodec" },
        { QtMediaServices::AverageLevel, "averageLevel" },
        { QtMediaServices::ChannelCount, "channelCount" },
        { QtMediaServices::PeakValue, "peakValue" },
        { QtMediaServices::SampleRate, "sampleRate" },

        // Music
        { QtMediaServices::AlbumTitle, "albumTitle" },
        { QtMediaServices::AlbumArtist, "albumArtist" },
        { QtMediaServices::ContributingArtist, "contributingArtist" },
        { QtMediaServices::Composer, "composer" },
        { QtMediaServices::Conductor, "conductor" },
        { QtMediaServices::Lyrics, "lyrics" },
        { QtMediaServices::Mood, "mood" },
        { QtMediaServices::TrackNumber, "trackNumber" },
        { QtMediaServices::TrackCount, "trackCount" },

        { QtMediaServices::CoverArtUrlSmall, "coverArtUrlSmall" },
        { QtMediaServices::CoverArtUrlLarge, "coverArtUrlLarge" },

        // Image/Video
        { QtMediaServices::Resolution, "resolution" },
        { QtMediaServices::PixelAspectRatio, "pixelAspectRatio" },

        // Video
        { QtMediaServices::VideoFrameRate, "videoFrameRate" },
        { QtMediaServices::VideoBitRate, "videoBitRate" },
        { QtMediaServices::VideoCodec, "videoCodec" },

        { QtMediaServices::PosterUrl, "posterUrl" },

        // Movie
        { QtMediaServices::ChapterNumber, "chapterNumber" },
        { QtMediaServices::Director, "director" },
        { QtMediaServices::LeadPerformer, "leadPerformer" },
        { QtMediaServices::Writer, "writer" },

        // Photos
        { QtMediaServices::CameraManufacturer, "cameraManufacturer" },
        { QtMediaServices::CameraModel, "cameraModel" },
        { QtMediaServices::Event, "event" },
        { QtMediaServices::Subject, "subject" },
        { QtMediaServices::Orientation, "orientation" },
        { QtMediaServices::ExposureTime, "exposureTime" },
        { QtMediaServices::FNumber, "fNumber" },
        { QtMediaServices::ExposureProgram, "exposureProgram" },
        { QtMediaServices::ISOSpeedRatings, "isoSpeedRatings" },
        { QtMediaServices::ExposureBiasValue, "exposureBiasValue" },
        { QtMediaServices::DateTimeOriginal, "dateTimeOriginal" },
        { QtMediaServices::DateTimeDigitized, "dateTimeDigitized" },
        { QtMediaServices::SubjectDistance, "subjectDistance" },
        { QtMediaServices::MeteringMode, "meteringMode" },
        { QtMediaServices::LightSource, "lightSource" },
        { QtMediaServices::Flash, "flash" },
        { QtMediaServices::FocalLength, "focalLength" },
        { QtMediaServices::ExposureMode, "exposureMode" },
        { QtMediaServices::WhiteBalance, "whiteBalance" },
        { QtMediaServices::DigitalZoomRatio, "digitalZoomRatio" },
        { QtMediaServices::FocalLengthIn35mmFilm, "focalLengthIn35mmFilm" },
        { QtMediaServices::SceneCaptureType, "sceneCaptureType" },
        { QtMediaServices::GainControl, "gainControl" },
        { QtMediaServices::Contrast, "contrast" },
        { QtMediaServices::Saturation, "saturation" },
        { QtMediaServices::Sharpness, "sharpness" },
        { QtMediaServices::DeviceSettingDescription, "deviceSettingDescription" }
    };

    class QMetaDataControlObject : public QObject
    {
    public:
        inline QObjectData *data() { return d_ptr.data(); }
    };
}

QMetaDataControlMetaObject::QMetaDataControlMetaObject(QMetaDataControl *control, QObject *object)
    : m_control(control)
    , m_object(object)
    , m_string(0)
    , m_data(0)
    , m_propertyOffset(0)
    , m_signalOffset(0)
{
    const QMetaObject *superClass = m_object->metaObject();

    const int propertyCount =  sizeof(qt_metaDataKeys) / sizeof(MetaDataKey);
    const int dataSize = sizeof(uint)
            * (13                   // QMetaObjectPrivate members.
            + 5                     // 5 members per signal.
            + 4 * propertyCount     // 3 members per property + 1 notify signal per property.
            + 1);                   // Terminating value.

    m_data = reinterpret_cast<uint *>(qMalloc(dataSize));

    QMetaObjectPrivate *pMeta = reinterpret_cast<QMetaObjectPrivate *>(m_data);

    pMeta->revision = 3;
    pMeta->className = 0;
    pMeta->classInfoCount = 0;
    pMeta->classInfoData = 0;
    pMeta->methodCount = 1;
    pMeta->methodData = 13;
    pMeta->propertyCount = propertyCount;
    pMeta->propertyData = 18;
    pMeta->enumeratorCount = 0;
    pMeta->enumeratorData = 0;
    pMeta->constructorCount = 0;
    pMeta->constructorData = 0;
    pMeta->flags = 0x01;    // Dynamic meta object flag.

    const int classNameSize = qstrlen(superClass->className()) + 1;

    int stringIndex = classNameSize + 1;

    // __metaDataChanged() signal.
    static const char *changeSignal = "__metaDataChanged()";
    const int changeSignalSize = qstrlen(changeSignal) + 1;

    m_data[13] = stringIndex;                             // Signature.
    m_data[14] = classNameSize;                           // Parameters.
    m_data[15] = classNameSize;                           // Type.
    m_data[16] = classNameSize;                           // Tag.
    m_data[17] = MethodSignal | AccessProtected;          // Flags.

    stringIndex += changeSignalSize;

    const char *qvariantName = "QVariant";
    const int qvariantSize = qstrlen(qvariantName) + 1;
    const int qvariantIndex = stringIndex;

    stringIndex += qvariantSize;

    // Properties.
    for (int i = 0; i < propertyCount; ++i) {       
        m_data[18 + 3 * i] = stringIndex;                                       // Name.
        m_data[19 + 3 * i] = qvariantIndex;                                     // Type.
        m_data[20 + 3 * i] 
                = Readable | Writable | Notify | Dynamic | (0xffffffff << 24);  // Flags.
        m_data[18 + propertyCount * 3 + i] = 0;                                 // Notify signal.

        stringIndex += qstrlen(qt_metaDataKeys[i].name) + 1;
    }

    // Terminating value.
    m_data[18 + propertyCount * 4] = 0;

    // Build string.
    m_string = reinterpret_cast<char *>(qMalloc(stringIndex + 1));

    // Class name.
    qMemCopy(m_string, superClass->className(), classNameSize);

    stringIndex = classNameSize;

    // Null m_string.
    m_string[stringIndex] = '\0';
    stringIndex += 1;

    // __metaDataChanged() signal.
    qMemCopy(m_string + stringIndex, changeSignal, changeSignalSize);
    stringIndex += changeSignalSize;

    qMemCopy(m_string + stringIndex, qvariantName, qvariantSize);
    stringIndex += qvariantSize;

    // Properties.
    for (int i = 0; i < propertyCount; ++i) {
        const int propertyNameSize = qstrlen(qt_metaDataKeys[i].name) + 1;

        qMemCopy(m_string + stringIndex, qt_metaDataKeys[i].name, propertyNameSize);
        stringIndex += propertyNameSize;
    }

    // Terminating character.
    m_string[stringIndex] = '\0';

    d.superdata = superClass;
    d.stringdata = m_string;
    d.data = m_data;
    d.extradata = 0;

    static_cast<QMetaDataControlObject *>(m_object)->data()->metaObject = this;

    m_propertyOffset = propertyOffset();
    m_signalOffset = methodOffset();
}

QMetaDataControlMetaObject::~QMetaDataControlMetaObject()
{
    static_cast<QMetaDataControlObject *>(m_object)->data()->metaObject = 0;

    qFree(m_data);
    qFree(m_string);
}

int QMetaDataControlMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (c == QMetaObject::ReadProperty && id >= m_propertyOffset) {
        int propId = id - m_propertyOffset;

        *reinterpret_cast<QVariant *>(a[0]) = m_control->metaData(qt_metaDataKeys[propId].key);

        return -1;
    } else if (c == QMetaObject::WriteProperty && id >= m_propertyOffset) {
        int propId = id - m_propertyOffset;

        m_control->setMetaData(qt_metaDataKeys[propId].key, *reinterpret_cast<QVariant *>(a[0]));

        return -1;
    } else {
        return m_object->qt_metacall(c, id, a);
    }
}

int QMetaDataControlMetaObject::createProperty(const char *, const char *)
{
    return -1;
}

void QMetaDataControlMetaObject::metaDataChanged()
{
    activate(m_object, m_signalOffset, 0);
}

QT_END_NAMESPACE
