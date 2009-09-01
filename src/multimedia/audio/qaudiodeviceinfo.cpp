/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights.  These rights are described in the Nokia Qt LGPL
** Exception version 1.1, included in the file LGPL_EXCEPTION.txt in this
** package.
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

#include "qaudiodevicefactory_p.h"
#include <QtMultimedia/qaudioengine.h>
#include <QtMultimedia/qaudiodeviceinfo.h>


QT_BEGIN_NAMESPACE

/*!
    \class QAudioDeviceInfo
    \brief The QAudioDeviceInfo class provides an interface to query audio devices and their functionality.

    \inmodule QtMultimedia
    \ingroup multimedia
    \since 4.6

    QAudioDeviceInfo lets you query for audio devices--such as sound
    cards and USB headsets--that are currently available on the system.
    The audio devices available are dependent on the platform or audio plugins installed.

    You can also query each device for the formats it supports. A
    format in this context is a set consisting of a specific byte
    order, channel, codec, frequency, sample rate, and sample type.  A
    format is represented by the QAudioFormat class.

    The values supported by the the device for each of these
    parameters can be fetched with
    supportedByteOrders(), supportedChannels(), supportedCodecs(),
    supportedFrequencies(), supportedSampleSizes(), and
    supportedSampleTypes(). The combinations supported are dependent on the platform,
    audio plugins installed and the audio device capabilities. If you need a specific format, you can check if
    the device supports it with isFormatSupported(), or fetch a
    supported format that is as close as possible to the format with
    nearestFormat().

    A QAudioDeviceInfo is constructed with a QAudioDeviceId, which is
    an identifier for a physical device. It is used by Qt to construct
    classes that communicate with the device--such as
    QAudioDeviceInfo, QAudioInput, and QAudioOutput. The static
    functions defaultInputDevice(), defaultOutputDevice(), and
    deviceList() let you get a hold of the ids for all available
    devices. You fetch ids based on whether you will use the device
    for input or output; this is specified by the QAudio::Mode enum.
    The QAudioDeviceId returned are only valid for the QAudio::Mode.

    For instance:

    \code
    foreach(QAudioDeviceId audioId, QAudioDeviceInfo::deviceList(QAudio::AudioOutput)) {
        QAudioDeviceInfo info(audioId);
        qDebug() << "Device name: " << info.deviceName();
    }
    \endcode

    In this code sample, we loop through all devices that are able to output
    sound, i.e., play an audio stream in a supported format. For each device we
    find, we simply print the deviceName().

    \sa QAudioOutput, QAudioInput, QAudioDeviceId
*/

/*!
    Construct a new audio device info and attach it to \a parent.
    Using the audio device with the specified \a id.
*/

QAudioDeviceInfo::QAudioDeviceInfo(const QAudioDeviceId &id, QObject *parent):
    QObject(parent)
{
    d = QAudioDeviceFactory::audioDeviceInfo(id);
}

/*!
    Destroy this audio device info.
*/

QAudioDeviceInfo::~QAudioDeviceInfo()
{
    delete d;
}

/*!
    Returns human readable name of audio device.

    Device names vary depending on platform/audio plugin being used.

    They are a unique string identifiers for the audio device.

    eg. default, Intel, U0x46d0x9a4
*/

QString QAudioDeviceInfo::deviceName() const
{
    return d->deviceName();
}

/*!
    Returns true if \a settings are supported by the audio device of this QAudioDeviceInfo.
*/

bool QAudioDeviceInfo::isFormatSupported(const QAudioFormat &settings) const
{
    return d->isFormatSupported(settings);
}

/*!
    Returns QAudioFormat of default settings.

    These settings are provided by the platform/audio plugin being used.

    They also are dependent on the QAudio::Mode being used.

    A typical audio system would provide something like:
    \list
    \o Input settings: 8000Hz mono 8 bit.
    \o Output settings: 44100Hz stereo 16 bit little endian.
    \endlist
*/

QAudioFormat QAudioDeviceInfo::preferredFormat() const
{
    return d->preferredFormat();
}

/*!
    Returns closest QAudioFormat to \a settings that system audio supports.

    These settings are provided by the platform/audio plugin being used.

    They also are dependent on the QAudio::Mode being used.
*/

QAudioFormat QAudioDeviceInfo::nearestFormat(const QAudioFormat &settings) const
{
    return d->nearestFormat(settings);
}

/*!
    Returns a list of supported codecs.

    All platform and plugin implementations should provide support for:

    "audio/pcm" - Linear PCM

    For writing plugins to support additional codecs refer to:

    http://www.iana.org/assignments/media-types/audio/
*/

QStringList QAudioDeviceInfo::supportedCodecs() const
{
    return d->codecList();
}

/*!
    Returns a list of supported frequencies.
*/

QList<int> QAudioDeviceInfo::supportedFrequencies() const
{
    return d->frequencyList();
}

/*!
    Returns a list of supported channels.
*/

QList<int> QAudioDeviceInfo::supportedChannels() const
{
    return d->channelsList();
}

/*!
    Returns a list of supported sample sizes.
*/

QList<int> QAudioDeviceInfo::supportedSampleSizes() const
{
    return d->sampleSizeList();
}

/*!
    Returns a list of supported byte orders.
*/

QList<QAudioFormat::Endian> QAudioDeviceInfo::supportedByteOrders() const
{
    return d->byteOrderList();
}

/*!
    Returns a list of supported sample types.
*/

QList<QAudioFormat::SampleType> QAudioDeviceInfo::supportedSampleTypes() const
{
    return d->sampleTypeList();
}

/*!
    Returns the name of the default input audio device.
    All platform and audio plugin implementations provide a default audio device to use.
*/

QAudioDeviceId QAudioDeviceInfo::defaultInputDevice()
{
    return QAudioDeviceFactory::defaultInputDevice();
}

/*!
    Returns the name of the default output audio device.
    All platform and audio plugin implementations provide a default audio device to use.
*/

QAudioDeviceId QAudioDeviceInfo::defaultOutputDevice()
{
    return QAudioDeviceFactory::defaultOutputDevice();
}

/*!
    Returns a list of audio devices that support \a mode.
*/

QList<QAudioDeviceId> QAudioDeviceInfo::deviceList(QAudio::Mode mode)
{
    return QAudioDeviceFactory::deviceList(mode);
}

QT_END_NAMESPACE

