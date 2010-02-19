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

#ifndef SYMBIANAUDIOUTILS_H
#define SYMBIANAUDIOUTILS_H

#include <QtCore/qnamespace.h>
#include <QtMultimedia/qaudioformat.h>
#include <QtMultimedia/qaudio.h>
#include <sounddevice.h>
#include "symbianaudio.h"

QT_BEGIN_NAMESPACE

namespace SymbianAudio {

/*
 * Helper class for querying DevSound codec / format support
 */
class DevSoundCapabilities {
public:
    DevSoundCapabilities(CMMFDevSound &devsound, QAudio::Mode mode);
    ~DevSoundCapabilities();

    const RArray<TFourCC>& fourCC() const   { return m_fourCC; }
    const TMMFCapabilities& caps() const    { return m_caps; }

private:
    void constructL(CMMFDevSound &devsound, QAudio::Mode mode);

private:
    RArray<TFourCC> m_fourCC;
    TMMFCapabilities m_caps;
};

namespace Utils {

/**
 * Convert native audio capabilities to QAudio lists.
 */
void capabilitiesNativeToQt(const DevSoundCapabilities &caps,
                            QList<int> &frequencies,
                            QList<int> &channels,
                            QList<int> &sampleSizes,
                            QList<QAudioFormat::Endian> &byteOrders,
                            QList<QAudioFormat::SampleType> &sampleTypes);

/**
 * Check whether format is supported.
 */
bool isFormatSupported(const QAudioFormat &format,
                       const DevSoundCapabilities &caps);

/**
 * Convert QAudioFormat to native format types.
 *
 * Note that, despite the name, DevSound uses TMMFCapabilities to specify
 * single formats as well as capabilities.
 *
 * Note that this function does not modify outputFormat.iBufferSize.
 */
bool formatQtToNative(const QAudioFormat &inputFormat,
                      TUint32 &outputFourCC,
                      TMMFCapabilities &outputFormat);

/**
 * Convert internal states to QAudio states.
 */
QAudio::State stateNativeToQt(State nativeState,
                              QAudio::State initializingState);

/**
 * Convert data length to number of samples.
 */
qint64 bytesToSamples(const QAudioFormat &format, qint64 length);

/**
 * Convert number of samples to data length.
 */
qint64 samplesToBytes(const QAudioFormat &format, qint64 samples);

} // namespace Utils
} // namespace SymbianAudio

QT_END_NAMESPACE

#endif
