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

#include "qaudioinput_symbian_p.h"

QT_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PushInterval = 50; // ms


//-----------------------------------------------------------------------------
// Private class
//-----------------------------------------------------------------------------

SymbianAudioInputPrivate::SymbianAudioInputPrivate(
                              QAudioInputPrivate *audioDevice)
    :   m_audioDevice(audioDevice)
{

}

SymbianAudioInputPrivate::~SymbianAudioInputPrivate()
{

}

qint64 SymbianAudioInputPrivate::readData(char *data, qint64 len)
{
    qint64 totalRead = 0;

    if (m_audioDevice->state() == QAudio::ActiveState ||
        m_audioDevice->state() == QAudio::IdleState) {

        while (totalRead < len) {
            const qint64 read = m_audioDevice->read(data + totalRead,
                                                    len - totalRead);
            if (read > 0)
                totalRead += read;
            else
                break;
        }
    }

    return totalRead;
}

qint64 SymbianAudioInputPrivate::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return 0;
}

void SymbianAudioInputPrivate::dataReady()
{
    emit readyRead();
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

QAudioInputPrivate::QAudioInputPrivate(const QByteArray &device,
                                     const QAudioFormat &format)
    :   m_device(device)
    ,   m_format(format)
    ,   m_clientBufferSize(SymbianAudio::DefaultBufferSize)
    ,   m_notifyInterval(SymbianAudio::DefaultNotifyInterval)
    ,   m_notifyTimer(new QTimer(this))
    ,   m_error(QAudio::NoError)
    ,   m_internalState(SymbianAudio::ClosedState)
    ,   m_externalState(QAudio::StoppedState)
    ,   m_pullMode(false)
    ,   m_sink(0)
    ,   m_pullTimer(new QTimer(this))
    ,   m_devSound(0)
    ,   m_devSoundBuffer(0)
    ,   m_devSoundBufferSize(0)
    ,   m_totalBytesReady(0)
    ,   m_devSoundBufferPos(0)
    ,   m_totalSamplesRecorded(0)
{
    qRegisterMetaType<CMMFBuffer *>("CMMFBuffer *");

    connect(m_notifyTimer.data(), SIGNAL(timeout()), this, SIGNAL(notify()));

    m_pullTimer->setInterval(PushInterval);
    connect(m_pullTimer.data(), SIGNAL(timeout()), this, SLOT(pullData()));
}

QAudioInputPrivate::~QAudioInputPrivate()
{
    close();
}

QIODevice* QAudioInputPrivate::start(QIODevice *device)
{
    stop();

    open();
    if (SymbianAudio::ClosedState != m_internalState) {
        if (device) {
            m_pullMode = true;
            m_sink = device;
        } else {
            m_sink = new SymbianAudioInputPrivate(this);
            m_sink->open(QIODevice::ReadOnly | QIODevice::Unbuffered);
        }

        m_elapsed.restart();
    }

    return m_sink;
}

void QAudioInputPrivate::stop()
{
    close();
}

void QAudioInputPrivate::reset()
{
    m_totalSamplesRecorded += getSamplesRecorded();
    m_devSound->stop();
    startRecording();
}

void QAudioInputPrivate::suspend()
{
    if (SymbianAudio::ActiveState == m_internalState
        || SymbianAudio::IdleState == m_internalState) {
        m_notifyTimer->stop();
        m_pullTimer->stop();
        const qint64 samplesRecorded = getSamplesRecorded();
        m_totalSamplesRecorded += samplesRecorded;

        const bool paused = m_devSound->pause();
        if (paused) {
            if (m_devSoundBuffer)
                m_devSoundBufferQ.append(m_devSoundBuffer);
            m_devSoundBuffer = 0;
            setState(SymbianAudio::SuspendedPausedState);
        } else {
            m_devSoundBuffer = 0;
            m_devSoundBufferQ.clear();
            m_devSoundBufferPos = 0;
            setState(SymbianAudio::SuspendedStoppedState);
        }
    }
}

void QAudioInputPrivate::resume()
{
    if (QAudio::SuspendedState == m_externalState) {
        if (SymbianAudio::SuspendedPausedState == m_internalState)
            m_devSound->resume();
        else
            m_devSound->start();
        startDataTransfer();
    }
}

int QAudioInputPrivate::bytesReady() const
{
    Q_ASSERT(m_devSoundBufferPos <= m_totalBytesReady);
    return m_totalBytesReady - m_devSoundBufferPos;
}

int QAudioInputPrivate::periodSize() const
{
    return bufferSize();
}

void QAudioInputPrivate::setBufferSize(int value)
{
    // Note that DevSound does not allow its client to specify the buffer size.
    // This functionality is available via custom interfaces, but since these
    // cannot be guaranteed to work across all DevSound implementations, we
    // do not use them here.
    // In order to comply with the expected bevahiour of QAudioInput, we store
    // the value and return it from bufferSize(), but the underlying DevSound
    // buffer size remains unchanged.
    if (value > 0)
        m_clientBufferSize = value;
}

int QAudioInputPrivate::bufferSize() const
{
    return m_devSoundBufferSize ? m_devSoundBufferSize : m_clientBufferSize;
}

void QAudioInputPrivate::setNotifyInterval(int ms)
{
    if (ms >= 0) {
        const int oldNotifyInterval = m_notifyInterval;
        m_notifyInterval = ms;
        if (m_notifyInterval && (SymbianAudio::ActiveState == m_internalState ||
                                 SymbianAudio::IdleState == m_internalState))
            m_notifyTimer->start(m_notifyInterval);
        else
            m_notifyTimer->stop();
    }
}

int QAudioInputPrivate::notifyInterval() const
{
    return m_notifyInterval;
}

qint64 QAudioInputPrivate::processedUSecs() const
{
    int samplesPlayed = 0;
    if (m_devSound && QAudio::SuspendedState != m_externalState)
        samplesPlayed = getSamplesRecorded();

    // Protect against division by zero
    Q_ASSERT_X(m_format.frequency() > 0, Q_FUNC_INFO, "Invalid frequency");

    const qint64 result = qint64(1000000) *
                          (samplesPlayed + m_totalSamplesRecorded)
                        / m_format.frequency();

    return result;
}

qint64 QAudioInputPrivate::elapsedUSecs() const
{
    const qint64 result = (QAudio::StoppedState == state()) ?
                              0 : m_elapsed.elapsed() * 1000;
    return result;
}

QAudio::Error QAudioInputPrivate::error() const
{
    return m_error;
}

QAudio::State QAudioInputPrivate::state() const
{
    return m_externalState;
}

QAudioFormat QAudioInputPrivate::format() const
{
    return m_format;
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void QAudioInputPrivate::open()
{
    Q_ASSERT_X(SymbianAudio::ClosedState == m_internalState,
        Q_FUNC_INFO, "DevSound already opened");

    Q_ASSERT(!m_devSound);
    m_devSound = new SymbianAudio::DevSoundWrapper(QAudio::AudioInput, this);

    connect(m_devSound, SIGNAL(initializeComplete(int)),
            this, SLOT(devsoundInitializeComplete(int)));
    connect(m_devSound, SIGNAL(bufferToBeProcessed(CMMFBuffer *)),
            this, SLOT(devsoundBufferToBeEmptied(CMMFBuffer *)));
    connect(m_devSound, SIGNAL(processingError(int)),
            this, SLOT(devsoundRecordError(int)));

    setState(SymbianAudio::InitializingState);
    m_devSound->initialize(m_format.codec());
}

void QAudioInputPrivate::startRecording()
{
    const int samplesRecorded = m_devSound->samplesProcessed();
    Q_ASSERT(samplesRecorded == 0);

    bool ok = m_devSound->setFormat(m_format);
    if (ok)
        ok = m_devSound->start();

    if (ok) {
        startDataTransfer();
    } else {
        setError(QAudio::OpenError);
        close();
    }
}

void QAudioInputPrivate::startDataTransfer()
{
    if (m_notifyInterval)
        m_notifyTimer->start(m_notifyInterval);

    if (m_pullMode)
        m_pullTimer->start();

    if (bytesReady()) {
        setState(SymbianAudio::ActiveState);
        if (!m_pullMode)
            pushData();
    } else {
        if (QAudio::SuspendedState == m_externalState)
            setState(SymbianAudio::ActiveState);
        else
            setState(SymbianAudio::IdleState);
    }
}

CMMFDataBuffer* QAudioInputPrivate::currentBuffer() const
{
    CMMFDataBuffer *result = m_devSoundBuffer;
    if (!result && !m_devSoundBufferQ.empty())
        result = m_devSoundBufferQ.front();
    return result;
}

void QAudioInputPrivate::pushData()
{
    Q_ASSERT_X(bytesReady(), Q_FUNC_INFO, "No data available");
    Q_ASSERT_X(!m_pullMode, Q_FUNC_INFO, "pushData called when in pull mode");
    qobject_cast<SymbianAudioInputPrivate *>(m_sink)->dataReady();
}

qint64 QAudioInputPrivate::read(char *data, qint64 len)
{
    // SymbianAudioInputPrivate is ready to read data

    Q_ASSERT_X(!m_pullMode, Q_FUNC_INFO,
        "read called when in pull mode");

    qint64 bytesRead = 0;

    CMMFDataBuffer *buffer = 0;
    while ((buffer = currentBuffer()) && (bytesRead < len)) {
        if (SymbianAudio::IdleState == m_internalState)
            setState(SymbianAudio::ActiveState);

        TDesC8 &inputBuffer = buffer->Data();

        Q_ASSERT(inputBuffer.Length() >= m_devSoundBufferPos);
        const qint64 inputBytes = inputBuffer.Length() - m_devSoundBufferPos;
        const qint64 outputBytes = len - bytesRead;
        const qint64 copyBytes = outputBytes < inputBytes ?
                                     outputBytes : inputBytes;

        memcpy(data, inputBuffer.Ptr() + m_devSoundBufferPos, copyBytes);

        m_devSoundBufferPos += copyBytes;
        data += copyBytes;
        bytesRead += copyBytes;

        if (inputBytes == copyBytes)
            bufferEmptied();
    }

    return bytesRead;
}

void QAudioInputPrivate::pullData()
{
    Q_ASSERT_X(m_pullMode, Q_FUNC_INFO,
        "pullData called when in push mode");

    CMMFDataBuffer *buffer = 0;
    while (buffer = currentBuffer()) {
        if (SymbianAudio::IdleState == m_internalState)
            setState(SymbianAudio::ActiveState);

        TDesC8 &inputBuffer = buffer->Data();

        Q_ASSERT(inputBuffer.Length() >= m_devSoundBufferPos);
        const qint64 inputBytes = inputBuffer.Length() - m_devSoundBufferPos;
        const qint64 bytesPushed = m_sink->write(
            (char*)inputBuffer.Ptr() + m_devSoundBufferPos, inputBytes);

        m_devSoundBufferPos += bytesPushed;

        if (inputBytes == bytesPushed)
            bufferEmptied();

        if (!bytesPushed)
            break;
    }
}

void QAudioInputPrivate::devsoundInitializeComplete(int err)
{
    Q_ASSERT_X(SymbianAudio::InitializingState == m_internalState,
        Q_FUNC_INFO, "Invalid state");

    if (!err && m_devSound->isFormatSupported(m_format))
        startRecording();
    else
        setError(QAudio::OpenError);
}

void QAudioInputPrivate::devsoundBufferToBeEmptied(CMMFBuffer *baseBuffer)
{
    // Following receipt of this signal, DevSound should not provide another
    // buffer until we have returned the current one.
    Q_ASSERT_X(!m_devSoundBuffer, Q_FUNC_INFO, "Buffer already held");

    CMMFDataBuffer *const buffer = static_cast<CMMFDataBuffer*>(baseBuffer);

    if (!m_devSoundBufferSize)
        m_devSoundBufferSize = buffer->Data().MaxLength();

    m_totalBytesReady += buffer->Data().Length();

    if (SymbianAudio::SuspendedPausedState == m_internalState) {
        m_devSoundBufferQ.append(buffer);
    } else {
        // Will be returned to DevSoundWrapper by bufferProcessed().
        m_devSoundBuffer = buffer;
        m_devSoundBufferPos = 0;

        if (bytesReady() && !m_pullMode)
            pushData();
    }
}

void QAudioInputPrivate::devsoundRecordError(int err)
{
    Q_UNUSED(err)
    setError(QAudio::IOError);
}

void QAudioInputPrivate::bufferEmptied()
{
    m_devSoundBufferPos = 0;

    if (m_devSoundBuffer) {
        m_totalBytesReady -= m_devSoundBuffer->Data().Length();
        m_devSoundBuffer = 0;
        m_devSound->bufferProcessed();
    } else {
        Q_ASSERT(!m_devSoundBufferQ.empty());
        m_totalBytesReady -= m_devSoundBufferQ.front()->Data().Length();
        m_devSoundBufferQ.erase(m_devSoundBufferQ.begin());

        // If the queue has been emptied, resume transfer from the hardware
        if (m_devSoundBufferQ.empty())
            if (!m_devSound->start())
                setError(QAudio::IOError);
    }

    Q_ASSERT(m_totalBytesReady >= 0);
}

void QAudioInputPrivate::close()
{
    m_notifyTimer->stop();
    m_pullTimer->stop();

    m_error = QAudio::NoError;

    if (m_devSound)
        m_devSound->stop();
    delete m_devSound;
    m_devSound = 0;

    m_devSoundBuffer = 0;
    m_devSoundBufferSize = 0;
    m_totalBytesReady = 0;

    if (!m_pullMode) // m_sink is owned
        delete m_sink;
    m_pullMode = false;
    m_sink = 0;

    m_devSoundBufferQ.clear();
    m_devSoundBufferPos = 0;
    m_totalSamplesRecorded = 0;

    setState(SymbianAudio::ClosedState);
}

qint64 QAudioInputPrivate::getSamplesRecorded() const
{
    qint64 result = 0;
    if (m_devSound)
        result = qint64(m_devSound->samplesProcessed());
    return result;
}

void QAudioInputPrivate::setError(QAudio::Error error)
{
    m_error = error;

    // Although no state transition actually occurs here, a stateChanged event
    // must be emitted to inform the client that the call to start() was
    // unsuccessful.
    if (QAudio::OpenError == error) {
        emit stateChanged(QAudio::StoppedState);
    } else {
        if (QAudio::UnderrunError == error)
            setState(SymbianAudio::IdleState);
        else
            // Close the DevSound instance.  This causes a transition to
            // StoppedState.  This must be done asynchronously in case the
            // current function was called from a DevSound event handler, in which
            // case deleting the DevSound instance may cause an exception.
            QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
    }
}

void QAudioInputPrivate::setState(SymbianAudio::State newInternalState)
{
    const QAudio::State oldExternalState = m_externalState;
    m_internalState = newInternalState;
    m_externalState = SymbianAudio::Utils::stateNativeToQt(m_internalState);

    if (m_externalState != oldExternalState)
        emit stateChanged(m_externalState);
}

QT_END_NAMESPACE
