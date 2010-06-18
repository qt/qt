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


#ifndef WAVFILE_H
#define WAVFILE_H

#include <QtCore/qobject.h>
#include <QtCore/qfile.h>
#include <QtMultimedia/qaudioformat.h>

/**
 * Helper class for reading WAV files
 *
 * See https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 */
class WavFile
{
public:
    WavFile(const QAudioFormat &format = QAudioFormat(),
              qint64 dataLength = 0);

    // Reads WAV header and seeks to start of data
    bool readHeader(QIODevice &device);

    // Writes WAV header
    bool writeHeader(QIODevice &device);

    // Read PCM data
    qint64 readData(QIODevice &device, QByteArray &buffer,
                    QAudioFormat outputFormat = QAudioFormat());

    const QAudioFormat& format() const;
    qint64 dataLength() const;

    static qint64 headerLength();

    static bool writeDataLength(QIODevice &device, qint64 dataLength);

private:
    QAudioFormat m_format;
    qint64 m_dataLength;
};

#endif

