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

#include <dshow.h>
#include <initguid.h>
#include <qnetwork.h>

#include "directshowmetadatacontrol.h"

#include "directshowplayerservice.h"

#include <QtCore/qcoreapplication.h>


QT_BEGIN_NAMESPACE

#ifndef QT_NO_WMSDK
namespace
{
    struct QWMMetaDataKeyLookup
    {
        QtMediaservices::MetaData key;
        const wchar_t *token;
    };
}

static const QWMMetaDataKeyLookup qt_wmMetaDataKeys[] =
{
    { QtMediaservices::Title, L"Title" },
    { QtMediaservices::SubTitle, L"WM/SubTitle" },
    { QtMediaservices::Author, L"Author" },
    { QtMediaservices::Comment, L"Comment" },
    { QtMediaservices::Description, L"Description" },
    { QtMediaservices::Category, L"WM/Category" },
    { QtMediaservices::Genre, L"WM/Genre" },
    //{ QtMediaservices::Date, 0 },
    { QtMediaservices::Year, L"WM/Year" },
    { QtMediaservices::UserRating, L"UserRating" },
    //{ QtMediaservices::MetaDatawords, 0 },
    { QtMediaservices::Language, L"Language" },
    { QtMediaservices::Publisher, L"WM/Publisher" },
    { QtMediaservices::Copyright, L"Copyright" },
    { QtMediaservices::ParentalRating, L"ParentalRating" },
    { QtMediaservices::RatingOrganisation, L"RatingOrganisation" },

    // Media
    { QtMediaservices::Size, L"FileSize" },
    { QtMediaservices::MediaType, L"MediaType" },
    { QtMediaservices::Duration, L"Duration" },

    // Audio
    { QtMediaservices::AudioBitRate, L"AudioBitRate" },
    { QtMediaservices::AudioCodec, L"AudioCodec" },
    { QtMediaservices::ChannelCount, L"ChannelCount" },
    { QtMediaservices::SampleRate, L"Frequency" },

    // Music
    { QtMediaservices::AlbumTitle, L"WM/AlbumTitle" },
    { QtMediaservices::AlbumArtist, L"WM/AlbumArtist" },
    { QtMediaservices::ContributingArtist, L"Author" },
    { QtMediaservices::Composer, L"WM/Composer" },
    { QtMediaservices::Conductor, L"WM/Conductor" },
    { QtMediaservices::Lyrics, L"WM/Lyrics" },
    { QtMediaservices::Mood, L"WM/Mood" },
    { QtMediaservices::TrackNumber, L"WM/TrackNumber" },
    //{ QtMediaservices::TrackCount, 0 },
    //{ QtMediaservices::CoverArtUriSmall, 0 },
    //{ QtMediaservices::CoverArtUriLarge, 0 },

    // Image/Video
    //{ QtMediaservices::Resolution, 0 },
    //{ QtMediaservices::PixelAspectRatio, 0 },

    // Video
    //{ QtMediaservices::FrameRate, 0 },
    { QtMediaservices::VideoBitRate, L"VideoBitRate" },
    { QtMediaservices::VideoCodec, L"VideoCodec" },

    //{ QtMediaservices::PosterUri, 0 },

    // Movie
    { QtMediaservices::ChapterNumber, L"ChapterNumber" },
    { QtMediaservices::Director, L"WM/Director" },
    { QtMediaservices::LeadPerformer, L"LeadPerformer" },
    { QtMediaservices::Writer, L"WM/Writer" },

    // Photos
    { QtMediaservices::CameraManufacturer, L"CameraManufacturer" },
    { QtMediaservices::CameraModel, L"CameraModel" },
    { QtMediaservices::Event, L"Event" },
    { QtMediaservices::Subject, L"Subject" }
};

static QVariant getValue(IWMHeaderInfo *header, const wchar_t *key)
{
    WORD streamNumber = 0;
    WMT_ATTR_DATATYPE type = WMT_TYPE_DWORD;
    WORD size = 0;

    if (header->GetAttributeByName(&streamNumber, key, &type, 0, &size) == S_OK) {
        switch (type) {
        case WMT_TYPE_DWORD:
            if (size == sizeof(DWORD)) {
                DWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return int(word);
                }
            }
            break;
        case WMT_TYPE_STRING:
            {
                QString string;
                string.resize(size / 2 - 1);

                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(const_cast<ushort *>(string.utf16())),
                        &size) == S_OK) {
                    return string;
                }
            }
            break;
        case WMT_TYPE_BINARY:
            {
                QByteArray bytes;
                bytes.resize(size);
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(bytes.data()),
                        &size) == S_OK) {
                    return bytes;
                }
            }
            break;
        case WMT_TYPE_BOOL:
            if (size == sizeof(DWORD)) {
                DWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return bool(word);
                }
            }
            break;
        case WMT_TYPE_QWORD:
            if (size == sizeof(QWORD)) {
                QWORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key,
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return qint64(word);
                }
            }
            break;
        case WMT_TYPE_WORD:
            if (size == sizeof(WORD)){
                WORD word;
                if (header->GetAttributeByName(
                        &streamNumber,
                        key, 
                        &type,
                        reinterpret_cast<BYTE *>(&word),
                        &size) == S_OK) {
                    return short(word);
                }
            }
            break;
        case WMT_TYPE_GUID:
            if (size == 16) {
            }
            break;
        default:
            break;
        }
    }
    return QVariant();
}
#endif

DirectShowMetaDataControl::DirectShowMetaDataControl(QObject *parent)
    : QMetaDataControl(parent)
    , m_content(0)
#ifndef QT_NO_WMSDK
    , m_headerInfo(0)
#endif
{
}

DirectShowMetaDataControl::~DirectShowMetaDataControl()
{
}

bool DirectShowMetaDataControl::isWritable() const
{
    return false;
}

bool DirectShowMetaDataControl::isMetaDataAvailable() const
{
#ifndef QT_NO_WMSDK
    return m_content || m_headerInfo;
#else
    return m_content;
#endif
}

QVariant DirectShowMetaDataControl::metaData(QtMediaservices::MetaData key) const
{
    QVariant value;

#ifndef QT_NO_WMSDK
    if (m_headerInfo) {
        static const int  count = sizeof(qt_wmMetaDataKeys) / sizeof(QWMMetaDataKeyLookup);
        for (int i = 0; i < count; ++i) {
            if (qt_wmMetaDataKeys[i].key == key) {
                value = getValue(m_headerInfo, qt_wmMetaDataKeys[i].token);
                break;
            }
        }
    }  else if (m_content) {
#else
    if (m_content) {
#endif
        BSTR string = 0;

        switch (key) {
        case QtMediaservices::Author:
            m_content->get_AuthorName(&string);
            break;
        case QtMediaservices::Title:
            m_content->get_Title(&string);
            break;
        case QtMediaservices::ParentalRating:
            m_content->get_Rating(&string);
            break;
        case QtMediaservices::Description:
            m_content->get_Description(&string);
            break;
        case QtMediaservices::Copyright:
            m_content->get_Copyright(&string);
            break;
        default:
            break;
        }

        if (string) {
            value = QString::fromUtf16(reinterpret_cast<ushort *>(string), ::SysStringLen(string));

            ::SysFreeString(string);
        }
    }
    return value;
}

void DirectShowMetaDataControl::setMetaData(QtMediaservices::MetaData, const QVariant &)
{
}

QList<QtMediaservices::MetaData> DirectShowMetaDataControl::availableMetaData() const
{
    return QList<QtMediaservices::MetaData>();
}

QVariant DirectShowMetaDataControl::extendedMetaData(const QString &) const
{
    return QVariant();
}

void DirectShowMetaDataControl::setExtendedMetaData(const QString &, const QVariant &)
{
}

QStringList DirectShowMetaDataControl::availableExtendedMetaData() const
{
    return QStringList();
}

void DirectShowMetaDataControl::updateGraph(IFilterGraph2 *graph, IBaseFilter *source)
{
    if (m_content)
        m_content->Release();

    if (!graph || graph->QueryInterface(
            IID_IAMMediaContent, reinterpret_cast<void **>(&m_content)) != S_OK) {
        m_content = 0;
    }

#ifdef QT_NO_WMSDK
    Q_UNUSED(source);
#else
    if (m_headerInfo)
        m_headerInfo->Release();

    m_headerInfo = com_cast<IWMHeaderInfo>(source, IID_IWMHeaderInfo);
#endif
    // DirectShowMediaPlayerService holds a lock at this point so defer emitting signals to a later
    // time.
    QCoreApplication::postEvent(this, new QEvent(QEvent::Type(MetaDataChanged)));
}

void DirectShowMetaDataControl::customEvent(QEvent *event)
{
    if (event->type() == QEvent::Type(MetaDataChanged)) {
        event->accept();

        emit metaDataChanged();
#ifndef QT_NO_WMSDK
        emit metaDataAvailableChanged(m_content || m_headerInfo);
#else
        emit metaDataAvailableChanged(m_content);
#endif
    } else {
        QMetaDataControl::customEvent(event);
    }
}

QT_END_NAMESPACE
