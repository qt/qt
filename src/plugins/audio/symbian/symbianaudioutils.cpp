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

#include "symbianaudioutils.h"
#include <mmffourcc.h>

QT_BEGIN_NAMESPACE

namespace SymbianAudio {

DevSoundCapabilities::DevSoundCapabilities(CMMFDevSound &devsound,
                                           QAudio::Mode mode)
{
    QT_TRAP_THROWING(constructL(devsound, mode));
}

DevSoundCapabilities::~DevSoundCapabilities()
{
    m_fourCC.Close();
}

void DevSoundCapabilities::constructL(CMMFDevSound &devsound,
                                      QAudio::Mode mode)
{
    m_caps = devsound.Capabilities();

    TMMFPrioritySettings settings;

    switch (mode) {
    case QAudio::AudioOutput:
        settings.iState = EMMFStatePlaying;
        devsound.GetSupportedInputDataTypesL(m_fourCC, settings);
        break;

    case QAudio::AudioInput:
        settings.iState = EMMFStateRecording;
        devsound.GetSupportedInputDataTypesL(m_fourCC, settings);
        break;

    default:
        Q_ASSERT_X(false, Q_FUNC_INFO, "Invalid mode");
    }
}

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

//-----------------------------------------------------------------------------
// Public functions
//-----------------------------------------------------------------------------

void capabilitiesNativeToQt(const DevSoundCapabilities &caps,
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
        if (caps.caps().iRate & SampleRateListNative[i])
            frequencies += SampleRateListQt[i];

    for (int i=0; i<ChannelsCount; ++i)
        if (caps.caps().iChannels & ChannelsListNative[i])
            channels += ChannelsListQt[i];

    for (int i=0; i<EncodingCount; ++i) {
        if (caps.fourCC().Find(EncodingFourCC[i]) != KErrNotFound) {
            sampleSizes += EncodingSampleSize[i];
            byteOrders += EncodingByteOrder[i];
            sampleTypes += EncodingSampleType[i];
        }
    }

}

bool isFormatSupported(const QAudioFormat &formatQt,
                       const DevSoundCapabilities &caps) {
    TMMFCapabilities formatNative;
    TUint32 fourCC;

    bool result = false;
    if (formatQt.codec() == "audio/pcm" &&
        formatQtToNative(formatQt, fourCC, formatNative)) {
        result =
                (formatNative.iRate & caps.caps().iRate)
            &&  (formatNative.iChannels & caps.caps().iChannels)
            &&  (caps.fourCC().Find(fourCC) != KErrNotFound);
    }
    return result;
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

    if (inputFormat.codec() == "audio/pcm") {
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

QAudio::State stateNativeToQt(State nativeState,
                              QAudio::State initializingState)
{
    switch (nativeState) {
    case ClosedState:
        return QAudio::StoppedState;
    case InitializingState:
        return initializingState;
    case ActiveState:
        return QAudio::ActiveState;
    case IdleState:
        return QAudio::IdleState;
    case SuspendedState:
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
} // namespace SymbianAudio

QT_END_NAMESPACE


