/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QCoreApplication>
#include "qaudiodeviceinfo_symbian_p.h"
#include "qaudio_symbian_p.h"

QT_BEGIN_NAMESPACE

QAudioDeviceInfoInternal::QAudioDeviceInfoInternal(QByteArray device,
                                               QAudio::Mode mode)
    :   m_deviceName(QLatin1String(device))
    ,   m_mode(mode)
    ,   m_updated(false)
{

}

QAudioDeviceInfoInternal::~QAudioDeviceInfoInternal()
{

}

QAudioFormat QAudioDeviceInfoInternal::preferredFormat() const
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
        format = QAudioFormat();
        format.setCodec(QLatin1String("audio/pcm"));
        if (m_capabilities.contains(format.codec())) {
            const Capabilities &codecCaps = m_capabilities[format.codec()];
            if (codecCaps.m_frequencies.size())
                format.setFrequency(codecCaps.m_frequencies[0]);
            if (codecCaps.m_channels.size())
                format.setChannels(codecCaps.m_channels[0]);
            if (codecCaps.m_sampleSizes.size())
                format.setSampleSize(codecCaps.m_sampleSizes[0]);
            if (codecCaps.m_byteOrders.size())
                format.setByteOrder(codecCaps.m_byteOrders[0]);
            if (codecCaps.m_sampleTypes.size())
                format.setSampleType(codecCaps.m_sampleTypes[0]);
        }
    }

    return format;
}

bool QAudioDeviceInfoInternal::isFormatSupported(
                                 const QAudioFormat &format) const
{
    getSupportedFormats();
    bool supported = false;
    if (m_capabilities.contains(format.codec())) {
        const Capabilities &codecCaps = m_capabilities[format.codec()];
        supported = codecCaps.m_frequencies.contains(format.frequency())
                &&  codecCaps.m_channels.contains(format.channels())
                &&  codecCaps.m_sampleSizes.contains(format.sampleSize())
                &&  codecCaps.m_byteOrders.contains(format.byteOrder())
                &&  codecCaps.m_sampleTypes.contains(format.sampleType());
    }
    return supported;
}

QAudioFormat QAudioDeviceInfoInternal::nearestFormat(const QAudioFormat &format) const
{
    if (isFormatSupported(format))
        return format;
    else
        return preferredFormat();
}

QString QAudioDeviceInfoInternal::deviceName() const
{
    return m_deviceName;
}

QStringList QAudioDeviceInfoInternal::codecList()
{
    getSupportedFormats();
    return m_capabilities.keys();
}

QList<int> QAudioDeviceInfoInternal::frequencyList()
{
    getSupportedFormats();
    return m_unionCapabilities.m_frequencies;
}

QList<int> QAudioDeviceInfoInternal::channelsList()
{
    getSupportedFormats();
    return m_unionCapabilities.m_channels;
}

QList<int> QAudioDeviceInfoInternal::sampleSizeList()
{
    getSupportedFormats();
    return m_unionCapabilities.m_sampleSizes;
}

QList<QAudioFormat::Endian> QAudioDeviceInfoInternal::byteOrderList()
{
    getSupportedFormats();
    return m_unionCapabilities.m_byteOrders;
}

QList<QAudioFormat::SampleType> QAudioDeviceInfoInternal::sampleTypeList()
{
    getSupportedFormats();
    return m_unionCapabilities.m_sampleTypes;
}

QByteArray QAudioDeviceInfoInternal::defaultInputDevice()
{
    return QByteArray("default");
}

QByteArray QAudioDeviceInfoInternal::defaultOutputDevice()
{
    return QByteArray("default");
}

QList<QByteArray> QAudioDeviceInfoInternal::availableDevices(QAudio::Mode)
{
    QList<QByteArray> result;
    result += QByteArray("default");
    return result;
}

void QAudioDeviceInfoInternal::devsoundInitializeComplete(int err)
{
    m_intializationResult = err;
    m_initializing = false;
}

// Helper function
template<typename T>
void appendUnique(QList<T> &left, const QList<T> &right)
{
    foreach (const T &value, right)
        if (!left.contains(value))
            left += value;
}

void QAudioDeviceInfoInternal::getSupportedFormats() const
{
    if (!m_updated) {
        QScopedPointer<SymbianAudio::DevSoundWrapper> devsound(new SymbianAudio::DevSoundWrapper(m_mode));
        connect(devsound.data(), SIGNAL(initializeComplete(int)),
                this, SLOT(devsoundInitializeComplete(int)));

        foreach (const QString& codec, devsound->supportedCodecs()) {
            m_initializing = true;
            devsound->initialize(codec);
            while (m_initializing)
                QCoreApplication::instance()->processEvents(QEventLoop::WaitForMoreEvents);
            if (KErrNone == m_intializationResult) {
                m_capabilities[codec].m_frequencies = devsound->supportedFrequencies();
                appendUnique(m_unionCapabilities.m_frequencies, devsound->supportedFrequencies());

                m_capabilities[codec].m_channels = devsound->supportedChannels();
                appendUnique(m_unionCapabilities.m_channels, devsound->supportedChannels());

                m_capabilities[codec].m_sampleSizes = devsound->supportedSampleSizes();
                appendUnique(m_unionCapabilities.m_sampleSizes, devsound->supportedSampleSizes());

                m_capabilities[codec].m_byteOrders = devsound->supportedByteOrders();
                appendUnique(m_unionCapabilities.m_byteOrders, devsound->supportedByteOrders());

                m_capabilities[codec].m_sampleTypes = devsound->supportedSampleTypes();
                appendUnique(m_unionCapabilities.m_sampleTypes, devsound->supportedSampleTypes());
            }
        }

        m_updated = true;
    }
}

QT_END_NAMESPACE
