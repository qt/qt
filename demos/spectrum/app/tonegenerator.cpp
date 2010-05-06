/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "spectrum.h"
#include "utils.h"
#include <QByteArray>
#include <QtMultimedia/QAudioFormat>
#include <QtCore/qmath.h>
#include <QtCore/qendian.h>

void generateTone(const SweptTone &tone, const QAudioFormat &format, QByteArray &buffer)
{
    Q_ASSERT(isPCMS16LE(format));

    const int channelBytes = format.sampleSize() / 8;
    const int sampleBytes = format.channels() * channelBytes;
    int length = buffer.size();
    const int numSamples = buffer.size() / sampleBytes;

    Q_ASSERT(length % sampleBytes == 0);
    Q_UNUSED(sampleBytes) // suppress warning in release builds

    unsigned char *ptr = reinterpret_cast<unsigned char *>(buffer.data());

    qreal phase = 0.0;

    const qreal d = 2 * M_PI / format.frequency();

    // We can't generate a zero-frequency sine wave
    const qreal startFreq = tone.startFreq ? tone.startFreq : 1.0;

    // Amount by which phase increases on each sample
    qreal phaseStep = d * startFreq;

    // Amount by which phaseStep increases on each sample
    // If this is non-zero, the output is a frequency-swept tone
    const qreal phaseStepStep = d * (tone.endFreq - startFreq) / numSamples;

    while (length) {
        const qreal x = tone.amplitude * qSin(phase);
        const qint16 value = realToPcm(x);
        for (int i=0; i<format.channels(); ++i) {
            qToLittleEndian<qint16>(value, ptr);
            ptr += channelBytes;
            length -= channelBytes;
        }

        phase += phaseStep;
        while (phase > 2 * M_PI)
            phase -= 2 * M_PI;
        phaseStep += phaseStepStep;
    }
}

