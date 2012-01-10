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

#include "qaudio_symbian_p.h"
#include <mmffourcc.h>

QT_BEGIN_NAMESPACE

namespace SymbianAudio {
namespace Utils {

//-----------------------------------------------------------------------------
// Static data
//-----------------------------------------------------------------------------

// Sample rate / frequency

typedef TMMFSampleRate SampleRateNative;
typedef int SampleRateQt;

const int SampleRateCount = 12;

const SampleRateNative SampleRateListNative[SampleRateCount] = {
        EMMFSampleRate8000Hz
    ,   EMMFSampleRate11025Hz
    ,   EMMFSampleRate12000Hz
    ,   EMMFSampleRate16000Hz
    ,   EMMFSampleRate22050Hz
    ,   EMMFSampleRate24000Hz
    ,   EMMFSampleRate32000Hz
    ,   EMMFSampleRate44100Hz
    ,   EMMFSampleRate48000Hz
    ,   EMMFSampleRate64000Hz
    ,   EMMFSampleRate88200Hz
    ,   EMMFSampleRate96000Hz
};

const SampleRateQt SampleRateListQt[SampleRateCount] = {
        8000
    ,   11025
    ,   12000
    ,   16000
    ,   22050
    ,   24000
    ,   32000
    ,   44100
    ,   48000
    ,   64000
    ,   88200
    ,   96000
};

// Channels

typedef TMMFMonoStereo ChannelsNative;
typedef int ChannelsQt;

const int ChannelsCount = 2;

const ChannelsNative ChannelsListNative[ChannelsCount] = {
        EMMFMono
    ,   EMMFStereo
};

const ChannelsQt ChannelsListQt[ChannelsCount] = {
        1
    ,   2
};

// Encoding

const int EncodingCount = 6;

const TUint32 EncodingFourCC[EncodingCount] = {
        KMMFFourCCCodePCM8              // 0
    ,   KMMFFourCCCodePCMU8             // 1
    ,   KMMFFourCCCodePCM16             // 2
    ,   KMMFFourCCCodePCMU16            // 3
    ,   KMMFFourCCCodePCM16B            // 4
    ,   KMMFFourCCCodePCMU16B           // 5
};

// The characterised DevSound API specification states that the iEncoding
// field in TMMFCapabilities is ignored, and that the FourCC should be used
// to specify the PCM encoding.
// See "SGL.GT0287.102 Multimedia DevSound Baseline Compatibility.doc" in the
// mm_info/mm_docs repository.
const TMMFSoundEncoding EncodingNative[EncodingCount] = {
        EMMFSoundEncoding16BitPCM       // 0
    ,   EMMFSoundEncoding16BitPCM       // 1
    ,   EMMFSoundEncoding16BitPCM       // 2
    ,   EMMFSoundEncoding16BitPCM       // 3
    ,   EMMFSoundEncoding16BitPCM       // 4
    ,   EMMFSoundEncoding16BitPCM       // 5
};


const int EncodingSampleSize[EncodingCount] = {
        8                               // 0
    ,   8                               // 1
    ,   16                              // 2
    ,   16                              // 3
    ,   16                              // 4
    ,   16                              // 5
};

const QAudioFormat::Endian EncodingByteOrder[EncodingCount] = {
        QAudioFormat::LittleEndian      // 0
    ,   QAudioFormat::LittleEndian      // 1
    ,   QAudioFormat::LittleEndian      // 2
    ,   QAudioFormat::LittleEndian      // 3
    ,   QAudioFormat::BigEndian         // 4
    ,   QAudioFormat::BigEndian         // 5
};

const QAudioFormat::SampleType EncodingSampleType[EncodingCount] = {
        QAudioFormat::SignedInt         // 0
    ,   QAudioFormat::UnSignedInt       // 1
    ,   QAudioFormat::SignedInt         // 2
    ,   QAudioFormat::UnSignedInt       // 3
    ,   QAudioFormat::SignedInt         // 4
    ,   QAudioFormat::UnSignedInt       // 5
};


//-----------------------------------------------------------------------------
// Private functions
//-----------------------------------------------------------------------------

// Helper functions for implementing parameter conversions

template<typename Input>
bool findValue(const Input *inputArray, int length, Input input, int &index) {
    bool result = false;
    for (int i=0; !result && i<length; ++i)
        if (inputArray[i] == input) {
            index = i;
            result = true;
        }
    return result;
}

template<typename Input, typename Output>
bool convertValue(const Input *inputArray, const Output *outputArray,
    int length, Input input, Output &output) {
    int index;
    const bool result = findValue<Input>(inputArray, length, input, index);
    if (result)
        output = outputArray[index];
    return result;
}

/**
 * Macro which is used to generate the implementation of the conversion
 * functions.  The implementation is just a wrapper around the templated
 * convertValue function, e.g.
 *
 * CONVERSION_FUNCTION_IMPL(SampleRate, Qt, Native)
 *
 * expands to
 *
 * bool SampleRateQtToNative(int input, TMMFSampleRate &output) {
 *      return convertValue<SampleRateQt, SampleRateNative>
 *          (SampleRateListQt, SampleRateListNative, SampleRateCount,
 *          input, output);
 * }
 */
#define CONVERSION_FUNCTION_IMPL(FieldLc, Field, Input, Output)               \
bool FieldLc##Input##To##Output(Field##Input input, Field##Output &output) {  \
    return convertValue<Field##Input, Field##Output>(Field##List##Input,      \
        Field##List##Output, Field##Count, input, output);                    \
}

//-----------------------------------------------------------------------------
// Local helper functions
//-----------------------------------------------------------------------------

CONVERSION_FUNCTION_IMPL(sampleRate, SampleRate, Qt, Native)
CONVERSION_FUNCTION_IMPL(sampleRate, SampleRate, Native, Qt)
CONVERSION_FUNCTION_IMPL(channels, Channels, Qt, Native)
CONVERSION_FUNCTION_IMPL(channels, Channels, Native, Qt)

bool sampleInfoQtToNative(int inputSampleSize,
                          QAudioFormat::Endian inputByteOrder,
                          QAudioFormat::SampleType inputSampleType,
                          TUint32 &outputFourCC,
                          TMMFSoundEncoding &outputEncoding) {

    bool found = false;

    for (int i=0; i<EncodingCount && !found; ++i) {
        if (    EncodingSampleSize[i] == inputSampleSize
            &&  EncodingByteOrder[i] == inputByteOrder
            &&  EncodingSampleType[i] == inputSampleType) {
            outputFourCC = EncodingFourCC[i];
            outputEncoding = EncodingNative[i]; // EMMFSoundEncoding16BitPCM
            found = true;
        }
    }

    return found;
}

void capabilitiesNativeToQt(const TMMFCapabilities &caps,
                            const TFourCC &fourcc,
                            QList<int> &frequencies,
                            QList<int> &channels,
                            QList<int> &sampleSizes,
                            QList<QAudioFormat::Endian> &byteOrders,
                            QList<QAudioFormat::SampleType> &sampleTypes) {

    frequencies.clear();
    sampleSizes.clear();
    byteOrders.clear();
    sampleTypes.clear();
    channels.clear();

    for (int i=0; i<SampleRateCount; ++i)
        if (caps.iRate & SampleRateListNative[i])
            frequencies += SampleRateListQt[i];

    for (int i=0; i<ChannelsCount; ++i)
        if (caps.iChannels & ChannelsListNative[i])
            channels += ChannelsListQt[i];

    for (int i=0; i<EncodingCount; ++i) {
        if (fourcc == EncodingFourCC[i]) {
            sampleSizes += EncodingSampleSize[i];
            byteOrders += EncodingByteOrder[i];
            sampleTypes += EncodingSampleType[i];
        }
    }
}

bool formatQtToNative(const QAudioFormat &inputFormat,
                      TUint32 &outputFourCC,
                      TMMFCapabilities &outputFormat) {

    bool result = false;

    // Need to use temporary variables because TMMFCapabilities fields are all
    // TInt, rather than MMF enumerated types.
    TMMFSampleRate outputSampleRate;
    TMMFMonoStereo outputChannels;
    TMMFSoundEncoding outputEncoding;

    if (inputFormat.codec() == QLatin1String("audio/pcm")) {
        result =
                sampleRateQtToNative(inputFormat.frequency(), outputSampleRate)
            &&  channelsQtToNative(inputFormat.channels(), outputChannels)
            &&  sampleInfoQtToNative(inputFormat.sampleSize(),
                                     inputFormat.byteOrder(),
                                     inputFormat.sampleType(),
                                     outputFourCC,
                                     outputEncoding);
    }

    if (result) {
        outputFormat.iRate = outputSampleRate;
        outputFormat.iChannels = outputChannels;
        outputFormat.iEncoding = outputEncoding;
    }

    return result;
}

QAudio::State stateNativeToQt(State nativeState)
{
    switch (nativeState) {
    case ClosedState:
        return QAudio::StoppedState;
    case InitializingState:
        return QAudio::StoppedState;
    case ActiveState:
        return QAudio::ActiveState;
    case IdleState:
        return QAudio::IdleState;
    case SuspendedPausedState:
    case SuspendedStoppedState:
        return QAudio::SuspendedState;
    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid state");
        return QAudio::StoppedState; // suppress compiler warning
    }
}

qint64 bytesToSamples(const QAudioFormat &format, qint64 length)
{
    return length / ((format.sampleSize() / 8) * format.channels());
}

qint64 samplesToBytes(const QAudioFormat &format, qint64 samples)
{
    return samples * (format.sampleSize() / 8) * format.channels();
}

} // namespace Utils


//-----------------------------------------------------------------------------
// DevSoundWrapper
//-----------------------------------------------------------------------------

DevSoundWrapper::DevSoundWrapper(QAudio::Mode mode, QObject *parent)
    :   QObject(parent)
    ,   m_mode(mode)
    ,   m_state(StateIdle)
    ,   m_devsound(0)
    ,   m_fourcc(0)
{
    QT_TRAP_THROWING(m_devsound = CMMFDevSound::NewL());

    switch (mode) {
    case QAudio::AudioOutput:
        m_nativeMode = EMMFStatePlaying;
        break;

    case QAudio::AudioInput:
        m_nativeMode = EMMFStateRecording;
        break;

    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid mode");
    }

    getSupportedCodecs();
}

DevSoundWrapper::~DevSoundWrapper()
{
    delete m_devsound;
}

const QList<QString>& DevSoundWrapper::supportedCodecs() const
{
    return m_supportedCodecs;
}

void DevSoundWrapper::initialize(const QString& codec)
{
    Q_ASSERT(StateInitializing != m_state);
    m_state = StateInitializing;
    if (QLatin1String("audio/pcm") == codec) {
        m_fourcc = KMMFFourCCCodePCM16;
        TRAPD(err, m_devsound->InitializeL(*this, m_fourcc, m_nativeMode));
        if (KErrNone != err) {
            m_state = StateIdle;
            emit initializeComplete(err);
        }
    } else {
        emit initializeComplete(KErrNotSupported);
    }
}

const QList<int>& DevSoundWrapper::supportedFrequencies() const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedFrequencies;
}

const QList<int>& DevSoundWrapper::supportedChannels() const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedChannels;
}

const QList<int>& DevSoundWrapper::supportedSampleSizes() const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedSampleSizes;
}

const QList<QAudioFormat::Endian>& DevSoundWrapper::supportedByteOrders() const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedByteOrders;
}

const QList<QAudioFormat::SampleType>& DevSoundWrapper::supportedSampleTypes() const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedSampleTypes;
}

bool DevSoundWrapper::isFormatSupported(const QAudioFormat &format) const
{
    Q_ASSERT(StateInitialized == m_state);
    return m_supportedCodecs.contains(format.codec())
        && m_supportedFrequencies.contains(format.frequency())
        && m_supportedChannels.contains(format.channels())
        && m_supportedSampleSizes.contains(format.sampleSize())
        && m_supportedSampleTypes.contains(format.sampleType())
        && m_supportedByteOrders.contains(format.byteOrder());
}

int DevSoundWrapper::samplesProcessed() const
{
    int result = 0;
    if (StateInitialized == m_state) {
        switch (m_mode) {
        case QAudio::AudioInput:
            result = m_devsound->SamplesRecorded();
            break;
        case QAudio::AudioOutput:
            result = m_devsound->SamplesPlayed();
            break;
        }
    }
    return result;
}

bool DevSoundWrapper::setFormat(const QAudioFormat &format)
{
    Q_ASSERT(StateInitialized == m_state);
    bool result = false;
    TUint32 fourcc;
    TMMFCapabilities nativeFormat;
    if (Utils::formatQtToNative(format, fourcc, nativeFormat)) {
        TMMFCapabilities currentNativeFormat = m_devsound->Config();
        nativeFormat.iBufferSize = currentNativeFormat.iBufferSize;
        TRAPD(err, m_devsound->SetConfigL(nativeFormat));
        result = (KErrNone == err);
    }
    return result;
}

bool DevSoundWrapper::start()
{
    Q_ASSERT(StateInitialized == m_state);
    int err = KErrArgument;
    switch (m_mode) {
    case QAudio::AudioInput:
        TRAP(err, m_devsound->RecordInitL());
        break;
    case QAudio::AudioOutput:
        TRAP(err, m_devsound->PlayInitL());
        break;
    }
    return (KErrNone == err);
}

bool DevSoundWrapper::pause()
{
    Q_ASSERT(StateInitialized == m_state);
    const bool canPause = isResumeSupported();
    if (canPause)
        m_devsound->Pause();
    else
        stop();
    return canPause;
}

void DevSoundWrapper::resume()
{
    Q_ASSERT(StateInitialized == m_state);
    Q_ASSERT(isResumeSupported());
    // TODO: QTBUG-13625
}

void DevSoundWrapper::stop()
{
    m_devsound->Stop();
}

void DevSoundWrapper::bufferProcessed()
{
    Q_ASSERT(StateInitialized == m_state);
    switch (m_mode) {
    case QAudio::AudioInput:
        m_devsound->RecordData();
        break;
    case QAudio::AudioOutput:
        m_devsound->PlayData();
        break;
    }
}

void DevSoundWrapper::getSupportedCodecs()
{
/*
 * TODO: once we support formats other than PCM, this function should
 * convert the array of FourCC codes into MIME types for each codec.
 *
    RArray<TFourCC> fourcc;
    QT_TRAP_THROWING(CleanupClosePushL(&fourcc));

    TMMFPrioritySettings settings;
    switch (mode) {
    case QAudio::AudioOutput:
        settings.iState = EMMFStatePlaying;
        m_devsound->GetSupportedInputDataTypesL(fourcc, settings);
        break;

    case QAudio::AudioInput:
        settings.iState = EMMFStateRecording;
        m_devsound->GetSupportedInputDataTypesL(fourcc, settings);
        break;

    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid mode");
    }

    CleanupStack::PopAndDestroy(); // fourcc
*/

    m_supportedCodecs.append(QLatin1String("audio/pcm"));
}

void DevSoundWrapper::populateCapabilities()
{
    m_supportedFrequencies.clear();
    m_supportedChannels.clear();
    m_supportedSampleSizes.clear();
    m_supportedByteOrders.clear();
    m_supportedSampleTypes.clear();

    const TMMFCapabilities caps = m_devsound->Capabilities();

    for (int i=0; i<Utils::SampleRateCount; ++i)
        if (caps.iRate & Utils::SampleRateListNative[i])
            m_supportedFrequencies += Utils::SampleRateListQt[i];

    for (int i=0; i<Utils::ChannelsCount; ++i)
        if (caps.iChannels & Utils::ChannelsListNative[i])
            m_supportedChannels += Utils::ChannelsListQt[i];

    for (int i=0; i<Utils::EncodingCount; ++i) {
        if (m_fourcc == Utils::EncodingFourCC[i]) {
            m_supportedSampleSizes += Utils::EncodingSampleSize[i];
            m_supportedByteOrders += Utils::EncodingByteOrder[i];
            m_supportedSampleTypes += Utils::EncodingSampleType[i];
        }
    }
}

bool DevSoundWrapper::isResumeSupported() const
{
    // TODO: QTBUG-13625
    return false;
}

void DevSoundWrapper::InitializeComplete(TInt aError)
{
    Q_ASSERT(StateInitializing == m_state);
    if (KErrNone == aError) {
        m_state = StateInitialized;
        populateCapabilities();
    } else {
        m_state = StateIdle;
    }
    emit initializeComplete(aError);
}

void DevSoundWrapper::ToneFinished(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's tone playback functions, so should
    // never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void DevSoundWrapper::BufferToBeFilled(CMMFBuffer *aBuffer)
{
    Q_ASSERT(QAudio::AudioOutput == m_mode);
    emit bufferToBeProcessed(aBuffer);
}

void DevSoundWrapper::PlayError(TInt aError)
{
    Q_ASSERT(QAudio::AudioOutput == m_mode);
    emit processingError(aError);
}

void DevSoundWrapper::BufferToBeEmptied(CMMFBuffer *aBuffer)
{
    Q_ASSERT(QAudio::AudioInput == m_mode);
    emit bufferToBeProcessed(aBuffer);
}

void DevSoundWrapper::RecordError(TInt aError)
{
    Q_ASSERT(QAudio::AudioInput == m_mode);
    emit processingError(aError);
}

void DevSoundWrapper::ConvertError(TInt aError)
{
    Q_UNUSED(aError)
    // This class doesn't use DevSound's format conversion functions, so
    // should never receive this callback.
    Q_ASSERT_X(false, Q_FUNC_INFO, "Unexpected callback");
}

void DevSoundWrapper::DeviceMessage(TUid aMessageType, const TDesC8 &aMsg)
{
    Q_UNUSED(aMessageType)
    Q_UNUSED(aMsg)
    // Ignore this callback.
}


} // namespace SymbianAudio

QT_END_NAMESPACE


