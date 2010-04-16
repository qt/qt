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

#include "qt7backend.h"
#include "qt7playermetadata.h"
#include "qt7playersession.h"
#include <QtCore/qvarlengtharray.h>

#import <QTKit/QTMovie.h>

#ifdef QUICKTIME_C_API_AVAILABLE
    #include <QuickTime/QuickTime.h>
    #undef check // avoid name clash;
#endif

QT_BEGIN_NAMESPACE

QT7PlayerMetaDataControl::QT7PlayerMetaDataControl(QT7PlayerSession *session, QObject *parent)
    :QMetaDataControl(parent), m_session(session)
{
}

QT7PlayerMetaDataControl::~QT7PlayerMetaDataControl()
{
}

bool QT7PlayerMetaDataControl::isMetaDataAvailable() const
{
    return !m_tags.isEmpty();
}

bool QT7PlayerMetaDataControl::isWritable() const
{
    return false;
}

QVariant QT7PlayerMetaDataControl::metaData(QtMediaServices::MetaData key) const
{
    return m_tags.value(key);
}

void QT7PlayerMetaDataControl::setMetaData(QtMediaServices::MetaData key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QList<QtMediaServices::MetaData> QT7PlayerMetaDataControl::availableMetaData() const
{
    return m_tags.keys();
}

QVariant QT7PlayerMetaDataControl::extendedMetaData(const QString &key) const
{
    Q_UNUSED(key);
    return QVariant();
}

void QT7PlayerMetaDataControl::setExtendedMetaData(const QString &key, QVariant const &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

QStringList QT7PlayerMetaDataControl::availableExtendedMetaData() const
{
    return QStringList();
}

#ifdef QUICKTIME_C_API_AVAILABLE

static QString stripCopyRightSymbol(const QString &key)
{
    return key.right(key.length()-1);
}

static QString convertQuickTimeKeyToUserKey(const QString &key)
{
    if (key == QLatin1String("com.apple.quicktime.displayname"))
        return QLatin1String("nam");
    else if (key == QLatin1String("com.apple.quicktime.album"))
        return QLatin1String("alb");
    else if (key == QLatin1String("com.apple.quicktime.artist"))
        return QLatin1String("ART");
    else
        return QLatin1String("???");
}

static OSStatus readMetaValue(QTMetaDataRef metaDataRef, QTMetaDataItem item, QTPropertyClass propClass,
                              QTPropertyID id, QTPropertyValuePtr *value, ByteCount *size)
{
    QTPropertyValueType type;
    ByteCount propSize;
    UInt32 propFlags;
    OSStatus err = QTMetaDataGetItemPropertyInfo(metaDataRef, item, propClass, id, &type, &propSize, &propFlags);

    if (err == noErr) {
        *value = malloc(propSize);
        if (*value != 0) {
            err = QTMetaDataGetItemProperty(metaDataRef, item, propClass, id, propSize, *value, size);

            if (err == noErr && (type == 'code' || type == 'itsk' || type == 'itlk')) {
                // convert from native endian to big endian
                OSTypePtr pType = (OSTypePtr)*value;
                *pType = EndianU32_NtoB(*pType);
            }
        }
        else
            return -1;
    }

    return err;
}

static UInt32 getMetaType(QTMetaDataRef metaDataRef, QTMetaDataItem item)
{
    QTPropertyValuePtr value = 0;
    ByteCount ignore = 0;
    OSStatus err = readMetaValue(
            metaDataRef, item, kPropertyClass_MetaDataItem, kQTMetaDataItemPropertyID_DataType, &value, &ignore);

    if (err == noErr) {
        UInt32 type = *((UInt32 *) value);
        if (value)
            free(value);
        return type;
    }

    return 0;
}

static QString cFStringToQString(CFStringRef str)
{
    if(!str)
        return QString();
    CFIndex length = CFStringGetLength(str);
    const UniChar *chars = CFStringGetCharactersPtr(str);
    if (chars)
        return QString(reinterpret_cast<const QChar *>(chars), length);

    QVarLengthArray<UniChar> buffer(length);
    CFStringGetCharacters(str, CFRangeMake(0, length), buffer.data());
    return QString(reinterpret_cast<const QChar *>(buffer.constData()), length);
}


static QString getMetaValue(QTMetaDataRef metaDataRef, QTMetaDataItem item, SInt32 id)
{
    QTPropertyValuePtr value = 0;
    ByteCount size = 0;
    OSStatus err = readMetaValue(metaDataRef, item, kPropertyClass_MetaDataItem, id, &value, &size);
    QString string;

    if (err == noErr) {
        UInt32 dataType = getMetaType(metaDataRef, item);
        switch (dataType){
        case kQTMetaDataTypeUTF8:
        case kQTMetaDataTypeMacEncodedText:
            string = cFStringToQString(CFStringCreateWithBytes(0, (UInt8*)value, size, kCFStringEncodingUTF8, false));
            break;
        case kQTMetaDataTypeUTF16BE:
            string = cFStringToQString(CFStringCreateWithBytes(0, (UInt8*)value, size, kCFStringEncodingUTF16BE, false));
            break;
        default:
            break;
        }

        if (value)
            free(value);
    }

    return string;
}


static void readFormattedData(QTMetaDataRef metaDataRef, OSType format, QMultiMap<QString, QString> &result)
{
    QTMetaDataItem item = kQTMetaDataItemUninitialized;
    OSStatus err = QTMetaDataGetNextItem(metaDataRef, format, item, kQTMetaDataKeyFormatWildcard, 0, 0, &item);
    while (err == noErr){
        QString key = getMetaValue(metaDataRef, item, kQTMetaDataItemPropertyID_Key);
        if (format == kQTMetaDataStorageFormatQuickTime)
            key = convertQuickTimeKeyToUserKey(key);
        else
            key = stripCopyRightSymbol(key);

        if (!result.contains(key)){
            QString val = getMetaValue(metaDataRef, item, kQTMetaDataItemPropertyID_Value);
            result.insert(key, val);
        }
        err = QTMetaDataGetNextItem(metaDataRef, format, item, kQTMetaDataKeyFormatWildcard, 0, 0, &item);
    }
}
#endif


void QT7PlayerMetaDataControl::updateTags()
{
    bool wasEmpty = m_tags.isEmpty();
    m_tags.clear();

    QTMovie *movie = (QTMovie*)m_session->movie();

    if (movie) {
        QMultiMap<QString, QString> metaMap;

#ifdef QUICKTIME_C_API_AVAILABLE
        QTMetaDataRef metaDataRef;
        OSStatus err = QTCopyMovieMetaData([movie quickTimeMovie], &metaDataRef);
        if (err == noErr) {
            readFormattedData(metaDataRef, kQTMetaDataStorageFormatUserData, metaMap);
            readFormattedData(metaDataRef, kQTMetaDataStorageFormatQuickTime, metaMap);
            readFormattedData(metaDataRef, kQTMetaDataStorageFormatiTunes, metaMap);
        }
#else
        AutoReleasePool pool;
        NSString *name = [movie attributeForKey:@"QTMovieDisplayNameAttribute"];
        metaMap.insert(QLatin1String("nam"), QString::fromUtf8([name UTF8String]));
#endif // QUICKTIME_C_API_AVAILABLE

        m_tags.insert(QtMediaServices::AlbumArtist, metaMap.value(QLatin1String("ART")));
        m_tags.insert(QtMediaServices::AlbumTitle, metaMap.value(QLatin1String("alb")));
        m_tags.insert(QtMediaServices::Title, metaMap.value(QLatin1String("nam")));
        m_tags.insert(QtMediaServices::Date, metaMap.value(QLatin1String("day")));
        m_tags.insert(QtMediaServices::Genre, metaMap.value(QLatin1String("gnre")));
        m_tags.insert(QtMediaServices::TrackNumber, metaMap.value(QLatin1String("trk")));
        m_tags.insert(QtMediaServices::Description, metaMap.value(QLatin1String("des")));
    }

    if (!wasEmpty || !m_tags.isEmpty())
        emit metaDataChanged();
}

QT_END_NAMESPACE

#include "moc_qt7playermetadata.cpp"
