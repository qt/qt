/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "engine.h"
#include "tonegenerator.h"
#include "utils.h"

#include <math.h>

#include <QCoreApplication>
#include <QMetaObject>
#include <QSet>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QDebug>
#include <QThread>
#include <QFile>

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const qint64 BufferDurationUs       = 10 * 1000000;
const int    NotifyIntervalMs       = 100;

// Size of the level calculation window in microseconds
const int    LevelWindowUs          = 0.1 * 1000000;


//-----------------------------------------------------------------------------
// Helper functions
//-----------------------------------------------------------------------------

QDebug& operator<<(QDebug &debug, const QAudioFormat &format)
{
    debug << format.frequency() << "Hz"
          << format.channels() << "channels";
    return debug;
}

//-----------------------------------------------------------------------------
// Constructor and destructor
//-----------------------------------------------------------------------------

Engine::Engine(QObject *parent)
    :   QObject(parent)
    ,   m_mode(QAudio::AudioInput)
    ,   m_state(QAudio::StoppedState)
    ,   m_generateTone(false)
    ,   m_file(0)
    ,   m_availableAudioInputDevices
            (QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    ,   m_audioInputDevice(QAudioDeviceInfo::defaultInputDevice())
    ,   m_audioInput(0)
    ,   m_audioInputIODevice(0)
    ,   m_recordPosition(0)
    ,   m_availableAudioOutputDevices
            (QAudioDeviceInfo::availableDevices(QAudio::AudioOutput))
    ,   m_audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
    ,   m_audioOutput(0)
    ,   m_playPosition(0)
    ,   m_dataLength(0)
    ,   m_rmsLevel(0.0)
    ,   m_peakLevel(0.0)
    ,   m_spectrumLengthBytes(0)
    ,   m_spectrumAnalyser()
    ,   m_spectrumPosition(0)
    ,   m_count(0)
{
    qRegisterMetaType<FrequencySpectrum>("FrequencySpectrum");
    CHECKED_CONNECT(&m_spectrumAnalyser,
                    SIGNAL(spectrumChanged(FrequencySpectrum)),
                    this,
                    SLOT(spectrumChanged(FrequencySpectrum)));

    initialize();

#ifdef DUMP_DATA
    createOutputDir();
#endif

#ifdef DUMP_SPECTRUM
    m_spectrumAnalyser.setOutputPath(outputPath());
#endif
}

Engine::~Engine()
{

}

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

bool Engine::loadFile(const QString &fileName)
{
    bool result = false;
    m_generateTone = false;

    Q_ASSERT(!fileName.isEmpty());
    Q_ASSERT(!m_file);
    m_file = new QFile(fileName, this);
    m_file->setFileName(fileName);
    Q_ASSERT(m_file->exists());
    if (m_file->open(QFile::ReadOnly)) {
        m_wavFile.readHeader(*m_file);
        if (isPCMS16LE(m_wavFile.format())) {
            result = initialize();
        } else {
            emit errorMessage(tr("Audio format not supported"),
                              formatToString(m_wavFile.format()));
        }
    } else {
        emit errorMessage(tr("Could not open file"), fileName);
    }

    delete m_file;
    m_file = 0;

    return result;
}

bool Engine::generateTone(const Tone &tone)
{
    Q_ASSERT(!m_file);
    m_generateTone = true;
    m_tone = tone;
    ENGINE_DEBUG << "Engine::generateTone"
                 << "startFreq" << m_tone.startFreq
                 << "endFreq" << m_tone.endFreq
                 << "amp" << m_tone.amplitude;
    return initialize();
}

bool Engine::generateSweptTone(qreal amplitude)
{
    Q_ASSERT(!m_file);
    m_generateTone = true;
    m_tone.startFreq = 1;
    m_tone.endFreq = 0;
    m_tone.amplitude = amplitude;
    ENGINE_DEBUG << "Engine::generateSweptTone"
                 << "startFreq" << m_tone.startFreq
                 << "amp" << m_tone.amplitude;
    return initialize();
}

bool Engine::initializeRecord()
{
    ENGINE_DEBUG << "Engine::initializeRecord";
    Q_ASSERT(!m_file);
    m_generateTone = false;
    m_tone = SweptTone();
    return initialize();
}

qint64 Engine::bufferDuration() const
{
    return BufferDurationUs;
}

qint64 Engine::dataDuration() const
{
    qint64 result = 0;
    if (QAudioFormat() != m_format)
        result = audioDuration(m_format, m_dataLength);
    return result;
}

qint64 Engine::audioBufferLength() const
{
    qint64 length = 0;
    if (QAudio::ActiveState == m_state || QAudio::IdleState == m_state) {
        Q_ASSERT(QAudioFormat() != m_format);
        switch (m_mode) {
        case QAudio::AudioInput:
            length = m_audioInput->bufferSize();
            break;
        case QAudio::AudioOutput:
            length = m_audioOutput->bufferSize();
            break;
        }
    }
    return length;
}

void Engine::setWindowFunction(WindowFunction type)
{
    m_spectrumAnalyser.setWindowFunction(type);
}


//-----------------------------------------------------------------------------
// Public slots
//-----------------------------------------------------------------------------

void Engine::startRecording()
{
    if (m_audioInput) {
        if (QAudio::AudioInput == m_mode &&
            QAudio::SuspendedState == m_state) {
            m_audioInput->resume();
        } else {
            m_spectrumAnalyser.cancelCalculation();
            spectrumChanged(0, 0, FrequencySpectrum());

            m_buffer.fill(0);
            setRecordPosition(0, true);
            stopPlayback();
            m_mode = QAudio::AudioInput;
            CHECKED_CONNECT(m_audioInput, SIGNAL(stateChanged(QAudio::State)),
                            this, SLOT(audioStateChanged(QAudio::State)));
            CHECKED_CONNECT(m_audioInput, SIGNAL(notify()),
                            this, SLOT(audioNotify()));
            m_count = 0;
            m_dataLength = 0;
            emit dataDurationChanged(0);
            m_audioInputIODevice = m_audioInput->start();
            CHECKED_CONNECT(m_audioInputIODevice, SIGNAL(readyRead()),
                            this, SLOT(audioDataReady()));
        }
    }
}

void Engine::startPlayback()
{
    if (m_audioOutput) {
        if (QAudio::AudioOutput == m_mode &&
            QAudio::SuspendedState == m_state) {
#ifdef Q_OS_WIN
            // The Windows backend seems to internally go back into ActiveState
            // while still returning SuspendedState, so to ensure that it doesn't
            // ignore the resume() call, we first re-suspend
            m_audioOutput->suspend();
#endif
            m_audioOutput->resume();
        } else {
            m_spectrumAnalyser.cancelCalculation();
            spectrumChanged(0, 0, FrequencySpectrum());

            setPlayPosition(0, true);
            stopRecording();
            m_mode = QAudio::AudioOutput;
            CHECKED_CONNECT(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
                            this, SLOT(audioStateChanged(QAudio::State)));
            CHECKED_CONNECT(m_audioOutput, SIGNAL(notify()),
                            this, SLOT(audioNotify()));
            m_count = 0;
            m_audioOutputIODevice.close();
            m_audioOutputIODevice.setBuffer(&m_buffer);
            m_audioOutputIODevice.open(QIODevice::ReadOnly);
            m_audioOutput->start(&m_audioOutputIODevice);
        }
    }
}

void Engine::suspend()
{
    if (QAudio::ActiveState == m_state ||
        QAudio::IdleState == m_state) {
        switch (m_mode) {
        case QAudio::AudioInput:
            m_audioInput->suspend();
            break;
        case QAudio::AudioOutput:
            m_audioOutput->suspend();
            break;
        }
    }
}

void Engine::setAudioInputDevice(const QAudioDeviceInfo &device)
{
    if (device.deviceName() != m_audioInputDevice.deviceName()) {
        m_audioInputDevice = device;
        initialize();
    }
}

void Engine::setAudioOutputDevice(const QAudioDeviceInfo &device)
{
    if (device.deviceName() != m_audioOutputDevice.deviceName()) {
        m_audioOutputDevice = device;
        initialize();
    }
}


//-----------------------------------------------------------------------------
// Private slots
//-----------------------------------------------------------------------------

void Engine::audioNotify()
{
    switch (m_mode) {
    case QAudio::AudioInput: {
            const qint64 recordPosition =
                    qMin(BufferDurationUs, m_audioInput->processedUSecs());
            setRecordPosition(recordPosition);

            // Calculate level of most recently captured data
            qint64 levelLength = audioLength(m_format, LevelWindowUs);
            levelLength = qMin(m_dataLength, levelLength);
            const qint64 levelPosition = m_dataLength - levelLength;
            calculateLevel(levelPosition, levelLength);

            // Calculate spectrum of most recently captured data
            if (m_dataLength >= m_spectrumLengthBytes) {
                const qint64 spectrumPosition = m_dataLength - m_spectrumLengthBytes;
                calculateSpectrum(spectrumPosition);
            }
        }
        break;
    case QAudio::AudioOutput: {
            const qint64 playPosition =
                    qMin(dataDuration(), m_audioOutput->processedUSecs());
            setPlayPosition(playPosition);

            qint64 analysisPosition = audioLength(m_format, playPosition);

            // Calculate level of data starting at current playback position
            const qint64 levelLength = audioLength(m_format, LevelWindowUs);
            if (analysisPosition + levelLength < m_dataLength)
                calculateLevel(analysisPosition, levelLength);

            if (analysisPosition + m_spectrumLengthBytes < m_dataLength)
                calculateSpectrum(analysisPosition);

            if (dataDuration() == playPosition)
                stopPlayback();
        }
        break;
    }
}

void Engine::audioStateChanged(QAudio::State state)
{
    ENGINE_DEBUG << "Engine::audioStateChanged from" << m_state
                 << "to" << state;

    if (QAudio::StoppedState == state) {
        // Check error
        QAudio::Error error = QAudio::NoError;
        switch (m_mode) {
        case QAudio::AudioInput:
            error = m_audioInput->error();
            break;
        case QAudio::AudioOutput:
            error = m_audioOutput->error();
            break;
        }
        if (QAudio::NoError != error) {
            reset();
            return;
        }
    }
    setState(state);
}

void Engine::audioDataReady()
{
    const qint64 bytesReady = m_audioInput->bytesReady();
    const qint64 bytesSpace = m_buffer.size() - m_dataLength;
    const qint64 bytesToRead = qMin(bytesReady, bytesSpace);

    const qint64 bytesRead = m_audioInputIODevice->read(
                                       m_buffer.data() + m_dataLength,
                                       bytesToRead);

    if (bytesRead) {
        m_dataLength += bytesRead;

        const qint64 duration = audioDuration(m_format, m_dataLength);
        emit dataDurationChanged(duration);
    }

    if (m_buffer.size() == m_dataLength)
        stopRecording();
}

void Engine::spectrumChanged(const FrequencySpectrum &spectrum)
{
    ENGINE_DEBUG << "Engine::spectrumChanged" << "pos" << m_spectrumPosition;
    const qint64 positionUs = audioDuration(m_format, m_spectrumPosition);
    const qint64 lengthUs = audioDuration(m_format, m_spectrumLengthBytes);
    emit spectrumChanged(positionUs, lengthUs, spectrum);
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void Engine::reset()
{
    stopRecording();
    stopPlayback();
    setState(QAudio::AudioInput, QAudio::StoppedState);
    setFormat(QAudioFormat());
    delete m_audioInput;
    m_audioInput = 0;
    m_audioInputIODevice = 0;
    setRecordPosition(0);
    delete m_audioOutput;
    m_audioOutput = 0;
    setPlayPosition(0);
    m_buffer.clear();
    m_dataLength = 0;
    m_spectrumPosition = 0;
    emit dataDurationChanged(0);
    setLevel(0.0, 0.0, 0);
}

bool Engine::initialize()
{
    bool result = false;

    reset();

    if (selectFormat()) {
        const qint64 bufferLength = audioLength(m_format, BufferDurationUs);
        m_buffer.resize(bufferLength);
        m_buffer.fill(0);
        emit bufferDurationChanged(BufferDurationUs);

        if (m_generateTone) {
            if (0 == m_tone.endFreq) {
                const qreal nyquist = nyquistFrequency(m_format);
                m_tone.endFreq = qMin(qreal(SpectrumHighFreq), nyquist);
            }

            // Call function defined in utils.h, at global scope
            ::generateTone(m_tone, m_format, m_buffer);
            m_dataLength = m_buffer.size();
            emit dataDurationChanged(bufferDuration());
            setRecordPosition(bufferDuration());
            result = true;
        } else if (m_file) {
            const qint64 length = m_wavFile.readData(*m_file, m_buffer, m_format);
            if (length) {
                m_dataLength = length;
                emit dataDurationChanged(dataDuration());
                setRecordPosition(dataDuration());
                result = true;
            }
        } else {
            m_audioInput = new QAudioInput(m_audioInputDevice, m_format, this);
            m_audioInput->setNotifyInterval(NotifyIntervalMs);
            result = true;
        }

        m_audioOutput = new QAudioOutput(m_audioOutputDevice, m_format, this);
        m_audioOutput->setNotifyInterval(NotifyIntervalMs);
        m_spectrumLengthBytes = SpectrumLengthSamples *
                                (m_format.sampleSize() / 8) * m_format.channels();
    } else {
        if (m_file)
            emit errorMessage(tr("Audio format not supported"),
                              formatToString(m_format));
        else if (m_generateTone)
            emit errorMessage(tr("No suitable format found"), "");
        else
            emit errorMessage(tr("No common input / output format found"), "");
    }

    ENGINE_DEBUG << "Engine::initialize" << "format" << m_format;

    return result;
}

bool Engine::selectFormat()
{
    bool foundSupportedFormat = false;

    if (m_file) {
        // Header is read from the WAV file; just need to check whether
        // it is supported by the audio output device
        QAudioFormat format = m_wavFile.format();
        if (m_audioOutputDevice.isFormatSupported(m_wavFile.format())) {
            setFormat(m_wavFile.format());
            foundSupportedFormat = true;
        } else {
            // Try flipping mono <-> stereo
            const int channels = (format.channels() == 1) ? 2 : 1;
            format.setChannels(channels);
            if (m_audioOutputDevice.isFormatSupported(format)) {
                setFormat(format);
                foundSupportedFormat = true;
            }
        }
    } else {

        QList<int> frequenciesList;
    #ifdef Q_OS_WIN
        // The Windows audio backend does not correctly report format support
        // (see QTBUG-9100).  Furthermore, although the audio subsystem captures
        // at 11025Hz, the resulting audio is corrupted.
        frequenciesList += 8000;
    #endif

        if (!m_generateTone)
            frequenciesList += m_audioInputDevice.supportedFrequencies();

        frequenciesList += m_audioOutputDevice.supportedFrequencies();
        frequenciesList = frequenciesList.toSet().toList(); // remove duplicates
        qSort(frequenciesList);
        ENGINE_DEBUG << "Engine::initialize frequenciesList" << frequenciesList;

        QList<int> channelsList;
        channelsList += m_audioInputDevice.supportedChannels();
        channelsList += m_audioOutputDevice.supportedChannels();
        channelsList = channelsList.toSet().toList();
        qSort(channelsList);
        ENGINE_DEBUG << "Engine::initialize channelsList" << channelsList;

        QAudioFormat format;
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setCodec("audio/pcm");
        format.setSampleSize(16);
        format.setSampleType(QAudioFormat::SignedInt);
        int frequency, channels;
        foreach (frequency, frequenciesList) {
            if (foundSupportedFormat)
                break;
            format.setFrequency(frequency);
            foreach (channels, channelsList) {
                format.setChannels(channels);
                const bool inputSupport = m_generateTone ||
                                          m_audioInputDevice.isFormatSupported(format);
                const bool outputSupport = m_audioOutputDevice.isFormatSupported(format);
                ENGINE_DEBUG << "Engine::initialize checking " << format
                             << "input" << inputSupport
                             << "output" << outputSupport;
                if (inputSupport && outputSupport) {
                    foundSupportedFormat = true;
                    break;
                }
            }
        }

        if (!foundSupportedFormat)
            format = QAudioFormat();

        setFormat(format);
    }

    return foundSupportedFormat;
}

void Engine::stopRecording()
{
    if (m_audioInput) {
        m_audioInput->stop();
        QCoreApplication::instance()->processEvents();
        m_audioInput->disconnect();
    }
    m_audioInputIODevice = 0;

#ifdef DUMP_AUDIO
    dumpData();
#endif
}

void Engine::stopPlayback()
{
    if (m_audioOutput) {
        m_audioOutput->stop();
        QCoreApplication::instance()->processEvents();
        m_audioOutput->disconnect();
        setPlayPosition(0);
    }
}

void Engine::setState(QAudio::State state)
{
    const bool changed = (m_state != state);
    m_state = state;
    if (changed)
        emit stateChanged(m_mode, m_state);
}

void Engine::setState(QAudio::Mode mode, QAudio::State state)
{
    const bool changed = (m_mode != mode || m_state != state);
    m_mode = mode;
    m_state = state;
    if (changed)
        emit stateChanged(m_mode, m_state);
}

void Engine::setRecordPosition(qint64 position, bool forceEmit)
{
    const bool changed = (m_recordPosition != position);
    m_recordPosition = position;
    if (changed || forceEmit)
        emit recordPositionChanged(m_recordPosition);
}

void Engine::setPlayPosition(qint64 position, bool forceEmit)
{
    const bool changed = (m_playPosition != position);
    m_playPosition = position;
    if (changed || forceEmit)
        emit playPositionChanged(m_playPosition);
}

void Engine::calculateLevel(qint64 position, qint64 length)
{
#ifdef DISABLE_LEVEL
    Q_UNUSED(position)
    Q_UNUSED(length)
#else
    Q_ASSERT(position + length <= m_dataLength);

    qreal peakLevel = 0.0;

    qreal sum = 0.0;
    const char *ptr = m_buffer.constData() + position;
    const char *const end = ptr + length;
    while (ptr < end) {
        const qint16 value = *reinterpret_cast<const qint16*>(ptr);
        const qreal fracValue = pcmToReal(value);
        peakLevel = qMax(peakLevel, fracValue);
        sum += fracValue * fracValue;
        ptr += 2;
    }
    const int numSamples = length / 2;
    qreal rmsLevel = sqrt(sum / numSamples);

    rmsLevel = qMax(qreal(0.0), rmsLevel);
    rmsLevel = qMin(qreal(1.0), rmsLevel);
    setLevel(rmsLevel, peakLevel, numSamples);

    ENGINE_DEBUG << "Engine::calculateLevel" << "pos" << position << "len" << length
                 << "rms" << rmsLevel << "peak" << peakLevel;
#endif
}

void Engine::calculateSpectrum(qint64 position)
{
#ifdef DISABLE_SPECTRUM
    Q_UNUSED(position)
#else
    Q_ASSERT(position + m_spectrumLengthBytes <= m_dataLength);
    Q_ASSERT(0 == m_spectrumLengthBytes % 2); // constraint of FFT algorithm

    // QThread::currentThread is marked 'for internal use only', but
    // we're only using it for debug output here, so it's probably OK :)
    ENGINE_DEBUG << "Engine::calculateSpectrum" << QThread::currentThread()
                 << "count" << m_count << "pos" << position << "len" << m_spectrumLengthBytes
                 << "spectrumAnalyser.isReady" << m_spectrumAnalyser.isReady();

    if(m_spectrumAnalyser.isReady()) {
        m_spectrumBuffer = QByteArray::fromRawData(m_buffer.constData() + position,
                                                   m_spectrumLengthBytes);
        m_spectrumPosition = position;
        m_spectrumAnalyser.calculate(m_spectrumBuffer, m_format);
    }
#endif
}

void Engine::setFormat(const QAudioFormat &format)
{
    const bool changed = (format != m_format);
    m_format = format;
    if (changed)
        emit formatChanged(m_format);
}

void Engine::setLevel(qreal rmsLevel, qreal peakLevel, int numSamples)
{
    m_rmsLevel = rmsLevel;
    m_peakLevel = peakLevel;
    emit levelChanged(m_rmsLevel, m_peakLevel, numSamples);
}

#ifdef DUMP_DATA
void Engine::createOutputDir()
{
    m_outputDir.setPath("output");

    // Ensure output directory exists and is empty
    if (m_outputDir.exists()) {
        const QStringList files = m_outputDir.entryList(QDir::Files);
        QString file;
        foreach (file, files)
            m_outputDir.remove(file);
    } else {
        QDir::current().mkdir("output");
    }
}
#endif // DUMP_DATA

#ifdef DUMP_AUDIO
void Engine::dumpData()
{
    const QString txtFileName = m_outputDir.filePath("data.txt");
    QFile txtFile(txtFileName);
    txtFile.open(QFile::WriteOnly | QFile::Text);
    QTextStream stream(&txtFile);
    const qint16 *ptr = reinterpret_cast<const qint16*>(m_buffer.constData());
    const int numSamples = m_dataLength / (2 * m_format.channels());
    for (int i=0; i<numSamples; ++i) {
        stream << i << "\t" << *ptr << "\n";
        ptr += m_format.channels();
    }

    const QString pcmFileName = m_outputDir.filePath("data.pcm");
    QFile pcmFile(pcmFileName);
    pcmFile.open(QFile::WriteOnly);
    pcmFile.write(m_buffer.constData(), m_dataLength);
}
#endif // DUMP_AUDIO
