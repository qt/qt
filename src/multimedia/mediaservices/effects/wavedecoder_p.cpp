/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#include "wavedecoder_p.h"

#include <QtCore/qtimer.h>
#include <QtCore/qendian.h>

QT_BEGIN_NAMESPACE

WaveDecoder::WaveDecoder(QIODevice *s, QObject *parent):
    QIODevice(parent),
    haveFormat(false),
    dataSize(0),
    remaining(0),
    source(s)
{
    open(QIODevice::ReadOnly | QIODevice::Unbuffered);

    if (source->bytesAvailable() >= qint64(sizeof(CombinedHeader) + sizeof(DATAHeader) + sizeof(quint16)))
        QTimer::singleShot(0, this, SLOT(handleData()));
    else
        connect(source, SIGNAL(readyRead()), SLOT(handleData()));
}

WaveDecoder::~WaveDecoder()
{
}

QAudioFormat WaveDecoder::audioFormat() const
{
    return format;
}

int WaveDecoder::duration() const
{
    return size() * 1000 / (format.sampleSize() / 8) / format.channels() / format.frequency();
}

qint64 WaveDecoder::size() const
{
    return haveFormat ? dataSize : 0;
}

bool WaveDecoder::isSequential() const
{
    return source->isSequential();
}

qint64 WaveDecoder::bytesAvailable() const
{
    return haveFormat ? source->bytesAvailable() : 0;
}

qint64 WaveDecoder::readData(char *data, qint64 maxlen)
{
    return haveFormat ? source->read(data, maxlen) : 0;
}

qint64 WaveDecoder::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return -1;
}

void WaveDecoder::handleData()
{
    if (source->bytesAvailable() < qint64(sizeof(CombinedHeader) + sizeof(DATAHeader) + sizeof(quint16)))
        return;

    source->disconnect(SIGNAL(readyRead()), this, SLOT(handleData()));
    source->read((char*)&header, sizeof(CombinedHeader));

    if (qstrncmp(header.riff.descriptor.id, "RIFF", 4) != 0 ||
        qstrncmp(header.riff.type, "WAVE", 4) != 0 ||
        qstrncmp(header.wave.descriptor.id, "fmt ", 4) != 0 ||
        (header.wave.audioFormat != 0 && header.wave.audioFormat != 1)) {

        emit invalidFormat();
    }
    else {
        DATAHeader dataHeader;

        if (qFromLittleEndian<quint32>(header.wave.descriptor.size) > sizeof(WAVEHeader)) {
            // Extended data available
            quint16 extraFormatBytes;
            source->peek((char*)&extraFormatBytes, sizeof(quint16));
            extraFormatBytes = qFromLittleEndian<quint16>(extraFormatBytes);
            source->read(sizeof(quint16) + extraFormatBytes);   // dump it all
        }

        source->read((char*)&dataHeader, sizeof(DATAHeader));

        int bps = qFromLittleEndian<quint16>(header.wave.bitsPerSample);

        format.setCodec(QLatin1String("audio/pcm"));
        format.setSampleType(bps == 8 ? QAudioFormat::UnSignedInt : QAudioFormat::SignedInt);
        format.setByteOrder(QAudioFormat::LittleEndian);
        format.setFrequency(qFromLittleEndian<quint32>(header.wave.sampleRate));
        format.setSampleSize(bps);
        format.setChannels(qFromLittleEndian<quint16>(header.wave.numChannels));

        dataSize = qFromLittleEndian<quint32>(dataHeader.descriptor.size);

        haveFormat = true;
        connect(source, SIGNAL(readyRead()), SIGNAL(readyRead()));
        emit formatKnown();
    }
}

QT_END_NAMESPACE
