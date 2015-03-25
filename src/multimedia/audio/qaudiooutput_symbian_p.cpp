/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qaudiooutput_symbian_p.h"

QT_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int UnderflowTimerInterval = 50; // ms


//-----------------------------------------------------------------------------
// Private class
//-----------------------------------------------------------------------------

SymbianAudioOutputPrivate::SymbianAudioOutputPrivate(
                               QAudioOutputPrivate *audioDevice)
    :   m_audioDevice(audioDevice)
{

}

SymbianAudioOutputPrivate::~SymbianAudioOutputPrivate()
{

}

qint64 SymbianAudioOutputPrivate::readData(char *data, qint64 len)
{
    Q_UNUSED(data)
    Q_UNUSED(len)
    return 0;
}

qint64 SymbianAudioOutputPrivate::writeData(const char *data, qint64 len)
{
    qint64 totalWritten = 0;

    if (m_audioDevice->state() == QAudio::ActiveState ||
        m_audioDevice->state() == QAudio::IdleState) {

        while (totalWritten < len) {
            const qint64 written = m_audioDevice->pushData(data + totalWritten,
                                                           len - totalWritten);
            if (written > 0)
                totalWritten += written;
            else
                break;
        }
    }

    return totalWritten;
}


//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

QAudioOutputPrivate::QAudioOutputPrivate(const QByteArray &device,
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
    ,   m_source(0)
    ,   m_devSound(0)
    ,   m_devSoundBuffer(0)
    ,   m_devSoundBufferSize(0)
    ,   m_bytesWritten(0)
    ,   m_pushDataReady(false)
    ,   m_bytesPadding(0)
    ,   m_underflow(false)
    ,   m_lastBuffer(false)
    ,   m_underflowTimer(new QTimer(this))
    ,   m_samplesPlayed(0)
    ,   m_totalSamplesPlayed(0)
{
    qRegisterMetaType<CMMFBuffer *>("CMMFBuffer *");

    connect(m_notifyTimer.data(), SIGNAL(timeout()), this, SIGNAL(notify()));

    m_underflowTimer->setInterval(UnderflowTimerInterval);
    connect(m_underflowTimer.data(), SIGNAL(timeout()), this,
            SLOT(underflowTimerExpired()));
}

QAudioOutputPrivate::~QAudioOutputPrivate()
{
    close();
}

QIODevice* QAudioOutputPrivate::start(QIODevice *device)
{
    stop();

    if (device) {
        m_pullMode = true;
        m_source = device;
    }

    open();

    if (SymbianAudio::ClosedState != m_internalState) {
        if (device) {
            connect(m_source, SIGNAL(readyRead()), this, SLOT(dataReady()));
        } else {
            m_source = new SymbianAudioOutputPrivate(this);
            m_source->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
        }

        m_elapsed.restart();
    }

    return m_source;
}

void QAudioOutputPrivate::stop()
{
    close();
}

void QAudioOutputPrivate::reset()
{
    m_totalSamplesPlayed += getSamplesPlayed();
    m_devSound->stop();
    m_bytesPadding = 0;
    startPlayback();
}

void QAudioOutputPrivate::suspend()
{
    if (SymbianAudio::ActiveState == m_internalState
        || SymbianAudio::IdleState == m_internalState) {
        m_notifyTimer->stop();
        m_underflowTimer->stop();
        const qint64 samplesWritten = SymbianAudio::Utils::bytesToSamples(
                                          m_format, m_bytesWritten);
        const qint64 samplesPlayed = getSamplesPlayed();
        m_totalSamplesPlayed += samplesPlayed;
        m_bytesWritten = 0;
        const bool paused = m_devSound->pause();
        if (paused) {
            setState(SymbianAudio::SuspendedPausedState);
        } else {
            m_devSoundBuffer = 0;
            // Calculate the amount of data dropped
            const qint64 paddingSamples = samplesWritten - samplesPlayed;
            Q_ASSERT(paddingSamples >= 0);
            m_bytesPadding = SymbianAudio::Utils::samplesToBytes(m_format,
                                                                 paddingSamples);
            setState(SymbianAudio::SuspendedStoppedState);
        }
    }
}

void QAudioOutputPrivate::resume()
{
    if (QAudio::SuspendedState == m_externalState) {
        if (SymbianAudio::SuspendedPausedState == m_internalState)
            m_devSound->resume();
        else
            startPlayback();
    }
}

int QAudioOutputPrivate::bytesFree() const
{
    int result = 0;
    if (m_devSoundBuffer) {
        const TDes8 &outputBuffer = m_devSoundBuffer->Data();
        result = outputBuffer.MaxLength() - outputBuffer.Length();
    }
    return result;
}

int QAudioOutputPrivate::periodSize() const
{
    return bufferSize();
}

void QAudioOutputPrivate::setBufferSize(int value)
{
    // Note that DevSound does not allow its client to specify the buffer size.
    // This functionality is available via custom interfaces, but since these
    // cannot be guaranteed to work across all DevSound implementations, we
    // do not use them here.
    // In order to comply with the expected bevahiour of QAudioOutput, we store
    // the value and return it from bufferSize(), but the underlying DevSound
    // buffer size remains unchanged.
    if (value > 0)
        m_clientBufferSize = value;
}

int QAudioOutputPrivate::bufferSize() const
{
    return m_devSoundBufferSize ? m_devSoundBufferSize : m_clientBufferSize;
}

void QAudioOutputPrivate::setNotifyInterval(int ms)
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

int QAudioOutputPrivate::notifyInterval() const
{
    return m_notifyInterval;
}

qint64 QAudioOutputPrivate::processedUSecs() const
{
    int samplesPlayed = 0;
    if (m_devSound && QAudio::SuspendedState != m_externalState)
        samplesPlayed = getSamplesPlayed();

    // Protect against division by zero
    Q_ASSERT_X(m_format.frequency() > 0, Q_FUNC_INFO, "Invalid frequency");

    const qint64 result = qint64(1000000) *
                          (samplesPlayed + m_totalSamplesPlayed)
                        / m_format.frequency();

    return result;
}

qint64 QAudioOutputPrivate::elapsedUSecs() const
{
    const qint64 result = (QAudio::StoppedState == state()) ?
                              0 : m_elapsed.elapsed() * 1000;
    return result;
}

QAudio::Error QAudioOutputPrivate::error() const
{
    return m_error;
}

QAudio::State QAudioOutputPrivate::state() const
{
    return m_externalState;
}

QAudioFormat QAudioOutputPrivate::format() const
{
    return m_format;
}


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void QAudioOutputPrivate::dataReady()
{
    // Client-provided QIODevice has data ready to read.

    Q_ASSERT_X(m_source->bytesAvailable(), Q_FUNC_INFO,
        "readyRead signal received, but no data available");

    if (!m_bytesPadding)
        pullData();
}

void QAudioOutputPrivate::underflowTimerExpired()
{
    const TInt samplesPlayed = getSamplesPlayed();
    if (m_samplesPlayed && (samplesPlayed == m_samplesPlayed)) {
        setError(QAudio::UnderrunError);
    } else {
        m_samplesPlayed = samplesPlayed;
        m_underflowTimer->start();
    }
}

void QAudioOutputPrivate::devsoundInitializeComplete(int err)
{
    Q_ASSERT_X(SymbianAudio::InitializingState == m_internalState,
        Q_FUNC_INFO, "Invalid state");

    if (!err && m_devSound->isFormatSupported(m_format))
        startPlayback();
    else
        setError(QAudio::OpenError);
}

void QAudioOutputPrivate::devsoundBufferToBeFilled(CMMFBuffer *bufferBase)
{
    // Following receipt of this signal, DevSound should not provide another
    // buffer until we have returned the current one.
    Q_ASSERT_X(!m_devSoundBuffer, Q_FUNC_INFO, "Buffer already held");

    // Will be returned to DevSoundWrapper by bufferProcessed().
    m_devSoundBuffer = static_cast<CMMFDataBuffer*>(bufferBase);

    if (!m_devSoundBufferSize)
        m_devSoundBufferSize = m_devSoundBuffer->Data().MaxLength();

    writePaddingData();

    if (m_pullMode && isDataReady() && !m_bytesPadding)
        pullData();
}

void QAudioOutputPrivate::devsoundPlayError(int err)
{
    switch (err) {
    case KErrUnderflow:
        m_underflow = true;
        if (m_pullMode && !m_lastBuffer)
            setError(QAudio::UnderrunError);
        else
            setState(SymbianAudio::IdleState);
        break;
    case KErrOverflow:
        // Silently consume this error when in playback mode
        break;
    default:
        setError(QAudio::IOError);
        break;
    }
}

void QAudioOutputPrivate::open()
{
    Q_ASSERT_X(SymbianAudio::ClosedState == m_internalState,
        Q_FUNC_INFO, "DevSound already opened");

    Q_ASSERT(!m_devSound);
    m_devSound = new SymbianAudio::DevSoundWrapper(QAudio::AudioOutput, this);

    connect(m_devSound, SIGNAL(initializeComplete(int)),
            this, SLOT(devsoundInitializeComplete(int)));
    connect(m_devSound, SIGNAL(bufferToBeProcessed(CMMFBuffer *)),
            this, SLOT(devsoundBufferToBeFilled(CMMFBuffer *)));
    connect(m_devSound, SIGNAL(processingError(int)),
            this, SLOT(devsoundPlayError(int)));

    setState(SymbianAudio::InitializingState);
    m_devSound->initialize(m_format.codec());
}

void QAudioOutputPrivate::startPlayback()
{
    bool ok = m_devSound->setFormat(m_format);
    if (ok)
        ok = m_devSound->start();

    if (ok) {
        if (isDataReady())
            setState(SymbianAudio::ActiveState);
        else
            setState(SymbianAudio::IdleState);

        if (m_notifyInterval)
            m_notifyTimer->start(m_notifyInterval);
        m_underflow = false;

        Q_ASSERT(m_devSound->samplesProcessed() == 0);

        writePaddingData();

        if (m_pullMode && m_source->bytesAvailable() && !m_bytesPadding)
            dataReady();
    } else {
        setError(QAudio::OpenError);
        close();
    }
}

void QAudioOutputPrivate::writePaddingData()
{
    // See comments in suspend()

    while (m_devSoundBuffer && m_bytesPadding) {
        if (SymbianAudio::IdleState == m_internalState)
            setState(SymbianAudio::ActiveState);

        TDes8 &outputBuffer = m_devSoundBuffer->Data();
        const qint64 outputBytes = bytesFree();
        const qint64 paddingBytes = outputBytes < m_bytesPadding ?
                                        outputBytes : m_bytesPadding;
        unsigned char *ptr = const_cast<unsigned char*>(outputBuffer.Ptr());
        Mem::FillZ(ptr, paddingBytes);
        outputBuffer.SetLength(outputBuffer.Length() + paddingBytes);
        Q_ASSERT(m_bytesPadding >= paddingBytes);
        m_bytesPadding -= paddingBytes;

        if (m_pullMode && m_source->atEnd())
            lastBufferFilled();
        if ((paddingBytes == outputBytes) || !m_bytesPadding)
            bufferFilled();
    }
}

qint64 QAudioOutputPrivate::pushData(const char *data, qint64 len)
{
    // Data has been written to SymbianAudioOutputPrivate

    Q_ASSERT_X(!m_pullMode, Q_FUNC_INFO,
        "pushData called when in pull mode");

    const unsigned char *const inputPtr =
        reinterpret_cast<const unsigned char*>(data);
    qint64 bytesWritten = 0;

    if (SymbianAudio::IdleState == m_internalState)
        setState(SymbianAudio::ActiveState);

    while (m_devSoundBuffer && (bytesWritten < len)) {
        // writePaddingData() is called from BufferToBeFilled(), so we should
        // never have any padding data left at this point.
        Q_ASSERT_X(0 == m_bytesPadding, Q_FUNC_INFO,
            "Padding bytes remaining in pushData");

        TDes8 &outputBuffer = m_devSoundBuffer->Data();

        const qint64 outputBytes = bytesFree();
        const qint64 inputBytes = len - bytesWritten;
        const qint64 copyBytes = outputBytes < inputBytes ?
                                     outputBytes : inputBytes;

        outputBuffer.Append(inputPtr + bytesWritten, copyBytes);
        bytesWritten += copyBytes;

        bufferFilled();
    }

    m_pushDataReady = (bytesWritten < len);

    // If DevSound is still initializing (m_internalState == InitializingState),
    // we cannot transition m_internalState to ActiveState, but we must emit
    // an (external) state change from IdleState to ActiveState.  The following
    // call triggers this signal.
    setState(m_internalState);

    return bytesWritten;
}

void QAudioOutputPrivate::pullData()
{
    Q_ASSERT_X(m_pullMode, Q_FUNC_INFO,
        "pullData called when in push mode");

    // writePaddingData() is called by BufferToBeFilled() before pullData(),
    // so we should never have any padding data left at this point.
    Q_ASSERT_X(0 == m_bytesPadding, Q_FUNC_INFO,
        "Padding bytes remaining in pullData");

    qint64 inputBytes = m_source->bytesAvailable();
    while (m_devSoundBuffer && inputBytes) {
        if (SymbianAudio::IdleState == m_internalState)
            setState(SymbianAudio::ActiveState);

        TDes8 &outputBuffer = m_devSoundBuffer->Data();

        const qint64 outputBytes = bytesFree();
        const qint64 copyBytes = outputBytes < inputBytes ?
                                     outputBytes : inputBytes;

        char *outputPtr = (char*)(outputBuffer.Ptr() + outputBuffer.Length());
        const qint64 bytesCopied = m_source->read(outputPtr, copyBytes);
        Q_ASSERT(bytesCopied == copyBytes);
        outputBuffer.SetLength(outputBuffer.Length() + bytesCopied);
        inputBytes -= bytesCopied;

        if (m_source->atEnd())
            lastBufferFilled();
        else if (copyBytes == outputBytes)
            bufferFilled();
    }
}

void QAudioOutputPrivate::bufferFilled()
{
    Q_ASSERT_X(m_devSoundBuffer, Q_FUNC_INFO, "No buffer to return");

    const TDes8 &outputBuffer = m_devSoundBuffer->Data();
    m_bytesWritten += outputBuffer.Length();

    m_devSoundBuffer = 0;

    m_samplesPlayed = getSamplesPlayed();
    m_underflowTimer->start();

    if (QAudio::UnderrunError == m_error)
        m_error = QAudio::NoError;

    m_devSound->bufferProcessed();
}

void QAudioOutputPrivate::lastBufferFilled()
{
    Q_ASSERT_X(m_devSoundBuffer, Q_FUNC_INFO, "No buffer to fill");
    Q_ASSERT_X(!m_lastBuffer, Q_FUNC_INFO, "Last buffer already sent");
    m_lastBuffer = true;
    m_devSoundBuffer->SetLastBuffer(ETrue);
    bufferFilled();
}

void QAudioOutputPrivate::close()
{
    m_notifyTimer->stop();
    m_underflowTimer->stop();

    m_error = QAudio::NoError;

    if (m_devSound)
        m_devSound->stop();
    delete m_devSound;
    m_devSound = 0;

    m_devSoundBuffer = 0;
    m_devSoundBufferSize = 0;

    if (!m_pullMode) // m_source is owned
        delete m_source;
    m_pullMode = false;
    m_source = 0;

    m_bytesWritten = 0;
    m_pushDataReady = false;
    m_bytesPadding = 0;
    m_underflow = false;
    m_lastBuffer = false;
    m_samplesPlayed = 0;
    m_totalSamplesPlayed = 0;

    setState(SymbianAudio::ClosedState);
}

qint64 QAudioOutputPrivate::getSamplesPlayed() const
{
    qint64 result = 0;
    if (m_devSound) {
        const qint64 samplesWritten = SymbianAudio::Utils::bytesToSamples(
                                          m_format, m_bytesWritten);

        if (m_underflow) {
            result = samplesWritten;
        } else {
            // This is necessary because some DevSound implementations report
            // that they have played more data than has actually been provided to them
            // by the client.
            const qint64 devSoundSamplesPlayed(m_devSound->samplesProcessed());
            result = qMin(devSoundSamplesPlayed, samplesWritten);
        }
    }
    return result;
}

void QAudioOutputPrivate::setError(QAudio::Error error)
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

void QAudioOutputPrivate::setState(SymbianAudio::State newInternalState)
{
    const QAudio::State oldExternalState = m_externalState;
    m_internalState = newInternalState;
    m_externalState = SymbianAudio::Utils::stateNativeToQt(m_internalState);

    if (m_externalState != oldExternalState)
        emit stateChanged(m_externalState);
}

bool QAudioOutputPrivate::isDataReady() const
{
    return (m_source && m_source->bytesAvailable())
        ||  m_bytesPadding
        ||  m_pushDataReady;
}

QT_END_NAMESPACE
