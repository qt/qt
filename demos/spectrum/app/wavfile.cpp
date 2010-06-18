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

#include <QtCore/qendian.h>
#include <QVector>
#include <QDebug>
#include "utils.h"
#include "wavfile.h"

struct chunk
{
    char        id[4];
    quint32     size;
};

struct RIFFHeader
{
    chunk       descriptor;     // "RIFF"
    char        type[4];        // "WAVE"
};

struct WAVEHeader
{
    chunk       descriptor;
    quint16     audioFormat;
    quint16     numChannels;
    quint32     sampleRate;
    quint32     byteRate;
    quint16     blockAlign;
    quint16     bitsPerSample;
};

struct DATAHeader
{
    chunk       descriptor;
};

struct CombinedHeader
{
    RIFFHeader  riff;
    WAVEHeader  wave;
    DATAHeader  data;
};

static const int HeaderLength = sizeof(CombinedHeader);


WavFile::WavFile(const QAudioFormat &format, qint64 dataLength)
    :   m_format(format)
    ,   m_dataLength(dataLength)
{

}

bool WavFile::readHeader(QIODevice &device)
{
    bool result = true;

    if (!device.isSequential())
        result = device.seek(0);
    // else, assume that current position is the start of the header

    if (result) {
        CombinedHeader header;
        result = (device.read(reinterpret_cast<char *>(&header), HeaderLength) == HeaderLength);
        if (result) {
            if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
                || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
                && memcmp(&header.riff.type, "WAVE", 4) == 0
                && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
                && header.wave.audioFormat == 1 // PCM
            ) {
                if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
                    m_format.setByteOrder(QAudioFormat::LittleEndian);
                else
                    m_format.setByteOrder(QAudioFormat::BigEndian);

                m_format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));
                m_format.setCodec("audio/pcm");
                m_format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
                m_format.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));

                switch(header.wave.bitsPerSample) {
                case 8:
                    m_format.setSampleType(QAudioFormat::UnSignedInt);
                    break;
                case 16:
                    m_format.setSampleType(QAudioFormat::SignedInt);
                    break;
                default:
                    result = false;
                }

                m_dataLength = device.size() - HeaderLength;
            } else {
                result = false;
            }
        }
    }

    return result;
}

bool WavFile::writeHeader(QIODevice &device)
{
    CombinedHeader header;

    memset(&header, 0, HeaderLength);

    // RIFF header
    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
        strncpy(&header.riff.descriptor.id[0], "RIFF", 4);
    else
        strncpy(&header.riff.descriptor.id[0], "RIFX", 4);
    qToLittleEndian<quint32>(quint32(m_dataLength + HeaderLength - 8),
                             reinterpret_cast<unsigned char*>(&header.riff.descriptor.size));
    strncpy(&header.riff.type[0], "WAVE", 4);

    // WAVE header
    strncpy(&header.wave.descriptor.id[0], "fmt ", 4);
    qToLittleEndian<quint32>(quint32(16),
                             reinterpret_cast<unsigned char*>(&header.wave.descriptor.size));
    qToLittleEndian<quint16>(quint16(1),
                             reinterpret_cast<unsigned char*>(&header.wave.audioFormat));
    qToLittleEndian<quint16>(quint16(m_format.channels()),
                             reinterpret_cast<unsigned char*>(&header.wave.numChannels));
    qToLittleEndian<quint32>(quint32(m_format.frequency()),
                             reinterpret_cast<unsigned char*>(&header.wave.sampleRate));
    qToLittleEndian<quint32>(quint32(m_format.frequency() * m_format.channels() * m_format.sampleSize() / 8),
                             reinterpret_cast<unsigned char*>(&header.wave.byteRate));
    qToLittleEndian<quint16>(quint16(m_format.channels() * m_format.sampleSize() / 8),
                             reinterpret_cast<unsigned char*>(&header.wave.blockAlign));
    qToLittleEndian<quint16>(quint16(m_format.sampleSize()),
                             reinterpret_cast<unsigned char*>(&header.wave.bitsPerSample));

    // DATA header
    strncpy(&header.data.descriptor.id[0], "data", 4);
    qToLittleEndian<quint32>(quint32(m_dataLength),
                             reinterpret_cast<unsigned char*>(&header.data.descriptor.size));

    return (device.write(reinterpret_cast<const char *>(&header), HeaderLength) == HeaderLength);
}

const QAudioFormat& WavFile::format() const
{
    return m_format;
}

qint64 WavFile::dataLength() const
{
    return m_dataLength;
}

qint64 WavFile::headerLength()
{
    return HeaderLength;
}

bool WavFile::writeDataLength(QIODevice &device, qint64 dataLength)
{
    bool result = false;
    if (!device.isSequential()) {
        device.seek(40);
        unsigned char dataLengthLE[4];
        qToLittleEndian<quint32>(quint32(dataLength), dataLengthLE);
        result = (device.write(reinterpret_cast<const char *>(dataLengthLE), 4) == 4);
    }
    return result;
}

#include <QFile>
#include <QTextStream>

qint64 WavFile::readData(QIODevice &device, QByteArray &buffer,
                         QAudioFormat outputFormat)
{
    if (QAudioFormat() == outputFormat)
        outputFormat = m_format;

    qint64 result = 0;

    QFile file("wav.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream;
    stream.setDevice(&file);

    if (isPCMS16LE(outputFormat) && isPCMS16LE(m_format)) {
        QVector<char> inputSample(2 * m_format.channels());

        qint16 *output = reinterpret_cast<qint16*>(buffer.data());

        while (result < buffer.size()) {
            if (device.read(inputSample.data(), inputSample.count())) {
                int inputIdx = 0;
                for (int outputIdx = 0; outputIdx < outputFormat.channels(); ++outputIdx) {
                    const qint16* input = reinterpret_cast<const qint16*>(inputSample.data() + 2 * inputIdx);
                    *output++ = qFromLittleEndian<qint16>(*input);
                    result += 2;
                    if (inputIdx < m_format.channels())
                        ++inputIdx;
                }
            } else {
                break;
            }
        }
    }
    return result;
}

