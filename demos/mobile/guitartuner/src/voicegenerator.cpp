/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "voicegenerator.h"

const int BufferSizeMilliseconds = 100;

VoiceGenerator::VoiceGenerator(const QAudioFormat &format,
                               qreal frequency, qreal amplitude,
                               QObject *parent) :
    QIODevice(parent),
    m_format(format),
    m_amplitude(0.5)
{
    Q_ASSERT(m_format.sampleSize() % 8 == 0);
    int sampleBytes = m_format.channels() * (m_format.sampleSize() / 8);
    // + 1 to round up, just to be sure that all samples fit.
    qint64 samplesInBuffer = m_format.sampleRate()
                             * BufferSizeMilliseconds / 1000 + 1;
    qint64 length = samplesInBuffer * sampleBytes;
    m_buffer.resize(length);
    m_max_position = 0;
    m_position = 0;
    m_amplitude = amplitude;
    setFrequency(frequency);
}

VoiceGenerator::~VoiceGenerator()
{
}

/**
  * Opens the parent QIODevice.
  */
void VoiceGenerator::start()
{
    open(QIODevice::ReadOnly);
}

/**
  * Closes the parent QIODevice. Resets the m_position to zero.
  */
void VoiceGenerator::stop()
{
    close();
    m_position = 0;
}

/**
  * Sets the frequency to new frequency.
  */
void VoiceGenerator::setFrequency(qreal frequency)
{
    Q_ASSERT(1 / frequency < BufferSizeMilliseconds);
    this->m_frequency = frequency;
    refreshData();
}

/**
  * Sets the amplitude for the voice.
  */
void VoiceGenerator::setAmplitude(qreal amplitude)
{
    Q_ASSERT(amplitude >= 0);
    m_amplitude = amplitude;
    refreshData();
}

/**
  * Returns the current frequency.
  */
qreal VoiceGenerator::frequency()
{
    return m_frequency;
}

/**
  * Generates voice data corresponding a sine voice with target frequency.
  * The number of data generated is calculated
  * and stored to m_max_position.
  */
void VoiceGenerator::refreshData()
{
    const int channelBytes = m_format.sampleSize() / 8;
    const int sampleSize = m_format.channels() * channelBytes;
    const qint64 voiceOscillationsInBuffer = BufferSizeMilliseconds
                                         * m_frequency / 1000;
    const qint64 voiceSamplesInBuffer = voiceOscillationsInBuffer
                                   * m_format.sampleRate() / m_frequency;
    m_max_position = voiceSamplesInBuffer * sampleSize;
    qint64 dataGenerationLength = m_buffer.size();


    Q_ASSERT(m_max_position % (sampleSize) == 0);
    Q_ASSERT(dataGenerationLength <= m_buffer.size());

    short *t = (short*)m_buffer.data();


/*
    int te ;
    static float fpos = 0.0f;
    //dataGenerationLength>>=1;           // in words
    for (int f=0; f<dataGenerationLength; f++) {
      te = (short)((sinf(fpos))* (65536.0f/2.0f));
      fpos += m_frequency/2000.0f;
      t[f]= te;
    };

    m_amplitude = 1.0f; */
    uchar *ptr = reinterpret_cast<uchar *>(m_buffer.data());
    int sampleIndex = 0;
    while (dataGenerationLength > 0) {
        qreal realValue = 0;
        if (sampleIndex < voiceSamplesInBuffer) {
            realValue = m_amplitude
                       *qSin(2.0f * M_PI * m_frequency
                             * qreal(sampleIndex % m_format.sampleRate())
                               / m_format.sampleRate());
        }
        for (int i=0; i<m_format.channels(); ++i) {
            setValue(ptr, realValue);
            ptr += channelBytes;
            dataGenerationLength -= channelBytes;
        }
        ++sampleIndex;
    }
}

/**
  * Stores the realValue into bytes pointed by ptr as an int value.
  * Align-safe.
  */
void VoiceGenerator::setValue(uchar *ptr, qreal realValue) {
    if (m_format.sampleSize() == 8)
    {
        quint8 value = 0;
        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            value = static_cast<quint8>(
                        qRound((1.0 + realValue) / 2
                               * M_MAX_AMPLITUDE_8BIT_UNSIGNED));
        } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            value = static_cast<qint8>(
                        qRound(realValue
                               * M_MAX_AMPLITUDE_8BIT_SIGNED));
        }
        *reinterpret_cast<quint8*>(ptr) = value;
    } else if (m_format.sampleSize() == 16) {
        quint16 value = 0;
        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            value = static_cast<quint16>(
                        qRound((1.0 + realValue) / 2
                               * M_MAX_AMPLITUDE_16BIT_UNSIGNED));
        } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            value = static_cast<qint16>(
                        qRound(realValue
                               * M_MAX_AMPLITUDE_16BIT_SIGNED));
        }
        if (m_format.byteOrder() == QAudioFormat::LittleEndian)
            qToLittleEndian<qint16>(value, ptr);
        else
            qToBigEndian<qint16>(value, ptr);
    }
}

qint64 VoiceGenerator::bytesAvailable() const
{
    return m_max_position + QIODevice::bytesAvailable();
}

/**
  * Called by the QIODevice. Puts maxlen amount of voice
  * samples into the data array.
  */
qint64 VoiceGenerator::readData(char *data, qint64 maxlen)
{
    qint64 total = 0;
    qint64 chunk = 0;
    while (total < maxlen) {
        if (maxlen - total >= m_max_position - m_position) {
            // the needed buffer is longer than the currently
            // available buffer from m_position to the m_max_position
            chunk = m_max_position - m_position;
            memcpy(data, m_buffer.constData() + m_position, chunk);
            m_position = 0;
        }
        else {
            // we can copy the needed data directly, and the loop will end
            chunk = maxlen - total;
            memcpy(data, m_buffer.constData() + m_position, chunk);
            m_position = (m_position + chunk) % m_max_position;
        }
        data += chunk;
        total += chunk;
    }
    return total;
}

/**
  * Empty implementation for writeData, since no data is provided
  * for the VoiceGenerator class.
  */
qint64 VoiceGenerator::writeData(const char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}
