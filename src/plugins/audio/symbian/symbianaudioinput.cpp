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

#include "symbianaudioinput.h"
#include "symbianaudioutils.h"

QT_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const int PushInterval = 50; // ms


//-----------------------------------------------------------------------------
// Private class
//-----------------------------------------------------------------------------

SymbianAudioInputPrivate::SymbianAudioInputPrivate(
                              SymbianAudioInput *audioDevice)
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

SymbianAudioInput::SymbianAudioInput(const QByteArray &device,
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
    ,   m_devSoundBuffer(0)
    ,   m_devSoundBufferSize(0)
    ,   m_totalBytesReady(0)
    ,   m_devSoundBufferPos(0)
    ,   m_totalSamplesRecorded(0)
{
    connect(m_notifyTimer.data(), SIGNAL(timeout()), this, SIGNAL(notify()));

    SymbianAudio::Utils::formatQtToNative(m_format, m_nativeFourCC,
                                          m_nativeFormat);

    m_pullTimer->setInterval(PushInterval);
    connect(m_pullTimer.data(), SIGNAL(timeout()), this, SLOT(pullData()));
}

SymbianAudioInput::~SymbianAudioInput()
{
    close();
}

QIODevice* SymbianAudioInput::start(QIODevice *device)
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

void SymbianAudioInput::stop()
{
    close();
}

void SymbianAudioInput::reset()
{
    m_totalSamplesRecorded += getSamplesRecorded();
    m_devSound->Stop();
    startRecording();
}

void SymbianAudioInput::suspend()
{
    if (SymbianAudio::ActiveState == m_internalState
        || SymbianAudio::IdleState == m_internalState) {
        m_notifyTimer->stop();
        m_pullTimer->stop();
        m_devSound->Pause();
        const qint64 samplesRecorded = getSamplesRecorded();
        m_totalSamplesRecorded += samplesRecorded;

        if (m_devSoundBuffer) {
            m_devSoundBufferQ.append(m_devSoundBuffer);
            m_devSoundBuffer = 0;
        }

        setState(SymbianAudio::SuspendedState);
    }
}

void SymbianAudioInput::resume()
{
    if (SymbianAudio::SuspendedState == m_internalState)
        startDataTransfer();
}

int SymbianAudioInput::bytesReady() const
{
    Q_ASSERT(m_devSoundBufferPos <= m_totalBytesReady);
    return m_totalBytesReady - m_devSoundBufferPos;
}

int SymbianAudioInput::periodSize() const
{
    return bufferSize();
}

void SymbianAudioInput::setBufferSize(int value)
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

int SymbianAudioInput::bufferSize() const
{
    return m_devSoundBufferSize ? m_devSoundBufferSize : m_clientBufferSize;
}

void SymbianAudioInput::setNotifyInterval(int ms)
{
    if (ms > 0) {
        const int oldNotifyInterval = m_notifyInterval;
        m_notifyInterval = ms;
        if (m_notifyTimer->isActive() && ms != oldNotifyInterval)
            m_notifyTimer->start(m_notifyInterval);
    }
}

int SymbianAudioInput::notifyInterval() const
{
    return m_notifyInterval;
}

qint64 SymbianAudioInput::processedUSecs() const
{
    int samplesPlayed = 0;
    if (m_devSound && SymbianAudio::SuspendedState != m_internalState)
        samplesPlayed = getSamplesRecorded();

    // Protect against division by zero
    Q_ASSERT_X(m_format.frequency() > 0, Q_FUNC_INFO, "Invalid frequency");

    const qint64 result = qint64(1000000) *
                          (samplesPlayed + m_totalSamplesRecorded)
                        / m_format.frequency();

    return result;
}

qint64 SymbianAudioInput::elapsedUSecs() const
{
    const qint64 result = (QAudio::StoppedState == state()) ?
                              0 : m_elapsed.elapsed() * 1000;
    return result;
}

QAudio::Error SymbianAudioInput::error() const
{
    return m_error;
}

QAudio::State SymbianAudioInput::state() const
{
    return m_externalState;
}

QAudioFormat SymbianAudioInput::format() const
{
    return m_format;
}

//-----------------------------------------------------------------------------
// MDevSoundObserver implementation
//-----------------------------------------------------------------------------

void SymbianAudioInput::InitializeComplete(TInt aError)
{
    Q_ASSERT_X(SymbianAudio::InitializingState == m_internalState,
        Q_FUNC_INFO, "Invalid state");

    if (KErrNone == aError)
        startRecording();
}

void SymbianAudioInput::ToneFinished(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's tone playback functions, so should
    // never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioInput::BufferToBeFilled(CMMFBuffer *aBuffer)
{
    Q_UNUSED(aBuffer)
    // This class doesn't use DevSound in play mode, so should never receive
    // this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioInput::PlayError(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound in play mode, so should never receive
    // this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioInput::BufferToBeEmptied(CMMFBuffer *aBuffer)
{
    // Following receipt of this callback, DevSound should not provide another
    // buffer until we have returned the current one.
    Q_ASSERT_X(!m_devSoundBuffer, Q_FUNC_INFO, "Buffer already held");

    CMMFDataBuffer *const buffer = static_cast<CMMFDataBuffer*>(aBuffer);

    if (!m_devSoundBufferSize)
        m_devSoundBufferSize = buffer->Data().MaxLength();

    m_totalBytesReady += buffer->Data().Length();

    if (SymbianAudio::SuspendedState == m_internalState) {
        m_devSoundBufferQ.append(buffer);
    } else {
        // Will be returned to DevSound by bufferEmptied().
        m_devSoundBuffer = buffer;
        m_devSoundBufferPos = 0;

        if (bytesReady() && !m_pullMode)
            pushData();
    }
}

void SymbianAudioInput::RecordError(TInt aError)
{
    Q_UNUSED(aError)
    setError(QAudio::IOError);
}

void SymbianAudioInput::ConvertError(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's format conversion functions, so
    // should never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void SymbianAudioInput::DeviceMessage(TUid aMessageType, const TDesC8 &aMsg)
{
    Q_UNUSED(aMessageType)
    Q_UNUSED(aMsg)
    // Ignore this callback.
}

//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

void SymbianAudioInput::open()
{
    Q_ASSERT_X(SymbianAudio::ClosedState == m_internalState,
        Q_FUNC_INFO, "DevSound already opened");

    QT_TRAP_THROWING( m_devSound.reset(CMMFDevSound::NewL()) )

    QScopedPointer<SymbianAudio::DevSoundCapabilities> caps(
        new SymbianAudio::DevSoundCapabilities(*m_devSound, QAudio::AudioInput));

    int err = SymbianAudio::Utils::isFormatSupported(m_format, *caps) ?
                  KErrNone : KErrNotSupported;

    if (KErrNone == err) {
        setState(SymbianAudio::InitializingState);
        TRAP(err, m_devSound->InitializeL(*this, m_nativeFourCC,
                                          EMMFStateRecording));
    }

    if (KErrNone != err) {
        setError(QAudio::OpenError);
        m_devSound.reset();
    }
}

void SymbianAudioInput::startRecording()
{
    const int samplesRecorded = m_devSound->SamplesRecorded();
    Q_ASSERT(samplesRecorded == 0);

    TRAPD(err, startDevSoundL());
    if (KErrNone == err) {
        startDataTransfer();
    } else {
        setError(QAudio::OpenError);
        close();
    }
}

void SymbianAudioInput::startDevSoundL()
{
    TMMFCapabilities nativeFormat = m_devSound->Config();
    m_nativeFormat.iBufferSize = nativeFormat.iBufferSize;
    m_devSound->SetConfigL(m_nativeFormat);
    m_devSound->RecordInitL();
}

void SymbianAudioInput::startDataTransfer()
{
    m_notifyTimer->start(m_notifyInterval);

    if (m_pullMode)
        m_pullTimer->start();

    if (bytesReady()) {
        setState(SymbianAudio::ActiveState);
        if (!m_pullMode)
            pushData();
    } else {
        if (SymbianAudio::SuspendedState == m_internalState)
            setState(SymbianAudio::ActiveState);
        else
            setState(SymbianAudio::IdleState);
    }
}

CMMFDataBuffer* SymbianAudioInput::currentBuffer() const
{
    CMMFDataBuffer *result = m_devSoundBuffer;
    if (!result && !m_devSoundBufferQ.empty())
        result = m_devSoundBufferQ.front();
    return result;
}

void SymbianAudioInput::pushData()
{
    Q_ASSERT_X(bytesReady(), Q_FUNC_INFO, "No data available");
    Q_ASSERT_X(!m_pullMode, Q_FUNC_INFO, "pushData called when in pull mode");
    qobject_cast<SymbianAudioInputPrivate *>(m_sink)->dataReady();
}

qint64 SymbianAudioInput::read(char *data, qint64 len)
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

        const qint64 inputBytes = bytesReady();
        const qint64 outputBytes = len - bytesRead;
        const qint64 copyBytes = outputBytes < inputBytes ?
                                     outputBytes : inputBytes;

        memcpy(data, inputBuffer.Ptr() + m_devSoundBufferPos, copyBytes);

        m_devSoundBufferPos += copyBytes;
        data += copyBytes;
        bytesRead += copyBytes;

        if (!bytesReady())
            bufferEmptied();
    }

    return bytesRead;
}

void SymbianAudioInput::pullData()
{
    Q_ASSERT_X(m_pullMode, Q_FUNC_INFO,
        "pullData called when in push mode");

    CMMFDataBuffer *buffer = 0;
    while (buffer = currentBuffer()) {
        if (SymbianAudio::IdleState == m_internalState)
            setState(SymbianAudio::ActiveState);

        TDesC8 &inputBuffer = buffer->Data();

        const qint64 inputBytes = bytesReady();
        const qint64 bytesPushed = m_sink->write(
            (char*)inputBuffer.Ptr() + m_devSoundBufferPos, inputBytes);

        m_devSoundBufferPos += bytesPushed;

        if (!bytesReady())
            bufferEmptied();

        if (!bytesPushed)
            break;
    }
}

void SymbianAudioInput::bufferEmptied()
{
    m_devSoundBufferPos = 0;

    if (m_devSoundBuffer) {
        m_totalBytesReady -= m_devSoundBuffer->Data().Length();
        m_devSoundBuffer = 0;
        m_devSound->RecordData();
    } else {
        Q_ASSERT(!m_devSoundBufferQ.empty());
        m_totalBytesReady -= m_devSoundBufferQ.front()->Data().Length();
        m_devSoundBufferQ.erase(m_devSoundBufferQ.begin());

        // If the queue has been emptied, resume transfer from the hardware
        if (m_devSoundBufferQ.empty())
            m_devSound->RecordInitL();
    }

    Q_ASSERT(m_totalBytesReady >= 0);
}

void SymbianAudioInput::close()
{
    m_notifyTimer->stop();
    m_pullTimer->stop();

    m_error = QAudio::NoError;

    if (m_devSound)
        m_devSound->Stop();
    m_devSound.reset();
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

qint64 SymbianAudioInput::getSamplesRecorded() const
{
    qint64 result = 0;
    if (m_devSound)
        result = qint64(m_devSound->SamplesRecorded());
    return result;
}

void SymbianAudioInput::setError(QAudio::Error error)
{
    m_error = error;

    // Although no state transition actually occurs here, a stateChanged event
    // must be emitted to inform the client that the call to start() was
    // unsuccessful.
    if (QAudio::OpenError == error)
        emit stateChanged(QAudio::StoppedState);

    // Close the DevSound instance.  This causes a transition to StoppedState.
    // This must be done asynchronously in case the current function was called
    // from a DevSound event handler, in which case deleting the DevSound
    // instance may cause an exception.
    QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
}

void SymbianAudioInput::setState(SymbianAudio::State newInternalState)
{
    const QAudio::State oldExternalState = m_externalState;
    m_internalState = newInternalState;
    m_externalState = SymbianAudio::Utils::stateNativeToQt(
                            m_internalState, initializingState());

    if (m_externalState != oldExternalState)
        emit stateChanged(m_externalState);
}

QAudio::State SymbianAudioInput::initializingState() const
{
    return QAudio::IdleState;
}

QT_END_NAMESPACE
