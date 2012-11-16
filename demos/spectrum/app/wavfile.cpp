/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
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
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
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
**
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
};



WavFile::WavFile(const QAudioFormat &format, qint64 dataLength)
    : m_format(format)
    , m_dataLength(dataLength)
    , m_dataPosition(0)
{
}

bool WavFile::readHeader(QIODevice &device)
{
    if (!device.isSequential()) {
        if (!device.seek(0))
            return false;
        // XXX: else, assume that current position is the start of the header
    }

    CombinedHeader header;
    if (device.read(reinterpret_cast<char *>(&header), sizeof(CombinedHeader)) != sizeof(CombinedHeader))
        return false;

    if ((memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0
        || memcmp(&header.riff.descriptor.id, "RIFX", 4) == 0)
        && memcmp(&header.riff.type, "WAVE", 4) == 0
        && memcmp(&header.wave.descriptor.id, "fmt ", 4) == 0
        && (header.wave.audioFormat == 1 || header.wave.audioFormat == 0)) {

        // Read off remaining header information
        DATAHeader dataHeader;

        if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
            // Extended data available
            quint16 extraFormatBytes;
            if (device.peek((char*)&extraFormatBytes, sizeof(quint16)) != sizeof(quint16))
                return false;
            const qint64 throwAwayBytes = sizeof(quint16) + qFromLittleEndian<quint16>(extraFormatBytes);
            if (device.read(throwAwayBytes).size() != throwAwayBytes)
                return false;
        }

        if (device.read((char*)&dataHeader, sizeof(DATAHeader)) != sizeof(DATAHeader))
            return false;

        // Establish format
        if (memcmp(&header.riff.descriptor.id, "RIFF", 4) == 0)
            m_format.setByteOrder(QAudioFormat::LittleEndian);
        else
            m_format.setByteOrder(QAudioFormat::BigEndian);

        int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);
        m_format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));
        m_format.setCodec("audio/pcm");
        m_format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
        m_format.setSampleSize(qFromLittleEndian<quint16>(header.wave.bitsPerSample));
        m_format.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);

        m_dataLength = qFromLittleEndian<quint32>(dataHeader.descriptor.size);
        m_dataPosition = 0;
    }

    return true;
}

bool WavFile::writeHeader(QIODevice &device)
{
    CombinedHeader header;
    DATAHeader dataHeader;

    memset(&header, 0, sizeof(CombinedHeader));

    // RIFF header
    if (m_format.byteOrder() == QAudioFormat::LittleEndian)
        strncpy(&header.riff.descriptor.id[0], "RIFF", 4);
    else
        strncpy(&header.riff.descriptor.id[0], "RIFX", 4);
    qToLittleEndian<quint32>(quint32(m_dataLength + sizeof(CombinedHeader) + sizeof(DATAHeader) - sizeof(chunk)),
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
    strncpy(dataHeader.descriptor.id, "data", 4);
    qToLittleEndian<quint32>(quint32(m_dataLength),
                             reinterpret_cast<unsigned char*>(&dataHeader.descriptor.size));

    return device.write(reinterpret_cast<const char *>(&header), sizeof(CombinedHeader)) == sizeof(CombinedHeader)
           && device.write(reinterpret_cast<const char*>(&dataHeader), sizeof(DATAHeader)) == sizeof(DATAHeader);
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
    return sizeof(CombinedHeader);
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

qint64 WavFile::readData(QIODevice &device, QByteArray &buffer,
                         QAudioFormat outputFormat)
{
    // Sanity checks
    if (!outputFormat.isValid())
        outputFormat = m_format;

    if (!isPCMS16LE(outputFormat) || !isPCMS16LE(m_format))
        return 0;

    if (m_dataPosition == m_dataLength)
        return 0;

    // Process
    qint64 result = 0;

    const int frameSize = 2 * m_format.channels(); // 16 bit samples
    QVector<char> inputSample(frameSize);

    qint16 *output = reinterpret_cast<qint16*>(buffer.data());

    while (result < buffer.size()) {
        if (m_dataPosition == m_dataLength)
            break;

        // XXX only working with particular alignments
        if (device.read(inputSample.data(), inputSample.count())) {
            int inputIdx = 0;
            for (int outputIdx = 0; outputIdx < outputFormat.channels(); ++outputIdx) {
                const qint16* input = reinterpret_cast<const qint16*>(inputSample.data() + 2 * inputIdx);
                *output++ = qFromLittleEndian<qint16>(*input);
                result += 2;
                if (inputIdx < m_format.channels())
                    ++inputIdx;
            }
            m_dataPosition += frameSize;
        } else {
            break;
        }
    }

    return result;
}

