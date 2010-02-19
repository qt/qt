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

#include "symbianaudiodeviceinfo.h"
#include "symbianaudioutils.h"

QT_BEGIN_NAMESPACE

SymbianAudioDeviceInfo::SymbianAudioDeviceInfo(QByteArray device,
                                               QAudio::Mode mode)
    :   m_deviceName(device)
    ,   m_mode(mode)
    ,   m_updated(false)
{
    QT_TRAP_THROWING(m_devsound.reset(CMMFDevSound::NewL()));
}

SymbianAudioDeviceInfo::~SymbianAudioDeviceInfo()
{

}

QAudioFormat SymbianAudioDeviceInfo::preferredFormat() const
{
    QAudioFormat format;
    switch (m_mode) {
    case QAudio::AudioOutput:
        format.setFrequency(44100);
        format.setChannels(2);
        format.setSampleSize(16);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);
        format.setCodec(QLatin1String("audio/pcm"));
        break;

    case QAudio::AudioInput:
        format.setFrequency(8000);
        format.setChannels(1);
        format.setSampleSize(16);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setSampleType(QAudioFormat::SignedInt);
        format.setCodec(QLatin1String("audio/pcm"));
        break;

    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid mode");
    }

    if (!isFormatSupported(format)) {
        if (m_frequencies.size())
            format.setFrequency(m_frequencies[0]);
        if (m_channels.size())
            format.setChannels(m_channels[0]);
        if (m_sampleSizes.size())
            format.setSampleSize(m_sampleSizes[0]);
        if (m_byteOrders.size())
            format.setByteOrder(m_byteOrders[0]);
        if (m_sampleTypes.size())
            format.setSampleType(m_sampleTypes[0]);
    }

    return format;
}

bool SymbianAudioDeviceInfo::isFormatSupported(
                                 const QAudioFormat &format) const
{
    getSupportedFormats();
    const bool supported =
            m_codecs.contains(format.codec())
        &&  m_frequencies.contains(format.frequency())
        &&  m_channels.contains(format.channels())
        &&  m_sampleSizes.contains(format.sampleSize())
        &&  m_byteOrders.contains(format.byteOrder())
        &&  m_sampleTypes.contains(format.sampleType());

    return supported;
}

QAudioFormat SymbianAudioDeviceInfo::nearestFormat(const QAudioFormat &format) const
{
    if (isFormatSupported(format))
        return format;
    else
        return preferredFormat();
}

QString SymbianAudioDeviceInfo::deviceName() const
{
    return m_deviceName;
}

QStringList SymbianAudioDeviceInfo::codecList()
{
    getSupportedFormats();
    return m_codecs;
}

QList<int> SymbianAudioDeviceInfo::frequencyList()
{
    getSupportedFormats();
    return m_frequencies;
}

QList<int> SymbianAudioDeviceInfo::channelsList()
{
    getSupportedFormats();
    return m_channels;
}

QList<int> SymbianAudioDeviceInfo::sampleSizeList()
{
    getSupportedFormats();
    return m_sampleSizes;
}

QList<QAudioFormat::Endian> SymbianAudioDeviceInfo::byteOrderList()
{
    getSupportedFormats();
    return m_byteOrders;
}

QList<QAudioFormat::SampleType> SymbianAudioDeviceInfo::sampleTypeList()
{
    getSupportedFormats();
    return m_sampleTypes;
}

QList<QByteArray> SymbianAudioDeviceInfo::deviceList(QAudio::Mode mode)
{
    Q_UNUSED(mode)
    QList<QByteArray> devices;
    devices.append("default");
    return devices;
}

void SymbianAudioDeviceInfo::getSupportedFormats() const
{
    if (!m_updated) {
        QScopedPointer<SymbianAudio::DevSoundCapabilities> caps(
            new SymbianAudio::DevSoundCapabilities(*m_devsound, m_mode));

        SymbianAudio::Utils::capabilitiesNativeToQt(*caps,
            m_frequencies, m_channels, m_sampleSizes,
            m_byteOrders, m_sampleTypes);

        m_codecs.append(QLatin1String("audio/pcm"));

        m_updated = true;
    }
}

QT_END_NAMESPACE
