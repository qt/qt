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

#include "symbianaudiooutput.h"
#include "symbianaudioutils.h"

QT_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int UnderflowTimerInterval = 50; // ms


//-----------------------------------------------------------------------------
// Private class
//-----------------------------------------------------------------------------

SymbianAudioOutputPrivate::SymbianAudioOutputPrivate(
                               SymbianAudioOutput *audioDevice)
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

SymbianAudioOutput::SymbianAudioOutput(const QByteArray &device,
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
    connect(m_notifyTimer.data(), SIGNAL(timeout()), this, SIGNAL(notify()));

    SymbianAudio::Utils::formatQtToNative(m_format, m_nativeFourCC,
                                          m_nativeFormat);

    m_underflowTimer->setInterval(UnderflowTimerInterval);
    connect(m_underflowTimer.data(), SIGNAL(timeout()), this,
            SLOT(underflowTimerExpired()));
}

SymbianAudioOutput::~SymbianAudioOutput()
{
    close();
}

QIODevice* SymbianAudioOutput::start(QIODevice *device)
{
    stop();

    // We have to set these before the call to open() because of the
    // logic in initializingState()
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

void SymbianAudioOutput::stop()
{
    close();
}

void SymbianAudioOutput::reset()
{
    m_totalSamplesPlayed += getSamplesPlayed();
    m_devSound->Stop();
    m_bytesPadding = 0;
    startPlayback();
}

void SymbianAudioOutput::suspend()
{
    if (SymbianAudio::ActiveState == m_internalState
        || SymbianAudio::IdleState == m_internalState) {
        m_notifyTimer->stop();
        m_underflowTimer->stop();

        const qint64 samplesWritten = SymbianAudio::Utils::bytesToSamples(
                                          m_format, m_bytesWritten);
        m_bytesWritten = 0;

        const qint64 samplesPlayed = getSamplesPlayed();

        // CMMFDevSound::Pause() is not guaranteed to work correctly in all
        // implementations, for play-mode DevSound sessions.  We therefore
        // have to implement suspend() by calling CMMFDevSound::Stop().
        // Because this causes buffered data to be dropped, we replace the
        // lost data with silence following a call to resume(), in order to
        // ensure that processedUSecs() returns the correct value.
        m_devSound->Stop();
        m_totalSamplesPlayed += samplesPlayed;

        // Calculate the amount of data dropped
        const qint64 paddingSamples = samplesWritten - samplesPlayed;
        m_bytesPadding = SymbianAudio::Utils::samplesToBytes(m_format,
                                                             paddingSamples);

        setState(SymbianAudio::SuspendedState);
    }
}

void SymbianAudioOutput::resume()
{
    if (SymbianAudio::SuspendedState == m_internalState)
        startPlayback();
}

int SymbianAudioOutput::bytesFree() const
{
    int result = 0;
    if (m_devSoundBuffer) {
        const TDes8 &outputBuffer = m_devSoundBuffer->Data();
        result = outputBuffer.MaxLength() - outputBuffer.Length();
    }
    return result;
}

int SymbianAudioOutput::periodSize() const
{
    return bufferSize();
}

void SymbianAudioOutput::setBufferSize(int value)
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

int SymbianAudioOutput::bufferSize() const
{
    return m_devSoundBufferSize ? m_devSoundBufferSize : m_clientBufferSize;
}

void SymbianAudioOutput::setNotifyInterval(int ms)
{
    if (ms > 0) {
        const int oldNotifyInterval = m_notifyInterval;
        m_notifyInterval = ms;
        if (m_notifyTimer->isActive() && ms != oldNotifyInterval)
            m_notifyTimer->start(m_notifyInterval);
    }
}

int SymbianAudioOutput::notifyInterval() const
{
    return m_notifyInterval;
}

qint64 SymbianAudioOutput::processedUSecs() const
{
    int samplesPlayed = 0;
    if (m_devSound && SymbianAudio::SuspendedState != m_internalState)
        samplesPlayed = getSamplesPlayed();

    // Protect against division by zero
    Q_ASSERT_X(m_format.frequency() > 0, Q_FUNC_INFO, "Invalid frequency");

    const qint64 result = qint64(1000000) *
                          (samplesPlayed + m_totalSamplesPlayed)
                        / m_format.frequency();

    return result;
}

qint64 SymbianAudioOutput::elapsedUSecs() const
{
    const qint64 result = (QAudio::StoppedState == state()) ?
                              0 : m_elapsed.elapsed() * 1000;
    return result;
}

QAudio::Error SymbianAudioOutput::error() const
{
    return m_error;
}

QAudio::State SymbianAudioOutput::state() const
{
    return m_externalState;
}

QAudioFormat SymbianAudioOutput::format() const
{
    return m_format;
}

//-----------------------------------------------------------------------------
// MDevSoundObserver implementation
//-----------------------------------------------------------------------------

void SymbianAudioOutput::InitializeComplete(TInt aError)
{
    Q_ASSERT_X(SymbianAudio::InitializingState == m_internalState,
        Q_FUNC_INFO, "Invalid state");

    if (KErrNone == aError)
        startPlayback();
}

void SymbianAudioOutput::ToneFinished(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's tone playback functions, so should
    // never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioOutput::BufferToBeFilled(CMMFBuffer *aBuffer)
{
    // Following receipt of this callback, DevSound should not provide another
    // buffer until we have returned the current one.
    Q_ASSERT_X(!m_devSoundBuffer, Q_FUNC_INFO, "Buffer already held");

    // Will be returned to DevSound by bufferFilled().
    m_devSoundBuffer = static_cast<CMMFDataBuffer*>(aBuffer);

    if (!m_devSoundBufferSize)
        m_devSoundBufferSize = m_devSoundBuffer->Data().MaxLength();

    writePaddingData();

    if (m_pullMode && isDataReady() && !m_bytesPadding)
        pullData();
}

void SymbianAudioOutput::PlayError(TInt aError)
{
    switch (aError) {
    case KErrUnderflow:
        m_underflow = true;
        if (m_pullMode && !m_lastBuffer)
            setError(QAudio::UnderrunError);
        else
            setState(SymbianAudio::IdleState);
        break;
    default:
        setError(QAudio::IOError);
        break;
    }
}

void SymbianAudioOutput::BufferToBeEmptied(CMMFBuffer *aBuffer)
{
    Q_UNUSED(aBuffer)
    // This class doesn't use DevSound in record mode, so should never receive
    // this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioOutput::RecordError(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound in record mode, so should never receive
    // this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioOutput::ConvertError(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's format conversion functions, so
    // should never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioOutput::DeviceMessage(TUid aMessageType, const TDesC8 &aMsg)
{
    Q_UNUSED(aMessageType)
    Q_UNUSED(aMsg)
    // Ignore this callback.
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void SymbianAudioOutput::dataReady()
{
    // Client-provided QIODevice has data ready to read.

    Q_ASSERT_X(m_source->bytesAvailable(), Q_FUNC_INFO,
        "readyRead signal received, but no data available");

    if (!m_bytesPadding)
        pullData();
}

void SymbianAudioOutput::underflowTimerExpired()
{
    const TInt samplesPlayed = getSamplesPlayed();
    if (m_samplesPlayed && (samplesPlayed == m_samplesPlayed)) {
        setError(QAudio::UnderrunError);
    } else {
        m_samplesPlayed = samplesPlayed;
        m_underflowTimer->start();
    }
}

void SymbianAudioOutput::open()
{
    Q_ASSERT_X(SymbianAudio::ClosedState == m_internalState,
        Q_FUNC_INFO, "DevSound already opened");

    QT_TRAP_THROWING( m_devSound.reset(CMMFDevSound::NewL()) )

    QScopedPointer<SymbianAudio::DevSoundCapabilities> caps(
        new SymbianAudio::DevSoundCapabilities(*m_devSound,
                                               QAudio::AudioOutput));

    int err = SymbianAudio::Utils::isFormatSupported(m_format, *caps) ?
                  KErrNone : KErrNotSupported;

    if (KErrNone == err) {
        setState(SymbianAudio::InitializingState);
        TRAP(err, m_devSound->InitializeL(*this, m_nativeFourCC,
                                          EMMFStatePlaying));
    }

    if (KErrNone != err) {
        setError(QAudio::OpenError);
        m_devSound.reset();
    }
}

void SymbianAudioOutput::startPlayback()
{
    TRAPD(err, startDevSoundL());
    if (KErrNone == err) {
        if (isDataReady())
            setState(SymbianAudio::ActiveState);
        else
            setState(SymbianAudio::IdleState);

        m_notifyTimer->start(m_notifyInterval);
        m_underflow = false;

        Q_ASSERT(m_devSound->SamplesPlayed() == 0);

        writePaddingData();

        if (m_pullMode && m_source->bytesAvailable() && !m_bytesPadding)
            dataReady();
    } else {
        setError(QAudio::OpenError);
        close();
    }
}

void SymbianAudioOutput::startDevSoundL()
{
    TMMFCapabilities nativeFormat = m_devSound->Config();
    m_nativeFormat.iBufferSize = nativeFormat.iBufferSize;
    m_devSound->SetConfigL(m_nativeFormat);
    m_devSound->PlayInitL();
}

void SymbianAudioOutput::writePaddingData()
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
        m_bytesPadding -= paddingBytes;

        if (m_pullMode && m_source->atEnd())
            lastBufferFilled();
        if (paddingBytes == outputBytes)
            bufferFilled();
    }
}

qint64 SymbianAudioOutput::pushData(const char *data, qint64 len)
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

void SymbianAudioOutput::pullData()
{
    Q_ASSERT_X(m_pullMode, Q_FUNC_INFO,
        "pullData called when in push mode");

    if (m_bytesPadding)
        m_bytesPadding = 1;

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

void SymbianAudioOutput::bufferFilled()
{
    Q_ASSERT_X(m_devSoundBuffer, Q_FUNC_INFO, "No buffer to return");

    const TDes8 &outputBuffer = m_devSoundBuffer->Data();
    m_bytesWritten += outputBuffer.Length();

    m_devSoundBuffer = 0;

    m_samplesPlayed = getSamplesPlayed();
    m_underflowTimer->start();

    if (QAudio::UnderrunError == m_error)
        m_error = QAudio::NoError;

    m_devSound->PlayData();
}

void SymbianAudioOutput::lastBufferFilled()
{
    Q_ASSERT_X(m_devSoundBuffer, Q_FUNC_INFO, "No buffer to fill");
    Q_ASSERT_X(!m_lastBuffer, Q_FUNC_INFO, "Last buffer already sent");
    m_lastBuffer = true;
    m_devSoundBuffer->SetLastBuffer(ETrue);
    bufferFilled();
}

void SymbianAudioOutput::close()
{
    m_notifyTimer->stop();
    m_underflowTimer->stop();

    m_error = QAudio::NoError;

    if (m_devSound)
        m_devSound->Stop();
    m_devSound.reset();
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

qint64 SymbianAudioOutput::getSamplesPlayed() const
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
            const qint64 devSoundSamplesPlayed(m_devSound->SamplesPlayed());
            result = qMin(devSoundSamplesPlayed, samplesWritten);
        }
    }
    return result;
}

void SymbianAudioOutput::setError(QAudio::Error error)
{
    m_error = error;

    // Although no state transition actually occurs here, a stateChanged event
    // must be emitted to inform the client that the call to start() was
    // unsuccessful.
    if (QAudio::OpenError == error)
        emit stateChanged(QAudio::StoppedState);

    if (QAudio::UnderrunError == error)
        setState(SymbianAudio::IdleState);
    else
        // Close the DevSound instance.  This causes a transition to
        // StoppedState.  This must be done asynchronously in case the
        // current function was called from a DevSound event handler, in which
        // case deleting the DevSound instance may cause an exception.
        QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}

void SymbianAudioOutput::setState(SymbianAudio::State newInternalState)
{
    const QAudio::State oldExternalState = m_externalState;
    m_internalState = newInternalState;
    m_externalState = SymbianAudio::Utils::stateNativeToQt(
                            m_internalState, initializingState());

    if (m_externalState != oldExternalState)
        emit stateChanged(m_externalState);
}

bool SymbianAudioOutput::isDataReady() const
{
    return (m_source && m_source->bytesAvailable())
        ||  m_bytesPadding
        ||  m_pushDataReady;
}

QAudio::State SymbianAudioOutput::initializingState() const
{
    return isDataReady() ? QAudio::ActiveState : QAudio::IdleState;
}

QT_END_NAMESPACE
