/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "voiceanalyzer.h"

/**
  * Constant used to scale the cut-off density for the fft helper.
  */
const static float CutOffScaler = 0.05;

/**
  * Force the precision to be "1/PrecisionPerNote" notes
  * near the target frequency.
  */
const static int PrecisionPerNote = 4;

/**
  * TargetFrequencyParameter is a constant which implies the index at
  * which corresponds to the target frequency.
  * 0.5 * N * 1/TargetFrequencyParameter is (about) the index which
  * corresponds to the given target frequency.
  * Effectively TargetFrequencyParameter = 2^z, and the z*TargetFrequency
  * is the maximum frequency that can be noticed.
  */
const static int TargetFrequencyParameter = 4;


VoiceAnalyzer::VoiceAnalyzer(const QAudioFormat &format, QObject *parent):
    QIODevice(parent),
    m_format(format),
    m_frequency(0),
    m_position(0),
    m_fftHelper(new FastFourierTransformer(this))
{
    Q_ASSERT(qFuzzyCompare(M_SAMPLE_COUNT_MULTIPLIER,
                           float(2)/(M_TWELTH_ROOT_OF_2 -1.0)));
    m_totalSampleCount = qRound(qreal(PrecisionPerNote)
                                *TargetFrequencyParameter
                                *M_SAMPLE_COUNT_MULTIPLIER);
    m_samples.reserve(m_totalSampleCount);
    int i = 2;
    int j = 1;
    for (; i < TargetFrequencyParameter; i *= 2) {
        j++;
    }
    m_maximumVoiceDifference = j*12;

    setCutOffPercentage(CutOffScaler);
}

/**
  * Opens the parent QIODevice. Sets up the analysation parameters.
  */
void VoiceAnalyzer::start(qreal frequency)
{
    m_stepSize = (qreal) 1.0 * m_format.sampleRate()
                         / (TargetFrequencyParameter*2*frequency);
    m_frequency = frequency;
    open(QIODevice::WriteOnly);
}

/**
  * Closes the parent QIODevice, thus the voice is not analysed anymore.
  * Resets the m_samples QList.
  */
void VoiceAnalyzer::stop()
{
    m_samples.clear();
    m_samples.reserve(m_totalSampleCount);
    close();
}

/**
  * Called when data is obtained. Stores each m_stepSize sample
  * into a QList to be analysed.
  */
qint64 VoiceAnalyzer::writeData(const char *data, qint64 maxlen)
{
    const int channelBytes = m_format.sampleSize() / 8;
    int sampleSize = m_format.channels() * channelBytes;
    int m_stepSizeInBytes = m_stepSize*sampleSize;
    // assert that each sample fits fully into the data
    Q_ASSERT((m_position % sampleSize)==0);
    const uchar *ptr = reinterpret_cast<const uchar *>(data);
    while (m_position < maxlen) {
        if (m_samples.size() < m_totalSampleCount) {
            m_samples.append(getValueInt16(ptr+m_position));
        }
        else {
            analyzeVoice();
            m_samples.clear();
            m_samples.reserve(m_totalSampleCount);
            // fast forward position to the first position after maxlen or to the maxlen
            m_position += ((m_stepSizeInBytes - 1 + maxlen - m_position) /
                           m_stepSizeInBytes) * m_stepSizeInBytes;
            break;
        }
        m_position += m_stepSizeInBytes;
    }
    m_position -= maxlen;
    return maxlen;
}

/**
  * Interprets ptr as a pointer to int value and returns it.
  */
qint16 VoiceAnalyzer::getValueInt16(const uchar *ptr)
{
    qint16 realValue = 0;
    if (m_format.sampleSize() == 8)
    {
        const qint16 value = *reinterpret_cast<const quint8*>(ptr);
        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            realValue = value - M_MAX_AMPLITUDE_8BIT_SIGNED - 1;
        } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            realValue = value;
        }
    } else if (m_format.sampleSize() == 16) {
        qint16 value = 0;
        if (m_format.byteOrder() == QAudioFormat::LittleEndian)
            value = qFromLittleEndian<quint16>(ptr);
        else
            value = qFromBigEndian<quint16>(ptr);

        if (m_format.sampleType() == QAudioFormat::UnSignedInt) {
            realValue = value - M_MAX_AMPLITUDE_16BIT_SIGNED;
        } else if (m_format.sampleType() == QAudioFormat::SignedInt) {
            realValue = value;
        }
    }
    return realValue;
}

/**
  * Takes a number between 0 and 1, scales it with CutOffScaler,
  * multiplies it with maximum density, and then gives it
  * to the fft helper.
  */
void VoiceAnalyzer::setCutOffPercentage(qreal cutoff)
{
    cutoff = CutOffScaler*cutoff;
    if (m_format.sampleSize() == 8) {
        float t = cutoff*m_totalSampleCount*M_MAX_AMPLITUDE_8BIT_SIGNED;
        m_fftHelper->setCutOffForDensity(t);
    }
    else if (m_format.sampleSize() == 16) {
        float t = cutoff*m_totalSampleCount*M_MAX_AMPLITUDE_16BIT_SIGNED;
        m_fftHelper->setCutOffForDensity(t);
    }
}

/**
  * Returns the current target frequency.
  */
qreal VoiceAnalyzer::frequency()
{
    return m_frequency;
}

/**
  * Returns the maximum absolute value sent by
  * the voiceDifference() signal.
  */
int VoiceAnalyzer::getMaximumVoiceDifference()
{
    return m_maximumVoiceDifference;
}

/**
  * Returns the maximum precision per note
  * near the target frequency.
  */
int VoiceAnalyzer::getMaximumPrecisionPerNote()
{
    return PrecisionPerNote;
}

/**
  * Analyzes the voice frequency and emits appropriate signals.
  */
void VoiceAnalyzer::analyzeVoice()
{
    m_fftHelper->calculateFFT(m_samples);
    int index = m_fftHelper->getMaximumDensityIndex();

    // If index == -1
    if (index == -1) {
        // The voice is to be filtered away.
        // Emit the lowVoice signal and return.
        emit lowVoice();
        qDebug() << "low voice";
        return;
    }
    // Else, continue

    // Let the correctIndex to be
    // the nearest index corresponding to the correct frequency.
    qreal stepSizeInFrequency = (qreal)m_format.sampleRate()
            / (m_totalSampleCount * m_stepSize);
    qreal newFrequency = qreal(index) * stepSizeInFrequency;
    // Calculate the nearest index corresponding to the correct frequency.
    int correctIndex = qRound(m_frequency / stepSizeInFrequency);
    qreal value = 0;

    // If the obtained frequency is more than
    // log_2(TargetFrequencyParameter) octaves less than the m_frequency:

    // Note:
    // Instead of m_frequency/TargetFrequencyParameter > newFrequency,
    // the comparison is done without a div instructions by
    // m_frequency > newFrequency * TargetFrequencyParameter.

    if (m_frequency > newFrequency * TargetFrequencyParameter) {
        // Set the difference value to be -m_maximumVoiceDifference.
        qDebug() << "compare" << "low" << newFrequency << m_frequency - stepSizeInFrequency * correctIndex << (m_frequency - stepSizeInFrequency * correctIndex) / stepSizeInFrequency;
        value = -m_maximumVoiceDifference;
    }
    // Else, if the obtained frequency is more than
    // log_2(TargetFrequencyParameter) octaves more than the m_frequency:
    else if (m_frequency*TargetFrequencyParameter < newFrequency) {
        // Set the difference value to be m_maximumVoiceDifference.
        qDebug() << "compare" << "high" << newFrequency << m_frequency - stepSizeInFrequency * correctIndex << (m_frequency - stepSizeInFrequency * correctIndex) / stepSizeInFrequency;
        value = m_maximumVoiceDifference;
    }
    // Else:
    else {
        // Calculate the difference between the obtained and the correct
        // frequency in tones.
        // Use stepSizeInFrequency * correctIndex instead of
        // m_frequency so that the value is zero when there is correct
        // voice obtained. Set the difference value to be
        // log(frequency / target frequency) * 12 / log(2).
        value = log(newFrequency / (stepSizeInFrequency * correctIndex))
                * 12 / M_LN2;
        qDebug() << "compare" << value << newFrequency << m_frequency - stepSizeInFrequency * correctIndex << (m_frequency - stepSizeInFrequency * correctIndex) / stepSizeInFrequency;
    }

    // Emit voiceDifference signal.
    QVariant valueVar(value); //Has to be QVariant for QML
    emit voiceDifference(valueVar);

    // If the correctIndex is index, emit the correctFrequency signal.
    if (correctIndex == index) {
        emit(correctFrequency());
    }
}

/**
  * Empty implementation for readData, since no data is provided
  * by the VoiceAnalyzer class.
  */
qint64 VoiceAnalyzer::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxlen);

    return 0;
}
